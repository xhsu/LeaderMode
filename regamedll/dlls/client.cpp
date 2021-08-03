#include "precompiled.h"

bool g_bClientPrintEnable = true;

const char *sPlayerModelFiles[] =
{
	"models/player.mdl",
	"models/player/leet/leet.mdl",
	"models/player/gign/gign.mdl",
	"models/player/vip/vip.mdl",
	"models/player/gsg9/gsg9.mdl",
	"models/player/guerilla/guerilla.mdl",
	"models/player/arctic/arctic.mdl",
	"models/player/sas/sas.mdl",
	"models/player/terror/terror.mdl",
	"models/player/urban/urban.mdl",
	"models/player/spetsnaz/spetsnaz.mdl", // CZ
	"models/player/militia/militia.mdl"    // CZ
};

static entity_field_alias_t entity_field_alias[] =
{
	{ "origin[0]", 0 },
	{ "origin[1]", 0 },
	{ "origin[2]", 0 },
	{ "angles[0]", 0 },
	{ "angles[1]", 0 },
	{ "angles[2]", 0 },
};

static entity_field_alias_t player_field_alias[] =
{
	{ "origin[0]", 0 },
	{ "origin[1]", 0 },
	{ "origin[2]", 0 },
};

static entity_field_alias_t custom_entity_field_alias[] =
{
	{ "origin[0]", 0 },
	{ "origin[1]", 0 },
	{ "origin[2]", 0 },
	{ "angles[0]", 0 },
	{ "angles[1]", 0 },
	{ "angles[2]", 0 },
	{ "skin",      0 },
	{ "sequence",  0 },
	{ "animtime",  0 },
};

bool g_bServerActive = false;
PLAYERPVSSTATUS g_PVSStatus[MAX_CLIENTS];
unsigned short m_usResetDecals;
unsigned short g_iShadowSprite;

void LinkUserMessages()
{
	if (gmsgGeigerRange)
		return;

	gmsgGeigerRange   = REG_USER_MSG("Geiger", 1);
	gmsgFlashlight    = REG_USER_MSG("Flashlight", 2);
	gmsgFlashBattery  = REG_USER_MSG("FlashBat", 1);
	gmsgHealth        = REG_USER_MSG("Health", 3);
	gmsgDamage        = REG_USER_MSG("Damage", 12);
	gmsgBattery       = REG_USER_MSG("Battery", 2);
	gmsgTrain         = REG_USER_MSG("Train", 1);
	gmsgHudTextPro    = REG_USER_MSG("HudTextPro", -1);
	gmsgHudText       = REG_USER_MSG("HudText", -1);
	gmsgSayText       = REG_USER_MSG("SayText", -1);
	gmsgTextMsg       = REG_USER_MSG("TextMsg", -1);
	gmsgResetHUD      = REG_USER_MSG("ResetHUD", 0);
	gmsgInitHUD       = REG_USER_MSG("InitHUD", 0);
	gmsgViewMode      = REG_USER_MSG("ViewMode", 0);
	gmsgShowGameTitle = REG_USER_MSG("GameTitle", 1);
	gmsgDeathMsg      = REG_USER_MSG("DeathMsg", -1);
	gmsgScoreAttrib   = REG_USER_MSG("ScoreAttrib", 2);
	gmsgScoreInfo     = REG_USER_MSG("ScoreInfo", 9);
	gmsgTeamInfo      = REG_USER_MSG("TeamInfo", 2);
	gmsgTeamScore     = REG_USER_MSG("TeamScore", 3);
	gmsgGameMode      = REG_USER_MSG("GameMode", 1);
	gmsgMOTD          = REG_USER_MSG("MOTD", -1);
	gmsgServerName    = REG_USER_MSG("ServerName", -1);
	gmsgAmmoPickup    = REG_USER_MSG("AmmoPickup", 2);
	gmsgWeapPickup    = REG_USER_MSG("WeapPickup", 1);
	gmsgItemPickup    = REG_USER_MSG("ItemPickup", -1);
	gmsgHideWeapon    = REG_USER_MSG("HideWeapon", 1);
	gmsgSetFOV        = REG_USER_MSG("SetFOV", 1);
	gmsgShowMenu      = REG_USER_MSG("ShowMenu", -1);
	gmsgShake         = REG_USER_MSG("ScreenShake", 6);
	gmsgFade          = REG_USER_MSG("ScreenFade", 10);
	gmsgAmmoX         = REG_USER_MSG("AmmoX", 2);
	gmsgSendAudio     = REG_USER_MSG("SendAudio", -1);
	gmsgRoundTime     = REG_USER_MSG("RoundTime", 2);
	gmsgMoney         = REG_USER_MSG("Money", 3);
	gmsgArmorType     = REG_USER_MSG("ArmorType", 1);
	gmsgBlinkAcct     = REG_USER_MSG("BlinkAcct", 1);
	gmsgStatusValue   = REG_USER_MSG("StatusValue", -1);
	gmsgStatusText    = REG_USER_MSG("StatusText", -1);
	gmsgStatusIcon    = REG_USER_MSG("StatusIcon", -1);
	gmsgBarTime       = REG_USER_MSG("BarTime", 2);
	gmsgReloadSound   = REG_USER_MSG("ReloadSound", 2);
	gmsgCrosshair     = REG_USER_MSG("Crosshair", 1);
	gmsgNVGToggle     = REG_USER_MSG("NVGToggle", 1);
	gmsgRadar         = REG_USER_MSG("Radar", 7);
	gmsgSpectator     = REG_USER_MSG("Spectator", 2);
	gmsgVGUIMenu      = REG_USER_MSG("VGUIMenu", 1);
	gmsgAllowSpec     = REG_USER_MSG("AllowSpec", 1);
	gmsgSendCorpse    = REG_USER_MSG("ClCorpse", -1);
	gmsgHLTV          = REG_USER_MSG("HLTV", 2);
	gmsgSpecHealth    = REG_USER_MSG("SpecHealth", 1);
	gmsgForceCam      = REG_USER_MSG("ForceCam", 3);
	gmsgReceiveW      = REG_USER_MSG("ReceiveW", 1);
	gmsgShadowIdx     = REG_USER_MSG("ShadowIdx", 4);
	gmsgBotVoice      = REG_USER_MSG("BotVoice", 2);
	gmsgBuyClose      = REG_USER_MSG("BuyClose", 0);
	gmsgSpecHealth2   = REG_USER_MSG("SpecHealth2", 2);
	gmsgBarTime2      = REG_USER_MSG("BarTime2", 4);
	gmsgItemStatus    = REG_USER_MSG("ItemStatus", 1);
	gmsgLocation      = REG_USER_MSG("Location", -1);
	gmsgBotProgress   = REG_USER_MSG("BotProgress", -1);
	gmsgBrass         = REG_USER_MSG("Brass", -1);
	gmsgFog           = REG_USER_MSG("Fog", 7);
	gmsgShowTimer     = REG_USER_MSG("ShowTimer", 0);
	gmsgHudTextArgs   = REG_USER_MSG("HudTextArgs", -1);
	gmsgRole		  = REG_USER_MSG("Role", 2);
	gmsgRadarPoint	  = REG_USER_MSG("RadarPoint", 8);
	gmsgRadarRP		  = REG_USER_MSG("RadarRP", 1);
	gmsgSetSlot		  = REG_USER_MSG("SetSlot", 2);
	gmsgShoot		  = REG_USER_MSG("Shoot", 2);
	gmsgSteelSight	  = REG_USER_MSG("SteelSight", 1);
	gmsgEqpSelect	  = REG_USER_MSG("EqpSelect", 1);
	gmsgSkillTimer	  = REG_USER_MSG("SkillTimer", 6);
	gmsgSound		  = REG_USER_MSG("Sound", -1);
	gmsgSecVMDL		  = REG_USER_MSG("SecVMDL", -1);
	gmsgEquipment	  = REG_USER_MSG("Equipment", 2);
	gmsgManpower	  = REG_USER_MSG("Manpower", 2);
	gmsgScheme		  = REG_USER_MSG("Scheme", 2);
	gmsgNewRound	  = REG_USER_MSG("NewRound", 0);
	gmsgGiveWpn		  = REG_USER_MSG("GiveWpn", 3);
}

void WriteSigonMessages()
{
	// send some MSG_INIT stuff.
	// abolished.
}

int CMD_ARGC_()
{
	if (!UseBotArgs)
		return CMD_ARGC();

	int i = 0;
	while (BotArgs[i])
		i++;

	return i;
}

const char *CMD_ARGV_(int i)
{
	if (!UseBotArgs)
		return CMD_ARGV(i);

	if (i < 4)
		return BotArgs[i];

	return nullptr;
}

NOXREF void set_suicide_frame(entvars_t *pev)
{
	if (!FStrEq(STRING(pev->model), "models/player.mdl"))
		return;

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_TOSS;
	pev->deadflag = DEAD_DEAD;
	pev->nextthink = -1;
}

void BlinkAccount(CBasePlayer *pPlayer, int numBlinks)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgBlinkAcct, nullptr, pPlayer->pev);
		WRITE_BYTE(numBlinks);
	MESSAGE_END();
}

BOOL EXT_FUNC ClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char *szRejectReason)
{
	return g_pGameRules->ClientConnected(pEntity, pszName, pszAddress, szRejectReason);
}

void EXT_FUNC ClientDisconnect(edict_t *pEntity)
{
	CBasePlayer *pPlayer = CBasePlayer::Instance(pEntity);

	if (!CSGameRules()->IsGameOver())
	{
		UTIL_ClientPrintAll(HUD_PRINTNOTIFY, "#Game_disconnected", STRING(pEntity->v.netname));

		pEntity->v.takedamage = DAMAGE_NO;
		pEntity->v.solid = SOLID_NOT;
		pEntity->v.flags = FL_DORMANT;

		if (pPlayer)
		{
			pPlayer->Disconnect();
		}

		UTIL_SetOrigin(&pEntity->v, pEntity->v.origin);
		g_pGameRules->ClientDisconnected(pEntity);
	}

	if (TheBots)
	{
		TheBots->ClientDisconnect(pPlayer);
	}
}

void respawn(entvars_t *pev, BOOL fCopyCorpse)
{
	if (gpGlobals->coop || gpGlobals->deathmatch)
	{
		if (CSGameRules()->m_iTotalRoundsPlayed > 0)
			CSGameRules()->MarkSpawnSkipped();

		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pev);
		pPlayer->Spawn();
	}
	else if (pev->deadflag > DEAD_NO)
	{
		SERVER_COMMAND("reload\n");
	}
}

// Suicide...
void EXT_FUNC ClientKill(edict_t *pEntity)
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = CBasePlayer::Instance(pev);

	if (pPlayer->GetObserverMode() != OBS_NONE)
		return;

	if (pPlayer->m_iJoiningState != JOINED)
		return;

	// prevent suiciding too often
	if (pPlayer->m_fNextSuicideTime > gpGlobals->time)
		return;

	pPlayer->m_LastHitGroup = HITGROUP_GENERIC;

	// don't let them suicide for 1 seconds after suiciding
	pPlayer->m_fNextSuicideTime = gpGlobals->time + 1.0f;

	// have the player kill themself
	pEntity->v.health = 0;
	pPlayer->Killed(pev, GIB_NEVER);
}

void ShowMenu(CBasePlayer *pPlayer, int bitsValidSlots, int nDisplayTime, const std::string& szText)
{
	constexpr int iLimit = 192U - sizeof(short) - sizeof(char) - sizeof(byte) - 1U;	// include '\0' at the end of a string.

	int len = (int)szText.length();

	if (len <= iLimit)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgShowMenu, nullptr, pPlayer->pev);
		WRITE_SHORT(bitsValidSlots);
		WRITE_CHAR(nDisplayTime);
		WRITE_BYTE(FALSE);
		WRITE_STRING(szText.c_str());
		MESSAGE_END();
	}
	else
	{
		int iProgress = 0;

		while ((len - iProgress) > 0)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgShowMenu, nullptr, pPlayer->pev);
			WRITE_SHORT(bitsValidSlots);
			WRITE_CHAR(nDisplayTime);
			WRITE_BYTE((len - iProgress) <= iLimit ? FALSE : TRUE);
			WRITE_STRING(szText.substr(iProgress, iLimit).c_str());
			MESSAGE_END();

			iProgress += iLimit;
		}
	}
}

void EXT_FUNC ShowVGUIMenu(CBasePlayer* pPlayer, VGUIMenu MenuType)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgVGUIMenu, nullptr, pPlayer->pev);
	WRITE_BYTE((int)MenuType);
	MESSAGE_END();
}

NOXREF int CountTeams()
{
	int iNumCT = 0, iNumTerrorist = 0;

	CBaseEntity *pEntity = nullptr;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
	{
		if (FNullEnt(pEntity->edict()))
			break;

		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);

		if (pPlayer->m_iTeam == UNASSIGNED)
			continue;

		if (pPlayer->IsDormant())
			continue;

		if (pPlayer->m_iTeam == SPECTATOR)
			continue;

		if (pPlayer->m_iTeam == CT)
			iNumCT++;

		else if (pPlayer->m_iTeam == TERRORIST)
			iNumTerrorist++;
	}

	return iNumCT - iNumTerrorist;
}

void ListPlayers(CBasePlayer *current)
{
	char message[120] = "", cNumber[12];

	CBaseEntity *pEntity = nullptr;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
	{
		if (FNullEnt(pEntity->edict()))
			break;

		if (pEntity->IsDormant())
			continue;

		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);
		int iUserID = GETPLAYERUSERID(ENT(pPlayer->pev));

		Q_sprintf(cNumber, "%d", iUserID);
		Q_strcpy(message, "\n");
		Q_strcat(message, cNumber);
		Q_strcat(message, " : ");
		Q_strcat(message, STRING(pPlayer->pev->netname));

		ClientPrint(current->pev, HUD_PRINTCONSOLE, message);
	}

	ClientPrint(current->pev, HUD_PRINTCONSOLE, "\n");
}

int CountTeamPlayers(int iTeam)
{
	int nCount = 0;
	CBaseEntity *pEntity = nullptr;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
	{
		if (FNullEnt(pEntity->edict()))
			break;

		if (pEntity->IsDormant())
			continue;

		if (GetClassPtr((CBasePlayer *)pEntity->pev)->m_iTeam == iTeam)
		{
			nCount++;
		}
	}

	return nCount;
}

void ProcessKickVote(CBasePlayer *pVotingPlayer, CBasePlayer *pKickPlayer)
{
	CBaseEntity *pTempEntity;
	CBasePlayer *pTempPlayer;
	int iValidVotes;
	int iVoteID;
	int iVotesNeeded;
	float fKickPercent;

	if (!pVotingPlayer || !pKickPlayer)
		return;

	int iTeamCount = CountTeamPlayers(pVotingPlayer->m_iTeam);

	if (iTeamCount < 3)
		return;

	iValidVotes = 0;
	pTempEntity = nullptr;
	iVoteID = pVotingPlayer->m_iCurrentKickVote;

	while ((pTempEntity = UTIL_FindEntityByClassname(pTempEntity, "player")))
	{
		if (FNullEnt(pTempEntity->edict()))
			break;

		pTempPlayer = GetClassPtr((CBasePlayer *)pTempEntity->pev);

		if (!pTempPlayer || pTempPlayer->m_iTeam == UNASSIGNED)
			continue;

		if (pTempPlayer->m_iTeam == pVotingPlayer->m_iTeam && pTempPlayer->m_iCurrentKickVote == iVoteID)
			iValidVotes++;
	}

	if (kick_percent.value < 0)
		CVAR_SET_STRING("mp_kickpercent", "0.0");

	else if (kick_percent.value > 1)
		CVAR_SET_STRING("mp_kickpercent", "1.0");

	iVotesNeeded = iValidVotes;
	fKickPercent = (iTeamCount * kick_percent.value + 0.5);

	if (iVotesNeeded >= int(fKickPercent))
	{
		SERVER_COMMAND(UTIL_VarArgs("kick #%d \"You have been voted off.\"\n", iVoteID));
		SERVER_EXECUTE();

		UTIL_ClientPrintAll(HUD_PRINTCENTER, "#Game_kicked", STRING(pKickPlayer->pev->netname));

		pTempEntity = nullptr;

		while ((pTempEntity = UTIL_FindEntityByClassname(pTempEntity, "player")))
		{
			if (FNullEnt(pTempEntity->edict()))
				break;

			pTempPlayer = GetClassPtr((CBasePlayer *)pTempEntity->pev);

			if (!pTempPlayer || pTempPlayer->m_iTeam == UNASSIGNED)
				continue;

			if (pTempPlayer->m_iTeam == pVotingPlayer->m_iTeam && pTempPlayer->m_iCurrentKickVote == iVoteID)
				pTempPlayer->m_iCurrentKickVote = 0;
		}
	}
}

