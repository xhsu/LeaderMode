/*

Created Date: Mar 11 2020
Remastered Date: May 15 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#include "precompiled.h"

void CHudAccountBalance::Initialize(void)
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
	gFontFuncs::AddGlyphSetToFont(m_hFont, "716", FONT_SIZE, FW_BOLD, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0xFFFF);

#ifdef _DEBUG
	gEngfuncs.pfnAddCommand("cl_debug_blinkacct", []() { MsgFunc_BlinkAcct(Q_atoi(gEngfuncs.Cmd_Argv(1))); });
	gEngfuncs.pfnAddCommand("cl_debug_money", []() { MsgFunc_Money(Q_atoi(gEngfuncs.Cmd_Argv(1))); });
#endif // _DEBUG
}

void CHudAccountBalance::Draw(float flTime, bool bIntermission)
{
	if (bIntermission || gHUD::m_bitsHideHUDDisplay & HIDEHUD_MONEY)
		return;

	if (m_iAccountDisplayed < 0 || m_wcsDelta.empty() || m_wcsNumber.empty())
		return;

	int x = ANCHOR.x - m_iNumTextWidth, y = ANCHOR.y;
	Vector vecColor = COLOR_REGULAR;

	if (m_flBlinkTime >= gHUD::m_flTime)
		vecColor += COLOR_DELTA_BLINK * gHUD::GetOscillation(2.0 * M_PI / BLINK_PERIOD);

	gFontFuncs::DrawSetTextFont(m_hFont);
	gFontFuncs::DrawSetTextPos(x, y);
	gFontFuncs::DrawSetTextColor(vecColor, m_flAlpha / 255.0);
	gFontFuncs::DrawPrintText(m_wcsNumber.c_str());

	gFontFuncs::DrawSetTextPos(ANCHOR_DELTA_SIGN.x, ANCHOR.y);
	gFontFuncs::DrawPrintText(TEXT_DOLLAR_SIGN);

	if (m_flAlphaOfDelta > 1 && m_pwcsSign)
	{
		x = ANCHOR.x - m_iDeltaTextWidth, y = ANCHOR_DELTA_SIGN.y;

		gFontFuncs::DrawSetTextFont(m_hFont);
		gFontFuncs::DrawSetTextPos(x, y);
		gFontFuncs::DrawSetTextColor(COLOR_OF_DELTA, m_flAlphaOfDelta / 255.0);
		gFontFuncs::DrawPrintText(m_wcsDelta.c_str());

		gFontFuncs::DrawSetTextPos(ANCHOR_DELTA_SIGN);
		gFontFuncs::DrawPrintText(m_pwcsSign);
	}
}

void CHudAccountBalance::Think(void)
{
	m_iAccountDelta = m_iAccount - m_iAccountDisplayed;

	switch (m_iPhase)
	{
	case STATIC:
		if (m_flTimeDeltaStatic < gHUD::m_flUCDTime && m_flTimeDeltaStatic != 0)
		{
			m_iPhase = MERGING;
			m_flTimeDeltaStatic = 0;
		}
		break;

	case MERGING:
	{
		int iThisFrameDecrement = round(float(m_iAccountDelta) * gHUD::m_flUCDTimeDelta);

		if (m_iAccountDelta > 0)
			iThisFrameDecrement = Q_max(1, iThisFrameDecrement);
		else if (m_iAccountDelta < 0)
			iThisFrameDecrement = Q_min(iThisFrameDecrement, -1);

		// sign doesn't matter here.
		m_iAccountDelta -= iThisFrameDecrement;
		m_iAccountDisplayed += iThisFrameDecrement;

		m_wcsNumber = std::to_wstring(m_iAccountDisplayed);
		m_wcsDelta = std::to_wstring(Q_abs(m_iAccountDelta));	// the sign is excluded.

		gFontFuncs::GetTextSize(m_hFont, m_wcsNumber.c_str(), &m_iNumTextWidth, nullptr);
		gFontFuncs::GetTextSize(m_hFont, m_wcsDelta.c_str(), &m_iDeltaTextWidth, nullptr);

		if (!m_iAccountDelta)
			m_iPhase = FADEOUT;

		break;
	}

	default:
	case FADEOUT:

		if (m_flBlinkTime < gHUD::m_flTime)
			m_flAlpha = Q_clamp<float>(m_flAlpha - gHUD::m_flUCDTimeDelta * 20.0, MIN_ALPHA, 255);	// Only starts fadeout if the blink time is over.

		m_flAlphaOfDelta = Q_clamp<float>(m_flAlphaOfDelta - gHUD::m_flUCDTimeDelta * 40.0, 0, 255);
		break;
	}
}

void CHudAccountBalance::MsgFunc_Money(int iNewMoney)
{
	if (iNewMoney == m_iAccount)
		return;

	m_iAccount = iNewMoney;
	m_iAccountDelta = m_iAccount - m_iAccountDisplayed;

	m_wcsNumber = std::to_wstring(m_iAccountDisplayed);
	m_wcsDelta = std::to_wstring(Q_abs(m_iAccountDelta));	// the sign is excluded.

	gFontFuncs::GetTextSize(m_hFont, m_wcsNumber.c_str(), &m_iNumTextWidth, nullptr);
	gFontFuncs::GetTextSize(m_hFont, m_wcsDelta.c_str(), &m_iDeltaTextWidth, nullptr);

	m_flAlpha = m_flAlphaOfDelta = 255;
	m_iPhase = STATIC;
	m_flTimeDeltaStatic = gHUD::m_flUCDTime + 2;
	m_pwcsSign = m_iAccountDelta >= 0 ? TEXT_PLUS : TEXT_MINUS;
	COLOR_OF_DELTA = m_iAccountDelta >= 0 ? COLOR_PLUS : COLOR_MINUS;
}

void CHudAccountBalance::MsgFunc_BlinkAcct(int iValue)
{
	m_flBlinkTime = gHUD::m_flTime + BLINK_PERIOD * (iValue * 2);	// Use drawing time is because this calculation happens in HUD_Redraw().
}

void CHudAccountBalance::Reset(void)
{
	m_iAccount = -1, m_iAccountDisplayed = -1, m_iAccountDelta = 0;
	m_flBlinkTime = 0;

	COLOR_OF_DELTA = Vector(1, 1, 1);
	ANCHOR = Vector2D(ScreenWidth - CHudStatusIcons::MARGIN.x, CHudStatusIcons::ANCHOR.y + CHudStatusIcons::SIZE.height + CHudStatusIcons::MARGIN_TEXTS + CHudStatusIcons::FONT_SIZE + MARGIN);
	gFontFuncs::GetTextSize(m_hFont, L"16000", &NUMBER_MAX_WIDTH, nullptr);
	ANCHOR_DELTA_SIGN = Vector2D(ANCHOR.x - NUMBER_MAX_WIDTH - MARGIN_NUMBER_SIGN, ANCHOR.y + FONT_SIZE + MARGIN_DELTA_TO_TOTAL);
	m_iNumTextWidth = 0, m_iDeltaTextWidth = 0;
	m_wcsNumber.clear(), m_wcsDelta.clear();
	m_pwcsSign = nullptr;
	m_flAlphaOfDelta = 0, m_flAlpha = 255;	// byte
	m_iPhase = FADEOUT;
	m_flTimeDeltaStatic = 0;
}

float CHudAccountBalance::GetBottom(void)
{
	return (m_flAlphaOfDelta > 1 && m_pwcsSign) ?
		ANCHOR_DELTA_SIGN.y + FONT_SIZE :	// Delta line exists.
		ANCHOR.y + FONT_SIZE;	// Regular number line only.
}
