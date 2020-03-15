/*

Created Date: 08 Mar 2020

*/

#include "precompiled.h"

bool g_bHasDefuser = false;
bool g_bHasNightvision = false;

// client.cpp
MSG_FUNC(CurWeapon)
{
	BEGIN_READ(pbuf, iSize);

	int iState = READ_BYTE();
	int iId = READ_BYTE();
	int iClip = READ_BYTE();

	gHUD::m_Ammo.MsgFunc_CurWeapon(iState, iId, iClip);
	return TRUE;
}

MSG_FUNC(Geiger)
{
	BEGIN_READ(pbuf, iSize);

	int iRange = READ_BYTE();

	gHUD::m_Geiger.MsgFunc_Geiger(iRange);
	return TRUE;
}

MSG_FUNC(Flashlight)
{
	BEGIN_READ(pbuf, iSize);

	BOOL FOn = READ_BYTE();
	int iBattery = READ_BYTE();

	gHUD::m_Flash.MsgFunc_Flashlight(FOn, iBattery);
	return TRUE;
}

MSG_FUNC(FlashBat)
{
	BEGIN_READ(pbuf, iSize);

	int iBattery = READ_BYTE();

	gHUD::m_Flash.MsgFunc_FlashBat(iBattery);
	return TRUE;
}

MSG_FUNC(Health)
{
	BEGIN_READ(pbuf, iSize);

	int iClient = READ_BYTE();
	int iHealth = READ_SHORT();

	g_PlayerExtraInfo[iClient].health = iHealth;

	if (iClient == gEngfuncs.GetLocalPlayer()->index)
		gHUD::m_Health.MsgFunc_Health(iHealth);

	return TRUE;
}

MSG_FUNC(Damage)
{
	BEGIN_READ(pbuf, iSize);

	int iArmor = READ_BYTE();	// LUNA: ???
	int iDmgTaken = READ_BYTE();	// LUNA: ???
	int bitsDamageTypes = READ_LONG();

	Vector vecDamageSrc;
	vecDamageSrc.x = READ_COORD();
	vecDamageSrc.y = READ_COORD();
	vecDamageSrc.z = READ_COORD();

	gHUD::m_Health.MsgFunc_Damage(iArmor, iDmgTaken, bitsDamageTypes, vecDamageSrc);
	return TRUE;
}

MSG_FUNC(Battery)
{
	BEGIN_READ(pbuf, iSize);

	int iArmour = READ_SHORT();

	gHUD::m_Battery.MsgFunc_Battery(iArmour);
	return TRUE;
}

MSG_FUNC(Train)
{
	BEGIN_READ(pbuf, iSize);

	int iSpeedLevel = READ_BYTE();

	gHUD::m_Train.MsgFunc_Train(iSpeedLevel);
	return TRUE;
}

MSG_FUNC(HudTextPro)
{
	BEGIN_READ(pbuf, iSize);

	char* pString = READ_STRING();
	BOOL hintMessage = READ_BYTE();

	gHUD::m_Message.MsgFunc_HudText(pString, hintMessage);
	return TRUE;
}

MSG_FUNC(HudText)
{
	BEGIN_READ(pbuf, iSize);

	char* pString = READ_STRING();
	BOOL hintMessage = READ_BYTE();

	if (!READ_OK())
		hintMessage = FALSE;

	gHUD::m_Message.MsgFunc_HudText(pString, hintMessage);
	return TRUE;
}

MSG_FUNC(SayText)
{
	gHUD::m_SayText.MsgFunc_SayText(iSize, pbuf);
	return TRUE;
}

MSG_FUNC(TextMsg)
{
	gHUD::m_TextMessage.MsgFunc_TextMsg(iSize, pbuf);
	return TRUE;
}

