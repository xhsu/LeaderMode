/*

Created Date: Mar 11 2020

*/

#include "precompiled.h"

void TrackPlayer(void)
{
	if (gHUD::m_Radar.m_iPlayerLastPointedAt <= MAX_CLIENTS)
		gHUD::m_Radar.m_bTrackArray[gHUD::m_Radar.m_iPlayerLastPointedAt] = true;
}

void ClearPlayers(void)
{
	gHUD::m_Radar.m_bTrackArray.fill(false);
}

void DrawRadar(void)
{
	gHUD::m_Radar.m_bDrawRadar = true;
}

void HideRadar(void)
{
	gHUD::m_Radar.m_bDrawRadar = false;
}

int CHudRadar::Init(void)
{
	gEngfuncs.pfnAddCommand("trackplayer", TrackPlayer);
	gEngfuncs.pfnAddCommand("clearplayers", ClearPlayers);
	gEngfuncs.pfnAddCommand("drawradar", ::DrawRadar);
	gEngfuncs.pfnAddCommand("hideradar", HideRadar);

	m_bTrackArray.fill(false);

	m_iPlayerLastPointedAt = 0;
	m_bDrawRadar = true;

	// CT
	m_rgiRadarIcons[Role_Breacher]		= LoadDDS("texture/HUD/ClassesIcon/CT/Breacher_Radar.dds");
	m_rgiRadarIcons[Role_Commander]		= LoadDDS("texture/HUD/ClassesIcon/CT/Commander_Radar.dds");
	m_rgiRadarIcons[Role_Medic]			= LoadDDS("texture/HUD/ClassesIcon/CT/Medic_Radar.dds");
	m_rgiRadarIcons[Role_Sharpshooter]	= LoadDDS("texture/HUD/ClassesIcon/CT/Sharpshooter_Radar.dds");
	m_rgiRadarIcons[Role_SWAT]			= LoadDDS("texture/HUD/ClassesIcon/CT/SWAT_Radar.dds");

	// T
	m_rgiRadarIcons[Role_Arsonist]		= LoadDDS("texture/HUD/ClassesIcon/T/Arsonist_Radar.dds");
	m_rgiRadarIcons[Role_Assassin]		= LoadDDS("texture/HUD/ClassesIcon/T/Assassin_Radar.dds");
	m_rgiRadarIcons[Role_Godfather]		= LoadDDS("texture/HUD/ClassesIcon/T/Godfather_Radar.dds");
	m_rgiRadarIcons[Role_LeadEnforcer]	= LoadDDS("texture/HUD/ClassesIcon/T/LeadEnforcer_Radar.dds");
	m_rgiRadarIcons[Role_MadScientist]	= LoadDDS("texture/HUD/ClassesIcon/T/MadScientist_Radar.dds");

	gHUD::AddHudElem(this);
	return 1;
}

int CHudRadar::VidInit(void)
{
	m_bitsFlags |= HUD_ACTIVE;

	m_HUD_radar = gHUD::GetSpriteIndex("radar");
	m_HUD_radaropaque = gHUD::GetSpriteIndex("radaropaque");

	m_hrad = &gHUD::GetSpriteRect(m_HUD_radar);
	m_hradopaque = &gHUD::GetSpriteRect(m_HUD_radaropaque);

	m_hrad->left = 0;
	m_hrad->top = 0;
	m_hrad->right = 128;
	m_hrad->bottom = 128;

	m_hradopaque->left = 0;
	m_hradopaque->top = 0;
	m_hradopaque->right = 128;
	m_hradopaque->bottom = 128;

	m_hRadar = gHUD::GetSprite(m_HUD_radar);
	m_hRadaropaque = gHUD::GetSprite(m_HUD_radaropaque);
	return 1;
}

int CHudRadar::Draw(float flTime)
{
	if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_HEALTH) || g_iUser1)	// TODO: maybe add an independent flag to turn radar off?
		return 1;

	if (!gHUD::m_bPlayerDead && m_bDrawRadar == true)
		DrawRadar(flTime);

	return 1;
}

