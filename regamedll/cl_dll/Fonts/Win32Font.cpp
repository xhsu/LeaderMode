#include "Win32Font.h"
#include "FontManager.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#pragma warning(disable:4244)

CWin32Font::CWin32Font()
{
	m_szName[0] = 0;
	m_iTall = 0;
	m_iWeight = 0;
	m_iFlags = 0;
	m_iMaxCharWidth = 0;
	m_hFont = NULL;
	m_hDC = NULL;
	m_hDIB = NULL;
	m_bAntiAliased = false;
	m_iBlur = 0;
	m_pGaussianDistribution = NULL;
	m_iScanLines = 0;
	m_bRotary = false;
	m_bAdditive = false;
}

CWin32Font::~CWin32Font()
{
	if (m_hFont)
		::DeleteObject(m_hFont);

	if (m_hDC)
		::DeleteDC(m_hDC);

	if (m_hDIB)
		::DeleteObject(m_hDIB);
}

extern bool g_bFontFound = false;

int CALLBACK FontEnumProc(const LOGFONT *lpelfe,
	const TEXTMETRIC *lpntme,
	DWORD FontType,
	LPARAM lParam
	)
{
	g_bFontFound = true;
	return 0;
}

bool CWin32Font::Create(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
{
	strncpy(m_szName, windowsFontName, sizeof(m_szName)-1);
	m_szName[sizeof(m_szName)-1] = 0;
	m_iTall = tall;
	m_iWeight = weight;
	m_iFlags = flags;
	m_bAntiAliased = (flags & FONTFLAG_ANTIALIAS) ? 1 : 0;
	m_iDropShadowOffset = (flags & FONTFLAG_DROPSHADOW) ? 1 : 0;
	m_iOutlineSize = (flags & FONTFLAG_OUTLINE) ? 1 : 0;
	m_iBlur = blur;
	m_iScanLines = scanlines;
	m_bRotary = (flags & FONTFLAG_ROTARY) ? 1 : 0;
	m_bAdditive = (flags & FONTFLAG_ADDITIVE) ? 1 : 0;

	int charset = (flags & FONTFLAG_SYMBOL) ? SYMBOL_CHARSET : ANSI_CHARSET;

	m_hDC = ::CreateCompatibleDC(NULL);

	LOGFONT logfont;
	logfont.lfCharSet = DEFAULT_CHARSET;
	logfont.lfPitchAndFamily = 0;
	strcpy(logfont.lfFaceName, windowsFontName);
	g_bFontFound = false;
	::EnumFontFamiliesExA(m_hDC, &logfont, &FontEnumProc, 0, 0);
	if (!g_bFontFound)
	{
		m_szName[0] = 0;
		return false;
	}

	m_hFont = ::CreateFontA(tall, 0, 0, 0,
		m_iWeight,
		flags & FONTFLAG_ITALIC,
		flags & FONTFLAG_UNDERLINE,
		flags & FONTFLAG_STRIKEOUT,
		charset,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		m_bAntiAliased ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		m_szName);

	if (!m_hFont)
	{
		m_szName[0] = 0;
		return false;
	}

	::SelectObject(m_hDC, m_hFont);
	::SetTextAlign(m_hDC, TA_LEFT | TA_TOP | TA_UPDATECP);

	::TEXTMETRIC tm;
	memset(&tm, 0, sizeof(tm));

	if (!GetTextMetricsA(m_hDC, &tm))
	{
		m_szName[0] = 0;
		return false;
	}

	m_iHeight = tm.tmHeight + m_iDropShadowOffset + 2 * m_iOutlineSize;
	m_iMaxCharWidth = tm.tmMaxCharWidth;
	m_iAscent = tm.tmAscent;

	m_rgiBitmapSize[0] = tm.tmMaxCharWidth + m_iOutlineSize * 2;
	m_rgiBitmapSize[1] = tm.tmHeight + m_iDropShadowOffset + m_iOutlineSize * 2;

	::BITMAPINFOHEADER header;
	memset(&header, 0, sizeof(header));
	header.biSize = sizeof(header);
	header.biWidth = m_rgiBitmapSize[0];
	header.biHeight = -m_rgiBitmapSize[1];
	header.biPlanes = 1;
	header.biBitCount = 32;
	header.biCompression = BI_RGB;

	m_hDIB = ::CreateDIBSection(m_hDC, (BITMAPINFO*)&header, DIB_RGB_COLORS, (void**)(&m_pBuf), NULL, 0);
	::SelectObject(m_hDC, m_hDIB);

	memset(m_ABCWidthsCache, 0, sizeof(m_ABCWidthsCache));
	ABC abc[ABCWIDTHS_CACHE_SIZE];
	if (::GetCharABCWidthsW(m_hDC, 0, ABCWIDTHS_CACHE_SIZE - 1, &abc[0]) || ::GetCharABCWidthsA(m_hDC, 0, ABCWIDTHS_CACHE_SIZE - 1, &abc[0]))
	{
		for (int i = 0; i < ABCWIDTHS_CACHE_SIZE; i++)
		{
			m_ABCWidthsCache[i].a = abc[i].abcA - m_iBlur - m_iOutlineSize;
			m_ABCWidthsCache[i].b = abc[i].abcB + ((m_iBlur + m_iOutlineSize) * 2) + m_iDropShadowOffset;
			m_ABCWidthsCache[i].c = abc[i].abcC - m_iBlur - m_iDropShadowOffset - m_iOutlineSize;
		}
	}
	else
	{
		for (int i = 0; i < ABCWIDTHS_CACHE_SIZE; i++)
		{
			m_ABCWidthsCache[i].b = (char)tm.tmAveCharWidth;
		}
	}

	if (m_iBlur > 1)
	{
		m_pGaussianDistribution = new float[m_iBlur * 2 + 1];
		double sigma = 0.683 * m_iBlur;
		for (int x = 0; x <= (m_iBlur * 2); x++)
		{
			int val = x - m_iBlur;
			m_pGaussianDistribution[x] = (float)(1.0f / sqrt(2 * 3.14 * sigma * sigma)) * pow(2.7, -1 * (val * val) / (2 * sigma * sigma));

			m_pGaussianDistribution[x] *= 1;
		}
	}

	return true;
}

void CWin32Font::GetCharRGBA(int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba)
{
	int a, b, c;
	GetCharABCWidths(ch, a, b, c);

	::SelectObject(m_hDC, m_hFont);

	int wide = b;
	int tall = m_iHeight;
	GLYPHMETRICS glyphMetrics;
	MAT2 mat2 = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
	int bytesNeeded = 0;

	if (m_bAntiAliased)
	{
		::SelectObject(m_hDC, m_hFont);
		bytesNeeded = ::GetGlyphOutlineW(m_hDC, ch, GGO_GRAY8_BITMAP, &glyphMetrics, 0, NULL, &mat2);
	}

	if (bytesNeeded > 0)
	{
		unsigned char *lpbuf = (unsigned char *)_malloca(bytesNeeded);
		::GetGlyphOutlineW(m_hDC, ch, GGO_GRAY8_BITMAP, &glyphMetrics, bytesNeeded, lpbuf, &mat2);

		wide = glyphMetrics.gmBlackBoxX;
		while (wide % 4 != 0)
		{
			wide++;
		}

		int pushDown = m_iAscent - glyphMetrics.gmptGlyphOrigin.y;
		int xstart = 0;

		if ((int)glyphMetrics.gmBlackBoxX >= b + 2)
		{
			xstart = (glyphMetrics.gmBlackBoxX - b) / 2;
		}

		for (unsigned int j = 0; j < glyphMetrics.gmBlackBoxY; j++)
		{
			for (unsigned int i = xstart; i < glyphMetrics.gmBlackBoxX; i++)
			{
				int x = rgbaX + i - xstart + m_iBlur + m_iOutlineSize;
				int y = rgbaY + j + pushDown;
				if ((x < rgbaWide) && (y < rgbaTall))
				{
					unsigned char grayscale = lpbuf[(j*wide + i)];

					float r, g, b, a;
					if (grayscale)
					{
						r = g = b = 1.0f;
						a = (grayscale + 0) / 64.0f;
						if (a > 1.0f) a = 1.0f;
					}
					else
					{
						r = g = b = a = 0.0f;
					}

					if (ch == '\t')
					{
						r = g = b = 0;
					}

					unsigned char *dst = &rgba[(y*rgbaWide + x) * 4];
					dst[0] = (unsigned char)(r * 255.0f);
					dst[1] = (unsigned char)(g * 255.0f);
					dst[2] = (unsigned char)(b * 255.0f);
					dst[3] = (unsigned char)(a * 255.0f);
				}
			}
		}

		_freea(lpbuf);
	}
	else
	{
		::SetBkColor(m_hDC, RGB(0, 0, 0));
		::SetTextColor(m_hDC, RGB(255, 255, 255));
		::SetBkMode(m_hDC, OPAQUE);
		::MoveToEx(m_hDC, -a, 0, NULL);

		wchar_t wch = (wchar_t)ch;
		::ExtTextOutW(m_hDC, 0, 0, 0, NULL, &wch, 1, NULL);
		::SetBkMode(m_hDC, TRANSPARENT);

		if (wide > m_rgiBitmapSize[0])
		{
			wide = m_rgiBitmapSize[0];
		}
		if (tall > m_rgiBitmapSize[1])
		{
			tall = m_rgiBitmapSize[1];
		}

		for (int j = 0; j < tall; j++)
		{
			for (int i = 0; i < wide - m_iDropShadowOffset; i++)
			{
				int x = rgbaX + i;
				int y = rgbaY + j;
				if ((x < rgbaWide) && (y < rgbaTall))
				{
					unsigned char *src = &m_pBuf[(j*m_rgiBitmapSize[0] + i) * 4];

					float r = (src[0]) / 255.0f;
					float g = (src[1]) / 255.0f;
					float b = (src[2]) / 255.0f;

					if (ch == '\t')
					{
						r = g = b = 0;
					}

					unsigned char *dst = &rgba[(y*rgbaWide + x) * 4];
					dst[0] = (unsigned char)(r * 255.0f);
					dst[1] = (unsigned char)(g * 255.0f);
					dst[2] = (unsigned char)(b * 255.0f);
					dst[3] = (unsigned char)((r * 0.34f + g * 0.55f + b * 0.11f) * 255.0f);
				}
			}
		}
	}

	if (m_iDropShadowOffset)
	{
		ApplyDropShadowToTexture(rgbaX, rgbaY, rgbaWide, rgbaTall, wide, tall, rgba);
	}
	if (m_iOutlineSize)
	{
		ApplyOutlineToTexture(rgbaX, rgbaY, rgbaWide, rgbaTall, wide, tall, rgba);
	}

	ApplyGaussianBlurToTexture(rgbaX, rgbaY, rgbaWide, rgbaTall, rgba);
	ApplyScanlineEffectToTexture(rgbaX, rgbaY, rgbaWide, rgbaTall, rgba);
	ApplyRotaryEffectToTexture(rgbaX, rgbaY, rgbaWide, rgbaTall, rgba);
}

void CWin32Font::ApplyRotaryEffectToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba)
{
	if (!m_bRotary)
		return;

	int y = rgbaTall * 0.5;

	unsigned char *line = &rgba[(rgbaX + ((y + rgbaY) * rgbaWide)) * 4];

	for (int x = 0; x < rgbaWide; x++, line += 4)
	{
		line[0] = 127;
		line[1] = 127;
		line[2] = 127;
		line[3] = 255;
	}
}