MSG_FUNC(WeaponList)
{
	BEGIN_READ(pbuf, iSize);

	WEAPON Weapon;
	Q_strlcpy(Weapon.szName, READ_STRING());
	Weapon.iAmmoType = (int)READ_CHAR();
	Weapon.iMax1 = READ_BYTE();

	if (Weapon.iMax1 == 255)
		Weapon.iMax1 = -1;

	Weapon.iAmmo2Type = READ_CHAR();
	Weapon.iMax2 = READ_BYTE();

	if (Weapon.iMax2 == 255)
		Weapon.iMax2 = -1;

	Weapon.iSlot = READ_CHAR();
	Weapon.iSlotPos = READ_CHAR();
	Weapon.iId = READ_CHAR();
	Weapon.iFlags = READ_BYTE();
	Weapon.iClip = 0;
	Weapon.szExtraName[0] = '\0';

	gWR.AddWeapon(&Weapon);
	return TRUE;
}

MSG_FUNC(ResetHUD)
{
	// this msg have no arguments.

	for (auto pHudElem : gHUD::m_lstAllHUDElems)
	{
		pHudElem->Reset();
	}

	gHUD::m_flMouseSensitivity = 0;

	for (int i = 1; i <= MAX_PLAYERS; i++)
		g_PlayerScoreAttrib[i] = 0;

	// UNDONE
	//if (gConfigs.bEnableClientUI)
		//g_pViewPort->Reset();

	return TRUE;
}

MSG_FUNC(InitHUD)
{
	// this msg have no arguments.

	for (auto pHudElem : gHUD::m_lstAllHUDElems)
	{
		pHudElem->InitHUDData();
	}

	g_bFreezeTimeOver = false;

	Q_memset(g_PlayerExtraInfo, NULL, sizeof(g_PlayerExtraInfo));

	// UNDONE: rain
	//ResetRain();

	// reset round time
	int iTime = 0;
	gHUD::m_roundTimer.MsgFunc_RoundTime(iTime);

	// reinitialize models. We assume that server already precached all models.
	g_iRShell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");
	g_iPShell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");
	g_iShotgunShell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shotgunshell.mdl");
	g_iBlackSmoke = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/black_smoke4.spr");

	/* UNDONE : Fog
	g_FogParameters.density = 0;
	g_FogParameters.affectsSkyBox = false;
	g_FogParameters.color[0] = 0;
	g_FogParameters.color[1] = 0;
	g_FogParameters.color[2] = 0;

	if (cl_fog_skybox)
		gEngfuncs.Cvar_SetValue(cl_fog_skybox->name, g_FogParameters.affectsSkyBox);

	if (cl_fog_density)
		gEngfuncs.Cvar_SetValue(cl_fog_density->name, g_FogParameters.density);

	if (cl_fog_r)
		gEngfuncs.Cvar_SetValue(cl_fog_r->name, g_FogParameters.color[0]);

	if (cl_fog_g)
		gEngfuncs.Cvar_SetValue(cl_fog_g->name, g_FogParameters.color[1]);

	if (cl_fog_b)
		gEngfuncs.Cvar_SetValue(cl_fog_b->name, g_FogParameters.color[2]);
	*/
	return TRUE;
}

MSG_FUNC(ViewMode)
{
	// this msg have no arguments.

	return TRUE;
}

MSG_FUNC(GameTitle)
{
	BEGIN_READ(pbuf, iSize);

	int iUnknown = READ_BYTE();	// LUNA: what's this???

	// although this message has one argument, but it just remains unused.
	gHUD::m_Message.MsgFunc_GameTitle();

	return TRUE;
}

MSG_FUNC(DeathMsg)
{
	BEGIN_READ(pbuf, iSize);

	int iKillId = READ_BYTE();
	int iVictimId = READ_BYTE();
	BOOL FHeadShot = READ_BYTE();

	char szWeapon[192];
	Q_strlcpy(szWeapon, READ_STRING());

	return TRUE;
}

MSG_FUNC(ScoreAttrib)
{
	BEGIN_READ(pbuf, iSize);

	int iPlayerId = READ_BYTE();
	int bitsFlags = READ_BYTE();

	return TRUE;
}

MSG_FUNC(ScoreInfo)
{
	BEGIN_READ(pbuf, iSize);

	int iPlayerId = READ_BYTE();
	int iScore = READ_SHORT();
	int iDeaths = READ_SHORT();
	int iPlayerClass = READ_SHORT();
	int iTeam = READ_SHORT();

	return TRUE;
}