void CheckStartMoney()
{
	int money = int(startmoney.value);
	int max_money = int(maxmoney.value);

	if (max_money > MAX_MONEY_THRESHOLD)
	{
		max_money = MAX_MONEY_THRESHOLD;
		CVAR_SET_FLOAT("mp_maxmoney", MAX_MONEY_THRESHOLD);
	}

	if (money > max_money)
		CVAR_SET_FLOAT("mp_startmoney", max_money);
	else if (money < 0)
		CVAR_SET_FLOAT("mp_startmoney", 0);
}

void EXT_FUNC ClientPutInServer(edict_t *pEntity)
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pev);

	pPlayer->SetCustomDecalFrames(-1);
	pPlayer->SetPrefsFromUserinfo(GET_INFO_BUFFER(pEntity));

	if (!g_pGameRules->IsMultiplayer())
	{
		pPlayer->Spawn();
		return;
	}

	pPlayer->m_bNotKilled = true;
	pPlayer->m_iIgnoreGlobalChat = IGNOREMSG_NONE;
	pPlayer->m_iTeamKills = 0;
	pPlayer->m_bJustConnected = true;
	pPlayer->Spawn();
	pPlayer->m_bTeamChanged = false;
	pPlayer->m_iNumSpawns = 0;

	CheckStartMoney();
	pPlayer->AddAccount(startmoney.value, RT_PLAYER_JOIN);

	pPlayer->m_fGameHUDInitialized = false;
	pPlayer->m_flDisplayHistory &= ~DHF_ROUND_STARTED;
	pPlayer->pev->flags |= FL_SPECTATOR;
	pPlayer->pev->solid = SOLID_NOT;
	pPlayer->pev->movetype = MOVETYPE_NOCLIP;
	pPlayer->pev->effects = (EF_NODRAW | EF_NOINTERP);
	pPlayer->pev->takedamage = DAMAGE_NO;
	pPlayer->pev->deadflag = DEAD_DEAD;
	pPlayer->pev->velocity = g_vecZero;
	pPlayer->pev->punchangle = g_vecZero;
	pPlayer->m_iJoiningState = READINGLTEXT;
	pPlayer->m_iTeam = UNASSIGNED;
	pPlayer->pev->fixangle = 1;
	pPlayer->m_iModelName = MODEL_URBAN;
	pPlayer->m_bContextHelp = true;
	pPlayer->m_bHasNightVision = false;
	pPlayer->m_iHostagesKilled = 0;
	pPlayer->m_iMapVote = 0;
	pPlayer->m_iCurrentKickVote = 0;
	pPlayer->m_fDeadTime = 0;
	pPlayer->has_disconnected = false;
	pPlayer->m_iMenu = Menu_OFF;

	SET_CLIENT_MAXSPEED(ENT(pPlayer->pev), 1);
	SET_MODEL(ENT(pPlayer->pev), "models/player.mdl");

	pPlayer->SetThink(nullptr);

	CBaseEntity *pTarget = nullptr;
	pPlayer->m_pIntroCamera = UTIL_FindEntityByClassname(nullptr, "trigger_camera");

	if (g_pGameRules && g_pGameRules->IsMultiplayer())
	{
		CSGameRules()->m_bMapHasCameras = (pPlayer->m_pIntroCamera != nullptr);
	}

	if (pPlayer->m_pIntroCamera)
	{
		// find the target (by default info_target) for the camera view direction.
		pTarget = UTIL_FindEntityByTargetname(nullptr, STRING(pPlayer->m_pIntroCamera->pev->target));
	}

	if (pPlayer->m_pIntroCamera && pTarget)
	{
		Vector CamAngles = UTIL_VecToAngles((pTarget->pev->origin - pPlayer->m_pIntroCamera->pev->origin).Normalize());
		CamAngles.x = -CamAngles.x;

		UTIL_SetOrigin(pPlayer->pev, pPlayer->m_pIntroCamera->pev->origin);

		pPlayer->pev->angles = CamAngles;
		pPlayer->pev->v_angle = pPlayer->pev->angles;

		pPlayer->m_fIntroCamTime = gpGlobals->time + 6;
		pPlayer->pev->view_ofs = g_vecZero;
	}

	if (TheBots)
	{
		TheBots->OnEvent(EVENT_PLAYER_CHANGED_TEAM, (CBaseEntity *)pPlayer);
	}

	pPlayer->m_iJoiningState = SHOWLTEXT;

	static char sName[128];
	Q_strcpy(sName, STRING(pPlayer->pev->netname));

	for (char *pApersand = sName; pApersand && *pApersand != '\0'; pApersand++)
	{
		if (*pApersand == '%')
			*pApersand = ' ';
	}

	UTIL_ClientPrintAll(HUD_PRINTNOTIFY, "#Game_connected", (sName[0] != '\0') ? sName : "<unconnected>");
}

void Host_Say(edict_t *pEntity, BOOL teamonly)
{
	int j;
	char *p;
	char text[128];
	char szTemp[256];
	const char *cpSay = "say";
	const char *cpSayTeam = "say_team";
	const char *pcmd = CMD_ARGV_(0);
	bool bSenderDead = false;

	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pev);

	if (pPlayer->m_flLastTalk != 0.0f && gpGlobals->time - pPlayer->m_flLastTalk < 0.66f)
		return;

	pPlayer->m_flLastTalk = gpGlobals->time;

	if (pPlayer->pev->deadflag != DEAD_NO)
		bSenderDead = true;

	// We can get a raw string now, without the "say " prepended
	if (CMD_ARGC_() == 0)
		return;

	if (!Q_stricmp(pcmd, cpSay) || !Q_stricmp(pcmd, cpSayTeam))
	{
		if (CMD_ARGC_() >= 2)
		{
			p = (char *)CMD_ARGS();
		}
		else
		{
			// say with a blank message, nothing to do
			return;
		}
	}
	else // Raw text, need to prepend argv[0]
	{
		if (CMD_ARGC_() >= 2)
		{
			Q_sprintf(szTemp, "%s %s", (char *)pcmd, (char *)CMD_ARGS());
		}
		else
		{
			// Just a one word command, use the first word...sigh
			Q_sprintf(szTemp, "%s", (char *)pcmd);
		}

		p = szTemp;
	}

	Q_StripPrecedingAndTrailingWhitespace(p);

	// remove quotes (leading & trailing) if present
	if (*p == '"')
	{
		p++;
		p[Q_strlen(p) - 1] = '\0';
	}

	// make sure the text has content
	if (/*!p || */!p[0] || !Q_UnicodeValidate(p))
	{
		// no character found, so say nothing
		return;
	}

	Q_StripUnprintableAndSpace(p);

	if (Q_strlen(p) <= 0)
		return;

	const char *placeName = nullptr;
	const char *pszFormat = nullptr;
	const char *pszConsoleFormat = nullptr;
	bool consoleUsesPlaceName = false;

	// team only
	if (teamonly)
	{
		if (pPlayer->m_iTeam == CT || pPlayer->m_iTeam == TERRORIST)
		{
			// search the place name where is located the player
			Place playerPlace = TheNavAreaGrid.GetPlace(&pPlayer->pev->origin);
			const BotPhraseList *placeList = TheBotPhrases->GetPlaceList();

			for (auto phrase : *placeList)
			{
				if (phrase->GetID() == playerPlace)
				{
					placeName = phrase->GetName();
					break;
				}
			}
		}

		if (pPlayer->m_iTeam == CT)
		{
			if (bSenderDead)
			{
				pszFormat = "#Cstrike_Chat_CT_Dead";
				pszConsoleFormat = "*DEAD*(Counter-Terrorist) %s : %s";
			}
			else if (placeName)
			{
				pszFormat = "#Cstrike_Chat_CT_Loc";
				pszConsoleFormat = "*(Counter-Terrorist) %s @ %s : %s";
				consoleUsesPlaceName = true;
			}
			else
			{
				pszFormat = "#Cstrike_Chat_CT";
				pszConsoleFormat = "(Counter-Terrorist) %s : %s";
			}
		}
		else if (pPlayer->m_iTeam == TERRORIST)
		{
			if (bSenderDead)
			{
				pszFormat = "#Cstrike_Chat_T_Dead";
				pszConsoleFormat = "*DEAD*(Terrorist) %s : %s";
			}
			else if (placeName)
			{
				pszFormat = "#Cstrike_Chat_T_Loc";
				pszConsoleFormat = "(Terrorist) %s @ %s : %s";
				consoleUsesPlaceName = true;
			}
			else
			{
				pszFormat = "#Cstrike_Chat_T";
				pszConsoleFormat = "(Terrorist) %s : %s";
			}
		}
		else
		{
			pszFormat = "#Cstrike_Chat_Spec";
			pszConsoleFormat = "(Spectator) %s : %s";
		}
	}
	// everyone
	else
	{
		if (bSenderDead)
		{
			if (pPlayer->m_iTeam == SPECTATOR)
			{
				pszFormat = "#Cstrike_Chat_AllSpec";
				pszConsoleFormat = "*SPEC* %s : %s";
			}
			else
			{
				pszFormat = "#Cstrike_Chat_AllDead";
				pszConsoleFormat = "*DEAD* %s : %s";
			}
		}
		else
		{
			pszFormat = "#Cstrike_Chat_All";
			pszConsoleFormat = "%s : %s";
		}
	}

	text[0] = '\0';

	// -3 for /n and null terminator
	j = sizeof(text) - 3 - Q_strlen(text) - Q_strlen(pszFormat);

	if (placeName)
	{
		j -= Q_strlen(placeName) + 1;
	}

	if ((signed int)Q_strlen(p) > j)
		p[j] = 0;

	for (char *pAmpersand = p; pAmpersand && *pAmpersand != '\0'; pAmpersand++)
	{
		if (pAmpersand[0] == '%')
		{
			if (pAmpersand[1] != 'l' && pAmpersand[1] != ' ' && pAmpersand[1] != '\0')
			{
				pAmpersand[0] = ' ';
			}
		}
	}

	Q_strcat(text, p);
	Q_strcat(text, "\n");

	// loop through all players
	// Start with the first player.
	// This may return the world in single player if the client types something between levels or during spawn
	// so check it, or it will infinite loop

	CBasePlayer *pReceiver = nullptr;
	while ((pReceiver = UTIL_FindEntityByClassname(pReceiver, "player")))
	{
		if (FNullEnt(pReceiver->edict()))
			break;

		if (!pReceiver->pev)
			continue;

		if (pReceiver->edict() == pEntity)
			continue;

		// Not a client ? (should never be true)
		if (!pReceiver->IsNetClient())
			continue;

		// can the receiver hear the sender? or has he muted him?
		if (gpGlobals->deathmatch != 0.0f && CSGameRules()->m_VoiceGameMgr.PlayerHasBlockedPlayer(pReceiver, pPlayer))
			continue;

		if (teamonly && pReceiver->m_iTeam != pPlayer->m_iTeam)
			continue;

		if ((pReceiver->pev->deadflag != DEAD_NO && !bSenderDead) || (pReceiver->pev->deadflag == DEAD_NO && bSenderDead))
		{
			if (!(pPlayer->pev->flags & FL_PROXY))
				continue;
		}

		if ((pReceiver->m_iIgnoreGlobalChat == IGNOREMSG_ENEMY && pReceiver->m_iTeam == pPlayer->m_iTeam)
			|| pReceiver->m_iIgnoreGlobalChat == IGNOREMSG_NONE)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgSayText, nullptr, pReceiver->pev);
				WRITE_BYTE(ENTINDEX(pEntity));
				WRITE_STRING(pszFormat);
				WRITE_STRING("");
				WRITE_STRING(text);

				if (placeName)
				{
					WRITE_STRING(placeName);
				}

			MESSAGE_END();
		}
	}

	// print to the sending client
	MESSAGE_BEGIN(MSG_ONE, gmsgSayText, nullptr, &pEntity->v);
		WRITE_BYTE(ENTINDEX(pEntity));
		WRITE_STRING(pszFormat);
		WRITE_STRING("");
		WRITE_STRING(text);

		if (placeName)
		{
			WRITE_STRING(placeName);
		}

	MESSAGE_END();

	// don't to type for listenserver
	if (IS_DEDICATED_SERVER())
	{
		// echo to server console
		if (pszConsoleFormat)
		{
			if (placeName && consoleUsesPlaceName)
				SERVER_PRINT(UTIL_VarArgs(pszConsoleFormat, STRING(pPlayer->pev->netname), placeName, text));
			else
				SERVER_PRINT(UTIL_VarArgs(pszConsoleFormat, STRING(pPlayer->pev->netname), text));
		}
		else
		{
			SERVER_PRINT(text);
		}
	}

	if (logmessages.value)
	{
		const char *temp = teamonly ? "say_team" : "say";
		const char *deadText = (pPlayer->m_iTeam != SPECTATOR && bSenderDead) ? " (dead)" : "";

		const char *szTeam = GetTeam(pPlayer->m_iTeam);

		UTIL_LogPrintf("\"%s<%i><%s><%s>\" %s \"%s\"%s\n", STRING(pPlayer->pev->netname), GETPLAYERUSERID(pPlayer->edict()), GETPLAYERAUTHID(pPlayer->edict()),
			szTeam, temp, p, deadText);
	}
}

void BuyEquipment(CBasePlayer *pPlayer, EquipmentIdType iSlot)
{
	if (!CSGameRules()->CanHaveEquipment(pPlayer, iSlot))
		return;

	int iItemPrice = 0;
	const char *pszItem = nullptr;

	bool bFullArmor = (int(pPlayer->pev->armorvalue) >= int(CSGameRules()->PlayerMaxArmour(pPlayer)));
	bool bHasHelmet = (pPlayer->pev->armortype >= ARMOR_VESTHELM);
	bool bEnoughMoney = false;

	switch (iSlot)
	{
		case EQP_KEVLAR:
		{
			if (bFullArmor)
			{
				if (g_bClientPrintEnable)
				{
					ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Already_Have_Kevlar");
				}

				return;
			}

			if (pPlayer->m_iAccount >= KEVLAR_PRICE)
			{
				if (bHasHelmet && g_bClientPrintEnable)
				{
					ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Already_Have_Helmet_Bought_Kevlar");
				}

				bEnoughMoney = true;
				pszItem = "item_kevlar";
				iItemPrice = KEVLAR_PRICE;
			}
			break;
		}
		case EQP_ASSAULT_SUIT:
		{
			if (bFullArmor)
			{
				if (bHasHelmet)
				{
					if (g_bClientPrintEnable)
					{
						ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Already_Have_Kevlar_Helmet");
					}

					return;
				}

				if (pPlayer->m_iAccount >= HELMET_PRICE)
				{
					if (g_bClientPrintEnable)
					{
						ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Already_Have_Kevlar_Bought_Helmet");
					}

					bEnoughMoney = true;
					pszItem = "item_assaultsuit";
					iItemPrice = HELMET_PRICE;
				}
				break;
			}
			else
			{
				if (bHasHelmet)
				{
					if (pPlayer->m_iAccount >= KEVLAR_PRICE)
					{
						if (g_bClientPrintEnable)
						{
							ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Already_Have_Helmet_Bought_Kevlar");
						}

						bEnoughMoney = true;
						pszItem = "item_assaultsuit";
						iItemPrice = KEVLAR_PRICE;
					}
				}
				else
				{
					if (pPlayer->m_iAccount >= ASSAULTSUIT_PRICE)
					{
						bEnoughMoney = true;
						pszItem = "item_assaultsuit";
						iItemPrice = ASSAULTSUIT_PRICE;
					}
				}
			}
			break;
		}
		case EQP_HEGRENADE:
		case EQP_FLASHBANG:
		case EQP_SMOKEGRENADE:
		case EQP_CRYOGRENADE:
		case EQP_INCENDIARY_GR:
		case EQP_HEALING_GR:
		case EQP_GAS_GR:
		case EQP_C4:
		{
			AmmoIdType iAmmoId = GetAmmoIdOfEquipment(iSlot);

			if (!CSGameRules()->CanHaveAmmo(pPlayer, iAmmoId))
			{
				if (g_bClientPrintEnable)
				{
					ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Cannot_Carry_Anymore");
				}

				return;
			}

			iItemPrice = GetEquipmentPrice(pPlayer->m_iRoleType, iSlot);

			if (pPlayer->m_iAccount >= iItemPrice)
			{
				EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "items/ammopickup1.wav", VOL_NORM, ATTN_STATIC);

				pPlayer->m_rgAmmo[iAmmoId]++;	// give player the equipment.
				bEnoughMoney = true;

				if (!pPlayer->m_iUsingGrenadeId || pPlayer->m_rgAmmo[GetAmmoIdOfEquipment(pPlayer->m_iUsingGrenadeId)] <= 0)	// choose the new grenade as our grenade.
					pPlayer->m_iUsingGrenadeId = iSlot;

				// give a detonator if player is buying a C4.
				if (iSlot == EQP_C4)
				{
					pPlayer->m_rgbHasEquipment[EQP_DETONATOR] = true;
					pPlayer->SendItemStatus();
				}
			}

			break;
		}
		case EQP_NVG:
		{
			if (pPlayer->m_bHasNightVision)
			{
				if (g_bClientPrintEnable)
				{
					ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Already_Have_One");
				}

				return;
			}

			if (pPlayer->m_iAccount >= NVG_PRICE)
			{
				if (!(pPlayer->m_flDisplayHistory & DHF_NIGHTVISION))
				{
					pPlayer->HintMessage("#Hint_use_nightvision");
					pPlayer->m_flDisplayHistory |= DHF_NIGHTVISION;
				}

				EMIT_SOUND(ENT(pPlayer->pev), CHAN_ITEM, "items/equip_nvg.wav", VOL_NORM, ATTN_NORM);

				bEnoughMoney = true;
				pPlayer->m_bHasNightVision = true;
				pPlayer->m_rgbHasEquipment[EQP_NVG] = true;
				pPlayer->AddAccount(-NVG_PRICE, RT_PLAYER_BOUGHT_SOMETHING);
				pPlayer->SendItemStatus();
			}
			break;
		}
		/* disused.
		case MENU_SLOT_ITEM_DEFUSEKIT:
		{
			return;
			break;
		}
		case MENU_SLOT_ITEM_SHIELD:
		{
			if (CSGameRules()->CanHavePlayerItem(pPlayer, WEAPON_SHIELDGUN, true))
				return;

			if (pPlayer->m_iAccount >= SHIELDGUN_PRICE)
			{
				bEnoughMoney = true;

				pPlayer->DropPrimary();
				pPlayer->GiveShield();
				pPlayer->AddAccount(-SHIELDGUN_PRICE, RT_PLAYER_BOUGHT_SOMETHING);

				EMIT_SOUND(ENT(pPlayer->pev), CHAN_ITEM, "items/gunpickup2.wav", VOL_NORM, ATTN_NORM);
			}
			break;
		}
		*/
	}

	if (!bEnoughMoney)
	{
		if (g_bClientPrintEnable)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Not_Enough_Money");
			BlinkAccount(pPlayer);
		}

		return;
	}

	if (pszItem)
	{
		pPlayer->GiveNamedItem(pszItem);
	}

	if (iItemPrice)
	{
		pPlayer->AddAccount(-iItemPrice, RT_PLAYER_BOUGHT_SOMETHING);
	}
}

