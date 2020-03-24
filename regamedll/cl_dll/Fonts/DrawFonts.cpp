#include "precompiled.h"
#include "FontTextureCache.h"

#pragma comment(lib, "opengl32.lib")

void F_Init(void);
void F_Shutdown(void);
void F_DrawSetTextFont(int font);
void F_DrawSetTextColor(int r, int g, int b, int a);
void F_DrawSetTextPos(int x, int y);
void F_DrawGetTextPos(int *x, int *y);
void F_DrawSetText3DPos(Vector vecSrc);
void F_DrawGetText3DPos(Vector *vecSrc);
void F_DrawSetText3DDir(Vector vecForward, Vector vecRight, Vector vecUp);
void F_DrawSetText3DHeight(float flHeight);
void F_DrawPrintText(const wchar_t *text);
void F_DrawPrint3DText(const wchar_t *text);
void F_DrawPrint3DTextVertical(const wchar_t *text);
void F_DrawOutlineText(const wchar_t *text);
void F_DrawUnicodeChar(wchar_t wch);
void F_Draw3DUnicodeChar(wchar_t wch);
void F_Draw3DUnicodeCharVertical(wchar_t wch);
int F_CreateFont(void);
bool F_AddGlyphSetToFont(int font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange);
bool F_AddCustomFontFile(const char *fontFileName);
int F_GetFontTall(int font);
void F_GetCharABCwide(int font, int ch, int *a, int *b, int *c);
int F_GetCharacterWidth(int font, int ch);
void F_GetTextSize(int font, const wchar_t *text, int *wide, int *tall);
int F_GetFontAscent(int font, wchar_t wch);
int F_GetFontFlags(int font);

static int s_Font;
static int s_Color[4];
static int s_Pos[2];
static Vector s_vec3DPos;
static Vector s_vec3DDir[3];
static float s_flRatio;

void F_Init(void)
{
	s_Font = 0;
}

void F_Shutdown(void)
{
	return FontManager().ClearAllFonts();
}

void F_DrawSetTextFont(int font)
{
	s_Font = font;
}

void F_DrawSetTextColor(int r, int g, int b, int a)
{
	s_Color[0] = r;
	s_Color[1] = g;
	s_Color[2] = b;
	s_Color[3] = a;
}

void F_DrawSetTextPos(int x, int y)
{
	s_Pos[0] = x;
	s_Pos[1] = y;
}

void F_DrawSetText3DPos(Vector vecSrc)
{
	s_vec3DPos = vecSrc;
}

void F_DrawGetText3DPos(Vector *vecSrc)
{
	*vecSrc = s_vec3DPos;
}

void F_DrawSetText3DDir(Vector vecForward, Vector vecRight, Vector vecUp)
{
	s_vec3DDir[0] = vecForward;
	s_vec3DDir[1] = vecRight;
	s_vec3DDir[2] = vecUp;
}

void F_DrawSetText3DHeight(float flHeight)
{
	if (flHeight <= 0.0f)
		s_flRatio = 1.0f;
	else
		s_flRatio = flHeight / (float)F_GetFontTall(s_Font);
}

void F_DrawGetTextPos(int *x, int *y)
{
	*x = s_Pos[0];
	*y = s_Pos[1];
}

void F_DrawPrintText(const wchar_t *text)
{
	int x, y;
	F_DrawGetTextPos(&x, &y);

	int fontTall = F_GetFontTall(s_Font);

	for (size_t i = 0; i < wcslen(text); ++i)
	{
		if (text[i] == '\n')
		{
			y = y + fontTall;
			F_DrawSetTextPos(x, y);
			continue;
		}

		if (text[i] == '\t')
			continue;

		F_DrawUnicodeChar(text[i]);
	}
}

void F_DrawPrint3DText(const wchar_t *text)
{
	Vector vecSrc;
	F_DrawGetText3DPos(&vecSrc);

	float fontTall = (float)F_GetFontTall(s_Font);

	for (size_t i = 0; i < wcslen(text); ++i)
	{
		if (text[i] == '\n')
		{
			vecSrc -= s_vec3DDir[2] * fontTall;
			F_DrawSetText3DPos(vecSrc);
			continue;
		}

		if (text[i] == '\t')
			continue;

		F_Draw3DUnicodeChar(text[i]);
	}
}