MSG_FUNC(TeamInfo)
{
	BEGIN_READ(pbuf, iSize);

	int iPlayerId = READ_BYTE();

	char szTeamName[192];
	Q_strlcpy(szTeamName, READ_STRING());

	return TRUE;
}

MSG_FUNC(TeamScore)
{
	BEGIN_READ(pbuf, iSize);

	char szTeamName[192];
	Q_strlcpy(szTeamName, READ_STRING());

	int iTeamScore = READ_SHORT();

	return TRUE;
}

MSG_FUNC(GameMode)
{
	BEGIN_READ(pbuf, iSize);

	int iGameMode = READ_BYTE();

	return TRUE;
}

static char s_szMOTD[2048] = "";

MSG_FUNC(MOTD)
{
	BEGIN_READ(pbuf, iSize);

	BOOL FGotAllTexts = READ_BYTE();

	Q_strlcat(s_szMOTD, READ_STRING());

	if (FGotAllTexts)
	{
		// UNDONE: display MOTD.

		// clear the buffer.
		Q_memset(s_szMOTD, NULL, sizeof(s_szMOTD));
	}

	return TRUE;
}

MSG_FUNC(ServerName)
{
	BEGIN_READ(pbuf, iSize);

	char szServerName[192];
	Q_strlcpy(szServerName, READ_STRING());

	return TRUE;
}

MSG_FUNC(AmmoPickup)
{
	BEGIN_READ(pbuf, iSize);

	int iAmmoId = READ_BYTE();
	int iAmount = READ_BYTE();

	gHR.AddToHistory(HISTSLOT_AMMO, iAmmoId, Q_abs(iAmount));
	return TRUE;
}

MSG_FUNC(WeapPickup)
{
	BEGIN_READ(pbuf, iSize);

	int iWeaponId = READ_BYTE();

	gHR.AddToHistory(HISTSLOT_WEAP, iWeaponId);
	return TRUE;
}

MSG_FUNC(ItemPickup)
{
	BEGIN_READ(pbuf, iSize);

	char szItemName[192];
	Q_strlcpy(szItemName, READ_STRING());

	gHR.AddToHistory(HISTSLOT_ITEM, szItemName);
	return TRUE;
}

MSG_FUNC(HideWeapon)
{
	BEGIN_READ(pbuf, iSize);

	int bitsWhat = READ_BYTE();

	gHUD::m_Ammo.MsgFunc_HideWeapon(bitsWhat);
	return TRUE;
}

MSG_FUNC(SetFOV)
{
	BEGIN_READ(pbuf, iSize);

	gHUD::m_iFOV = READ_BYTE();
	return TRUE;
}

MSG_FUNC(ShowMenu)
{
	gHUD::m_Menu.MsgFunc_ShowMenu(iSize, pbuf);
	return TRUE;
}

/*
// LUNA: according to Crsky, both Shake and Fade are handled by engine.
MSG_FUNC(ScreenShake)
{
	BEGIN_READ(pbuf, iSize);

	return TRUE;
}

MSG_FUNC(ScreenFade)
{
	BEGIN_READ(pbuf, iSize);

	return TRUE;
}
*/

MSG_FUNC(AmmoX)
{
	BEGIN_READ(pbuf, iSize);

	int iAmmoId = READ_BYTE();
	int iAmount = READ_BYTE();

	gWR.SetAmmo(iAmmoId, Q_abs(iAmount));
	return TRUE;
}

MSG_FUNC(SendAudio)
{
	BEGIN_READ(pbuf, iSize);

	int client = READ_BYTE();
	char* code = READ_STRING();
	int pitch = READ_SHORT();

	char name[64];
	Q_snprintf(name, sizeof(name) - 1, "misc/talk.wav");

	char* pName = name;
	char* pCode = Q_strstr(code, "%!");

	if (pCode)
	{
		{
			pCode++;

			while (*pCode)
			{
				if (*pCode <= 32 || *pCode > 122)
					break;

				*pName++ = *pCode++;
			}

			*pName = '\0';
		}

		gEngfuncs.pfnPlaySoundByNameAtPitch(name, 1.0, pitch);
	}
	else
	{
		gEngfuncs.pfnPlaySoundByNameAtPitch(code, 1.0, pitch);
	}

	g_PlayerExtraInfo[client].radarflashes = 22;
	g_PlayerExtraInfo[client].radarflash = gHUD::m_flTime;
	g_PlayerExtraInfo[client].radarflashon = 1;

	return TRUE;
}

