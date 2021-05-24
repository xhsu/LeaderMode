/*

Created Date: Mar 11 2020
Reincarnation Date: Nov 21 2020

*/

#include "precompiled.h"


void CHudRadar::Initialize(void)
{
	gHUD::m_lstElements.push_back({
		Initialize,
		nullptr,
		Reset,
		Draw,
		Think,
		nullptr,
		Reset,
		});

	// CT
	RADAR_ICONS[Role_Breacher] = LoadDDS("sprites/ClassesIcon/CT/Breacher_Radar.dds");
	RADAR_ICONS[Role_Commander] = LoadDDS("sprites/ClassesIcon/CT/Commander_Radar.dds");
	RADAR_ICONS[Role_Medic] = LoadDDS("sprites/ClassesIcon/CT/Medic_Radar.dds");
	RADAR_ICONS[Role_Sharpshooter] = LoadDDS("sprites/ClassesIcon/CT/Sharpshooter_Radar.dds");
	RADAR_ICONS[Role_SWAT] = LoadDDS("sprites/ClassesIcon/CT/SWAT_Radar.dds");

	// T
	RADAR_ICONS[Role_Arsonist] = LoadDDS("sprites/ClassesIcon/T/Arsonist_Radar.dds");
	RADAR_ICONS[Role_Assassin] = LoadDDS("sprites/ClassesIcon/T/Assassin_Radar.dds");
	RADAR_ICONS[Role_Godfather] = LoadDDS("sprites/ClassesIcon/T/Godfather_Radar.dds");
	RADAR_ICONS[Role_LeadEnforcer] = LoadDDS("sprites/ClassesIcon/T/LeadEnforcer_Radar.dds");
	RADAR_ICONS[Role_MadScientist] = LoadDDS("sprites/ClassesIcon/T/MadScientist_Radar.dds");
}

void CHudRadar::Reset(void)
{
	ANCHOR.x = MARGIN.width;
	ANCHOR.y = ScreenHeight - SIZE.height - CHudClassIndicator::MARGIN - CHudClassIndicator::PORTRAIT_SIZE.height - MARGIN.height;

	m_rgCustomPoints.clear();
}

void CHudRadar::Draw(float flTime, bool bIntermission)
{
	if (gHUD::m_bitsHideHUDDisplay & HIDEHUD_ALL || g_iUser1 || bIntermission)	// TODO: maybe add an independent flag to turn radar off?
		return;

	if (!CL_IsDead())
		DrawRadar();
}

