#include "precompiled.h"
#include "FontTextureCache.h"

#pragma comment(lib, "opengl32.lib")

static int s_Font;
static int s_Color[4];
static int s_Pos[2];
static Vector s_vec3DPos;
static Vector s_vec3DDir[3];
static float s_flRatio;

void gFontFuncs::Init(void)
{
	s_Font = 0;
}

void gFontFuncs::Shutdown(void)
{
	return FontManager().ClearAllFonts();
}

void gFontFuncs::DrawSetTextFont(int font)
{
	s_Font = font;
}

void gFontFuncs::DrawSetTextColor(int r, int g, int b, int a)
{
	s_Color[0] = r;
	s_Color[1] = g;
	s_Color[2] = b;
	s_Color[3] = a;
}

void gFontFuncs::DrawSetTextColor(const Vector& color, float a)
{
	s_Color[0] = round(color.r * 255.0f);
	s_Color[1] = round(color.g * 255.0f);
	s_Color[2] = round(color.b * 255.0f);
	s_Color[3] = round(a * 255.0f);
}

void gFontFuncs::DrawSetTextColor(unsigned long ulRGB, int a)
{
	s_Color[0] = (ulRGB & 0xFF0000) >> 16;
	s_Color[1] = (ulRGB & 0xFF00) >> 8;
	s_Color[2] = ulRGB & 0xFF;
	s_Color[3] = a;
}

void gFontFuncs::DrawSetTextPos(int x, int y)
{
	s_Pos[0] = x;
	s_Pos[1] = y;
}

void gFontFuncs::DrawSetText3DPos(Vector vecSrc)
{
	s_vec3DPos = vecSrc;
}

void gFontFuncs::DrawGetText3DPos(Vector *vecSrc)
{
	*vecSrc = s_vec3DPos;
}

void gFontFuncs::DrawSetText3DDir(Vector vecForward, Vector vecRight, Vector vecUp)
{
	s_vec3DDir[0] = vecForward;
	s_vec3DDir[1] = vecRight;
	s_vec3DDir[2] = vecUp;
}

void gFontFuncs::DrawSetText3DHeight(float flHeight)
{
	if (flHeight <= 0.0f)
		s_flRatio = 1.0f;
	else
		s_flRatio = flHeight / (float)gFontFuncs::GetFontTall(s_Font);
}

void gFontFuncs::DrawGetTextPos(int *x, int *y)
{
	*x = s_Pos[0];
	*y = s_Pos[1];
}

void gFontFuncs::DrawPrintText(const wchar_t *text)
{
	int x, y;
	gFontFuncs::DrawGetTextPos(&x, &y);

	int fontTall = gFontFuncs::GetFontTall(s_Font);

	for (size_t i = 0; i < wcslen(text); ++i)
	{
		if (text[i] == '\n')
		{
			y = y + fontTall;
			gFontFuncs::DrawSetTextPos(x, y);
			continue;
		}

		if (text[i] == '\t')
			continue;

		gFontFuncs::DrawUnicodeChar(text[i]);
	}
}

void gFontFuncs::DrawPrint3DText(const wchar_t *text)
{
	Vector vecSrc;
	gFontFuncs::DrawGetText3DPos(&vecSrc);

	float fontTall = (float)gFontFuncs::GetFontTall(s_Font);

	for (size_t i = 0; i < wcslen(text); ++i)
	{
		if (text[i] == '\n')
		{
			vecSrc -= s_vec3DDir[2] * fontTall;
			gFontFuncs::DrawSetText3DPos(vecSrc);
			continue;
		}

		if (text[i] == '\t')
			continue;

		gFontFuncs::Draw3DUnicodeChar(text[i]);
	}
}

void gFontFuncs::DrawPrint3DTextVertical(const wchar_t *text)
{
	Vector vecSrc;
	gFontFuncs::DrawGetText3DPos(&vecSrc);

	float fontTall = (float)gFontFuncs::GetFontTall(s_Font);

	for (size_t i = 0; i < wcslen(text); ++i)
	{
		if (text[i] == '\n')
		{
			vecSrc -= s_vec3DDir[1] * fontTall;
			gFontFuncs::DrawSetText3DPos(vecSrc);
			continue;
		}

		if (text[i] == '\t')
			continue;

		gFontFuncs::Draw3DUnicodeCharVertical(text[i]);
	}
}

