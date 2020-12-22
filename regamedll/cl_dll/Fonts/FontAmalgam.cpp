#include "FontAmalgam.h"

CFontAmalgam::CFontAmalgam()
{
	m_Fonts.resize(4);
	m_iMaxHeight = 0;
	m_iMaxWidth = 0;
	memset(m_szName, NULL, sizeof(m_szName));
}

CFontAmalgam::~CFontAmalgam()
{

}

const char *CFontAmalgam::Name()
{
	return m_szName;
}

void CFontAmalgam::SetName(const char *name)
{
	strncpy(m_szName, name, sizeof(m_szName)-1);
	m_szName[sizeof(m_szName)-1] = 0;
}

void CFontAmalgam::AddFont(CWin32Font *font, int lowRange, int highRange)
{
	TFontRange newFont;
	memset(&newFont, NULL, sizeof(newFont));

	newFont.font = font;
	newFont.lowRange = lowRange;
	newFont.highRange = highRange;

	m_Fonts.emplace_back(newFont);

	m_iMaxHeight = max(font->GetHeight(), m_iMaxHeight);
	m_iMaxWidth = max(font->GetMaxCharWidth(), m_iMaxWidth);
}

CWin32Font *CFontAmalgam::GetFontForChar(int ch)
{
	for (unsigned i = 0; i < m_Fonts.size(); i++)
	{
		if (ch >= m_Fonts[i].lowRange && ch <= m_Fonts[i].highRange)
		{
			return m_Fonts[i].font;
		}
	}

	return NULL;
}

int CFontAmalgam::GetFontHeight()
{
	return m_iMaxHeight;
}

int CFontAmalgam::GetFontMaxWidth()
{
	return m_iMaxWidth;
}

int CFontAmalgam::GetFontLowRange(int i)
{
	return m_Fonts[i].lowRange;
}

int CFontAmalgam::GetFontHighRange(int i)
{
	return m_Fonts[i].highRange;
}

const char *CFontAmalgam::GetFontName(int i)
{
	if (m_Fonts[i].font)
	{
		return m_Fonts[i].font->GetName();
	}
	else
		return NULL;
}

int CFontAmalgam::GetFlags(int i)
{
	if (m_Fonts.size() && m_Fonts[i].font)
	{
		return m_Fonts[i].font->GetFlags();
	}
	else
	{
		return 0;
	}
}

int CFontAmalgam::GetCount()
{
	return m_Fonts.size();
}
