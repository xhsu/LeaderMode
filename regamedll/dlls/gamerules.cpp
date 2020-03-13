#include "precompiled.h"

CCStrikeGameMgrHelper g_GameMgrHelper;
CHalfLifeMultiplay* g_pGameRules = nullptr;
RewardAccount CHalfLifeMultiplay::m_rgRewardAccountRules[RR_END];
RewardAccount CHalfLifeMultiplay::m_rgRewardAccountRules_default[] = {
	REWARD_CTS_WIN,                         // RR_CTS_WIN
	REWARD_TERRORISTS_WIN,                  // RR_TERRORISTS_WIN
	REWARD_LOSER_BONUS_DEFAULT,             // RR_LOSER_BONUS_DEFAULT
	REWARD_LOSER_BONUS_MIN,                 // RR_LOSER_BONUS_MIN
	REWARD_LOSER_BONUS_MAX,                 // RR_LOSER_BONUS_MAX
	REWARD_LOSER_BONUS_ADD,                 // RR_LOSER_BONUS_ADD
	REWARD_LEADER_KILLED,					// RR_LEADER_KILLED,
	REWARD_KILLED_LEADER,					// RR_KILLED_ENEMY_LEADER,
};

const char* g_rgszTacticalSchemeNames[SCHEMES_COUNT] =
{
	"Disputing",
	"Superior Firepower Doctrine",
	"Mass Assault Doctrine",
	"Grand Battleplan Doctrine",
	"Mobile Warfare Doctrine",
};

const char* g_rgszTacticalSchemeDesc[SCHEMES_COUNT] =
{
	"/yIf most of you are /gUNDETERMINED/y, or you are having a /tdisputation/y: Your team /twon't receive/y any buff.",
	"/gSuperior Firepower Doctrine/y: Refill /g4%%%%/y of your /tmaximum/y clip /teach second/y.",
	"/gMass Assault Doctrine/y: /gMinimize /tredeployment interval/y of your squad along with /gdoubled/y menpower.",
	"/gGrand Battleplan Doctrine/y: Slowly /grefill accounts/y of all your squad members. In addition, you will receive /trudimentary equipments/y after each redeployment.",
	"/gMobile Warfare Doctrine/y: /gRedeployment loci/y are relocated near the /tSquad Leader/y. In addition, you are allowed to purchase gears /geverywhere/y.",
};

const ChatColor g_rgiTacticalSchemeDescColor[SCHEMES_COUNT] = { GREYCHAT, REDCHAT, BLUECHAT, BLUECHAT, GREYCHAT };

bool IsBotSpeaking()
{
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);

		if (!pPlayer || !pPlayer->IsBot())
			continue;

		CCSBot *pBot = static_cast<CCSBot *>(pPlayer);

		if (pBot->IsUsingVoice())
			return true;
	}

	return false;
}

CHalfLifeMultiplay* EXT_FUNC InstallGameRules()
{
	SERVER_COMMAND("exec game.cfg\n");
	SERVER_EXECUTE();

	return new CHalfLifeMultiplay;
}

// this is the game name that gets seen in the server browser
const char* CHalfLifeMultiplay::GetGameDescription()
{
	if (m_GameDesc)
		return m_GameDesc;

	return "Counter-Strike";
}

bool CHalfLifeMultiplay::IsInCareerRound()
{
	return IsMatchStarted() ? false : true;
}

void CHalfLifeMultiplay::SetCareerMatchLimit(int minWins, int winDifference)
{
	if (!IsCareer())
	{
		return;
	}

	if (!m_iCareerMatchWins)
	{
		m_iCareerMatchWins = minWins;
		m_iRoundWinDifference = winDifference;
	}
}

BOOL CHalfLifeMultiplay::IsCareer()
{
	return IS_CAREER_MATCH();
}

void EXT_FUNC CHalfLifeMultiplay::ServerDeactivate()
{
	if (!IsCareer())
	{
		return;
	}

	CVAR_SET_FLOAT("pausable", 0);
	CVAR_SET_FLOAT("mp_windifference", 1);
	UTIL_LogPrintf("Career End\n");
}

bool CCStrikeGameMgrHelper::CanPlayerHearPlayer(CBasePlayer *pListener, CBasePlayer *pSender)
{
	if (!GetCanHearPlayer(pListener, pSender))
	{
		return false;
	}

	switch ((int)sv_alltalk.value)
	{
	case 1: // allows everyone to talk
		return true;
	case 2:
		return (pListener->m_iTeam == pSender->m_iTeam);
	case 3:
		return (pListener->m_iTeam == pSender->m_iTeam || pListener->m_iTeam == SPECTATOR || pListener->m_iTeam == UNASSIGNED);
	case 4:
		return (pListener->IsAlive() == pSender->IsAlive() || pSender->IsAlive());
	default:
	{
		if (pListener->m_iTeam != pSender->m_iTeam) // Different teams can't hear each other
		{
			return false;
		}

		if (pListener->GetObserverMode() != OBS_NONE) // 2 spectators don't need isAlive() checks.
		{
			return true;
		}

		BOOL bListenerAlive = pListener->IsAlive();
		BOOL bSenderAlive = pSender->IsAlive();

		return (bListenerAlive == bSenderAlive || bSenderAlive); // Dead/alive voice chats are separated, but dead can hear alive.
	}
	}
}

void CCStrikeGameMgrHelper::ResetCanHearPlayer(edict_t* pEdict)
{
	int index = ENTINDEX(pEdict) - 1;

	m_iCanHearMasks[index].Init(TRUE);
	for (int iOtherClient = 0; iOtherClient < VOICE_MAX_PLAYERS; iOtherClient++)
	{
		if (index != iOtherClient) {
			m_iCanHearMasks[iOtherClient][index] = TRUE;
		}
	}
}

void CCStrikeGameMgrHelper::SetCanHearPlayer(CBasePlayer* pListener, CBasePlayer* pSender, bool bCanHear)
{
	if (!pListener->IsPlayer() || !pSender->IsPlayer())
	{
		return;
	}

	int listener = pListener->entindex() - 1;
	int sender = pSender->entindex() - 1;
	m_iCanHearMasks[listener][sender] = bCanHear ? TRUE : FALSE;
}

bool CCStrikeGameMgrHelper::GetCanHearPlayer(CBasePlayer* pListener, CBasePlayer* pSender)
{
	if (!pListener->IsPlayer() || !pSender->IsPlayer())
	{
		return true;
	}

	int listener = pListener->entindex() - 1;
	int sender = pSender->entindex() - 1;
	return m_iCanHearMasks[listener][sender] != FALSE;
}

void Broadcast(const char *sentence)
{
	char text[32];

	if (!sentence)
	{
		return;
	}

	Q_strcpy(text, "%!MRAD_");
	Q_strcat(text, UTIL_VarArgs("%s", sentence));

	MESSAGE_BEGIN(MSG_BROADCAST, gmsgSendAudio);
		WRITE_BYTE(0);
		WRITE_STRING(text);
		WRITE_SHORT(PITCH_NORM);
	MESSAGE_END();
}

char *GetTeam(int team)
{
	switch (team)
	{
	case CT:		return "CT";
	case TERRORIST:	return "TERRORIST";
	case SPECTATOR:	return "SPECTATOR";
	default:		return "";
	}
}

void CHalfLifeMultiplay::EndRoundMessage(const char *sentence, ScenarioEventEndRound event)
{
	char *team = nullptr;
	const char *message = sentence;
	bool bTeamTriggered = true;

	if (sentence[0] == '#')
		message = sentence + 1;

	if (sentence[0])
	{
		UTIL_ClientPrintAll(HUD_PRINTCENTER, sentence);

		switch (event)
		{
		case ROUND_TERRORISTS_WIN:
			team = GetTeam(TERRORIST);
			// tell bots the terrorists won the round
			if (TheBots)
			{
				TheBots->OnEvent(EVENT_TERRORISTS_WIN);
			}
			break;

		case ROUND_CTS_WIN:
			team = GetTeam(CT);
			// tell bots the CTs won the round
			if (TheBots)
			{
				TheBots->OnEvent(EVENT_CTS_WIN);
			}
			break;

		default:
			bTeamTriggered = false;
			// tell bots the round was a draw
			if (TheBots)
			{
				TheBots->OnEvent(EVENT_ROUND_DRAW);
			}
			break;
		}

		if (bTeamTriggered)
		{
			UTIL_LogPrintf("Team \"%s\" triggered \"%s\" (CT \"%i\") (T \"%i\")\n", team, message, m_iNumCTWins, m_iNumTerroristWins);
		}
		else
		{
			UTIL_LogPrintf("World triggered \"%s\" (CT \"%i\") (T \"%i\")\n", message, m_iNumCTWins, m_iNumTerroristWins);
		}
	}

	UTIL_LogPrintf("World triggered \"Round_End\"\n");
}

void CHalfLifeMultiplay::ReadMultiplayCvars()
{
	m_iRoundTime = int(CVAR_GET_FLOAT("mp_roundtime") * 60);
	m_iIntroRoundTime = int(CVAR_GET_FLOAT("mp_freezetime"));
	m_iLimitTeams = int(CVAR_GET_FLOAT("mp_limitteams"));


	// a limit of 500 minutes because
	// if you do more minutes would be a bug in the HUD RoundTime in the form 00:00
	if (m_iRoundTime > 30000)
	{
		CVAR_SET_FLOAT("mp_roundtime", 500);
		m_iRoundTime = 30000;
	}
	else if (m_iRoundTime < 0)
	{
		CVAR_SET_FLOAT("mp_roundtime", 0);
		m_iRoundTime = 0;
	}
	if (m_iIntroRoundTime < 0)
	{
		CVAR_SET_FLOAT("mp_freezetime", 0);
		m_iIntroRoundTime = 0;
	}
	if (m_iLimitTeams < 0)
	{
		CVAR_SET_FLOAT("mp_limitteams", 0);
		m_iLimitTeams = 0;
	}

	// auto-disable ff
	if (freeforall.value)
	{
		CVAR_SET_FLOAT("mp_friendlyfire", 0);
	}
}

CHalfLifeMultiplay::CHalfLifeMultiplay()
{
	m_bFreezePeriod = TRUE;

	m_VoiceGameMgr.Init(&g_GameMgrHelper, gpGlobals->maxClients);
	Q_memcpy(m_rgRewardAccountRules, m_rgRewardAccountRules_default, sizeof(m_rgRewardAccountRules));

	m_flIntermissionEndTime = 0;
	m_flIntermissionStartTime = 0;
	m_flRestartRoundTime = 0;

	m_iAccountCT = 0;
	m_iAccountTerrorist = 0;
	m_iRoundWinStatus = WINSTATUS_NONE;
	m_iNumCTWins = 0;
	m_iNumTerroristWins = 0;
	m_iNumCT = 0;
	m_iNumTerrorist = 0;
	m_iNumSpawnableCT = 0;
	m_iNumSpawnableTerrorist = 0;
	m_bMapHasCameras = FALSE;

	m_iLoserBonus = m_rgRewardAccountRules[RR_LOSER_BONUS_DEFAULT];
	m_iNumConsecutiveCTLoses = 0;
	m_iNumConsecutiveTerroristLoses = 0;
	m_bLevelInitialized = false;
	m_tmNextPeriodicThink = 0;
	m_bGameStarted = false;
	m_bCompleteReset = false;

	// by default everyone can buy
	m_bCTCantBuy = false;
	m_bTCantBuy = false;

	m_iTotalGunCount = 0;
	m_iTotalGrenadeCount = 0;
	m_iTotalArmourCount = 0;
	m_iUnBalancedRounds = 0;
	m_iNumEscapeRounds = 0;
	m_bRoundTerminating = false;

	m_iMaxRounds = int(CVAR_GET_FLOAT("mp_maxrounds"));

	if (m_iMaxRounds < 0)
	{
		m_iMaxRounds = 0;
		CVAR_SET_FLOAT("mp_maxrounds", 0);
	}

	m_iTotalRoundsPlayed = 0;
	m_iMaxRoundsWon = int(CVAR_GET_FLOAT("mp_winlimit"));

	if (m_iMaxRoundsWon < 0)
	{
		m_iMaxRoundsWon = 0;
		CVAR_SET_FLOAT("mp_winlimit", 0);
	}

	Q_memset(m_iMapVotes, 0, sizeof(m_iMapVotes));

	m_iLastPick = 1;
	m_iStoredSpectValue = int(allow_spectators.value);

	if (!IS_DEDICATED_SERVER())
	{
		// NOTE: cvar cl_himodels refers for the client side
		CVAR_SET_FLOAT("cl_himodels", 0);
	}

	ReadMultiplayCvars();

	m_iIntroRoundTime += 2;
	m_fMaxIdlePeriod = (((m_iRoundTime < 60) ? 60 : m_iRoundTime) * 2);

	float flAutoKickIdle = autokick_timeout.value;
	if (flAutoKickIdle > 0.0)
	{
		m_fMaxIdlePeriod = flAutoKickIdle;
	}

	m_bInCareerGame = false;
	m_iRoundTimeSecs = m_iIntroRoundTime;

	if (IS_DEDICATED_SERVER())
	{
		CVAR_SET_FLOAT("pausable", 0);
	}
	else if (IsCareer())
	{
		CVAR_SET_FLOAT("pausable", 1);
		CVAR_SET_FLOAT("sv_aim", 0);
		CVAR_SET_FLOAT("sv_maxspeed", 322);
		CVAR_SET_FLOAT("sv_cheats", 0);
		CVAR_SET_FLOAT("mp_windifference", 2);

		m_bInCareerGame = true;
		UTIL_LogPrintf("Career Start\n");
	}
	else
	{
		// 3/31/99
		// Added lservercfg file cvar, since listen and dedicated servers should not
		// share a single config file. (sjb)

		// listen server
		CVAR_SET_FLOAT("pausable", 0);

		const char *lservercfgfile = CVAR_GET_STRING("lservercfgfile");
		if (lservercfgfile && lservercfgfile[0] != '\0')
		{
			char szCommand[256];

			ALERT(at_console, "Executing listen server config file\n");
			Q_sprintf(szCommand, "exec %s\n", lservercfgfile);
			SERVER_COMMAND(szCommand);
		}
	}

	m_fRoundStartTime = 0;
	m_fRoundStartTimeReal = 0;

#ifndef CSTRIKE
	InstallBotControl();
#endif

	InstallCommands();

	m_bSkipSpawn = m_bInCareerGame;

	m_fCareerRoundMenuTime = 0;
	m_fCareerMatchMenuTime = 0;
	m_iCareerMatchWins = 0;

	m_iRoundWinDifference = int(CVAR_GET_FLOAT("mp_windifference"));

	if (IsCareer())
	{
		CCareerTaskManager::Create();
	}

	if (m_iRoundWinDifference < 1)
	{
		m_iRoundWinDifference = 1;
		CVAR_SET_FLOAT("mp_windifference", 1);
	}

	InstallTutor(CVAR_GET_FLOAT("tutor_enable") != 0.0f);

	m_bSkipShowMenu = false;
	m_bNeededPlayers = false;
	m_flTimeLimit = 0.0f;
	m_flGameStartTime = 0.0f;
	m_bTeamBalanced = false;

	// initialize FX message.
	m_TextParam_Notification.x = -1.0f;
	m_TextParam_Notification.y = 0.3f;
	m_TextParam_Notification.effect = 0;
	m_TextParam_Notification.r1 = m_TextParam_Notification.r2 = 255;
	m_TextParam_Notification.g1 = m_TextParam_Notification.g2 = 100;
	m_TextParam_Notification.b1 = m_TextParam_Notification.b2 = 255;
	m_TextParam_Notification.a1 = m_TextParam_Notification.a2 = 255;
	m_TextParam_Notification.fadeinTime = 0.1f;
	m_TextParam_Notification.fadeoutTime = 0.2f;
	m_TextParam_Notification.holdTime = 6.0f;
	m_TextParam_Notification.fxTime = 6.0f;
	m_TextParam_Notification.channel = 1;

	m_TextParam_Hud.x = -1.0f;
	m_TextParam_Hud.y = 0.85f;
	m_TextParam_Hud.effect = 0;
	m_TextParam_Hud.r1 = m_TextParam_Hud.r2 = 255;
	m_TextParam_Hud.g1 = m_TextParam_Hud.g2 = 255;
	m_TextParam_Hud.b1 = m_TextParam_Hud.b2 = 0;
	m_TextParam_Hud.a1 = m_TextParam_Hud.a2 = 255;
	m_TextParam_Hud.fadeinTime = 0;
	m_TextParam_Hud.fadeoutTime = 0;
	m_TextParam_Hud.holdTime = 3600.0f;
	m_TextParam_Hud.fxTime = 6.0f;
	m_TextParam_Hud.channel = 2;
}