void F_DrawPrint3DTextVertical(const wchar_t *text)
{
	Vector vecSrc;
	F_DrawGetText3DPos(&vecSrc);

	float fontTall = (float)F_GetFontTall(s_Font);

	for (size_t i = 0; i < wcslen(text); ++i)
	{
		if (text[i] == '\n')
		{
			vecSrc -= s_vec3DDir[1] * fontTall;
			F_DrawSetText3DPos(vecSrc);
			continue;
		}

		if (text[i] == '\t')
			continue;

		F_Draw3DUnicodeCharVertical(text[i]);
	}
}

void F_DrawOutlineText(const wchar_t *text)
{
	static int pos[2], color[4];

	// save pos and color
	memcpy(pos, s_Pos, sizeof(s_Pos));
	memcpy(color, s_Color, sizeof(s_Color));

	// draw outline color
	F_DrawSetTextColor(0, 0, 0, color[3]);

	// left
	F_DrawSetTextPos(pos[0] - 1, pos[1]);
	F_DrawPrintText(text);

	// right
	F_DrawSetTextPos(pos[0] + 1, pos[1]);
	F_DrawPrintText(text);

	// top
	F_DrawSetTextPos(pos[0], pos[1] - 1);
	F_DrawPrintText(text);

	// bottom
	F_DrawSetTextPos(pos[0], pos[1] + 1);
	F_DrawPrintText(text);
	
	// draw text
	F_DrawSetTextColor(color[0], color[1], color[2], color[3]);
	F_DrawSetTextPos(pos[0], pos[1]);
	F_DrawPrintText(text);
}

void F_DrawUnicodeChar(wchar_t wch)
{
	if (!s_Font)
		return;

	int x, y;
	x = s_Pos[0];
	y = s_Pos[1];

	int a, b, c;
	F_GetCharABCwide(s_Font, wch, &a, &b, &c);

	int rgbaWide, rgbaTall;

	rgbaWide = b;
	rgbaTall = F_GetFontTall(s_Font);

	x  = x + a;

	int textureID;
	float *texCoords = 0;

	if (!FontTextureCache().GetTextureForChar(s_Font, FONT_DRAW_DEFAULT, wch, &textureID, &texCoords))
		return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4ub(s_Color[0], s_Color[1], s_Color[2], s_Color[3]);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glBegin(GL_QUADS);
	glTexCoord2f(texCoords[0], texCoords[1]);
	glVertex3f((float)x, (float)y, 0.0f);
	glTexCoord2f(texCoords[2], texCoords[1]);
	glVertex3f(float(x + rgbaWide), (float)y, 0.0f);
	glTexCoord2f(texCoords[2], texCoords[3]);
	glVertex3f(float(x + rgbaWide), float(y + rgbaTall), 0.0f);
	glTexCoord2f(texCoords[0], texCoords[3]);
	glVertex3f((float)x, float(y + rgbaTall), 0.0f);
	glEnd();

	glDisable(GL_BLEND);

	s_Pos[0] = x + b + c;
	s_Pos[1] = y;
}

void F_Draw3DUnicodeChar(wchar_t wch)
{
	if (!s_Font)
		return;

	Vector vecSrc = s_vec3DPos;

	int a, b, c;
	F_GetCharABCwide(s_Font, wch, &a, &b, &c);

	int rgbaWide, rgbaTall;

	rgbaWide = b;
	rgbaTall = F_GetFontTall(s_Font);

	// don't know why, but this line is useless in 3D drawing
	//vecSrc += s_vec3DDir[1] * (float)a;

	int textureID;
	float *texCoords = 0;

	if (!FontTextureCache().GetTextureForChar(s_Font, FONT_DRAW_DEFAULT, wch, &textureID, &texCoords))
		return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4ub(s_Color[0], s_Color[1], s_Color[2], s_Color[3]);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glBegin(GL_QUADS);
	glTexCoord2f(texCoords[0], texCoords[1]);
	glVertex3f(vecSrc.x, vecSrc.y, vecSrc.z);
	vecSrc += s_vec3DDir[1] * rgbaWide * s_flRatio;
	glTexCoord2f(texCoords[2], texCoords[1]);
	glVertex3f(vecSrc.x, vecSrc.y, vecSrc.z);
	vecSrc -= s_vec3DDir[2] * rgbaTall * s_flRatio;
	glTexCoord2f(texCoords[2], texCoords[3]);
	glVertex3f(vecSrc.x, vecSrc.y, vecSrc.z);
	vecSrc -= s_vec3DDir[1] * rgbaWide * s_flRatio;
	glTexCoord2f(texCoords[0], texCoords[3]);
	glVertex3f(vecSrc.x, vecSrc.y, vecSrc.z);
	glEnd();

	glDisable(GL_BLEND);

	s_vec3DPos += float(b + c) * s_flRatio * s_vec3DDir[1];
}

