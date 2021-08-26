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

#include <concepts>

//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
struct Color
{
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
	constexpr void SetColor(std::integral auto r, std::integral auto g, std::integral auto b, std::integral auto a)
	{
		_color[0] = static_cast<uint8>(r);
		_color[1] = static_cast<uint8>(g);
		_color[2] = static_cast<uint8>(b);
		_color[3] = static_cast<uint8>(a);
	}

	constexpr void SetColor(std::floating_point auto r, std::floating_point auto g, std::floating_point auto b, std::floating_point auto a)
	{
		_color[0] = static_cast<uint8>(r * 255.0);
		_color[1] = static_cast<uint8>(g * 255.0);
		_color[2] = static_cast<uint8>(b * 255.0);
		_color[3] = static_cast<uint8>(a * 255.0);
	}

	constexpr void GetColor(std::integral auto& r, std::integral auto& g, std::integral auto& b, std::integral auto& a) const
	{
		r = static_cast<std::remove_reference_t<decltype(r)>>(_color[0]);
		g = static_cast<std::remove_reference_t<decltype(g)>>(_color[1]);
		b = static_cast<std::remove_reference_t<decltype(b)>>(_color[2]);
		a = static_cast<std::remove_reference_t<decltype(a)>>(_color[3]);
	}

	constexpr void GetColor(std::floating_point auto& r, std::floating_point auto& g, std::floating_point auto& b, std::floating_point auto& a) const
	{
		r = static_cast<std::remove_reference_t<decltype(r)>>(_color[0]) / 255.0;
		g = static_cast<std::remove_reference_t<decltype(g)>>(_color[1]) / 255.0;
		b = static_cast<std::remove_reference_t<decltype(b)>>(_color[2]) / 255.0;
		a = static_cast<std::remove_reference_t<decltype(a)>>(_color[3]) / 255.0;
	}

	constexpr void SetRawColor(uint32 hexColorAGBR)
	{
		*((int*)this) = hexColorAGBR;
	}

	constexpr void SetRawColor(uint32 ulRGB, uint8 a)
	{
		_color[0] = (ulRGB & 0xFF0000) >> 16;	// r
		_color[1] = (ulRGB & 0xFF00) >> 8;		// g
		_color[2] = ulRGB & 0xFF;				// b
		_color[3] = a;
	}

	constexpr uint32 GetRawColor() const	// Returns 0xAABBGGRR
	{
		return *((uint32*)this);
	}

	constexpr uint32 GetRawRGB() const	// Returns 0xRRGGBB
	{
		return static_cast<uint32>(_color[0] << 16 | _color[1] << 8 | _color[2]);
	}

	inline constexpr uint8 r() const { return _color[0]; }
	inline constexpr uint8 g() const { return _color[1]; }
	inline constexpr uint8 b() const { return _color[2]; }
	inline constexpr uint8 a() const { return _color[3]; }

	constexpr uint8& operator[](int index)
	{
		if (index >= _countof(_color))
			throw;

		return _color[index];
	}

	constexpr uint8& operator[](char index)
	{
		switch (index)
		{
		case 'r':
		case 'R':
			return _color[0];

		case 'g':
		case 'G':
			return _color[1];

		case 'b':
		case 'B':
			return _color[2];

		case 'a':
		case 'A':
			return _color[3];

		default:
			throw;
		}
	}

	constexpr const uint8& operator[](std::integral auto index) const
	{
		if (index >= _countof(_color))
			throw;

		return _color[index];
	}

	constexpr const uint8& operator[](char index) const
	{
		switch (index)
		{
		case 'r':
		case 'R':
			return _color[0];

		case 'g':
		case 'G':
			return _color[1];

		case 'b':
		case 'B':
			return _color[2];

		case 'a':
		case 'A':
			return _color[3];

		default:
			throw;
		}
	}

	constexpr bool operator== (const Color& rhs) const { return (*((int*)this) == *((int*)&rhs)); }	// operator!= will be automatically generated in C++20.
	constexpr Color& operator=(const Color& rhs) { *((int*)this) = *((int*)&rhs); return *this; }

	constexpr decltype(auto) operator~() const { return Color(*((uint32*)this) ^ 0xFFFFFF); }	// Reversed color. It is easier on HEX calculation.

	constexpr decltype(auto) operator*(double fl) const
	{
		Color c;
		c._color[0] = static_cast<uint8>((double)_color[0] * fl);
		c._color[1] = static_cast<uint8>((double)_color[1] * fl);
		c._color[2] = static_cast<uint8>((double)_color[2] * fl);
		c._color[3] = _color[3];

		return c;
	}

private:
	uint8 _color[4];
};


#endif // COLOR_H