void EXT_FUNC CHalfLifeMultiplay::RemoveGuns()
{
	CBaseEntity *toremove = nullptr;

	while ((toremove = UTIL_FindEntityByClassname(toremove, "weaponbox")))
		((CWeaponBox *)toremove)->Kill();

	toremove = nullptr;

	while ((toremove = UTIL_FindEntityByClassname(toremove, "weapon_shield")))
	{
		toremove->SetThink(&CBaseEntity::SUB_Remove);
		toremove->pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CHalfLifeMultiplay::UpdateTeamScores()
{
	MESSAGE_BEGIN(MSG_ALL, gmsgTeamScore);
		WRITE_STRING("CT");
		WRITE_SHORT(m_iNumCTWins);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ALL, gmsgTeamScore);
		WRITE_STRING("TERRORIST");
		WRITE_SHORT(m_iNumTerroristWins);
	MESSAGE_END();
}

void EXT_FUNC CHalfLifeMultiplay::CleanUpMap()
{
	UTIL_RestartOther("multi_manager");

	// Release or reset everything entities in depending of flags ObjectCaps
	// (FCAP_MUST_RESET / FCAP_MUST_RELEASE)
	UTIL_ResetEntities();

	// Recreate all the map entities from the map data (preserving their indices),
	// then remove everything else except the players.
	UTIL_RestartOther("cycler_sprite");
	UTIL_RestartOther("light");
	UTIL_RestartOther("func_breakable");
	UTIL_RestartOther("func_door");

	UTIL_RestartOther("func_button");
	UTIL_RestartOther("func_rot_button");
	UTIL_RestartOther("env_render");
	UTIL_RestartOther("env_spark");
	UTIL_RestartOther("trigger_push");

	UTIL_RestartOther("func_water");
	UTIL_RestartOther("func_door_rotating");
	UTIL_RestartOther("func_tracktrain");
	UTIL_RestartOther("func_vehicle");
	UTIL_RestartOther("func_train");
	UTIL_RestartOther("armoury_entity");
	UTIL_RestartOther("ambient_generic");
	UTIL_RestartOther("env_sprite");

	UTIL_RestartOther("trigger_once");
	UTIL_RestartOther("func_wall_toggle");
	UTIL_RestartOther("func_healthcharger");
	UTIL_RestartOther("func_recharge");
	UTIL_RestartOther("trigger_hurt");
	UTIL_RestartOther("multisource");
	UTIL_RestartOther("env_beam");
	UTIL_RestartOther("env_laser");
	UTIL_RestartOther("trigger_auto");

	// Remove grenades and C4
	const int grenadesRemoveCount = 20;
	UTIL_RemoveOther("grenade", grenadesRemoveCount);

	UTIL_RemoveOther("gib");
	UTIL_RemoveOther("DelayedUse");

	RemoveGuns();
	PLAYBACK_EVENT((FEV_GLOBAL | FEV_RELIABLE), 0, m_usResetDecals);
}

void CHalfLifeMultiplay::QueueCareerRoundEndMenu(float tmDelay, int iWinStatus)
{
	if (!TheCareerTasks)
		return;

	if (m_fCareerMatchMenuTime != 0.0f)
		return;

	m_fCareerRoundMenuTime = tmDelay + gpGlobals->time;

	bool humansAreCTs = (Q_strcmp(humans_join_team.string, "CT") == 0);

	switch (iWinStatus)
	{
	case WINSTATUS_CTS:
		TheCareerTasks->HandleEvent(humansAreCTs ? EVENT_ROUND_WIN : EVENT_ROUND_LOSS);
		break;
	case WINSTATUS_TERRORISTS:
		TheCareerTasks->HandleEvent(humansAreCTs ? EVENT_ROUND_LOSS : EVENT_ROUND_WIN);
		break;
	default:
		TheCareerTasks->HandleEvent(EVENT_ROUND_DRAW);
		break;
	}

	if (m_fCareerMatchMenuTime == 0.0f && m_iCareerMatchWins)
	{
		bool canTsWin = true;
		bool canCTsWin = true;

		if (m_iNumCTWins < m_iCareerMatchWins || (m_iNumCTWins - m_iNumTerroristWins < m_iRoundWinDifference))
			canCTsWin = false;

		if (m_iNumTerroristWins < m_iCareerMatchWins || (m_iNumTerroristWins - m_iNumCTWins < m_iRoundWinDifference))
			canTsWin = false;

		if (!TheCareerTasks->AreAllTasksComplete())
		{
			if (humansAreCTs)
				return;

			canTsWin = false;
		}

		if (canCTsWin || canTsWin)
		{
			m_fCareerRoundMenuTime = 0;
			m_fCareerMatchMenuTime = gpGlobals->time + 3.0f;
		}
	}
}

// Check if the scenario has been won/lost.
void EXT_FUNC CHalfLifeMultiplay::CheckWinConditions()
{
	// If a winner has already been determined.. then get the heck out of here
	if (m_iRoundWinStatus != WINSTATUS_NONE)
		return;

	int scenarioFlags = UTIL_ReadFlags(round_infinite.string);

	// Initialize the player counts..
	int NumDeadCT, NumDeadTerrorist, NumAliveTerrorist, NumAliveCT;
	InitializePlayerCounts(NumAliveTerrorist, NumAliveCT, NumDeadTerrorist, NumDeadCT);

	if (HasRoundInfinite())
		return;

	// other player's check
	m_bNeededPlayers = false;
	if (!(scenarioFlags & SCENARIO_BLOCK_NEED_PLAYERS) && NeededPlayersCheck())
		return;

	// Team Extermination check
	// CounterTerrorists won by virture of elimination
	if (!(scenarioFlags & SCENARIO_BLOCK_TEAM_EXTERMINATION) && TeamExterminationCheck(NumAliveTerrorist, NumAliveCT, NumDeadTerrorist, NumDeadCT))
		return;

	// scenario not won - still in progress
}

void CHalfLifeMultiplay::InitializePlayerCounts(int &NumAliveTerrorist, int &NumAliveCT, int &NumDeadTerrorist, int &NumDeadCT)
{
	NumAliveTerrorist = NumAliveCT = NumDeadCT = NumDeadTerrorist = 0;
	m_iNumTerrorist = m_iNumCT = m_iNumSpawnableTerrorist = m_iNumSpawnableCT = 0;

	// initialize count dead/alive players

	// Count how many dead players there are on each team.
	CBaseEntity *pEntity = nullptr;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
	{
		if (FNullEnt(pEntity->edict()))
			break;

		if (pEntity->IsDormant())
			continue;

		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);
		switch (pPlayer->m_iTeam)
		{
		case CT:
		{
			m_iNumCT++;

			if (pPlayer->m_iMenu != Menu_ChooseAppearance)
			{
				m_iNumSpawnableCT++;
			}

			if (pPlayer->pev->deadflag != DEAD_NO)
				NumDeadCT++;
			else
				NumAliveCT++;

			break;
		}
		case TERRORIST:
		{
			m_iNumTerrorist++;

			if (pPlayer->m_iMenu != Menu_ChooseAppearance)
			{
				m_iNumSpawnableTerrorist++;
			}

			if (pPlayer->pev->deadflag != DEAD_NO)
				NumDeadTerrorist++;
			else
				NumAliveTerrorist++;

			break;
		}
		default:
			break;
		}
	}
}

bool EXT_FUNC CHalfLifeMultiplay::OnRoundEnd(int winStatus, ScenarioEventEndRound event, float tmDelay)
{
	switch (event)
	{
	case ROUND_CTS_WIN:                         return Round_Cts(tmDelay);
	case ROUND_TERRORISTS_WIN:                  return Round_Ts(tmDelay);
	case ROUND_END_DRAW:                        return Round_Draw(tmDelay);
	case ROUND_GAME_COMMENCE:                   return NeededPlayersCheck(tmDelay);
	case ROUND_GAME_RESTART:                    return RestartRoundCheck(tmDelay);
	case ROUND_GAME_OVER:                       return RoundOver(tmDelay);
	case ROUND_NONE:
	default:
		break;
	}

	return false;
}

bool EXT_FUNC CHalfLifeMultiplay::NeededPlayersCheck(float tmDelay)
{
	// Start the round immediately when the first person joins
	UTIL_LogPrintf("World triggered \"Game_Commencing\"\n");

	// Make sure we are not on the FreezePeriod.
	m_bFreezePeriod = FALSE;
	m_bCompleteReset = true;

	EndRoundMessage("#Game_Commencing", ROUND_GAME_COMMENCE);
	TerminateRound(tmDelay, WINSTATUS_DRAW);

	m_bGameStarted = true;
	if (TheBots)
	{
		TheBots->OnEvent(EVENT_GAME_COMMENCE);
	}

	return true;
}

bool EXT_FUNC CHalfLifeMultiplay::NeededPlayersCheck()
{
	// We needed players to start scoring
	// Do we have them now?
	// start the game, after the players entered in game
	if (!m_iNumSpawnableTerrorist || !m_iNumSpawnableCT)
	{
		UTIL_ClientPrintAll(HUD_PRINTCONSOLE, "#Game_scoring");
		m_bNeededPlayers = true;
		m_bGameStarted = false;
	}

	if (!m_bGameStarted && m_iNumSpawnableTerrorist != 0 && m_iNumSpawnableCT != 0)
	{
		if (IsCareer())
		{
			CBasePlayer *pPlayer = UTIL_PlayerByIndex(gpGlobals->maxClients);
			if (!pPlayer || !pPlayer->IsBot())
			{
				return true;
			}
		}

		return OnRoundEnd(WINSTATUS_DRAW, ROUND_GAME_COMMENCE, IsCareer() ? 0 : 3);
	}

	return false;
}

bool CHalfLifeMultiplay::Round_Cts(float tmDelay)
{
	Broadcast("ctwin");
	m_iAccountCT += m_rgRewardAccountRules[RR_CTS_WIN];

	if (!m_bNeededPlayers)
	{
		m_iNumCTWins++;
		// Update the clients team score
		UpdateTeamScores();
	}

	EndRoundMessage("#CTs_Win", ROUND_CTS_WIN);
	TerminateRound(tmDelay, WINSTATUS_CTS);

	if (IsCareer())
	{
		QueueCareerRoundEndMenu(tmDelay, WINSTATUS_CTS);
	}

	return true;
}

bool CHalfLifeMultiplay::Round_Ts(float tmDelay)
{
	Broadcast("terwin");
	m_iAccountTerrorist += m_rgRewardAccountRules[RR_TERRORISTS_WIN];

	if (!m_bNeededPlayers)
	{
		m_iNumTerroristWins++;
		// Update the clients team score
		UpdateTeamScores();
	}

	EndRoundMessage("#Terrorists_Win", ROUND_TERRORISTS_WIN);
	TerminateRound(tmDelay, WINSTATUS_TERRORISTS);

	if (IsCareer())
	{
		QueueCareerRoundEndMenu(tmDelay, WINSTATUS_TERRORISTS);
	}

	return true;
}

bool CHalfLifeMultiplay::Round_Draw(float tmDelay)
{
	EndRoundMessage("#Round_Draw", ROUND_END_DRAW);
	Broadcast("rounddraw");
	TerminateRound(tmDelay, WINSTATUS_DRAW);
	return true;
}

bool CHalfLifeMultiplay::TeamExterminationCheck(int NumAliveTerrorist, int NumAliveCT, int NumDeadTerrorist, int NumDeadCT)
{
	if ((m_iNumCT > 0 && m_iNumSpawnableCT > 0) && (m_iNumTerrorist > 0 && m_iNumSpawnableTerrorist > 0))
	{
		if (NumAliveTerrorist == 0 && NumDeadTerrorist != 0 && NumAliveCT > 0)
		{
			bool nowin = false;

			if (!nowin)
			{
				return OnRoundEnd(WINSTATUS_CTS, ROUND_CTS_WIN, GetRoundRestartDelay());
			}
		}

		// Terrorists WON
		else if (NumAliveCT == 0 && NumDeadCT != 0)
		{
			return OnRoundEnd(WINSTATUS_TERRORISTS, ROUND_TERRORISTS_WIN, GetRoundRestartDelay());
		}
	}
	else if (NumAliveCT == 0 && NumAliveTerrorist == 0)
	{
		return OnRoundEnd(WINSTATUS_DRAW, ROUND_END_DRAW, GetRoundRestartDelay());
	}

	return false;
}

void CHalfLifeMultiplay::SwapAllPlayers()
{
	CBaseEntity *pEntity = nullptr;

	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
	{
		if (FNullEnt(pEntity->edict()))
			break;

		if (pEntity->IsDormant())
			continue;

		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);
		pPlayer->SwitchTeam();
	}

	// Swap Team victories
	SWAP(m_iNumTerroristWins, m_iNumCTWins);

	// Update the clients team score
	UpdateTeamScores();
}

void EXT_FUNC CHalfLifeMultiplay::BalanceTeams()
{
	int iNumToSwap;
	TeamName iTeamToSwap = UNASSIGNED;

	m_bTeamBalanced = false;

	if (m_iNumCT > m_iNumTerrorist)
	{
		iTeamToSwap = CT;
		iNumToSwap = (m_iNumCT - m_iNumTerrorist) / 2;
	}
	else if (m_iNumTerrorist > m_iNumCT)
	{
		iTeamToSwap = TERRORIST;
		iNumToSwap = (m_iNumTerrorist - m_iNumCT) / 2;
	}
	else
	{
		// Teams are even.. Get out of here.
		return;
	}

	// Don't swap more than 4 players at a time.. This is a naive method of avoiding infinite loops.
	if (iNumToSwap > 4)
		iNumToSwap = 4;

	// last person to join the server
	int iHighestUserID = 0;
	CBasePlayer *toSwap = nullptr;

	for (int i = 1; i <= iNumToSwap; i++)
	{
		iHighestUserID = 0;
		toSwap = nullptr;

		CBaseEntity *pEntity = nullptr;

		// search for player with highest UserID = most recently joined to switch over
		while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
		{
			if (FNullEnt(pEntity->edict()))
				break;

			if (pEntity->IsDormant())
				continue;

			if (pEntity->entindex() == THE_COMMANDER->entindex() || pEntity->entindex() == THE_GODFATHER->entindex())
				continue;

			CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);

			if (pPlayer->CanSwitchTeam(iTeamToSwap) && GETPLAYERUSERID(pPlayer->edict()) > iHighestUserID)
			{
				iHighestUserID = GETPLAYERUSERID(pPlayer->edict());
				toSwap = pPlayer;
			}
		}

		if (toSwap) {
			m_bTeamBalanced = true;
			toSwap->SwitchTeam();
			m_bTeamBalanced = false;
		}
	}
}

void EXT_FUNC CHalfLifeMultiplay::CheckMapConditions()
{
	// See if the map has func_buyzone entities
	// Used by CBasePlayer::HandleSignals() to support maps without these entities
	m_bMapHasBuyZone = (UTIL_FindEntityByClassname(nullptr, "func_buyzone") != nullptr);
}

