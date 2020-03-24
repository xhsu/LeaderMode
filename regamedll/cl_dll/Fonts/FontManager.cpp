#include "FontManager.h"

static CFontManager s_FontManager;

CFontManager &FontManager(void)
{
	return s_FontManager;
}

CFontManager::CFontManager()
{
	m_FontAmalgams.reserve(sizeof(CFontAmalgam) * 100);
	m_Win32Fonts.reserve(sizeof(CWin32Font*) * 100);
}

CFontManager::~CFontManager()
{
	ClearAllFonts();
}

void CFontManager::ClearAllFonts()
{
	for (unsigned i = 0; i < m_Win32Fonts.size(); i++)
	{
		delete m_Win32Fonts[i];
	}

	m_Win32Fonts.clear();
}

int CFontManager::CreateFont()
{
	int i = m_FontAmalgams.size();
	m_FontAmalgams.emplace_back();

	if (!i)
	{
		i++;	// make sure i is not 0
		m_FontAmalgams.emplace_back();
	}

	return i;
}

bool CFontManager::AddGlyphSetToFont(int font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange)
{
	CWin32Font *winFont = NULL;
	for (unsigned i = 0; i < m_Win32Fonts.size(); i++)
	{
		if (m_Win32Fonts[i]->IsEqualTo(windowsFontName, tall, weight, blur, scanlines, flags))
		{
			winFont = m_Win32Fonts[i];
			break;
		}
	}

	if (!winFont)
	{
		winFont = new CWin32Font();
		if (!winFont->Create(windowsFontName, tall, weight, blur, scanlines, flags))
		{
			delete winFont;
			return false;
		}

		// successfully created? add to lib.
		m_Win32Fonts.emplace_back(winFont);
	}

	m_FontAmalgams[font].AddFont(winFont, lowRange, highRange);
	return true;
}

int CFontManager::GetFontByName(const char *name)
{
	for (unsigned i = 1; i < m_FontAmalgams.size(); i++)
	{
		if (!_stricmp(name, m_FontAmalgams[i].Name()))
		{
			return i;
		}
	}
	return 0;
}

CWin32Font *CFontManager::GetFontForChar(int font, wchar_t wch)
{
	return m_FontAmalgams[font].GetFontForChar(wch);
}

void CFontManager::GetCharABCwide(int font, int ch, int &a, int &b, int &c)
{
	CWin32Font *winFont = m_FontAmalgams[font].GetFontForChar(ch);
	if (winFont)
	{
		winFont->GetCharABCWidths(ch, a, b, c);
	}
	else
	{
		a = c = 0;
		b = m_FontAmalgams[font].GetFontMaxWidth();
	}
}

int CFontManager::GetFontTall(int font)
{
	return m_FontAmalgams[font].GetFontHeight();
}

int CFontManager::GetFontAscent(int font, wchar_t wch)
{
	CWin32Font *winFont = m_FontAmalgams[font].GetFontForChar(wch);

	if (winFont)
		return winFont->GetAscent();
	else
		return 0;
}

bool CFontManager::IsFontAdditive(int font)
{
	return (m_FontAmalgams[font].GetFlags(0) & FONTFLAG_ADDITIVE) ? true : false;
}

int CFontManager::GetCharacterWidth(int font, int ch)
{
	if (iswprint(ch))
	{
		int a, b, c;
		GetCharABCwide(font, ch, a, b, c);
		return (a + b + c);
	}
	return 0;
}

void CFontManager::GetTextSize(int font, const wchar_t *text, int &wide, int &tall)
{
	wide = 0;
	tall = 0;

	if (!text)
		return;

	tall = GetFontTall(font);

	int xx = 0;
	for (int i = 0;; i++)
	{
		wchar_t ch = text[i];
		if (ch == 0)
		{
			break;
		}
		else if (ch == '\n')
		{
			tall += GetFontTall(font);
			xx = 0;
		}
		else if (ch == '&')
		{

		}
		else
		{
			xx += GetCharacterWidth(font, ch);
			if (xx > wide)
			{
				wide = xx;
			}
		}
	}
}

int CFontManager::GetFontFlags(int font)
{
	return m_FontAmalgams[font].GetFlags(0);
}