void CHudRadar::DrawRadarDot(int x, int y, float z_diff, int iBaseDotSize, int flags, int r, int g, int b, int a)
{
	if (flags & RADAR_DOT_BOMBCARRIER)
	{
		gEngfuncs.pfnFillRGBA(x - 3, y - 3, iBaseDotSize + 3, iBaseDotSize + 3, r, g, b, a);
	}
	else if (z_diff < -128)
	{
		z_diff *= -1;

		if (z_diff > 3096)
			z_diff = 3096;

		int iBar = (int)(z_diff / 400) + 2 * iBaseDotSize;

		gEngfuncs.pfnFillRGBA(x, y, 1, iBar, r, g, b, a);
		gEngfuncs.pfnFillRGBA(x - 2, y + iBar, 5, 1, r, g, b, a);
	}
	else if (z_diff > 128)
	{
		if (z_diff > 3096)
			z_diff = 3096;

		int iBar = (int)(z_diff / 400) + 2 * iBaseDotSize;

		gEngfuncs.pfnFillRGBA(x - 2, y, 5, 1, r, g, b, a);
		gEngfuncs.pfnFillRGBA(x, y + 1, 1, iBar - 1, r, g, b, a);
	}
	else
	{
		if (flags & RADAR_DOT_HOSTAGE)
		{
			gEngfuncs.pfnFillRGBA(x - 1, y - 1, iBaseDotSize + 1, iBaseDotSize + 1, r, g, b, a);
		}
		else if (flags & RADAR_DOT_BOMB)
		{
			if (flags & RADAR_DOT_BOMB_PLANTED)
			{
				gEngfuncs.pfnFillRGBA(x, y, iBaseDotSize, iBaseDotSize, r, g, b, a);
				gEngfuncs.pfnFillRGBA(x - 2, y - 2, iBaseDotSize, iBaseDotSize, r, g, b, a);
				gEngfuncs.pfnFillRGBA(x - 2, y + 2, iBaseDotSize, iBaseDotSize, r, g, b, a);
				gEngfuncs.pfnFillRGBA(x + 2, y - 2, iBaseDotSize, iBaseDotSize, r, g, b, a);
				gEngfuncs.pfnFillRGBA(x + 2, y + 2, iBaseDotSize, iBaseDotSize, r, g, b, a);
			}
			else
			{
				gEngfuncs.pfnFillRGBA(x - 1, y - 1, iBaseDotSize + 1, iBaseDotSize + 1, r, g, b, a);
			}
		}
		else
		{
			gEngfuncs.pfnFillRGBA(x, y, iBaseDotSize, iBaseDotSize, r, g, b, a);
		}
	}
}

