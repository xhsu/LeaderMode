/*

Created Date: 07 Mar 2020

*/

#include "precompiled.h"

hud_player_info_t g_PlayerInfoList[MAX_PLAYERS + 1];
int g_PlayerScoreAttrib[MAX_PLAYERS + 1];
TEMPENTITY* g_DeadPlayerModels[MAX_PLAYERS + 1];
CScreenFade gScreenFadeMgr;

Vector g_LocationColor = Vector(0.0, 0.8, 0.0);

cvar_t* cl_lw = NULL;
cvar_t* cl_righthand = NULL;
cvar_t* cl_radartype = NULL;
cvar_t* cl_dynamiccrosshair = NULL;
cvar_t* cl_crosshair_color = NULL;
cvar_t* cl_crosshair_size = NULL;
cvar_t* cl_crosshair_translucent = NULL;
cvar_t* cl_crosshair_type = NULL;
cvar_t* cl_killeffect = NULL;
cvar_t* cl_killmessage = NULL;
cvar_t* cl_headname = NULL;
cvar_t* cl_newmenu = NULL;
cvar_t* cl_newmenu_drawbox = NULL;
cvar_t* cl_newradar = NULL;
cvar_t* cl_newradar_size = NULL;
cvar_t* cl_newradar_dark = NULL;
cvar_t* cl_newchat = NULL;
cvar_t* cl_shadows = NULL;
cvar_t* cl_scoreboard = NULL;
cvar_t* cl_fog_skybox = NULL;
cvar_t* cl_fog_density = NULL;
cvar_t* cl_fog_r = NULL;
cvar_t* cl_fog_g = NULL;
cvar_t* cl_fog_b = NULL;
cvar_t* cl_minmodel = NULL;
cvar_t* cl_min_t = NULL;
cvar_t* cl_min_ct = NULL;
cvar_t* cl_corpsestay = NULL;
cvar_t* cl_corpsefade = NULL;

cvar_t* hud_saytext_time = nullptr;

namespace gHUD
{
	std::list<CBaseHudElement*> m_lstHudElements;

	cvar_s* m_pCvarDraw = nullptr;

	client_sprite_t* m_pSpriteList = nullptr;
	int m_iPlayerNum = 0;
	float m_flTime = 1;
	float m_fOldTime = 0;
	double m_flTimeDelta = 1;
	int m_iFOV = 90;
	int m_iLastFOVDiff = 90;
	float m_flDisplayedFOV = 90.0f;
	int m_iSpriteCount = 0;
	int m_iSpriteCountAllRes = 0;
	int m_iRes = 640;
	std::vector<hSprite> m_rghSprites;
	std::vector<wrect_t> m_rgrcRects;
	std::vector<std::string> m_rgszSpriteNames;
	char m_szGameMode[32] = "\0";
	int m_HUD_number_0 = 0;
	int m_iFontHeight = 0;
	int m_iFontEngineHeight = 0;
	bool m_bIntermission = false;
	char m_szMOTD[2048] = "\0";
	float m_flTimeLeft = 0;
	int m_bitsHideHUDDisplay = 0;
	float m_flMouseSensitivity = 0;
	int m_PlayerFOV[MAX_CLIENTS];
	Vector m_vecOrigin = Vector();
	Vector m_vecAngles = Vector();
	int m_iKeyBits = 0;
	int m_iWeaponBits = 0;
	bool m_bPlayerDead = true;
	int m_hCambriaFont = 0;
	int m_hTrajanProFont = 0;
	GLUquadric* m_pGLUquadricHandle = nullptr;
	float m_flUCDTime = 1;
	float m_flUCDOldTime = 0;
	float m_flUCDTimeDelta = 1;
	std::array<hSprite, AMMO_MAXTYPE> m_rghAmmoSprite;
	std::array<wrect_t, AMMO_MAXTYPE> m_rgrcAmmoSprite;
	client_sprite_t* m_pAmmoTxtList = nullptr;	// original data. for others to inquiry.
	int m_iAmmoTxtListCount = 0;

	SCREENINFO m_scrinfo;

	// HUD elements.
	std::list<element_t> m_lstElements;
	//CHudHealth m_Health;
	//CHudSpectator m_Spectator;
	CHudGeiger m_Geiger;
	//CHudBattery m_Battery;
	CHudTrain m_Train;
	//CHudFlashlight m_Flash;
	CHudMessage m_Message;
	CHudStatusBar m_StatusBar;
	//CHudDeathNotice m_DeathNotice;
	CHudSayText m_SayText;
	CHudMenu m_Menu;
	CHudNightVision m_NightVision;
	CHudTextMessage m_TextMessage;
	CHudRoundTimer m_roundTimer;
	//CHudAccountBalance m_accountBalance;
	CHudHeadName m_headName;
	//CHudRadar m_Radar;
	//CHudStatusIcons m_StatusIcons;
	//CHudScenarioStatus m_scenarioStatus;
	CHudProgressBar m_progressBar;
	CHudVGUI2Print m_VGUI2Print;
	CHudSniperScope m_SniperScope;
	//CHudCrosshair m_Crosshair;
	CHudWeaponList m_WeaponList;
	//CHudGrenade m_Grenade;
	CHudScoreboard m_Scoreboard;
	//CHudClassIndicator m_ClassIndicator;
	CUIBuyMenu m_UI_BuyMenu;
};

