#pragma once

#ifndef DRAWFONTS_H
#define DRAWFONTS_H

#ifdef CreateFont
#undef CreateFont
#endif

#ifdef GetCharABCwide
#undef GetCharABCwide
#endif

#include "FontManager.h"

namespace gFontFuncs
{
	void	Init				( void );
	void	Shutdown			( void );
	void	DrawSetTextFont		( int font );
	void	DrawSetTextColor	( int r, int g, int b, int a );
	void	DrawSetTextColor	( const Vector& color, float a );
	void	DrawSetTextColor	( unsigned long ulRGB, int a );
	void	DrawSetTextPos		( int x, int y );
	void	DrawGetTextPos		( int *x, int *y );
	void	DrawPrintText		( const wchar_t *text );
	void	DrawOutlineText		( const wchar_t *text );
	void	DrawUnicodeChar		( wchar_t wch );
	int		CreateFont			( void );
	bool	AddGlyphSetToFont	( int font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange );
	bool	AddCustomFontFile	( const char *fontFileName );
	int		GetFontTall			( int font );
	void	GetCharABCwide		( int font, int ch, int *a, int *b, int *c );
	int		GetCharacterWidth	( int font, int ch );
	void	GetTextSize			( int font, const wchar_t *text, int *wide, int *tall );
	int		GetFontAscent		( int font, wchar_t wch );
	int		GetFontFlags		( int font );

	// extra: 3D UI
	void	DrawSetText3DPos		( Vector vecSrc );
	void	DrawGetText3DPos		( Vector *vecSrc );
	void	DrawSetText3DDir		( Vector vecForward, Vector vecRight, Vector vecUp );
	void	DrawSetText3DHeight		( float flHeight );
	void	DrawPrint3DText			( const wchar_t *text );
	void	DrawPrint3DTextVertical	( const wchar_t *text );
	void	Draw3DUnicodeChar		( wchar_t wch );
	void	Draw3DUnicodeCharVertical( wchar_t wch );
};

#endif
