/*

Created Date: Mar 11 2020
Remastered Date: May 11 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#include "precompiled.h"

void CHudBattery::Initialize(void)
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

	m_iIdAssaultSuit = LoadDDS("sprites/Inventory/AssaultSuit.dds");
	m_iIdKevlar = LoadDDS("sprites/Inventory/Kevlar.dds");

	m_hFont = gFontFuncs::CreateFont();
	gFontFuncs::AddGlyphSetToFont(m_hFont, "716", TEXT_HEIGHT, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0xFFFF);

#ifdef _DEBUG
	gEngfuncs.pfnAddCommand("cl_debug_setap", CmdFunc_SetAP);
	gEngfuncs.pfnAddCommand("cl_debug_setat", CmdFunc_SetArmorType);
#endif // _DEBUG

}

void CHudBattery::Draw(float flTime, bool bIntermission)
{
	if (bIntermission)
		return;

	if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_HEALTH) || g_iUser1)
		return;

	if (!m_iAP || m_iArmorType == ARMOR_NONE)
		return;

	// Icon.
	DrawUtils::glRegularTexDrawingInit(0xFFFFFF, m_flAlpha);	// Pure white.
	DrawUtils::glSetTexture(m_iArmorType == ARMOR_KEVLAR ? m_iIdKevlar : m_iIdAssaultSuit);
	DrawUtils::Draw2DQuad(ICON_ANCHOR, ICON_ANCHOR + ICON_SIZE);

	// Progress bar.
	DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, m_flAlpha);

	// Outter line
	DrawUtils::Draw2DQuadProgressBar2(BAR_ANCHOR, BAR_SIZE, BORDER_THICKNESS, 1);

	// Inner block
	DrawUtils::glSetColor(COLOR, m_flAlpha / 255.0);
	DrawUtils::Draw2DQuadNoTex(INNERBLOCK_ANCHOR, INNERBLOCK_ANCHOR + INNERBLOCK_SIZE);

	DrawUtils::glRegularPureColorDrawingExit();

	// Absolute value.
	gFontFuncs::DrawSetTextFont(m_hFont);
	gFontFuncs::DrawSetTextPos(TEXT_ANCHOR.x, TEXT_ANCHOR.y);
	gFontFuncs::DrawSetTextColor(255, 255, 255, 255);	// Text do not fade with others.
	gFontFuncs::DrawPrintText(m_wcsAPText.c_str());
}

void CHudBattery::Think(void)
{
	m_flAlpha = Q_clamp<float>(m_flAlpha - gHUD::m_flUCDTimeDelta * 20.0f, MIN_ALPHA, 255);

	INNERBLOCK_SIZE.width = (BAR_SIZE.width - BORDER_THICKNESS * 2 - PROGRESS_BAR_MARGIN * 2) * float(m_iAP) / GetMaxArmour()/*float(m_iMaxAP)*/;
}

void CHudBattery::Reset(void)
{
	m_iMaxAP = 100, m_iAP = 0, m_iArmorType = ARMOR_NONE;
	m_wcsAPText = L"0";
	gFontFuncs::GetTextSize(m_hFont, m_wcsAPText.c_str(), &m_iTextLength, nullptr);

	ICON_ANCHOR = CHudClassIndicator::PORTRAIT_ANCHOR + Vector2D(CHudClassIndicator::PORTRAIT_SIZE.width + MARGIN_LEFT, 0);
	BAR_ANCHOR = ICON_ANCHOR + Vector2D(ICON_SIZE.width + GAP_SIZE, 0);
	INNERBLOCK_ANCHOR = BAR_ANCHOR + Vector2D(BORDER_THICKNESS + PROGRESS_BAR_MARGIN, BORDER_THICKNESS + PROGRESS_BAR_MARGIN);
	INNERBLOCK_SIZE = Vector2D(0, INNERBLOCK_HEIGHT);	// No width upon reset.
	TEXT_ANCHOR = BAR_ANCHOR + Vector2D((BAR_SIZE.width - float(m_iTextLength)) / 2, (BAR_SIZE.height - TEXT_HEIGHT) / 2);
	m_flAlpha = 255;
}

void CHudBattery::MsgFunc_Battery(const int& iNewArmourValue)
{
	if (iNewArmourValue != m_iAP)
	{
		m_iAP = iNewArmourValue;
		m_flAlpha = 255;

		if (iNewArmourValue <= 0)
		{
			m_iArmorType = ARMOR_NONE;
			m_iAP = 0;	// unsigned. Cannot go lower than 0.
		}
		else if (m_iArmorType == ARMOR_NONE)	// At least I have armor now!
			m_iArmorType = ARMOR_KEVLAR;

		// Update the text.
		m_wcsAPText = std::to_wstring(iNewArmourValue);
		gFontFuncs::GetTextSize(m_hFont, m_wcsAPText.c_str(), &m_iTextLength, nullptr);
		TEXT_ANCHOR = BAR_ANCHOR + Vector2D((BAR_SIZE.width - float(m_iTextLength)) / 2, (BAR_SIZE.height - TEXT_HEIGHT) / 2);
	}
}

void CHudBattery::MsgFunc_ArmorType(const int& iArmourType)
{
	m_iArmorType = iArmourType;
	m_flAlpha = 255;	// light up for a while.
}

#ifdef _DEBUG
void CHudBattery::CmdFunc_SetArmorType(void)
{
	MsgFunc_ArmorType(Q_atoi(gEngfuncs.Cmd_Argv(1)));
}

void CHudBattery::CmdFunc_SetAP(void)
{
	MsgFunc_Battery(Q_atoi(gEngfuncs.Cmd_Argv(1)));
}
#endif // _DEBUG

float CHudBattery::GetMaxArmour(void)
{
	if (g_iRoleType == Role_SWAT)
		return 200.0f;

	return 100.0f;
}