void EXT_FUNC CHalfLifeMultiplay::RestartRound()
{
	// tell bots that the round is restarting
	if (TheBots)
	{
		TheBots->RestartRound();
	}

	if (!m_bCompleteReset)
	{
		m_iTotalRoundsPlayed++;
	}

	ClearBodyQue();

	// Hardlock the player accelaration to 5.0
	CVAR_SET_FLOAT("sv_accelerate", 5.0);
	CVAR_SET_FLOAT("sv_friction", 4.0);
	CVAR_SET_FLOAT("sv_stopspeed", 75);

	// Tabulate the number of players on each team.
	m_iNumCT = CountTeamPlayers(CT);
	m_iNumTerrorist = CountTeamPlayers(TERRORIST);

	// reset all players health for HLTV
	MESSAGE_BEGIN(MSG_SPEC, gmsgHLTV);
		WRITE_BYTE(0);							// 0 = all players
		WRITE_BYTE(100 | DRC_FLAG_FACEPLAYER);	// 100 health + msg flag
	MESSAGE_END();

	// reset all players FOV for HLTV
	MESSAGE_BEGIN(MSG_SPEC, gmsgHLTV);
		WRITE_BYTE(0);		// all players
		WRITE_BYTE(0);		// to default FOV value
	MESSAGE_END();

	auto shouldBalancedOnNextRound = []() -> bool
	{
		return autoteambalance.value == 1;
	};

	if (shouldBalancedOnNextRound() && m_iUnBalancedRounds >= 1)
	{
		BalanceTeams();
	}

	if ((m_iNumCT - m_iNumTerrorist) >= 2 || (m_iNumTerrorist - m_iNumCT) >= 2)
	{
		m_iUnBalancedRounds++;
	}
	else
	{
		m_iUnBalancedRounds = 0;
	}

	// Warn the players of an impending auto-balance next round...
	if (shouldBalancedOnNextRound() && m_iUnBalancedRounds == 1)
	{
		UTIL_ClientPrintAll(HUD_PRINTCENTER, "#Auto_Team_Balance_Next_Round");
	}
	else if (autoteambalance.value >= 2 && m_iUnBalancedRounds >= 1)
	{
		BalanceTeams();
	}

	if (m_bCompleteReset)
	{
		// bounds check
		if (timelimit.value < 0)
		{
			CVAR_SET_FLOAT("mp_timelimit", 0);
		}

		m_flGameStartTime = gpGlobals->time;

		// Reset timelimit
		if (timelimit.value)
			m_flTimeLimit = gpGlobals->time + (timelimit.value * 60);

		// Reset total # of rounds played
		m_iTotalRoundsPlayed = 0;
		m_iMaxRounds = int(CVAR_GET_FLOAT("mp_maxrounds"));

		if (m_iMaxRounds < 0)
		{
			m_iMaxRounds = 0;
			CVAR_SET_FLOAT("mp_maxrounds", 0);
		}

		m_iMaxRoundsWon = int(CVAR_GET_FLOAT("mp_winlimit"));

		if (m_iMaxRoundsWon < 0)
		{
			m_iMaxRoundsWon = 0;
			CVAR_SET_FLOAT("mp_winlimit", 0);
		}

		// Reset score info
		m_iNumTerroristWins = 0;
		m_iNumCTWins = 0;
		m_iNumConsecutiveTerroristLoses = 0;
		m_iNumConsecutiveCTLoses = 0;

		// Reset team scores
		UpdateTeamScores();

		// Reset the player stats
		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);
			if (pPlayer && !FNullEnt(pPlayer->pev))
			{
				pPlayer->Reset();
			}
		}

		if (TheBots)
		{
			TheBots->OnEvent(EVENT_NEW_MATCH);
		}
	}

	m_bFreezePeriod = TRUE;
	m_bRoundTerminating = false;

	ReadMultiplayCvars();

	float flAutoKickIdle = autokick_timeout.value;

	// set the idlekick max time (in seconds)
	if (flAutoKickIdle > 0)
		m_fMaxIdlePeriod = flAutoKickIdle;
	else
		m_fMaxIdlePeriod = (((m_iRoundTime < 60) ? 60 : m_iRoundTime) * 2);

	// This makes the round timer function as the intro timer on the client side
	m_iRoundTimeSecs = m_iIntroRoundTime;

	// Check to see if there's a mapping info paramater entity
	if (g_pMapInfo)
		g_pMapInfo->CheckMapInfo();

	CheckMapConditions();

	int acct_tmp = 0;

	// Scale up the loser bonus when teams fall into losing streaks
	if (m_iRoundWinStatus == WINSTATUS_TERRORISTS)	// terrorists won
	{
		// check to see if they just broke a losing streak
		if (m_iNumConsecutiveTerroristLoses > 1)
		{
			// this is the default losing bonus
			m_iLoserBonus = m_rgRewardAccountRules[RR_LOSER_BONUS_MIN];
		}

		m_iNumConsecutiveTerroristLoses = 0;	// starting fresh
		m_iNumConsecutiveCTLoses++;				// increment the number of wins the CTs have had
	}
	else if (m_iRoundWinStatus == WINSTATUS_CTS)
	{
		// check to see if they just broke a losing streak
		if (m_iNumConsecutiveCTLoses > 1)
		{
			// this is the default losing bonus
			m_iLoserBonus = m_rgRewardAccountRules[RR_LOSER_BONUS_MIN];
		}

		m_iNumConsecutiveCTLoses = 0;		// starting fresh
		m_iNumConsecutiveTerroristLoses++;	// increment the number of wins the Terrorists have had
	}

	// check if the losing team is in a losing streak & that the loser bonus hasen't maxed out.
	if (m_iNumConsecutiveTerroristLoses > 1 && m_iLoserBonus < m_rgRewardAccountRules[RR_LOSER_BONUS_MAX])
	{
		// help out the team in the losing streak
		m_iLoserBonus += m_rgRewardAccountRules[RR_LOSER_BONUS_ADD];
	}
	else if (m_iNumConsecutiveCTLoses > 1 && m_iLoserBonus < m_rgRewardAccountRules[RR_LOSER_BONUS_MAX])
	{
		// help out the team in the losing streak
		m_iLoserBonus += m_rgRewardAccountRules[RR_LOSER_BONUS_ADD];
	}

	// assign the wining and losing bonuses
	if (m_iRoundWinStatus == WINSTATUS_TERRORISTS)	// terrorists won
	{
		m_iAccountTerrorist += acct_tmp;
		m_iAccountCT += m_iLoserBonus;
	}
	else if (m_iRoundWinStatus == WINSTATUS_CTS)	// CT Won
	{
		m_iAccountTerrorist += m_iLoserBonus;
		m_iAccountCT += acct_tmp;
	}

	// Update individual players accounts and respawn players

	// the round time stamp must be set before players are spawned
	m_fRoundStartTime = m_fRoundStartTimeReal = gpGlobals->time;

	// Adrian - No cash for anyone at first rounds! ( well, only the default. )
	if (m_bCompleteReset)
	{
		// No extra cash!
		m_iAccountTerrorist = m_iAccountCT = 0;

		// We are starting fresh. So it's like no one has ever won or lost.
		m_iNumTerroristWins = 0;
		m_iNumCTWins = 0;
		m_iNumConsecutiveTerroristLoses = 0;
		m_iNumConsecutiveCTLoses = 0;
		m_iLoserBonus = m_rgRewardAccountRules[RR_LOSER_BONUS_DEFAULT];
	}

	// Respawn entities (glass, doors, etc..)
	CleanUpMap();

	// tell bots that the round is restarting
	CBaseEntity *pEntity = nullptr;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
	{
		if (FNullEnt(pEntity->edict()))
			break;

		if (pEntity->pev->flags == FL_DORMANT)
			continue;

		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);

		pPlayer->m_iNumSpawns = 0;
		pPlayer->m_bTeamChanged = false;

		if (pPlayer->m_iTeam == CT)
		{
			if (!pPlayer->m_bReceivesNoMoneyNextRound)
			{
				pPlayer->AddAccount(m_iAccountCT, RT_ROUND_BONUS);
			}
		}
		else if (pPlayer->m_iTeam == TERRORIST)
		{
			if (!pPlayer->m_bReceivesNoMoneyNextRound)
			{
				pPlayer->AddAccount(m_iAccountTerrorist, RT_ROUND_BONUS);
			}
		}

		if (pPlayer->m_iTeam != UNASSIGNED && pPlayer->m_iTeam != SPECTATOR)
		{
			pPlayer->RoundRespawn();
		}

		// deprive all roles.
		// the function call of commander/godfather resign is included.
		pPlayer->AssignRole(Role_UNASSIGNED);

		// stop music from last round, etc...
		CLIENT_COMMAND(pPlayer->edict(), "stopsound\n");
		CLIENT_COMMAND(pPlayer->edict(), "mp3 stop\n");
	}

	// re-calculate menpower.
	m_rgiMenpowers[CT] = m_rgiMenpowers[TERRORIST] = menpower_per_player.value * (m_iNumCT + m_iNumTerrorist) / 2.0f;
	m_rgbMenpowerBroadcast[CT] = m_rgbMenpowerBroadcast[TERRORIST] = false;

	for (int iTeam = TERRORIST; iTeam <= CT; iTeam++)	// Doctrine_MassAssault on new round.
		if (m_rgTeamTacticalScheme[iTeam] == Doctrine_MassAssault)
			m_rgiMenpowers[iTeam] *= 2;

	if (TheBots)
	{
		TheBots->OnEvent(EVENT_BUY_TIME_START);
	}

	// Reset game variables
	m_flIntermissionEndTime = 0;
	m_flIntermissionStartTime = 0;
	m_flRestartRoundTime = 0;
	m_iAccountTerrorist = m_iAccountCT = 0;
	m_iRoundWinStatus = WINSTATUS_NONE;
	m_bLevelInitialized = false;
	m_bCompleteReset = false;
	m_flNextTSBallotBoxesOpen = 0;
}

BOOL CHalfLifeMultiplay::TeamFull(int team_id)
{
	switch (team_id)
	{
	case TERRORIST:
		return (m_iNumTerrorist >= m_iSpawnPointCount_Terrorist);

	case CT:
		return (m_iNumCT >= m_iSpawnPointCount_CT);
	}

	return FALSE;
}

// checks to see if the desired team is stacked, returns true if it is
BOOL CHalfLifeMultiplay::TeamStacked(int newTeam_id, int curTeam_id)
{
	// players are allowed to change to their own team
	if (newTeam_id == curTeam_id)
		return FALSE;

	if (!m_iLimitTeams)
		return FALSE;

	switch (newTeam_id)
	{
	case TERRORIST:
		if (curTeam_id != UNASSIGNED && curTeam_id != SPECTATOR)
			return ((m_iNumTerrorist + 1) > (m_iNumCT + m_iLimitTeams - 1));
		else
			return ((m_iNumTerrorist + 1) > (m_iNumCT + m_iLimitTeams));
	case CT:
		if (curTeam_id != UNASSIGNED && curTeam_id != SPECTATOR)
			return ((m_iNumCT + 1) > (m_iNumTerrorist + m_iLimitTeams - 1));
		else
			return ((m_iNumCT + 1) > (m_iNumTerrorist + m_iLimitTeams));
	}

	return FALSE;
}

void CHalfLifeMultiplay::Think()
{
	MonitorTutorStatus();
	m_VoiceGameMgr.Update(gpGlobals->frametime);

	if (g_psv_clienttrace->value != 1.0f)
	{
		CVAR_SET_FLOAT("sv_clienttrace", 1);
	}

	if (!m_fRoundStartTime)
	{
		// initialize the timer time stamps, this happens once only
		m_fRoundStartTime = m_fRoundStartTimeReal = gpGlobals->time;
	}

	if (m_flForceCameraValue != forcecamera.value
		|| m_flForceChaseCamValue != forcechasecam.value
		|| m_flFadeToBlackValue != fadetoblack.value)
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgForceCam);
			WRITE_BYTE(forcecamera.value != 0);
			WRITE_BYTE(forcechasecam.value != 0);
			WRITE_BYTE(fadetoblack.value != 0);
		MESSAGE_END();

		m_flForceCameraValue = forcecamera.value;
		m_flForceChaseCamValue = forcechasecam.value;
		m_flFadeToBlackValue = fadetoblack.value;
	}

	// Check game rules
	if (CheckGameOver())
		return;

	// have we hit the timelimit?
	if (CheckTimeLimit())
		return;

	// did somebody hit the fraglimit ?
	if (CheckFragLimit())
		return;

	if (!IsCareer())
	{
		// have we hit the max rounds?
		if (CheckMaxRounds())
			return;

		if (CheckWinLimit())
			return;
	}

	if (!IsCareer() || (m_fCareerMatchMenuTime <= 0.0 || m_fCareerMatchMenuTime >= gpGlobals->time))
	{
		if (m_iStoredSpectValue != allow_spectators.value)
		{
			m_iStoredSpectValue = allow_spectators.value;

			MESSAGE_BEGIN(MSG_ALL, gmsgAllowSpec);
				WRITE_BYTE(int(allow_spectators.value));
			MESSAGE_END();
		}

		// Check for the end of the round.
		if (IsFreezePeriod())
		{
			CheckFreezePeriodExpired();
		}
		else
		{
			CheckRoundTimeExpired();
		}

		if (m_flRestartRoundTime > 0.0f && m_flRestartRoundTime <= gpGlobals->time)
		{
			if (!IsCareer() || !m_fCareerRoundMenuTime)
			{
				RestartRound();
			}
			else if (TheCareerTasks)
			{
				bool isBotSpeaking = false;

				if (m_flRestartRoundTime + 10.0f > gpGlobals->time)
				{
					isBotSpeaking = IsBotSpeaking();
				}

				if (!isBotSpeaking)
				{
					if (m_fCareerMatchMenuTime == 0.0f && m_iCareerMatchWins)
					{
						bool canCTsWin = true;
						bool canTsWin = true;

						if (m_iNumCTWins < m_iCareerMatchWins || (m_iNumCTWins - m_iNumTerroristWins < m_iRoundWinDifference))
							canCTsWin = false;

						if (m_iNumTerroristWins < m_iCareerMatchWins || (m_iNumTerroristWins - m_iNumCTWins < m_iRoundWinDifference))
							canTsWin = false;

						if (!Q_strcmp(humans_join_team.string, "CT"))
						{
							if (!TheCareerTasks->AreAllTasksComplete())
							{
								canCTsWin = false;
							}
						}
						else if (!TheCareerTasks->AreAllTasksComplete())
						{
							canTsWin = false;
						}

						if (canCTsWin || canTsWin)
						{
							m_fCareerRoundMenuTime = 0;
							m_fCareerMatchMenuTime = gpGlobals->time + 3.0f;

							return;
						}
					}

					m_bFreezePeriod = TRUE;

					for (int i = 1; i <= gpGlobals->maxClients; i++)
					{
						CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);

						if (pPlayer && !pPlayer->IsBot())
						{
							MESSAGE_BEGIN(MSG_ONE, gmsgCZCareerHUD, nullptr, pPlayer->pev);
								WRITE_STRING("ROUND");
								WRITE_LONG(m_iNumCTWins);
								WRITE_LONG(m_iNumTerroristWins);
								WRITE_BYTE(m_iCareerMatchWins);
								WRITE_BYTE(m_iRoundWinDifference);
								WRITE_BYTE(m_iRoundWinStatus);
							MESSAGE_END();

							pPlayer->m_iHideHUD |= HIDEHUD_ALL;
							m_flRestartRoundTime = gpGlobals->time + 100000.0;

							UTIL_LogPrintf("Career Round %d %d %d %d\n", m_iRoundWinStatus, m_iNumCTWins, m_iNumTerroristWins, TheCareerTasks->AreAllTasksComplete());
							break;
						}
					}

					m_fCareerRoundMenuTime = 0;
				}
			}

			if (TheTutor)
			{
				TheTutor->PurgeMessages();
			}
		}

		CheckLevelInitialized();

		if (gpGlobals->time > m_tmNextPeriodicThink)
		{
			CheckRestartRound();
			m_tmNextPeriodicThink = gpGlobals->time + 1.0f;

			if (g_psv_accelerate->value != 5.0f)
			{
				CVAR_SET_FLOAT("sv_accelerate", 5.0f);
			}

			if (g_psv_friction->value != 4.0f)
			{
				CVAR_SET_FLOAT("sv_friction", 4.0f);
			}

			if (g_psv_stopspeed->value != 75.0f)
			{
				CVAR_SET_FLOAT("sv_stopspeed", 75.0f);
			}

			m_iMaxRounds = int(maxrounds.value);

			if (m_iMaxRounds < 0)
			{
				m_iMaxRounds = 0;
				CVAR_SET_FLOAT("mp_maxrounds", 0);
			}

			m_iMaxRoundsWon = int(winlimit.value);

			if (m_iMaxRoundsWon < 0)
			{
				m_iMaxRoundsWon = 0;
				CVAR_SET_FLOAT("mp_winlimit", 0);
			}
		}
	}
	else
	{
		if (m_fCareerMatchMenuTime + 10 <= gpGlobals->time || !IsBotSpeaking())
		{
			UTIL_CareerDPrintf("Ending career match...one team has won the specified number of rounds\n");

			MESSAGE_BEGIN(MSG_ALL, gmsgCZCareer);
				WRITE_STRING("MATCH");
				WRITE_LONG(m_iNumCTWins);
				WRITE_LONG(m_iNumTerroristWins);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ALL, gmsgCZCareerHUD);
				WRITE_STRING("MATCH");
				WRITE_LONG(m_iNumCTWins);
				WRITE_LONG(m_iNumTerroristWins);
				WRITE_BYTE(m_iCareerMatchWins);
				WRITE_BYTE(m_iRoundWinDifference);
				WRITE_BYTE(m_iRoundWinStatus);
			MESSAGE_END();

			UTIL_LogPrintf("Career Match %d %d %d %d\n", m_iRoundWinStatus, m_iNumCTWins, m_iNumTerroristWins, TheCareerTasks->AreAllTasksComplete());
			SERVER_COMMAND("setpause\n");
		}
	}

	// menpower check. just a broadcasting function, nothing related to win-lost determination.
	CheckMenpower(CT);
	CheckMenpower(TERRORIST);

	// team tactical scheme think
	if (m_flNextTSBallotBoxesOpen <= gpGlobals->time)
	{
		m_flNextTSBallotBoxesOpen = gpGlobals->time + ballot_boxes_opening_interval.value;

		if (IsFreezePeriod())
			m_flNextTSBallotBoxesOpen = gpGlobals->time + 0.1f;

		TacticalSchemes rgSave[4];
		for (int iTeam = TERRORIST; iTeam <= CT; iTeam++)
		{
			rgSave[iTeam] = m_rgTeamTacticalScheme[iTeam];
			m_rgTeamTacticalScheme[iTeam] = CalcTSVoteResult(TeamName(iTeam));

			if (rgSave[iTeam] != m_rgTeamTacticalScheme[iTeam])
			{
				CBasePlayer* pPlayer = nullptr;
				for (int i = 1; i <= gpGlobals->maxClients; i++)
				{
					pPlayer = UTIL_PlayerByIndex(i);

					if (!pPlayer || FNullEnt(pPlayer) || pPlayer->IsDormant())
						continue;

					if (pPlayer->IsBot())
						continue;

					if (pPlayer->m_iTeam == iTeam)
						UTIL_HudMessage(pPlayer, m_TextParam_Notification, "Executing new tactical scheme: %s", g_rgszTacticalSchemeNames[m_rgTeamTacticalScheme[iTeam]]);
				}

				if (m_rgTeamTacticalScheme[iTeam] == Doctrine_MassAssault)	// switching to Doctrine_MassAssault
					m_rgiMenpowers[iTeam] *= 2;
				else if (rgSave[iTeam] == Doctrine_MassAssault)	// switching to others
					m_rgiMenpowers[iTeam] /= 2;
			}
		}
	}
}