void F_Draw3DUnicodeCharVertical(wchar_t wch)
{
	if (!s_Font)
		return;

	Vector vecSrc = s_vec3DPos;

	int a, b, c;
	F_GetCharABCwide(s_Font, wch, &a, &b, &c);

	int rgbaWide, rgbaTall;

	rgbaWide = b;
	rgbaTall = F_GetFontTall(s_Font);

	// don't know why, but this line is useless in 3D drawing
	//vecSrc += s_vec3DDir[1] * (float)a;

	int textureID;
	float *texCoords = 0;

	if (!FontTextureCache().GetTextureForChar(s_Font, FONT_DRAW_DEFAULT, wch, &textureID, &texCoords))
		return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4ub(s_Color[0], s_Color[1], s_Color[2], s_Color[3]);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glBegin(GL_QUADS);
	glTexCoord2f(texCoords[0], texCoords[3]);
	glVertex3f(vecSrc.x, vecSrc.y, vecSrc.z);
	vecSrc += s_vec3DDir[1] * rgbaTall * s_flRatio;
	glTexCoord2f(texCoords[0], texCoords[1]);
	glVertex3f(vecSrc.x, vecSrc.y, vecSrc.z);
	vecSrc -= s_vec3DDir[2] * rgbaWide * s_flRatio;
	glTexCoord2f(texCoords[2], texCoords[1]);
	glVertex3f(vecSrc.x, vecSrc.y, vecSrc.z);
	vecSrc -= s_vec3DDir[1] * rgbaTall * s_flRatio;
	glTexCoord2f(texCoords[2], texCoords[3]);
	glVertex3f(vecSrc.x, vecSrc.y, vecSrc.z);
	glEnd();

	glDisable(GL_BLEND);

	s_vec3DPos -= float(b + c) * s_flRatio * s_vec3DDir[2];
}

int F_CreateFont(void)
{
	return FontManager().CreateFont();
}

bool F_AddGlyphSetToFont(int font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange)
{
	return FontManager().AddGlyphSetToFont(font, windowsFontName, tall, weight, blur, scanlines, flags, lowRange, highRange);
}

bool F_AddCustomFontFile(const char *fontFileName)
{
	return AddFontResourceExA(fontFileName, FR_PRIVATE, 0) != 0;
}

int F_GetFontTall(int font)
{
	return FontManager().GetFontTall(font);
}

void F_GetCharABCwide(int font, int ch, int *a, int *b, int *c)
{
	return FontManager().GetCharABCwide(font, ch, *a, *b, *c);
}

int F_GetCharacterWidth(int font, int ch)
{
	return FontManager().GetCharacterWidth(font, ch);
}

void F_GetTextSize(int font, const wchar_t *text, int *wide, int *tall)
{
	return FontManager().GetTextSize(font, text, *wide, *tall);
}

int F_GetFontAscent(int font, wchar_t wch)
{
	return FontManager().GetFontAscent(font, wch);
}

int F_GetFontFlags(int font)
{
	return FontManager().GetFontFlags(font);
}

fonts_api_t gFontFuncs = 
{
	F_Init,
	F_Shutdown,
	F_DrawSetTextFont,
	F_DrawSetTextColor,
	F_DrawSetTextPos,
	F_DrawGetTextPos,
	F_DrawPrintText,
	F_DrawOutlineText,
	F_DrawUnicodeChar,
	F_CreateFont,
	F_AddGlyphSetToFont,
	F_AddCustomFontFile,
	F_GetFontTall,
	F_GetCharABCwide,
	F_GetCharacterWidth,
	F_GetTextSize,
	F_GetFontAscent,
	F_GetFontFlags,

	F_DrawSetText3DPos,
	F_DrawGetText3DPos,
	F_DrawSetText3DDir,
	F_DrawSetText3DHeight,
	F_DrawPrint3DText,
	F_DrawPrint3DTextVertical,
	F_Draw3DUnicodeChar,
	F_Draw3DUnicodeCharVertical
};
