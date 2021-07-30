module;

#include <stdlib.h>

#include <algorithm>
#include <cmath>
#include <concepts>
#include <limits>
#include <memory>
#include <type_traits>

export module Color;

using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;

// Completement of std::integral and std::floating_point.
template<typename T> concept Arithmetic = std::is_arithmetic_v<T>;

typedef union
{
	uint32 color32;
	uint8 ubcolors[4];

} color32_helper_t;

//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
export struct Color4b
{
	// constructors
	constexpr Color4b() : _color() { *((int*)this) = 0; }
	constexpr Color4b(uint8 r, uint8 g, uint8 b) : _color() { _color[0] = r; _color[1] = g; _color[2] = b; _color[3] = 0; }
	constexpr Color4b(uint8 r, uint8 g, uint8 b, uint8 a) : _color() { _color[0] = r; _color[1] = g; _color[2] = b; _color[3] = a; }
	constexpr Color4b(uint32 ulRGB, uint8 a) : _color() { _color[0] = (ulRGB & 0xFF0000) >> 16; _color[1] = (ulRGB & 0xFF00) >> 8; _color[2] = ulRGB & 0xFF; _color[3] = a; }
	constexpr Color4b(uint32 color32) : _color() { SetRawColor(color32); }
	
	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	constexpr void SetColor(Arithmetic auto r, Arithmetic auto g, Arithmetic auto b, Arithmetic auto a)
	{
		_color[0] = (unsigned char)r;
		_color[1] = (unsigned char)g;
		_color[2] = (unsigned char)b;
		_color[3] = (unsigned char)a;
	}

	constexpr void GetColor(Arithmetic auto& r, Arithmetic auto& g, Arithmetic auto& b, Arithmetic auto& a) const
	{
		r = static_cast<std::remove_reference_t<decltype(r)>>(_color[0]);
		g = static_cast<std::remove_reference_t<decltype(g)>>(_color[1]);
		b = static_cast<std::remove_reference_t<decltype(b)>>(_color[2]);
		a = static_cast<std::remove_reference_t<decltype(a)>>(_color[3]);
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

	constexpr const uint8& operator[](int index) const
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

	constexpr bool operator== (const Color4b& rhs) const { return (*((int*)this) == *((int*)&rhs)); }
	constexpr bool operator!= (const Color4b& rhs) const { return !(operator==(rhs)); }
	constexpr Color4b& operator=(const Color4b& rhs) { *((int*)this) = *((int*)&rhs); return *this; }

	constexpr decltype(auto) operator~() const { return Color4b(*((uint32*)this) ^ 0xFFFFFF); }	// Reversed color. It is easier on HEX calculation.

private:
	uint8 _color[4];
};

export struct Color4f
{
	constexpr Color4f() : _r(0), _g(0), _b(0), _a(0) {}
	constexpr Color4f(uint8 r, uint8 g, uint8 b) : _r(static_cast<double>(r) / 255.0), _g(static_cast<double>(g) / 255.0), _b(static_cast<double>(b) / 255.0), _a(0) {}
	constexpr Color4f(uint8 r, uint8 g, uint8 b, uint8 a) : _r(static_cast<double>(r) / 255.0), _g(static_cast<double>(g) / 255.0), _b(static_cast<double>(b) / 255.0), _a(static_cast<double>(a) / 255.0) {}
	constexpr Color4f(uint32 ulRGB, uint8 a) : _r(), _g(), _b(), _a() { SetRawColor(ulRGB, a); }
	constexpr Color4f(uint32 hexColorAGBR) : _r(), _g(), _b(), _a() { SetRawColor(hexColorAGBR); }
	constexpr Color4f(const Color4b& color4ub) : _r(), _g(), _b(), _a() { SetRawColor(color4ub); }

	constexpr void SetRGB(std::floating_point auto& r, std::floating_point auto& g, std::floating_point auto& b)
	{
		_r = std::clamp(static_cast<double>(r), 0.0, 1.0);
		_g = std::clamp(static_cast<double>(g), 0.0, 1.0);
		_b = std::clamp(static_cast<double>(b), 0.0, 1.0);
	}

	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	constexpr void SetRGB(std::integral auto r, std::integral auto g, std::integral auto b)
	{
		_r = std::clamp(static_cast<double>(r) / 255.0, 0.0, 1.0);
		_g = std::clamp(static_cast<double>(g) / 255.0, 0.0, 1.0);
		_b = std::clamp(static_cast<double>(b) / 255.0, 0.0, 1.0);
	}