bool CHalfLifeMultiplay::CheckGameOver()
{
	// someone else quit the game already
	if (m_bGameOver)
	{
		// bounds check
		int time = int(CVAR_GET_FLOAT("mp_chattime"));

		if (time < 1)
			CVAR_SET_STRING("mp_chattime", "1");

		else if (time > MAX_INTERMISSION_TIME)
			CVAR_SET_STRING("mp_chattime", UTIL_dtos1(MAX_INTERMISSION_TIME));

		m_flIntermissionEndTime = m_flIntermissionStartTime + mp_chattime.value;

		// check to see if we should change levels now
		if (m_flIntermissionEndTime < gpGlobals->time && !IsCareer())
		{
			if (!UTIL_HumansInGame()				// if only bots, just change immediately
				|| IsMultiplayer()
				|| m_iEndIntermissionButtonHit		// check that someone has pressed a key, or the max intermission time is over
				|| ((m_flIntermissionStartTime + MAX_INTERMISSION_TIME) < gpGlobals->time))
			{
				// intermission is over
				ChangeLevel();
			}
		}

		return true;
	}

	return false;
}

bool CHalfLifeMultiplay::CheckTimeLimit()
{
	if (timelimit.value < 0)
	{
		CVAR_SET_FLOAT("mp_timelimit", 0);
		return false;
	}

	if (!IsCareer())
	{
		if (timelimit.value)
		{
			m_flTimeLimit = m_flGameStartTime + timelimit.value * 60.0f;

			if (gpGlobals->time >= m_flTimeLimit)
			{
				ALERT(at_console, "Changing maps because time limit has been met\n");
				GoToIntermission();
				return true;
			}
		}

		static int lastTime = 0;
		int timeRemaining = (int)(timelimit.value ? (m_flTimeLimit - gpGlobals->time) : 0);

		// Updates once per second
		if (timeRemaining != lastTime)
		{
			lastTime = timeRemaining;
			g_engfuncs.pfnCvar_DirectSet(&timeleft, UTIL_VarArgs("%02d:%02d", timeRemaining / 60, timeRemaining % 60));
		}
	}

	return false;
}

bool CHalfLifeMultiplay::CheckMaxRounds()
{
	if (m_iMaxRounds != 0 && m_iTotalRoundsPlayed >= m_iMaxRounds)
	{
		ALERT(at_console, "Changing maps due to maximum rounds have been met\n");
		GoToIntermission();
		return true;
	}

	return false;
}

bool CHalfLifeMultiplay::CheckWinLimit()
{
	// has one team won the specified number of rounds?
	if (m_iMaxRoundsWon != 0 && (m_iNumCTWins >= m_iMaxRoundsWon || m_iNumTerroristWins >= m_iMaxRoundsWon))
	{
		if ((m_iNumCTWins - m_iNumTerroristWins >= m_iRoundWinDifference) || (m_iNumTerroristWins - m_iNumCTWins >= m_iRoundWinDifference))
		{
			ALERT(at_console, "Changing maps...one team has won the specified number of rounds\n");
			GoToIntermission();
			return true;
		}
	}

	return false;
}

bool CHalfLifeMultiplay::CheckFragLimit()
{
	int fragsRemaining = 0;

	if (fraglimit.value >= 1)
	{
		int bestFrags = fraglimit.value;

		// check if any player is over the frag limit
		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			auto pPlayer = UTIL_PlayerByIndex(i);

			if (!pPlayer || pPlayer->has_disconnected)
				continue;

			if (pPlayer->pev->frags >= fraglimit.value)
			{
				ALERT(at_console, "Changing maps because frag limit has been met\n");
				GoToIntermission();
				return true;
			}

			int remain = (int)(fraglimit.value - pPlayer->pev->frags);
			if (remain < bestFrags)
			{
				bestFrags = remain;
			}
		}

		fragsRemaining = bestFrags;
	}

	static int lastFrags = 0;

	// Updates when frags change
	if (fragsRemaining != lastFrags)
	{
		lastFrags = fragsRemaining;
		g_engfuncs.pfnCvar_DirectSet(&fragsleft, UTIL_VarArgs("%i", fragsRemaining));
	}

	return false;
}

void EXT_FUNC CHalfLifeMultiplay::OnRoundFreezeEnd()
{
	// Log this information
	UTIL_LogPrintf("World triggered \"Round_Start\"\n");

	// Freeze period expired: kill the flag
	m_bFreezePeriod = FALSE;

	char CT_sentence[40];
	char T_sentence[40];

	switch (RANDOM_LONG(0, 3))
	{
	case 0:
		Q_strncpy(CT_sentence, "%!MRAD_MOVEOUT", sizeof(CT_sentence));
		Q_strncpy(T_sentence, "%!MRAD_MOVEOUT", sizeof(T_sentence));
		break;
	case 1:
		Q_strncpy(CT_sentence, "%!MRAD_LETSGO", sizeof(CT_sentence));
		Q_strncpy(T_sentence, "%!MRAD_LETSGO", sizeof(T_sentence));
		break;
	case 2:
		Q_strncpy(CT_sentence, "%!MRAD_LOCKNLOAD", sizeof(CT_sentence));
		Q_strncpy(T_sentence, "%!MRAD_LOCKNLOAD", sizeof(T_sentence));
		break;
	default:
		Q_strncpy(CT_sentence, "%!MRAD_GO", sizeof(CT_sentence));
		Q_strncpy(T_sentence, "%!MRAD_GO", sizeof(T_sentence));
		break;
	}

	// Reset the round time
	m_fRoundStartTimeReal = m_fRoundStartTime = gpGlobals->time;

	// in seconds
	m_iRoundTimeSecs = m_iRoundTime;

	bool bCTPlayed = false;
	bool bTPlayed = false;

	if (TheCareerTasks)
	{
		TheCareerTasks->HandleEvent(EVENT_ROUND_START);
	}

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *plr = UTIL_PlayerByIndex(i);
		if (!plr || plr->pev->flags == FL_DORMANT)
			continue;

		if (plr->m_iJoiningState == JOINED)
		{
			if (plr->m_iTeam == CT && !bCTPlayed)
			{
				plr->Radio(CT_sentence);
				bCTPlayed = true;
			}
			else if (plr->m_iTeam == TERRORIST && !bTPlayed)
			{
				plr->Radio(T_sentence);
				bTPlayed = true;
			}

			if (plr->m_iTeam != SPECTATOR)
			{
				plr->ResetMaxSpeed();
				plr->m_bCanShoot = true;
			}
		}

		plr->SyncRoundTimer();
	}

	// allocate roles left.
	if (!THE_COMMANDER.IsValid())
		AssignCommander(RandomNonroleCharacter(CT));

	if (!THE_GODFATHER.IsValid())
		AssignGodfather(RandomNonroleCharacter(TERRORIST));

	for (int iRole = 1; iRole < ROLE_COUNT; iRole++)
	{
		if (!m_rgpCharacters[iRole].IsValid())
		{
			if (iRole <= Role_Medic)	// CT
			{
				CBasePlayer* pPlayer = RandomNonroleCharacter(CT);

				if (pPlayer)
					pPlayer->AssignRole((RoleTypes)iRole);

				// it is not either command nor godfather.
				// we don't need special functions to assign them.
			}
			else if (iRole <= Role_Arsonist)
			{
				CBasePlayer* pPlayer = RandomNonroleCharacter(TERRORIST);

				if (pPlayer)
					pPlayer->AssignRole((RoleTypes)iRole);
			}
		}
	}

	if (TheBots)
	{
		TheBots->OnEvent(EVENT_ROUND_START);
	}

	if (TheCareerTasks)
	{
		TheCareerTasks->HandleEvent(EVENT_ROUND_START);
	}
}

void CHalfLifeMultiplay::CheckFreezePeriodExpired()
{
	if (GetRoundRemainingTime() > 0)
		return;

	OnRoundFreezeEnd();
}

bool CHalfLifeMultiplay::RoundOver(float tmDelay)
{
	EndRoundMessage("#Cstrike_Tutor_Round_Over", ROUND_GAME_OVER);
	Broadcast("rounddraw");
	TerminateRound(tmDelay, WINSTATUS_DRAW);
	return true;
}

void CHalfLifeMultiplay::CheckRoundTimeExpired()
{
	if (HasRoundInfinite(SCENARIO_BLOCK_TIME_EXPRIRED))
		return;

	if (!HasRoundTimeExpired())
		return;

	if (roundover.value)
	{
		// round is over
		if (!OnRoundEnd(WINSTATUS_DRAW, ROUND_GAME_OVER, GetRoundRestartDelay()))
			return;
	}

	// This is done so that the portion of code has enough time to do it's thing.
	m_fRoundStartTime = gpGlobals->time + 60.0f;
}

void CHalfLifeMultiplay::CheckLevelInitialized()
{
	if (!m_bLevelInitialized)
	{
		// Count the number of spawn points for each team
		// This determines the maximum number of players allowed on each
		CBaseEntity *pEnt = nullptr;

		m_iSpawnPointCount_Terrorist = 0;
		m_iSpawnPointCount_CT = 0;

		while ((pEnt = UTIL_FindEntityByClassname(pEnt, "info_player_deathmatch")))
			m_iSpawnPointCount_Terrorist++;

		while ((pEnt = UTIL_FindEntityByClassname(pEnt, "info_player_start")))
			m_iSpawnPointCount_CT++;

		m_bLevelInitialized = true;
	}
}

bool CHalfLifeMultiplay::RestartRoundCheck(float tmDelay)
{
	// log the restart
	UTIL_LogPrintf("World triggered \"Restart_Round_(%i_%s)\"\n", (int)tmDelay, (tmDelay == 1) ? "second" : "seconds");
	UTIL_LogPrintf("Team \"CT\" scored \"%i\" with \"%i\" players\n", m_iNumCTWins, m_iNumCT);
	UTIL_LogPrintf("Team \"TERRORIST\" scored \"%i\" with \"%i\" players\n", m_iNumTerroristWins, m_iNumTerrorist);

	// let the players know
	UTIL_ClientPrintAll(HUD_PRINTCENTER, "#Game_will_restart_in", UTIL_dtos1(tmDelay), (tmDelay == 1) ? "SECOND" : "SECONDS");
	UTIL_ClientPrintAll(HUD_PRINTCONSOLE, "#Game_will_restart_in_console", UTIL_dtos1(tmDelay), (tmDelay == 1) ? "SECOND" : "SECONDS");

	m_flRestartRoundTime = gpGlobals->time + tmDelay;
	m_bCompleteReset = true;

	CVAR_SET_FLOAT("sv_restartround", 0);
	CVAR_SET_FLOAT("sv_restart", 0);

	CareerRestart();
	return true;
}

void CHalfLifeMultiplay::CheckRestartRound()
{
	// Restart the round if specified by the server
	int iRestartDelay = int(restartround.value);
	if (!iRestartDelay)
	{
		iRestartDelay = sv_restart.value;
	}

	if (iRestartDelay > 0)
	{
		OnRoundEnd(WINSTATUS_NONE, ROUND_GAME_RESTART, iRestartDelay);
	}
}

bool CHalfLifeMultiplay::HasRoundTimeExpired()
{
	if (!m_iRoundTime)
		return false;

	// We haven't completed other objectives, so go for this!.
	if (GetRoundRemainingTime() > 0 || m_iRoundWinStatus != WINSTATUS_NONE)
	{
		return false;
	}

	return false;
}

bool CHalfLifeMultiplay::CanSkillBeUsed()
{
	return !IsFreezePeriod();
}

void CHalfLifeMultiplay::AssignCommander(CBasePlayer *pPlayer)
{
	float flSucceedHealth = commander_maxhealth.value;

	if (THE_COMMANDER.IsValid())
	{
		CBasePlayer *iAbdicator = THE_COMMANDER;

		MESSAGE_BEGIN(MSG_ALL, gmsgScoreAttrib);
		WRITE_BYTE(iAbdicator->entindex());
		WRITE_BYTE(0);
		MESSAGE_END();

		pPlayer ? iAbdicator->AssignRole(FindAvaliableRole(TERRORIST)) : iAbdicator->m_iRoleType = Role_UNASSIGNED;	// no new godfather means new round.
		flSucceedHealth = iAbdicator->pev->health;	// this health will be assign to new leader. prevents the confidence motion mechanism abused by players.

		iAbdicator->pev->health = 100.0f * (iAbdicator->pev->health / iAbdicator->pev->max_health);
		iAbdicator->pev->max_health = 100.0f;

		// UNDONE: reset player model.
	}

	if (FNullEnt(pPlayer))	// no new commander assigned? new round?
	{
		THE_COMMANDER = nullptr;
		return;
	}

	if (!pPlayer->IsAlive())	// what if this guy was dead?
	{
		pPlayer->Spawn();
		pPlayer->pev->button = 0;
		pPlayer->pev->nextthink = -1;
	}

	// LONG LIVE THE KING!
	THE_COMMANDER = pPlayer;
	// UNDONE: set player model.
	THE_COMMANDER->pev->health = flSucceedHealth;
	THE_COMMANDER->pev->max_health = commander_maxhealth.value;
	THE_COMMANDER->AssignRole(Role_Commander);

	UTIL_HudMessage(THE_COMMANDER, CSGameRules()->m_TextParam_Notification, "You are now the %s!", g_rgszRoleNames[Role_Commander]);

	MESSAGE_BEGIN(MSG_ALL, gmsgScoreAttrib);
	WRITE_BYTE(THE_COMMANDER->entindex());	// head of CTs
	WRITE_BYTE(SCORE_STATUS_VIP);
	MESSAGE_END();

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		pPlayer = UTIL_PlayerByIndex(i);

		if (!pPlayer || !pPlayer->IsAlive() || pPlayer->IsBot())
			continue;

		switch (pPlayer->m_iTeam)
		{
		case CT:
			UTIL_PrintChatColor(pPlayer, BLUECHAT, "/t%s/y is your /t%s/y in this operation./g PROTECT HIM!", STRING(THE_COMMANDER->pev->netname), g_rgszRoleNames[Role_Commander]);
			break;

		case TERRORIST:
			UTIL_PrintChatColor(pPlayer, REDCHAT, "/y%s/t is the /y%s/t, the head of CTs. To stop this assault, kill him ASAP.", STRING(THE_COMMANDER->pev->netname), g_rgszRoleNames[Role_Commander]);
			break;

		default:
			UTIL_PrintChatColor(pPlayer, BLUECHAT, "/t%s/y is the /t%s/y of this round.", STRING(THE_COMMANDER->pev->netname), g_rgszRoleNames[Role_Commander]);
			break;
		}
	}
}

void CHalfLifeMultiplay::AssignGodfather(CBasePlayer* pPlayer)
{
	float flSucceedHealth = godfather_maxhealth.value;

	if (THE_GODFATHER.IsValid())	// deal with the old godfather...
	{
		CBasePlayer* iAbdicator = THE_GODFATHER;

		MESSAGE_BEGIN(MSG_ALL, gmsgScoreAttrib);
		WRITE_BYTE(iAbdicator->entindex());
		WRITE_BYTE(0);
		MESSAGE_END();

		pPlayer ? iAbdicator->AssignRole(FindAvaliableRole(TERRORIST)) : iAbdicator->m_iRoleType = Role_UNASSIGNED;	// no new godfather means new round.
		flSucceedHealth = iAbdicator->pev->health;	// this health will be assign to new leader. prevents the confidence motion mechanism abused by players.

		iAbdicator->pev->health = 100.0f * (iAbdicator->pev->health / iAbdicator->pev->max_health);
		iAbdicator->pev->max_health = 100.0f;

		// UNDONE: reset player model.
	}

	if (FNullEnt(pPlayer))	// no new commander assigned? new round?
	{
		THE_GODFATHER = nullptr;
		return;
	}

	if (!pPlayer->IsAlive())	// what if this guy was dead?
	{
		pPlayer->Spawn();
		pPlayer->pev->button = 0;
		pPlayer->pev->nextthink = -1;
	}

	// LONG LIVE THE KING!
	THE_GODFATHER = pPlayer;
	// UNDONE: set player model.
	THE_GODFATHER->pev->health = flSucceedHealth;
	THE_GODFATHER->pev->max_health = godfather_maxhealth.value;
	THE_GODFATHER->AssignRole(Role_Godfather);

	UTIL_HudMessage(THE_GODFATHER, CSGameRules()->m_TextParam_Notification, "You are now the %s!", g_rgszRoleNames[Role_Godfather]);

	MESSAGE_BEGIN(MSG_ALL, gmsgScoreAttrib);
	WRITE_BYTE(THE_GODFATHER->entindex());	// head of TRs
	WRITE_BYTE(SCORE_STATUS_BOMB);
	MESSAGE_END();

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		pPlayer = UTIL_PlayerByIndex(i);

		if (!pPlayer || !pPlayer->IsAlive() || pPlayer->IsBot())
			continue;

		switch (pPlayer->m_iTeam)
		{
		case CT:
			UTIL_PrintChatColor(pPlayer, REDCHAT, "/t%s/y is the /t%s/y, the most valued target of this operation.", STRING(THE_GODFATHER->pev->netname), g_rgszRoleNames[Role_Godfather]);
			break;

		case TERRORIST:
			UTIL_PrintChatColor(pPlayer, GREYCHAT, "/g%s/t is your boss and /g%s/t. Make sure he survive in this assault.", STRING(THE_GODFATHER->pev->netname), g_rgszRoleNames[Role_Godfather]);
			break;

		default:
			UTIL_PrintChatColor(pPlayer, REDCHAT, "/t%s/y is the /t%s/y of this round.", STRING(THE_GODFATHER->pev->netname), g_rgszRoleNames[Role_Godfather]);
			break;
		}
	}
}

