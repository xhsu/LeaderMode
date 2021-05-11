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
}

void CHudBattery::Draw(float flTime, bool bIntermission)
{
	if (bIntermission)
		return;

	if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_HEALTH) || g_iUser1)
		return;

	if (!m_iAP || m_iArmorType == ARMOR_NONE)
		return;

	gEngfuncs.pTriAPI->Brightness(1.0);
	gEngfuncs.pTriAPI->Color4fRendermode(1, 1, 1, m_flAlpha / 255.0, kRenderTransColor);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);

	glBindTexture(GL_TEXTURE_2D, m_iArmorType == ARMOR_KEVLAR ? m_iIdKevlar : m_iIdAssaultSuit);
	DrawUtils::Draw2DQuad(ICON_MARGINE, ICON_MARGINE + ICON_SIZE);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Outter line
	DrawUtils::Draw2DQuadProgressBar2(BAR_MARGINE, BAR_SIZE, BORDER_THICKNESS, 1);

	// Inner block
	glColor4f(COLOR.r, COLOR.g, COLOR.b, m_flAlpha / 255.0);
	DrawUtils::Draw2DQuadNoTex(INNERBLOCK_MARGINE, INNERBLOCK_MARGINE + INNERBLOCK_SIZE);

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	gFontFuncs::DrawSetTextFont(m_hFont);
	gFontFuncs::DrawSetTextPos(TEXT_MARGINE.x, TEXT_MARGINE.y);
	gFontFuncs::DrawSetTextColor(255, 255, 255, 255);	// Text do not fade with others.
	gFontFuncs::DrawPrintText(m_wcsAPText.c_str());
}

void CHudBattery::Think(void)
{
	m_flAlpha = Q_clamp(m_flAlpha - gHUD::m_flUCDTimeDelta * 20.0f, 128.0f, 255.0f);

	INNERBLOCK_SIZE.width = (BAR_SIZE.width - BORDER_THICKNESS * 2 - PROGRESS_BAR_GAP_SIZE * 2) * float(m_iAP) / GetMaxArmour()/*float(m_iMaxAP)*/;
}

void CHudBattery::Reset(void)
{
	m_iMaxAP = 100, m_iAP = 0, m_iArmorType = ARMOR_NONE;
	m_wcsAPText = L"0";
	gFontFuncs::GetTextSize(m_hFont, m_wcsAPText.c_str(), &m_iTextLength, nullptr);
	ICON_MARGINE = Vector2D();
	BAR_MARGINE = ICON_MARGINE + Vector2D(ICON_SIZE.width + GAP_SIZE, 0);
	INNERBLOCK_MARGINE = BAR_MARGINE + Vector2D(BORDER_THICKNESS + PROGRESS_BAR_GAP_SIZE, BORDER_THICKNESS + PROGRESS_BAR_GAP_SIZE);
	INNERBLOCK_SIZE = Vector2D(0, BAR_SIZE.height - BORDER_THICKNESS * 2 - PROGRESS_BAR_GAP_SIZE * 2);	// No width on reset.
	TEXT_MARGINE = BAR_MARGINE + Vector2D((BAR_SIZE.width - float(m_iTextLength)) / 2, INNERBLOCK_SIZE.height - TEXT_HEIGHT / 2);
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
		TEXT_MARGINE = BAR_MARGINE + Vector2D((BAR_SIZE.width - float(m_iTextLength)) / 2, 0);
	}
}

void CHudBattery::MsgFunc_ArmorType(const int& iArmourType)
{
	m_iArmorType = iArmourType;
}

float CHudBattery::GetMaxArmour(void)
{
	if (g_iRoleType == Role_SWAT)
		return 200.0f;

	return 100.0f;
}