MSG_FUNC(RoundTime)
{
	BEGIN_READ(pbuf, iSize);

	int iRoundTime = READ_SHORT();

	gHUD::m_roundTimer.MsgFunc_RoundTime(iRoundTime);
	return TRUE;
}

MSG_FUNC(Money)
{
	BEGIN_READ(pbuf, iSize);

	int iAccount = READ_LONG();
	BOOL FTrackChange = READ_BYTE();

	gHUD::m_accountBalance.MsgFunc_Money(iAccount, FTrackChange);
	return TRUE;
}

MSG_FUNC(ArmorType)
{
	BEGIN_READ(pbuf, iSize);

	int iArmourType = READ_BYTE();

	gHUD::m_Battery.MsgFunc_ArmorType(iArmourType);
	return TRUE;
}

MSG_FUNC(BlinkAcct)
{
	BEGIN_READ(pbuf, iSize);

	int iNumBlinks = READ_BYTE();

	gHUD::m_accountBalance.MsgFunc_BlinkAcct(iNumBlinks);
	return TRUE;
}

MSG_FUNC(StatusValue)
{
	BEGIN_READ(pbuf, iSize);

	int iType = READ_BYTE();
	int iValue = READ_SHORT();

	gHUD::m_StatusBar.MsgFunc_StatusValue(iType, iValue);
	return TRUE;
}

MSG_FUNC(StatusText)
{
	BEGIN_READ(pbuf, iSize);

	int iLine = READ_BYTE();	// unsure. named after CSBTE.

	char szText[192];
	Q_strlcpy(szText, READ_STRING());

	gHUD::m_StatusBar.MsgFunc_StatusText(iLine, szText);
	return TRUE;
}

MSG_FUNC(StatusIcon)
{
	gHUD::m_StatusIcons.MsgFunc_StatusIcon(iSize, pbuf);

	return TRUE;
}

MSG_FUNC(BarTime)
{
	BEGIN_READ(pbuf, iSize);

	int iTime = READ_SHORT();

	gHUD::m_progressBar.MsgFunc_BarTime(iTime);
	return TRUE;
}

MSG_FUNC(ReloadSound)
{
	BEGIN_READ(pbuf, iSize);

	int vol = READ_BYTE();

	if (READ_BYTE())
	{
		gEngfuncs.pfnPlaySoundByName("weapon/generic_reload.wav", vol / 255.0f);
	}
	else
	{
		gEngfuncs.pfnPlaySoundByName("weapon/generic_shot_reload.wav", vol / 255.0f);
	}

	return TRUE;
}

MSG_FUNC(Crosshair)
{
	BEGIN_READ(pbuf, iSize);

	BOOL FDrawn = READ_BYTE();

	gHUD::m_Ammo.MsgFunc_Crosshair(FDrawn);
	return TRUE;
}

MSG_FUNC(NVGToggle)
{
	BEGIN_READ(pbuf, iSize);

	BOOL FOn = READ_BYTE();

	gHUD::m_NightVision.MsgFunc_NVGToggle(FOn);
	return TRUE;
}

MSG_FUNC(Radar)
{
	BEGIN_READ(pbuf, iSize);

	int cl = READ_BYTE();
	g_PlayerExtraInfo[cl].origin[0] = READ_COORD();
	g_PlayerExtraInfo[cl].origin[1] = READ_COORD();
	g_PlayerExtraInfo[cl].origin[2] = READ_COORD();

	return TRUE;
}

