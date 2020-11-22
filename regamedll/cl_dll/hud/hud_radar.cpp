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

bool LoadOverviewInfo(const char* fileName, overview_t* data)
{
	char* buffer = (char*)gEngfuncs.COM_LoadFile((char*)fileName, 5, nullptr);
	if (!buffer) {
		return false;
	}
	char* parsePos = buffer;
	char token[128];
	bool parseSuccess = false;
	while (true) {
		parsePos = gEngfuncs.COM_ParseFile(parsePos, token);
		if (!parsePos) {
			break;
		}
		if (!Q_stricmp(token, "global")) {
			parsePos = gEngfuncs.COM_ParseFile(parsePos, token);
			if (!parsePos) {
				goto error;
			}
			if (Q_stricmp(token, "{")) {
				goto error;
			}
			while (true) {
				parsePos = gEngfuncs.COM_ParseFile(parsePos, token);
				if (!parsePos) {
					goto error;
				}
				if (!Q_stricmp(token, "zoom")) {
					parsePos = gEngfuncs.COM_ParseFile(parsePos, token);
					data->zoom = atof(token);
				}
				else if (!Q_stricmp(token, "origin")) {
					parsePos = gEngfuncs.COM_ParseFile(parsePos, token);
					data->originX = atof(token);
					parsePos = gEngfuncs.COM_ParseFile(parsePos, token);
					data->originY = atof(token);
					parsePos = gEngfuncs.COM_ParseFile(parsePos, token);
				}
				else if (!Q_stricmp(token, "rotated")) {
					parsePos = gEngfuncs.COM_ParseFile(parsePos, token);
					data->rotated = atoi(token) != 0;
				}
				else if (!Q_stricmp(token, "}")) {
					break;
				}
				else {
					goto error;
				}
			}
		}
		else if (!Q_stricmp(token, "layer")) {
			parsePos = gEngfuncs.COM_ParseFile(parsePos, token);
			if (!parsePos) {
				goto error;
			}
			if (strcmp(token, "{")) {
				goto error;
			}
			while (true) {
				parsePos = gEngfuncs.COM_ParseFile(parsePos, token);
				if (!Q_stricmp(token, "image")) {
					parsePos = gEngfuncs.COM_ParseFile(parsePos, token);
					data->textureId = LoadDDS(token);
				}
				else if (!Q_stricmp(token, "height")) {
					parsePos = gEngfuncs.COM_ParseFile(parsePos, token);
				}
				else if (!Q_stricmp(token, "}")) {
					break;
				}
				else {
					goto error;
				}
			}
		}
		else {
			goto error;
		}
	}
	parseSuccess = true;
error:
	if (buffer) {
		gEngfuncs.COM_FreeFile(buffer);
	}
	return parseSuccess;
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

	// common
	m_iIdArrow = LoadDDS("texture/HUD/Radar/Arrow.dds");

	gHUD::AddHudElem(this);
	return 1;
}

int CHudRadar::VidInit(void)
{
	m_bitsFlags |= HUD_ACTIVE;

	m_HUD_radar = gHUD::GetSpriteIndex("radar");
	m_HUD_radaropaque = gHUD::GetSpriteIndex("radaropaque");

	m_hrad = gHUD::GetSpriteRect(m_HUD_radar);
	m_hradopaque = gHUD::GetSpriteRect(m_HUD_radaropaque);

	m_hRadar = gHUD::GetSprite(m_HUD_radar);
	m_hRadaropaque = gHUD::GetSprite(m_HUD_radaropaque);

	return 1;
}