void CHudRadar::Think(void)
{
	// Player radio flash.
	for (auto iter = std::begin(g_PlayerExtraInfo); iter != std::end(g_PlayerExtraInfo); ++iter)
	{
		if (iter->m_flTimeNextRadarFlash != -1.0 && gHUD::m_flUCDTime > iter->m_flTimeNextRadarFlash && iter->m_iRadarFlashRemains > 0)
		{
			iter->m_flTimeNextRadarFlash = gHUD::m_flUCDTime + 0.15f;
			iter->m_iRadarFlashRemains--;
			iter->m_bRadarFlashing = !iter->m_bRadarFlashing;
		}
	}

	// Custom radar points.
	for (auto iter = m_rgCustomPoints.begin(); iter != m_rgCustomPoints.end(); /* Do nothing. */)
	{
		if (iter->second.m_iFlashCounts <= 0 && iter->second.m_flTimeSwitchPhase <= gHUD::m_flUCDTime)	// the last flash
		{
			iter = m_rgCustomPoints.erase(iter);
		}
		else
		{
			iter++;
		}
	}
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

void CHudRadar::DrawRadar(const Vector2D& vecAnchorLT, const Vector2D& vecSize)
{
	int iBaseDotSize = 2;
	Vector color;
	Vector2D vecTranslated;
	float flZDiff = 0;
	bool bClampped = false;
	const Vector2D vecAnchorRB = vecAnchorLT + vecSize;

	auto ClampCoord = [&bClampped, &vecSize](Vector2D& vecToClamp)
	{
		// this is because we don't want the icon clipping through radar border.
		// Don't need to consider anchor and margin here.
		if (vecToClamp.x < ICON_SIZE / 2 + BORDER_THICKNESS || vecToClamp.x > vecSize.x - ICON_SIZE / 2 - BORDER_THICKNESS ||
			vecToClamp.y < ICON_SIZE / 2 + BORDER_THICKNESS || vecToClamp.y > vecSize.y - ICON_SIZE / 2 - BORDER_THICKNESS)
		{
			// makes sure that they stay on the radar border..
			vecToClamp.x = Q_clamp(vecToClamp.x, float(ICON_SIZE / 2 + BORDER_THICKNESS), float(vecSize.x - ICON_SIZE / 2 - BORDER_THICKNESS));
			vecToClamp.y = Q_clamp(vecToClamp.y, float(ICON_SIZE / 2 + BORDER_THICKNESS), float(vecSize.y - ICON_SIZE / 2 - BORDER_THICKNESS));

			bClampped = true;
		}
		else
			bClampped = false;
	};

	// If the window is not a square, it would be some trouble.
	float X_DIAMETER = DIAMETER;
	float Y_DIAMETER = DIAMETER * vecSize.y / vecSize.x;

	if (vecSize.y > vecSize.x)
	{
		X_DIAMETER = DIAMETER * vecSize.x / vecSize.y;
		Y_DIAMETER = DIAMETER;
	}

	// This line comes from experience, not some mathematical deduction.
	if (!OverviewMgr::m_bRotated)
		std::swap(X_DIAMETER, Y_DIAMETER);

	// draw black board.
	DrawUtils::glRegularPureColorDrawingInit(0x000000, 128U);
	DrawUtils::Draw2DQuad(vecAnchorLT, vecAnchorRB);
	DrawUtils::glRegularPureColorDrawingExit();

	// Got a minimap? Draw it first!
	if (OverviewMgr::m_iIdTexture)
	{
		DrawUtils::glRegularTexDrawingInit(0xFFFFFF, 255U);
		DrawUtils::glSetTexture(OverviewMgr::m_iIdTexture);

		// Build two points: left-top and right-bottom
		Vector2D vecLT = OverviewMgr::m_mxTransform * Vector2D(gHUD::m_vecOrigin.x - X_DIAMETER / 2.0f, gHUD::m_vecOrigin.y - Y_DIAMETER / 2.0f);
		Vector2D vecRB = OverviewMgr::m_mxTransform * Vector2D(gHUD::m_vecOrigin.x + X_DIAMETER / 2.0f, gHUD::m_vecOrigin.y + Y_DIAMETER / 2.0f);

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

		DrawUtils::Draw2DQuadCustomTex(vecAnchorLT, vecAnchorRB, vecs);
	}

	// Draw the border to clampl the map range.
	DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, 255U);
	DrawUtils::Draw2DQuadProgressBar2(vecAnchorLT, vecSize, BORDER_THICKNESS, 1);
	DrawUtils::glRegularPureColorDrawingExit();

	// Draw ourself.
	vecTranslated = vecAnchorLT + vecSize / 2;	// I must be the centre of this radar map. Otherwise it will be meaningless.
	color = gHUD::GetColor(gHUD::m_iPlayerNum);

	if (g_iRoleType > Role_UNASSIGNED && g_iRoleType < ROLE_COUNT)
	{
		DrawUtils::glRegularTexDrawingInit(color, 1);
		DrawUtils::glSetTexture(RADAR_ICONS[g_iRoleType]);
		DrawUtils::Draw2DQuad(vecTranslated.x - ICON_SIZE / 2, vecTranslated.y - ICON_SIZE / 2, vecTranslated.x + ICON_SIZE / 2, vecTranslated.y + ICON_SIZE / 2);
	}
	else
	{
		color *= 255.0f;	// due to gEngfuncs.pfnFillRGBA() do not accept 0~1 as its color parameters.
		DrawRadarDot(vecTranslated, 0, iBaseDotSize, RADAR_DOT_NORMAL, color.r, color.g, color.b, 235);
	}

	// Swap them back, we are not dealing with overview map right now.
	if (!OverviewMgr::m_bRotated)
		std::swap(X_DIAMETER, Y_DIAMETER);

	// Build a matrix which transform a world point onto our mini-map.
	m_mxRadarTransform =

		// Step 5: Offset it to avoid negative value. After the first 4 steps, the center of the coordinate system is (0, 0), the left-top corner of the screen. We have to make it centered with the radar's center.
		Matrix3x3::Translation2D(vecSize / 2) *

		// Step 4: Reverse our Y coord, since the 2D coord system on our monitor is +X for RIGHT, +Y for DOWNWARD.
		Matrix3x3::Stretch2D(1, -1) *

		// Step 3: You will have to scale the distance between you and the dots into your visible range. These are the scale factors.
		Matrix3x3::Stretch2D(vecSize.x / X_DIAMETER, vecSize.y / Y_DIAMETER) *

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

		// Makes sure it drops within our border.
		ClampCoord(vecTranslated);

		// offset it with the radar location.
		vecTranslated += vecAnchorLT;

		// determind color.
		color = gHUD::GetColor(i);

		// the noobie and illegit players have no icon.
		if (g_PlayerExtraInfo[i].m_iRoleType > Role_UNASSIGNED && g_PlayerExtraInfo[i].m_iRoleType < ROLE_COUNT)
		{
			DrawUtils::glRegularTexDrawingInit(color, bClampped ? 0.5 : 1.0);
			DrawUtils::glSetTexture(RADAR_ICONS[g_PlayerExtraInfo[i].m_iRoleType]);
			DrawUtils::Draw2DQuad(vecTranslated.x - ICON_SIZE / 2, vecTranslated.y - ICON_SIZE / 2, vecTranslated.x + ICON_SIZE / 2, vecTranslated.y + ICON_SIZE / 2);

			if (Q_abs(flZDiff) > 128)
			{
				// take the same color for the above/below sign.
				DrawUtils::glRegularPureColorDrawingInit(color, bClampped ? 0.5 : 1.0);

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

				DrawUtils::glRegularPureColorDrawingExit();
			}
		}
		else
		{
			color *= 255.0f;	// due to gEngfuncs.pfnFillRGBA() do not accept 0~1 as its color parameters.
			DrawRadarDot(vecTranslated, flZDiff, iBaseDotSize, RADAR_DOT_NORMAL, color.r, color.g, color.b, 235);
		}

		if (g_PlayerExtraInfo[i].m_bRadarFlashing && g_PlayerExtraInfo[i].m_iRadarFlashRemains > 0)
		{
			color.r = 230;
			color.g = 110;
			color.b = 25;

			DrawRadarDot(vecTranslated, flZDiff, iBaseDotSize, RADAR_DOT_BOMBCARRIER, color.r, color.g, color.b, 245);
		}
	}

	for (auto iter = m_rgCustomPoints.begin(); iter != m_rgCustomPoints.end(); ++iter)	// for the tracking skills
	{
		if (iter->second.m_flTimeSwitchPhase <= gHUD::m_flUCDTime)
		{
			iter->second.m_flTimeSwitchPhase = gHUD::m_flUCDTime + iter->second.m_flFlashInterval;
			iter->second.m_bPhase = !iter->second.m_bPhase;
			iter->second.m_iFlashCounts--;
		}

		if (iter->second.m_bPhase)
		{
			vecTranslated = m_mxRadarTransform * iter->second.m_vecCoord.Make2D();
			flZDiff = iter->second.m_vecCoord.z - gHUD::m_vecOrigin.z;

			ClampCoord(vecTranslated);	// Remember, it is need anyway!
			vecTranslated += vecAnchorLT;

			DrawRadarDot(vecTranslated, flZDiff, iBaseDotSize * iter->second.m_iDotSize, iter->second.m_bitsFlags, iter->second.m_color);
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