MSG_FUNC(Spectator)
{
	BEGIN_READ(pbuf, iSize);

	int iPlayerId = READ_BYTE();
	int iUnknown = READ_BYTE();	// BTE and MoE just ignore this message.

	return TRUE;
}

MSG_FUNC(VGUIMenu)
{
	BEGIN_READ(pbuf, iSize);

	int iMenuType = READ_BYTE();
	int bitsMask = READ_SHORT();
	int iTime = READ_BYTE();	// unsure, unused(BTE & MoE), according to HL enging Wiki.
	BOOL FMultipart = READ_BYTE();	// unsure, unused(BTE & MoE), according to HL enging Wiki.
	char* pszMenuName = READ_STRING();		// unsure, unused(BTE & MoE), according to HL enging Wiki.

	// UNDONE
	/*if (gConfigs.bEnableClientUI)
	{
		g_pViewPort->ShowVGUIMenu(iMenuType);
	}*/

	return TRUE;
}

MSG_FUNC(TutorText)
{
	BEGIN_READ(pbuf, iSize);

	char szMessage[192];
	Q_strlcpy(szMessage, READ_STRING());

	int iNumArgs = READ_BYTE();

	char szParam[192];
	Q_strlcpy(szParam, READ_STRING());

	int iEventId = READ_SHORT();
	BOOL FIsDead = READ_SHORT();
	int iType = READ_SHORT();

	return TRUE;
}

MSG_FUNC(TutorLine)
{
	BEGIN_READ(pbuf, iSize);

	int iEntityId = READ_SHORT();
	int iEventId = READ_SHORT();	// unused in ReGameDLL-CS

	return TRUE;
}

MSG_FUNC(TutorState)
{
	BEGIN_READ(pbuf, iSize);

	char* psz = READ_STRING();	// just a nullptr in ReGameDLL-CS

	return TRUE;
}

MSG_FUNC(TutorClose)
{
	// this msg have no arguments.

	return TRUE;
}

MSG_FUNC(AllowSpec)
{
	BEGIN_READ(pbuf, iSize);

	BOOL FAllowSpec = READ_BYTE();

	return TRUE;
}

MSG_FUNC(BombDrop)
{
	BEGIN_READ(pbuf, iSize);

	Vector vecOrigin;
	vecOrigin.x = READ_COORD();
	vecOrigin.y = READ_COORD();
	vecOrigin.z = READ_COORD();

	BOOL FBombPlanted = READ_BYTE();

	return TRUE;
}

MSG_FUNC(BombPickup)
{
	// this msg have no arguments.

	return TRUE;
}

MSG_FUNC(ClCorpse)
{
	char szModel[64];
	Vector vOrigin;
	Vector vAngles;
	float flAnimTime;
	int iSequence;
	int iBody;
	int iTeam;
	int iIndex;
	char* pModel;
	cl_entity_t* pEntity;

	BEGIN_READ(pbuf, iSize);

	pModel = READ_STRING();
	vOrigin.x = READ_LONG() / 128.0;
	vOrigin.y = READ_LONG() / 128.0;
	vOrigin.z = READ_LONG() / 128.0;
	vAngles.x = READ_COORD();
	vAngles.y = READ_COORD();
	vAngles.z = READ_COORD();
	flAnimTime = gEngfuncs.GetClientTime() + READ_LONG() / 100.0;
	iSequence = READ_BYTE();
	iBody = READ_BYTE();
	iTeam = READ_BYTE();
	iIndex = READ_BYTE();

	Q_snprintf(szModel, sizeof(szModel), "models/player/%s/%s.mdl", pModel, pModel);
	szModel[sizeof(szModel) - 1] = '\0';

	pEntity = gEngfuncs.GetEntityByIndex(iIndex);

	if (pEntity)
	{
		vOrigin = pEntity->curstate.origin;
	}

	CreateCorpse(vOrigin, vAngles, szModel, flAnimTime, iSequence, iBody);
	return TRUE;
}

MSG_FUNC(HostagePos)
{
	BEGIN_READ(pbuf, iSize);

	BOOL FInitiation = READ_BYTE();
	int iHostageId = READ_BYTE();

	Vector vecOrigin;
	vecOrigin.x = READ_COORD();
	vecOrigin.y = READ_COORD();
	vecOrigin.z = READ_COORD();

	return TRUE;
}