void CWin32Font::ApplyScanlineEffectToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba)
{
	if (m_iScanLines < 2)
		return;

	for (int y = 0; y < rgbaTall; y++)
	{
		if (y % m_iScanLines == 0)
			continue;

		unsigned char *line = &rgba[(rgbaX + ((y + rgbaY) * rgbaWide)) * 4];

		for (int x = 0; x < rgbaWide; x++, line += 4)
		{
			line[0] *= 0.7;
			line[1] *= 0.7;
			line[2] *= 0.7;
		}
	}
}

void CWin32Font::ApplyDropShadowToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, int charWide, int charTall, unsigned char *rgba)
{
	for (int y = charTall - 1; y >= m_iDropShadowOffset; y--)
	{
		for (int x = charWide - 1; x >= m_iDropShadowOffset; x--)
		{
			unsigned char *dest = &rgba[(x + rgbaX + ((rgbaY + y) * rgbaWide)) * 4];
			if (dest[3] == 0)
			{
				unsigned char *src = &rgba[(x + rgbaX - m_iDropShadowOffset + ((rgbaY + y - m_iDropShadowOffset) * rgbaWide)) * 4];
				dest[0] = 0;
				dest[1] = 0;
				dest[2] = 0;
				dest[3] = src[3];
			}
		}
	}
}

