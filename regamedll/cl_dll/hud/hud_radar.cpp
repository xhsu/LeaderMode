/*

Created Date: Mar 11 2020
Reincarnation Date: Nov 21 2020

*/

#include "precompiled.h"

int CHudRadar::Init(void)
{
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

// view.cpp
extern Vector v_angles, v_origin;

void CHudRadar::DrawRadar(float flTime)
{
	int iBaseDotSize = 2;
	Vector color;
	Vector2D vecTranslated;
	float flZDiff = 0;
	bool bClampped = false;

	// draw white board.
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(0, 0, 0, 0.5);
	DrawUtils::Draw2DQuad(BORDER_GAP, BORDER_GAP, BORDER_GAP + HUD_SIZE, BORDER_GAP + HUD_SIZE);

	glColor4f(1, 1, 1, 1);
	DrawUtils::Draw2DQuadProgressBar(BORDER_GAP, BORDER_GAP, HUD_SIZE, HUD_SIZE, 2, 1);

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	// Got a minimap? Draw it first!
	if (OverviewMgr::m_iIdTexture)
	{
		gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
		gEngfuncs.pTriAPI->Brightness(1.0);

		// in order to make transparent fx on dds texture...
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1, 1, 1, 1);

		gEngfuncs.pTriAPI->CullFace(TRI_NONE);
		
		glBindTexture(GL_TEXTURE_2D, OverviewMgr::m_iIdTexture);

		// Build two points: left-top and right-bottom
		Vector2D vecLT = OverviewMgr::m_mxTransform * Vector2D(gHUD::m_vecOrigin.x - DIAMETER / 2.0f, gHUD::m_vecOrigin.y - DIAMETER / 2.0f);
		Vector2D vecRB = OverviewMgr::m_mxTransform * Vector2D(gHUD::m_vecOrigin.x + DIAMETER / 2.0f, gHUD::m_vecOrigin.y + DIAMETER / 2.0f);

		// and myself.
		Vector2D vecMe = OverviewMgr::m_mxTransform * Vector2D(gHUD::m_vecOrigin.x, gHUD::m_vecOrigin.y);

		// Build an dynamic matrix. Map the origin onto radar map.
		Matrix3x3 mxMiniMapTransform =

			// Step 4: Scale it with the entire texture GL coord, i.e., 0~1
			Matrix3x3::Squeeze2D(OverviewMgr::m_iWidth, OverviewMgr::m_iHeight) *

			// Step 3: Add the centre point back.
			Matrix3x3::Translation2D(vecMe) *

			// Step 2: Rotate it according to our viewing yaw.
			Matrix3x3::Rotation2D(OverviewMgr::m_bRotated ? 270.0f - v_angles.yaw : 180.0f - v_angles.yaw) *

			// Step 1: Subtract it with our own origin, make us right on centre.
			Matrix3x3::Translation2D(-vecMe);

		Vector2D vecs[4];
		vecs[0] = mxMiniMapTransform * vecLT;
		vecs[1] = mxMiniMapTransform * Vector2D(vecRB.x, vecLT.y);
		vecs[2] = mxMiniMapTransform * vecRB;
		vecs[3] = mxMiniMapTransform * Vector2D(vecLT.x, vecRB.y);

		if (OverviewMgr::m_bRotated)
		{
			// This is only used in de_dust_cz, for its bugged overview.
			//std::swap(vecs[0], vecs[3]);
			//std::swap(vecs[1], vecs[2]);

			// This is the regular method.
			// USAGI: assume the centre of overview is (0, 0).
			// Reflected about line x = 0. (i.e., Y axis.)
			std::swap(vecs[0], vecs[1]);
			std::swap(vecs[2], vecs[3]);

			// Inverted about (0, 0).
			for (auto& v : vecs)
			{
				v.x = 1.0f - v.x;
				v.y = 1.0f - v.y;
			}
		}

		DrawUtils::Draw2DQuadCustomTex(Vector2D(BORDER_GAP, BORDER_GAP), Vector2D(BORDER_GAP + HUD_SIZE, BORDER_GAP + HUD_SIZE), vecs);
	}

	// Draw ourself.
	vecTranslated = Vector2D(BORDER_GAP + HUD_SIZE / 2, BORDER_GAP + HUD_SIZE / 2);	// I must be the centre of this radar map. Otherwise it will be meaningless.
	color = gHUD::GetColor(gHUD::m_iPlayerNum);

	if (g_iRoleType > Role_UNASSIGNED && g_iRoleType < ROLE_COUNT)
	{
		gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
		gEngfuncs.pTriAPI->Brightness(1.0);

		// in order to make transparent fx on dds texture...
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		gEngfuncs.pTriAPI->CullFace(TRI_NONE);

		glColor4f(color.r, color.g, color.b, 1);
		glBindTexture(GL_TEXTURE_2D, m_rgiRadarIcons[g_iRoleType]);
		DrawUtils::Draw2DQuad(vecTranslated.x - ICON_SIZE / 2, vecTranslated.y - ICON_SIZE / 2, vecTranslated.x + ICON_SIZE / 2, vecTranslated.y + ICON_SIZE / 2);
	}
	else
	{
		color *= 255.0f;	// due to gEngfuncs.pfnFillRGBA() do not accept 0~1 as its color parameters.
		DrawRadarDot(vecTranslated, 0, iBaseDotSize, RADAR_DOT_NORMAL, color.r, color.g, color.b, 235);
	}

	// Build a matrix which transform a world point onto our mini-map.
	m_mxRadarTransform =

		// Step 5: Offset it to avoid negative value. After the first 4 steps, the center of the coordinate system is (0, 0), the left-top corner of the screen. We have to make it centered with the radar's center.
		Matrix3x3::Translation2D(Vector2D(HUD_SIZE / 2, HUD_SIZE / 2)) *

		// Step 4: Reverse our Y coord, since the 2D coord system on our monitor is +X for RIGHT, +Y for DOWNWARD.
		Matrix3x3::Stretch2D(1, -1) *

		// Step 3: Squeeze the coord to fit our radar and map range.
		Matrix3x3::Stretch2D(HUD_SIZE / DIAMETER) *

		// Step 2: Rotate the point according to our yaw.
		Matrix3x3::Rotation2D(90.0f - v_angles.yaw) *

		// Step 1: Make ourself the centre of coord system.
		Matrix3x3::Translation2D(-v_origin.Make2D());

	for (size_t i = 0; i < MAX_CLIENTS; i++)
	{
		if (!g_PlayerInfoList[i].name || !g_PlayerInfoList[i].name[0])
			continue;

		if (gHUD::m_iPlayerNum == i || g_PlayerExtraInfo[i].m_iTeam != g_iTeam || g_PlayerExtraInfo[i].m_bIsDead)
			continue;

		if (g_PlayerExtraInfo[i].m_iHealth <= 0)	// no dead guy allowed.
			continue;

		// translate the location we received to radar coord.
		vecTranslated = m_mxRadarTransform * gEngfuncs.GetEntityByIndex(i)->origin.Make2D();
		flZDiff = gEngfuncs.GetEntityByIndex(i)->origin.z - gHUD::m_vecOrigin.z;

		// this is because we don't want the icon clipping through radar border.
		if (vecTranslated.x < ICON_SIZE / 2 || vecTranslated.x > HUD_SIZE - ICON_SIZE / 2 ||
			vecTranslated.y < ICON_SIZE / 2 || vecTranslated.y > HUD_SIZE - ICON_SIZE / 2)
		{
			// makes sure that they stay on the radar border..
			vecTranslated.x = Q_clamp(vecTranslated.x, float(ICON_SIZE / 2), float(HUD_SIZE - ICON_SIZE / 2));
			vecTranslated.y = Q_clamp(vecTranslated.y, float(ICON_SIZE / 2), float(HUD_SIZE - ICON_SIZE / 2));

			bClampped = true;
		}
		else
			bClampped = false;

		// offset it with the radar location.
		vecTranslated.x += BORDER_GAP;
		vecTranslated.y += BORDER_GAP;

		// determind color.
		color = gHUD::GetColor(i);

		// the noobie and illegit players have no icon.
		if (g_PlayerExtraInfo[i].m_iRoleType > Role_UNASSIGNED && g_PlayerExtraInfo[i].m_iRoleType < ROLE_COUNT)
		{
			gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
			gEngfuncs.pTriAPI->Brightness(1.0);

			// in order to make transparent fx on dds texture...
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// to distinguish, the clampped icons will dim out a little bit.
			glColor4f(color.r, color.g, color.b, bClampped ? 0.5 : 1.0);

			gEngfuncs.pTriAPI->CullFace(TRI_NONE);

			glBindTexture(GL_TEXTURE_2D, m_rgiRadarIcons[g_PlayerExtraInfo[i].m_iRoleType]);
			DrawUtils::Draw2DQuad(vecTranslated.x - ICON_SIZE / 2, vecTranslated.y - ICON_SIZE / 2, vecTranslated.x + ICON_SIZE / 2, vecTranslated.y + ICON_SIZE / 2);

			if (Q_abs(flZDiff) > 128)
			{
				glDisable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				// take the same color for the above/below sign.
				glColor4f(color.r, color.g, color.b, bClampped ? 0.5 : 1.0);

				Vector2D vecPeak, vecLeftBottom, vecRightBottom;

				if (flZDiff < 0)
				{
					vecPeak = Vector2D(vecTranslated.x + ICON_SIZE * 0.75, vecTranslated.y + ICON_SIZE / 2);
					vecLeftBottom = Vector2D(vecTranslated.x + ICON_SIZE * 0.5, vecTranslated.y);
					vecRightBottom = Vector2D(vecTranslated.x + ICON_SIZE, vecTranslated.y);
				}
				else
				{
					vecPeak = Vector2D(vecTranslated.x + ICON_SIZE * 0.75f, vecTranslated.y - ICON_SIZE / 2);
					vecLeftBottom = Vector2D(vecTranslated.x + ICON_SIZE * 0.5, vecTranslated.y);
					vecRightBottom = Vector2D(vecTranslated.x + ICON_SIZE, vecTranslated.y);
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
		{
			color *= 255.0f;	// due to gEngfuncs.pfnFillRGBA() do not accept 0~1 as its color parameters.
			DrawRadarDot(vecTranslated, flZDiff, iBaseDotSize, RADAR_DOT_NORMAL, color.r, color.g, color.b, 235);
		}

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

			DrawRadarDot(vecTranslated, flZDiff, iBaseDotSize, RADAR_DOT_BOMBCARRIER, color.r, color.g, color.b, 245);
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
			vecTranslated = m_mxRadarTransform * m_rgCustomPoints[i].m_vecCoord.Make2D();
			flZDiff = m_rgCustomPoints[i].m_vecCoord.z - gHUD::m_vecOrigin.z;
			DrawRadarDot(vecTranslated, flZDiff, iBaseDotSize * m_rgCustomPoints[i].m_iDotSize, m_rgCustomPoints[i].m_bitsFlags, m_rgCustomPoints[i].m_color);
		}
	}

	// the text indicates where player is.
	DrawPlayerLocation();
}

void CHudRadar::DrawPlayerLocation(void)
{
	const wchar_t* locString;
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
		locString = UTIL_GetLocalisation(g_PlayerExtraInfo[gHUD::m_iPlayerNum].m_szLocationText);
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

	x = Q_max(0, center_x - (string_width / 2));
	y = center_y + (string_height / 2);

	if (wcslen(locString) > 0)
		gHUD::m_VGUI2Print.DrawVGUI2String(locString, x, y, g_LocationColor[0], g_LocationColor[1], g_LocationColor[2]);
}
