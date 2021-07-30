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
	constexpr Color(uint8 r, uint8 g, uint8 b) : _color() { _color[0] = r; _color[1] = g; _color[2] = b; _color[3] = 0; }
	constexpr Color(uint8 r, uint8 g, uint8 b, uint8 a) : _color() { _color[0] = r; _color[1] = g; _color[2] = b; _color[3] = a; }
	constexpr Color(uint32 ulRGB, uint8 a) : _color() { _color[0] = (ulRGB & 0xFF0000) >> 16; _color[1] = (ulRGB & 0xFF00) >> 8; _color[2] = ulRGB & 0xFF; _color[3] = a; }
	constexpr Color(uint32 color32) : _color() { SetRawColor(color32); }
	
	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	template <typename TR, typename TG, typename TB, typename TA>
	constexpr void SetColor(TR r, TG g, TB b, TA a = 0)
	{
		_color[0] = (unsigned char)r;
		_color[1] = (unsigned char)g;
		_color[2] = (unsigned char)b;
		_color[3] = (unsigned char)a;
	}

	template <typename TR, typename TG, typename TB, typename TA>
	constexpr void GetColor(TR &r, TG &g, TB &b, TA &a) const
	{
		r = static_cast<TR>(_color[0]);
		g = static_cast<TG>(_color[1]);
		b = static_cast<TB>(_color[2]);
		a = static_cast<TA>(_color[3]);
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

	constexpr uint32 GetRawColor() const
	{
		return *((uint32*)this);
	}

	constexpr uint32 GetRawRGB() const
	{
		return static_cast<uint32>(_color[0] << 16 | _color[1] << 8 | _color[2]);
	}

	inline constexpr uint8 r() const	{ return _color[0]; }
	inline constexpr uint8 g() const	{ return _color[1]; }
	inline constexpr uint8 b() const	{ return _color[2]; }
	inline constexpr uint8 a() const	{ return _color[3]; }
	
	constexpr uint8& operator[](int index)
	{
		if (index >= _countof(_color))
			throw;

		return _color[index];
	}

	constexpr const uint8& operator[](int index) const
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
	uint8 _color[4];
};


#endif // COLOR_H