CBaseWeapon*BuyWeapon(CBasePlayer *pPlayer, WeaponIdType weaponID)
{
	if (weaponID <= 0 || weaponID >= LAST_WEAPON)
		return nullptr;

	if (!CSGameRules()->CanHavePlayerItem(pPlayer, weaponID, true))
		return nullptr;

	const WeaponInfo *info = &g_rgWpnInfo[weaponID];
	if (!info || !Q_strlen(info->m_pszInternalName))
		return nullptr;

	int iCost = GetCost(pPlayer->m_iRoleType, weaponID);

	if (pPlayer->m_iAccount < iCost)
	{
		if (g_bClientPrintEnable)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Not_Enough_Money");
			BlinkAccount(pPlayer);
		}

		return nullptr;
	}

	if (IsPrimaryWeapon(weaponID))
	{
		pPlayer->DropPrimary();
	}
	else if (IsSecondaryWeapon(weaponID))
	{
		pPlayer->DropSecondary();
	}

	auto pWeapon = CBaseWeapon::Give(weaponID, pPlayer);
	pPlayer->AddPlayerItem(pWeapon);
	pPlayer->AddAccount(-iCost, RT_PLAYER_BOUGHT_SOMETHING);

	if (refill_bpammo_weapons.value > 1)
	{
		pPlayer->GiveAmmo(g_rgAmmoInfo[info->m_iAmmoType].m_iCountPerBox, info->m_iAmmoType);
	}

	return pWeapon;
}

void EXT_FUNC HandleMenu_ChooseAppearance(CBasePlayer *pPlayer, int slot)
{
	struct
	{
		ModelName model_id;
		const char *model_name;
		int model_name_index;

	} appearance;

	Q_memset(&appearance, 0, sizeof(appearance));

	if (pPlayer->m_iTeam == TERRORIST)
	{
		if ((slot > CZ_NUM_SKIN || slot < 1) && (!TheBotProfiles->GetCustomSkin(slot) || !pPlayer->IsBot()))
		{
			slot = RANDOM_LONG(1, CZ_NUM_SKIN);
		}

		switch (slot)
		{
		case 1:
			appearance.model_id = MODEL_TERROR;
			appearance.model_name = "terror";
			break;
		case 2:
			appearance.model_id = MODEL_LEET;
			appearance.model_name = "leet";
			break;
		case 3:
			appearance.model_id = MODEL_ARCTIC;
			appearance.model_name = "arctic";
			break;
		case 4:
			appearance.model_id = MODEL_GUERILLA;
			appearance.model_name = "guerilla";
			break;
		case 5:
			appearance.model_id = MODEL_MILITIA;
			appearance.model_name = "militia";
			break;
		default:
			if (TheBotProfiles->GetCustomSkinModelname(slot) && pPlayer->IsBot())
			{
				appearance.model_name = (char*)TheBotProfiles->GetCustomSkinModelname(slot);
			}
			else
			{
				appearance.model_id = MODEL_TERROR;
				appearance.model_name = "terror";
			}
			break;
		}

		// default T model models/player/terror/terror.mdl
		appearance.model_name_index = 8;

	}
	else if (pPlayer->m_iTeam == CT)
	{
		if ((slot > CZ_NUM_SKIN || slot < 1) && (!TheBotProfiles->GetCustomSkin(slot) || !pPlayer->IsBot()))
		{
			slot = RANDOM_LONG(1, CZ_NUM_SKIN);
		}

		switch (slot)
		{
		case 1:
			appearance.model_id = MODEL_URBAN;
			appearance.model_name = "urban";
			break;
		case 2:
			appearance.model_id = MODEL_GSG9;
			appearance.model_name = "gsg9";
			break;
		case 3:
			appearance.model_id = MODEL_SAS;
			appearance.model_name = "sas";
			break;
		case 4:
			appearance.model_id = MODEL_GIGN;
			appearance.model_name = "gign";
			break;
		case 5:
			appearance.model_id = MODEL_SPETSNAZ;
			appearance.model_name = "spetsnaz";
			break;
		default:
			if (TheBotProfiles->GetCustomSkinModelname(slot) && pPlayer->IsBot())
			{
				appearance.model_name = (char*)TheBotProfiles->GetCustomSkinModelname(slot);
			}
			else
			{
				appearance.model_id = MODEL_URBAN;
				appearance.model_name = "urban";
			}
			break;
		}

		// default CT model models/player/urban/urban.mdl
		appearance.model_name_index = 9;
	}

	pPlayer->ResetMenu();

	// Reset the player's state
	switch (pPlayer->m_iJoiningState)
	{
	case JOINED:
		CSGameRules()->CheckWinConditions();
		break;

	case PICKINGTEAM:
	{
		pPlayer->m_iJoiningState = GETINTOGAME;
		break;
	}
	}

	pPlayer->pev->body = 0;
	pPlayer->m_iModelName = appearance.model_id;

	pPlayer->SetClientUserInfoModel(GET_INFO_BUFFER(pPlayer->edict()), appearance.model_name);
	pPlayer->SetNewPlayerModel(sPlayerModelFiles[appearance.model_name_index]);
}

// returns true if the selection has been handled and the player's menu
// can be closed...false if the menu should be displayed again
BOOL EXT_FUNC HandleMenu_ChooseTeam(CBasePlayer *pPlayer, int slot)
{
	TeamName team = UNASSIGNED;

	switch (slot)
	{
	case MENU_SLOT_TEAM_TERRORIST:
		team = TERRORIST;
		break;
	case MENU_SLOT_TEAM_CT:
		team = CT;
		break;
	case MENU_SLOT_TEAM_RANDOM:
	{
		// Attempt to auto-select a team
		team = CSGameRules()->SelectDefaultTeam();
		if (team == UNASSIGNED)
		{
			if (cv_bot_auto_vacate.value > 0.0f && !pPlayer->IsBot())
			{
				team = (RANDOM_LONG(0, 1) == 0) ? TERRORIST : CT;

				bool atLeastOneLeft = UTIL_KickBotFromTeam(team);

				if (!atLeastOneLeft)
				{
					// no bots on that team, try the other
					team = (team == CT) ? TERRORIST : CT;

					atLeastOneLeft = UTIL_KickBotFromTeam(team);

					if (!atLeastOneLeft)
					{
						// couldn't kick any bots, fail
						team = UNASSIGNED;
					}
				}

				if (atLeastOneLeft)
				{
					CONSOLE_ECHO("These bots has left the game to make room for human players.\n");
				}
			}
		}

		break;
	}
	case MENU_SLOT_TEAM_SPECT:
	{
		// Prevent this is the cvar is set
		// spectator proxy
		if (!allow_spectators.value && !(pPlayer->pev->flags & FL_PROXY))
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Cannot_Be_Spectator");
			CLIENT_COMMAND(ENT(pPlayer->pev), "slot10\n");

			return FALSE;
		}

		// are we already a spectator?
		if (pPlayer->m_iTeam == SPECTATOR)
		{
			return TRUE;
		}

		// Only spectate if we are in the freeze period or dead.
		// This is done here just in case.
		if (g_pGameRules->IsFreezePeriod() || pPlayer->pev->deadflag != DEAD_NO)
		{
			if (pPlayer->m_iTeam != UNASSIGNED && pPlayer->pev->deadflag == DEAD_NO)
			{
				ClientKill(pPlayer->edict());

				// add 1 to frags to balance out the 1 subtracted for killing yourself
				pPlayer->pev->frags++;
			}

			pPlayer->RemoveAllItems(TRUE);


			if (pPlayer->m_iTeam != SPECTATOR)
			{
				// notify other clients of player joined to team spectator
				UTIL_LogPrintf("\"%s<%i><%s><%s>\" joined team \"SPECTATOR\"\n", STRING(pPlayer->pev->netname),
					GETPLAYERUSERID(pPlayer->edict()), GETPLAYERAUTHID(pPlayer->edict()), GetTeam(pPlayer->m_iTeam));
			}

			pPlayer->m_iTeam = SPECTATOR;
			pPlayer->m_iJoiningState = JOINED;

			// Reset money
			pPlayer->AddAccount(0, RT_PLAYER_SPEC_JOIN, false);

			MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
				WRITE_BYTE(ENTINDEX(pPlayer->edict()));
				WRITE_SHORT(int(pPlayer->pev->frags));
				WRITE_SHORT(pPlayer->m_iDeaths);
				WRITE_SHORT(0);
				WRITE_SHORT(0);
			MESSAGE_END();

			pPlayer->m_pIntroCamera = nullptr;
			pPlayer->m_bTeamChanged = true;

			if (TheBots)
			{
				TheBots->OnEvent(EVENT_PLAYER_CHANGED_TEAM, pPlayer);
			}

			pPlayer->TeamChangeUpdate();

			edict_t *pentSpawnSpot = g_pGameRules->GetPlayerSpawnSpot(pPlayer);
			pPlayer->StartObserver(pentSpawnSpot->v.origin, pentSpawnSpot->v.angles);

			// do we have fadetoblack on? (need to fade their screen back in)
			if (fadetoblack.value)
			{
				UTIL_ScreenFade(pPlayer, Vector(0, 0, 0), 0.001, 0, 0, FFADE_IN);
			}

			return TRUE;
		}
		else
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Cannot_Be_Spectator");
			CLIENT_COMMAND(ENT(pPlayer->pev), "slot10\n");

			return FALSE;
		}

		break;
	}
	default:
		return FALSE;
	}

	// If the code gets this far, the team is not TEAM_UNASSIGNED
	// Player is switching to a new team (It is possible to switch to the
	// same team just to choose a new appearance)
	if (CSGameRules()->TeamFull(team))
	{
		// The specified team is full
		// attempt to kick a bot to make room for this player
		bool madeRoom = false;
		if (cv_bot_auto_vacate.value > 0 && !pPlayer->IsBot())
		{
			if (UTIL_KickBotFromTeam(team))
			{
				CONSOLE_ECHO("These bots has left the game to make room for human players.\n");
				madeRoom = true;
			}
		}

		if (!madeRoom)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, (team == TERRORIST) ? "#Terrorists_Full" : "#CTs_Full");
			return FALSE;
		}
	}

	// players are allowed to change to their own team so they can just change their model
	if (CSGameRules()->TeamStacked(team, pPlayer->m_iTeam))
	{
		// The specified team is full
		ClientPrint(pPlayer->pev, HUD_PRINTCENTER, (team == TERRORIST) ? "#Too_Many_Terrorists" : "#Too_Many_CTs");
		return FALSE;
	}

	if (team != SPECTATOR && !pPlayer->IsBot() && !(pPlayer->pev->flags & FL_FAKECLIENT) && auto_join_team.value != 1.0f)
	{
		int humanTeam = UNASSIGNED;
		if (!Q_stricmp(humans_join_team.string, "CT"))
		{
			humanTeam = CT;
		}
		else if (!Q_stricmp(humans_join_team.string, "T"))
		{
			humanTeam = TERRORIST;
		}

		if (humanTeam != UNASSIGNED && team != humanTeam)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, (team == TERRORIST) ? "#Humans_Join_Team_CT" : "#Humans_Join_Team_T");
			return FALSE;
		}
	}

	// If we already died and changed teams once, deny
	if (pPlayer->m_bTeamChanged)
	{
		if (pPlayer->pev->deadflag != DEAD_NO)
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Only_1_Team_Change");
			return FALSE;
		}
	}

	if (pPlayer->m_iTeam == SPECTATOR && team != SPECTATOR)
	{
		// If they're switching into spectator, setup spectator properties..
		pPlayer->m_bNotKilled = true;
		pPlayer->m_iIgnoreGlobalChat = IGNOREMSG_NONE;
		pPlayer->m_iTeamKills = 0;

		CheckStartMoney();

		// all players start with "mp_startmoney" bucks
		pPlayer->AddAccount(startmoney.value, RT_PLAYER_SPEC_JOIN, false);

		pPlayer->pev->solid = SOLID_NOT;
		pPlayer->pev->movetype = MOVETYPE_NOCLIP;
		pPlayer->pev->effects = (EF_NODRAW | EF_NOINTERP);
		pPlayer->pev->takedamage = DAMAGE_NO;
		pPlayer->pev->deadflag = DEAD_DEAD;
		pPlayer->pev->punchangle = g_vecZero;

		pPlayer->m_bHasNightVision = false;
		pPlayer->m_iHostagesKilled = 0;
		pPlayer->m_fDeadTime = 0;
		pPlayer->has_disconnected = false;
		pPlayer->m_iJoiningState = PICKINGTEAM;

		pPlayer->SendItemStatus();

		SET_MODEL(ENT(pPlayer->pev), "models/player.mdl");
	}

	// Just select a model randomly. We are making a new set of model.
	HandleMenu_ChooseAppearance(pPlayer, 6);

	TeamName oldTeam;
	const char *szOldTeam, *szNewTeam;

	// Switch their actual team...
	pPlayer->m_bTeamChanged = true;
	oldTeam = pPlayer->m_iTeam;
	pPlayer->m_iTeam = team;

	if (TheBots)
	{
		TheBots->OnEvent(EVENT_PLAYER_CHANGED_TEAM, pPlayer);
	}

	pPlayer->TeamChangeUpdate();

	szOldTeam = GetTeam(oldTeam);
	szNewTeam = GetTeam(team);

	// Notify others that this player has joined a new team
	UTIL_ClientPrintAll(HUD_PRINTNOTIFY, (team == TERRORIST) ? "#Game_join_terrorist" : "#Game_join_ct",
		(STRING(pPlayer->pev->netname) && STRING(pPlayer->pev->netname)[0] != 0) ? STRING(pPlayer->pev->netname) : "<unconnected>");

	UTIL_LogPrintf("\"%s<%i><%s><%s>\" joined team \"%s\"\n", STRING(pPlayer->pev->netname), GETPLAYERUSERID(pPlayer->edict()), GETPLAYERAUTHID(pPlayer->edict()), szOldTeam, szNewTeam);
	return TRUE;
}

