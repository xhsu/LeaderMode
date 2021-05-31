#pragma once

#ifndef FONTTEXTURECACHE_H
#define FONTTEXTURECACHE_H

#include "FontManager.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

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
		int font{ 0 };
		wchar_t wch{ 0 };
	};

	struct CacheContent_t
	{
		unsigned char page{ 0U };
		float texCoords[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
	};

	struct Page_t
	{
		// FIXME: used to be a short, change it for glGenTexture.
		unsigned int textureID[FONT_DRAW_TYPE_COUNT];
		short fontHeight;
		short wide, tall;
		short nextX, nextY;
	};

	struct CacheEntryHashFunc	// HACKHACK: assume the font and wchar_t won't bypass unsigned short.
	{
		unsigned long operator()(CFontTextureCache::CacheEntry_t const& lhs) const
		{
			unsigned short a = (unsigned short)std::clamp<int>(lhs.font, 0, 65535);
			unsigned short b = *(unsigned short*)&lhs.wch;

			return (a << 16) + b;
		}
	};

	struct CacheEntryKeyEqual
	{
		bool operator()(const CacheEntry_t& lhs, const CacheEntry_t& rhs) const
		{
			return (lhs.font == rhs.font && lhs.wch == rhs.wch);
		}
	};

private:
	bool AllocatePageForChar(int charWide, int charTall, int &pageIndex, int &drawX, int &drawY, int &twide, int &ttall);
	int ComputePageType(int charTall) const;

	typedef std::vector<Page_t> FontPageList_t;

	std::unordered_map<
		CacheEntry_t,
		CacheContent_t,
		CacheEntryHashFunc,
		CacheEntryKeyEqual
	> m_CharCache;

	FontPageList_t m_PageList;
	int m_pCurrPage[FONT_PAGE_SIZE_COUNT];

	static int s_pFontPageSize[FONT_PAGE_SIZE_COUNT];
};

CFontTextureCache &FontTextureCache(void);

#endif
