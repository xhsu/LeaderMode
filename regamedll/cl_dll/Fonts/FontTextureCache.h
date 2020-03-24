#pragma once

#ifndef FONTTEXTURECACHE_H
#define FONTTEXTURECACHE_H

#include "FontManager.h"
#include <vector>
#include <map>

enum FontDrawType
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

class CFontTextureCache
{
public:
	CFontTextureCache();
	~CFontTextureCache();

	bool GetTextureForChar(int font, int type, wchar_t wch, int *textureID, float **texCoords);

public:
	enum
	{
		FONT_PAGE_SIZE_16,
		FONT_PAGE_SIZE_32,
		FONT_PAGE_SIZE_64,
		FONT_PAGE_SIZE_128,

		FONT_PAGE_SIZE_COUNT,
	};

	struct CacheEntry_t
	{
		int font;
		wchar_t wch;
		unsigned char page;
		float texCoords[4];
	};

	struct Page_t
	{
		// FIXME: used to be a short, change it for glGenTexture.
		unsigned int textureID[FONT_DRAW_TYPE_COUNT];
		short fontHeight;
		short wide, tall;
		short nextX, nextY;
	};

	struct CacheEntryLessFunc
	{
		bool operator()(CFontTextureCache::CacheEntry_t const& lhs, CFontTextureCache::CacheEntry_t const& rhs) const
		{
			if (lhs.font < rhs.font)
				return true;
			else if (lhs.font > rhs.font)
				return false;

			return (lhs.wch < rhs.wch);
		}
	};

private:
	bool AllocatePageForChar(int charWide, int charTall, int &pageIndex, int &drawX, int &drawY, int &twide, int &ttall);
	int ComputePageType(int charTall) const;

	typedef std::vector<Page_t> FontPageList_t;

	//std::map<HCacheEntry, CacheEntry_t, CacheEntryLessFunc> m_CharCache;
	std::vector<CacheEntry_t> m_CharCache;
	FontPageList_t m_PageList;
	int m_pCurrPage[FONT_PAGE_SIZE_COUNT];
	//HCacheEntry m_LRUListHeadIndex;

	static int s_pFontPageSize[FONT_PAGE_SIZE_COUNT];
};

CFontTextureCache &FontTextureCache(void);

#endif
