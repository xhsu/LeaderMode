/*

Created Date: Mar 11 2020
Rebirth Date: May 18 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#include "precompiled.h"

void CHudAmmo::Initialize(void)
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

	m_hFont = gFontFuncs::CreateFont();
	gFontFuncs::AddGlyphSetToFont(m_hFont, "716", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0xFFFF);
}

void CHudAmmo::Draw(float flTime, bool bIntermission)
{
	if (bIntermission)
		return;

	if (CL_IsDead())
		return;

	if (gHUD::m_bitsHideHUDDisplay & HIDEHUD_WEAPONS)
		return;

	if (!g_pCurWeapon)
		return;

	if (g_pCurWeapon->m_pItemInfo->m_iMaxClip > 0)
	{
		// Clip.
		DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, m_flAlpha);
		DrawUtils::Draw2DQuadProgressBar2(ANCHOR, SIZE, BORDER_THICKNESS, 1);

		DrawUtils::glSetColor(COLOR_CLIP, m_flAlpha / 255.0);
		DrawUtils::Draw2DQuadNoTex(ANCHOR_INNERBLOCK, ANCHOR_INNERBLOCK + SIZE_INNERBLOCK);
		DrawUtils::glRegularPureColorDrawingExit();

		gFontFuncs::DrawSetTextFont(m_hFont);
		gFontFuncs::DrawSetTextPos(ANCHOR_CLIP_TEXT);
		gFontFuncs::DrawSetTextColor(0xFFFFFF, m_flAlpha);
		gFontFuncs::DrawPrintText(m_wcsClip.c_str());
	}

	if (g_pCurWeapon->m_iPrimaryAmmoType != AMMO_NONE)
	{
		// Ammo.
		DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, m_flAlpha);
		DrawUtils::Draw2DQuadProgressBar2(ANCHOR_AMMO, SIZE, BORDER_THICKNESS, 1);
		DrawUtils::glRegularPureColorDrawingExit();

		gFontFuncs::DrawSetTextFont(m_hFont);
		gFontFuncs::DrawSetTextPos(ANCHOR_AMMO_TEXT);
		gFontFuncs::DrawSetTextColor(0xFFFFFF, m_flAlpha);
		gFontFuncs::DrawPrintText(m_wcsAmmo.c_str());
	}
}

void CHudAmmo::Think(void)
{
	m_flAlpha = Q_clamp<float>(m_flAlpha - gHUD::m_flUCDTimeDelta * 20.0f, MIN_ALPHA, 255);

	m_wcsAmmo = (g_pCurWeapon && (g_pCurWeapon->m_iPrimaryAmmoType != AMMO_NONE)) ? std::to_wstring(gPseudoPlayer.m_rgAmmo[g_pCurWeapon->m_iPrimaryAmmoType]) : L"\0";
	m_wcsClip = (g_pCurWeapon && (g_pCurWeapon->m_pItemInfo->m_iMaxClip > 0)) ? std::to_wstring(g_pCurWeapon->m_iClip) : L"\0";

	int iTextWidth = 0;
	gFontFuncs::GetTextSize(m_hFont, m_wcsAmmo.c_str(), &iTextWidth, nullptr);
	ANCHOR_AMMO_TEXT.x = ANCHOR_AMMO.x + (SIZE.width - iTextWidth) / 2;

	gFontFuncs::GetTextSize(m_hFont, m_wcsClip.c_str(), &iTextWidth, nullptr);
	ANCHOR_CLIP_TEXT.x = ANCHOR.x + (SIZE.width - iTextWidth) / 2;
	SIZE_INNERBLOCK.width = (g_pCurWeapon && (g_pCurWeapon->m_pItemInfo->m_iMaxClip > 0)) ? INNERBLOCK_MAX_WIDTH * ((float)g_pCurWeapon->m_iClip / (float)g_pCurWeapon->m_pItemInfo->m_iMaxClip) : 0;
}

void CHudAmmo::Reset(void)
{
	ANCHOR = CHudEquipments::ANCHOR - Vector2D(MARGIN_WITH_EQP + SIZE.width, 0), ANCHOR_INNERBLOCK = ANCHOR + Vector2D(MARGIN_INNERBLOCK + BORDER_THICKNESS);
	SIZE_INNERBLOCK = Vector2D(INNERBLOCK_MAX_WIDTH, INNERBLOCK_HEIGHT);
	ANCHOR_AMMO = ANCHOR + Vector2D(0, SIZE.height + MARGIN_CLIP_BPAMMO);
	ANCHOR_CLIP_TEXT.y = ANCHOR.y + (SIZE.height - INNERBLOCK_HEIGHT) / 2, ANCHOR_AMMO_TEXT.y = ANCHOR_AMMO.y + (SIZE.height - INNERBLOCK_HEIGHT) / 2;	// Only reset its Y value.
	m_flAlpha = 255;	// byte
	m_wcsClip = L"\0", m_wcsAmmo = L"\0";
}