RoleTypes CHalfLifeMultiplay::FindAvaliableRole(TeamName iTeam)
{
	if (iTeam != CT && iTeam != TERRORIST)
		return Role_UNASSIGNED;

	RoleTypes iStart = Role_Commander, iEnd = Role_Medic;
	if (iTeam == TERRORIST)
	{
		iStart = Role_Godfather;
		iEnd = Role_Arsonist;
	}

	CBasePlayer* pPlayer = nullptr;
	for (int j = iStart; j <= iEnd; j++)
	{
		bool bUnoccupied = true;

		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			pPlayer = UTIL_PlayerByIndex(i);

			if (!pPlayer)
				continue;

			if (pPlayer->m_iRoleType == j)
			{
				bUnoccupied = false;
				break;
			}
		}

		if (bUnoccupied)
			return RoleTypes(j);
	}

	return Role_UNASSIGNED;
}

// Living players on the given team need to be marked as not receiving any money next round.
void CHalfLifeMultiplay::MarkLivingPlayersOnTeamAsNotReceivingMoneyNextRound(int iTeam)
{
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);

		if (!pPlayer || FNullEnt(pPlayer->pev))
			continue;

		if (pPlayer->m_iTeam == iTeam)
		{
			if (pPlayer->pev->health > 0 && pPlayer->pev->deadflag == DEAD_NO)
			{
				pPlayer->m_bReceivesNoMoneyNextRound = true;
			}
		}
	}
}

void CHalfLifeMultiplay::CareerRestart()
{
	m_bGameOver = false;

	if (m_flRestartRoundTime == 0.0f)
	{
		m_flRestartRoundTime = gpGlobals->time + 1.0f;
	}

	// for reset everything
	m_bCompleteReset = true;
	m_fCareerRoundMenuTime = 0;
	m_fCareerMatchMenuTime = 0;

	if (TheCareerTasks)
	{
		TheCareerTasks->Reset(false);
	}

	m_bSkipSpawn = false;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);

		if (!pPlayer || FNullEnt(pPlayer->pev))
			continue;

		if (!pPlayer->IsBot())
		{
			pPlayer->ForceClientDllUpdate();
		}
	}
}

BOOL CHalfLifeMultiplay::IsMultiplayer()
{
	return TRUE;
}

BOOL CHalfLifeMultiplay::IsDeathmatch()
{
	return TRUE;
}

BOOL CHalfLifeMultiplay::IsCoOp()
{
	return gpGlobals->coop;
}

BOOL EXT_FUNC CHalfLifeMultiplay::FShouldSwitchWeapon(CBasePlayer *pPlayer, CBaseWeapon *pWeapon)
{
	// TODO: maybe reuse this ?
	/*if (!pWeapon->CanDeploy())
	{
		// that weapon can't deploy anyway.
		return FALSE;
	}*/

	if (!pPlayer->m_pActiveItem)
	{
		// player doesn't have an active item!
		return TRUE;
	}

	if (!pPlayer->m_iAutoWepSwitch)
		return FALSE;

	// TODO: maybe reuse this?
	/*if (!pPlayer->m_pActiveItem->CanHolster())
	{
		// can't put away the active item.
		return FALSE;
	}*/

	if (pWeapon->m_pItemInfo->m_iWeight > pPlayer->m_pActiveItem->m_pItemInfo->m_iWeight)
		return TRUE;

	return FALSE;
}

BOOL EXT_FUNC CHalfLifeMultiplay::GetNextBestWeapon(CBasePlayer *pPlayer, CBaseWeapon *pCurrentWeapon)
{
	CBaseWeapon *pCheck;
	CBaseWeapon *pBest; // this will be used in the event that we don't find a weapon in the same category.
	int iBestWeight;
	int i;

	// TODO
	/*if (!pCurrentWeapon->CanHolster())
	{
		// can't put this gun away right now, so can't switch.
		return FALSE;
	}*/

	iBestWeight = -1; // no weapon lower than -1 can be autoswitched to
	pBest = nullptr;

	for (i = 0; i < MAX_ITEM_TYPES; i++)
	{
		pCheck = pPlayer->m_rgpPlayerItems[i];

		if (pCheck)
		{
			// don't reselect the weapon we're trying to get rid of
			if (pCheck->m_pItemInfo->m_iWeight > iBestWeight && pCheck != pCurrentWeapon)
			{
				//ALERT (at_console, "Considering %s\n", STRING(pCheck->pev->classname));
				// we keep updating the 'best' weapon just in case we can't find a weapon of the same weight
				// that the player was using. This will end up leaving the player with his heaviest-weighted weapon.

				// TODO
				//if (pCheck->CanDeploy())
				//{
					// if this weapon is useable, flag it as the best
					iBestWeight = pCheck->m_pItemInfo->m_iWeight;
					pBest = pCheck;
				//}
			}

			// this is for original CBasePlayerItem.
			//pCheck = pCheck->m_pNext;
		}
	}

	// if we make it here, we've checked all the weapons and found no useable
	// weapon in the same catagory as the current weapon.

	// if pBest is null, we didn't find ANYTHING. Shouldn't be possible- should always
	// at least get the crowbar, but ya never know.
	if (!pBest)
	{
		return FALSE;
	}

	pPlayer->SwitchWeapon(pBest);
	return TRUE;
}

BOOL CHalfLifeMultiplay::ClientCommand_DeadOrAlive(CBasePlayer *pPlayer, const char *pcmd)
{
	return m_VoiceGameMgr.ClientCommand(pPlayer, pcmd);
}

BOOL CHalfLifeMultiplay::ClientCommand(CBasePlayer *pPlayer, const char *pcmd)
{
	return FALSE;
}

bool CHalfLifeMultiplay::ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128])
{
	if (pEntity)
	{
		CBasePlayer* pPlayer = CBasePlayer::Instance(pEntity);
		if (pPlayer)
		{
			pPlayer->Reset();
		}
	}

	m_VoiceGameMgr.ClientConnected(pEntity);
	return TRUE;
}

void CHalfLifeMultiplay::UpdateGameMode(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, nullptr, pPlayer->edict());
		WRITE_BYTE(1);
	MESSAGE_END();
}

void CHalfLifeMultiplay::InitHUD(CBasePlayer *pl)
{
	int i;

	// notify other clients of player joining the game
	UTIL_LogPrintf("\"%s<%i><%s><>\" entered the game\n", STRING(pl->pev->netname), GETPLAYERUSERID(pl->edict()), GETPLAYERAUTHID(pl->edict()));

	UpdateGameMode(pl);

	if (!CVAR_GET_FLOAT("sv_cheats"))
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgViewMode, nullptr, pl->edict());
		MESSAGE_END();
	}

	// sending just one score makes the hud scoreboard active; otherwise
	// it is just disabled for single play
	MESSAGE_BEGIN(MSG_ONE, gmsgScoreInfo, nullptr, pl->edict());
		WRITE_BYTE(ENTINDEX(pl->edict()));
		WRITE_SHORT(0);
		WRITE_SHORT(0);
		WRITE_SHORT(0);
		WRITE_SHORT(pl->m_iTeam);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ONE, gmsgShadowIdx, nullptr, pl->edict());
		WRITE_LONG(g_iShadowSprite);
	MESSAGE_END();

	if (IsCareer())
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgCZCareer, nullptr, pl->edict());
			WRITE_STRING("START");
			WRITE_SHORT(m_iRoundTime);
		MESSAGE_END();
	}
	else
		SendMOTDToClient(pl->edict());

	// loop through all active players and send their score info to the new client
	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		// FIXME: Probably don't need to cast this just to read m_iDeaths
		CBasePlayer *plr = UTIL_PlayerByIndex(i);
		if (!plr)
			continue;

		if (plr->IsDormant())
			continue;

		MESSAGE_BEGIN(MSG_ONE, gmsgScoreInfo, nullptr, pl->edict());
			WRITE_BYTE(i);	// client number
			WRITE_SHORT(int(plr->pev->frags));
			WRITE_SHORT(plr->m_iDeaths);
			WRITE_SHORT(0);
			WRITE_SHORT(plr->m_iTeam);
		MESSAGE_END();
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgTeamScore, nullptr, pl->edict());
		WRITE_STRING("TERRORIST");
		WRITE_SHORT(m_iNumTerroristWins);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ONE, gmsgTeamScore, nullptr, pl->edict());
		WRITE_STRING("CT");
		WRITE_SHORT(m_iNumCTWins);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ONE, gmsgAllowSpec, nullptr, pl->edict());
		WRITE_BYTE(int(allow_spectators.value));
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ONE, gmsgForceCam, nullptr, pl->edict());
		WRITE_BYTE(forcecamera.value != 0);
		WRITE_BYTE(forcechasecam.value != 0);
		WRITE_BYTE(fadetoblack.value != 0);
	MESSAGE_END();

	if (m_bGameOver)
	{
		MESSAGE_BEGIN(MSG_ONE, SVC_INTERMISSION, nullptr, pl->edict());
		MESSAGE_END();
	}

	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *plr = UTIL_PlayerByIndex(i);
		if (!plr)
			continue;

		if (plr->IsDormant())
			continue;

		MESSAGE_BEGIN(MSG_ONE, gmsgTeamInfo, nullptr, pl->edict());
			WRITE_BYTE(plr->entindex());
			WRITE_STRING(GetTeamName(plr->m_iTeam));
		MESSAGE_END();

		plr->SetScoreboardAttributes(pl);

		if (pl->entindex() != i)
		{
			if (plr->pev->deadflag == DEAD_NO && plr->m_iTeam == pl->m_iTeam)
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgRadar, nullptr, pl->edict());
					WRITE_BYTE(plr->entindex());
					WRITE_COORD(plr->pev->origin.x);
					WRITE_COORD(plr->pev->origin.y);
					WRITE_COORD(plr->pev->origin.z);
				MESSAGE_END();
			}
		}

		MESSAGE_BEGIN(MSG_ONE, gmsgHealthInfo, nullptr, pl->edict());
			WRITE_BYTE(plr->entindex());
			WRITE_LONG(plr->ShouldToShowHealthInfo(pl) ? plr->m_iClientHealth : -1 /* means that 'HP' field will be hidden */);
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ONE, gmsgAccount, nullptr, pl->edict());
			WRITE_BYTE(plr->entindex());
			WRITE_LONG(plr->ShouldToShowAccount(pl) ? plr->m_iAccount : -1 /* means that this 'Money' will be hidden */);
		MESSAGE_END();
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgShowTimer, nullptr, pl->pev);
	MESSAGE_END();
}

void CHalfLifeMultiplay::ClientDisconnected(edict_t *pClient)
{
	if (pClient)
	{
		CBasePlayer *pPlayer = CBasePlayer::Instance(pClient);
		if (pPlayer)
		{
			pPlayer->has_disconnected = true;
			pPlayer->pev->deadflag = DEAD_DEAD;
			pPlayer->SetScoreboardAttributes();

			pPlayer->m_iCurrentKickVote = 0;

			if (pPlayer->m_iMapVote)
			{
				m_iMapVotes[pPlayer->m_iMapVote]--;

				if (m_iMapVotes[pPlayer->m_iMapVote] < 0)
				{
					m_iMapVotes[pPlayer->m_iMapVote] = 0;
				}
			}

			MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
				WRITE_BYTE(ENTINDEX(pClient));
				WRITE_SHORT(0);
				WRITE_SHORT(0);
				WRITE_SHORT(0);
				WRITE_SHORT(0);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ALL, gmsgTeamInfo);
				WRITE_BYTE(ENTINDEX(pClient));
				WRITE_STRING("UNASSIGNED");
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ALL, gmsgLocation);
				WRITE_BYTE(ENTINDEX(pClient));
				WRITE_STRING("");
			MESSAGE_END();

			char *team = GetTeam(pPlayer->m_iTeam);

			FireTargets("game_playerleave", pPlayer, pPlayer, USE_TOGGLE, 0);
			UTIL_LogPrintf("\"%s<%i><%s><%s>\" disconnected\n", STRING(pPlayer->pev->netname), GETPLAYERUSERID(pPlayer->edict()), GETPLAYERAUTHID(pPlayer->edict()), team);

			// destroy all of the players weapons and items
			pPlayer->RemoveAllItems(TRUE);

			CBasePlayer *pObserver = nullptr;
			while ((pObserver = UTIL_FindEntityByClassname(pObserver, "player")))
			{
				if (FNullEnt(pObserver->edict()))
					break;

				if (!pObserver->pev || pObserver == pPlayer)
					continue;

				// If a spectator was chasing this player, move him/her onto the next player
				if (pObserver->m_hObserverTarget == pPlayer)
				{
					int iMode = pObserver->pev->iuser1;

					pObserver->pev->iuser1 = OBS_NONE;
					pObserver->Observer_SetMode(iMode);
				}
			}
		}
	}

	CheckWinConditions();
}

float EXT_FUNC CHalfLifeMultiplay::FlPlayerFallDamage(CBasePlayer *pPlayer)
{
	pPlayer->m_flFallVelocity -= MAX_PLAYER_SAFE_FALL_SPEED;
	return pPlayer->m_flFallVelocity * DAMAGE_FOR_FALL_SPEED * 1.25;
}

BOOL EXT_FUNC CHalfLifeMultiplay::FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker)
{
	if (!pAttacker || PlayerRelationship(pPlayer, pAttacker) != GR_TEAMMATE)
	{
		return TRUE;
	}

	if (friendlyfire.value != 0.0f || pAttacker == pPlayer)
	{
		return TRUE;
	}

	return FALSE;
}

void CHalfLifeMultiplay::PlayerThink(CBasePlayer *pPlayer)
{
	if (m_bGameOver)
	{
		// check for button presses
		if (!IsCareer() && (pPlayer->m_afButtonPressed & (IN_DUCK | IN_ATTACK | IN_ATTACK2 | IN_USE | IN_JUMP)))
		{
			m_iEndIntermissionButtonHit = TRUE;
		}

		// clear attack/use commands from player
		pPlayer->m_afButtonPressed = 0;
		pPlayer->pev->button = 0;
		pPlayer->m_afButtonReleased = 0;
	}

	if (!pPlayer->m_bCanShoot && !IsFreezePeriod())
	{
		pPlayer->m_bCanShoot = true;
	}

	if (pPlayer->m_pActiveItem)
	{
		if (pPlayer->m_pActiveItem->m_bitsFlags & WPNSTATE_SHIELD_DRAWN)
		{
			pPlayer->m_bCanShoot = false;
		}

		// Superior Firepower Doctrine allows you have your clip constantly regen.
		if (m_rgTeamTacticalScheme[pPlayer->m_iTeam] == Doctrine_SuperiorFirepower && pPlayer->m_flTSThink < gpGlobals->time)
		{
			pPlayer->m_flTSThink = gpGlobals->time + 1.0f;

			int iClipAdd = 0; //int(float(pWeapon->iinfo()->m_iMaxClip) * 0.04f)

			if (pPlayer->m_pActiveItem->m_pItemInfo->m_iMaxClip >= 113)	// 4.5 / 0.04
				iClipAdd = 5;
			else if (pPlayer->m_pActiveItem->m_pItemInfo->m_iMaxClip >= 88)
				iClipAdd = 4;
			else if (pPlayer->m_pActiveItem->m_pItemInfo->m_iMaxClip >= 63)
				iClipAdd = 3;
			else if (pPlayer->m_pActiveItem->m_pItemInfo->m_iMaxClip >= 38)
				iClipAdd = 2;
			else if (pPlayer->m_pActiveItem->m_pItemInfo->m_iMaxClip >= 13 && pPlayer->m_pActiveItem->m_iId != WEAPON_M14EBR)	// nerf the stupit semi-auto sharpshooter rifle.
				iClipAdd = 1;

			if (iClipAdd > 0 && pPlayer->m_pActiveItem->m_iClip < 127)
			{
				pPlayer->m_pActiveItem->m_iClip += iClipAdd;

				if (pPlayer->m_pActiveItem->m_iClip > 127)
					pPlayer->m_pActiveItem->m_iClip = 127;

				if (!RANDOM_LONG(0, 3))	// if you play it constantly, it IS annoying.
				{
					char szSFX[64];
					Q_snprintf(szSFX, sizeof(szSFX) - 1, SFX_TSD_SFD, RANDOM_LONG(1, 7));
					UTIL_PlayEarSound(pPlayer, szSFX);
				}
			}
		}
	}

	// Doctrine_GrandBattleplan gives player 50$ every 5 sec.
	if (m_rgTeamTacticalScheme[pPlayer->m_iTeam] == Doctrine_GrandBattleplan && pPlayer->m_flTSThink < gpGlobals->time)
	{
		pPlayer->m_flTSThink = gpGlobals->time + 5.0f;

		pPlayer->AddAccount(50, RT_GBD_GIFTED);
		UTIL_PlayEarSound(pPlayer, SFX_TSD_GBD);
	}

	if (pPlayer->m_iMenu != Menu_ChooseTeam && pPlayer->m_iJoiningState == SHOWTEAMSELECT)
	{
		int slot = MENU_SLOT_TEAM_UNDEFINED;
		if (!Q_stricmp(humans_join_team.string, "T"))
		{
			slot = MENU_SLOT_TEAM_TERRORIST;
		}
		else if (!Q_stricmp(humans_join_team.string, "CT"))
		{
			slot = MENU_SLOT_TEAM_CT;
		}
		else if (!Q_stricmp(humans_join_team.string, "any") && auto_join_team.value != 0.0f)
		{
			slot = MENU_SLOT_TEAM_RANDOM;
		}
		else if (!Q_stricmp(humans_join_team.string, "SPEC") && auto_join_team.value != 0.0f)
		{
			slot = MENU_SLOT_TEAM_SPECT;
		}
		else
		{
			if (allow_spectators.value == 0.0f)
				ShowVGUIMenu(pPlayer, VGUI_Menu_Team, (MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_5), "#Team_Select");
			else
				ShowVGUIMenu(pPlayer, VGUI_Menu_Team, (MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_5 | MENU_KEY_6), "#Team_Select_Spect");
		}

		pPlayer->m_iMenu = Menu_ChooseTeam;
		pPlayer->m_iJoiningState = PICKINGTEAM;

		if (slot != MENU_SLOT_TEAM_UNDEFINED && !pPlayer->IsBot() && !(pPlayer->pev->flags & FL_FAKECLIENT))
		{
			m_bSkipShowMenu = (auto_join_team.value != 0.0f) && !(pPlayer->pev->flags & FL_FAKECLIENT);

			if (HandleMenu_ChooseTeam(pPlayer, slot))
			{
				if (slot != MENU_SLOT_TEAM_SPECT && (IsCareer() || m_bSkipShowMenu))
				{
					// slot 6 - chooses randomize the appearance to model player
					HandleMenu_ChooseAppearance(pPlayer, 6);
				}
			}
			else
			{
				m_bSkipShowMenu = false;
				if (allow_spectators.value == 0.0f)
					ShowVGUIMenu(pPlayer, VGUI_Menu_Team, (MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_5), "#Team_Select");
				else
					ShowVGUIMenu(pPlayer, VGUI_Menu_Team, (MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_5 | MENU_KEY_6), "#Team_Select_Spect");
			}

			m_bSkipShowMenu = false;
		}
	}
}

