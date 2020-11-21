/*

Created Date: Mar 11 2020

*/

#include "precompiled.h"

int CHudAccountBalance::Init(void)
{
	gHUD::AddHudElem(this);

	Reset();
	return 1;
}

int CHudAccountBalance::VidInit(void)
{
	m_HUD_dollar = gHUD::GetSpriteIndex("dollar");
	m_HUD_minus = gHUD::GetSpriteIndex("minus");
	m_HUD_plus = gHUD::GetSpriteIndex("plus");

	m_hNumberFont = gFontFuncs.CreateFont();
	gFontFuncs.AddGlyphSetToFont(m_hNumberFont, "Scriptina", 48, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS|FONTFLAG_ADDITIVE, 0x0, 0xFFFF);
	m_hSignFont = gFontFuncs.CreateFont();
	gFontFuncs.AddGlyphSetToFont(m_hSignFont, "Scriptina", 72, FW_BOLD, 1, 0, FONTFLAG_ANTIALIAS | FONTFLAG_ADDITIVE, 0x0, 0xFFFF);

	int r, g, b;
	int r1, g1, b1;
	UnpackRGB(r, g, b, 0xFFFFFF);
	UnpackRGB(r1, g1, b1, RGB_REDISH);
	m_vecBlinkColourDelta = Vector(r1, g1, b1) - Vector(r, g, b);

	return 1;
}

void CHudAccountBalance::Reset(void)
{
	m_bitsFlags |= HUD_ACTIVE;
	m_iAccount = 0;
	m_iAccountDelta = 0;
	m_flAlpha = 0;
	m_bStartFade = false;
	m_flBlinkTime = 0;
}

int CHudAccountBalance::Draw(float flTime)
{
	if (gEngfuncs.IsSpectateOnly())
		return 1;

	if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_MONEY) || g_iUser1)
		return 1;

	if (gHUD::m_bPlayerDead)
		return 1;

	if (m_bStartFade && m_flAlpha > 0)
	{
		m_flAlpha -= gHUD::m_flTimeDelta * 100;
		m_flAlpha = Q_clamp(m_flAlpha, 0.0f, 255.0f);
	}

	// get a posision below radar.
	wchar_t wszText[64];
	int iWidth, iHeight;
	int x = 10;
	int y = 192 + 10;	// FIXME

	int r, g, b;
	int r1, g1, b1;

	if (m_flBlinkTime > gEngfuncs.GetClientTime())
	{
		UnpackRGB(r, g, b, 0xFFFFFF);
		Vector vecDelta = Q_max(0.0f, Q_sin(BLINK_OMEGA * (m_flBlinkTime - gEngfuncs.GetClientTime()))) * m_vecBlinkColourDelta;	// wipe out the negative phase of sine.

		r += vecDelta.x;
		g += vecDelta.y;
		b += vecDelta.z;
	}
	else
	{
		UnpackRGB(r, g, b, 0xFFFFFF);
	}

	_snwprintf(wszText, wcharsmax(wszText), L"$ %d", m_iAccount);
	gFontFuncs.GetTextSize(m_hSignFont, wszText, &iWidth, &iHeight);

	gFontFuncs.DrawSetTextFont(m_hSignFont);
	gFontFuncs.DrawSetTextPos(x, y);
	gFontFuncs.DrawSetTextColor(r, g, b, 255);
	gFontFuncs.DrawPrintText(wszText);

	y += iHeight;
	_snwprintf(wszText, wcharsmax(wszText), m_bDeltaPositiveSign ? L"+%d" : L"-%d", Q_abs(m_iAccountDelta));
	UnpackRGB(r1, g1, b1, m_bDeltaPositiveSign ? RGB_GREENISH : RGB_REDISH);
	gFontFuncs.DrawSetTextFont(m_hNumberFont);
	gFontFuncs.DrawSetTextPos(x, y);
	gFontFuncs.DrawSetTextColor(r1, g1, b1, m_flAlpha);
	gFontFuncs.DrawPrintText(wszText);

	return 1;
}

void CHudAccountBalance::Think(void)
{
	int iLastDelta = m_iAccountDelta;

	if (m_iAccountDelta > 0)
	{
		m_iAccountDelta--;
		m_iAccount++;
	}
	else if (m_iAccountDelta < 0)
	{
		m_iAccountDelta++;
		m_iAccount--;
	}

	// do nothing when m_iAccountDelta == 0

	if (iLastDelta && !m_iAccountDelta)	// which means the number is running out.
	{
		m_bStartFade = true;
		m_flAlpha = 255;
		m_bDeltaPositiveSign = iLastDelta > 0;
	}
}

void CHudAccountBalance::MsgFunc_Money(int& iNewMoney)
{
	m_iAccount += m_iAccountDelta;
	m_iAccountDelta = iNewMoney - m_iAccount;
	m_bDeltaPositiveSign = m_iAccountDelta > 0;

	if (m_iAccountDelta)
	{
		m_bStartFade = false;
		m_flAlpha = 255;
	}
	else
	{
		m_bStartFade = true;
		m_flAlpha = 0;
	}
}

void CHudAccountBalance::MsgFunc_BlinkAcct(int& iValue)
{
	m_flBlinkTime = gEngfuncs.GetClientTime() + BLINK_PERIOD * (iValue * 2);
}