MSG_FUNC(HostageK)
{
	BEGIN_READ(pbuf, iSize);

	int iHostageId = READ_BYTE();

	return TRUE;
}

MSG_FUNC(HLTV)
{
	BEGIN_READ(pbuf, iSize);

	int iParam1 = READ_BYTE();
	int iParam2 = READ_BYTE();

	return TRUE;
}

MSG_FUNC(SpecHealth)
{
	BEGIN_READ(pbuf, iSize);

	int iObservedPlayerHealth = READ_BYTE();

	return TRUE;
}

MSG_FUNC(ForceCam)
{
	BEGIN_READ(pbuf, iSize);

	// it is say that this msg is useless.
	// this function in MoE is empty.

	BOOL FForceCam = READ_BYTE();
	BOOL FChaseCam = READ_BYTE();
	BOOL FFadeToBlack = READ_BYTE();

	return TRUE;
}

MSG_FUNC(ADStop)
{
	// this msg have no arguments.
	// this function in MoE is empty.

	return TRUE;
}

MSG_FUNC(ReceiveW)
{
	BEGIN_READ(pbuf, iSize);

	int iMode = READ_BYTE();

	return TRUE;
}

MSG_FUNC(CZCareer)
{
	BEGIN_READ(pbuf, iSize);

	READ_STRING();	// type
	// some parameters.

	// does nothing in multiplayer.

	return TRUE;
}

MSG_FUNC(CZCareerHUD)
{
	BEGIN_READ(pbuf, iSize);

	READ_STRING();	// type
	// some parameters.

	// does nothing in multiplayer.

	return TRUE;
}

MSG_FUNC(ShadowIdx)
{
	BEGIN_READ(pbuf, iSize);

	g_StudioRenderer.StudioSetShadowSprite(READ_LONG());

	return TRUE;
}

MSG_FUNC(TaskTime)
{
	BEGIN_READ(pbuf, iSize);

	int iRemainingTime = READ_SHORT();
	BOOL FShouldCountDown = READ_BYTE();
	int iFadeOutDelay = READ_BYTE();

	return TRUE;
}

MSG_FUNC(Scenario)
{
	gHUD::m_scenarioStatus.MsgFunc_Scenario(iSize, pbuf);

	return TRUE;
}

MSG_FUNC(BotVoice)
{
	BEGIN_READ(pbuf, iSize);

	BOOL FActive = READ_BYTE();
	int iPlayerId = READ_BYTE();

	return TRUE;
}

MSG_FUNC(BuyClose)
{
	// this msg have no arguments.

	return TRUE;
}

MSG_FUNC(SpecHealth2)
{
	BEGIN_READ(pbuf, iSize);

	int iHealth = READ_BYTE();
	int iPlayerId = READ_BYTE();

	return TRUE;
}

MSG_FUNC(BarTime2)
{
	BEGIN_READ(pbuf, iSize);

	int iTime = READ_SHORT();
	float flStartPercent = (float)READ_SHORT() / 100.0f;

	gHUD::m_progressBar.MsgFunc_BarTime2(iTime, flStartPercent);
	return TRUE;
}

MSG_FUNC(ItemStatus)
{
	BEGIN_READ(pbuf, iSize);

	int bitsItemStatus = READ_BYTE();

	g_bHasDefuser = bitsItemStatus & ITEM_STATUS_DEFUSER;
	g_bHasNightvision = bitsItemStatus & ITEM_STATUS_NIGHTVISION;
	return TRUE;
}

MSG_FUNC(Location)
{
	BEGIN_READ(pbuf, iSize);

	int iPlayerId = READ_BYTE();
	
	Q_strlcpy(g_PlayerExtraInfo[iPlayerId].location, READ_STRING());

	return TRUE;
}