void Radio1(CBasePlayer *pPlayer, int slot)
{
	if (pPlayer->m_flRadioTime >= gpGlobals->time)
		return;

	if (pPlayer->m_iRadioMessages <= 0)
		return;

	pPlayer->m_iRadioMessages--;
	pPlayer->m_flRadioTime = gpGlobals->time + radio_timeout.value;

	switch (slot)
	{
	case 1:
		pPlayer->Radio("%!MRAD_COVERME", "#Cover_me");
		break;
	case 2:
		pPlayer->Radio("%!MRAD_TAKEPOINT", "#You_take_the_point");
		break;
	case 3:
		pPlayer->Radio("%!MRAD_POSITION", "#Hold_this_position");
		break;
	case 4:
		pPlayer->Radio("%!MRAD_REGROUP", "#Regroup_team");
		break;
	case 5:
		pPlayer->Radio("%!MRAD_FOLLOWME", "#Follow_me");
		break;
	case 6:
		pPlayer->Radio("%!MRAD_HITASSIST", "#Taking_fire");
		break;
	}

	if (TheBots)
	{
		TheBots->OnEvent((GameEventType)(EVENT_START_RADIO_1 + slot), pPlayer);
	}
}

void Radio2(CBasePlayer *pPlayer, int slot)
{
	if (pPlayer->m_flRadioTime >= gpGlobals->time)
		return;

	if (pPlayer->m_iRadioMessages <= 0)
		return;

	pPlayer->m_iRadioMessages--;
	pPlayer->m_flRadioTime = gpGlobals->time + radio_timeout.value;

	switch (slot)
	{
	case 1:
		pPlayer->Radio("%!MRAD_GO", "#Go_go_go");
		break;
	case 2:
		pPlayer->Radio("%!MRAD_FALLBACK", "#Team_fall_back");
		break;
	case 3:
		pPlayer->Radio("%!MRAD_STICKTOG", "#Stick_together_team");
		break;
	case 4:
		pPlayer->Radio("%!MRAD_GETINPOS", "#Get_in_position_and_wait");
		break;
	case 5:
		pPlayer->Radio("%!MRAD_STORMFRONT", "#Storm_the_front");
		break;
	case 6:
		pPlayer->Radio("%!MRAD_REPORTIN", "#Report_in_team");
		break;
	}

	if (TheBots)
	{
		TheBots->OnEvent((GameEventType)(EVENT_START_RADIO_2 + slot), pPlayer);
	}
}

void Radio3(CBasePlayer *pPlayer, int slot)
{
	if (pPlayer->m_flRadioTime >= gpGlobals->time)
		return;

	if (pPlayer->m_iRadioMessages <= 0)
		return;

	pPlayer->m_iRadioMessages--;
	pPlayer->m_flRadioTime = gpGlobals->time + radio_timeout.value;

	switch (slot)
	{
	case 1:
		if (RANDOM_LONG(0, 1))
			pPlayer->Radio("%!MRAD_AFFIRM", "#Affirmative");
		else
			pPlayer->Radio("%!MRAD_ROGER", "#Roger_that");

		break;
	case 2:
		pPlayer->Radio("%!MRAD_ENEMYSPOT", "#Enemy_spotted");
		break;
	case 3:
		pPlayer->Radio("%!MRAD_BACKUP", "#Need_backup");
		break;
	case 4:
		pPlayer->Radio("%!MRAD_CLEAR", "#Sector_clear");
		break;
	case 5:
		pPlayer->Radio("%!MRAD_INPOS", "#In_position");
		break;
	case 6:
		pPlayer->Radio("%!MRAD_REPRTINGIN", "#Reporting_in");
		break;
	case 7:
		pPlayer->Radio("%!MRAD_BLOW", "#Get_out_of_there");
		break;
	case 8:
		pPlayer->Radio("%!MRAD_NEGATIVE", "#Negative");
		break;
	case 9:
		pPlayer->Radio("%!MRAD_ENEMYDOWN", "#Enemy_down");
		break;
	}

	if (TheBots)
	{
		TheBots->OnEvent((GameEventType)(EVENT_START_RADIO_3 + slot), pPlayer);
	}
}

bool EXT_FUNC BuyGunAmmo(CBasePlayer *pPlayer, CBaseWeapon *weapon, bool bBlinkMoney)
{
	// Ensure that the weapon uses ammo
	if (weapon->m_iPrimaryAmmoType <= 0 || weapon->m_iPrimaryAmmoType >= AMMO_MAXTYPE)
		return false;

	// Can only buy if the player does not already have full ammo
	if (!CSGameRules()->CanHaveAmmo(pPlayer, weapon->m_iPrimaryAmmoType))
		return false;

	// Purchase the ammo if the player has enough money
	if (pPlayer->m_iAccount >= weapon->m_pAmmoInfo->m_iCostPerBox)
	{
		if (!pPlayer->GiveAmmo(weapon->m_pAmmoInfo->m_iCountPerBox, weapon->m_iPrimaryAmmoType))
			return false;

		EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "items/9mmclip1.wav", VOL_NORM, ATTN_NORM);

		pPlayer->AddAccount(-weapon->m_pAmmoInfo->m_iCostPerBox, RT_PLAYER_BOUGHT_SOMETHING);
		return true;
	}

	if (bBlinkMoney)
	{
		if (g_bClientPrintEnable)
		{
			// Not enough money.. let the player know
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Not_Enough_Money");
			BlinkAccount(pPlayer);
		}
	}

	return false;
}

bool BuyAmmo(CBasePlayer *pPlayer, int nSlot, bool bBlinkMoney)
{
	if (nSlot < PRIMARY_WEAPON_SLOT || nSlot > PISTOL_SLOT)
		return false;

	// Buy one ammo clip for all weapons in the given slot
	//
	// nSlot == 1 : Primary weapons
	// nSlot == 2 : Secondary weapons

	CBaseWeapon *pItem = pPlayer->m_rgpPlayerItems[nSlot];

	if (pPlayer->HasShield())
	{
		if (pPlayer->m_rgpPlayerItems[PISTOL_SLOT])
			pItem = pPlayer->m_rgpPlayerItems[PISTOL_SLOT];
	}

	if (pItem)
	{
		while (BuyGunAmmo(pPlayer, pItem, bBlinkMoney))
		{
			/* this is for original CBasePlayerItem
			pItem = pItem->m_pNext;

			if (!pItem)
			{
				return true;
			}
			*/
		}
	}

	return false;
}

CBaseEntity *EntityFromUserID(int userID)
{
	CBaseEntity *pTempEntity = nullptr;
	while ((pTempEntity = UTIL_FindEntityByClassname(pTempEntity, "player")))
	{
		if (FNullEnt(pTempEntity->edict()))
			break;

		CBasePlayer *pTempPlayer = GetClassPtr((CBasePlayer *)pTempEntity->pev);

		if (pTempPlayer->m_iTeam != UNASSIGNED && userID == GETPLAYERUSERID(pTempEntity->edict()))
		{
			return pTempPlayer;
		}
	}

	return nullptr;
}

NOXREF int CountPlayersInServer()
{
	int count = 0;
	CBaseEntity *pTempEntity = nullptr;
	while ((pTempEntity = UTIL_FindEntityByClassname(pTempEntity, "player")))
	{
		if (FNullEnt(pTempEntity->edict()))
			break;

		CBasePlayer *pTempPlayer = GetClassPtr((CBasePlayer *)pTempEntity->pev);

		if (pTempPlayer->m_iTeam != UNASSIGNED)
		{
			count++;
		}
	}

	return count;
}

struct RadioStruct
{
	int slot;
	void (*func)(CBasePlayer *, int);
	const char *alias;

} radioInfo[] = {
	{ 1, Radio1, "coverme" },
	{ 2, Radio1, "takepoint" },
	{ 3, Radio1, "holdpos" },
	{ 4, Radio1, "regroup" },
	{ 5, Radio1, "followme" },
	{ 6, Radio1, "takingfire" },

	{ 1, Radio2, "go" },
	{ 2, Radio2, "fallback" },
	{ 3, Radio2, "sticktog" },
	{ 4, Radio2, "getinpos" },
	{ 5, Radio2, "stormfront" },
	{ 6, Radio2, "report" },

	{ 1, Radio3, "roger" },
	{ 2, Radio3, "enemyspot" },
	{ 3, Radio3, "needbackup" },
	{ 4, Radio3, "sectorclear" },
	{ 5, Radio3, "inposition" },
	{ 6, Radio3, "reportingin" },
	{ 7, Radio3, "getout" },
	{ 8, Radio3, "negative" },
	{ 9, Radio3, "enemydown" },
};

BOOL HandleRadioAliasCommands(CBasePlayer *pPlayer, const char *pszCommand)
{
	for (auto& radio : radioInfo)
	{
		if (FStrEq(pszCommand, radio.alias))
		{
			radio.func(pPlayer, radio.slot);
			return TRUE;
		}
	}

	return FALSE;
}

