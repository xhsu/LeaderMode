/*

Created Date: Mar 12 2020
Remastered Date: May 14 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#include "precompiled.h"

std::array<GLuint, SCHEMES_COUNT> g_rgiIdSchemeIcons = { 0, 0, 0, 0, 0 };

void CHudStatusIcons::Initialize(void)
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

	g_rgiIdSchemeIcons[Scheme_UNASSIGNED] = LoadDDS("sprites/Schemes/Disputing.dds");
	g_rgiIdSchemeIcons[Doctrine_GrandBattleplan] = LoadDDS("sprites/Schemes/Doctrine_GrandBattleplan.dds");
	g_rgiIdSchemeIcons[Doctrine_MassAssault] = LoadDDS("sprites/Schemes/Doctrine_MassAssault.dds");
	g_rgiIdSchemeIcons[Doctrine_MobileWarfare] = LoadDDS("sprites/Schemes/Doctrine_MobileWarfare.dds");
	g_rgiIdSchemeIcons[Doctrine_SuperiorFirepower] = LoadDDS("sprites/Schemes/Doctrine_SuperiorFirepower.dds");

#ifdef _DEBUG
	gEngfuncs.pfnAddCommand("cl_debug_setscheme", []() { MsgFunc_Scheme((TacticalSchemes)Q_atoi(gEngfuncs.Cmd_Argv(1))); });
#endif // _DEBUG

}

void CHudStatusIcons::Draw(float flTime, bool bIntermission)
{
	if (bIntermission || gHUD::m_bitsHideHUDDisplay & HIDEHUD_ALL)
		return;

	if (m_mapShowing.empty())
		return;

	if (CL_IsDead())
		return;

	float x = ANCHOR.x - SIZE.width, y = ANCHOR.y;
	Vector vecColour;
	float flAlpha = 255;	// byte

	for (const auto& [key, icon] : m_mapShowing)
	{
		if (icon.m_bitsFlags & STATUSICON_OSCLI_ALPHA)
			flAlpha = icon.m_flAlpha + icon.m_flOscillationAlphaDelta * gHUD::GetOscillation(2.0 * M_PI / icon.m_flAlphaOscillationPeriod);
		else
			flAlpha = icon.m_flAlpha;

		if (icon.m_bitsFlags & STATUSICON_OSCLI_COLOUR)
			vecColour = icon.m_vecColour + icon.m_vecOscillationColorDelta * gHUD::GetOscillation(2.0 * M_PI / icon.m_flColorOscillationPeriod);
		else
			vecColour = icon.m_vecColour;

		flAlpha /= 255.0;	// GLfloat, [0-1]

		DrawUtils::glRegularTexDrawingInit(vecColour, flAlpha);
		DrawUtils::glSetTexture(icon.m_iIdTexture);
		DrawUtils::Draw2DQuad(x, y, x + SIZE.width, y + SIZE.height);

		if (icon.m_wcsText.length())
		{
			Vector2D vecTextOrg = Vector2D(x + (SIZE.width - icon.m_iTextLength) / 2, y + SIZE.height + MARGIN_TEXTS);

			if (icon.m_bitsFlags & STATUSICON_TEXT_HUE_INDEPENDENT)
				vecColour = icon.m_vecTextColour;
			if (icon.m_bitsFlags & (STATUSICON_TEXT_ALPHA_INDEPENDENT | STATUSICON_TEXT_ALPHA_NOFADE))	// no fade version store in the same place.
				flAlpha = icon.m_flTextAlpha;	// GLfloat

			gFontFuncs::DrawSetTextFont(m_hFont);
			gFontFuncs::DrawSetTextPos(vecTextOrg);
			gFontFuncs::DrawSetTextColor(vecColour, flAlpha);
			gFontFuncs::DrawPrintText(icon.m_wcsText.c_str());
		}

		x -= SIZE.width;
		x -= MARGIN_ICONS;
	}
}

void CHudStatusIcons::Think(void)
{
	// Fade out on everything.
	for (auto& [key, icon] : m_mapShowing)
	{
		if (!(icon.m_bitsFlags & (STATUSICON_OSCLI_ALPHA | STATUSICON_ICON_ALPHA_NOFADE)))
			icon.m_flAlpha = Q_clamp<float>(icon.m_flAlpha - gHUD::m_flUCDTimeDelta * 20.0f, MIN_ALPHA, 255);	// byte

		if (icon.m_bitsFlags & STATUSICON_TEXT_ALPHA_INDEPENDENT && !(icon.m_bitsFlags & STATUSICON_TEXT_ALPHA_NOFADE))
			icon.m_flTextAlpha = Q_clamp<float>(icon.m_flAlpha - gHUD::m_flUCDTimeDelta * (20.0f / 255.0f), (double)MIN_ALPHA / 255.0, 1.0);	// GLfloat
	}
}

void CHudStatusIcons::MsgFunc_StatusIcon(int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);

	/*
	WRITE_STRING("keyname");
	WRITE_SHORT(bigsFlags);
	*/
	std::string szKey = READ_STRING();
	m_mapShowing[szKey].m_bitsFlags = READ_SHORT();

	if (m_mapShowing[szKey].m_bitsFlags & STATUSICON_MSGFL_DEL_THIS)
	{
		m_mapShowing.erase(szKey);
		return;
	}

	/*
	WRITE_STRING("sprite path");
	WRITE_BYTE(r);
	WRITE_BYTE(g);
	WRITE_BYTE(b);
	WRITE_BYTE(a);
	*/
	std::string szSprite = READ_STRING();

	if (m_mapTextureLib[szSprite] == 0U)	// Texture no found.
		m_mapTextureLib[szSprite] = LoadDDS(szSprite.c_str());

	m_mapShowing[szKey].m_iIdTexture = m_mapTextureLib[szSprite];
	m_mapShowing[szKey].m_vecColour.r = (float)READ_BYTE() / 255.0;
	m_mapShowing[szKey].m_vecColour.g = (float)READ_BYTE() / 255.0;
	m_mapShowing[szKey].m_vecColour.b = (float)READ_BYTE() / 255.0;
	m_mapShowing[szKey].m_flAlpha = (float)READ_BYTE();

	/*
	* (optional)
	WRITE_BYTE(another r);
	WRITE_BYTE(another g);
	WRITE_BYTE(another b);
	WRITE_BYTE(OscillationPeriod in 0.1's);
	*/
	if (m_mapShowing[szKey].m_bitsFlags & STATUSICON_OSCLI_COLOUR)
	{
		m_mapShowing[szKey].m_vecOscillationColorDelta.r = (float)READ_BYTE() / 255.0;
		m_mapShowing[szKey].m_vecOscillationColorDelta.g = (float)READ_BYTE() / 255.0;
		m_mapShowing[szKey].m_vecOscillationColorDelta.b = (float)READ_BYTE() / 255.0;

		m_mapShowing[szKey].m_vecOscillationColorDelta -= m_mapShowing[szKey].m_vecColour;

		m_mapShowing[szKey].m_flColorOscillationPeriod = (float)READ_BYTE() / 10.0f;
	}

	/*
	* (optional)
	WRITE_BYTE(another a);
	WRITE_BYTE(OscillationPeriod in 0.1's);
	*/
	if (m_mapShowing[szKey].m_bitsFlags & STATUSICON_OSCLI_ALPHA)
	{
		m_mapShowing[szKey].m_flOscillationAlphaDelta = (float)READ_BYTE() - m_mapShowing[szKey].m_flAlpha;
		m_mapShowing[szKey].m_flAlphaOscillationPeriod = (float)READ_BYTE() / 10.0f;
	}

	/*
	* (optional)
	WRITE_STRING("extra string, as KEYBIND, LOCALISABLE or LITERAL");
	*/
	if (m_mapShowing[szKey].m_bitsFlags & STATUSICON_TEXT_KEYBIND)
	{
		m_mapShowing[szKey].m_wcsText = L"[ " + std::wstring(ANSIToUnicode(gExtFuncs.pfnKey_NameForBinding(READ_STRING()))) + L" ]";
	}
	else if (m_mapShowing[szKey].m_bitsFlags & STATUSICON_TEXT_LOC)
	{
		m_mapShowing[szKey].m_wcsText = UTIL_GetLocalisation(READ_STRING());
	}
	else if (m_mapShowing[szKey].m_bitsFlags & STATUSICON_TEXT_LITERAL)	// we can't use 'else' it because that we have to reserve a situation that no extra text is needed.
	{
		m_mapShowing[szKey].m_wcsText = ANSIToUnicode(READ_STRING());
	}

	if (m_mapShowing[szKey].m_wcsText.length())
	{
		gFontFuncs::GetTextSize(m_hFont, m_mapShowing[szKey].m_wcsText.c_str(), &m_mapShowing[szKey].m_iTextLength, nullptr);
	}

	/*
	* (optional)
	WRITE_BYTE(text r);
	WRITE_BYTE(text g);
	WRITE_BYTE(text b);
	*/
	if (m_mapShowing[szKey].m_bitsFlags & STATUSICON_TEXT_HUE_INDEPENDENT)
	{
		m_mapShowing[szKey].m_vecTextColour.r = (float)READ_BYTE() / 255.0;
		m_mapShowing[szKey].m_vecTextColour.g = (float)READ_BYTE() / 255.0;
		m_mapShowing[szKey].m_vecTextColour.b = (float)READ_BYTE() / 255.0;
	}

	/*
	* (optional)
	WRITE_BYTE(text a);
	*/
	if (m_mapShowing[szKey].m_bitsFlags & STATUSICON_TEXT_ALPHA_INDEPENDENT)
	{
		m_mapShowing[szKey].m_flTextAlpha = (float)READ_BYTE() / 255.0;
	}
	else if (m_mapShowing[szKey].m_bitsFlags & STATUSICON_TEXT_ALPHA_NOFADE)	// not independent, but won't fade out.
	{
		m_mapShowing[szKey].m_flTextAlpha = m_mapShowing[szKey].m_flAlpha / 255.0;
	}
}