MSG_FUNC(BotProgress)
{
	BEGIN_READ(pbuf, iSize);

	int iMode = READ_BYTE();
	float flPercentage = 0.0f;
	char szText[192] = "";

	switch (iMode)
	{
	case 0:	// BOT_PROGGRESS_DRAW
		flPercentage = (float)READ_BYTE() / 100.0f;
		Q_strlcpy(szText, READ_STRING());
		break;

	case 1:	// BOT_PROGGRESS_START
		Q_strlcpy(szText, READ_STRING());
		break;

	case 2:	// BOT_PROGGRESS_HIDE
	default:
		break;
	}

	return TRUE;
}

MSG_FUNC(Brass)
{
	BEGIN_READ(pbuf, iSize);

	//READ_BYTE();	// unused, value == TE_MODEL

	Vector vecOrigin;
	vecOrigin.x = READ_COORD();
	vecOrigin.y = READ_COORD();
	vecOrigin.z = READ_COORD();

	//READ_COORD();	// unused, vecLeft.
	//READ_COORD();	// unused, it parses the client side, but does not use it
	//READ_COORD();	// unused

	Vector vecVelocity;
	vecVelocity.x = READ_COORD();
	vecVelocity.y = READ_COORD();
	vecVelocity.z = READ_COORD();

	vec_t flRotation = READ_ANGLE();
	int iModelIndex = READ_SHORT();
	int iSoundType = READ_BYTE();
	//READ_BYTE();	// unused, lifetime. by default it's 2.5sec.
	int iPlayerId = READ_BYTE();

	if (EV_IsLocal(iPlayerId))
	{
		if (!cl_righthand->value)
			vecVelocity *= -1.0f;

		vecOrigin = gEngfuncs.GetViewModel()->attachment[1];	// use the weapon attachment instead.
		vecVelocity *= 1.5f;
		vecVelocity.z += 45.0f;
	}

	EV_EjectBrass(vecOrigin, vecVelocity, flRotation, iModelIndex, iSoundType);
	return TRUE;
}

MSG_FUNC(Fog)
{
	BEGIN_READ(pbuf, iSize);

	color24 color;
	color.r = READ_BYTE();
	color.g = READ_BYTE();
	color.b = READ_BYTE();

	int a = READ_BYTE();
	int b = READ_BYTE();
	int c = READ_BYTE();
	int d = READ_BYTE();

	return TRUE;
}

MSG_FUNC(ShowTimer)
{
	// this msg have no arguments.

	return TRUE;
}

MSG_FUNC(HudTextArgs)
{
	// this stuff have a indetermind argument number.
	// in MoE this is a empty func.
	// in BTE it has a full function.

	// LUNA: I choose BTE to copy.
	gHUD::m_Message.MsgFunc_HudTextArgs(iSize, pbuf);
	return TRUE;
}

MSG_FUNC(Account)
{
	BEGIN_READ(pbuf, iSize);

	int iPlayerId = READ_BYTE();

	g_PlayerExtraInfo[iPlayerId].m_iAccount = READ_LONG();

	return TRUE;
}

MSG_FUNC(HealthInfo)
{
	BEGIN_READ(pbuf, iSize);

	int iPlayerId = READ_BYTE();

	g_PlayerExtraInfo[iPlayerId].health = READ_LONG();

	return TRUE;
}


// player.cpp
MSG_FUNC(Logo)
{
	BEGIN_READ(pbuf, iSize);

	BOOL FOn = READ_BYTE();

	return TRUE;
}


// voice_gamemgr.cpp
MSG_FUNC(VoiceMask)
{
	BEGIN_READ(pbuf, iSize);

	// UNDONE
	// this is a fucking chaos. I honestly don't know what to do.

	return TRUE;
}

MSG_FUNC(ReqState)
{
	// this msg have no arguments.
	// another voice_mgr message.

	return TRUE;
}