void CWin32Font::ApplyOutlineToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, int charWide, int charTall, unsigned char *rgba)
{
	int x, y;
	for (y = 0; y < charTall; y++)
	{
		for (x = 0; x < charWide; x++)
		{
			unsigned char *src = &rgba[(x + rgbaX + ((rgbaY + y) * rgbaWide)) * 4];
			if (src[3] == 0)
			{
				int shadowX, shadowY;
				for (shadowX = -m_iOutlineSize; shadowX <= m_iOutlineSize; shadowX++)
				{
					for (shadowY = -m_iOutlineSize; shadowY <= m_iOutlineSize; shadowY++)
					{
						if (shadowX == 0 && shadowY == 0)
						{
							continue;
						}
						int testX, testY;
						testX = shadowX + x;
						testY = shadowY + y;
						if (testX < 0 || testX >= charWide ||
							testY < 0 || testY >= charTall)
						{
							continue;
						}
						unsigned char *test = &rgba[(testX + rgbaX + ((rgbaY + testY) * rgbaWide)) * 4];
						if (test[0] != 0 && test[1] != 0 && test[2] != 0 && test[3] != 0)
						{
							src[0] = 0;
							src[1] = 0;
							src[2] = 0;
							src[3] = 255;
						}
					}
				}
			}
		}
	}
}