// Purpose: Player has just spawned. Equip them.
void EXT_FUNC CHalfLifeMultiplay::PlayerSpawn(CBasePlayer *pPlayer)
{
	// This is tied to the joining state (m_iJoiningState).. add it when the joining state is there.
	if (pPlayer->m_bJustConnected)
		return;

	pPlayer->pev->weapons |= (1 << WEAPON_SUIT);
	pPlayer->OnSpawnEquip();
	pPlayer->SetPlayerModel();

	if (respawn_immunitytime.value > 0)
		pPlayer->SetSpawnProtection(respawn_immunitytime.value);
}

BOOL EXT_FUNC CHalfLifeMultiplay::FPlayerCanRespawn(CBasePlayer *pPlayer)
{
	// Player cannot respawn while in the starting menu.
	if (pPlayer->m_iMenu == Menu_ChooseAppearance || pPlayer->m_iJoiningState != JOINED)
	{
		return FALSE;
	}

	// Tabulate the number of players on each team.
	m_iNumCT = CountTeamPlayers(CT);
	m_iNumTerrorist = CountTeamPlayers(TERRORIST);

	if (m_iNumCT <= 0 || m_iNumTerrorist <= 0)
	{
		return TRUE;
	}

	// Player may spawn in freeze phase no matter what.
	if (IsFreezePeriod())
	{
		return TRUE;
	}

	// They cannot respawn without menpower consume, unless it's Freezing phase.
	if (m_rgiMenpowers[pPlayer->m_iTeam] <= 0)
	{
		return FALSE;
	}

	// Player may respawn if their corresponding team leader is alive.
	if ((pPlayer->m_iTeam == CT && THE_COMMANDER.IsValid() && THE_COMMANDER->IsAlive())
		|| (pPlayer->m_iTeam == TERRORIST && THE_GODFATHER.IsValid() && THE_GODFATHER->IsAlive()))
	{
		return TRUE;
	}

	return FALSE;
}

float CHalfLifeMultiplay::FlPlayerSpawnTime(CBasePlayer *pPlayer)
{
	return gpGlobals->time;
}

BOOL CHalfLifeMultiplay::AllowAutoTargetCrosshair()
{
	return FALSE;
}

// How many points awarded to anyone that kills this player?
int CHalfLifeMultiplay::IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled)
{
	if (pKilled == THE_GODFATHER || pKilled == THE_COMMANDER)
		return 10;

	return 1;
}

// Someone/something killed this player
void EXT_FUNC CHalfLifeMultiplay::PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor)
{
	DeathNotice(pVictim, pKiller, pInflictor);

	pVictim->m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
	pVictim->m_iDeaths++;
	pVictim->m_bNotKilled = false;
	pVictim->m_iTrain = (TRAIN_NEW | TRAIN_OFF);
	SET_VIEW(ENT(pVictim->pev), ENT(pVictim->pev));

	CBasePlayer *peKiller = nullptr;
	CBaseEntity *ktmp = CBaseEntity::Instance(pKiller);

	if (ktmp && ktmp->Classify() == CLASS_PLAYER)
	{
		peKiller = static_cast<CBasePlayer *>(ktmp);
	}
	else if (ktmp && ktmp->Classify() == CLASS_VEHICLE)
	{
		CBasePlayer *pDriver = static_cast<CBasePlayer *>(((CFuncVehicle *)ktmp)->m_pDriver);
		if (pDriver)
		{
			pKiller = pDriver->pev;
			peKiller = static_cast<CBasePlayer *>(pDriver);
		}
	}

	if (pVictim == THE_GODFATHER || pVictim == THE_COMMANDER)	// one of two leaders is killed.
	{
		// send the message to everyone.
		UTIL_HudMessageAll(m_TextParam_Notification, "the %s is killed!", g_rgszRoleNames[pVictim->m_iRoleType]);
		UTIL_PrintChatColor(nullptr, pVictim->m_iTeam == CT ? BLUECHAT : REDCHAT, "/tthe %s is killed!", g_rgszRoleNames[pVictim->m_iRoleType]);

		// SFX
		CBasePlayer* pPlayer = nullptr;
		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			pPlayer = UTIL_PlayerByIndex(i);

			if (!pPlayer || FNullEnt(pPlayer) || pPlayer->IsDormant())
				continue;

			if (pPlayer->m_iTeam == SPECTATOR || pPlayer->m_iTeam == UNASSIGNED)	// UNDONE. these two should play the same SFX with whoever they're watching at.
				continue;

			if (pVictim->m_iTeam != pPlayer->m_iTeam)	// mostly, the winner
			{
				CLIENT_COMMAND(pPlayer->edict(), "spk %s\n", SFX_GAME_WON);
				CLIENT_COMMAND(pPlayer->edict(), "mp3 play %s\n", MUSIC_GAME_WON);
			}
			else	// your leader is dead!
			{
				CLIENT_COMMAND(pPlayer->edict(), "spk %s\n", SFX_GAME_LOST);
				CLIENT_COMMAND(pPlayer->edict(), "mp3 play %s\n", MUSIC_GAME_LOST);
			}
		}

		m_iAccountCT += m_rgRewardAccountRules[pVictim->m_iTeam == CT ? RR_LEADER_KILLED : RR_KILLED_ENEMY_LEADER];
		m_iAccountTerrorist += m_rgRewardAccountRules[pVictim->m_iTeam == CT ? RR_KILLED_ENEMY_LEADER : RR_LEADER_KILLED];
	}

	FireTargets("game_playerdie", pVictim, pVictim, USE_TOGGLE, 0);

	// Did the player kill himself?
	if (pVictim->pev == pKiller)
	{
		// Players lose a frag for killing themselves
		pKiller->frags -= 1;
	}
	else if (peKiller && peKiller->IsPlayer())
	{
		// if a player dies in a deathmatch game and the killer is a client, award the killer some points
		CBasePlayer *killer = GetClassPtr((CBasePlayer *)pKiller);
		bool killedByFFA = IsFreeForAll();

		if (killer->m_iTeam == pVictim->m_iTeam && !killedByFFA)	// teamkill handler
		{
			// if a player dies by from teammate
			pKiller->frags -= IPointsForKill(peKiller, pVictim);

			killer->AddAccount(PAYBACK_FOR_KILLED_TEAMMATES, RT_TEAMMATES_KILLED);
			killer->m_iTeamKills++;
			killer->m_bJustKilledTeammate = true;

			ClientPrint(killer->pev, HUD_PRINTCENTER, "#Killed_Teammate");
			ClientPrint(killer->pev, HUD_PRINTCONSOLE, "#Game_teammate_kills", UTIL_dtos1(killer->m_iTeamKills));

			if (autokick.value && max_teamkills.value && killer->m_iTeamKills >= (int)max_teamkills.value)
			{
				ClientPrint(killer->pev, HUD_PRINTCONSOLE, "#Banned_For_Killing_Teammates");

				int iUserID = GETPLAYERUSERID(killer->edict());
				if (iUserID != -1)
				{
					SERVER_COMMAND(UTIL_VarArgs("kick #%d \"For killing too many teammates\"\n", iUserID));
				}
			}

			if (pVictim == THE_GODFATHER || pVictim == THE_COMMANDER)	// TK your leader? WTF?
			{
				ClientPrint(killer->pev, HUD_PRINTCONSOLE, "#Banned_For_Killing_Teammates");

				int iUserID = GETPLAYERUSERID(killer->edict());
				if (iUserID != -1)
				{
					SERVER_COMMAND(UTIL_VarArgs("kick #%d \"For sabotaging the normal gameplay\"\n", iUserID));
				}
			}

			if (!(killer->m_flDisplayHistory & DHF_FRIEND_KILLED))
			{
				killer->m_flDisplayHistory |= DHF_FRIEND_KILLED;
				killer->HintMessage("#Hint_careful_around_teammates");
			}
		}
		else
		{
			// if a player dies in a deathmatch game and the killer is a client, award the killer some points
			pKiller->frags += IPointsForKill(peKiller, pVictim);

			if (pVictim != THE_GODFATHER && pVictim != THE_COMMANDER)
				killer->AddAccount(REWARD_KILLED_ENEMY, RT_ENEMY_KILLED);
			else
				killer->AddAccount(REWARD_KILLED_LEADER, RT_LEADER_KILLED);	// give a bunch of money if you can kill the leader.

			if (!(killer->m_flDisplayHistory & DHF_ENEMY_KILLED))
			{
				killer->m_flDisplayHistory |= DHF_ENEMY_KILLED;
				killer->HintMessage("#Hint_win_round_by_killing_enemy");
			}
		}

		FireTargets("game_playerkill", peKiller, peKiller, USE_TOGGLE, 0);
	}
	else
	{
		// killed by the world
		pKiller->frags -= 1;
	}

	// update the scores
	// killed scores
	MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
		WRITE_BYTE(ENTINDEX(pVictim->edict()));
		WRITE_SHORT(int(pVictim->pev->frags));
		WRITE_SHORT(pVictim->m_iDeaths);
		WRITE_SHORT(0);
		WRITE_SHORT(pVictim->m_iTeam);
	MESSAGE_END();

	// killers score, if it's a player
	if (peKiller && peKiller->IsPlayer())
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
			WRITE_BYTE(ENTINDEX(peKiller->edict()));
			WRITE_SHORT(int(peKiller->pev->frags));
			WRITE_SHORT(peKiller->m_iDeaths);
			WRITE_SHORT(0);
			WRITE_SHORT(peKiller->m_iTeam);
		MESSAGE_END();

		// let the killer paint another decal as soon as he'd like.
		peKiller->m_flNextDecalTime = gpGlobals->time;

		// post event for skill.
		peKiller->OnPlayerKills(pVictim);
	}
}

void EXT_FUNC CHalfLifeMultiplay::DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor)
{
	// by default, the player is killed by the world
	const char *killer_weapon_name = "world";
	int killer_index = 0;

	// Is the killer a client?
	if (pKiller->flags & FL_CLIENT)
	{
		killer_index = ENTINDEX(ENT(pKiller));

		if (pevInflictor)
		{
			if (pevInflictor == pKiller)
			{
				// If the inflictor is the killer, then it must be their current weapon doing the damage
				CBasePlayer *pAttacker = CBasePlayer::Instance(pKiller);
				if (pAttacker && pAttacker->IsPlayer())
				{
					if (pAttacker->m_pActiveItem)
					{
						killer_weapon_name = pAttacker->m_pActiveItem->m_pItemInfo->m_pszClassName;
					}
				}
			}
			else
			{
				// it's just that easy
				killer_weapon_name = STRING(pevInflictor->classname);
			}
		}
	}
	else if (pevInflictor)
	{
		killer_weapon_name = STRING(pevInflictor->classname);
	}

	// strip the monster_* or weapon_* from the inflictor's classname
	const char cut_weapon[] = "weapon_";
	const char cut_monster[] = "monster_";
	const char cut_func[] = "func_";

	// replace the code names with the 'real' names
	if (!Q_strncmp(killer_weapon_name, cut_weapon, sizeof(cut_weapon) - 1))
		killer_weapon_name += sizeof(cut_weapon) - 1;

	else if (!Q_strncmp(killer_weapon_name, cut_monster, sizeof(cut_monster) - 1))
		killer_weapon_name += sizeof(cut_monster) - 1;

	else if (!Q_strncmp(killer_weapon_name, cut_func, sizeof(cut_func) - 1))
		killer_weapon_name += sizeof(cut_func) - 1;

	if (!TheTutor)
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgDeathMsg);
			WRITE_BYTE(killer_index);				// the killer
			WRITE_BYTE(ENTINDEX(pVictim->edict()));	// the victim
			WRITE_BYTE(pVictim->m_bHeadshotKilled);	// is killed headshot
			WRITE_STRING(killer_weapon_name);		// what they were killed by (should this be a string?)
		MESSAGE_END();
	}

	// Did he kill himself?
	if (pVictim->pev == pKiller)
	{
		// killed self
		char *team = GetTeam(pVictim->m_iTeam);
		UTIL_LogPrintf("\"%s<%i><%s><%s>\" committed suicide with \"%s\"\n", STRING(pVictim->pev->netname), GETPLAYERUSERID(pVictim->edict()),
			GETPLAYERAUTHID(pVictim->edict()), team, killer_weapon_name);
	}
	else if (pKiller->flags & FL_CLIENT)
	{
		CBasePlayer *pAttacker = CBasePlayer::Instance(pKiller);

		const char *VictimTeam = GetTeam(pVictim->m_iTeam);
		const char *KillerTeam = (pAttacker && pAttacker->IsPlayer()) ? GetTeam(pAttacker->m_iTeam) : "";

		UTIL_LogPrintf("\"%s<%i><%s><%s>\" killed \"%s<%i><%s><%s>\" with \"%s\"\n", STRING(pKiller->netname), GETPLAYERUSERID(ENT(pKiller)), GETPLAYERAUTHID(ENT(pKiller)),
			KillerTeam, STRING(pVictim->pev->netname), GETPLAYERUSERID(pVictim->edict()), GETPLAYERAUTHID(pVictim->edict()), VictimTeam, killer_weapon_name);
	}
	else
	{
		// killed by the world
		char *team = GetTeam(pVictim->m_iTeam);
		UTIL_LogPrintf("\"%s<%i><%s><%s>\" committed suicide with \"%s\" (world)\n", STRING(pVictim->pev->netname), GETPLAYERUSERID(pVictim->edict()),
			GETPLAYERAUTHID(pVictim->edict()), team, killer_weapon_name);
	}

	// TODO: It is called in CBasePlayer::Killed too, most likely,
	// an unnecessary call. (Need investigate)
	CheckWinConditions();

	MESSAGE_BEGIN(MSG_SPEC, SVC_DIRECTOR);
		WRITE_BYTE(9);								// command length in bytes
		WRITE_BYTE(DRC_CMD_EVENT);					// player killed
		WRITE_SHORT(ENTINDEX(pVictim->edict()));	// index number of primary entity

	if (pevInflictor)
		WRITE_SHORT(ENTINDEX(ENT(pevInflictor)));	// index number of secondary entity
	else
		WRITE_SHORT(ENTINDEX(ENT(pKiller)));		// index number of secondary entity

	if (pVictim->m_bHeadshotKilled)
		WRITE_LONG(9 | DRC_FLAG_DRAMATIC | DRC_FLAG_SLOWMOTION);
	else
		WRITE_LONG(7 | DRC_FLAG_DRAMATIC);			// eventflags (priority and flags)

	MESSAGE_END();
}

// Player has grabbed a weapon that was sitting in the world
void CHalfLifeMultiplay::PlayerGotWeapon(CBasePlayer *pPlayer, CBaseWeapon *pWeapon)
{
}

bool CHalfLifeMultiplay::CanHaveAmmo(CBasePlayer* pPlayer, AmmoIdType iId)
{
	return !!(pPlayer->m_rgAmmo[iId] < GetAmmoInfo(iId)->m_iMax);
}

