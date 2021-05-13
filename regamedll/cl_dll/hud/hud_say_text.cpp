/*

Created Date: Mar 11 2020
Reincarnation Data: Dec 01 2020

*/

#include "precompiled.h"

#define MAX_LINE_WIDTH (ScreenWidth - 40)

static int iDummy, s_iLineHeight = 0;

int CHudSayText::Init(void)
{
	Reset();

	m_pCVar_saytext = gEngfuncs.pfnRegisterVariable("hud_saytext", "1", 0);
	m_pCVar_saytext_time = gEngfuncs.pfnRegisterVariable("hud_saytext_time", "10", 0);

	m_bitsFlags |= HUD_ACTIVE | HUD_INTERMISSION;

	gHUD::AddHudElem(this);
	return 1;
}

void CHudSayText::Reset(void)
{
	m_lstContents.clear();
}

int CHudSayText::Draw(float flTime)
{
	int x = 10;
	int y = GetTextPrintY();

	for (auto& line : m_lstContents)
	{
		for (auto& segment : line.m_lstLineData)
		{
			x = gHUD::m_VGUI2Print.DrawVGUI2String(segment.m_wcsText.c_str(), x, y, segment.m_vecColor.r, segment.m_vecColor.g, segment.m_vecColor.b);
			gHUD::m_VGUI2Print.GetStringSize(segment.m_wcsText.c_str(), &iDummy, &s_iLineHeight);
		}

		x = 10;
		y += s_iLineHeight;
	}

	return 1;
}

void CHudSayText::Think(void)
{
	// remove all out-dated messages.
	auto iterator = m_lstContents.cbegin();
	while (iterator != m_lstContents.cend())
	{
		if (iterator->m_flTimeCreated < gHUD::m_flUCDTime - m_pCVar_saytext_time->value)
		{
			// save a cache in console.
			std::wstring wcs = L"";
			for (auto& elem : iterator->m_lstLineData)
				wcs += elem.m_wcsText;

			// make a new line, you know the console.
			wcs += L"\n";
			gEngfuncs.pfnConsolePrint(UnicodeToUTF8(wcs.c_str()));

			m_lstContents.erase(iterator++);
		}
		else
		{
			++iterator;
		}
	}
}

#define SAYTEXT_COLOR_KEY				L"$["
#define SAYTEXT_CONFIG_TERMINATE_KEY	L"]"

int CHudSayText::GetTextPrintY(void)
{
	return (gHUD::m_Health.m_flLastDrawingY/* - gHUD::m_Battery.m_iHeight - CHudClassIndicator::FONT_TALL*/) - (s_iLineHeight * 5) - (s_iLineHeight * 0.5);
}

void CHudSayText::GetColorFromText(int client_index, const std::wstring& wcsColorText, Vector& vecColor)
{
	// Specific color.
	if (UTIL_CaseInsensitiveCompare(wcsColorText, std::wstring(L"Yellowish")))
		vecColor = VEC_YELLOWISH;
	else if (UTIL_CaseInsensitiveCompare(wcsColorText, std::wstring(L"Redish")))
		vecColor = VEC_REDISH;
	else if (UTIL_CaseInsensitiveCompare(wcsColorText, std::wstring(L"Greenish")))
		vecColor = VEC_GREENISH;
	else if (UTIL_CaseInsensitiveCompare(wcsColorText, std::wstring(L"Cyan")))
		vecColor = VEC_CYANISH;
	else if (UTIL_CaseInsensitiveCompare(wcsColorText, std::wstring(L"Springgreen")))
		vecColor = VEC_SPRINGGREENISH;
	else if (UTIL_CaseInsensitiveCompare(wcsColorText, std::wstring(L"Orange")))
		vecColor = VEC_ORANGE;
	else if (UTIL_CaseInsensitiveCompare(wcsColorText, std::wstring(L"Violet")))
		vecColor = VEC_VIOLET;

	// Team color
	else if (UTIL_CaseInsensitiveCompare(wcsColorText, std::wstring(L"Team")))
	{
		switch (g_PlayerExtraInfo[client_index].m_iTeam)
		{
		case TEAM_CT:
			vecColor = VEC_CT_COLOUR;
			break;

		case TEAM_TERRORIST:
			vecColor = VEC_T_COLOUR;
			break;

		default:
			vecColor = VEC_SILVERISH;
			break;
		}
	}

	// you can specify them as well.
	else if (UTIL_CaseInsensitiveCompare(wcsColorText, std::wstring(L"CT")))
		vecColor = VEC_CT_COLOUR;
	else if (UTIL_CaseInsensitiveCompare(wcsColorText, std::wstring(L"Terrorist")))
		vecColor = VEC_T_COLOUR;
	else if (UTIL_CaseInsensitiveCompare(wcsColorText, std::wstring(L"Spec")))
		vecColor = VEC_SILVERISH;

	// default color: yellow.
	else
		vecColor = VEC_YELLOWISH;
}