void CHudStatusIcons::MsgFunc_Scheme(TacticalSchemes iScheme)
{
	m_mapShowing["scheme"].m_iIdTexture = g_rgiIdSchemeIcons[iScheme];

	switch (iScheme)
	{
	case Scheme_UNASSIGNED:
		m_mapShowing["scheme"].m_bitsFlags = STATUSICON_OSCLI_ALPHA | STATUSICON_OSCLI_COLOUR | STATUSICON_TEXT_KEYBIND | STATUSICON_TEXT_ALPHA_INDEPENDENT;
		m_mapShowing["scheme"].m_flAlpha = MIN_ALPHA;	// under oscillating mode, this is the baseline of each oscillation.
		break;

	default:
		m_mapShowing["scheme"].m_bitsFlags = STATUSICON_TEXT_KEYBIND;
		m_mapShowing["scheme"].m_flAlpha = 255;
		break;
	}
}


void CHudStatusIcons::Reset(void)
{
	ANCHOR = Vector2D(ScreenWidth, 0) + Vector2D(-MARGIN.x, MARGIN.y);

	m_mapShowing.clear();

	// The first item must be scheme.
	m_mapShowing["scheme"].m_bitsFlags = STATUSICON_OSCLI_ALPHA | STATUSICON_OSCLI_COLOUR | STATUSICON_TEXT_KEYBIND | STATUSICON_TEXT_ALPHA_INDEPENDENT;
	m_mapShowing["scheme"].m_iIdTexture = g_rgiIdSchemeIcons[Scheme_UNASSIGNED];

	m_mapShowing["scheme"].m_vecColour = Vector(1, 1, 1);
	m_mapShowing["scheme"].m_vecOscillationColorDelta = VEC_REDISH - Vector(1, 1, 1);
	m_mapShowing["scheme"].m_flColorOscillationPeriod = 2;
	m_mapShowing["scheme"].m_flAlpha = MIN_ALPHA;
	m_mapShowing["scheme"].m_flOscillationAlphaDelta = 255 - MIN_ALPHA;
	m_mapShowing["scheme"].m_flAlphaOscillationPeriod = 2;

	m_mapShowing["scheme"].m_wcsText = L"[ " + std::wstring(ANSIToUnicode(gExtFuncs.pfnKey_NameForBinding("votescheme"))) + L" ]";
	gFontFuncs::GetTextSize(m_hFont, m_mapShowing["scheme"].m_wcsText.c_str(), &m_mapShowing["scheme"].m_iTextLength, nullptr);
	m_mapShowing["scheme"].m_vecTextColour = Vector(1, 1, 1);
	m_mapShowing["scheme"].m_flTextAlpha = 255;
}
