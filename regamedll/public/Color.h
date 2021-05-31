//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef COLOR_H
#define COLOR_H

#ifdef _WIN32
#pragma once
#endif

#include <stdexcept>

//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
class Color
{
public:
	// constructors
	constexpr Color() : _color() { *((int*)this) = 0; }
	constexpr Color(BYTE r, BYTE g, BYTE b) : _color() { _color[0] = r; _color[1] = g; _color[2] = b; _color[3] = 0; }
	constexpr Color(BYTE r, BYTE g, BYTE b, BYTE a) : _color() { _color[0] = r; _color[1] = g; _color[2] = b; _color[3] = a; }
	constexpr Color(uint32 ulRGB, BYTE a) : _color() { _color[0] = (ulRGB & 0xFF0000) >> 16; _color[1] = (ulRGB & 0xFF00) >> 8; _color[2] = ulRGB & 0xFF; _color[3] = a; }
	constexpr Color(uint32 color32) : _color() { SetRawColor(color32); }
	
	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	constexpr void SetColor(int r, int g, int b, int a = 0)
	{
		_color[0] = (unsigned char)r;
		_color[1] = (unsigned char)g;
		_color[2] = (unsigned char)b;
		_color[3] = (unsigned char)a;
	}

	constexpr void GetColor(int &r, int &g, int &b, int &a) const
	{
		r = _color[0];
		g = _color[1];
		b = _color[2];
		a = _color[3];
	}

	constexpr void SetRawColor(uint32 color32)
	{
		*((int*)this) = color32;
	}

	constexpr void SetRawColor(uint32 ulRGB, BYTE a)
	{
		_color[0] = (ulRGB & 0xFF0000) >> 16;	// r
		_color[1] = (ulRGB & 0xFF00) >> 8;		// g
		_color[2] = ulRGB & 0xFF;				// b
		_color[3] = a;
	}

	constexpr int GetRawColor() const
	{
		return *((int *)this);
	}

	inline constexpr int r() const	{ return _color[0]; }
	inline constexpr int g() const	{ return _color[1]; }
	inline constexpr int b() const	{ return _color[2]; }
	inline constexpr int a() const	{ return _color[3]; }
	
	constexpr unsigned char &operator[](int index)
	{
		if (index >= _countof(_color))
			throw;

		return _color[index];
	}

	constexpr const unsigned char &operator[](int index) const
	{
		if (index >= _countof(_color))
			throw;

		return _color[index];
	}

	constexpr bool operator == (const Color &rhs) const
	{
		return ( *((int *)this) == *((int *)&rhs) );
	}

	constexpr bool operator != (const Color &rhs) const
	{
		return !(operator==(rhs));
	}

	constexpr Color& operator=(const Color& rhs)
	{
		SetRawColor(rhs.GetRawColor());
		return *this;
	}

private:
	unsigned char _color[4];
};


#endif // COLOR_H