	constexpr void GetRGB(Arithmetic auto& r, Arithmetic auto& g, Arithmetic auto& b) const
	{
		r = static_cast<std::remove_reference_t<decltype(r)>>(_r * 255.0);
		g = static_cast<std::remove_reference_t<decltype(g)>>(_g * 255.0);
		b = static_cast<std::remove_reference_t<decltype(b)>>(_b * 255.0);
	}

	static constexpr Color4f RGB(std::integral auto r, std::integral auto g, std::integral auto b)
	{
		Color4f obj;
		obj.SetRGB(r, g, b);
		return obj;
	}

	static constexpr Color4f RGB(std::floating_point auto r, std::floating_point auto g, std::floating_point auto b)
	{
		Color4f obj;
		obj['r'] = static_cast<double>(r);
		obj['g'] = static_cast<double>(g);
		obj['b'] = static_cast<double>(b);

		return obj;
	}

	constexpr void SetRawColor(uint32 hexColorAGBR)
	{
		color32_helper_t color;
		color.color32 = hexColorAGBR;

		_r = static_cast<double>(color.ubcolors[0]) / 255.0;
		_g = static_cast<double>(color.ubcolors[1]) / 255.0;
		_b = static_cast<double>(color.ubcolors[2]) / 255.0;
		_a = static_cast<double>(color.ubcolors[3]) / 255.0;
	}

	constexpr void SetRawColor(uint32 ulRGB, uint8 a)
	{
		_r = static_cast<double>((ulRGB & 0xFF0000) >> 16) / 255.0;
		_g = static_cast<double>((ulRGB & 0xFF00) >> 8) / 255.0;
		_b = static_cast<double>(ulRGB & 0xFF) / 255.0;
		_a = static_cast<double>(a) / 255.0;
	}

	constexpr void SetRawColor(const Color4b& color4ub)
	{
		_r = static_cast<double>(color4ub.r()) / 255.0;
		_g = static_cast<double>(color4ub.g()) / 255.0;
		_b = static_cast<double>(color4ub.b()) / 255.0;
		_a = static_cast<double>(color4ub.a()) / 255.0;
	}

	constexpr uint32 GetRawColor() const	// Returns 0xAABBGGRR
	{
		color32_helper_t color;
		color.ubcolors[0] = static_cast<uint8>(_r * 255.0);
		color.ubcolors[1] = static_cast<uint8>(_g * 255.0);
		color.ubcolors[2] = static_cast<uint8>(_b * 255.0);
		color.ubcolors[3] = static_cast<uint8>(_a * 255.0);

		return color.color32;
	}

	constexpr uint32 GetRawRGB() const	// Returns 0xRRGGBB
	{
		auto r = static_cast<uint8>(_r * 255.0);
		auto g = static_cast<uint8>(_g * 255.0);
		auto b = static_cast<uint8>(_b * 255.0);

		return static_cast<uint32>(r << 16 | g << 8 | b);
	}

	constexpr Color4b GetColor4ubObj() const
	{
		color32_helper_t color;
		GetRGB(color.ubcolors[0], color.ubcolors[1], color.ubcolors[2]);

		color.ubcolors[3] = static_cast<uint8>(_a * 255.0);

		return Color4b(color.color32);
	}

	constexpr void SetHSV(const double& h, const double& s, const double& v)	// HSV to RGB. H[0-360], S[0-1], V[0-1]
	{
		if (s < DBL_EPSILON)	// < is bogus, just shuts up warnings
		{
			_r = v;
			_g = v;
			_b = v;

			return;
		}

		double hh = h;
		if (hh >= 360.0)
			hh = 0.0;

		hh /= 60.0;

		long i = static_cast<long>(hh);
		double ff = hh - i;
		double p = v * (1.0 - s);
		double q = v * (1.0 - (s * ff));
		double t = v * (1.0 - (s * (1.0 - ff)));

		switch (i)
		{
		case 0:
			_r = v;
			_g = t;
			_b = p;
			break;

		case 1:
			_r = q;
			_g = v;
			_b = p;
			break;

		case 2:
			_r = p;
			_g = v;
			_b = t;
			break;

		case 3:
			_r = p;
			_g = q;
			_b = v;
			break;

		case 4:
			_r = t;
			_g = p;
			_b = v;
			break;

		case 5:
		default:
			_r = v;
			_g = p;
			_b = q;
			break;
		}
	}

