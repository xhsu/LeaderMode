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
	std::list<CBaseHUDElement*> m_lstAllHUDElems;

	cvar_s* m_pCvarDraw = nullptr;
	cvar_s* default_fov = nullptr;

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
	hSprite* m_rghSprites = nullptr;
	wrect_t* m_rgrcRects = nullptr;
	char* m_rgszSpriteNames = nullptr;
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

	SCREENINFO m_scrinfo;

	// HUD elements.
	CHudAmmo m_Ammo;
	CHudHealth m_Health;
	CHudSpectator m_Spectator;
	CHudGeiger m_Geiger;
	CHudBattery m_Battery;
	CHudTrain m_Train;
	CHudFlashlight m_Flash;
	CHudMessage m_Message;
	CHudStatusBar m_StatusBar;
	CHudDeathNotice m_DeathNotice;
	CHudSayText m_SayText;
	CHudMenu m_Menu;
	CHudNightVision m_NightVision;
	CHudTextMessage m_TextMessage;
	CHudRoundTimer m_roundTimer;
	CHudAccountBalance m_accountBalance;
	CHudHeadName m_headName;
	CHudRadar m_Radar;
	CHudStatusIcons m_StatusIcons;
	CHudScenarioStatus m_scenarioStatus;
	CHudProgressBar m_progressBar;
	CHudVGUI2Print m_VGUI2Print;
	CHudSniperScope m_SniperScope;
	CHudCrosshair m_Crosshair;
	CHudWeaponList m_WeaponList;
};

void gHUD::Init(void)
{
	Shutdown();

	/* UNDONE
	if (gConfigs.bEnableClientUI)
		InstallCounterStrikeViewportHook();
		*/

	m_pSpriteList = nullptr;
	m_iPlayerNum = 0;
	m_flTime = 1;
	m_iFOV = 0;

	// we can't use this in init() since all these elements are adding themselves into std::list in init().
	/*for (auto pHudElement : m_lstAllHUDElems)
	{
		pHudElement->Init();
	}*/

	// instead, we should:
	m_Ammo.Init();
	m_Health.Init();
	m_SayText.Init();	// m_SayText should place before m_Spectator, since m_Spectator.init() is calling some vars from m_SayText.Init().
	m_Spectator.Init();
	m_Geiger.Init();
	m_Battery.Init();
	m_Train.Init();
	m_Flash.Init();
	m_Message.Init();
	m_StatusBar.Init();
	m_DeathNotice.Init();
	m_Menu.Init();
	m_NightVision.Init();
	m_TextMessage.Init();
	m_roundTimer.Init();
	m_accountBalance.Init();
	m_headName.Init();
	m_Radar.Init();
	m_StatusIcons.Init();
	m_scenarioStatus.Init();
	m_progressBar.Init();
	m_VGUI2Print.Init();
	m_SniperScope.Init();
	m_Crosshair.Init();
	m_WeaponList.Init();

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

	default_fov = CVAR_CREATE("default_fov", "90", 0);

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
	gEngfuncs.pfnAddCommand("invnext", CommandFunc_NextWeapon);
	gEngfuncs.pfnAddCommand("invprev", CommandFunc_PrevWeapon);
	gEngfuncs.pfnAddCommand("adjust_crosshair", CommandFunc_Adjust_Crosshair);
	gEngfuncs.pfnAddCommand("lastinv", CommandFunc_SelectLastItem);
}