void EXT_FUNC InternalCommand(edict_t *pEntity, const char *pcmd, const char *parg1)
{
	const char *pstr = nullptr;
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pev);

	if (FStrEq(pcmd, "say"))
	{
		if (gpGlobals->time >= pPlayer->m_flLastCommandTime[CMD_SAY])
		{
			pPlayer->m_flLastCommandTime[CMD_SAY] = gpGlobals->time + 0.3f;
			Host_Say(pEntity, FALSE);
		}
	}
	else if (FStrEq(pcmd, "say_team"))
	{
		if (gpGlobals->time >= pPlayer->m_flLastCommandTime[CMD_SAYTEAM])
		{
			pPlayer->m_flLastCommandTime[CMD_SAYTEAM] = gpGlobals->time + 0.3f;
			Host_Say(pEntity, TRUE);
		}
	}
	else if (FStrEq(pcmd, "fullupdate"))
	{
		if (gpGlobals->time >= pPlayer->m_flLastCommandTime[CMD_FULLUPDATE])
		{
			pPlayer->m_flLastCommandTime[CMD_FULLUPDATE] = gpGlobals->time + 0.6f;
			pPlayer->ForceClientDllUpdate();
		}
	}
	else if (FStrEq(pcmd, "vote"))
	{
		if (gpGlobals->time >= pPlayer->m_flLastCommandTime[CMD_VOTE])
		{
			pPlayer->m_flLastCommandTime[CMD_VOTE] = gpGlobals->time + 0.3f;

			if (gpGlobals->time < pPlayer->m_flNextVoteTime)
			{
				ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Wait_3_Seconds");
				return;
			}

			pPlayer->m_flNextVoteTime = gpGlobals->time + 3;

			if (pPlayer->m_iTeam != UNASSIGNED)
			{
				int iVoteID;
				bool bVoteFail = false;
				int iNumArgs = CMD_ARGC_();
				int iVoteLength = Q_strlen(parg1);
				if (iNumArgs != 2 || iVoteLength <= 0 || iVoteLength > 6)
				{
					bVoteFail = true;
				}

				iVoteID = Q_atoi(parg1);
				if (iVoteID <= 0)
				{
					bVoteFail = true;
				}

				if (bVoteFail)
				{
					ListPlayers(pPlayer);
					ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Game_vote_usage");
					return;
				}

				if (CountTeamPlayers(pPlayer->m_iTeam) < 3)
				{
					ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Cannot_Vote_With_Less_Than_Three");
					return;
				}

				CBaseEntity *pKickEntity = EntityFromUserID(iVoteID);
				if (pKickEntity)
				{
					CBasePlayer *pKickPlayer = GetClassPtr((CBasePlayer *)pKickEntity->pev);

					if (pKickPlayer->m_iTeam != pPlayer->m_iTeam)
					{
						ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Game_vote_players_on_your_team");
						return;
					}

					if (pKickPlayer == pPlayer)
					{
						ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Game_vote_not_yourself");
						return;
					}

					ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Game_vote_cast", UTIL_dtos1(iVoteID));
					pPlayer->m_iCurrentKickVote = iVoteID;
					ProcessKickVote(pPlayer, pKickPlayer);
				}
				else
				{
					ListPlayers(pPlayer);
					ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Game_vote_player_not_found", UTIL_dtos1(iVoteID));
				}
			}
		}
	}
	else if (FStrEq(pcmd, "listmaps"))
	{
		if (gpGlobals->time >= pPlayer->m_flLastCommandTime[CMD_LISTMAPS])
		{
			pPlayer->m_flLastCommandTime[CMD_LISTMAPS] = gpGlobals->time + 0.3f;
			CSGameRules()->DisplayMaps(pPlayer, 0);
		}
	}
	else if (FStrEq(pcmd, "votemap"))
	{
		if (gpGlobals->time >= pPlayer->m_flLastCommandTime[CMD_VOTEMAP])
		{
			pPlayer->m_flLastCommandTime[CMD_VOTEMAP] = gpGlobals->time + 0.3f;

			if (gpGlobals->time < pPlayer->m_flNextVoteTime)
			{
				ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Wait_3_Seconds");
				return;
			}

			pPlayer->m_flNextVoteTime = gpGlobals->time + 3;

			if (pPlayer->m_iTeam != UNASSIGNED)
			{
				if (gpGlobals->time < 180)
				{
					ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Cannot_Vote_Map");
					return;
				}

				bool bFailed = false;
				int iNumArgs = CMD_ARGC_();
				int iVoteLength = Q_strlen(parg1);
				if (iNumArgs != 2 || iVoteLength > 5)
				{
					bFailed = true;
				}

				int iVoteID = Q_atoi(parg1);
				if (iVoteID < 1 || iVoteID > MAX_VOTE_MAPS)
				{
					bFailed = true;
				}

				if (iVoteID > GetMapCount())
				{
					bFailed = true;
				}

				if (bFailed)
				{
					CSGameRules()->DisplayMaps(pPlayer, 0);
					ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Game_votemap_usage");
					return;
				}

				if (CountTeamPlayers(pPlayer->m_iTeam) < 2)
				{
					ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Cannot_Vote_Need_More_People");
					return;
				}

				if (pPlayer->m_iMapVote)
				{
					if (--CSGameRules()->m_iMapVotes[pPlayer->m_iMapVote] < 0)
					{
						CSGameRules()->m_iMapVotes[pPlayer->m_iMapVote] = 0;
					}
				}

				ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "#Game_voted_for_map", UTIL_dtos1(iVoteID));
				pPlayer->m_iMapVote = iVoteID;
				CSGameRules()->ProcessMapVote(pPlayer, iVoteID);
			}
		}
	}
	else if (FStrEq(pcmd, "timeleft"))
	{
		if (gpGlobals->time > pPlayer->m_iTimeCheckAllowed)
		{
			pPlayer->m_iTimeCheckAllowed = int(gpGlobals->time + 1.0f);

			if (!timelimit.value)
			{
				ClientPrint(pPlayer->pev, HUD_PRINTTALK, "#Game_no_timelimit");
				return;
			}

			int timeRemaining = (int)CSGameRules()->GetTimeLeft();
			if (timeRemaining < 0)
				timeRemaining = 0;

			int iMinutes = timeRemaining / 60;
			int iSeconds = timeRemaining % 60;

			char secs[5];
			char *temp = UTIL_dtos2(iSeconds);

			if (iSeconds >= 10)
			{
				secs[0] = temp[0];
				secs[1] = temp[1];
				secs[2] = '\0';
			}
			else
			{
				secs[0] = '0';
				secs[1] = temp[0];
				secs[2] = '\0';
			}

			ClientPrint(pPlayer->pev, HUD_PRINTTALK, "#Game_timelimit", UTIL_dtos1(iMinutes), secs);
		}
	}
	else if (FStrEq(pcmd, "listplayers"))
	{
		if (gpGlobals->time >= pPlayer->m_flLastCommandTime[CMD_LISTPLAYERS])
		{
			pPlayer->m_flLastCommandTime[CMD_LISTPLAYERS] = gpGlobals->time + 0.3f;
			ListPlayers(pPlayer);
		}
	}
	else if (FStrEq(pcmd, "client_buy_open") || FStrEq(pcmd, "buy3"))
	{
		if (pPlayer->m_signals.GetState() & SIGNAL_BUY)
		{
			OpenMenu_Buy3(pPlayer);
		}
		else
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgBuyClose, nullptr, pPlayer->pev);
			MESSAGE_END();
		}
	}
	else if (FStrEq(pcmd, "client_buy_close"))
	{
		if (pPlayer->m_iMenu == Menu_Buy3)
		{
			pPlayer->m_iMenu = Menu_OFF;
		}
	}
	else if (FStrEq(pcmd, "menuselect"))
	{
		int slot = Q_atoi(parg1);
		if (pPlayer->m_iJoiningState == JOINED || pPlayer->m_iMenu != Menu_ChooseTeam)
		{
			if (slot == 10)
			{
				pPlayer->m_iMenu = Menu_OFF;
			}
		}

		auto canOpenOldMenu = [pPlayer]()-> bool {
			return pPlayer->m_bVGUIMenus == false;
		};

		switch (pPlayer->m_iMenu)
		{
			case Menu_OFF:
				break;

			case Menu_ChooseTeam:
			{
				if (canOpenOldMenu() && !HandleMenu_ChooseTeam(pPlayer, slot))
				{
					pPlayer->m_iMenu = Menu_ChooseTeam;
					if (pPlayer->m_iJoiningState == JOINED)
						ShowVGUIMenu(pPlayer, VGUIMenu::TEAM_IG);
					else
						ShowVGUIMenu(pPlayer, VGUIMenu::TEAM);
				}
				break;
			}
			case Menu_IGChooseTeam:
			{
				if (canOpenOldMenu())
					HandleMenu_ChooseTeam(pPlayer, slot);

				break;
			}
			case Menu_Radio1:
			{
				Radio1(pPlayer, slot);
				break;
			}
			case Menu_Radio2:
			{
				Radio2(pPlayer, slot);
				break;
			}
			case Menu_Radio3:
			{
				Radio3(pPlayer, slot);
				break;
			}
			case Menu_DeclareRole:
			{
				if (!MenuHandler_DeclareRole(pPlayer, slot))
					OpenMenu_DeclareRole(pPlayer);

				break;
			}
			case Menu_VoteTS:
			{
				if (!MenuHandler_VoteTacticalSchemes(pPlayer, slot))
					OpenMenu_VoteTacticalSchemes(pPlayer);

				break;
			}
			case Menu_Buy3:
			{
				MenuHandler_Buy3(pPlayer, slot);
				break;
			}
			case Menu_BuyPistols:
			{
				if (!MenuHandler_BuyPistols(pPlayer, slot))
					MenuHandler_Buy3(pPlayer, BuyMenu_BuyPistols);

				break;
			}
			case Menu_BuyShotguns:
			{
				if (!MenuHandler_BuyShotguns(pPlayer, slot))
					MenuHandler_Buy3(pPlayer, BuyMenu_BuyShotguns);

				break;
			}
			case Menu_BuySMGs:
			{
				if (!MenuHandler_BuySMGs(pPlayer, slot))
					MenuHandler_Buy3(pPlayer, BuyMenu_BuySMGs);

				break;
			}
			case Menu_BuyAssaultFirearms:
			{
				if (!MenuHandler_BuyAssaultFirearms(pPlayer, slot))
					MenuHandler_Buy3(pPlayer, BuyMenu_BuyAssaultFirearms);

				break;
			}
			case Menu_BuySniperRifle:
			{
				if (!MenuHandler_BuySniperRifles(pPlayer, slot))
					MenuHandler_Buy3(pPlayer, BuyMenu_BuySniperRifles);

				break;
			}
			case Menu_BuyEquipments:
			{
				if (!MenuHandler_BuyEquipments(pPlayer, slot))
					MenuHandler_Buy3(pPlayer, BuyMenu_BuyEquipments);

				break;
			}
			default:
				ALERT(at_console, "ClientCommand(): Invalid menu selected\n");
				break;
		}
	}
	else if (FStrEq(pcmd, "chooseteam"))
	{
		if (pPlayer->m_bTeamChanged)
		{
			if (pPlayer->pev->deadflag != DEAD_NO)
			{
				ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Only_1_Team_Change");
				return;
			}
		}

		pPlayer->m_iMenu = Menu_ChooseTeam;

		if (CSGameRules()->IsFreezePeriod() || pPlayer->pev->deadflag != DEAD_NO)
			ShowVGUIMenu(pPlayer, VGUIMenu::TEAM_IG);
		else
			ShowVGUIMenu(pPlayer, VGUIMenu::TEAM_NO_SPEC_IG);
	}
	else if (FStrEq(pcmd, "showbriefing"))
	{
		if (pPlayer->m_iMenu == Menu_OFF)
		{
			if (g_szMapBriefingText[0] != '\0')
			{
				if (pPlayer->m_iTeam != UNASSIGNED && !(pPlayer->m_afPhysicsFlags & PFLAG_OBSERVER))
				{
					pPlayer->MenuPrint(g_szMapBriefingText);
					pPlayer->m_bMissionBriefing = true;
				}
			}
		}
	}
	else if (FStrEq(pcmd, "ignoremsg"))
	{
		if (pPlayer->m_iIgnoreGlobalChat == IGNOREMSG_NONE)
		{
			pPlayer->m_iIgnoreGlobalChat = IGNOREMSG_ENEMY;
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Ignore_Broadcast_Messages");
		}
		else if (pPlayer->m_iIgnoreGlobalChat == IGNOREMSG_ENEMY)
		{
			pPlayer->m_iIgnoreGlobalChat = IGNOREMSG_TEAM;
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Ignore_Broadcast_Team_Messages");
		}
		else if (pPlayer->m_iIgnoreGlobalChat == IGNOREMSG_TEAM)
		{
			pPlayer->m_iIgnoreGlobalChat = IGNOREMSG_NONE;
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Accept_All_Messages");
		}
	}
	else if (FStrEq(pcmd, "ignorerad"))
	{
		pPlayer->m_bIgnoreRadio = !pPlayer->m_bIgnoreRadio;
		ClientPrint(pPlayer->pev, HUD_PRINTCENTER, pPlayer->m_bIgnoreRadio ? "#Ignore_Radio" : "#Accept_Radio");
	}
	else if (FStrEq(pcmd, "spectate") && (pPlayer->pev->flags & FL_PROXY)) // always allow proxies to become a spectator
	{
		// clients wants to become a spectator
		HandleMenu_ChooseTeam(pPlayer, MENU_SLOT_TEAM_SPECT);
	}
	else if (FStrEq(pcmd, "specmode"))
	{
		// new spectator mode
		int mode = Q_atoi(parg1);
		if (pPlayer->GetObserverMode() != OBS_NONE && pPlayer->CanSwitchObserverModes())
			pPlayer->Observer_SetMode(mode);
		else
			pPlayer->m_iObserverLastMode = mode;
	}
	else if (FStrEq(pcmd, "spec_set_ad"))
	{
		float val = Q_atof(parg1);
		pPlayer->SetObserverAutoDirector(val > 0.0f);
	}
	else if (FStrEq(pcmd, "follownext"))
	{
		// follow next player
		int arg = Q_atoi(parg1);
		if (pPlayer->GetObserverMode() != OBS_NONE && pPlayer->CanSwitchObserverModes())
		{
			pPlayer->Observer_FindNextPlayer(arg != 0);
		}
	}
	else if (FStrEq(pcmd, "follow"))
	{
		if (pPlayer->GetObserverMode() != OBS_NONE && pPlayer->CanSwitchObserverModes())
		{
			pPlayer->Observer_FindNextPlayer(false, parg1);
		}
	}
	else if (FStrEq(pcmd, "cl_setautobuy"))
	{
	}
	else if (FStrEq(pcmd, "cl_setrebuy"))
	{
		if (pPlayer->pev->deadflag != DEAD_NO)
			return;

		pPlayer->SaveRebuy();
	}
	else
	{
		if (CSGameRules()->ClientCommand_DeadOrAlive(GetClassPtr((CBasePlayer *)pev), pcmd))
			return;

		if (TheBots)
		{
			if (TheBots->ClientCommand(GetClassPtr((CBasePlayer *)pev), pcmd))
				return;
		}

		if (FStrEq(pcmd, "mp_debug"))
		{
			UTIL_SetDprintfFlags(parg1);
		}
		else if (FStrEq(pcmd, "jointeam"))
		{
			int slot = Q_atoi(parg1);
			if (HandleMenu_ChooseTeam(pPlayer, slot))
			{
				if (slot == MENU_SLOT_TEAM_SPECT)
				{
					pPlayer->ResetMenu();
				}
			}
			else
			{
				pPlayer->m_iMenu = Menu_ChooseTeam;
				if (pPlayer->m_iJoiningState == JOINED)
					ShowVGUIMenu(pPlayer, VGUIMenu::TEAM_NO_SPEC_IG);
				else
					ShowVGUIMenu(pPlayer, VGUIMenu::TEAM_NO_SPEC);
			}
		}
		else if (FStrEq(pcmd, "joinclass"))	// Command abolished.
		{
			ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#Command_Not_Available");
			return;
		}
		else if (pPlayer->pev->deadflag == DEAD_NO)
		{
			if (FStrEq(pcmd, "nightvision"))
			{
				if (gpGlobals->time >= pPlayer->m_flLastCommandTime[CMD_NIGHTVISION])
				{
					pPlayer->m_flLastCommandTime[CMD_NIGHTVISION] = gpGlobals->time + 0.3f;

					if (!pPlayer->m_bHasNightVision)
						return;

					if (pPlayer->m_bNightVisionOn)
					{
						EMIT_SOUND(ENT(pPlayer->pev), CHAN_ITEM, "items/nvg_off.wav", RANDOM_FLOAT(0.92, 1), ATTN_NORM);

						MESSAGE_BEGIN(MSG_ONE, gmsgNVGToggle, nullptr, pPlayer->pev);
							WRITE_BYTE(0); // disable nightvision
						MESSAGE_END();

						pPlayer->m_bNightVisionOn = false;

						for (int i = 1; i <= gpGlobals->maxClients; i++)
						{
							CBasePlayer *pObserver = UTIL_PlayerByIndex(i);
							if (pObserver && pObserver->IsObservingPlayer(pPlayer))
							{
								EMIT_SOUND(ENT(pObserver->pev), CHAN_ITEM, "items/nvg_off.wav", RANDOM_FLOAT(0.92, 1), ATTN_NORM);

								MESSAGE_BEGIN(MSG_ONE, gmsgNVGToggle, nullptr, pObserver->pev);
									WRITE_BYTE(0); // disable nightvision
								MESSAGE_END();

								pObserver->m_bNightVisionOn = false;
							}
						}
					}
					else
					{
						EMIT_SOUND(ENT(pPlayer->pev), CHAN_ITEM, "items/nvg_on.wav", RANDOM_FLOAT(0.92, 1), ATTN_NORM);

						MESSAGE_BEGIN(MSG_ONE, gmsgNVGToggle, nullptr, pPlayer->pev);
							WRITE_BYTE(1); // enable nightvision
						MESSAGE_END();

						pPlayer->m_bNightVisionOn = true;

						for (int i = 1; i <= gpGlobals->maxClients; i++)
						{
							CBasePlayer *pObserver = UTIL_PlayerByIndex(i);
							if (pObserver && pObserver->IsObservingPlayer(pPlayer))
							{
								EMIT_SOUND(ENT(pObserver->pev), CHAN_ITEM, "items/nvg_on.wav", RANDOM_FLOAT(0.92, 1), ATTN_NORM);

								MESSAGE_BEGIN(MSG_ONE, gmsgNVGToggle, nullptr, pObserver->pev);
									WRITE_BYTE(1);  // enable nightvision
								MESSAGE_END();

								pObserver->m_bNightVisionOn = true;
							}
						}
					}
				}
			}
			else if (FStrEq(pcmd, "radio1"))
			{
				ShowMenu(pPlayer, (MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_5 | MENU_KEY_6 | MENU_KEY_0), -1, "#RadioA");
				pPlayer->m_iMenu = Menu_Radio1;
			}
			else if (FStrEq(pcmd, "radio2"))
			{
				ShowMenu(pPlayer, (MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_5 | MENU_KEY_6 | MENU_KEY_0), -1, "#RadioB");
				pPlayer->m_iMenu = Menu_Radio2;
				return;
			}
			else if (FStrEq(pcmd, "radio3"))
			{
				ShowMenu(pPlayer, (MENU_KEY_1 | MENU_KEY_2 | MENU_KEY_3 | MENU_KEY_4 | MENU_KEY_5 | MENU_KEY_6 | MENU_KEY_7 | MENU_KEY_8 | MENU_KEY_9 | MENU_KEY_0), -1, "#RadioC");
				pPlayer->m_iMenu = Menu_Radio3;
			}
			else if (FStrEq(pcmd, "drop"))
			{
				// player is dropping an item.
				if (pPlayer->HasShield())
				{
					pPlayer->DropShield();
				}
				else
					pPlayer->DropPlayerItem(WeaponClassnameToID(parg1));
			}
			else if (FStrEq(pcmd, "fov"))
			{
#if 0
				if (CVAR_GET_FLOAT("sv_cheats") != 0.0f && CMD_ARGC() > 1)
					GetClassPtr<CCSPlayer>((CBasePlayer *)pev)->m_iFOV = Q_atoi(CMD_ARGV(1));
				else
					CLIENT_PRINTF(pEntity, print_console, UTIL_VarArgs("\"fov\" is \"%d\"\n", int(GetClassPtr<CCSPlayer>((CBasePlayer *)pev)->m_iFOV)));
#endif
			}
			else if (FStrEq(pcmd, "use"))
			{
				GetClassPtr((CBasePlayer *)pev)->SelectItem(parg1);
			}
			else if (((pstr = Q_strstr(pcmd, "weapon_"))) && (pstr == pcmd))
			{
				GetClassPtr((CBasePlayer *)pev)->SelectItem(pcmd);
			}
			else if (FStrEq(pcmd, "lastinv"))
			{
				pPlayer->SelectLastItem();
			}
			else if (FStrEq(pcmd, "buyammo1"))
			{
				if (pPlayer->m_signals.GetState() & SIGNAL_BUY)
				{
					if (pPlayer->m_rgpPlayerItems[PRIMARY_WEAPON_SLOT])
						BuyGunAmmo(pPlayer, pPlayer->m_rgpPlayerItems[PRIMARY_WEAPON_SLOT]);	// buy only 1 box of ammo.
				}
			}
			else if (FStrEq(pcmd, "buyammo2"))
			{
				if (pPlayer->m_signals.GetState() & SIGNAL_BUY)
				{
					if (pPlayer->m_rgpPlayerItems[PISTOL_SLOT])
						BuyGunAmmo(pPlayer, pPlayer->m_rgpPlayerItems[PISTOL_SLOT]);	// buy only 1 box of ammo.
				}
			}
			else if (FStrEq(pcmd, "primammo"))
			{
				if (pPlayer->m_signals.GetState() & SIGNAL_BUY)
				{
					BuyAmmo(pPlayer, PRIMARY_WEAPON_SLOT);	// buy full ammo.
				}
			}
			else if (FStrEq(pcmd, "secammo"))
			{
				if (pPlayer->m_signals.GetState() & SIGNAL_BUY)
				{
					BuyAmmo(pPlayer, PISTOL_SLOT);	// buy full ammo.
				}
			}
			else if (FStrEq(pcmd, "buy"))
			{
				if (pPlayer->m_signals.GetState() & SIGNAL_BUY)
				{
					if (TheBots)
					{
						TheBots->OnEvent(EVENT_TUTOR_BUY_MENU_OPENNED);
					}
				}
			}
			else if (FStrEq(pcmd, "cl_autobuy"))
			{
				if (pPlayer->m_signals.GetState() & SIGNAL_BUY)
				{
					pPlayer->ParseAutoBuy();
				}
			}
			else if (FStrEq(pcmd, "cl_rebuy"))
			{
				if (pPlayer->m_signals.GetState() & SIGNAL_BUY)
				{
					pPlayer->ParseRebuy();
				}
			}
			else if (FStrEq(pcmd, "smartradio"))
			{
				pPlayer->SmartRadio();
			}
			else if (FStrEq(pcmd, "give"))
			{
				if (CMD_ARGC() > 1 && FStrnEq(parg1, "weapon_", sizeof("weapon_") - 1))
				{
					const auto pInfo = GetWeaponInfo(parg1);
					if (pInfo)
					{
						//pPlayer->GiveNamedItem(pInfo->m_pszInternalName);
						pPlayer->AddPlayerItem(CBaseWeapon::Give(pInfo->m_iId, pPlayer));
					}
				}
			}
			else if (FStrEq(pcmd, "executeskill"))
			{
				if (!Q_strlen(parg1))
				{
					for (auto& pSkill : pPlayer->m_rgpSkills)
					{
						if (pSkill)
							pSkill->Execute();
					}
				}
				else
				{
					int iSkillType = atoi(parg1);

					switch (iSkillType)
					{
					case SkillType_Attack:
					case SkillType_Defense:
					case SkillType_Auxiliary:
					case SkillType_UNASSIGNED:
					case SkillType_WeaponEnhance:
						if (pPlayer->m_rgpSkills[iSkillType])
							pPlayer->m_rgpSkills[iSkillType]->Execute();

						break;

					default:
						break;
					}
				}
			}
			else if (FStrEq(pcmd, "role"))
			{
				int iRole = Q_atoi(parg1);
				if (iRole >= Role_UNASSIGNED && iRole < ROLE_COUNT)
					pPlayer->AssignRole(RoleTypes(iRole));
			}
			else if (FStrEq(pcmd, "addmoney"))
			{
				int iMoney = Q_atoi(parg1);
				pPlayer->AddAccount(iMoney);
			}
			else if (FStrEq(pcmd, "declarerole"))
			{
				OpenMenu_DeclareRole(pPlayer);
			}
			else if (FStrEq(pcmd, "votescheme"))
			{
				OpenMenu_VoteTacticalSchemes(pPlayer);
			}
			else if (FStrEq(pcmd, "eqpselect"))
			{
				EquipmentIdType iId = (EquipmentIdType)Q_atoi(parg1);
				pPlayer->m_iUsingGrenadeId = iId;
				pPlayer->ResetUsingEquipment();	// it is not a reset, it just a check.
			}
#ifdef _DEBUG
			else if (FStrEq(pcmd, "dot.electrify"))
			{
				gElectrifiedDOTMgr::Set(pPlayer, atoi(parg1), pPlayer->pev->origin + pPlayer->pev->view_ofs);
			}
			else if (FStrEq(pcmd, "dot.poison"))
			{
				gPoisonDOTMgr::Set(pPlayer, pPlayer, atoi(parg1));
			}
			else if (FStrEq(pcmd, "dot.ignite"))
			{
				gBurningDOTMgr::Set(pPlayer, pPlayer, atoi(parg1));
			}
#endif
			else if (FStrEq(pcmd, "melee"))
			{
				if (pPlayer->m_pActiveItem)
					pPlayer->m_pActiveItem->Melee();
			}
			else if (FStrEq(pcmd, "showallweapons"))
			{
				for (auto pWeapon : CBaseWeapon::m_lstWeapons)
				{
					if (pWeapon->m_pPlayer.IsValid())
						SERVER_PRINT(SharedVarArgs("Wpn: %s, Owner: %s\n", pWeapon->m_pItemInfo->m_pszExternalName, STRING(pWeapon->m_pPlayer->pev->netname)));
					else
					{
						SERVER_PRINT(SharedVarArgs("Wpn: %s, Owner: CWeaponBox\n", pWeapon->m_pItemInfo->m_pszExternalName));
					}
				}
			}
			else if (FStrEq(pcmd, "buyweapon"))
			{
				WeaponIdType iId = WeaponClassnameToID(parg1);

				if (iId <= WEAPON_NONE || iId >= LAST_WEAPON)
					return;

				BuyWeapon(pPlayer, iId);
			}
			else if (FStrEq(pcmd, "changemode"))
			{
				if (pPlayer->m_pActiveItem)
					pPlayer->m_pActiveItem->AlterAct();
			}
#ifdef _DEBUG
			else if (FStrEq(pcmd, "origin.tp"))
			{
				Vector vecOrg = Vector(Q_atoi(CMD_ARGV(1)), Q_atoi(CMD_ARGV(2)), Q_atoi(CMD_ARGV(3)));
				SET_ORIGIN(pEntity, vecOrg);
			}
			else if (FStrEq(pcmd, "origin.shift"))
			{
				Vector vecOrg = Vector(Q_atoi(CMD_ARGV(1)), Q_atoi(CMD_ARGV(2)), Q_atoi(CMD_ARGV(3)));
				SET_ORIGIN(pEntity, pEntity->v.origin + vecOrg);
			}
#endif
			else if (FStrEq(pcmd, "detonate"))
			{
				CGrenade::C4_Detonate(pPlayer);
			}
			else if (FStrEq(pcmd, "__shoot"))
			{
				auto p = InterpretPrimaryAttackMessage();
				pPlayer->ClientRequestFireWeapon(p);

#ifdef _DEBUG_CUSTOM_CLIENT_TO_SERVER_MESSAGE
				auto szCommandString = std::format("[Received] {:d} {} {} {} {} {} {} {:d} {:d}\n",
					(int)p->m_iId,
					p->m_vecSrc.x, p->m_vecSrc.y, p->m_vecSrc.z,
					p->m_vecViewAngles.x, p->m_vecViewAngles.y, p->m_vecViewAngles.z,
					p->m_iClip,
					p->m_iRandomSeed
				);

				SERVER_PRINT(szCommandString.c_str());
#endif
			}
			else
			{
				if (HandleRadioAliasCommands(pPlayer, pcmd))
					return;

				if (!g_pGameRules->ClientCommand(GetClassPtr((CBasePlayer *)pev), pcmd))
				{
					// tell the user they entered an unknown command
					char command[128];

					// check the length of the command (prevents crash)
					// max total length is 192 ...and we're adding a string below ("Unknown command: %s\n")
					Q_strncpy(command, pcmd, sizeof(command) - 1);
					command[sizeof(command) - 1] = '\0';

					// Add extra '\n' to make command string safe
					// This extra '\n' is removed by the client, so it is ok
					command[sizeof(command) - 2] = '\0';
					command[Q_strlen(command)] = '\n';

					// tell the user they entered an unknown command
					ClientPrint(&pEntity->v, HUD_PRINTCONSOLE, "#Game_unknown_command", command);
				}
			}
		}
	}
}