	constexpr void GetHSV(double& h, double& s, double& v) const
	{
		double r = _r * 255.0, g = _g * 255.0, b = _b * 255.0;	// mask the function call.
		auto min = std::min({ r, g, b });
		auto max = std::max({ r, g, b });

		v = std::max({ _r, _g, _b });	// v

		double delta = max - min;
		if (delta < DBL_EPSILON)
		{
			s = 0;
			h = std::numeric_limits<double>::quiet_NaN();	// undefined

			return;
		}

		if (max > 0.0)	// NOTE: if Max is == 0, this divide would cause a crash
		{
			s = (delta / max);	// s
		}
		else
		{
			// if max is 0, then r = g = b = 0              
			// s = 0, h is undefined
			s = 0.0;
			h = std::numeric_limits<double>::quiet_NaN();	// its now undefined

			return;
		}

		if (r >= max)					// > is bogus, just keeps compilor happy
			h = (g - b) / delta;	// between yellow & magenta
		else if (g >= max)
			h = 2.0 + (b - r) / delta;	// between cyan & yellow
		else
			h = 4.0 + (r - g) / delta;	// between magenta & cyan

		h *= 60.0;	// degrees

		if (h < 0.0)
			h += 360.0;
	}

	static constexpr Color4f HSV(const double& h, const double& s, const double& v)
	{
		Color4f obj;
		obj.SetHSV(h, s, v);
		return obj;
	}

	constexpr void SetHSL(const double& h, const double& s, const double& l)	// HSV to RGB. H[0-360], S[0-1], V[0-1]
	{
		if (s < DBL_EPSILON)
		{
			_r = _g = _b = l; // achromatic
		}
		else
		{
			auto hue2rgb = [](const double& p, const double& q, double t)
			{
				if (t < DBL_EPSILON) t += 1.0;
				if (t > 1.0) t -= 1.0;
				if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
				if (t < 1.0 / 2.0) return q;
				if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
				return p;
			};

			auto q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
			auto p = 2.0 * l - q;
			auto t = h / 360.0;

			_r = hue2rgb(p, q, t + 1.0 / 3.0);
			_g = hue2rgb(p, q, t);
			_b = hue2rgb(p, q, t - 1.0 / 3.0);
		}
	}

	constexpr void GetHSL(double& h, double& s, double& l) const
	{
		auto min = std::min({ _r, _g, _b });
		auto max = std::max({ _r, _g, _b });
		auto delta = max - min;

		l = (max + min) / 2.0;

		if (delta < DBL_EPSILON)
		{
			h = s = 0; // achromatic
		}
		else
		{
			s = l > 0.5 ? delta / (2.0 - max - min) : delta / (max + min);

			if (max == _r)
				h = (_g - _b) / delta + (_g < _b ? 6.0 : 0);
			else if (max == _g)
				h = (_b - _r) / delta + 2.0;
			else if (max == _b)
				h = (_r - _g) / delta + 4.0;

			h /= 6.0;
			h *= 360.0;
		}
	}

	static constexpr Color4f HSL(const double& h, const double& s, const double& l)
	{
		Color4f obj;
		obj.SetHSL(h, s, l);
		return obj;
	}

	// Check function.
	constexpr void Rationalize()
	{
		_r = std::clamp(_r, 0.0, 1.0);
		_g = std::clamp(_g, 0.0, 1.0);
		_b = std::clamp(_b, 0.0, 1.0);
		_a = std::clamp(_a, 0.0, 1.0);
	}

	// Color component assigning functions.
	// RGB type.
	inline constexpr void SetR(std::integral auto r) { _r = static_cast<double>(r) / 255.0; }
	inline constexpr void SetG(std::integral auto g) { _g = static_cast<double>(g) / 255.0; }
	inline constexpr void SetB(std::integral auto b) { _b = static_cast<double>(b) / 255.0; }
	inline constexpr void SetA(std::integral auto a) { _a = static_cast<double>(a) / 255.0; }