void gHUD::Shutdown(void)
{
	m_lstAllHUDElems.clear();
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

			m_rghSprites = new hSprite[m_iSpriteCount];
			m_rgrcRects = new wrect_t[m_iSpriteCount];
			m_rgszSpriteNames = new char[m_iSpriteCount * MAX_SPRITE_NAME_LENGTH];
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
					Q_strncpy(&m_rgszSpriteNames[index * MAX_SPRITE_NAME_LENGTH], p->szName, MAX_SPRITE_NAME_LENGTH);
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

	m_iPlayerNum = 0;
	m_szGameMode[0] = '\0';
	m_HUD_number_0 = GetSpriteIndex("number_0");
	m_iFontHeight = m_rgrcRects[m_HUD_number_0].bottom - m_rgrcRects[m_HUD_number_0].top;
	m_bIntermission = 0;
	m_szMOTD[0] = 0;
	m_flTimeLeft = 0;

	for (auto pHudElement : m_lstAllHUDElems)
	{
		pHudElement->VidInit();
	}

	// UNDONE
	//GetClientVoiceHud()->VidInit();

	m_iFontEngineHeight = VGUI_SURFACE->GetFontTall(font);

	// UNDONE
	//if (gConfigs.bEnableClientUI)
		//g_pViewPort->VidInit();
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

	if (m_pCvarDraw->value)
	{
		for (auto pHudElements : m_lstAllHUDElems)
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
	}

	// UNDONE
	//if (gConfigs.bEnableClientUI)
		//g_pViewPort->SetPaintEnabled(m_pCvarDraw->value);

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
	// draw or not, you must think.
	for (auto pHudElements : m_lstAllHUDElems)
	{
		if (pHudElements->m_bitsFlags & HUD_ACTIVE)
			pHudElements->Think();
	}

	// Is Player Dead??
	m_bPlayerDead = CL_IsDead();

	/*int newfov = HUD_GetFOV();

	if (newfov == 0)
		m_iFOV = default_fov->value;
	else
		m_iFOV = newfov;*/

	if (m_iFOV == default_fov->value)
		m_flMouseSensitivity = 0;
	else
		m_flMouseSensitivity = sensitivity->value * (m_flDisplayedFOV / (float)default_fov->value) * zoom_sensitivity_ratio->value;

	if (m_iFOV == 0)
		m_iFOV = Q_max(default_fov->value, 90.0f);

	if (gEngfuncs.IsSpectateOnly())
	{
		if (g_iUser1 == OBS_IN_EYE && g_iUser2 != 0)
		{
			if (g_iUser2 <= gEngfuncs.GetMaxClients())
				m_iFOV = m_PlayerFOV[g_iUser2];
			else
				m_iFOV = Q_max(default_fov->value, 90.0f);
		}
		else
			m_iFOV = m_Spectator.GetFOV();
	}

	// make FOV transition nice and smooth.
	m_flDisplayedFOV += (float(m_iFOV) - m_flDisplayedFOV) * m_flTimeDelta * 7.0f;	// this 7.0 is the transition speed.
}