// Use CMD_ARGV, CMD_ARGV, and CMD_ARGC to get pointers the character string command.
void EXT_FUNC ClientCommand_(edict_t *pEntity)
{
	// Is the client spawned yet?
	if (!pEntity->pvPrivateData)
		return;

	static char command[128] = "";
	Q_strncpy(command, CMD_ARGV_(0), sizeof command - 1);
	command[sizeof command - 1] = '\0';

	InternalCommand(pEntity, command, CMD_ARGV_(1));
}

// called after the player changes userinfo - gives dll a chance to modify it before it gets sent into the rest of the engine.
void EXT_FUNC ClientUserInfoChanged(edict_t *pEntity, char *infobuffer)
{
	// Is the client spawned yet?
	if (!pEntity->pvPrivateData)
		return;

	CBasePlayer *pPlayer = CBasePlayer::Instance(pEntity);
	char *szBufferName = GET_KEY_VALUE(infobuffer, "name");

	// msg everyone if someone changes their name, and it isn't the first time (changing no name to current name)
	if (pEntity->v.netname && STRING(pEntity->v.netname)[0] != '\0' && !FStrEq(STRING(pEntity->v.netname), szBufferName))
	{
		char szName[32];
		Q_snprintf(szName, sizeof(szName), "%s", szBufferName);

		// First parse the name and remove any %'s
		for (char *pPct = szName; pPct && *pPct != '\0'; pPct++)
		{
			// Replace it with a space
			if (*pPct == '%' || *pPct == '&')
				*pPct = ' ';
		}

		if (szName[0] == '#')
			szName[0] = '*';

		// Can set it a new name?
		if (!pPlayer->SetClientUserInfoName(infobuffer, szName))
		{
			// so to back old name into buffer
			SET_CLIENT_KEY_VALUE(pPlayer->entindex(), infobuffer, "name", (char *)STRING(pPlayer->pev->netname));
		}
	}

	// was already checking on pvPrivateData
	g_pGameRules->ClientUserInfoChanged(pPlayer, infobuffer);
}

void EXT_FUNC ServerDeactivate()
{
	// It's possible that the engine will call this function more times than is necessary
	// Therefore, only run it one time for each call to ServerActivate
	if (!g_bServerActive)
		return;

	g_bServerActive = false;

	// Peform any shutdown operations here...
	g_pGameRules->ServerDeactivate();

	if (TheBots)
	{
		TheBots->ServerDeactivate();
	}
}

#define SetCStrikeFlags_SIG	"\xA1\x2A\x2A\x2A\x2A\x85\xC0\x0F\x85\x2A\x2A\x2A\x2A\x56\x68\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08"

void EXT_FUNC ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
	int i;
	CBaseEntity *pClass;

	// Patch the game.
	// (Fake that we are czero)
	// (For we can use pev->gamestate to enable/disable shield)
	HMODULE hEngine = GetModuleHandle("hw.dll");
	if (!hEngine)
		hEngine = GetModuleHandle("swds.dll");	// Via HLDS.exe

	DWORD addr = (DWORD)MH_SearchPattern((void*)MH_GetModuleBase(hEngine), MH_GetModuleSize(hEngine), SetCStrikeFlags_SIG, sizeof(SetCStrikeFlags_SIG) - 1U);
	addr += (DWORD)0x5D;
	addr += (DWORD)0x2;
	**(BOOL**)addr = TRUE;	// g_bIsCZero

	// Every call to ServerActivate should be matched by a call to ServerDeactivate
	g_bServerActive = true;
	EmptyEntityHashTable();

	// Clients have not been initialized yet
	for (i = 0; i < edictCount; i++)
	{
		edict_t *pEdict = &pEdictList[i];

		if (pEdict->free)
			continue;

		// Clients aren't necessarily initialized until ClientPutInServer()
		if (i < clientMax || !pEdict->pvPrivateData)
			continue;

		pClass = CBaseEntity::Instance(pEdict);

		// Activate this entity if it's got a class & isn't dormant
		if (pClass && !pClass->IsDormant())
		{
			AddEntityHashValue(&pEdict->v, STRING(pEdict->v.classname), CLASSNAME);
			pClass->Activate();
		}
		else
		{
			ALERT(at_console, "Can't instance %s\n", STRING(pEdict->v.classname));
		}
	}

	// Link user messages here to make sure first client can get them...
	LinkUserMessages();
	WriteSigonMessages();

	if (g_pGameRules)
	{
		g_pGameRules->CheckMapConditions();
	}

	if (TheBots)
	{
		TheBots->ServerActivate();
	}

	CSGameRules()->ServerActivate();
}

void EXT_FUNC PlayerPreThink(edict_t *pEntity)
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (pPlayer)
	{
		pPlayer->PreThink();
	}
}

void EXT_FUNC PlayerPostThink(edict_t *pEntity)
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (pPlayer)
	{
		pPlayer->PostThink();
	}
}

void EXT_FUNC ParmsNewLevel()
{
	;
}

void EXT_FUNC ParmsChangeLevel()
{
	// retrieve the pointer to the save data
	SAVERESTOREDATA *pSaveData = (SAVERESTOREDATA *)gpGlobals->pSaveData;

	if (pSaveData)
	{
		pSaveData->connectionCount = BuildChangeList(pSaveData->levelList, MAX_LEVEL_CONNECTIONS);
	}
}

//static float g_flLastServerTime = 0.0f;	// You can't and shouldn't use it outside of StartFrame().
double g_flTrueServerFrameRate = 0.0;

void EXT_FUNC StartFrame()
{
	// LUNA: Credits to Crsky.
	// If you run the client.dll standalone, the cmd->msec is UTTERLY wrong.
	// You will have to calculate it manually.
	// On server side, manually calculate the change of gpGlobals->time.
	// On client side, manually calculate the change of "time" parameter from ExportFunc::HUD_PostRunCmd().

	//g_flTrueServerFrameRate = gpGlobals->time - g_flLastServerTime;
	//g_flLastServerTime = gpGlobals->time;
	static auto last_hr_clock = std::chrono::high_resolution_clock::now();	// init only.
	auto dur = std::chrono::high_resolution_clock::now() - last_hr_clock;
	last_hr_clock = std::chrono::high_resolution_clock::now();
	g_flTrueServerFrameRate = std::chrono::duration_cast<std::chrono::microseconds>(dur).count() / 1000000.0;	// Method offered by Crsky.

	/*
	MESSAGE_BEGIN(MSG_ALL, gmsgFrameRate);
	WRITE_LONG(*(int*)&g_flTrueServerFrameRate);
	MESSAGE_END();
	*/

	if (g_pGameRules)
	{
		g_pGameRules->Think();

		if (g_pGameRules->IsGameOver())
			return;
	}

	gpGlobals->teamplay = 1.0f;

	if (TheBots)
	{
		TheBots->StartFrame();
	}

	if (!CBaseWeapon::m_lstWeapons.empty())
	{
		CBaseWeapon::TheWeaponsThink();
	}

	EndFrame();
}

void EndFrame()
{
	TriggerSetOriginManager::Update();
}

void ClientPrecache()
{
	int i;

	PRECACHE_SOUND("weapons/dryfire_pistol.wav");
	PRECACHE_SOUND("weapons/dryfire_rifle.wav");
	PRECACHE_SOUND("player/pl_shot1.wav");
	PRECACHE_SOUND("player/pl_die1.wav");
	PRECACHE_SOUND("player/headshot1.wav");
	PRECACHE_SOUND("player/headshot2.wav");
	PRECACHE_SOUND("player/headshot3.wav");
	PRECACHE_SOUND("player/bhit_flesh-1.wav");
	PRECACHE_SOUND("player/bhit_flesh-2.wav");
	PRECACHE_SOUND("player/bhit_flesh-3.wav");
	PRECACHE_SOUND("player/bhit_kevlar-1.wav");
	PRECACHE_SOUND("player/bhit_helmet-1.wav");
	PRECACHE_SOUND("player/die1.wav");
	PRECACHE_SOUND("player/die2.wav");
	PRECACHE_SOUND("player/die3.wav");
	PRECACHE_SOUND("player/death6.wav");
	PRECACHE_SOUND("radio/locknload.wav");
	PRECACHE_SOUND("radio/letsgo.wav");
	PRECACHE_SOUND("radio/moveout.wav");
	PRECACHE_SOUND("radio/com_go.wav");
	PRECACHE_SOUND("radio/rescued.wav");
	PRECACHE_SOUND("radio/rounddraw.wav");
	PRECACHE_SOUND("items/kevlar.wav");
	PRECACHE_SOUND("items/ammopickup2.wav");
	PRECACHE_SOUND("items/nvg_on.wav");
	PRECACHE_SOUND("items/nvg_off.wav");
	PRECACHE_SOUND("items/equip_nvg.wav");
	PRECACHE_SOUND("weapons/c4_beep1.wav");
	PRECACHE_SOUND("weapons/c4_beep2.wav");
	PRECACHE_SOUND("weapons/c4_beep3.wav");
	PRECACHE_SOUND("weapons/c4_beep4.wav");
	PRECACHE_SOUND("weapons/c4_beep5.wav");
	PRECACHE_SOUND("weapons/c4_explode1.wav");
	PRECACHE_SOUND("weapons/c4_plant.wav");
	PRECACHE_SOUND("weapons/c4_disarm.wav");
	PRECACHE_SOUND("weapons/c4_disarmed.wav");
	PRECACHE_SOUND("weapons/explode3.wav");
	PRECACHE_SOUND("weapons/explode4.wav");
	PRECACHE_SOUND("weapons/explode5.wav");
	PRECACHE_SOUND("player/sprayer.wav");
	PRECACHE_SOUND("player/pl_fallpain2.wav");
	PRECACHE_SOUND("player/pl_fallpain3.wav");
	PRECACHE_SOUND("player/pl_snow1.wav");
	PRECACHE_SOUND("player/pl_snow2.wav");
	PRECACHE_SOUND("player/pl_snow3.wav");
	PRECACHE_SOUND("player/pl_snow4.wav");
	PRECACHE_SOUND("player/pl_snow5.wav");
	PRECACHE_SOUND("player/pl_snow6.wav");
	PRECACHE_SOUND("player/pl_step1.wav");
	PRECACHE_SOUND("player/pl_step2.wav");
	PRECACHE_SOUND("player/pl_step3.wav");
	PRECACHE_SOUND("player/pl_step4.wav");
	PRECACHE_SOUND("common/npc_step1.wav");
	PRECACHE_SOUND("common/npc_step2.wav");
	PRECACHE_SOUND("common/npc_step3.wav");
	PRECACHE_SOUND("common/npc_step4.wav");
	PRECACHE_SOUND("player/pl_metal1.wav");
	PRECACHE_SOUND("player/pl_metal2.wav");
	PRECACHE_SOUND("player/pl_metal3.wav");
	PRECACHE_SOUND("player/pl_metal4.wav");
	PRECACHE_SOUND("player/pl_dirt1.wav");
	PRECACHE_SOUND("player/pl_dirt2.wav");
	PRECACHE_SOUND("player/pl_dirt3.wav");
	PRECACHE_SOUND("player/pl_dirt4.wav");
	PRECACHE_SOUND("player/pl_duct1.wav");
	PRECACHE_SOUND("player/pl_duct2.wav");
	PRECACHE_SOUND("player/pl_duct3.wav");
	PRECACHE_SOUND("player/pl_duct4.wav");
	PRECACHE_SOUND("player/pl_grate1.wav");
	PRECACHE_SOUND("player/pl_grate2.wav");
	PRECACHE_SOUND("player/pl_grate3.wav");
	PRECACHE_SOUND("player/pl_grate4.wav");
	PRECACHE_SOUND("player/pl_slosh1.wav");
	PRECACHE_SOUND("player/pl_slosh2.wav");
	PRECACHE_SOUND("player/pl_slosh3.wav");
	PRECACHE_SOUND("player/pl_slosh4.wav");
	PRECACHE_SOUND("player/pl_tile1.wav");
	PRECACHE_SOUND("player/pl_tile2.wav");
	PRECACHE_SOUND("player/pl_tile3.wav");
	PRECACHE_SOUND("player/pl_tile4.wav");
	PRECACHE_SOUND("player/pl_tile5.wav");
	PRECACHE_SOUND("player/pl_swim1.wav");
	PRECACHE_SOUND("player/pl_swim2.wav");
	PRECACHE_SOUND("player/pl_swim3.wav");
	PRECACHE_SOUND("player/pl_swim4.wav");
	PRECACHE_SOUND("player/pl_ladder1.wav");
	PRECACHE_SOUND("player/pl_ladder2.wav");
	PRECACHE_SOUND("player/pl_ladder3.wav");
	PRECACHE_SOUND("player/pl_ladder4.wav");
	PRECACHE_SOUND("player/pl_wade1.wav");
	PRECACHE_SOUND("player/pl_wade2.wav");
	PRECACHE_SOUND("player/pl_wade3.wav");
	PRECACHE_SOUND("player/pl_wade4.wav");
	PRECACHE_SOUND("debris/wood1.wav");
	PRECACHE_SOUND("debris/wood2.wav");
	PRECACHE_SOUND("debris/wood3.wav");
	PRECACHE_SOUND("plats/train_use1.wav");
	PRECACHE_SOUND("plats/vehicle_ignition.wav");
	PRECACHE_SOUND("buttons/spark5.wav");
	PRECACHE_SOUND("buttons/spark6.wav");
	PRECACHE_SOUND("debris/glass1.wav");
	PRECACHE_SOUND("debris/glass2.wav");
	PRECACHE_SOUND("debris/glass3.wav");
	PRECACHE_SOUND(SOUND_FLASHLIGHT_ON);
	PRECACHE_SOUND(SOUND_FLASHLIGHT_OFF);
	PRECACHE_SOUND("common/bodysplat.wav");
	PRECACHE_SOUND("player/pl_pain2.wav");
	PRECACHE_SOUND("player/pl_pain4.wav");
	PRECACHE_SOUND("player/pl_pain5.wav");
	PRECACHE_SOUND("player/pl_pain6.wav");
	PRECACHE_SOUND("player/pl_pain7.wav");

	for (unsigned i = 0; i < _countof(sPlayerModelFiles); i++)
		PRECACHE_MODEL(sPlayerModelFiles[i]);

	for (i = FirstCustomSkin; i <= LastCustomSkin; i++)
	{
		const char* fname = TheBotProfiles->GetCustomSkinFname(i);

		if (!fname)
			break;

		PRECACHE_MODEL((char*)fname);
	}

	PRECACHE_SOUND("common/wpn_hudoff.wav");
	PRECACHE_SOUND("common/wpn_hudon.wav");
	PRECACHE_SOUND("common/wpn_moveselect.wav");
	PRECACHE_SOUND("common/wpn_select.wav");
	PRECACHE_SOUND("common/wpn_denyselect.wav");
	PRECACHE_SOUND("player/geiger6.wav");
	PRECACHE_SOUND("player/geiger5.wav");
	PRECACHE_SOUND("player/geiger4.wav");
	PRECACHE_SOUND("player/geiger3.wav");
	PRECACHE_SOUND("player/geiger2.wav");
	PRECACHE_SOUND("player/geiger1.wav");

	g_iShadowSprite = PRECACHE_MODEL("sprites/shadow_circle.spr");

	PRECACHE_MODEL("sprites/wall_puff1.spr");
	PRECACHE_MODEL("sprites/wall_puff2.spr");
	PRECACHE_MODEL("sprites/wall_puff3.spr");
	PRECACHE_MODEL("sprites/wall_puff4.spr");
	PRECACHE_MODEL("sprites/black_smoke1.spr");
	PRECACHE_MODEL("sprites/black_smoke2.spr");
	PRECACHE_MODEL("sprites/black_smoke3.spr");
	PRECACHE_MODEL("sprites/black_smoke4.spr");
	PRECACHE_MODEL("sprites/gas_puff_01.spr");

	PRECACHE_MODEL("sprites/fast_wallpuff1.spr");
	PRECACHE_MODEL("sprites/pistol_smoke1.spr");
	PRECACHE_MODEL("sprites/pistol_smoke2.spr");
	PRECACHE_MODEL("sprites/rifle_smoke1.spr");
	PRECACHE_MODEL("sprites/rifle_smoke2.spr");
	PRECACHE_MODEL("sprites/rifle_smoke3.spr");
	PRECACHE_GENERIC("sprites/scope_arc.tga");
	PRECACHE_GENERIC("sprites/scope_arc_nw.tga");
	PRECACHE_GENERIC("sprites/scope_arc_ne.tga");
	PRECACHE_GENERIC("sprites/scope_arc_sw.tga");

	m_usResetDecals = g_engfuncs.pfnPrecacheEvent(1, "events/decal_reset.sc");
}