	// HSV/HSL type.
	inline constexpr void SetH(Arithmetic auto h) { _UpdateHSVBuffer(); SetHSV(static_cast<double>(h), _s, _v); }
	inline constexpr void SetS_hsv(std::floating_point auto s) { _UpdateHSVBuffer(); SetHSV(_h, static_cast<double>(s), _v); }
	inline constexpr void SetV(std::floating_point auto v) { _UpdateHSVBuffer(); SetHSV(_h, _s, static_cast<double>(v)); }
	inline constexpr void SetS_hsl(std::floating_point auto s) { _UpdateHSLBuffer(); SetHSL(_h, static_cast<double>(s), _l); }
	inline constexpr void SetL(std::floating_point auto l) { _UpdateHSLBuffer(); SetHSL(_h, _s, static_cast<double>(l)); }

	// Color component retrieve functions.
	// RGB type.
	inline constexpr uint8 r() const { return static_cast<uint8>(_r * 255.0); }
	inline constexpr uint8 g() const { return static_cast<uint8>(_g * 255.0); }
	inline constexpr uint8 b() const { return static_cast<uint8>(_b * 255.0); }
	inline constexpr uint8 a() const { return static_cast<uint8>(_a * 255.0); }

	// HSV/HSL type.
	constexpr double h() const	// Degree: [0-360]
	{
		double r = _r * 255.0, g = _g * 255.0, b = _b * 255.0;	// mask the function call.

		auto min = std::min({ r, g, b });
		auto max = std::max({ r, g, b });

		double delta = max - min;
		if (delta < DBL_EPSILON)
		{
			return std::numeric_limits<double>::quiet_NaN();	// In this case, h value is undefined.
		}

		if (max < DBL_EPSILON)
		{
			// if max is 0, then r = g = b = 0              
			// s = 0, h is undefined
			return std::numeric_limits<double>::quiet_NaN();	// its now undefined
		}

		double h = 0.0;

		if (r >= max)					// > is bogus, just keeps compilor happy
			h = (g - b) / delta;	// between yellow & magenta
		else if (g >= max)
			h = 2.0 + (b - r) / delta;	// between cyan & yellow
		else
			h = 4.0 + (r - g) / delta;	// between magenta & cyan

		h *= 60.0;	// degrees

		if (h < 0.0)
			h += 360.0;

		return h;
	}
	constexpr double s_hsv() const	// Percentage: [0.0-1.0]
	{
		auto min = std::min({ _r, _g, _b });
		auto max = std::max({ _r, _g, _b });

		double delta = max - min;
		if (delta < DBL_EPSILON)
		{
			return 0.0;
		}

		if (max > DBL_EPSILON)	// NOTE: if Max is == 0, this divide would cause a crash
		{
			return (delta / max);	// s
		}
		else
		{
			// if max is 0, then r = g = b = 0              
			// s = 0, h is undefined
			return 0.0;
		}
	}
	constexpr double v() const	// Percentage: [0.0-1.0]
	{
		return std::max({ _r, _g, _b });	// v
	}
	constexpr double s_hsl() const
	{
		auto min = std::min({ _r, _g, _b });
		auto max = std::max({ _r, _g, _b });
		auto delta = max - min;

		return l() > 0.5 ? delta / (2.0 - max - min) : delta / (max + min);
	}
	constexpr double l() const	// Percentage: [0.0-1.0]
	{
		return 0.5 * (std::min({ _r, _g, _b }) + std::max({ _r, _g, _b }));	// Use [0-1] floating color here.
	}

	// Operators.
	// Retrieve these actually returns you the original floating color value.
	constexpr double& operator[](int index)
	{
		if (index >= 4)	// [0 - R, 1 - G, 2 - B, 3 - A]
			throw;

		return ((double*)(&_r))[index];
	}

	constexpr double& operator[](char index)
	{
		switch (index)
		{
		case 'r':
		case 'R':
			return _r;
			
		case 'g':
		case 'G':
			return _g;

		case 'b':
		case 'B':
			return _b;

		case 'a':
		case 'A':
			return _a;

		default:
			throw;
		}
	}

	// Const version for those const object.
	constexpr const double operator[](int index) const
	{
		if (index >= 4)	// [0 - R, 1 - G, 2 - B, 3 - A]
			throw;

		return ((const double*)(&_r))[index];
	}

