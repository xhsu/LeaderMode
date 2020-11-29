/*

Created Date: Mar 26 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#include "precompiled.h"

bool CBaseText::Draw(float flTime)
{
	if (!m_hFont)
		return false;

	if (!wcslen(m_wszWords))
		return false;

	float x = GetX();
	float y = GetY();

	gFontFuncs::DrawSetTextFont(m_hFont);
	gFontFuncs::DrawSetTextPos(x, y);
	gFontFuncs::DrawSetTextColor(m_ubColour.r, m_ubColour.g, m_ubColour.b, m_ubColour.a);
	gFontFuncs::DrawPrintText(m_wszWords);

	if (!m_lstChildren.empty())
	{
		for (auto pChild : m_lstChildren)
		{
			if (pChild->m_bitsFlags & HUD_ACTIVE)
				pChild->Draw(flTime);
		}
	}

	return true;
}

void CBaseText::SetUFT8(const char* str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	MultiByteToWideChar(CP_UTF8, 0, str, -1, m_wszWords, len);
	m_wszWords[len] = L'\0';
}

void CBaseText::SetANSI(const char* str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, str, -1, m_wszWords, len);
	m_wszWords[len] = '\0';
}

void CBaseText::SetLocalise(const char* key)
{
	auto pwcText = VGUI_LOCALISE->Find(key);

	if (pwcText)
	{
		wcsncpy_s(m_wszWords, pwcText, wcharsmax(m_wszWords));
	}
	else
	{
		SetANSI(key);	// assume all key is input in ANSI style.
	}
}

int CBaseText::CreateFont(const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
{
	m_iFontSize = tall;
	m_hFont = gFontFuncs::CreateFont();
	gFontFuncs::AddGlyphSetToFont(m_hFont, windowsFontName, tall, weight, blur, scanlines, flags, 0x0, 0xFFFF);
	return m_hFont;
}