void gHUD::Init(void)
{
	m_lstHudElements.clear();
	m_lstElements.clear();

	/* UNDONE
	if (gConfigs.bEnableClientUI)
		InstallCounterStrikeViewportHook();
		*/

	m_pSpriteList = nullptr;
	m_iPlayerNum = 0;
	m_flTime = 1;
	m_iFOV = 0;

	m_flTime = 1;
	m_fOldTime = 0;
	m_flTimeDelta = 1;
	m_flUCDTime = 1;
	m_flUCDOldTime = 0;
	m_flUCDTimeDelta = 1;

	// we can't use this in init() since all these elements are adding themselves into std::list in init().
	/*for (auto pHudElement : m_lstHudElements)
	{
		pHudElement->Init();
	}*/

	// instead, we should:
	AddElementsToList<CHudRadar, CHudClassIndicator, CHudBattery, CHudHealth,	// Bottom-left. Everything is depened on CRadar, like a motherboard.
		CHudMatchStatus,	// Top.
		CHudStatusIcons, CHudAccountBalance, CHudDeathNotice,	// Top-right, everything else depends on CHudStatusIcons.
		CHudEquipments,	CHudAmmo,// Bottom-right. Everything is depend on CHudEquipments.
		CHudCrosshair, CHudSpectator>();
	//m_Health.Init();
	//m_SayText.Init();	// m_SayText should place before m_Spectator, since m_Spectator.init() is calling some vars from m_SayText.Init().
	//m_Spectator.Init();
	//m_Geiger.Init();
	//m_Battery.Init();
	//m_Train.Init();
	//m_Flash.Init();
	//m_Message.Init();
	//m_StatusBar.Init();
	//m_TextMessage.Init();
	//m_roundTimer.Init();
	//m_accountBalance.Init();
	//m_headName.Init();
	//m_Radar.Init();
	//m_StatusIcons.Init();
	//m_scenarioStatus.Init();
	//m_progressBar.Init();
	//m_VGUI2Print.Init();
	//m_Grenade.Init();
	//m_SniperScope.Init();	// this is a important borderline. Any HUD should not be block by Scope DDS should place behind on this.
	//m_DeathNotice.Init();
	//m_Menu.Init();
	//m_NightVision.Init();
	//m_Crosshair.Init();
	//m_WeaponList.Init();
	//m_ClassIndicator.Init();
	//m_Scoreboard.Init();	// this is definately the last layer.

	// UI is always above all other HUD elements.
	m_UI_BuyMenu.Init();

	// UNDONE
	//GetClientVoice()->Init(&g_VoiceStatusHelper);

	m_pCvarDraw = gEngfuncs.pfnGetCvarPointer("hud_draw");

	if (!m_pCvarDraw)
		m_pCvarDraw = gEngfuncs.pfnRegisterVariable("hud_draw", "1", FCVAR_ARCHIVE);

	cl_lw = gEngfuncs.pfnGetCvarPointer("cl_lw");	// engine cvar
	cl_righthand = gEngfuncs.pfnRegisterVariable("cl_righthand", "1", FCVAR_ARCHIVE);	// engine cvar
	cl_minmodel = gEngfuncs.pfnGetCvarPointer("cl_minmodel");
	cl_min_t = gEngfuncs.pfnGetCvarPointer("cl_min_t");
	cl_min_ct = gEngfuncs.pfnGetCvarPointer("cl_min_ct");
	cl_corpsestay = gEngfuncs.pfnRegisterVariable("cl_corpsestay", "600", FCVAR_ARCHIVE);
	cl_corpsefade = gEngfuncs.pfnRegisterVariable("cl_corpsefade", "0", FCVAR_ARCHIVE);
	cl_crosshair_type = gEngfuncs.pfnRegisterVariable("cl_crosshair_type", "0", FCVAR_ARCHIVE);
	cl_killeffect = gEngfuncs.pfnRegisterVariable("cl_killeffect", "1", FCVAR_ARCHIVE);
	cl_killmessage = gEngfuncs.pfnRegisterVariable("cl_killmessage", "1", FCVAR_ARCHIVE);
	cl_headname = gEngfuncs.pfnRegisterVariable("cl_headname", "1", FCVAR_ARCHIVE);
	cl_newmenu = gEngfuncs.pfnRegisterVariable("cl_newmenu", "1", FCVAR_ARCHIVE);
	cl_newmenu_drawbox = gEngfuncs.pfnRegisterVariable("cl_newmenu_drawbox", "1", FCVAR_ARCHIVE);
	cl_newradar = gEngfuncs.pfnRegisterVariable("cl_newradar", "1", FCVAR_ARCHIVE);
	cl_newradar_size = gEngfuncs.pfnRegisterVariable("cl_newradar_size", "0.175", FCVAR_ARCHIVE);
	cl_newradar_dark = gEngfuncs.pfnRegisterVariable("cl_newradar_dark", "0.8", FCVAR_ARCHIVE);
	cl_newchat = gEngfuncs.pfnRegisterVariable("cl_newchat", "1", FCVAR_ARCHIVE);
	cl_scoreboard = gEngfuncs.pfnRegisterVariable("cl_scoreboard", "1", FCVAR_ARCHIVE);

	cl_fog_density = gEngfuncs.pfnRegisterVariable("cl_fog_density", "0", 0);
	cl_fog_r = gEngfuncs.pfnRegisterVariable("cl_fog_r", "0", 0);
	cl_fog_g = gEngfuncs.pfnRegisterVariable("cl_fog_g", "0", 0);
	cl_fog_b = gEngfuncs.pfnRegisterVariable("cl_fog_b", "0", 0);
	cl_fog_skybox = gEngfuncs.pfnRegisterVariable("cl_fog_skybox", "0", 0);

	m_iFontEngineHeight = VGUI_SURFACE->GetFontTall(font);

	/* UNDONE
	if (gConfigs.bEnableClientUI)
		g_pViewPort->Init();
	*/

	gEngfuncs.pfnAddCommand("slot1", CommandFunc_Slot1);
	gEngfuncs.pfnAddCommand("slot2", CommandFunc_Slot2);
	gEngfuncs.pfnAddCommand("slot3", CommandFunc_Slot3);
	gEngfuncs.pfnAddCommand("slot4", CommandFunc_Slot4);
	gEngfuncs.pfnAddCommand("slot5", CommandFunc_Slot5);
	gEngfuncs.pfnAddCommand("slot6", CommandFunc_Slot6);
	gEngfuncs.pfnAddCommand("slot7", CommandFunc_Slot7);
	gEngfuncs.pfnAddCommand("slot8", CommandFunc_Slot8);
	gEngfuncs.pfnAddCommand("slot9", CommandFunc_Slot9);
	gEngfuncs.pfnAddCommand("slot10", CommandFunc_Slot10);
	gEngfuncs.pfnAddCommand("cancelselect", CommandFunc_CancelSelect); 
	gEngfuncs.pfnAddCommand("invnext", CommandFunc_NextWeapon);
	gEngfuncs.pfnAddCommand("invprev", CommandFunc_PrevWeapon);
	gEngfuncs.pfnAddCommand("lastinv", CommandFunc_SelectLastItem);
	gEngfuncs.pfnAddCommand("eqpnext", CommandFunc_NextEquipment);
	gEngfuncs.pfnAddCommand("eqpprev", CommandFunc_PrevEquipment);
	gEngfuncs.pfnAddCommand("changemode", CommandFunc_AlterAct);
	gEngfuncs.pfnAddCommand("updateoverview", &OverviewMgr::OnHUDReset);
	gEngfuncs.pfnAddCommand("showteam", []() {g_pViewport->m_pTeamMenu->Show(!g_pViewport->m_pTeamMenu->IsVisible()); });
	gEngfuncs.pfnAddCommand("declarerole", []() {g_pViewport->m_pRoleMenu->Show(!g_pViewport->m_pRoleMenu->IsVisible()); });
}

void gHUD::Shutdown(void)
{
	m_lstHudElements.clear();
	m_lstElements.clear();

	for (auto& hudpfns : m_lstElements)
	{
		if (hudpfns.pfnShutdown)
			(*hudpfns.pfnShutdown)();
	}
}

