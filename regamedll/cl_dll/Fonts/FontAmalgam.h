#pragma once

#ifndef FONTAMALGAM_H
#define FONTAMALGAM_H

#include "Win32Font.h"
#include <vector>

class CFontAmalgam
{
public:
	CFontAmalgam();
	~CFontAmalgam();

	const char *Name();
	void SetName(const char *name);
	void AddFont(CWin32Font *font, int lowRange, int highRange);
	CWin32Font *GetFontForChar(int ch);
	int GetFontHeight();
	int GetFontMaxWidth();
	int GetFontLowRange(int i);
	int GetFontHighRange(int i);
	int GetFlags(int i);
	const char *GetFontName(int i);
	int GetCount();

private:
	struct TFontRange
	{
		int lowRange;
		int highRange;
		CWin32Font *font;
	};

	std::vector<TFontRange> m_Fonts;
	char m_szName[32];
	int m_iMaxWidth;
	int m_iMaxHeight;
};

#endif