	constexpr const double operator[](char index) const
	{
		switch (index)
		{
		case 'r':
		case 'R':
			return _r;

		case 'g':
		case 'G':
			return _g;

		case 'b':
		case 'B':
			return _b;

		case 'a':
		case 'A':
			return _a;

		case 'h':
		case 'H':
			return h();

		// Undefinable 's'
		// Which should be return? S from HSV or S from HSL?
		// case 's':
		// case 'S':

		case 'v':
		case 'V':
			return v();

		case 'l':
		case 'L':
			return l();

		default:
			throw;
		}
	}

	constexpr bool operator== (const Color4f& rhs) const { return _r == rhs._r && _g == rhs._g && _b == rhs._b && _a == rhs._a; }	// Shame on C++, 'memcmp' should be a constexpr function.
	constexpr bool operator== (const Color4b& rhs) const { return GetRawColor() == rhs.GetRawColor(); }
	constexpr bool operator!= (const Color4f& rhs) const { return _r != rhs._r || _g != rhs._g || _b != rhs._b || _a != rhs._a; }
	constexpr bool operator!= (const Color4b& rhs) const { return GetRawColor() != rhs.GetRawColor(); }

	constexpr Color4f& operator=(const Color4f& rhs) { _r = rhs._r; _g = rhs._g; _b = rhs._b; _a = rhs._a; return *this; }	// Shame on C++, 'memcpy' should be a constexpr function.
	constexpr Color4f& operator=(const Color4b& rhs) { SetRawColor(rhs); return *this; }

	constexpr decltype(auto) operator+(const Color4f& v) const { return RGB(_r + v._r, _g + v._g, _b + v._b); }
	constexpr decltype(auto) operator-(const Color4f& v) const { return RGB(_r - v._r, _g - v._g, _b - v._b); }
	constexpr decltype(auto) operator+=(const Color4f& v) { return (*this = *this + v); }
	constexpr decltype(auto) operator-=(const Color4f& v) { return (*this = *this - v); }

	constexpr decltype(auto) operator+(const Color4b& v) const { return *this + Color4f(v); }
	constexpr decltype(auto) operator-(const Color4b& v) const { return *this - Color4f(v); }
	constexpr decltype(auto) operator+=(const Color4b& v) { return (*this = *this + v); }
	constexpr decltype(auto) operator-=(const Color4b& v) { return (*this = *this - v); }

	constexpr decltype(auto) operator*(float fl) const { return RGB(_r * fl, _g * fl, _b * fl); }
	constexpr decltype(auto) operator/(float fl) const { return RGB(_r / fl, _g / fl, _b / fl); }
	constexpr decltype(auto) operator*=(float fl) { return (*this = *this * fl); }
	constexpr decltype(auto) operator/=(float fl) { return (*this = *this / fl); }

	constexpr decltype(auto) operator~() const { return Color4f(GetRawRGB() ^ 0xFFFFFF); }	// Inverse color. By definition it is the hue that actually 'reversed'. i.e. (hue + 180) % 360.

private:
	double _r, _g, _b, _a;

	// Private static members.
private:
	static inline double _h = 0, _s = 0, _v = 0, _l = 0;

	inline constexpr void _UpdateHSVBuffer() const { GetHSV(_h, _s, _v); }
	inline constexpr void _UpdateHSLBuffer() const { GetHSL(_h, _s, _l); }
};

export inline constexpr bool operator== (const Color4b& lhs, const Color4f& rhs) { return rhs == lhs; }
export inline constexpr bool operator!= (const Color4b& lhs, const Color4f& rhs) { return rhs != lhs; }
export inline constexpr decltype(auto) operator+(const Color4b& lhs, const Color4f& rhs) { return rhs + lhs; }
export inline constexpr decltype(auto) operator-(const Color4b& lhs, const Color4f& rhs) { return rhs - lhs; }
export inline constexpr decltype(auto) operator+=(Color4b& lhs, const Color4f& rhs) { return (lhs = (lhs + rhs).GetColor4ubObj()); }
export inline constexpr decltype(auto) operator-=(Color4b& lhs, const Color4f& rhs) { return (lhs = (lhs - rhs).GetColor4ubObj()); }
export inline constexpr decltype(auto) operator*(float fl, const Color4f& c) { return c * fl; }
export inline constexpr decltype(auto) operator/(float fl, const Color4f& c) { return c / fl; }