const char *EXT_FUNC GetGameDescription()
{
	if (CSGameRules())
	{
		return CSGameRules()->GetGameDescription();
	}

	return "Leader Mode";
}

void EXT_FUNC SysEngine_Error(const char *error_string)
{
	;
}

void EXT_FUNC PlayerCustomization(edict_t *pEntity, customization_t *pCust)
{
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (!pPlayer)
	{
		ALERT(at_console, "PlayerCustomization:  Couldn't get player!\n");
		return;
	}

	if (!pCust)
	{
		ALERT(at_console, "PlayerCustomization:  NULL customization!\n");
		return;
	}

	switch (pCust->resource.type)
	{
	case t_decal:
		pPlayer->SetCustomDecalFrames(pCust->nUserData2);
		break;
	case t_sound:
	case t_skin:
	case t_model:
		break;
	default:
		ALERT(at_console, "PlayerCustomization:  Unknown customization type!\n");
		break;
	}
}

void EXT_FUNC SpectatorConnect(edict_t *pEntity)
{
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
	{
		pPlayer->SpectatorConnect();
	}
}

void EXT_FUNC SpectatorDisconnect(edict_t *pEntity)
{
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
	{
		pPlayer->SpectatorDisconnect();
	}
}

void EXT_FUNC SpectatorThink(edict_t *pEntity)
{
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
	{
		pPlayer->SpectatorThink();
	}
}

void EXT_FUNC SetupVisibility(edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas)
{
	edict_t *pView = pClient;

	// Find the client's PVS
	if (pViewEntity)
	{
		pView = pViewEntity;
	}

	if (pClient->v.flags & FL_PROXY)
	{
		*pvs = nullptr;	// the spectator proxy sees
		*pas = nullptr;	// and hears everything
		return;
	}

	CBasePlayer *pPlayer = CBasePlayer::Instance(pClient);
	if (pPlayer && pPlayer->pev->iuser2 && pPlayer->m_hObserverTarget)
	{
		if (pPlayer->m_afPhysicsFlags & PFLAG_OBSERVER)
		{
			pView = pPlayer->m_hObserverTarget->edict();
			UTIL_SetOrigin(pPlayer->pev, pPlayer->m_hObserverTarget->pev->origin);
		}
	}

	Vector org = pView->v.origin + pView->v.view_ofs;

	if (pView->v.flags & FL_DUCKING)
	{
		org = org + (VEC_HULL_MIN - VEC_DUCK_HULL_MIN);
	}

	*pvs = ENGINE_SET_PVS((float *)&org);
	*pas = ENGINE_SET_PAS((float *)&org);
}

void ResetPlayerPVS(edict_t *client, int clientnum)
{
	PLAYERPVSSTATUS *pvs = &g_PVSStatus[clientnum];

	Q_memset(pvs, 0, sizeof(*pvs));
	pvs->headnode = client->headnode;
	pvs->num_leafs = client->num_leafs;
	Q_memcpy(pvs->leafnums, client->leafnums, sizeof(pvs->leafnums));
}

bool CheckPlayerPVSLeafChanged(edict_t *client, int clientnum)
{
	PLAYERPVSSTATUS *pvs = &g_PVSStatus[clientnum];
	if (pvs->headnode != client->headnode || pvs->num_leafs != client->num_leafs)
		return true;

	for (int i = 0; i < pvs->num_leafs; i++)
	{
		if (client->leafnums[i] != pvs->leafnums[i])
			return true;
	}

	return false;
}

void MarkEntityInPVS(int clientnum, int entitynum, float time, bool inpvs)
{
	PLAYERPVSSTATUS *pvs;
	ENTITYPVSSTATUS *es;

	pvs = &g_PVSStatus[clientnum];
	es = &pvs->m_Status[entitynum];

	if (inpvs)
		es->m_fTimeEnteredPVS = time;
	else
		es->m_fTimeEnteredPVS = 0;
}

bool CheckEntityRecentlyInPVS(int clientnum, int entitynum, float currenttime)
{
	PLAYERPVSSTATUS *pvs;
	ENTITYPVSSTATUS *es;

	pvs = &g_PVSStatus[clientnum];
	es = &pvs->m_Status[entitynum];

	if (es->m_fTimeEnteredPVS && es->m_fTimeEnteredPVS + 1.0f >= currenttime)
	{
		return true;
	}

	return false;
}

BOOL EXT_FUNC AddToFullPack(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, BOOL player, unsigned char *pSet)
{
	if (ent != host)
	{
		if ((ent->v.effects & EF_NODRAW) == EF_NODRAW)
			return FALSE;

		if (ent->v.owner == host)
		{
			// the owner can't see this entity
			if ((ent->v.effects & EF_OWNER_NO_VISIBILITY) == EF_OWNER_NO_VISIBILITY)
				return FALSE;
		}
		// no one can't see this entity except the owner
		else if ((ent->v.effects & EF_OWNER_VISIBILITY) == EF_OWNER_VISIBILITY)
			return FALSE;
	}

	if (!ent->v.modelindex || !STRING(ent->v.model))
		return FALSE;

	if ((ent->v.flags & FL_SPECTATOR) == FL_SPECTATOR && ent != host)
		return FALSE;

	int i;
	int hostnum = ENTINDEX(host) - 1;
	if (CheckPlayerPVSLeafChanged(host, hostnum))
		ResetPlayerPVS(host, hostnum);

	if ((ent->v.effects & EF_FORCEVISIBILITY) != EF_FORCEVISIBILITY)
	{
		if (ent != host)
		{
			if (!CheckEntityRecentlyInPVS(hostnum, e, gpGlobals->time))
			{
				if (!ENGINE_CHECK_VISIBILITY(ent, pSet))
				{
					MarkEntityInPVS(hostnum, e, 0, false);
					return FALSE;
				}

				MarkEntityInPVS(hostnum, e, gpGlobals->time, true);
			}
		}
	}

	if ((ent->v.flags & FL_SKIPLOCALHOST) == FL_SKIPLOCALHOST && (hostflags & 1) && ent->v.owner == host)
		return FALSE;

	if (host->v.groupinfo)
	{
		UTIL_SetGroupTrace(host->v.groupinfo, GROUP_OP_AND);

		if (ent->v.groupinfo)
		{
			if (g_groupop == GROUP_OP_AND)
			{
				if (!(ent->v.groupinfo & host->v.groupinfo))
					return FALSE;
			}
			else if (g_groupop == GROUP_OP_NAND)
			{
				if (ent->v.groupinfo & host->v.groupinfo)
					return FALSE;
			}
		}

		UTIL_UnsetGroupTrace();
	}

	Q_memset(state, 0, sizeof(entity_state_t));

	state->number = e;
	state->entityType = ENTITY_NORMAL;

	if (ent->v.flags & FL_CUSTOMENTITY)
		state->entityType = ENTITY_BEAM;

	state->animtime = int(1000.0 * ent->v.animtime) / 1000.0;

	Q_memcpy(state->origin, ent->v.origin, sizeof(float) * 3);
	Q_memcpy(state->angles, ent->v.angles, sizeof(float) * 3);
	Q_memcpy(state->mins, ent->v.mins, sizeof(float) * 3);
	Q_memcpy(state->maxs, ent->v.maxs, sizeof(float) * 3);
	Q_memcpy(state->startpos, ent->v.startpos, sizeof(float) * 3);
	Q_memcpy(state->endpos, ent->v.endpos, sizeof(float) * 3);

	state->impacttime = ent->v.impacttime;
	state->starttime = ent->v.starttime;
	state->modelindex = ent->v.modelindex;
	state->frame = ent->v.frame;
	state->skin = ent->v.skin;
	state->effects = ent->v.effects;

	// don't send unhandled custom bits to client
	state->effects &= ~(EF_FORCEVISIBILITY | EF_OWNER_VISIBILITY | EF_OWNER_NO_VISIBILITY);

	if  (ent->v.skin == CONTENTS_LADDER && (host->v.iuser3 & PLAYER_PREVENT_CLIMB) == PLAYER_PREVENT_CLIMB)
	{
		state->skin = CONTENTS_EMPTY;
	}

	if (!player && ent->v.animtime && !ent->v.velocity.x && !ent->v.velocity.y && !ent->v.velocity.z)
		state->eflags |= EFLAG_SLERP;

	state->scale = ent->v.scale;
	state->solid = ent->v.solid;
	state->colormap = ent->v.colormap;
	state->movetype = ent->v.movetype;
	state->sequence = ent->v.sequence;
	state->framerate = ent->v.framerate;
	state->body = ent->v.body;

	for (i = 0; i < 4; i++)
		state->controller[i] = ent->v.controller[i];

	for (i = 0; i < 2; i++)
		state->blending[i] = ent->v.blending[i];

	if (THE_GODFATHER.IsValid() && ent == THE_GODFATHER->edict())
	{
		state->rendermode = kRenderNormal;
		state->renderamt = 1;
		state->renderfx = kRenderFxGlowShell;
		state->rendercolor.r = 255;
		state->rendercolor.g = 0;
		state->rendercolor.b = 0;
	}
	else if (THE_COMMANDER.IsValid() && ent == THE_COMMANDER->edict())
	{
		state->rendermode = kRenderNormal;
		state->renderamt = 1;
		state->renderfx = kRenderFxGlowShell;
		state->rendercolor.r = 0;
		state->rendercolor.g = 0;
		state->rendercolor.b = 255;
	}
	else
	{
		state->rendermode = ent->v.rendermode;
		state->renderamt = int(ent->v.renderamt);
		state->renderfx = ent->v.renderfx;
		state->rendercolor.r = byte(ent->v.rendercolor.x);
		state->rendercolor.g = byte(ent->v.rendercolor.y);
		state->rendercolor.b = byte(ent->v.rendercolor.z);
	}

	if (player)
	{
		CBasePlayer* pPlayer = CBasePlayer::Instance(ent);
		if (pPlayer->m_flFrozenNextThink > 0.0f)	// a frozen character.
		{
			state->renderfx = kRenderFxGlowShell;
			state->renderamt = 50;
			state->rendermode = kRenderNormal;
			state->rendercolor.r = 80;
			state->rendercolor.g = 80;
			state->rendercolor.b = 100;
		}
	}

	state->aiment = 0;

	if (ent->v.aiment)
		state->aiment = ENTINDEX(ent->v.aiment);

	state->owner = 0;
	if (ent->v.owner)
	{
		int owner = ENTINDEX(ent->v.owner);
		if (owner >= 1 && owner <= gpGlobals->maxClients)
			state->owner = owner;
	}

	if (player)
	{
		Q_memcpy(state->basevelocity, ent->v.basevelocity, sizeof(float) * 3);

		state->weaponmodel = MODEL_INDEX(STRING(ent->v.weaponmodel));
		state->gaitsequence = ent->v.gaitsequence;

		state->spectator = (ent->v.flags & FL_SPECTATOR) ? TRUE : FALSE;
		state->friction = ent->v.friction;
		state->gravity = ent->v.gravity;
		state->usehull = (ent->v.flags & FL_DUCKING) ? 1 : 0;
		state->health = int(ent->v.health);
	}
	else
		state->playerclass = ent->v.playerclass;

	state->iuser4 = ent->v.iuser4;

	CBasePlayer::Instance(host)->OnAddToFullPack(state, ent, player);

	if (player)
		CBasePlayer::Instance(ent)->OnBeingAddToFullPack(state, CBasePlayer::Instance(host));

	return TRUE;
}