void CHudSayText::AddToSayText(int iClientIndex, const char* formatStr, const char* sstr1, const char* sstr2, const char* sstr3, const char* sstr4)
{
	// if this is an localisable text, it will be localised, otherwise it will still be convered to an Unicode text.
	const wchar_t* pwcsText = UTIL_GetLocalisation(formatStr);
	const wchar_t* pwcsArg1 = UTIL_GetLocalisation(sstr1);
	const wchar_t* pwcsArg2 = UTIL_GetLocalisation(sstr2);
	const wchar_t* pwcsArg3 = UTIL_GetLocalisation(sstr3);
	const wchar_t* pwcsArg4 = UTIL_GetLocalisation(sstr4);

	// get back on track.
	AddToSayText(iClientIndex, pwcsText, pwcsArg1, pwcsArg2, pwcsArg3, pwcsArg4);
}

void CHudSayText::AddToSayText(int iClientIndex, const wchar_t* pwcsText, const wchar_t* pwcsArg1, const wchar_t* pwcsArg2, const wchar_t* pwcsArg3, const wchar_t* pwcsArg4)
{
	static wchar_t wcsFinalDraft[1024] = L"";
	const wchar_t* pwcsTest = wcsstr(pwcsText, L"%s");
	if (pwcsTest && pwcsTest[2] > '0' && pwcsTest[2] < '9')	// two letters afterward. Is it the form of "Blablabla %s1, bla %s2 bla"?
	{
		// use VGUI API here.
		VGUI_LOCALISE->ConstructString(wcsFinalDraft, sizeof(wcsFinalDraft), (wchar_t*)pwcsText, 4, pwcsArg1, pwcsArg2, pwcsArg3, pwcsArg4);
	}
	else
	{
		_snwprintf(wcsFinalDraft, wcharsmax(wcsFinalDraft), pwcsText, pwcsArg1, pwcsArg2, pwcsArg3, pwcsArg4);
	}

	linedata_t lstLineData;
	std::wstring wcs = wcsFinalDraft;
	size_t iPosition = 0U, iLastPosition = 0U, iColorTextStarts = 0U, iColorTextEnds = 0U;
	Vector vecColor = VEC_YELLOWISH;

	while ((iPosition = wcs.find(SAYTEXT_COLOR_KEY, iLastPosition)) != std::wstring::npos)
	{
		// prevent empty first string.
		if (iPosition != iLastPosition)
		{
			// the second parameter is length, not the second position.
			lstLineData.emplace_back(
				vecColor,
				wcs.substr(iLastPosition, iPosition - iLastPosition)
			);
		}

		// I don't know why, but we have to keep this at post position. concluded by experiments.
		// _countof(CONST_SZ) - 1U is used for these situation: text position. (remove the additional '\0' at the end.)
		iColorTextStarts = iPosition + _countof(SAYTEXT_COLOR_KEY) - 1U;
		iColorTextEnds = wcs.find(SAYTEXT_CONFIG_TERMINATE_KEY, iPosition);
		GetColorFromText(iClientIndex, wcs.substr(iColorTextStarts, iColorTextEnds - iColorTextStarts), vecColor);

		// ignore "]" symble as well.
		iLastPosition = wcs.find(SAYTEXT_CONFIG_TERMINATE_KEY, iPosition) + _countof(SAYTEXT_CONFIG_TERMINATE_KEY) - 1U;
	}

	// for the last segment.
	// its color already been read above in the very last while loop.
	lstLineData.emplace_back(
		vecColor,
		wcs.substr(iLastPosition)
	);

	// add this line into the larger list.
	m_lstContents.push_back(lstLineData);
}