int gHUD::UpdateClientData(client_data_t* cdata, float time)
{
	Q_memcpy(m_vecOrigin, cdata->origin, sizeof(vec3_t));
	Q_memcpy(m_vecAngles, cdata->viewangles, sizeof(vec3_t));

	m_iKeyBits = CL_ButtonBits(0);
	m_iWeaponBits = cdata->iWeaponBits;

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

int gHUD::GetSpriteIndex(const char* SpriteName)
{
	for (int i = 0; i < m_iSpriteCount; i++)
	{
		if (Q_strncmp(SpriteName, m_rgszSpriteNames + (i * MAX_SPRITE_NAME_LENGTH), MAX_SPRITE_NAME_LENGTH) == 0)
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
	int iWidth = GetSpriteRect(m_HUD_number_0).right - GetSpriteRect(m_HUD_number_0).left;
	int k;

	if (iNumber > 0)
	{
		if (iNumber >= 10000)
		{
			k = iNumber / 10000;
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		if (iNumber >= 1000)
		{
			k = (iNumber % 10000) / 1000;
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		if (iNumber >= 100)
		{
			k = (iNumber % 1000) / 100;
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		if (iNumber >= 10)
		{
			k = (iNumber % 100) / 10;
			gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		k = iNumber % 10;
		gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
		x += iWidth;
	}
	else if (iFlags & DHN_DRAWZERO)
	{
		gEngfuncs.pfnSPR_Set(GetSprite(m_HUD_number_0), r, g, b);

		if (iFlags & (DHN_5DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		if (iFlags & (DHN_5DIGITS | DHN_4DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS))
		{
			if (iFlags & DHN_FILLZERO)
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0));

			x += iWidth;
		}

		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0));
		x += iWidth;
	}

	return x;
}

int gHUD::DrawHudNumber(int x, int y, int iNumber, int r, int g, int b)
{
	static char szBuffer[16];
	const char* pszPosint;
	int iWidth = GetSpriteRect(m_HUD_number_0).right - GetSpriteRect(m_HUD_number_0).left;
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
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
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

void gHUD::AddHudElem(CBaseHUDElement* phudelem)
{
	m_lstAllHUDElems.push_back(phudelem);
}

float gHUD::GetSensitivity(void)
{
	return m_flMouseSensitivity;
}

hSprite gHUD::GetSprite(int index)
{
	return (index < 0) ? 0 : m_rghSprites[index];
}

wrect_t gHUD::GetSpriteRect(int index)
{
	return m_rgrcRects[index];
}

client_sprite_t* gHUD::GetSpriteFromList(client_sprite_t* pList, const char* psz, int iRes, int iCount)
{
	if (!pList)
		return NULL;

	int i = iCount;
	client_sprite_t* p = pList;

	while (i--)
	{
		if (!Q_stricmp(psz, p->szName) && p->iRes == iRes)
			return p;

		p++;
	}

	return NULL;
}

void gHUD::SlotInput(int iSlot)
{
	if (m_bIntermission)
		return;

	// UNDONE
	//if (gViewPortInterface && gViewPortInterface->SlotInput(iSlot))
		//return;

	if (m_Menu.m_fMenuDisplayed)
	{
		m_Menu.SelectMenuItem(iSlot);	// never -1 over here.
		return;
	}

	if (iSlot <= SLOT_NO || iSlot >= MAX_ITEM_TYPES)
		return;

	const char* psz = g_rgItemInfo[gHUD::m_WeaponList.m_rgiWeapons[iSlot]].m_pszInternalName;
	if (psz && Q_strlen(psz))
	{
		char sz[128];
		Q_strlcpy(sz, psz);
		gEngfuncs.pfnServerCmd(sz);
		g_iSelectedWeapon = gHUD::m_WeaponList.m_rgiWeapons[iSlot];

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
		const char* psz = g_rgItemInfo[iId].m_pszInternalName;
		if (psz && Q_strlen(psz))
		{
			char sz[128];
			Q_strlcpy(sz, psz);
			gEngfuncs.pfnServerCmd(sz);
			g_iSelectedWeapon = iId;

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
		const char* psz = g_rgItemInfo[iId].m_pszInternalName;
		if (psz && Q_strlen(psz))
		{
			char sz[128];
			Q_strlcpy(sz, psz);
			gEngfuncs.pfnServerCmd(sz);
			g_iSelectedWeapon = iId;

			gEngfuncs.pfnPlaySoundByName(WEAPONLIST_WHEEL_SFX, VOL_NORM);
			return;
		}
	}

	gEngfuncs.pfnPlaySoundByName(WEAPONLIST_EMPTY_SFX, VOL_NORM);
}

void CommandFunc_Adjust_Crosshair(void)
{
	// forward this call, since most of this command is reagarding changing the variables in m_Ammo.
	gHUD::m_Crosshair.Adjust_Crosshair();
}

void CommandFunc_SelectLastItem(void)	// an equivlent function of void CBasePlayer::SelectLastItem() on SV.
{
	if (gPseudoPlayer.m_pActiveItem && !gPseudoPlayer.m_pActiveItem->CanHolster())
		return;

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
		return;

	// TODO
	/*if (!gPseudoPlayer.m_pLastItem->CanDeploy())
		return;*/

	if (gPseudoPlayer.m_pActiveItem)
	{
		gPseudoPlayer.m_pActiveItem->Holster();
	}

	SWAP(gPseudoPlayer.m_pActiveItem, gPseudoPlayer.m_pLastItem);

	gPseudoPlayer.m_pActiveItem->Deploy();

	gPseudoPlayer.ResetMaxSpeed();

	// don't forget to forward this command to SV.
	gEngfuncs.pfnServerCmd("lastinv");
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
