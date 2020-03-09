/*

Created Date: 07 Mar 2020

*/

#include "cl_base.h"

hud_player_info_t g_PlayerInfoList[MAX_PLAYERS + 1];
int g_PlayerScoreAttrib[MAX_PLAYERS + 1];
TEMPENTITY* g_DeadPlayerModels[MAX_PLAYERS + 1];

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
	int m_iSpriteCount = 0;
	int m_iSpriteCountAllRes = 0;
	int m_iRes = 640;
	hSprite* m_rghSprites = nullptr;
	wrect_t* m_rgrcRects = nullptr;
	char* m_rgszSpriteNames = nullptr;
	char m_szGameMode[32] = "\0";
	int m_HUD_number_0 = 0;
	int m_iFontHeight = 0;
	int m_iIntermission = FALSE;
	char m_szMOTD[2048] = "\0";
	float m_flTimeLeft = 0;
	int m_bitsHideHUDDisplay = 0;
	float m_flMouseSensitivity = 0;
	int m_PlayerFOV[MAX_CLIENTS];
	Vector m_vecOrigin = Vector();
	Vector m_vecAngles = Vector();
	int m_iKeyBits = 0;
	int m_iWeaponBits = 0;

	SCREENINFO m_scrinfo;
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

	for (auto pHudElement : m_lstAllHUDElems)
	{
		pHudElement->Init();
	}

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

	/* UNDONE
	if (gConfigs.bEnableClientUI)
		g_pViewPort->Init();
	*/
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
	m_iIntermission = 0;
	m_szMOTD[0] = 0;
	m_flTimeLeft = 0;

	for (auto pHudElement : m_lstAllHUDElems)
	{
		pHudElement->VidInit();
	}

	// UNDONE
	//GetClientVoiceHud()->VidInit();

	//m_iFontEngineHeight = vgui::surface()->GetFontTall(font);

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
		if (m_iIntermission && !intermission)
		{
			m_iIntermission = intermission;

			g_pViewPort->HideAllVGUIMenu();
			g_pViewPort->UpdateSpectatorPanel();
		}
		else if (!m_iIntermission && intermission)
		{
			m_iIntermission = intermission;

			g_pViewPort->HideAllVGUIMenu();
			g_pViewPort->ShowScoreBoard();
			g_pViewPort->UpdateSpectatorPanel();
		}
	}
	*/
	m_iIntermission = intermission;

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
	for (auto pHudElements : m_lstAllHUDElems)
	{
		if (pHudElements->m_bitsFlags & HUD_ACTIVE)
			pHudElements->Think();
	}

	int newfov = HUD_GetFOV();

	if (newfov == 0)
		m_iFOV = default_fov->value;
	else
		m_iFOV = newfov;

	if (m_iFOV == default_fov->value)
		m_flMouseSensitivity = 0;
	else
		m_flMouseSensitivity = sensitivity->value * ((float)newfov / (float)default_fov->value) * CVAR_GET_FLOAT("zoom_sensitivity_ratio");

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
		// UNDONE
		//else
			//m_iFOV = m_Spectator.GetFOV();
	}
}

int gHUD::UpdateClientData(client_data_t* cdata, float time)
{
	Q_memcpy(m_vecOrigin, cdata->origin, sizeof(vec3_t));
	Q_memcpy(m_vecAngles, cdata->viewangles, sizeof(vec3_t));

	m_iKeyBits = CL_ButtonBits(0);
	m_iWeaponBits = cdata->iWeaponBits;

	Think();

	cdata->fov = m_iFOV;

	CL_ResetButtonBits(m_iKeyBits);
	return 1;
}

void gHUD::CalcRefdef(ref_params_s* pparams)
{
	// UNDONE
	//m_ThirdPerson.CalcRefdef(pparams);
	//m_ViewModel.CalcRefdef(pparams);
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

wrect_t& gHUD::GetSpriteRect(int index)
{
	return m_rgrcRects[index];
}
