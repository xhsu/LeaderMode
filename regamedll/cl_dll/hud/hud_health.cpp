/*

Created Date: Mar 11 2020
Remastered Date: May 13 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#include "precompiled.h"


void CHudHealth::Initialize(void)
{
	gHUD::m_lstElements.push_back({
	Initialize,
	nullptr,
	ConnectToServer,
	Draw,
	Think,
	nullptr,
	ServerAsksReset,
	});

	m_hFont = gFontFuncs::CreateFont();
	gFontFuncs::AddGlyphSetToFont(m_hFont, "716", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0xFFFF);

#ifdef _DEBUG
	gEngfuncs.pfnAddCommand("cl_debug_sethealth", CmdFunc_Health);
#endif // _DEBUG

}

void CHudHealth::ConnectToServer(void)
{
	ServerAsksReset();

	m_wcsHPText = L"0";
	m_iHealth = 0;
	m_flPercentage = 0;
}

// view.cpp
//extern Vector v_angles, v_origin;

void CHudHealth::Draw(float flTime, bool bIntermission)
{
	if (bIntermission)
		return;

	if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_HEALTH) || g_iUser1)
		return;

	if (!m_iHealth)
		return;

	// Progress bar.
	DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, m_flAlpha);

	// Outter line
	DrawUtils::Draw2DQuadProgressBar2(ANCHOR, SIZE, BORDER_THICKNESS, 1);

	if (m_iHealth > 0)
	{
		// Inner block
		DrawUtils::glSetColor(COLOR, m_flAlpha / 255.0);
		DrawUtils::Draw2DQuadNoTex(INNERBLOCK_ANCHOR, INNERBLOCK_ANCHOR + INNERBLOCK_SIZE);
	}

	DrawUtils::glRegularPureColorDrawingExit();

	// Absolute value.

	// Shake the text if the health is getting low.
	auto vecTextOrigin = TEXT_ANCHOR;
	if (m_flPercentage < 0.2f)
		vecTextOrigin += Vector2D(RANDOM_FLOAT(-LOW_HP_SHAKING_AMP, LOW_HP_SHAKING_AMP), RANDOM_FLOAT(-LOW_HP_SHAKING_AMP, LOW_HP_SHAKING_AMP));

	gFontFuncs::DrawSetTextFont(m_hFont);
	gFontFuncs::DrawSetTextPos(vecTextOrigin);
	gFontFuncs::DrawSetTextColor(255, 255, 255, 255);	// Text do not fade with others.
	gFontFuncs::DrawPrintText(m_wcsHPText.c_str());
	/*
	auto mx =

		// Step 3: Reverse our Y coord, since the 2D coord system on our monitor is +X for RIGHT, +Y for DOWNWARD.
		Matrix3x3::Stretch2D(1, -1) *

		// Step 2: Rotate the point according to our yaw.
		Matrix3x3::Rotation2D(90.0f - v_angles.yaw) *

		// Step 1: Make ourself the centre of coord system.
		Matrix3x3::Translation2D(-v_origin.Make2D());

	auto org0 = (mx * Vector2D()).SetLength(100);
	auto org = org0 + Vector2D(ScreenWidth / 2, ScreenHeight / 2);
	auto angle = org0 ^ Vector2D(0, -1);

	if (DotProduct2D(org0, Vector2D(-1, 0)) > FLT_EPSILON)	// on the left side.
		angle = 360 - angle;

	constexpr auto s = 20;
	Vector2D orgs[4];
	orgs[0] = org + Vector2D(-s, -s).Rotate(angle);
	orgs[1] = org + Vector2D(-s, s).Rotate(angle);
	orgs[2] = org + Vector2D(s, s).Rotate(angle);
	orgs[3] = org + Vector2D(s, -s).Rotate(angle);

	DrawUtils::glRegularTexDrawingInit(0xFFFFFF, 255);
	DrawUtils::glSetTexture(CHudRadar::RADAR_ICONS[Role_Breacher]);

	gEngfuncs.pTriAPI->Begin(TRI_QUADS);

	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	gEngfuncs.pTriAPI->Vertex3f(orgs[0].x, orgs[0].y, 0);

	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	gEngfuncs.pTriAPI->Vertex3f(orgs[1].x, orgs[1].y, 0);

	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	gEngfuncs.pTriAPI->Vertex3f(orgs[2].x, orgs[2].y, 0);

	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	gEngfuncs.pTriAPI->Vertex3f(orgs[3].x, orgs[3].y, 0);

	gEngfuncs.pTriAPI->End();*/
}

void CHudHealth::Think(void)
{
	if (m_flPercentage >= 0.2f)
		m_flAlpha = Q_clamp<float>(m_flAlpha - gHUD::m_flUCDTimeDelta * 20.0f, MIN_ALPHA, 255);
	else
		m_flAlpha = MIN_ALPHA + (255 - MIN_ALPHA) * gHUD::GetOscillation(2.0 * M_PI / LOW_HP_ALPHA_OSCL_PERIOD);
}

void CHudHealth::ServerAsksReset(void)
{
	m_flAlpha = 255;
	m_iHealth = 0;
	m_wcsHPText = L"0";
	m_flPercentage = 0;
	gFontFuncs::GetTextSize(m_hFont, m_wcsHPText.c_str(), &m_iTextLength, nullptr);

	ANCHOR = CHudBattery::ICON_ANCHOR + Vector2D(0, CHudBattery::ICON_SIZE.height + MARGIN.height);
	INNERBLOCK_SIZE = Vector2D(SIZE.width - BORDER_THICKNESS * 2 - PROGRESS_BAR_MARGIN * 2, INNERBLOCK_HEIGHT);
	INNERBLOCK_ANCHOR = ANCHOR + Vector2D(BORDER_THICKNESS + PROGRESS_BAR_MARGIN);
	TEXT_ANCHOR = ANCHOR + Vector2D((SIZE.width - m_iTextLength) / 2, (SIZE.height - FONT_SIZE) / 2);
}

void CHudHealth::MsgFunc_Health(int iNewHealth)
{
	if (iNewHealth != m_iHealth)
	{
		m_flAlpha = 255;
		m_iHealth = iNewHealth;
		m_flPercentage = float(iNewHealth) / GetMaxHealth()/*float(m_iMaxHP)*/;

		// Update the text.
		m_wcsHPText = std::to_wstring(iNewHealth);
		gFontFuncs::GetTextSize(m_hFont, m_wcsHPText.c_str(), &m_iTextLength, nullptr);
		TEXT_ANCHOR = ANCHOR + Vector2D((SIZE.width - m_iTextLength) / 2, (SIZE.height - FONT_SIZE) / 2);

		// Update the innerblock
		INNERBLOCK_SIZE.width = (SIZE.width - BORDER_THICKNESS * 2 - PROGRESS_BAR_MARGIN * 2) * m_flPercentage;
	}
}

void CHudHealth::MsgFunc_Damage(int armor, int damageTaken, int bitsDamage, const Vector& vecFrom)
{
	// TODO, UNDONE
}

#ifdef _DEBUG
void CHudHealth::CmdFunc_Health(void)
{
	MsgFunc_Health(Q_atoi(gEngfuncs.Cmd_Argv(1)));
}
#endif // _DEBUG

float CHudHealth::GetMaxHealth(void)
{
	if (g_iRoleType == Role_Commander || g_iRoleType == Role_Godfather)
		return 1000.0f;

	return 100.0f;
}