// Returns FALSE if the player is not allowed to pick up this weapon
bool CHalfLifeMultiplay::CanHavePlayerItem(CBasePlayer* pPlayer, WeaponIdType iId, bool bPrintHint)
{
	// only living players can have items
	if (pPlayer->pev->deadflag != DEAD_NO)
	{
		return false;
	}

	if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][iId] == WPN_F && iId != WEAPON_KNIFE)	// you should never forbid a player from getting his knife.
	{
		if (g_bClientPrintEnable && bPrintHint)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Cannot_Buy_This");
			UTIL_PrintChatColor(pPlayer, REDCHAT, "/yYou are unqualified to use /t%s/y since you are /g%s/y.", g_rgszWeaponAlias[iId], g_rgszRoleNames[pPlayer->m_iRoleType]);
		}

		return false;
	}

	if (pPlayer->IsBot() && TheCSBots() && !TheCSBots()->IsWeaponUseable(iId))
	{
		return false;
	}

	if (pPlayer->HasShield() && iId == WEAPON_P99)
		return false;

	if (pPlayer->HasShield() && iId == WEAPON_SHIELDGUN)
		return false;

	if (pPlayer->m_rgpPlayerItems[PISTOL_SLOT] && pPlayer->m_rgpPlayerItems[PISTOL_SLOT]->m_iId == WEAPON_P99 && iId == WEAPON_SHIELDGUN)
		return false;

	if (pPlayer->m_rgpPlayerItems[PRIMARY_WEAPON_SLOT] && pPlayer->m_rgpPlayerItems[PRIMARY_WEAPON_SLOT]->m_iId == iId)
	{
		if (g_bClientPrintEnable && bPrintHint)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Cstrike_Already_Own_Weapon");
		}

		return false;
	}

	if (pPlayer->m_rgpPlayerItems[PISTOL_SLOT] && pPlayer->m_rgpPlayerItems[PISTOL_SLOT]->m_iId == iId)
	{
		if (g_bClientPrintEnable && bPrintHint)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Cstrike_Already_Own_Weapon");
		}

		return false;
	}

	if (g_rgItemInfo[iId].m_iAmmoType > 0)
	{
		if (!CanHaveAmmo(pPlayer, g_rgItemInfo[iId].m_iAmmoType))
		{
			// we can't carry anymore ammo for this gun. We can only
			// have the gun if we aren't already carrying one of this type
			if (pPlayer->HasPlayerItem(iId))
			{
				return false;
			}
		}
	}
	else
	{
		// weapon doesn't use ammo, don't take another if you already have it.
		if (pPlayer->HasPlayerItem(iId))
		{
			return FALSE;
		}
	}

	// note: will fall through to here if GetItemInfo doesn't fill the struct!
	return TRUE;
}

BOOL CHalfLifeMultiplay::CanHaveItem(CBasePlayer *pPlayer, CItem *pItem)
{
	return TRUE;
}

void CHalfLifeMultiplay::PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem)
{
	;
}

int CHalfLifeMultiplay::ItemShouldRespawn(CItem *pItem)
{
	if (pItem->pev->spawnflags & SF_NORESPAWN)
	{
		return GR_ITEM_RESPAWN_NO;
	}

	return GR_ITEM_RESPAWN_YES;
}

// At what time in the future may this Item respawn?
float CHalfLifeMultiplay::FlItemRespawnTime(CItem *pItem)
{
	return gpGlobals->time + ITEM_RESPAWN_TIME;
}

// Where should this item respawn?
// Some game variations may choose to randomize spawn locations
Vector CHalfLifeMultiplay::VecItemRespawnSpot(CItem *pItem)
{
	return pItem->pev->origin;
}

void CHalfLifeMultiplay::PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, int iCount)
{
	;
}

BOOL CHalfLifeMultiplay::IsAllowedToSpawn(CBaseEntity *pEntity)
{
	return TRUE;
}

float CHalfLifeMultiplay::FlHealthChargerRechargeTime()
{
	return 60;
}

float CHalfLifeMultiplay::FlHEVChargerRechargeTime()
{
	return 30;
}

int EXT_FUNC CHalfLifeMultiplay::DeadPlayerWeapons(CBasePlayer *pPlayer)
{
	return GR_PLR_DROP_GUN_ACTIVE;
}

int CHalfLifeMultiplay::DeadPlayerAmmo(CBasePlayer *pPlayer)
{
	return GR_PLR_DROP_AMMO_ACTIVE;
}

edict_t *EXT_FUNC CHalfLifeMultiplay::GetPlayerSpawnSpot(CBasePlayer *pPlayer)
{
	// gat valid spawn point
	edict_t* pentSpawnSpot = pPlayer->EntSelectSpawnPoint();

	// Move the player to the place it said.
#ifndef PLAY_GAMEDLL
	pPlayer->pev->origin = pentSpawnSpot->v.origin + Vector(0, 0, 1);
#else
	// TODO: fix test demo
	pPlayer->pev->origin = pentSpawnSpot->v.origin;
	pPlayer->pev->origin.z += 1;
#endif

	pPlayer->pev->v_angle = g_vecZero;
	pPlayer->pev->velocity = g_vecZero;
	pPlayer->pev->angles = pentSpawnSpot->v.angles;
	pPlayer->pev->punchangle = g_vecZero;
	pPlayer->pev->fixangle = 1;

	if (IsMultiplayer())
	{
		if (pentSpawnSpot->v.target)
		{
			FireTargets(STRING(pentSpawnSpot->v.target), pPlayer, pPlayer, USE_TOGGLE, 0);
		}
	}

	// redeployment of Doctrine_MobileWarfare
	if (!IsFreezePeriod() && m_rgTeamTacticalScheme[pPlayer->m_iTeam] == Doctrine_MobileWarfare
		&& ( (pPlayer->m_iTeam == CT && THE_COMMANDER.IsValid()) || (pPlayer->m_iTeam == TERRORIST && THE_GODFATHER.IsValid()) ))
	{
		CBasePlayer* pLeader = THE_COMMANDER;
		if (pPlayer->m_iTeam == TERRORIST)
			pLeader = THE_GODFATHER;

		Vector vecCandidates[8], vecDest;
		bool bFind = false;

		for (int i = 0; i < 8; i++)
			vecCandidates[i] = pLeader->pev->origin;

		vecCandidates[0] += Vector(0,		128,	0);
		vecCandidates[1] += Vector(128,		128,	0);
		vecCandidates[2] += Vector(128,		0,		0);
		vecCandidates[3] += Vector(128,		-128,	0);
		vecCandidates[4] += Vector(0,		-128,	0);
		vecCandidates[5] += Vector(-128,	-128,	0);
		vecCandidates[6] += Vector(-128,	0,		0);
		vecCandidates[7] += Vector(-128,	128,	0);

		TraceResult tr[8];
		for (int i = 0; i < 8; i++)
		{
			TRACE_HULL(pLeader->pev->origin, vecCandidates[i], dont_ignore_monsters, head_hull, pLeader->edict(), &tr[i]);

			if (tr[i].flFraction >= 1.0 && UTIL_CheckPassibility(vecCandidates[i]))
			{
				bFind = true;
				vecDest = vecCandidates[i];
				break;
			}

			if (UTIL_CheckPassibility(tr[i].vecEndPos))
			{
				bFind = true;
				vecDest = tr[i].vecEndPos;
				break;
			}
		}

		if (bFind)
		{
			// make sure your minion landed on ground properly.
			Vector vecGroundPos = vecDest - Vector(0, 0, 9999);
			TRACE_HULL(vecDest, vecGroundPos, dont_ignore_monsters, head_hull, pLeader->edict(), &tr[0]);
			vecDest = tr[0].vecEndPos;

			pPlayer->pev->flags |= FL_DUCKING;
			SET_SIZE(pPlayer->edict(), Vector(-16.0f, -16.0f, -18.0f), Vector(16.0f, 16.0f, 32.0f));
			pPlayer->pev->view_ofs = Vector(0, 0, 12);
			pPlayer->pev->origin = vecDest;
		}
	}

	return pentSpawnSpot;
}

int CHalfLifeMultiplay::PlayerRelationship(CBasePlayer *pPlayer, CBaseEntity *pTarget)
{
	if (IsFreeForAll())
	{
		return GR_NOTTEAMMATE;
	}

	if (!pPlayer || !pTarget)
	{
		return GR_NOTTEAMMATE;
	}

	if (!pTarget->IsPlayer())
	{
		return GR_NOTTEAMMATE;
	}

	if (pPlayer->m_iTeam != static_cast<CBasePlayer *>(pTarget)->m_iTeam)
	{
		return GR_NOTTEAMMATE;
	}

	return GR_TEAMMATE;
}

BOOL CHalfLifeMultiplay::FAllowFlashlight()
{
	return flashlight.value ? TRUE : FALSE;
}

BOOL CHalfLifeMultiplay::FAllowMonsters()
{
	return FALSE;
}

void EXT_FUNC CHalfLifeMultiplay::GoToIntermission()
{
	if (m_bGameOver)
	{
		// intermission has already been triggered, so ignore.
		return;
	}

	UTIL_LogPrintf("Team \"CT\" scored \"%i\" with \"%i\" players\n", m_iNumCTWins, m_iNumCT);
	UTIL_LogPrintf("Team \"TERRORIST\" scored \"%i\" with \"%i\" players\n", m_iNumTerroristWins, m_iNumTerrorist);

	if (IsCareer())
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgCZCareer);
			WRITE_STRING("MATCH");
			WRITE_LONG(m_iNumCTWins);
			WRITE_LONG(m_iNumTerroristWins);
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ALL, gmsgCZCareerHUD);
			WRITE_STRING("MATCH");
			WRITE_LONG(m_iNumCTWins);
			WRITE_LONG(m_iNumTerroristWins);
			WRITE_BYTE(m_iCareerMatchWins);
			WRITE_BYTE(m_iRoundWinDifference);
			WRITE_BYTE(m_iRoundWinStatus);
		MESSAGE_END();

		if (TheCareerTasks)
		{
			UTIL_LogPrintf("Career Match %d %d %d %d\n", m_iRoundWinStatus, m_iNumCTWins, m_iNumTerroristWins, TheCareerTasks->AreAllTasksComplete());
		}
	}

	MESSAGE_BEGIN(MSG_ALL, SVC_INTERMISSION);
	MESSAGE_END();

	if (IsCareer())
	{
		SERVER_COMMAND("setpause\n");
	}

	// bounds check
	int time = int(CVAR_GET_FLOAT("mp_chattime"));
	if (time < 1)
		CVAR_SET_STRING("mp_chattime", "1");
	else if (time > MAX_INTERMISSION_TIME)
		CVAR_SET_STRING("mp_chattime", UTIL_dtos1(MAX_INTERMISSION_TIME));

	m_flIntermissionEndTime = gpGlobals->time + int(mp_chattime.value);
	m_flIntermissionStartTime = gpGlobals->time;

	m_bGameOver = true;
	m_iEndIntermissionButtonHit = FALSE;
	m_iSpawnPointCount_Terrorist = 0;
	m_iSpawnPointCount_CT = 0;
	m_bLevelInitialized = false;
}

// Clean up memory used by mapcycle when switching it
void DestroyMapCycle(mapcycle_t *cycle)
{
	mapcycle_item_t *p, *n, *start;
	p = cycle->items;

	if (p)
	{
		start = p;
		p = p->next;
		while (p != start)
		{
			n = p->next;
			delete p;
			p = n;
		}

		delete cycle->items;
	}

	cycle->items = nullptr;
	cycle->next_item = nullptr;
}

// Parses mapcycle.txt file into mapcycle_t structure
int ReloadMapCycleFile(char *filename, mapcycle_t *cycle)
{
	char szBuffer[MAX_RULE_BUFFER];
	char szMap[MAX_MAPNAME_LENGHT];
	int length;
	char *pToken;
	char *pFileList;
	char *aFileList = pFileList = (char *)LOAD_FILE_FOR_ME(filename, &length);
	bool hasBuffer;
	mapcycle_item_s *item, *newlist = nullptr, *next;

	if (pFileList && length)
	{
		// the first map name in the file becomes the default
		while (true)
		{
			hasBuffer = false;
			Q_memset(szBuffer, 0, sizeof(szBuffer));

			pFileList = SharedParse(pFileList);
			pToken = SharedGetToken();

			if (Q_strlen(pToken) <= 0)
				break;

			Q_strcpy(szMap, pToken);

			// Any more tokens on this line?
			if (SharedTokenWaiting(pFileList))
			{
				pFileList = SharedParse(pFileList);
				if (Q_strlen(pToken) > 0)
				{
					hasBuffer = true;
					Q_strcpy(szBuffer, pToken);
				}
			}

			// Check map
			if (IS_MAP_VALID(szMap))
			{
				// Create entry
				char *s;

				item = new mapcycle_item_s;

				Q_strcpy(item->mapname, szMap);

				item->minplayers = 0;
				item->maxplayers = 0;

				Q_memset(item->rulebuffer, 0, sizeof(item->rulebuffer));

				if (hasBuffer)
				{
					s = GET_KEY_VALUE(szBuffer, "minplayers");

					if (s && s[0] != '\0')
					{
						item->minplayers = Q_atoi(s);
						item->minplayers = Q_max(item->minplayers, 0);
						item->minplayers = Q_min(item->minplayers, gpGlobals->maxClients);
					}

					s = GET_KEY_VALUE(szBuffer, "maxplayers");
					if (s && s[0] != '\0')
					{
						item->maxplayers = Q_atoi(s);
						item->maxplayers = Q_max(item->maxplayers, 0);
						item->maxplayers = Q_min(item->maxplayers, gpGlobals->maxClients);
					}

					// Remove keys
					REMOVE_KEY_VALUE(szBuffer, "minplayers");
					REMOVE_KEY_VALUE(szBuffer, "maxplayers");

					Q_strcpy(item->rulebuffer, szBuffer);
				}

				item->next = cycle->items;
				cycle->items = item;
			}
			else
			{
				ALERT(at_console, "Skipping %s from mapcycle, not a valid map\n", szMap);
			}
		}

		FREE_FILE(aFileList);
	}

	// Fixup circular list pointer
	item = cycle->items;

	// Reverse it to get original order
	while (item)
	{
		next = item->next;
		item->next = newlist;
		newlist = item;
		item = next;
	}

	cycle->items = newlist;
	item = cycle->items;

	// Didn't parse anything
	if (!item)
	{
		return 0;
	}

	while (item->next)
	{
		item = item->next;
	}

	item->next = cycle->items;
	cycle->next_item = item->next;

	return 1;
}

// Determine the current # of active players on the server for map cycling logic
int CountPlayers()
{
	int nCount = 0;
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);
		if (pPlayer)
		{
			nCount++;
		}
	}

	return nCount;
}

// Parse commands/key value pairs to issue right after map xxx command is issued on server level transition
void ExtractCommandString(char *s, char *szCommand)
{
	// Now make rules happen
	char pkey[512];
	char value[512]; // use two buffers so compares

	// work without stomping on each other
	char *c;
	int nCount;

	while (*s)
	{
		if (*s == '\\')
		{
			// skip the slash
			s++;
		}

		// Copy a key
		c = pkey;
		nCount = 0;

		while (*s != '\\')
		{
			if (!*s)
			{
				// allow value to be ended with NULL
				break;
			}

			if (nCount >= sizeof(pkey))
			{
				s++;
				// skip oversized key chars till the slash or EOL
				continue;
			}

			*c++ = *s++;
			nCount++;
		}

		*c = '\0';
		s++;	// skip the slash

		// Copy a value
		c = value;
		nCount = 0;

		while (*s != '\\')
		{
			if (!*s)
			{
				// allow value to be ended with NULL
				break;
			}

			if (nCount >= sizeof(value))
			{
				s++;
				// skip oversized value chars till the slash or EOL
				continue;
			}

			*c++ = *s++;
			nCount++;
		}

		*c = '\0';

		Q_strcat(szCommand, pkey);
		if (Q_strlen(value) > 0)
		{
			Q_strcat(szCommand, " ");
			Q_strcat(szCommand, value);
		}
		Q_strcat(szCommand, "\n");

		/*if (!*s)
		{
			return;
		}

		s++;*/
	}
}

void CHalfLifeMultiplay::ResetAllMapVotes()
{
	CBaseEntity *pEntity = nullptr;

	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
	{
		if (FNullEnt(pEntity->edict()))
			break;

		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);
		if (pPlayer->m_iTeam != UNASSIGNED)
		{
			pPlayer->m_iMapVote = 0;
		}
	}

	for (int j = 0; j < MAX_VOTE_MAPS; j++)
		m_iMapVotes[j] = 0;
}

int GetMapCount()
{
	static mapcycle_t mapcycle;
	char *mapcfile = (char *)CVAR_GET_STRING("mapcyclefile");

	DestroyMapCycle(&mapcycle);
	ReloadMapCycleFile(mapcfile, &mapcycle);

	int nCount = 0;
	auto item = mapcycle.next_item;

	do
	{
		if (!item)
			break;

		nCount++;
		item = item->next;
	} while (item != mapcycle.next_item);

	return nCount;
}