void CWin32Font::ApplyGaussianBlurToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba)
{
	if (!m_pGaussianDistribution)
		return;

	unsigned char *src = (unsigned char *)_malloca(rgbaWide * rgbaTall * 4);
	memcpy(src, rgba, rgbaWide * rgbaTall * 4);
	unsigned char *dest = rgba;
	for (int y = 0; y < rgbaTall; y++)
	{
		for (int x = 0; x < rgbaWide; x++)
		{
			GetBlurValueForPixel(src, m_iBlur, m_pGaussianDistribution, x, y, rgbaWide, rgbaTall, dest);

			dest += 4;
		}
	}

	_freea(src);
}

void CWin32Font::GetBlurValueForPixel(unsigned char *src, int blur, float *gaussianDistribution, int srcX, int srcY, int srcWide, int srcTall, unsigned char *dest)
{
	int r = 0, g = 0, b = 0, a = 0;

	float accum = 0.0f;

	int maxX = min(srcX + blur, srcWide);
	int minX = max(srcX - blur, 0);
	for (int x = minX; x <= maxX; x++)
	{
		int maxY = min(srcY + blur, srcTall - 1);
		int minY = max(srcY - blur, 0);
		for (int y = minY; y <= maxY; y++)
		{
			unsigned char *srcPos = src + ((x + (y * srcWide)) * 4);

			float weight = gaussianDistribution[x - srcX + blur];
			float weight2 = gaussianDistribution[y - srcY + blur];
			accum += (srcPos[0] * (weight * weight2));
		}
	}

	r = g = b = a = (int)accum;

	int pixelCount = 1;
	dest[0] = min(r / pixelCount, 255);
	dest[1] = min(g / pixelCount, 255);
	dest[2] = min(b / pixelCount, 255);
	dest[3] = min(a / pixelCount, 255);
	dest[3] = 255;
}

bool CWin32Font::IsEqualTo(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
{
	if (!_stricmp(windowsFontName, m_szName)
		&& m_iTall == tall
		&& m_iWeight == weight
		&& m_iBlur == blur
		&& m_iFlags == flags)
		return true;

	return false;
}

bool CWin32Font::IsValid()
{
	if (m_szName[0])
		return true;

	return false;
}

void CWin32Font::SetAsActiveFont(HDC hdc)
{
	::SelectObject(hdc, m_hFont);
}

void CWin32Font::GetCharABCWidths(int ch, int &a, int &b, int &c)
{
	if (ch < ABCWIDTHS_CACHE_SIZE)
	{
		a = m_ABCWidthsCache[ch].a;
		b = m_ABCWidthsCache[ch].b;
		c = m_ABCWidthsCache[ch].c;
	}
	else
	{
		ABC abc;
		if (::GetCharABCWidthsW(m_hDC, ch, ch, &abc))
		{
			a = abc.abcA;
			b = abc.abcB;
			c = abc.abcC;
		}
		else
		{
			a = c = 0;
			b = m_iMaxCharWidth;
		}
	}
}

int CWin32Font::GetHeight()
{
	return m_iHeight;
}

int CWin32Font::GetMaxCharWidth()
{
	return m_iMaxCharWidth;
}

int CWin32Font::GetFlags()
{
	return m_iFlags;
}

int CWin32Font::GetAscent(void)
{
	return m_iAscent;
}
