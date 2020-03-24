#ifndef WIN32FONT_H
#define WIN32FONT_H

#define WIN32_LEAN_AND_MEAN
#define OEMRESOURCE
#include <windows.h>

#ifdef GetCharABCWidths
#undef GetCharABCWidths
#endif

class CWin32Font
{
public:
	CWin32Font();
	~CWin32Font();

	bool Create(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags);
	void GetCharRGBA(int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba);
	bool IsEqualTo(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags);
	bool IsValid();
	void GetCharABCWidths(int ch, int &a, int &b, int &c);
	void SetAsActiveFont(HDC hdc);
	int GetHeight();
	int GetMaxCharWidth();
	int GetFlags();
	const char *GetName() { return m_szName; }
	virtual int GetAscent(void);

private:
	void ApplyScanlineEffectToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba);
	void ApplyGaussianBlurToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba);
	void ApplyDropShadowToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, int charWide, int charTall, unsigned char *rgba);
	void ApplyOutlineToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, int charWide, int charTall, unsigned char *rgba);
	void ApplyRotaryEffectToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba);
	static inline void GetBlurValueForPixel(unsigned char *src, int blur, float *gaussianDistribution, int x, int y, int wide, int tall, unsigned char *dest);

	char m_szName[32];
	int m_iTall;
	int m_iWeight;
	int m_iFlags;
	bool m_bAntiAliased;
	bool m_bRotary;
	bool m_bAdditive;
	int m_iDropShadowOffset;
	int m_iOutlineSize;

	int m_iHeight;
	int m_iMaxCharWidth;
	int m_iAscent;

	struct abc_t
	{
		char a;
		char b;
		char c;
		char pad;
	};
	enum { ABCWIDTHS_CACHE_SIZE = 256 };
	abc_t m_ABCWidthsCache[ABCWIDTHS_CACHE_SIZE];

	HFONT m_hFont;
	HDC m_hDC;
	HBITMAP m_hDIB;
	int m_rgiBitmapSize[2];
	unsigned char *m_pBuf;

	int m_iScanLines;
	int m_iBlur;
	float *m_pGaussianDistribution;
};


#endif