void CHalfLifeMultiplay::DisplayMaps(CBasePlayer *pPlayer, int iVote)
{
	static mapcycle_t mapcycle2;
	char *mapcfile = (char *)CVAR_GET_STRING("mapcyclefile");
	char *pszNewMap = nullptr;

	int iCount = 0, done = 0;

	DestroyMapCycle(&mapcycle2);
	ReloadMapCycleFile(mapcfile, &mapcycle2);

	mapcycle_item_s *item = mapcycle2.next_item;
	while (!done && item)
	{
		if (item->next == mapcycle2.next_item)
			done = 1;

		iCount++;

		if (pPlayer)
		{
			if (m_iMapVotes[iCount] == 1)
			{
				ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Vote", UTIL_dtos1(iCount), item->mapname, UTIL_dtos2(1));
			}
			else
			{
				ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Votes", UTIL_dtos1(iCount), item->mapname, UTIL_dtos2(m_iMapVotes[iCount]));
			}
		}

		if (iCount == iVote)
		{
			pszNewMap = item->mapname;
		}

		item = item->next;
	}

	if (!pszNewMap || !iVote)
	{
		return;
	}

	if (Q_strcmp(pszNewMap, STRING(gpGlobals->mapname)) != 0)
	{
		CHANGE_LEVEL(pszNewMap, nullptr);
		return;
	}

	if (timelimit.value)
	{
		timelimit.value += 30;
		UTIL_ClientPrintAll(HUD_PRINTCENTER, "#Map_Vote_Extend");
	}

	ResetAllMapVotes();
}

void CHalfLifeMultiplay::ProcessMapVote(CBasePlayer *pPlayer, int iVote)
{
	CBaseEntity *pEntity = nullptr;
	int iValidVotes = 0, iNumPlayers = 0;

	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
	{
		if (FNullEnt(pEntity->edict()))
			break;

		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);

		if (pPlayer->m_iTeam != UNASSIGNED)
		{
			iNumPlayers++;

			if (pPlayer->m_iMapVote == iVote)
				iValidVotes++;
		}
	}

	m_iMapVotes[iVote] = iValidVotes;

	float ratio = mapvoteratio.value;
	if (mapvoteratio.value > 1)
	{
		ratio = 1;
		CVAR_SET_STRING("mp_mapvoteratio", "1.0");
	}
	else if (mapvoteratio.value < 0.35f)
	{
		ratio = 0.35f;
		CVAR_SET_STRING("mp_mapvoteratio", "0.35");
	}

	int iRequiredVotes = 2;
	if (iNumPlayers > 2)
	{
		iRequiredVotes = int(iNumPlayers * ratio + 0.5f);
	}

	if (iValidVotes < iRequiredVotes)
	{
		DisplayMaps(pPlayer, 0);
		ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Game_required_votes", UTIL_dtos1(iRequiredVotes));
	}
	else
	{
		DisplayMaps(nullptr, iVote);
	}
}

// Server is changing to a new level, check mapcycle.txt for map name and setup info
void EXT_FUNC CHalfLifeMultiplay::ChangeLevel()
{
	static char szPreviousMapCycleFile[256];
	static mapcycle_t mapcycle;

	char szNextMap[MAX_MAPNAME_LENGHT];
	char szFirstMapInList[MAX_MAPNAME_LENGHT];
	char szCommands[1500];
	char szRules[1500];
	int minplayers = 0, maxplayers = 0;

	// the absolute default level is de_dust
	Q_strcpy(szFirstMapInList, "de_dust");

	int curplayers;
	bool do_cycle = true;

	// find the map to change to
	char *mapcfile = (char *)CVAR_GET_STRING("mapcyclefile");
	assert(mapcfile != nullptr);

	szCommands[0] = '\0';
	szRules[0] = '\0';

	curplayers = CountPlayers();

	// Has the map cycle filename changed?
	if (Q_stricmp(mapcfile, szPreviousMapCycleFile) != 0)
	{
		Q_strcpy(szPreviousMapCycleFile, mapcfile);

		DestroyMapCycle(&mapcycle);

		if (!ReloadMapCycleFile(mapcfile, &mapcycle) || !mapcycle.items)
		{
			ALERT(at_console, "Unable to load map cycle file %s\n", mapcfile);
			do_cycle = false;
		}
	}

	if (do_cycle && mapcycle.items)
	{
		bool keeplooking = false;
		bool found = false;
		mapcycle_item_s *item;

		// Assume current map
		Q_strcpy(szNextMap, STRING(gpGlobals->mapname));
		Q_strcpy(szFirstMapInList, STRING(gpGlobals->mapname));

		// Traverse list
		for (item = mapcycle.next_item; item->next != mapcycle.next_item; item = item->next)
		{
			keeplooking = false;

			assert(item != nullptr);

			if (item->minplayers != 0)
			{
				if (curplayers >= item->minplayers)
				{
					found = true;
					minplayers = item->minplayers;
				}
				else
				{
					keeplooking = true;
				}
			}

			if (item->maxplayers != 0)
			{
				if (curplayers <= item->maxplayers)
				{
					found = true;
					maxplayers = item->maxplayers;
				}
				else
				{
					keeplooking = true;
				}
			}

			if (keeplooking)
			{
				continue;
			}

			found = true;
			break;
		}

		if (!found)
		{
			item = mapcycle.next_item;
		}

		// Increment next item pointer
		mapcycle.next_item = item->next;

		// Perform logic on current item
		Q_strcpy(szNextMap, item->mapname);
		ExtractCommandString(item->rulebuffer, szCommands);
		Q_strcpy(szRules, item->rulebuffer);
	}

	if (!IS_MAP_VALID(szNextMap))
	{
		Q_strcpy(szNextMap, szFirstMapInList);
	}

	m_bGameOver = true;

	ALERT(at_console, "CHANGE LEVEL: %s\n", szNextMap);
	if (minplayers || maxplayers)
	{
		ALERT(at_console, "PLAYER COUNT:  min %i max %i current %i\n", minplayers, maxplayers, curplayers);
	}

	if (Q_strlen(szRules) > 0)
	{
		ALERT(at_console, "RULES:  %s\n", szRules);
	}

	CHANGE_LEVEL(szNextMap, nullptr);
	if (Q_strlen(szCommands) > 0)
	{
		SERVER_COMMAND(szCommands);
	}
}

void CHalfLifeMultiplay::SendMOTDToClient(edict_t *client)
{
	// read from the MOTD.txt file
	int length, char_count = 0;
	char *pFileList;
	char *aFileList = pFileList = (char *)LOAD_FILE_FOR_ME((char *)CVAR_GET_STRING("motdfile"), &length);

	// send the server name
	MESSAGE_BEGIN(MSG_ONE, gmsgServerName, nullptr, client);
		WRITE_STRING(CVAR_GET_STRING("hostname"));
	MESSAGE_END();

	// Send the message of the day
	// read it chunk-by-chunk, and send it in parts
	while (pFileList && *pFileList && char_count < MAX_MOTD_LENGTH)
	{
		char chunk[MAX_MOTD_CHUNK + 1];

		if (Q_strlen(pFileList) < sizeof(chunk))
		{
			Q_strcpy(chunk, pFileList);
		}
		else
		{
			Q_strncpy(chunk, pFileList, sizeof(chunk) - 1);
			// Q_strncpy doesn't always append the null terminator
			chunk[sizeof(chunk) - 1] = '\0';
		}

		char_count += Q_strlen(chunk);

		if (char_count < MAX_MOTD_LENGTH)
			pFileList = aFileList + char_count;
		else
			*pFileList = '\0';

		MESSAGE_BEGIN(MSG_ONE, gmsgMOTD, nullptr, client);
			WRITE_BYTE((*pFileList != '\0') ? FALSE : TRUE); // FALSE means there is still more message to come
			WRITE_STRING(chunk);
		MESSAGE_END();
	}

	FREE_FILE(aFileList);
}

void EXT_FUNC CHalfLifeMultiplay::ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer)
{
	pPlayer->SetPlayerModel();
	pPlayer->SetPrefsFromUserinfo(infobuffer);
}

void CHalfLifeMultiplay::ServerActivate()
{
	// Check to see if there's a mapping info paramater entity
	if (g_pMapInfo)
		g_pMapInfo->CheckMapInfo();

	ReadMultiplayCvars();
	CheckMapConditions();

	CVAR_SET_FLOAT("sv_maxvelocity", 99999.0);	// stupit limitation.
	CVAR_SET_FLOAT("sv_maxspeed", 9999.0);
}

TeamName CHalfLifeMultiplay::SelectDefaultTeam()
{
	TeamName team = UNASSIGNED;
	if (m_iNumTerrorist < m_iNumCT)
	{
		team = TERRORIST;
	}
	else if (m_iNumTerrorist > m_iNumCT)
	{
		team = CT;
	}
	// Choose the team that's losing
	else if (m_iNumTerroristWins < m_iNumCTWins)
	{
		team = TERRORIST;
	}
	else if (m_iNumCTWins < m_iNumTerroristWins)
	{
		team = CT;
	}
	else
	{
		// Teams and scores are equal, pick a random team
		team = (RANDOM_LONG(0, 1) == 0) ? CT : TERRORIST;
	}

	if (TeamFull(team))
	{
		// Pick the opposite team
		team = (team == TERRORIST) ? CT : TERRORIST;

		// No choices left
		if (TeamFull(team))
		{
			return UNASSIGNED;
		}
	}

	return team;
}

void CHalfLifeMultiplay::ChangePlayerTeam(CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib)
{
	if (!pTeamName || !pTeamName[0])
		return;

	if (!pPlayer->IsAlive() || pPlayer->m_iJoiningState != JOINED)
		return;

	TeamName newTeam;
	if (!Q_stricmp(pTeamName, "CT"))
	{
		newTeam = CT;
	}
	else if (!Q_stricmp(pTeamName, "TERRORIST"))
	{
		newTeam = TERRORIST;
	}
	else if (!Q_stricmp(pTeamName, "SPECTATOR"))
	{
		newTeam = SPECTATOR;
	}
	else
	{
		return;
	}

	if (pPlayer->m_iTeam != UNASSIGNED && pPlayer->m_iTeam != newTeam)
	{
		if (bKill)
		{
			pPlayer->m_LastHitGroup = HITGROUP_GENERIC;

			// have the player kill themself
			pPlayer->pev->health = 0;
			pPlayer->Killed(pPlayer->pev, bGib ? GIB_ALWAYS : GIB_NEVER);

			// add 1 to frags to balance out the 1 subtracted for killing yourself
			pPlayer->pev->frags++;
		}

		pPlayer->m_iTeam = newTeam;
		pPlayer->SetPlayerModel();
		pPlayer->TeamChangeUpdate();

		CSGameRules()->CheckWinConditions();
	}
}

bool CHalfLifeMultiplay::CanPlayerBuy(CBasePlayer *pPlayer) const
{
	if (pPlayer->m_iTeam == CT && m_bCTCantBuy)
	{
		return false;
	}
	else if (pPlayer->m_iTeam == TERRORIST && m_bTCantBuy)
	{
		return false;
	}
	else if (m_bCTCantBuy && m_bTCantBuy)
	{
		return false;
	}

	return true;
}

CBasePlayer* CHalfLifeMultiplay::RandomNonroleCharacter(TeamName iTeam)
{
	int iCandidateCount = 0;
	CBasePlayer* rgpCandidates[33];

	CBasePlayer* pCandidate = nullptr;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		pCandidate = UTIL_PlayerByIndex(i);

		if (!pCandidate || FNullEnt(pCandidate) || pCandidate->IsDormant())
			continue;

		if (human_role_priority.value > 0.0 && pCandidate->IsBot())	// exclude bots if cvar says so.
			continue;

		if (pCandidate->m_iTeam != iTeam)
			continue;

		if (pCandidate->m_iRoleType != Role_UNASSIGNED)
			continue;

		iCandidateCount++;
		rgpCandidates[iCandidateCount] = pCandidate;
	}

	if (!iCandidateCount)	// include bots this time.
	{
		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			pCandidate = UTIL_PlayerByIndex(i);

			if (!pCandidate || FNullEnt(pCandidate) || pCandidate->IsDormant())
				continue;

			if (pCandidate->m_iTeam != iTeam)
				continue;

			if (pCandidate->m_iRoleType != Role_UNASSIGNED)
				continue;

			iCandidateCount++;
			rgpCandidates[iCandidateCount] = pCandidate;
		}
	}

	if (iCandidateCount > 0)
		return rgpCandidates[RANDOM_LONG(1, iCandidateCount)];

	return nullptr;	// no found.
}

void CHalfLifeMultiplay::CheckMenpower(TeamName iTeam)
{
	if (m_rgiMenpowers[iTeam] <= 0 && !m_rgbMenpowerBroadcast[iTeam])	// broadcast it if we didn't
	{
		UTIL_HudMessageAll(m_TextParam_Notification, "The menpower of %s is depleted!", iTeam == CT ? "CT" : "TERRORIST");
		UTIL_PlayEarSound(nullptr, SFX_MENPOWER_DEPLETED);

		m_rgbMenpowerBroadcast[iTeam] = true;
	}
}

int CHalfLifeMultiplay::IDamageMoney(CBasePlayer* pVictim, CBasePlayer* pAttacker, float flDamage)
{
	if (FNullEnt(pAttacker) || FNullEnt(pVictim))
		return FALSE;

	if (flDamage <= 0)
		return FALSE;

	if (pVictim->m_iTeam == pAttacker->m_iTeam)
		return int(flDamage * -3.0f);	// penalty of hurting teammate.

	return int(flDamage);
}

bool CHalfLifeMultiplay::HasRoleOccupied(RoleTypes iRole, TeamName iTeam)
{
	CBasePlayer* pPlayer = nullptr;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		pPlayer = UTIL_PlayerByIndex(i);

		if (!pPlayer || FNullEnt(pPlayer) || pPlayer->IsDormant())
			continue;

		if (pPlayer->m_iRoleType == iRole && pPlayer->m_iTeam == iTeam)
		{
			return true;
		}
	}

	return false;
}

TacticalSchemes CHalfLifeMultiplay::CalcTSVoteResult(TeamName iTeam)
{
	int rgiBallotBoxes[SCHEMES_COUNT] = { 0, 0, 0, 0, 0 };
	GetTSVoteDetail(iTeam, rgiBallotBoxes);

	TacticalSchemes iWinner = Scheme_UNASSIGNED;
	int iWinnerVotes = 0;
	for (int i = 0; i < SCHEMES_COUNT; i++)
	{
		if (rgiBallotBoxes[i] > iWinnerVotes)
		{
			iWinner = (TacticalSchemes)i;
			iWinnerVotes = rgiBallotBoxes[i];
		}
		else if (rgiBallotBoxes[i] == iWinnerVotes && iWinnerVotes > 0)
		{
			return Scheme_UNASSIGNED;	// a disputation occurs.
		}
	}

	return iWinner;
}

void CHalfLifeMultiplay::GetTSVoteDetail(TeamName iTeam, int* rgiBallotBoxes)
{
	CBasePlayer* pPlayer = nullptr;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		pPlayer = UTIL_PlayerByIndex(i);

		if (!pPlayer || FNullEnt(pPlayer) || pPlayer->IsDormant())
			continue;

		// UNDONE: maybe bots can think & vote someday?
		if (pPlayer->IsBot())
			continue;

		if (pPlayer->m_iTeam == iTeam)
		{
			rgiBallotBoxes[pPlayer->m_iVotedTS]++;
		}
	}
}

void CHalfLifeMultiplay::GiveDefaultItems(CBasePlayer* pPlayer)
{
	pPlayer->RemoveAllItems(FALSE);

	pPlayer->AddPlayerItem(CBaseWeapon::Give(WEAPON_USP, pPlayer));
	pPlayer->GiveAmmo(24, AMMO_45acp);

	if (m_rgTeamTacticalScheme[pPlayer->m_iTeam] != Doctrine_GrandBattleplan)
		return;

	// bonus equipments for Doctrine_GrandBattleplan
	pPlayer->GiveNamedItem("item_kevlar");	// vest without helmet
	pPlayer->m_bHasNightVision = true;	// NVG
	pPlayer->SendItemStatus();

	if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][WEAPON_HEGRENADE] != WPN_F)
		pPlayer->GiveAmmo(1, AMMO_HEGrenade);

	if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][WEAPON_FLASHBANG] != WPN_F)
		pPlayer->GiveAmmo(1, AMMO_Flashbang);

	if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][WEAPON_SMOKEGRENADE] != WPN_F)
		pPlayer->GiveAmmo(1, AMMO_SmokeGrenade);
}

float CHalfLifeMultiplay::PlayerMaxArmour(CBasePlayer* pPlayer)
{
	if (pPlayer->m_iRoleType == Role_SWAT)
		return swat_max_armour.value;

	return MAX_NORMAL_BATTERY;
}

CBaseWeapon* CHalfLifeMultiplay::SelectProperGrenade(CBasePlayer* pPlayer)
{
	return pPlayer->m_rgpPlayerItems[GRENADE_SLOT];

	// WPN_UNDONE
	/*if (FNullEnt(pPlayer->m_rgpPlayerItems[GRENADE_SLOT]))
		return nullptr;

	char* pGrenadeName = nullptr;

	switch (pPlayer->m_iRoleType)
	{
	case Role_Sharpshooter:
	case Role_Breacher:
	case Role_Arsonist:
		pGrenadeName = "weapon_hegrenade";
		break;

	case Role_Medic:
	case Role_MadScientist:
		pGrenadeName = "weapon_smokegrenade";
		break;

	default:
		pGrenadeName = "weapon_flashbang";
		break;
	}

	CBaseEntity* pEntity = nullptr;
	CBasePlayerWeapon* pWeapon = nullptr;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, pGrenadeName)))
	{
		if (FNullEnt(pEntity))
			continue;

		pWeapon = (CBasePlayerWeapon*)pEntity;
		if (pWeapon->m_pPlayer != pPlayer)
			continue;

		return pWeapon;
	}

	return nullptr;*/
}