void gHUD::VidInit(void)
{
	hud_saytext_time = gEngfuncs.pfnRegisterVariable("hud_saytext_time", "5", 0);

	cl_dynamiccrosshair = CVAR_CREATE("cl_dynamiccrosshair", "1", FCVAR_ARCHIVE);
	cl_radartype = CVAR_CREATE("cl_radartype", "0", FCVAR_ARCHIVE);
	cl_crosshair_color = CVAR_CREATE("cl_crosshair_color", "50 250 50", FCVAR_ARCHIVE);
	cl_crosshair_size = CVAR_CREATE("cl_crosshair_size", "auto", FCVAR_ARCHIVE);
	cl_crosshair_translucent = CVAR_CREATE("cl_crosshair_translucent", "1", FCVAR_ARCHIVE);
	cl_shadows = CVAR_CREATE("cl_shadows", "1", FCVAR_ARCHIVE);

	m_scrinfo.iSize = sizeof(m_scrinfo);
	gEngfuncs.pfnGetScreenInfo(&m_scrinfo);

	if (ScreenWidth < 640)
		m_iRes = 320;
	else
		m_iRes = 640;

	if (!m_pSpriteList)
	{
		m_pSpriteList = gEngfuncs.pfnSPR_GetList("sprites/hud.txt", &m_iSpriteCountAllRes);

		if (m_pSpriteList)
		{
			m_iSpriteCount = 0;
			client_sprite_t* p = m_pSpriteList;

			for (int j = 0; j < m_iSpriteCountAllRes; j++)
			{
				if (p->iRes == m_iRes)
					m_iSpriteCount++;

				p++;
			}

			m_rghSprites.resize(m_iSpriteCount);
			m_rgrcRects.resize(m_iSpriteCount);
			m_rgszSpriteNames.resize(m_iSpriteCount);
			p = m_pSpriteList;

			int index = 0;

			for (int j = 0; j < m_iSpriteCountAllRes; j++)
			{
				if (p->iRes == m_iRes)
				{
					char sz[256];
					Q_snprintf(sz, sizeof(sz) - 1, "sprites/%s.spr", p->szSprite);
					m_rghSprites[index] = gEngfuncs.pfnSPR_Load(sz);
					m_rgrcRects[index] = p->rc;
					m_rgszSpriteNames[index] = p->szName;

					index++;
				}

				p++;
			}
		}
	}
	else
	{
		client_sprite_t* p = m_pSpriteList;
		int index = 0;

		for (int j = 0; j < m_iSpriteCountAllRes; j++)
		{
			if (p->iRes == m_iRes)
			{
				char sz[256];
				Q_snprintf(sz, sizeof(sz) - 1, "sprites/%s.spr", p->szSprite);
				m_rghSprites[index] = gEngfuncs.pfnSPR_Load(sz);
				index++;
			}

			p++;
		}
	}

	m_pAmmoTxtList = gEngfuncs.pfnSPR_GetList("sprites/ammo.txt", &m_iAmmoTxtListCount);

	if (m_pAmmoTxtList)
	{
		char sz[128];
		client_sprite_t* p = nullptr;

		m_rghAmmoSprite.fill(0);
		m_rgrcAmmoSprite.fill({ 0, 0, 0, 0 });

		for (int j = 0; j < AMMO_MAXTYPE; j++)
		{
			if (!g_rgAmmoInfo[j].m_pszName || g_rgAmmoInfo[j].m_pszName[0] == '\0')
				continue;

			p = gHUD::GetSpriteFromList(m_pAmmoTxtList, g_rgAmmoInfo[j].m_pszName, 640, m_iAmmoTxtListCount);

			if (p)
			{
				Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
				m_rghAmmoSprite[j] = gEngfuncs.pfnSPR_Load(sz);
				m_rgrcAmmoSprite[j] = p->rc;
			}
			else
				m_rghAmmoSprite[j] = NULL;
		}
	}

	m_iPlayerNum = 0;
	m_szGameMode[0] = '\0';
	m_HUD_number_0 = GetSpriteIndex("number_0");
	m_iFontHeight = m_rgrcRects[m_HUD_number_0].bottom - m_rgrcRects[m_HUD_number_0].top;
	m_bIntermission = 0;
	m_szMOTD[0] = 0;
	m_flTimeLeft = 0;

	for (auto& pHudElement : m_lstHudElements)
	{
		pHudElement->VidInit();
	}

	// UNDONE
	//GetClientVoiceHud()->VidInit();

	m_iFontEngineHeight = VGUI_SURFACE->GetFontTall(font);

	// custom font function set.
	m_hCambriaFont = gFontFuncs::CreateFont();
	gFontFuncs::AddGlyphSetToFont(m_hCambriaFont, "Cambria", 24, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
	gFontFuncs::AddGlyphSetToFont(m_hCambriaFont, "TW-Kai", 24, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
	m_hTrajanProFont = gFontFuncs::CreateFont();
	gFontFuncs::AddGlyphSetToFont(m_hTrajanProFont, "Trajan Pro", 24, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
	gFontFuncs::AddGlyphSetToFont(m_hTrajanProFont, "I.MingCP", 24, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);

	// UNDONE
	//if (gConfigs.bEnableClientUI)
		//g_pViewPort->VidInit();

	m_pGLUquadricHandle = gluNewQuadric();

	for (auto& hudpfns : m_lstElements)
	{
		if (hudpfns.pfnConnectToServer)
			(*hudpfns.pfnConnectToServer)();
	}
}

int gHUD::Redraw(float flTime, int intermission)
{
	m_scrinfo.iSize = sizeof(m_scrinfo);
	gEngfuncs.pfnGetScreenInfo(&m_scrinfo);	// LUNA: why keep doing this?

	for (int i = 1; i <= MAX_CLIENTS; i++)
	{
		gEngfuncs.pfnGetPlayerInfo(i, &g_PlayerInfoList[i]);

		if (g_PlayerInfoList[i].thisplayer)
			m_iPlayerNum = i;
	}

	m_fOldTime = m_flTime;
	m_flTime = flTime;
	m_flTimeDelta = (double)m_flTime - m_fOldTime;

	if (m_flTimeDelta < 0)
		m_flTimeDelta = 0;
	/* UNDONE
	if (gConfigs.bEnableClientUI)
	{
		if (m_bIntermission && !intermission)
		{
			m_bIntermission = intermission;

			g_pViewPort->HideAllVGUIMenu();
			g_pViewPort->UpdateSpectatorPanel();
		}
		else if (!m_bIntermission && intermission)
		{
			m_bIntermission = intermission;

			g_pViewPort->HideAllVGUIMenu();
			g_pViewPort->ShowScoreBoard();
			g_pViewPort->UpdateSpectatorPanel();
		}
	}
	*/
	m_bIntermission = intermission;

	if (m_pCvarDraw->value && !g_pViewport->IsAnyClientUIUsingMouse())	// Hide hud in any interactive screen.
	{
		for (auto& pHudElements : m_lstHudElements)
		{
			if (!intermission)
			{
				if (m_bitsHideHUDDisplay & HIDEHUD_ALL)
					break;

				if (pHudElements->m_bitsFlags & HUD_ACTIVE)
					pHudElements->Draw(flTime);
			}
			else
			{
				if (pHudElements->m_bitsFlags & HUD_INTERMISSION)
					pHudElements->Draw(flTime);
			}
		}

		for (auto& hudpfns : m_lstElements)
		{
			if (hudpfns.pfnDraw)
				(*hudpfns.pfnDraw)(flTime, m_bIntermission);
		}
	}

	// UNDONE
	//if (gConfigs.bEnableClientUI)
		//g_pViewPort->SetPaintEnabled(m_pCvarDraw->value);

	// draw a cursor.
	if (!g_bMouseControlledByGame)
	{
		int x = 0, y = 0;
		gEngfuncs.GetMousePosition(&x, &y);
		gEngfuncs.pfnFillRGBABlend(x, y, 2, 2, 255, 255, 255, 255);
	}

	// FIXME: deserted code: draw a disk on the screen.
	//glTranslatef(ScreenWidth / 2, ScreenHeight / 2, 0);
	//gluPartialDisk(m_pGLUquadricHandle, 30.0, 40.0, 90, 4, 0.0, 90.0);
	gScreenFadeMgr.Think();
	gScreenFadeMgr.Draw();
	return 1;
}

float g_lastFOV = 0.0f;

float HUD_GetFOV(void)
{
	// UNDONE
	//if (gEngfuncs.pDemoAPI->IsPlayingback())
		//g_lastFOV = g_demozoom;

	return g_lastFOV;
}

void gHUD::Think(void)
{
	// Is Player Dead??
	m_bPlayerDead = CL_IsDead();

	/*int newfov = HUD_GetFOV();

	if (newfov == 0)
		m_iFOV = DEFAULT_FOV;
	else
		m_iFOV = newfov;*/

	if (m_iFOV == DEFAULT_FOV)
		m_flMouseSensitivity = 0;
	else
		m_flMouseSensitivity = sensitivity->value * (m_flDisplayedFOV / (float)DEFAULT_FOV) * zoom_sensitivity_ratio->value;

	if (m_iFOV == 0)
		m_iFOV = Q_max<float>(DEFAULT_FOV, 90.0f);

	if (gEngfuncs.IsSpectateOnly())
	{
		if (g_iUser1 == OBS_IN_EYE && g_iUser2 != 0)
		{
			if (g_iUser2 <= gEngfuncs.GetMaxClients())
				m_iFOV = m_PlayerFOV[g_iUser2];
			else
				m_iFOV = Q_max<float>(DEFAULT_FOV, 90.0f);
		}
		else
			m_iFOV = CHudSpectator::m_flFOV;
	}

	// make FOV transition nice and smooth.
	m_flDisplayedFOV += (float(m_iFOV) - m_flDisplayedFOV) * m_flTimeDelta * 7.0f;	// this 7.0 is the transition speed.

	// draw or not, you must think.
	for (auto& pHudElements : m_lstHudElements)
	{
		if (pHudElements->m_bitsFlags & HUD_ACTIVE || pHudElements->m_bitsFlags & HUD_ENFORCE_THINK)
			pHudElements->Think();
	}

	for (auto& hudpfns : m_lstElements)
	{
		if (hudpfns.pfnThink)
			(*hudpfns.pfnThink)();
	}
}

int gHUD::UpdateClientData(client_data_t* cdata, float time)
{
	Q_memcpy(m_vecOrigin, cdata->origin, sizeof(vec3_t));
	Q_memcpy(m_vecAngles, cdata->viewangles, sizeof(vec3_t));

	m_iKeyBits = CL_ButtonBits(0);
	m_iWeaponBits = cdata->iWeaponBits;

	// therefore, UCD & Think() should use this set of time.
	m_flUCDOldTime = m_flUCDTime;
	m_flUCDTime = time;
	m_flUCDTimeDelta = m_flUCDTime - m_flUCDOldTime;

	Think();

	// this is how m_iFOV have its effect.
	// however, we use m_flDisplayedFOV to make a Source-liked effect.
	cdata->fov = m_flDisplayedFOV;

	CL_ResetButtonBits(m_iKeyBits);
	return 1;
}

void gHUD::CalcRefdef(ref_params_s* pparams)
{
}

bool gHUD::KeyEvent(bool bDown, int iKeyIndex, const char* pszCurrentBinding)	// Return true to allow engine to process the key, otherwise, act on it as needed
{
	if (!g_bInGameWorld)
		return true;

	if (pszCurrentBinding && !Q_strcmp(pszCurrentBinding, "buy"))	// allow the exit from buy menu.
		return true;

	bool bGoToEngine = true;

	if (!gHUD::m_UI_BuyMenu.m_Baseboard.KeyEvent(bDown, iKeyIndex, pszCurrentBinding))
		bGoToEngine = false;

	return (g_bInGameWorld && bGoToEngine);	// if it is not in the game world, of course it should be controlled by engine. otherwise, consider it.
}

int gHUD::GetSpriteIndex(const char* SpriteName)
{
	for (int i = 0; i < m_iSpriteCount; i++)
	{
		if (Q_strncmp(SpriteName, m_rgszSpriteNames[i].c_str(), MAX_SPRITE_NAME_LENGTH) == 0)
			return i;
	}

	return -1;
}

int gHUD::DrawHudString(int xpos, int ypos, int iMaxX, const char* szIt, int r, int g, int b)
{
	xpos += gEngfuncs.pfnDrawString(xpos, ypos, szIt, r, g, b);
	return xpos;
}

int gHUD::DrawHudNumberString(int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b)
{
	char szString[32];
	Q_snprintf(szString, sizeof(szString) - 1, "%d", iNumber);
	return DrawHudStringReverse(xpos, ypos, iMinX, szString, r, g, b);
}

int gHUD::DrawHudStringReverse(int xpos, int ypos, int iMinX, const char* szString, int r, int g, int b)
{
	xpos -= gEngfuncs.pfnDrawStringReverse(xpos, ypos, szString, r, g, b);
	return xpos;
}

int gHUD::DrawHudNumber(int x, int y, int iFlags, int iNumber, int r, int g, int b)
{
	int iWidth = GetSpriteRect(m_HUD_number_0)->right - GetSpriteRect(m_HUD_number_0)->left;
	int k;

	if (iNumber > 0)
	{
		if (iNumber >= 10000)
		{
			k = iNumber / 10000;
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		if (iNumber >= 1000)
		{
			k = (iNumber % 10000) / 1000;
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		if (iNumber >= 100)
		{
			k = (iNumber % 1000) / 100;
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		if (iNumber >= 10)
		{
			k = (iNumber % 100) / 10;
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		k = iNumber % 10;
		gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0 + k));
		x += iWidth;
	}
	else if (iFlags & DHN_DRAWZERO)
	{
		gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0), r, g, b);

		if (iFlags & (DHN_5DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		if (iFlags & (DHN_5DIGITS | DHN_4DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0));
		x += iWidth;
	}

	return x;
}

int gHUD::DrawHudNumber(int x, int y, int iNumber, int r, int g, int b)
{
	static char szBuffer[16];
	const char* pszPosint;
	int iWidth = GetSpriteRect(m_HUD_number_0)->right - GetSpriteRect(m_HUD_number_0)->left;
	int k;

	Q_snprintf(szBuffer, sizeof(szBuffer) - 1, "%5i", iNumber);
	pszPosint = szBuffer;

	if (iNumber < 0)
		pszPosint++;

	while (*pszPosint)
	{
		k = *pszPosint - '0';

		if (k >= 0 && k <= 9)
		{
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, GetSpriteRect(m_HUD_number_0 + k));
		}

		x += iWidth;
		pszPosint++;
	}

	return x;
}

int gHUD::GetNumWidth(int iNumber, int iFlags)
{
	if (iFlags & (DHN_5DIGITS))
		return 5;

	if (iFlags & (DHN_4DIGITS))
		return 4;

	if (iFlags & (DHN_3DIGITS))
		return 3;

	if (iFlags & (DHN_2DIGITS))
		return 2;

	if (iNumber <= 0)
	{
		if (iFlags & (DHN_DRAWZERO))
			return 1;
		else
			return 0;
	}

	if (iNumber < 10)
		return 1;

	if (iNumber < 100)
		return 2;

	if (iNumber < 1000)
		return 3;

	if (iNumber < 10000)
		return 4;

	return 5;
}

int gHUD::GetNumBits(int iNumber)
{
	int k = iNumber;
	int bits = 0;

	while (k)
	{
		k /= 10;
		bits++;
	}

	return bits;
}

void gHUD::AddHudElem(CBaseHudElement* phudelem)
{
	m_lstHudElements.push_back(phudelem);
}

float gHUD::GetSensitivity(void)
{
	return m_flMouseSensitivity;
}

hSprite gHUD::GetSprite(int index)
{
	return (index < 0) ? 0 : m_rghSprites[index];
}

const wrect_t* gHUD::GetSpriteRect(int index)
{
	return &m_rgrcRects[index];
}

client_sprite_t* gHUD::GetSpriteFromList(client_sprite_t* pList, const char* psz, int iRes, int iCount)
{
	if (!pList)
		return nullptr;

	int i = iCount;
	client_sprite_t* p = pList;

	while (i--)
	{
		if (!Q_stricmp(psz, p->szName) && p->iRes == iRes)
			return p;

		p++;
	}

	return nullptr;
}

void gHUD::GetSprite(client_sprite_t* p, hSprite& hSPR, wrect_t& rcSPR)
{
	char sz[128];

	if (p)
	{
		Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
		hSPR = gEngfuncs.pfnSPR_Load(sz);
		rcSPR = p->rc;
	}
	else
	{
		hSPR = NULL;
		Q_memset(&rcSPR, NULL, sizeof(rcSPR));
	}
}

static constexpr Vector GODFATHER_COLOR_DIFF = VEC_SPRINGGREENISH - VEC_T_COLOUR;
static constexpr Vector COMMANDER_COLOR_DIFF = VEC_SPRINGGREENISH - VEC_CT_COLOUR;

Vector gHUD::GetColor(size_t iPlayerIndex)
{
	switch (g_PlayerExtraInfo[iPlayerIndex].m_iRoleType)
	{
	case Role_Arsonist:
	case Role_Assassin:
	case Role_LeadEnforcer:
	case Role_MadScientist:
	case Role_Breacher:
	case Role_Medic:
	case Role_Sharpshooter:
	case Role_SWAT:
	default:
		switch (g_PlayerExtraInfo[iPlayerIndex].m_iTeam)
		{
		case TEAM_CT:
			return VEC_CT_COLOUR;

		case TEAM_TERRORIST:
			return VEC_T_COLOUR;

		case TEAM_SPECTATOR:
			return VEC_GRAY;

		default:
			return VEC_YELLOWISH;
		}

	case Role_Commander:
		return VEC_CT_COLOUR + GetOscillation() * COMMANDER_COLOR_DIFF;

	case Role_Godfather:
		return VEC_T_COLOUR + GetOscillation() * GODFATHER_COLOR_DIFF;
	}
}

bool gHUD::GetSprite(const char* szSpriteName, hSprite* phSPR, const wrect_t** pprcSPR)
{
	auto index = GetSpriteIndex(szSpriteName);

	if (!index)
		return false;

	if (phSPR)
		*phSPR = GetSprite(index);

	if (pprcSPR)
		*pprcSPR = GetSpriteRect(index);

	return true;
}

float gHUD::GetOscillation(float omega)
{
	return (Q_sin(gHUD::m_flTime * omega) + 1.0f) / 2.0f;
}

float gHUD::GetOscillationUnfreezable(float omega)
{
	return (Q_sin(gHUD::m_flUCDTime * omega) + 1.0f) / 2.0f;
}

void gHUD::AddFontFiles(void)
{
	gFontFuncs::AddCustomFontFile("resource/fonts/716.TTF");
}

void gHUD::SlotInput(int iSlot)
{
	if (m_bIntermission)
		return;

	// UNDONE
	//if (gViewPortInterface && gViewPortInterface->SlotInput(iSlot))
		//return;

	if (m_Menu.m_bMenuDisplayed)
	{
		m_Menu.SelectMenuItem(iSlot);	// never -1 over here.
		return;
	}

	if (iSlot <= SLOT_NO || iSlot >= MAX_ITEM_TYPES)
		return;

	const char* psz = g_rgWpnInfo[gHUD::m_WeaponList.m_rgiWeapons[iSlot]].m_pszInternalName;
	if (psz && Q_strlen(psz))
	{
		char sz[128];
		Q_strlcpy(sz, psz);
		gEngfuncs.pfnServerCmd(sz);
		gPseudoPlayer.StartSwitchingWeapon(gHUD::m_WeaponList.m_rgiWeapons[iSlot]);

		gEngfuncs.pfnPlaySoundByName(WEAPONLIST_SELECT_SFX, VOL_NORM);
	}
	else
	{
		gEngfuncs.pfnPlaySoundByName(WEAPONLIST_EMPTY_SFX, VOL_NORM);
	}
}

void CommandFunc_Slot1(void) { gHUD::SlotInput(1); }
void CommandFunc_Slot2(void) { gHUD::SlotInput(2); }
void CommandFunc_Slot3(void) { gHUD::SlotInput(3); }
void CommandFunc_Slot4(void) { gHUD::SlotInput(4); }
void CommandFunc_Slot5(void) { gHUD::SlotInput(5); }
void CommandFunc_Slot6(void) { gHUD::SlotInput(6); }
void CommandFunc_Slot7(void) { gHUD::SlotInput(7); }
void CommandFunc_Slot8(void) { gHUD::SlotInput(8); }
void CommandFunc_Slot9(void) { gHUD::SlotInput(9); }
void CommandFunc_Slot10(void) { gHUD::SlotInput(10); }

void CommandFunc_CancelSelect(void)
{
	gEngfuncs.pfnClientCmd("escape\n");	// Just forword this.
}

void CommandFunc_NextWeapon(void)
{
	if (gHUD::m_bPlayerDead)
		return;

	if (!g_pCurWeapon)
		return;

	bool bFound = false;
	WeaponIdType iId = WEAPON_NONE;

	for (int i = g_pCurWeapon->m_pItemInfo->m_iSlot + 1; i < MAX_ITEM_TYPES; i++)
	{
		if (gHUD::m_WeaponList.m_rgiWeapons[i] > WEAPON_NONE && gHUD::m_WeaponList.m_rgiWeapons[i] < LAST_WEAPON)
		{
			bFound = true;
			iId = gHUD::m_WeaponList.m_rgiWeapons[i];
			break;
		}
	}

	if (!bFound)
	{
		for (int i = PRIMARY_WEAPON_SLOT; i < MAX_ITEM_TYPES; i++)
		{
			if (gHUD::m_WeaponList.m_rgiWeapons[i] > WEAPON_NONE && gHUD::m_WeaponList.m_rgiWeapons[i] < LAST_WEAPON)
			{
				bFound = true;
				iId = gHUD::m_WeaponList.m_rgiWeapons[i];
				break;
			}
		}
	}

	if (bFound)
	{
		const char* psz = g_rgWpnInfo[iId].m_pszInternalName;
		if (psz && Q_strlen(psz))
		{
			char sz[128];
			Q_strlcpy(sz, psz);
			gEngfuncs.pfnServerCmd(sz);
			gPseudoPlayer.StartSwitchingWeapon(iId);

			gEngfuncs.pfnPlaySoundByName(WEAPONLIST_WHEEL_SFX, VOL_NORM);
			return;
		}
	}

	gEngfuncs.pfnPlaySoundByName(WEAPONLIST_EMPTY_SFX, VOL_NORM);
}

void CommandFunc_PrevWeapon(void)
{
	if (gHUD::m_bPlayerDead)
		return;

	if (!g_pCurWeapon)
		return;

	bool bFound = false;
	WeaponIdType iId = WEAPON_NONE;

	for (int i = g_pCurWeapon->m_pItemInfo->m_iSlot - 1; i > SLOT_NO; i--)
	{
		if (gHUD::m_WeaponList.m_rgiWeapons[i] > WEAPON_NONE&& gHUD::m_WeaponList.m_rgiWeapons[i] < LAST_WEAPON)
		{
			bFound = true;
			iId = gHUD::m_WeaponList.m_rgiWeapons[i];
			break;
		}
	}

	if (!bFound)
	{
		for (int i = EQUIPMENT_SLOT; i > SLOT_NO; i--)
		{
			if (gHUD::m_WeaponList.m_rgiWeapons[i] > WEAPON_NONE&& gHUD::m_WeaponList.m_rgiWeapons[i] < LAST_WEAPON)
			{
				bFound = true;
				iId = gHUD::m_WeaponList.m_rgiWeapons[i];
				break;
			}
		}
	}

	if (bFound)
	{
		const char* psz = g_rgWpnInfo[iId].m_pszInternalName;
		if (psz && Q_strlen(psz))
		{
			char sz[128];
			Q_strlcpy(sz, psz);
			gEngfuncs.pfnServerCmd(sz);
			gPseudoPlayer.StartSwitchingWeapon(iId);

			gEngfuncs.pfnPlaySoundByName(WEAPONLIST_WHEEL_SFX, VOL_NORM);
			return;
		}
	}

	gEngfuncs.pfnPlaySoundByName(WEAPONLIST_EMPTY_SFX, VOL_NORM);
}

void CommandFunc_SelectLastItem(void)	// an equivlent function of void CBasePlayer::SelectLastItem() on SV.
{
	// this cmd can cancel grenade throw aswell.
	if (g_pCurWeapon && g_pCurWeapon->m_bitsFlags & WPNSTATE_QUICK_THROWING)
	{
		g_pCurWeapon->m_bitsFlags |= WPNSTATE_QT_EXIT;
		gPseudoPlayer.m_flNextAttack = 0;
		goto LAB_LASTINV_END;
	}

	if (gPseudoPlayer.m_pActiveItem && !gPseudoPlayer.m_pActiveItem->CanHolster())
		goto LAB_LASTINV_END;

	if (!gPseudoPlayer.m_pLastItem || gPseudoPlayer.m_pLastItem == gPseudoPlayer.m_pActiveItem)
	{
		for (int i = PRIMARY_WEAPON_SLOT; i < MAX_ITEM_TYPES; i++)
		{
			if (g_rgpClientWeapons[gHUD::m_WeaponList.m_rgiWeapons[i]] && g_rgpClientWeapons[gHUD::m_WeaponList.m_rgiWeapons[i]] != gPseudoPlayer.m_pActiveItem)
			{
				gPseudoPlayer.m_pLastItem = g_rgpClientWeapons[gHUD::m_WeaponList.m_rgiWeapons[i]];
				break;
			}
		}
	}

	if (!gPseudoPlayer.m_pLastItem || gPseudoPlayer.m_pLastItem == gPseudoPlayer.m_pActiveItem)
		goto LAB_LASTINV_END;

	// on client, we only needs to do one thing.
	gPseudoPlayer.StartSwitchingWeapon(gPseudoPlayer.m_pLastItem);

LAB_LASTINV_END:
	// don't forget to forward this command to SV.
	gEngfuncs.pfnServerCmd("lastinv\n");
}

EquipmentIdType FindNextEquipment(bool bLoopFromStart, EquipmentIdType iStartingFrom)
{
	iStartingFrom = (EquipmentIdType)Q_clamp((int)iStartingFrom, EQP_NONE + 1, EQP_COUNT - 1);

	AmmoIdType iAmmoId = AMMO_NONE;
	EquipmentIdType iCandidate = EQP_NONE;

	for (int i = iStartingFrom + 1; i < EQP_COUNT; i++)
	{
		iAmmoId = GetAmmoIdOfEquipment((EquipmentIdType)i);

		if ((!iAmmoId || gPseudoPlayer.m_rgAmmo[iAmmoId] <= 0) && !gPseudoPlayer.m_rgbHasEquipment[i])	// you can still selection some item even if it has no ammo. These items are not to be consumed.
			continue;

		iCandidate = (EquipmentIdType)i;
		break;
	}

	if (!iCandidate && bLoopFromStart)
	{
		for (int i = EQP_NONE; i < EQP_COUNT; i++)
		{
			iAmmoId = GetAmmoIdOfEquipment((EquipmentIdType)i);

			if ((!iAmmoId || gPseudoPlayer.m_rgAmmo[iAmmoId] <= 0) && !gPseudoPlayer.m_rgbHasEquipment[i])
				continue;

			iCandidate = (EquipmentIdType)i;
			break;
		}
	}

	// Loop back to self: no found.
	if (iCandidate == iStartingFrom)
		iCandidate = EQP_NONE;

	return iCandidate;
}

void CommandFunc_NextEquipment(void)
{
	// the drawing sequence is the select sequence. and the drawing sequence is the index.

	// you can't do this on the halfway.
	if (g_pCurWeapon && g_pCurWeapon->m_bitsFlags & WPNSTATE_QUICK_THROWING)
		return;

	gPseudoPlayer.m_iUsingGrenadeId = FindNextEquipment(true);
	gEngfuncs.pfnServerCmd(SharedVarArgs("eqpselect %d\n", gPseudoPlayer.m_iUsingGrenadeId));

	CHudEquipments::OnNext();
}

EquipmentIdType FindLastEquipment(bool bLoopFromEnd, EquipmentIdType iStartingFrom)
{
	iStartingFrom = (EquipmentIdType)Q_clamp((int)iStartingFrom, EQP_NONE + 1, EQP_COUNT - 1);

	AmmoIdType iAmmoId = AMMO_NONE;
	EquipmentIdType iCandidate = EQP_NONE;

	for (int i = iStartingFrom - 1; i > EQP_NONE; i--)
	{
		iAmmoId = GetAmmoIdOfEquipment((EquipmentIdType)i);

		if ((!iAmmoId || gPseudoPlayer.m_rgAmmo[iAmmoId] <= 0) && !gPseudoPlayer.m_rgbHasEquipment[i])	// you can still selection some item even if it has no ammo. These items are not to be consumed.
			continue;

		iCandidate = (EquipmentIdType)i;
		break;
	}

	if (!iCandidate && bLoopFromEnd)
	{
		for (int i = EQP_COUNT - 1; i > EQP_NONE; i--)
		{
			iAmmoId = GetAmmoIdOfEquipment((EquipmentIdType)i);

			if ((!iAmmoId || gPseudoPlayer.m_rgAmmo[iAmmoId] <= 0) && !gPseudoPlayer.m_rgbHasEquipment[i])
				continue;

			iCandidate = (EquipmentIdType)i;
			break;
		}
	}

	// Loop back to self: no found.
	if (iCandidate == iStartingFrom)
		iCandidate = EQP_NONE;

	return iCandidate;
}

int CountEquipments(void)
{
	AmmoIdType iAmmoId = AMMO_NONE;
	int iCount = 0;

	for (int i = EQP_NONE; i < EQP_COUNT; i++)
	{
		iAmmoId = GetAmmoIdOfEquipment((EquipmentIdType)i);

		if ((!iAmmoId || gPseudoPlayer.m_rgAmmo[iAmmoId] <= 0) && !gPseudoPlayer.m_rgbHasEquipment[i])	// you can still selection some item even if it has no ammo. These items are not to be consumed.
			continue;

		iCount++;
	}

	return iCount;
}

void CommandFunc_PrevEquipment(void)
{
	// the drawing sequence is the select sequence. and the drawing sequence is the index.

	// you can't do this on the halfway.
	if (g_pCurWeapon && g_pCurWeapon->m_bitsFlags & WPNSTATE_QUICK_THROWING)
		return;

	gPseudoPlayer.m_iUsingGrenadeId = FindLastEquipment(true);
	gEngfuncs.pfnServerCmd(SharedVarArgs("eqpselect %d\n", gPseudoPlayer.m_iUsingGrenadeId));

	CHudEquipments::OnPrev();
}

void CommandFunc_AlterAct(void)
{
	// only pass to server when it is allowed in client.
	if (g_pCurWeapon && g_pCurWeapon->AlterAct())
		gEngfuncs.pfnServerCmd("changemode\n");
}

/*
=====================
CScreenFade::Draw
=====================
*/
void CScreenFade::Draw(void)
{
	if (m_flAlpha < 3)
		return;

	// LUNA: DONT use gEngfuncs.pfnFillRGBA here!

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(m_sColor.r / 255.0, m_sColor.g / 255.0, m_sColor.b / 255.0, m_flAlpha / 255.0);

	glBegin(GL_POLYGON);
	glVertex2f(0, 0);
	glVertex2f(0, ScreenHeight);
	glVertex2f(ScreenWidth, ScreenHeight);
	glVertex2f(ScreenWidth, 0);
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}

/*
=====================
CScreenFade::Think
=====================
*/
void CScreenFade::Think(void)
{
	if (memcmp(&m_sColor, &m_sTargetColor, sizeof(color24)))
	{
		m_sColor.r += (m_sTargetColor.r - m_sColor.r) * gHUD::m_flTimeDelta * 4;
		m_sColor.g += (m_sTargetColor.g - m_sColor.g) * gHUD::m_flTimeDelta * 4;
		m_sColor.b += (m_sTargetColor.b - m_sColor.b) * gHUD::m_flTimeDelta * 4;
	}

	if (m_iPhase == SF_FADEIN)
	{
		if (m_flFadeSpeed > 0)
			m_flAlpha = fmin(m_flAlpha + m_flFadeSpeed * gHUD::m_flTimeDelta, 255.0f);
		else if (m_flFadeSpeed < 0)
			m_flAlpha = fmin(m_flAlpha + (m_flTargetAlpha - m_flAlpha) * gHUD::m_flTimeDelta * Q_fabs(m_flFadeSpeed), 255);
		else
			m_flAlpha = fmin(m_flAlpha + (m_flTargetAlpha - m_flAlpha) * gHUD::m_flTimeDelta, 255);

		if (m_flAlpha >= m_flTargetAlpha)
		{
			m_flTimeThink = gEngfuncs.GetClientTime() + m_flStayLength;
			m_iPhase = SF_STAY;
		}
	}
	else if (m_iPhase == SF_STAY)
	{
		if (m_flTimeThink < gEngfuncs.GetClientTime())
			m_iPhase = SF_FADEOUT;
	}
	else if (m_iPhase == SF_FADEOUT)
	{
		if (m_flFadeSpeed > 0)
			m_flAlpha = fmax(m_flAlpha - m_flFadeSpeed * gHUD::m_flTimeDelta, 0);
		else if (m_flFadeSpeed < 0)
			m_flAlpha = fmax(m_flAlpha + (0 - m_flAlpha) * gHUD::m_flTimeDelta * m_flFadeSpeed, 0);
		else
			m_flAlpha = fmax(m_flAlpha + (0 - m_flAlpha) * gHUD::m_flTimeDelta, 0);

		// then ... it's simply done.. nothing to do else.
	}
}

namespace OverviewMgr
{
	// variables declaration.
	float m_flZoom = 0;
	Vector2D m_vecOrigin = Vector2D();
	bool m_bRotated = false;
	GLuint m_iIdTexture = 0U;
	int m_iHeight = 0;
	int m_iWidth = 0;
	Matrix3x3 m_mxTransform = Matrix3x3::Identity();
	model_t* m_pMapSprite = nullptr;
	Vector2D m_vecInsetWindowAnchor = Vector2D(4, 4);
	Vector2D m_vecInsetWindowSize = Vector2D(240, 180);
};

void OverviewMgr::OnHUDReset(void)
{
	m_flZoom = 0;
	m_vecOrigin = Vector2D();
	m_bRotated = false;
	m_iIdTexture = 0U;
	m_iHeight = 0;
	m_iWidth = 0;
	m_mxTransform = Matrix3x3::Identity();
	m_pMapSprite = nullptr;
	m_vecInsetWindowAnchor = Vector2D(4, 4);
	m_vecInsetWindowSize = Vector2D(240, 180);

	char szPath[128], szMap[64];

	// remove "maps/" words.
	Q_strlcpy(szMap, gEngfuncs.pfnGetLevelName() + 5U);

	// truncate ".bmp" words.
	szMap[Q_strlen(szMap) - 4U] = 0;

	// and we can have the txt file read.
	Q_slprintf(szPath, "overviews/%s.txt", szMap);

	if (!LoadOverviewInfo(szPath))
		return;

	// Build the constant Transfrom matrix.
	// The maxtrics composition is interpreted right from the left.
	// Therefore, when we building these matrics, we have to do it from the last step to the first step.

	// Step 3: Translate it with the sprite origin, make its value from [0~width] or [0~height] respectively.
	m_mxTransform *= Matrix3x3::Translation2D(Vector2D(OverviewMgr::m_iWidth, OverviewMgr::m_iHeight) / 2.0f);

	// Step 2: Linear stretch it, map it on the sprite.
	// Reference: https://www.cnblogs.com/crsky/p/9441540.html
	m_mxTransform *= Matrix3x3::Squeeze2D(
		8192.0f / m_flZoom / float(m_iWidth),
		8192.0f / m_flZoom / float(4.0 / 3.0) / float(m_iHeight)
	);

	if (m_bRotated)
		m_mxTransform *= Matrix3x3(
			-1,	0,	0,
			0,	1,	0,
			0,	0,	1
		);
	else
		m_mxTransform *= Matrix3x3(
			0,	-1,	0,
			-1,	0,	0,
			0,	0,	1
		);

	// Step 1: Translate the origin, match it with the map sprite origin.
	m_mxTransform *= Matrix3x3::Translation2D(-m_vecOrigin);
}

bool OverviewMgr::LoadOverviewInfo(const char* pszFilePath)
{
	// API problem. Have to enforce the conversion.
	char* pszBuffer = (char*)gEngfuncs.COM_LoadFile((char*)pszFilePath, 5, nullptr);

	if (!pszBuffer)
		return false;

	char* pszParsePos = pszBuffer;
	char szToken[128];
	bool bSuccessful = false;

	while (true)
	{
		pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);

		if (!pszParsePos)
			break;

		// block1: "GLOBAL"
		if (!Q_stricmp(szToken, "global"))
		{
			pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);

			if (!pszParsePos)
				goto error;

			// the first line followed have to be "{"
			if (Q_stricmp(szToken, "{"))
				goto error;

			while (true)
			{
				pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);
				if (!pszParsePos)
					goto error;

				if (!Q_stricmp(szToken, "zoom"))
				{
					pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);
					m_flZoom = Q_atof(szToken);
				}
				else if (!Q_stricmp(szToken, "origin"))
				{
					pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);
					m_vecOrigin.x = Q_atof(szToken);
					pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);
					m_vecOrigin.y = Q_atof(szToken);

					// we don't need a Z coord, but we have to process it either way.
					pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);
				}
				else if (!Q_stricmp(szToken, "rotated"))
				{
					pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);
					m_bRotated = !!Q_atoi(szToken);
				}
				else if (!Q_strcmp(szToken, "inset"))
				{
					pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);
					m_vecInsetWindowAnchor.x = Q_atof(szToken);
					pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);
					m_vecInsetWindowAnchor.y = Q_atof(szToken);
					pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);
					m_vecInsetWindowSize.width = Q_atof(szToken);
					pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);
					m_vecInsetWindowSize.height = Q_atof(szToken);

				}
				else if (!Q_stricmp(szToken, "}"))
					break;

				// there can't be anything else!
				else
					goto error;
			}
		}
		else if (!Q_stricmp(szToken, "layer"))
		{
			pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);

			if (!pszParsePos)
				goto error;

			if (Q_stricmp(szToken, "{"))
				goto error;

			while (true)
			{
				pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);

				if (!Q_stricmp(szToken, "image"))
				{
					pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);
					//szToken[Q_strlen(szToken) - 4U] = 0;
					//Q_strcat(szToken, ".dds");
					//m_iIdTexture = LoadDDS(szToken, &m_iWidth, &m_iHeight);
					const char* pTestLocation = &szToken[Q_strlen(szToken) - 4U];

					// support only these 3 formats.
					if (!Q_strnicmp(pTestLocation, ".bmp", 4U))
					{
						m_iIdTexture = LoadBMP(szToken, &m_iWidth, &m_iHeight);
						m_pMapSprite = gEngfuncs.LoadMapSprite(szToken);	// exclusive to BMP formats.
					}
					else if (!Q_strnicmp(pTestLocation, ".dds", 4U))
						m_iIdTexture = LoadDDS(szToken, &m_iWidth, &m_iHeight);
					else if (!Q_strnicmp(pTestLocation, ".tga", 4U))
						m_iIdTexture = LoadTGA(szToken, &m_iWidth, &m_iHeight);
					else
						goto error;
				}
				else if (!Q_stricmp(szToken, "height"))
					pszParsePos = gEngfuncs.COM_ParseFile(pszParsePos, szToken);	// useless, but we have to do it.

				else if (!Q_stricmp(szToken, "}"))
					break;
				else
					goto error;
			}
		}
		else
			goto error;
	}

	// if "goto error" happenned, it will bypass this line.
	bSuccessful = true;

error:
	if (pszBuffer)
		gEngfuncs.COM_FreeFile(pszBuffer);

	return bSuccessful;
}