void CHudRadar::DrawRadar(float flTime)
{
	// UNDONE
	/*if (gConfigs.bEnableClientUI && cl_newradar->value)
		return;*/

	float fRange;
	int iRadarRadius;
	int iBaseDotSize;
	int r, g, b;
	Vector vecTranslated;

	if (ScreenWidth >= 640)
	{
		fRange = 2048;
		iRadarRadius = 128;
		iBaseDotSize = 2;
	}
	else
	{
		fRange = 1024;
		iRadarRadius = 64;
		iBaseDotSize = 1;
	}

	if (g_PlayerExtraInfo[gHUD::m_iPlayerNum].m_bIsDead)
		return;

	if (cl_radartype && cl_radartype->value != 0)
	{
		gEngfuncs.pfnSPR_Set(m_hRadaropaque, 200, 200, 200);
		gEngfuncs.pfnSPR_DrawHoles(0, 0, 0, m_hradopaque);
	}
	else
	{
		gEngfuncs.pfnSPR_Set(m_hRadar, 25, 75, 25);
		gEngfuncs.pfnSPR_DrawAdditive(0, 0, 0, m_hrad);
	}

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (!g_PlayerInfoList[i].name || !g_PlayerInfoList[i].name[0])
			continue;

		if (gHUD::m_iPlayerNum == i || g_PlayerExtraInfo[i].m_iTeam != g_iTeam || g_PlayerExtraInfo[i].m_bIsDead)
			continue;

		if (g_PlayerExtraInfo[i].m_iHealth <= 0)	// no dead guy allowed.
			continue;

		// translate the location we received to radar coord.
		vecTranslated = Translate(g_PlayerExtraInfo[i].m_vecOrigin, fRange, iRadarRadius);

		if (vecTranslated.x < 0 || vecTranslated.x > iRadarRadius || vecTranslated.y < 0 || vecTranslated.y > iRadarRadius)
			continue;

		if ((g_PlayerExtraInfo[i].m_bIsGodfather && (g_iTeam == TEAM_TERRORIST || g_iTeam == TEAM_UNASSIGNED)) || (g_PlayerExtraInfo[i].m_bIsCommander && (g_iTeam == TEAM_CT || g_iTeam == TEAM_UNASSIGNED)))
		{
			r = 250;
			g = 0;
			b = 0;
		}
		else
		{
			if (m_bTrackArray[i])
			{
				iBaseDotSize *= 2;
				r = 185;
				g = 20;
				b = 20;
			}
			else
			{
				r = 150;
				g = 75;
				b = 250;
			}
		}

		// the noobie and illegit players have no icon.
		if (g_PlayerExtraInfo[i].m_iRoleType > Role_UNASSIGNED && g_PlayerExtraInfo[i].m_iRoleType < ROLE_COUNT)
		{
			// class icon
			gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
			gEngfuncs.pTriAPI->Brightness(1.0);

			// in order to make transparent fx on dds texture...
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(VEC_YELLOWISH.r, VEC_YELLOWISH.g, VEC_YELLOWISH.b, 235.0 / 255.0);

			gEngfuncs.pTriAPI->CullFace(TRI_NONE);

			glBindTexture(GL_TEXTURE_2D, m_rgiRadarIcons[g_PlayerExtraInfo[i].m_iRoleType]);
			DrawUtils::Draw2DQuad(vecTranslated.x - 2, vecTranslated.y - 2, vecTranslated.x + 2, vecTranslated.y + 2);
		}
		else
			DrawRadarDot(vecTranslated, iBaseDotSize, RADAR_DOT_NORMAL, r, g, b, 235);

		if (g_PlayerExtraInfo[i].m_flTimeNextRadarFlash != -1.0 && flTime > g_PlayerExtraInfo[i].m_flTimeNextRadarFlash && g_PlayerExtraInfo[i].m_iRadarFlashRemains > 0)
		{
			g_PlayerExtraInfo[i].m_flTimeNextRadarFlash = flTime + 0.15f;
			g_PlayerExtraInfo[i].m_iRadarFlashRemains--;
			g_PlayerExtraInfo[i].m_bRadarFlashing = !g_PlayerExtraInfo[i].m_bRadarFlashing;
		}

		if (g_PlayerExtraInfo[i].m_bRadarFlashing && g_PlayerExtraInfo[i].m_iRadarFlashRemains > 0)
		{
			r = 230;
			g = 110;
			b = 25;

			DrawRadarDot(vecTranslated.x, vecTranslated.y, vecTranslated.z, iBaseDotSize, RADAR_DOT_BOMBCARRIER, r, g, b, 245);
		}
	}

	for (int i = 0; i < MAX_POINTS; i++)	// for the tracking skills
	{
		if (!m_rgCustomPoints[i].m_bGlobalOn)
			continue;

		if (m_rgCustomPoints[i].m_iFlashCounts <= 0 && m_rgCustomPoints[i].m_flTimeSwitchPhase <= gEngfuncs.GetClientTime())	// the last flash.
		{
			m_rgCustomPoints[i].m_bGlobalOn = false;
			continue;
		}

		if (m_rgCustomPoints[i].m_flTimeSwitchPhase <= gEngfuncs.GetClientTime())
		{
			m_rgCustomPoints[i].m_flTimeSwitchPhase = gEngfuncs.GetClientTime() + m_rgCustomPoints[i].m_flFlashInterval;
			m_rgCustomPoints[i].m_bPhase = !m_rgCustomPoints[i].m_bPhase;
			m_rgCustomPoints[i].m_iFlashCounts--;
		}

		if (m_rgCustomPoints[i].m_bPhase)
		{
			Vector vec = Translate(m_rgCustomPoints[i].m_vecCoord, fRange, iRadarRadius);
			DrawRadarDot(vec, iBaseDotSize * m_rgCustomPoints[i].m_iDotSize, m_rgCustomPoints[i].m_bitsFlags, m_rgCustomPoints[i].m_color);
		}
	}

	// the text indicates where player is.
	DrawPlayerLocation();
}