// Creates baselines used for network encoding, especially for player data since players are not spawned until connect time.
void EXT_FUNC CreateBaseline(int player, int eindex, struct entity_state_s *baseline, edict_t *entity, int playermodelindex, Vector player_mins, Vector player_maxs)
{
	baseline->origin = entity->v.origin;
	baseline->angles = entity->v.angles;

	baseline->frame = entity->v.frame;
	baseline->skin = (short)entity->v.skin;

	// render information
	baseline->rendermode = byte(entity->v.rendermode);
	baseline->renderamt = byte(entity->v.renderamt);
	baseline->rendercolor.r	= byte(entity->v.rendercolor.x);
	baseline->rendercolor.g	= byte(entity->v.rendercolor.y);
	baseline->rendercolor.b	= byte(entity->v.rendercolor.z);
	baseline->renderfx = byte(entity->v.renderfx);

	if (player)
	{
		baseline->mins = player_mins;
		baseline->maxs = player_maxs;

		baseline->colormap = eindex;
		baseline->modelindex = playermodelindex;
		baseline->friction = 1.0;
		baseline->movetype = MOVETYPE_WALK;

		baseline->solid = SOLID_SLIDEBOX;
		baseline->scale = entity->v.scale;
		baseline->framerate = 1.0;
		baseline->gravity = 1.0;
	}
	else
	{
		baseline->mins = entity->v.mins;
		baseline->maxs = entity->v.maxs;

		baseline->colormap = 0;
		baseline->modelindex = entity->v.modelindex;
		baseline->movetype = entity->v.movetype;

		baseline->scale = entity->v.scale;
		baseline->solid = entity->v.solid;
		baseline->framerate = entity->v.framerate;
		baseline->gravity = entity->v.gravity;
	}
}

void Entity_FieldInit(struct delta_s *pFields)
{
	entity_field_alias[FIELD_ORIGIN0].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_ORIGIN0].name);
	entity_field_alias[FIELD_ORIGIN1].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_ORIGIN1].name);
	entity_field_alias[FIELD_ORIGIN2].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_ORIGIN2].name);
	entity_field_alias[FIELD_ANGLES0].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_ANGLES0].name);
	entity_field_alias[FIELD_ANGLES1].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_ANGLES1].name);
	entity_field_alias[FIELD_ANGLES2].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_ANGLES2].name);
}

// Callback for sending entity_state_t info over network.
void Entity_Encode(struct delta_s *pFields, const unsigned char *from, const unsigned char *to)
{
	entity_state_t *f, *t;
	int localplayer = 0;
	static int initialized = 0;

	if (!initialized)
	{
		Entity_FieldInit(pFields);
		initialized = 1;
	}

	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	// Never send origin to local player, it's sent with more resolution in clientdata_t structure
	localplayer = (t->number - 1) == ENGINE_CURRENT_PLAYER();

	if (localplayer)
	{
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
	}
	if (t->impacttime != 0 && t->starttime != 0)
	{
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);

		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ANGLES0].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ANGLES1].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ANGLES2].field);
	}
	if (t->movetype == MOVETYPE_FOLLOW && t->aiment != 0)
	{
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
	}
	else if (t->aiment != f->aiment)
	{
		DELTA_SETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_SETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_SETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
	}
}

void Player_FieldInit(struct delta_s *pFields)
{
	player_field_alias[FIELD_ORIGIN0].field = DELTA_FINDFIELD(pFields, player_field_alias[FIELD_ORIGIN0].name);
	player_field_alias[FIELD_ORIGIN1].field = DELTA_FINDFIELD(pFields, player_field_alias[FIELD_ORIGIN1].name);
	player_field_alias[FIELD_ORIGIN2].field = DELTA_FINDFIELD(pFields, player_field_alias[FIELD_ORIGIN2].name);
}

// Callback for sending entity_state_t for players info over network.
void Player_Encode(struct delta_s *pFields, const unsigned char *from, const unsigned char *to)
{
	entity_state_t *f, *t;
	int localplayer = 0;

	static int initialized = 0;
	if (!initialized)
	{
		Player_FieldInit(pFields);
		initialized = 1;
	}

	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	// Never send origin to local player, it's sent with more resolution in clientdata_t structure
	localplayer = (t->number - 1) == ENGINE_CURRENT_PLAYER();

	if (localplayer)
	{
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
	}
	if (t->movetype == MOVETYPE_FOLLOW && t->aiment != 0)
	{
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
	}
	else if (t->aiment != f->aiment)
	{
		DELTA_SETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_SETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_SETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
	}
}

void Custom_Entity_FieldInit(delta_s *pFields)
{
	custom_entity_field_alias[CUSTOMFIELD_ORIGIN0].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ORIGIN0].name);
	custom_entity_field_alias[CUSTOMFIELD_ORIGIN1].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ORIGIN1].name);
	custom_entity_field_alias[CUSTOMFIELD_ORIGIN2].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ORIGIN2].name);
	custom_entity_field_alias[CUSTOMFIELD_ANGLES0].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ANGLES0].name);
	custom_entity_field_alias[CUSTOMFIELD_ANGLES1].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ANGLES1].name);
	custom_entity_field_alias[CUSTOMFIELD_ANGLES2].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ANGLES2].name);
	custom_entity_field_alias[CUSTOMFIELD_SKIN].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_SKIN].name);
	custom_entity_field_alias[CUSTOMFIELD_SEQUENCE].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_SEQUENCE].name);
	custom_entity_field_alias[CUSTOMFIELD_ANIMTIME].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ANIMTIME].name);
}

// Callback for sending entity_state_t info ( for custom entities ) over network.
void Custom_Encode(struct delta_s *pFields, const unsigned char *from, const unsigned char *to)
{
	entity_state_t *f, *t;
	int beamType;
	static int initialized = 0;

	if (!initialized)
	{
		Custom_Entity_FieldInit(pFields);
		initialized = 1;
	}

	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	beamType = t->rendermode & 0x0F;

	if (beamType != BEAM_POINTS)
	{
		if (beamType != BEAM_ENTPOINT)
		{
			DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ORIGIN0].field);
			DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ORIGIN1].field);
			DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ORIGIN2].field);
		}

		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ANGLES0].field);
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ANGLES1].field);
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ANGLES2].field);
	}

	if (beamType != BEAM_ENTS && beamType != BEAM_ENTPOINT)
	{
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_SKIN].field);
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_SEQUENCE].field);
	}

	// animtime is compared by rounding first
	// see if we really shouldn't actually send it
	if (int(f->animtime) == int(t->animtime))
	{
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ANIMTIME].field);
	}
}

// Allows game .dll to override network encoding of certain types of entities and tweak values, etc.
void EXT_FUNC RegisterEncoders()
{
	DELTA_ADDENCODER("Entity_Encode", Entity_Encode);
	DELTA_ADDENCODER("Custom_Encode", Custom_Encode);
	DELTA_ADDENCODER("Player_Encode", Player_Encode);
}

int EXT_FUNC GetWeaponData(edict_t *pEdict, struct weapon_data_s *info)
{
#ifdef CLIENT_WEAPONS
	entvars_t *pev = &pEdict->v;
	CBasePlayer *pPlayer = CBasePlayer::Instance(pev);

	Q_memset(info, 0, sizeof(weapon_data_t) * ENGINE_WEAPON_LIMIT);

	if (!pPlayer)
		return 1;

	// go through all of the weapons and make a list of the ones to pack
	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		auto pWeapon = pPlayer->m_rgpPlayerItems[i];
		if (pWeapon)	// there's a weapon here. Should I pack it?
		{
			// Get The basic data
			WeaponInfo II;
			Q_memcpy(&II, pWeapon->m_pItemInfo, sizeof(WeaponInfo));

			if (II.m_iId >= 0 && II.m_iId < ENGINE_WEAPON_LIMIT)
			{
				auto item = &info[II.m_iId];

				item->m_iId = II.m_iId;
				item->m_iClip = pWeapon->m_iClip;
				item->m_flTimeWeaponIdle = Q_max(pWeapon->m_flTimeWeaponIdle, -0.001f);
				item->m_flNextPrimaryAttack = Q_max(pWeapon->m_flNextPrimaryAttack, -0.001f);
				item->m_flNextSecondaryAttack = Q_max(pWeapon->m_flNextSecondaryAttack, -0.001f);
				//item->m_flNextReload = Q_max(pWeapon->m_flNextReload, -0.001f); // TODO: what are these???
				item->m_fInReload = pWeapon->m_bInReload;
				item->m_fInSpecialReload = pWeapon->m_bInZoom;
				item->m_fInZoom = pWeapon->m_iShotsFired;
				item->m_fAimedDamage = pWeapon->m_flLastFire;
				item->m_iWeaponState = pWeapon->m_bitsFlags;
				//item->fuser2 = weapon->m_flStartThrow;
				//item->fuser3 = weapon->m_flReleaseThrow;
				//item->iuser1 = weapon->m_iSwing;
			}

			// this is for original CBasePlayerItem.
			//pPlayerItem = pPlayerItem->m_pNext;
		}
	}
#else
	Q_memset(info, 0, sizeof(weapon_data_t) * ENGINE_WEAPON_LIMIT);
#endif

	return 1;
}

// Data sent to current client only engine sets cd to 0 before calling.
void EXT_FUNC UpdateClientData(const edict_t *ent, int sendweapons, struct clientdata_s *cd)
{
	if (!ent || !ent->pvPrivateData)
		return;

	entvars_t *pevOrg = nullptr;
	entvars_t *pev = const_cast<entvars_t *>(&ent->v);
	CBasePlayer *pPlayer = CBasePlayer::Instance(pev);

	// if user is spectating different player in First person, override some vars
	if (pPlayer && pPlayer->pev->iuser1 == OBS_IN_EYE && pPlayer->m_hObserverTarget)
	{
		pevOrg = pev;
		pev = pPlayer->m_hObserverTarget->pev;
		pPlayer = CBasePlayer::Instance(pev);
	}

	cd->flags = pev->flags;
	cd->health = pev->health;
	cd->viewmodel = MODEL_INDEX(DUMMY_VIEW_MODEL);	// Remove the ability that server can assign client's view model.
	cd->waterlevel = pev->waterlevel;
	cd->watertype = pev->watertype;
	cd->weapons = pev->weapons;
	cd->origin = pev->origin;
	cd->velocity = pev->velocity;
	cd->view_ofs = pev->view_ofs;
	cd->punchangle = pev->punchangle;
	cd->bInDuck = pev->bInDuck;
	cd->flTimeStepSound = pev->flTimeStepSound;
	cd->flDuckTime = pev->flDuckTime;
	cd->flSwimTime = pev->flSwimTime;
	cd->waterjumptime = int(pev->teleport_time);

	Q_strcpy(cd->physinfo, ENGINE_GETPHYSINFO(ent));

	cd->maxspeed = pev->maxspeed;
	cd->fov = pev->fov;
	cd->weaponanim = pev->weaponanim;
	cd->pushmsec = pev->pushmsec;

	if (pevOrg)
	{
		cd->iuser1 = pevOrg->iuser1;
		cd->iuser2 = pevOrg->iuser2;
		cd->iuser3 = pevOrg->iuser3;
	}
	else
	{
		cd->iuser1 = pev->iuser1;
		cd->iuser2 = pev->iuser2;
		cd->iuser3 = pev->iuser3;
	}

	cd->fuser1 = pev->fuser1;
	cd->fuser3 = pev->fuser3;
	cd->fuser2 = pev->fuser2;

	if (sendweapons && pPlayer)
	{
		cd->ammo_shells = pPlayer->m_rgAmmo[5];
		cd->ammo_nails = pPlayer->m_rgAmmo[10];
		cd->ammo_cells = pPlayer->m_rgAmmo[4];
		cd->ammo_rockets = pPlayer->m_rgAmmo[3];
		cd->vuser2.x = pPlayer->m_rgAmmo[2];
		cd->vuser2.y = pPlayer->m_rgAmmo[6];
		cd->vuser2.z = pPlayer->m_rgAmmo[8];
		cd->vuser3.x = pPlayer->m_rgAmmo[1];
		cd->vuser3.y = pPlayer->m_rgAmmo[7];
		cd->vuser3.z = pPlayer->m_rgAmmo[9];
		cd->m_flNextAttack = pPlayer->m_flNextAttack;

		int iUser3 = 0;
		if (pPlayer->m_bCanShoot)
			iUser3 |= PLAYER_CAN_SHOOT;

		if (g_pGameRules->IsFreezePeriod())
			iUser3 |= PLAYER_FREEZE_TIME_OVER;

		if (pPlayer->m_signals.GetState() & SIGNAL_BOMB)
			iUser3 |= PLAYER_IN_BOMB_ZONE;

		if (pPlayer->HasShield())
			iUser3 |= PLAYER_HOLDING_SHIELD;

		if (pPlayer->pev->iuser1 == OBS_NONE && !pevOrg)
		{
			// useful for mods
			iUser3 |= pev->iuser3;

			cd->iuser3 = iUser3;
		}

		if (pPlayer->m_pActiveItem)
		{
			cd->m_iId = pPlayer->m_pActiveItem->m_iId;

			if ((unsigned int)pPlayer->m_pActiveItem->m_iPrimaryAmmoType < MAX_AMMO_SLOTS)
			{
				cd->vuser4.x = pPlayer->m_pActiveItem->m_iPrimaryAmmoType;
				cd->vuser4.y = pPlayer->m_rgAmmo[pPlayer->m_pActiveItem->m_iPrimaryAmmoType];
			}
			else
			{
				cd->vuser4.x = -1.0;
				cd->vuser4.y = 0;
			}
		}
	}
}

void EXT_FUNC CmdStart(const edict_t *pEdict, const struct usercmd_s *cmd, unsigned int random_seed)
{
	entvars_t *pev = const_cast<entvars_t *>(&pEdict->v);
	CBasePlayer *pPlayer = CBasePlayer::Instance(pev);

	if (!pPlayer)
		return;

	if (pPlayer->pev->groupinfo)
		UTIL_SetGroupTrace(pPlayer->pev->groupinfo, GROUP_OP_AND);

	pPlayer->random_seed = random_seed;
}

void EXT_FUNC CmdEnd(const edict_t *pEdict)
{
	entvars_t *pev = const_cast<entvars_t *>(&pEdict->v);
	CBasePlayer *pPlayer = CBasePlayer::Instance(pev);

	if (!pPlayer)
		return;

	if (pPlayer->pev->groupinfo)
		UTIL_UnsetGroupTrace();

	if (pev->flags & FL_DUCKING)
		UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
}

int EXT_FUNC ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size)
{
	// Parse stuff from args
	int max_buffer_size = *response_buffer_size;

	// Zero it out since we aren't going to respond.
	// If we wanted to response, we'd write data into response_buffer
	*response_buffer_size = 0;

	// Since we don't listen for anything here, just respond that it's a bogus message
	// If we didn't reject the message, we'd return 1 for success instead.
	return 0;
}

BOOL EXT_FUNC GetHullBounds(int hullnumber, float *mins, float *maxs)
{
	if (hullbounds_sets.value == 0.0f)
	{
		return (hullnumber < 3) ? TRUE : FALSE;
	}

	switch (hullnumber)
	{
	case 0: // Normal player
		Q_memcpy(mins, (float*)VEC_HULL_MIN, sizeof(vec3_t));
		Q_memcpy(maxs, (float*)VEC_HULL_MAX, sizeof(vec3_t));
		return TRUE;
	case 1: // Crouched player
		Q_memcpy(mins, (float*)VEC_DUCK_HULL_MIN, sizeof(vec3_t));
		Q_memcpy(maxs, (float*)VEC_DUCK_HULL_MAX, sizeof(vec3_t));
		return TRUE;
	case 2: // Point based hull
		Q_memcpy(mins, (float*)Vector(0, 0, 0), sizeof(vec3_t));
		Q_memcpy(maxs, (float*)Vector(0, 0, 0), sizeof(vec3_t));
		return TRUE;
	default:
		return FALSE;
	}
}

// Create pseudo-baselines for items that aren't placed in the map at spawn time, but which are likely
// to be created during play ( e.g., grenades, ammo packs, projectiles, corpses, etc. )
void EXT_FUNC CreateInstancedBaselines()
{
#ifndef REGAMEDLL_FIXES
	int iret = 0;
	entity_state_t state;

	Q_memset(&state, 0, sizeof(state));

	// Create any additional baselines here for things like grendates, etc.
	// iret = ENGINE_INSTANCE_BASELINE(pc->pev->classname, &state);

	// Destroy objects.
	// UTIL_Remove(pc);
#endif
}

int EXT_FUNC InconsistentFile(const edict_t *pEdict, const char *filename, char *disconnect_message)
{
	// Server doesn't care?
	if (!CVAR_GET_FLOAT("mp_consistency"))
		return 0;

	// Default behavior is to kick the player
	sprintf(disconnect_message, "Server is enforcing file consistency for %s\n", filename);	// LUNA: confirmed usage of sprintf().

	// Kick now with specified disconnect message.
	return 1;
}

// The game .dll should return 1 if lag compensation should be allowed ( could also just set
// the sv_unlag cvar.
// Most games right now should return 0, until client-side weapon prediction code is written
// and tested for them ( note you can predict weapons, but not do lag compensation, too,
// if you want.
int EXT_FUNC AllowLagCompensation()
{
	return 1;
}