void CHudRadar::Reset(void)
{
	// map
	char szPath[128], szMap[64];
	Q_strcpy(szMap, gEngfuncs.pfnGetLevelName() + 5U);
	szMap[Q_strlen(szMap) - 4U] = 0;

	Q_sprintf(szPath, "overviews/%s.dds", szMap);
	//m_pMapSprite = gEngfuncs.LoadMapSprite(szPath);
	m_Overview.m_iId = LoadDDS(szPath, &m_Overview.m_iWidth, &m_Overview.m_iHeight);

	Q_sprintf(szPath, "overviews/%s.txt", szMap);
	LoadOverviewInfo(szPath, &m_OVData);

	if (m_Overview.m_iId)
	{
		m_Overview.m_vecScale.x = 8192.0f / m_OVData.zoom;
		m_Overview.m_vecScale.y = 8192.0f / m_OVData.zoom / float(4.0 / 3.0);

		m_Overview.m_vecScale.x /= float(m_Overview.m_iWidth);
		m_Overview.m_vecScale.y /= float(m_Overview.m_iHeight);
	}
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

static constexpr float	RADAR_BORDER = 12;
static constexpr float	RADAR_HUD_SIZE = 240;
static constexpr float	RADAR_RANGE = 2048;
static constexpr int	RADAR_ICON_SIZE = 16;
static const Vector GODFATHER_COLOR_DIFF = VEC_SPRINGGREENISH - VEC_T_COLOUR;
static const Vector COMMANDER_COLOR_DIFF = VEC_SPRINGGREENISH - VEC_CT_COLOUR;

// view.cpp
extern Vector v_angles;

void CHudRadar::DrawRadar(float flTime)
{
	int iBaseDotSize = 2;
	Vector color;
	Vector vecTranslated;
	bool bClampped = false;

	// draw white board.
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(1, 1, 1, 0.5);
	DrawUtils::Draw2DQuad(RADAR_BORDER, RADAR_BORDER, RADAR_BORDER + RADAR_HUD_SIZE, RADAR_BORDER + RADAR_HUD_SIZE);

	glColor4f(1, 1, 1, 1);
	DrawUtils::Draw2DQuadProgressBar(RADAR_BORDER, RADAR_BORDER, RADAR_HUD_SIZE, RADAR_HUD_SIZE, 2, 1);

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	if (m_Overview.m_iId)
	{
		gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
		gEngfuncs.pTriAPI->Brightness(1.0);

		// in order to make transparent fx on dds texture...
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1, 1, 1, 1);

		gEngfuncs.pTriAPI->CullFace(TRI_NONE);
		
		glBindTexture(GL_TEXTURE_2D, m_Overview.m_iId);

		// Build two points: left-top and right-bottom
		Vector2D vecLT = OverviewMgr::m_mxTransform * Vector2D(gHUD::m_vecOrigin.x - RADAR_RANGE / 2.0f, gHUD::m_vecOrigin.y - RADAR_RANGE / 2.0f);
		Vector2D vecRB = OverviewMgr::m_mxTransform * Vector2D(gHUD::m_vecOrigin.x + RADAR_RANGE / 2.0f, gHUD::m_vecOrigin.y + RADAR_RANGE / 2.0f);

		// and myself.
		Vector2D vecMe = OverviewMgr::m_mxTransform * Vector2D(gHUD::m_vecOrigin.x, gHUD::m_vecOrigin.y);

		// Build an dynamic matrix. Map the origin onto radar map.
		Matrix3x3 mxRadarTransform = Matrix3x3::Identity();

		// Step 4: Scale it with the entire texture GL coord, i.e., 0~1
		mxRadarTransform *= Matrix3x3(
			1.0f / float(OverviewMgr::m_iWidth),	0.0f,									0.0f,
			0.0f,									1.0f / float(OverviewMgr::m_iHeight),	0.0f,
			0.0f,									0.0f,									1.0f
		);

		// Step 3: Add the centre point back.
		mxRadarTransform *= Matrix3x3::Translation2D(vecMe);

		// Step 2: Rotate it according to our viewing yaw.
		mxRadarTransform *= Matrix3x3::Rotation2D(180.0f - v_angles.yaw);

		// Step 1: Subtract it with our own origin, make us right on centre.
		mxRadarTransform *= Matrix3x3::Translation2D(-vecMe);

		Vector2D vecs[4];
		vecs[0] = mxRadarTransform * vecLT;
		vecs[1] = mxRadarTransform * Vector2D(vecRB.x, vecLT.y);
		vecs[2] = mxRadarTransform * vecRB;
		vecs[3] = mxRadarTransform * Vector2D(vecLT.x, vecRB.y);

		DrawUtils::Draw2DQuadCustomTex(Vector2D(RADAR_BORDER, RADAR_BORDER), Vector2D(RADAR_BORDER + RADAR_HUD_SIZE, RADAR_BORDER + RADAR_HUD_SIZE), vecs);
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
		vecTranslated = Translate(g_PlayerExtraInfo[i].m_vecOrigin, RADAR_RANGE, RADAR_HUD_SIZE);

		// this is because we don't want the icon clipping through radar border.
		if (vecTranslated.x < RADAR_ICON_SIZE / 2 || vecTranslated.x > RADAR_HUD_SIZE - RADAR_ICON_SIZE / 2 ||
			vecTranslated.y < RADAR_ICON_SIZE / 2 || vecTranslated.y > RADAR_HUD_SIZE - RADAR_ICON_SIZE / 2)
		{
			// makes sure that they stay on the radar border..
			vecTranslated.x = Q_clamp(vecTranslated.x, float(RADAR_ICON_SIZE / 2), float(RADAR_HUD_SIZE - RADAR_ICON_SIZE / 2));
			vecTranslated.y = Q_clamp(vecTranslated.y, float(RADAR_ICON_SIZE / 2), float(RADAR_HUD_SIZE - RADAR_ICON_SIZE / 2));

			bClampped = true;
		}
		else
			bClampped = false;

		// offset it with the radar location.
		vecTranslated.x += RADAR_BORDER;
		vecTranslated.y += RADAR_BORDER;

		if ((g_PlayerExtraInfo[i].m_bIsGodfather && (g_iTeam == TEAM_TERRORIST || g_iTeam == TEAM_UNASSIGNED)) || (g_PlayerExtraInfo[i].m_bIsCommander && (g_iTeam == TEAM_CT || g_iTeam == TEAM_UNASSIGNED)))
		{
			color.r = 250;
			color.g = 0;
			color.b = 0;
		}
		else
		{
			if (m_bTrackArray[i])
			{
				iBaseDotSize *= 2;
				color.r = 185;
				color.g = 20;
				color.b = 20;
			}
			else
			{
				color.r = 150;
				color.g = 75;
				color.b = 250;
			}
		}

		// the noobie and illegit players have no icon.
		if (g_PlayerExtraInfo[i].m_iRoleType > Role_UNASSIGNED && g_PlayerExtraInfo[i].m_iRoleType < ROLE_COUNT)
		{
			gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
			gEngfuncs.pTriAPI->Brightness(1.0);

			// in order to make transparent fx on dds texture...
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			switch (g_PlayerExtraInfo[i].m_iRoleType)
			{
			case Role_Arsonist:
			case Role_Assassin:
			case Role_LeadEnforcer:
			case Role_MadScientist:
				color = VEC_T_COLOUR;
				break;

			case Role_Breacher:
			case Role_Medic:
			case Role_Sharpshooter:
			case Role_SWAT:
				color = VEC_CT_COLOUR;
				break;

			case Role_Commander:
				color = VEC_CT_COLOUR + ((Q_sin(gHUD::m_flTime * 2.0f) + 1.0f) / 2.0f) * COMMANDER_COLOR_DIFF;
				break;

			case Role_Godfather:
				color = VEC_T_COLOUR + ((Q_sin(gHUD::m_flTime * 2.0f) + 1.0f) / 2.0f) * GODFATHER_COLOR_DIFF;
				break;

			default:
				color = VEC_YELLOWISH;
				break;
			}

			// to distinguish, the clampped icons will dim out a little bit.
			glColor4f(color.r, color.g, color.b, bClampped ? 0.5 : 1.0);

			gEngfuncs.pTriAPI->CullFace(TRI_NONE);

			glBindTexture(GL_TEXTURE_2D, m_rgiRadarIcons[g_PlayerExtraInfo[i].m_iRoleType]);
			DrawUtils::Draw2DQuad(vecTranslated.x - RADAR_ICON_SIZE / 2, vecTranslated.y - RADAR_ICON_SIZE / 2, vecTranslated.x + RADAR_ICON_SIZE / 2, vecTranslated.y + RADAR_ICON_SIZE / 2);

			if (Q_abs(vecTranslated.z) > 128)
			{
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				// take the same color for the above/below sign.
				glColor4f(color.r, color.g, color.b, bClampped ? 0.5 : 1.0);

				Vector2D vecPeak, vecLeftBottom, vecRightBottom;

				if (vecTranslated.z < 0)
				{
					vecPeak = Vector2D(vecTranslated.x + RADAR_ICON_SIZE * 0.75, vecTranslated.y + RADAR_ICON_SIZE / 2);
					vecLeftBottom = Vector2D(vecTranslated.x + RADAR_ICON_SIZE * 0.5, vecTranslated.y);
					vecRightBottom = Vector2D(vecTranslated.x + RADAR_ICON_SIZE, vecTranslated.y);
				}
				else
				{
					vecPeak = Vector2D(vecTranslated.x + RADAR_ICON_SIZE * 0.75f, vecTranslated.y - RADAR_ICON_SIZE / 2);
					vecLeftBottom = Vector2D(vecTranslated.x + RADAR_ICON_SIZE * 0.5, vecTranslated.y);
					vecRightBottom = Vector2D(vecTranslated.x + RADAR_ICON_SIZE, vecTranslated.y);
				}

				glBegin(GL_POLYGON);
				glVertex2f(vecPeak.x, vecPeak.y);
				glVertex2f(vecLeftBottom.x, vecLeftBottom.y);
				glVertex2f(vecRightBottom.x, vecRightBottom.y);
				glEnd();

				glDisable(GL_BLEND);
				glEnable(GL_TEXTURE_2D);
			}
		}
		else
			DrawRadarDot(vecTranslated, iBaseDotSize, RADAR_DOT_NORMAL, color.r, color.g, color.b, 235);

		if (g_PlayerExtraInfo[i].m_flTimeNextRadarFlash != -1.0 && flTime > g_PlayerExtraInfo[i].m_flTimeNextRadarFlash && g_PlayerExtraInfo[i].m_iRadarFlashRemains > 0)
		{
			g_PlayerExtraInfo[i].m_flTimeNextRadarFlash = flTime + 0.15f;
			g_PlayerExtraInfo[i].m_iRadarFlashRemains--;
			g_PlayerExtraInfo[i].m_bRadarFlashing = !g_PlayerExtraInfo[i].m_bRadarFlashing;
		}

		if (g_PlayerExtraInfo[i].m_bRadarFlashing && g_PlayerExtraInfo[i].m_iRadarFlashRemains > 0)
		{
			color.r = 230;
			color.g = 110;
			color.b = 25;

			DrawRadarDot(vecTranslated.x, vecTranslated.y, vecTranslated.z, iBaseDotSize, RADAR_DOT_BOMBCARRIER, color.r, color.g, color.b, 245);
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
			Vector vec = Translate(m_rgCustomPoints[i].m_vecCoord, RADAR_RANGE, RADAR_HUD_SIZE);
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

Vector CHudRadar::Translate(const Vector& vecOrigin, float flScanRange, float flRadarHudRadius)
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

	if ((-1.0f * y_diff) > flScanRange)
	{
		flScale = (-1.0f * y_diff) / flScanRange;

		xnew_diff /= flScale;
		ynew_diff /= flScale;
	}

	// mapping out the point in real scale into radar size scale.
	xnew_diff /= flScanRange / flRadarHudRadius;
	ynew_diff /= flScanRange / flRadarHudRadius;

	return Vector(	(flRadarHudRadius / 2) + round(xnew_diff),
					(flRadarHudRadius / 2) + round(ynew_diff),
					z_diff);
}

Vector2D CHudRadar::Translate(const Vector& vecOrigin)
{
	Vector2D vecResult;

	if (m_OVData.rotated)
	{
		vecResult.x = ((vecOrigin.x - m_OVData.originX) / m_Overview.m_vecScale.x) + float(m_Overview.m_iWidth) / 2.0f;
		vecResult.y = -((vecOrigin.y - m_OVData.originY) / m_Overview.m_vecScale.y) + float(m_Overview.m_iHeight) / 2.0f;
	}
	else
	{
		vecResult.x = -((vecOrigin.y - m_OVData.originY) / m_Overview.m_vecScale.y) + float(m_Overview.m_iWidth) / 2.0f;
		vecResult.y = -((vecOrigin.x - m_OVData.originX) / m_Overview.m_vecScale.x) + float(m_Overview.m_iHeight) / 2.0f;
	}

	return vecResult;
}

void CHudRadar::Translate(Vector2D& vecOrigin)
{
	Vector2D vecCopy = vecOrigin;

	if (m_OVData.rotated)
	{
		vecOrigin.x = ((vecCopy.x - m_OVData.originX) / m_Overview.m_vecScale.x) + float(m_Overview.m_iWidth) / 2.0f;
		vecOrigin.y = -((vecCopy.y - m_OVData.originY) / m_Overview.m_vecScale.y) + float(m_Overview.m_iHeight) / 2.0f;
	}
	else
	{
		vecOrigin.x = -((vecCopy.y - m_OVData.originY) / m_Overview.m_vecScale.y) + float(m_Overview.m_iWidth) / 2.0f;
		vecOrigin.y = -((vecCopy.x - m_OVData.originX) / m_Overview.m_vecScale.x) + float(m_Overview.m_iHeight) / 2.0f;
	}
}
