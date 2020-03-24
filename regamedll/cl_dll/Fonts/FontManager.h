#pragma once

#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "FontAmalgam.h"
#include <vector>

#ifdef CreateFont
#undef CreateFont
#endif

enum FontFlag
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
};

class CFontManager
{
public:
	CFontManager();
	~CFontManager();

	void ClearAllFonts();
	int CreateFont();
	bool AddGlyphSetToFont(int font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange);
	int GetFontByName(const char *name);
	void GetCharABCwide(int font, int ch, int &a, int &b, int &c);
	int GetFontTall(int font);
	int GetFontAscent(int font, wchar_t wch);
	int GetCharacterWidth(int font, int ch);
	void GetTextSize(int font, const wchar_t *text, int &wide, int &tall);
	CWin32Font *GetFontForChar(int font, wchar_t wch);
	bool IsFontAdditive(int font);
	int GetFontFlags(int font);

private:
	std::vector<CFontAmalgam> m_FontAmalgams;
	std::vector<CWin32Font *> m_Win32Fonts;
};

CFontManager &FontManager();

#endif