// export func
void Msg_Init(void)
{
	// client.cpp
	HOOK_USER_MSG(CurWeapon);
	HOOK_USER_MSG(Geiger);
	HOOK_USER_MSG(Flashlight);
	HOOK_USER_MSG(FlashBat);
	HOOK_USER_MSG(Health);
	HOOK_USER_MSG(Damage);
	HOOK_USER_MSG(Battery);
	HOOK_USER_MSG(Train);
	HOOK_USER_MSG(HudTextPro);
	HOOK_USER_MSG(HudText);
	HOOK_USER_MSG(SayText);
	HOOK_USER_MSG(TextMsg);
	HOOK_USER_MSG(WeaponList);
	HOOK_USER_MSG(ResetHUD);
	HOOK_USER_MSG(InitHUD);
	HOOK_USER_MSG(ViewMode);
	HOOK_USER_MSG(GameTitle);
	HOOK_USER_MSG(DeathMsg);
	HOOK_USER_MSG(ScoreAttrib);
	HOOK_USER_MSG(ScoreInfo);
	HOOK_USER_MSG(TeamInfo);
	HOOK_USER_MSG(TeamScore);
	HOOK_USER_MSG(GameMode);
	HOOK_USER_MSG(MOTD);
	HOOK_USER_MSG(ServerName);
	HOOK_USER_MSG(AmmoPickup);
	HOOK_USER_MSG(WeapPickup);
	HOOK_USER_MSG(ItemPickup);
	HOOK_USER_MSG(HideWeapon);
	HOOK_USER_MSG(SetFOV);
	HOOK_USER_MSG(ShowMenu);
	//HOOK_USER_MSG(ScreenShake);	// LUNA: according to Crsky, both Shake and Fade are handled by engine.
	//HOOK_USER_MSG(ScreenFade);
	HOOK_USER_MSG(AmmoX);
	HOOK_USER_MSG(SendAudio);
	HOOK_USER_MSG(RoundTime);
	HOOK_USER_MSG(Money);
	HOOK_USER_MSG(ArmorType);
	HOOK_USER_MSG(BlinkAcct);
	HOOK_USER_MSG(StatusValue);
	HOOK_USER_MSG(StatusText);
	HOOK_USER_MSG(StatusIcon);
	HOOK_USER_MSG(BarTime);
	HOOK_USER_MSG(ReloadSound);
	HOOK_USER_MSG(Crosshair);
	HOOK_USER_MSG(NVGToggle);
	HOOK_USER_MSG(Radar);
	HOOK_USER_MSG(Spectator);
	HOOK_USER_MSG(VGUIMenu);
	HOOK_USER_MSG(TutorText);
	HOOK_USER_MSG(TutorLine);
	HOOK_USER_MSG(TutorState);
	HOOK_USER_MSG(TutorClose);
	HOOK_USER_MSG(AllowSpec);
	HOOK_USER_MSG(BombDrop);
	HOOK_USER_MSG(BombPickup);
	HOOK_USER_MSG(ClCorpse);
	HOOK_USER_MSG(HostagePos);
	HOOK_USER_MSG(HostageK);
	HOOK_USER_MSG(HLTV);
	HOOK_USER_MSG(SpecHealth);
	HOOK_USER_MSG(ForceCam);
	HOOK_USER_MSG(ADStop);
	HOOK_USER_MSG(ReceiveW);
	HOOK_USER_MSG(CZCareer);
	HOOK_USER_MSG(CZCareerHUD);
	HOOK_USER_MSG(ShadowIdx);
	HOOK_USER_MSG(TaskTime);
	HOOK_USER_MSG(Scenario);
	HOOK_USER_MSG(BotVoice);
	HOOK_USER_MSG(BuyClose);
	HOOK_USER_MSG(SpecHealth2);
	HOOK_USER_MSG(BarTime2);
	HOOK_USER_MSG(ItemStatus);
	HOOK_USER_MSG(Location);
	HOOK_USER_MSG(BotProgress);
	HOOK_USER_MSG(Brass);
	HOOK_USER_MSG(Fog);
	HOOK_USER_MSG(ShowTimer);
	HOOK_USER_MSG(HudTextArgs);
	HOOK_USER_MSG(Account);
	HOOK_USER_MSG(HealthInfo);

	// player.cpp
	HOOK_USER_MSG(Logo);

	// voice_gamemgr.cpp
	HOOK_USER_MSG(VoiceMask);
	HOOK_USER_MSG(ReqState);
}