void gFontFuncs::DrawOutlineText(const wchar_t *text)
{
	static int pos[2], color[4];

	// save pos and color
	memcpy(pos, s_Pos, sizeof(s_Pos));
	memcpy(color, s_Color, sizeof(s_Color));

	// draw outline color
	gFontFuncs::DrawSetTextColor(0, 0, 0, color[3]);

	// left
	gFontFuncs::DrawSetTextPos(pos[0] - 1, pos[1]);
	gFontFuncs::DrawPrintText(text);

	// right
	gFontFuncs::DrawSetTextPos(pos[0] + 1, pos[1]);
	gFontFuncs::DrawPrintText(text);

	// top
	gFontFuncs::DrawSetTextPos(pos[0], pos[1] - 1);
	gFontFuncs::DrawPrintText(text);

	// bottom
	gFontFuncs::DrawSetTextPos(pos[0], pos[1] + 1);
	gFontFuncs::DrawPrintText(text);
	
	// draw text
	gFontFuncs::DrawSetTextColor(color[0], color[1], color[2], color[3]);
	gFontFuncs::DrawSetTextPos(pos[0], pos[1]);
	gFontFuncs::DrawPrintText(text);
}

void gFontFuncs::DrawUnicodeChar(wchar_t wch)
{
	if (!s_Font)
		return;

	int x, y;
	x = s_Pos[0];
	y = s_Pos[1];

	int a, b, c;
	gFontFuncs::GetCharABCwide(s_Font, wch, &a, &b, &c);

	int rgbaWide, rgbaTall;

	rgbaWide = b;
	rgbaTall = gFontFuncs::GetFontTall(s_Font);

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

void gFontFuncs::Draw3DUnicodeChar(wchar_t wch)
{
	if (!s_Font)
		return;

	Vector vecSrc = s_vec3DPos;

	int a, b, c;
	gFontFuncs::GetCharABCwide(s_Font, wch, &a, &b, &c);

	int rgbaWide, rgbaTall;

	rgbaWide = b;
	rgbaTall = gFontFuncs::GetFontTall(s_Font);

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

void gFontFuncs::Draw3DUnicodeCharVertical(wchar_t wch)
{
	if (!s_Font)
		return;

	Vector vecSrc = s_vec3DPos;

	int a, b, c;
	gFontFuncs::GetCharABCwide(s_Font, wch, &a, &b, &c);

	int rgbaWide, rgbaTall;

	rgbaWide = b;
	rgbaTall = gFontFuncs::GetFontTall(s_Font);

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

int gFontFuncs::CreateFont(void)
{
	return FontManager().CreateFont();
}

bool gFontFuncs::AddGlyphSetToFont(int font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange)
{
	return FontManager().AddGlyphSetToFont(font, windowsFontName, tall, weight, blur, scanlines, flags, lowRange, highRange);
}

bool gFontFuncs::AddCustomFontFile(const char *fontFileName)
{
#ifdef FILE_SYSTEM
	char szFullPath[256];
	FILE_SYSTEM->GetLocalPath(fontFileName, szFullPath, charsmax(szFullPath));

	return AddFontResourceExA(szFullPath, FR_PRIVATE, 0) != 0;
#else
	return AddFontResourceExA(fontFileName, FR_PRIVATE, 0) != 0;
#endif // FILE_SYSTEM
}

int gFontFuncs::GetFontTall(int font)
{
	return FontManager().GetFontTall(font);
}

void gFontFuncs::GetCharABCwide(int font, int ch, int *a, int *b, int *c)
{
	return FontManager().GetCharABCwide(font, ch, *a, *b, *c);
}

int gFontFuncs::GetCharacterWidth(int font, int ch)
{
	return FontManager().GetCharacterWidth(font, ch);
}

void gFontFuncs::GetTextSize(int font, const wchar_t *text, int *wide, int *tall)
{
	int dummy1 = 0, dummy2 = 0;
	if (!wide)
		wide = &dummy1;
	if (!tall)
		tall = &dummy2;

	return FontManager().GetTextSize(font, text, *wide, *tall);
}

int gFontFuncs::GetFontAscent(int font, wchar_t wch)
{
	return FontManager().GetFontAscent(font, wch);
}

int gFontFuncs::GetFontFlags(int font)
{
	return FontManager().GetFontFlags(font);
}