void CHudRadar::DrawPlayerLocation(void)
{
	wchar_t* locString;
	int center_x, center_y;
	int string_width, string_height;
	int x, y;

	if (g_PlayerExtraInfo[gHUD::m_iPlayerNum].m_szLocationText[0] != '#')
	{
		static wchar_t locBuffer[512];
		VGUI_LOCALISE->ConvertANSIToUnicode(g_PlayerExtraInfo[gHUD::m_iPlayerNum].m_szLocationText, locBuffer, sizeof(locBuffer));
		locString = locBuffer;
	}
	else
	{
		locString = VGUI_LOCALISE->Find(g_PlayerExtraInfo[gHUD::m_iPlayerNum].m_szLocationText);
	}

	if (!locString)
		return;

	center_y = 128;
	center_x = 32;

	if (ScreenWidth < 640)
		center_y = 64;

	if (ScreenWidth >= 640)
		center_x = 64;

	gHUD::m_VGUI2Print.GetStringSize(locString, &string_width, &string_height);

	x = max(0, center_x - (string_width / 2));
	y = center_y + (string_height / 2);

	if (wcslen(locString) > 0)
		gHUD::m_VGUI2Print.DrawVGUI2String(locString, x, y, g_LocationColor[0], g_LocationColor[1], g_LocationColor[2]);
}

int CHudRadar::GetRadarSize(void)
{
	// UNDONE
	/*if (cl_newradar->value && gConfigs.bEnableClientUI)
		return cl_newradar_size->value * ScreenWidth;*/

	if (ScreenWidth >= 640)
		return 128;
	else
		return 64;
}

Vector CHudRadar::Translate(const Vector& vecOrigin, float flRange, float flRadarRadius)
{
	float x_diff, y_diff, z_diff;
	float flOffset;
	float xnew_diff, ynew_diff;
	float flScale;

	x_diff = vecOrigin.x - gHUD::m_vecOrigin.x;
	y_diff = vecOrigin.y - gHUD::m_vecOrigin.y;
	z_diff = vecOrigin.z - gHUD::m_vecOrigin.z;

	flOffset = Q_atan(y_diff / x_diff);
	flOffset *= 180.0f;
	flOffset /= M_PI;

	if ((x_diff < 0) && (y_diff >= 0))
		flOffset = 180.0f + flOffset;
	else if ((x_diff < 0) && (y_diff < 0))
		flOffset = 180.0f + flOffset;
	else if ((x_diff >= 0) && (y_diff < 0))
		flOffset = 360.0f + flOffset;

	y_diff = -1.0f * Q_sqrt((x_diff * x_diff) + (y_diff * y_diff));
	x_diff = 0;

	flOffset = gHUD::m_vecAngles.yaw - flOffset;
	flOffset *= M_PI;
	flOffset /= 180.0f;

	xnew_diff = x_diff * Q_cos(flOffset) - y_diff * Q_sin(flOffset);
	ynew_diff = x_diff * Q_sin(flOffset) + y_diff * Q_cos(flOffset);

	if ((-1.0f * y_diff) > flRange)
	{
		flScale = (-1.0f * y_diff) / flRange;

		xnew_diff /= flScale;
		ynew_diff /= flScale;
	}

	xnew_diff /= 32;
	ynew_diff /= 32;

	Vector vecResult;
	vecResult.x = (flRadarRadius / 2) + (int)xnew_diff;
	vecResult.y = (flRadarRadius / 2) + (int)ynew_diff;
	vecResult.z = z_diff;

	return vecResult;
}
