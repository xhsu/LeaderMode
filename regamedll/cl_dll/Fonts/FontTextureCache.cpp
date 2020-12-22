#include "FontTextureCache.h"
#include "Win32Font.h"
#include "FontManager.h"

#include <gl/gl.h>

#pragma comment(lib, "opengl32.lib")

static CFontTextureCache s_FontTextureCache;

CFontTextureCache &FontTextureCache(void)
{
	return s_FontTextureCache;
}

template <class T>
inline T clamp(T const &val, T const &minVal, T const &maxVal)
{
	if (val < minVal)
		return minVal;
	else if (val > maxVal)
		return maxVal;
	else
		return val;
}

int CFontTextureCache::s_pFontPageSize[FONT_PAGE_SIZE_COUNT] =
{
	16,
	32,
	64,
	128,
};

CFontTextureCache::CFontTextureCache()
{
	CacheEntry_t listHead = { 0, 0 };
	m_CharCache.emplace_back(listHead);

	for (int i = 0; i < FONT_PAGE_SIZE_COUNT; ++i)
	{
		m_pCurrPage[i] = -1;
	}
}

CFontTextureCache::~CFontTextureCache()
{

}

bool CFontTextureCache::GetTextureForChar(int font, int type, wchar_t wch, int *textureID, float **texCoords)
{
	static CacheEntry_t cacheitem;

	if (type == FONT_DRAW_DEFAULT)
	{
		type = FONT_DRAW_NONADDITIVE;
	}

	int typePage = (int)type - 1;
	typePage = clamp(typePage, 0, (int)FONT_DRAW_TYPE_COUNT - 1);


	cacheitem.font = font;
	cacheitem.wch = wch;

	*texCoords = cacheitem.texCoords;

	auto it = m_CharCache.begin();
	// Iterate through the vector
	while (it != m_CharCache.end())
	{
		// Check if value of this entry matches with given value
		if (it->font == cacheitem.font && it->wch == cacheitem.wch)
		{
			// we found it, let's leave.
			int page = it->page;
			*textureID = m_PageList[page].textureID[typePage];
			*texCoords = it->texCoords;
			return true;
		}

		// Go to next entry in vector
		it++;
	}

	CWin32Font *winFont = FontManager().GetFontForChar(font, wch);
	if (!winFont)
		return false;

	int fontTall = winFont->GetHeight();
	int a, b, c;
	winFont->GetCharABCWidths(wch, a, b, c);
	int fontWide = b;

	int page, drawX, drawY, twide, ttall;
	if (!AllocatePageForChar(fontWide, fontTall, page, drawX, drawY, twide, ttall))
		return false;

	int nByteCount = s_pFontPageSize[FONT_PAGE_SIZE_COUNT - 1] * s_pFontPageSize[FONT_PAGE_SIZE_COUNT - 1] * 4;
	unsigned char* rgba = (unsigned char*)_malloca(nByteCount);
	memset(rgba, 0, nByteCount);
	winFont->GetCharRGBA(wch, 0, 0, fontWide, fontTall, rgba);

	glBindTexture(GL_TEXTURE_2D, m_PageList[page].textureID[typePage]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, drawX, drawY, fontWide, fontTall, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
	_freea(rgba);

	cacheitem.page = page;

	double adjust = 0.0f;

	cacheitem.texCoords[0] = (float)((double)drawX / ((double)twide + adjust));
	cacheitem.texCoords[1] = (float)((double)drawY / ((double)ttall + adjust));
	cacheitem.texCoords[2] = (float)((double)(drawX + fontWide) / (double)twide);
	cacheitem.texCoords[3] = (float)((double)(drawY + fontTall) / (double)ttall);
	m_CharCache.emplace_back(cacheitem);

	*textureID = m_PageList[page].textureID[typePage];
	return true;
}

int CFontTextureCache::ComputePageType(int charTall) const
{
	for (int i = 0; i < FONT_PAGE_SIZE_COUNT; ++i)
	{
		if (charTall < s_pFontPageSize[i])
			return i;
	}

	return -1;
}

bool CFontTextureCache::AllocatePageForChar(int charWide, int charTall, int &pageIndex, int &drawX, int &drawY, int &twide, int &ttall)
{
	int nPageType = ComputePageType(charTall);
	pageIndex = m_pCurrPage[nPageType];

	int nNextX = 0;
	bool bNeedsNewPage = true;
	if (pageIndex > -1)
	{
		nNextX = m_PageList[pageIndex].nextX + charWide;

		if (nNextX > m_PageList[pageIndex].wide)
		{
			m_PageList[pageIndex].nextX = 0;
			nNextX = charWide;
			m_PageList[pageIndex].nextY += m_PageList[pageIndex].fontHeight + 1;
		}

		bNeedsNewPage = ((m_PageList[pageIndex].nextY + m_PageList[pageIndex].fontHeight + 1) > m_PageList[pageIndex].tall);
	}

	if (bNeedsNewPage)
	{
		// the new index must be the last.
		pageIndex = m_PageList.size();

		Page_t newPage;
		m_pCurrPage[nPageType] = pageIndex;

		for (int i = 0; i < FONT_DRAW_TYPE_COUNT; ++i)
		{
			//newPage.textureID[i] = g_pSurface->CreateNewTextureID();
			glGenTextures(1, &newPage.textureID[i]);
		}

		newPage.fontHeight = s_pFontPageSize[nPageType];
		newPage.wide = 256;
		newPage.tall = 256;
		newPage.nextX = 0;
		newPage.nextY = 0;

		nNextX = charWide;

		//unsigned char rgba[256 * 256 * 4];
		constexpr size_t RGBA_SIZE = 256U * 256U * 4U;
		unsigned char* rgba = (unsigned char*)malloc(RGBA_SIZE);
		memset(rgba, 0, RGBA_SIZE);
		int typePageNonAdditive = (int)(FONT_DRAW_NONADDITIVE) - 1;

		glBindTexture(GL_TEXTURE_2D, newPage.textureID[typePageNonAdditive]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newPage.wide, newPage.tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// place newPage into vector.
		m_PageList.emplace_back(newPage);

		free(rgba);
	}

	drawX = m_PageList[pageIndex].nextX;
	drawY = m_PageList[pageIndex].nextY;
	twide = m_PageList[pageIndex].wide;
	ttall = m_PageList[pageIndex].tall;

	m_PageList[pageIndex].nextX = nNextX + 1;

	return true;
}
