module;

#define _USE_MATH_DEFINES
#include <float.h>
#include <math.h>

#include <bit>
#include <cmath>
#include <concepts>

export module Vector;

export inline constexpr float rsqrt(float x)
{
#ifdef __SSE__
	return _mm_rsqrt_ps(_mm_set1_ps(x))[0];
#else
	/*
	* Reference:
	* Walczyk, C.J.; Moroz, L.V.; Cie´sli ´nski, J.L. Improving the Accuracy of the Fast Inverse Square Root by Modifying Newton–Raphson
	Corrections. Entropy 2021, 23, 86.
	*/

	float xhalf = 0.5f * x;
	int i = std::bit_cast<int, float>(x);
	i = 0x5f376908 - (i >> 1);

	float y = std::bit_cast<float, int>(i);
	y = y * (1.50087896f - xhalf * y * y);
	y = y * (1.50000057f - xhalf * y * y);	// Second iteration.

	return y;
#endif
}

export template <typename T>
concept IsDouble = requires (T t)
{
	{t.x} -> std::same_as<float>;
	{t.y} -> std::same_as<float>;
};

export using vec_t = float;

// Used for many pathfinding and many other operations that are treated as planar rather than 3D.
export struct Vector2D
{
	// Construction/destruction
	constexpr Vector2D(Vector2D&& s) = default;
	Vector2D& operator=(const Vector2D& s) = default;
	Vector2D& operator=(Vector2D&& s) = default;
	constexpr Vector2D() : x(0), y(0) {}
	template<typename TX, typename TY> constexpr Vector2D(TX X, TY Y) : x(static_cast<vec_t>(X)), y(static_cast<vec_t>(Y)) {}
	template<typename T> constexpr Vector2D(T sideLength) : width(static_cast<vec_t>(sideLength)), height(static_cast<vec_t>(sideLength)) {}
	template<typename T> constexpr Vector2D(const T& v) : x(v.x), y(v.y) {}
	template<typename T> explicit constexpr Vector2D(const T rgfl[2]) : x(rgfl[0]), y(rgfl[1]) {}

	// Operators
	constexpr decltype(auto) operator-()         const { return Vector2D(-x, -y); }
	constexpr bool operator==(const Vector2D& v) const { return x == v.x && y == v.y; }
	constexpr bool operator!=(const Vector2D& v) const { return !(*this == v); }

	constexpr decltype(auto) operator+(const Vector2D& v)  const { return Vector2D(x + v.x, y + v.y); }
	constexpr decltype(auto) operator-(const Vector2D& v)  const { return Vector2D(x - v.x, y - v.y); }
	constexpr decltype(auto) operator*(const Vector2D& v)  const { return Vector2D(x * v.x, y * v.y); }
	constexpr decltype(auto) operator/(const Vector2D& v)  const { return Vector2D(x / v.x, y / v.y); }

	constexpr decltype(auto) operator+=(const Vector2D& v) { return (*this = *this + v); }
	constexpr decltype(auto) operator-=(const Vector2D& v) { return (*this = *this - v); }
	constexpr decltype(auto) operator*=(const Vector2D& v) { return (*this = *this * v); }
	constexpr decltype(auto) operator/=(const Vector2D& v) { return (*this = *this / v); }

	template<typename T> constexpr decltype(auto) operator*(T fl) const { return Vector2D(x * fl, y * fl); }
	template<typename T> constexpr decltype(auto) operator/(T fl) const { return Vector2D(x / fl, y / fl); }

	constexpr decltype(auto) operator=(std::nullptr_t) { return Vector2D(0, 0); }
	template<typename T> constexpr decltype(auto) operator*=(T fl) { return (*this = *this * fl); }
	template<typename T> constexpr decltype(auto) operator/=(T fl) { return (*this = *this / fl); }

	// Static methods
	static constexpr decltype(auto) Zero() { return Vector2D(0, 0); }
	static constexpr decltype(auto) I() { return Vector2D(1, 0); }
	static constexpr decltype(auto) J() { return Vector2D(0, 1); }

	// Methods
	inline void Clear() { x = 0; y = 0; }
	template<class OutputIter> inline void CopyToIter(OutputIter arr) const { *arr++ = x; *arr++ = y; }
	constexpr float Length() const { return 1.0f / rsqrt(x * x + y * y); }	// Get the vector's magnitude
	constexpr vec_t LengthSquared() const { return x * x + y * y; }	// Get the vector's magnitude squared

	constexpr operator float* () { return &x; } // Vectors will now automatically convert to float * when needed
	constexpr operator const float* () const { return &x; } // Vectors will now automatically convert to float * when needed

	constexpr Vector2D Normalize() const
	{
		if (LengthSquared() <= FLT_EPSILON)
			return Zero();

		auto invsqrt = rsqrt(x * x + y * y);
		return Vector2D(x * invsqrt, y * invsqrt);
	}
	constexpr float NormalizeInPlace()
	{
		if (LengthSquared() <= FLT_EPSILON)
		{
			x = 1;
			y = 0;

			return 0.0f;
		}

		auto fl = rsqrt(x * x + y * y);

		x *= fl;
		y *= fl;

		return 1.0f / fl;
	}
	template<typename T> constexpr Vector2D SetLength(T newlen) const
	{
		if (LengthSquared() <= FLT_EPSILON)
			return Zero();

		auto fl = static_cast<float>(newlen) * rsqrt(x * x + y * y);
		return Vector2D(x * fl, y * fl);
	}
	template<typename T> constexpr void SetLengthInPlace(T newlen)
	{
		if (LengthSquared() <= FLT_EPSILON)
		{
			x = 0;
			y = 0;

			return;
		}

		auto fl = static_cast<float>(newlen) * rsqrt(x * x + y * y);

		x *= fl;
		y *= fl;
	}

	// LUNA: comparison with Vector3 was moved to the end of this file.
	template<typename T> constexpr bool operator< (T fl) const { return !!(LengthSquared() < (fl * fl)); }
	template<typename T> constexpr bool operator<= (T fl) const { return !!(LengthSquared() <= (fl * fl)); }
	constexpr bool operator< (const Vector2D& v) const { return !!(LengthSquared() < v.LengthSquared()); }
	constexpr bool operator<= (const Vector2D& v) const { return !!(LengthSquared() <= v.LengthSquared()); }
	template<typename T> constexpr bool operator> (T fl) const { return !!(LengthSquared() > (fl * fl)); }
	template<typename T> constexpr bool operator>= (T fl) const { return !!(LengthSquared() >= (fl * fl)); }
	constexpr bool operator> (const Vector2D& v) const { return !!(LengthSquared() > v.LengthSquared()); }
	constexpr bool operator>= (const Vector2D& v) const { return !!(LengthSquared() >= v.LengthSquared()); }

	template<typename T = float>
	constexpr bool IsZero(T tolerance = FLT_EPSILON) const
	{
		return (x > -tolerance && x < tolerance&&
			y > -tolerance && y < tolerance);
	}

	explicit constexpr operator bool () const { return !IsZero(); }	// Can be placed in if() now.

	// anti-clockwise.
	template<typename T>
	Vector2D Rotate(T angle) const
	{
		auto a = (static_cast<double>(angle) * M_PI / 180.0);
		auto c = cos(a);
		auto s = sin(a);

		return Vector2D(
			c * x - s * y,
			s * x + c * y
		);
	}

	// Members
	union { vec_t x; vec_t width; };
	union { vec_t y; vec_t height; };
};

export inline constexpr auto DotProduct(const Vector2D& a, const Vector2D& b)
{
	return (a.x * b.x + a.y * b.y);
}

export template<typename T>
inline constexpr Vector2D operator*(T fl, const Vector2D& v)
{
	return v * fl;
}

export inline auto operator^(const Vector2D& a, const Vector2D& b)
{
	auto length_ab = a.Length() * b.Length();

	if (length_ab < FLT_EPSILON)
		return 0.0;

	return acos(DotProduct(a, b) / length_ab) * (180.0 / M_PI);
}

template<typename T>
inline constexpr auto Q_abs(const T& v)
{
	return v > static_cast<T>(0) ? v : -v;
}

// 3D Vector
// Same data-layout as engine's vec3_t, which is a vec_t[3]
export struct Vector
{
	// Construction
	constexpr Vector(Vector&& s) = default;
	Vector& operator=(const Vector& s) = default;
	Vector& operator=(Vector&& s) = default;
	constexpr Vector() : x(0), y(0), z(0) {}
	template<typename TX, typename TY, typename TZ> constexpr Vector(TX X, TY Y, TZ Z) : x(static_cast<vec_t>(X)), y(static_cast<vec_t>(Y)), z(static_cast<vec_t>(Z)) {}
	template<typename TZ> constexpr Vector(const Vector2D& v2d, TZ Z) : x(v2d.x), y(v2d.y), z(static_cast<vec_t>(Z)) {}
	template<typename T> constexpr Vector(const T rgfl[3]) : x(static_cast<vec_t>(rgfl[0])), y(static_cast<vec_t>(rgfl[1])), z(static_cast<vec_t>(rgfl[2])) {}

	// Operators
	constexpr decltype(auto) operator-()       const { return Vector(-x, -y, -z); }
	constexpr bool operator==(const Vector& v) const { return Q_abs(x - v.x) <= FLT_EPSILON && Q_abs(y - v.y) <= FLT_EPSILON && Q_abs(z - v.z) <= FLT_EPSILON; }
	constexpr bool operator!=(const Vector& v) const { return !(*this == v); }

	constexpr decltype(auto) operator=(std::nullptr_t) { return Vector(0, 0, 0); }

	constexpr decltype(auto) operator+(const Vector& v) const { return Vector(x + v.x, y + v.y, z + v.z); }
	constexpr decltype(auto) operator-(const Vector& v) const { return Vector(x - v.x, y - v.y, z - v.z); }
	constexpr decltype(auto) operator+=(const Vector& v) { return (*this = *this + v); }
	constexpr decltype(auto) operator-=(const Vector& v) { return (*this = *this - v); }

	constexpr decltype(auto) operator*(float fl) const { return Vector(vec_t(x * fl), vec_t(y * fl), vec_t(z * fl)); }
	constexpr decltype(auto) operator/(float fl) const { return Vector(vec_t(x / fl), vec_t(y / fl), vec_t(z / fl)); }
	constexpr decltype(auto) operator*=(float fl) { return (*this = *this * fl); }
	constexpr decltype(auto) operator/=(float fl) { return (*this = *this / fl); }

	// Static methods
	static constexpr decltype(auto) Zero() { return Vector(0, 0, 0); }
	static constexpr decltype(auto) I() { return Vector(1, 0, 0); }
	static constexpr decltype(auto) J() { return Vector(0, 1, 0); }
	static constexpr decltype(auto) K() { return Vector(0, 0, 1); }

	void Clear()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	template<class OutputIter>
	void CopyToIter(OutputIter arr) const
	{
		*arr++ = x;
		*arr++ = y;
		*arr++ = z;
	}

	constexpr float Length() const { return 1.0f / rsqrt(x * x + y * y + z * z); }	// Get the vector's magnitude
	constexpr vec_t LengthSquared() const { return (x * x + y * y + z * z); }	// Get the vector's magnitude squared
	constexpr float Length2D() const { return 1.0f / rsqrt(x * x + y * y); }	// Get the vector's magnitude, but only consider its X and Y component
	constexpr float Length2DSquared() const { return (x * x + y * y); }

	constexpr operator float* () { return &x; } // Vectors will now automatically convert to float * when needed
	constexpr operator const float* () const { return &x; } // Vectors will now automatically convert to float * when needed

	// for out precision normalize
	constexpr Vector Normalize() const
	{
		if (LengthSquared() <= FLT_EPSILON)
			return Zero();

		auto invsqrt = rsqrt(x * x + y * y + z * z);
		return Vector(x * invsqrt, y * invsqrt, z * invsqrt);
	}

	constexpr float NormalizeInPlace()
	{
		if (LengthSquared() <= FLT_EPSILON)
		{
			x = 1;
			y = 0;
			z = 0;

			return 0.0f;
		}

		auto fl = rsqrt(x * x + y * y + z * z);

		x *= fl;
		y *= fl;
		z *= fl;

		return 1.0f / fl;
	}

	template<typename T> constexpr Vector SetLength(T newlen) const
	{
		if (LengthSquared() <= FLT_EPSILON)
			return Zero();

		auto fl = static_cast<float>(newlen) * rsqrt(x * x + y * y + z * z);
		return Vector(x * fl, y * fl, z * fl);
	}

	template<typename T> constexpr void SetLengthInPlace(T newlen)
	{
		if (LengthSquared() <= FLT_EPSILON)
		{
			x = 0;
			y = 0;
			z = 0;

			return;
		}

		auto fl = static_cast<float>(newlen) * rsqrt(x * x + y * y + z * z);

		x *= fl;
		y *= fl;
		z *= fl;
	}

	constexpr Vector2D Make2D() const
	{
		return Vector2D(x, y);
	}

	template<typename T> constexpr bool operator< (T fl) const { return !!(LengthSquared() < (fl * fl)); }
	template<typename T> constexpr bool operator<= (T fl) const { return !!(LengthSquared() <= (fl * fl)); }
	constexpr bool operator< (const Vector& v) const { return !!(LengthSquared() < v.LengthSquared()); }
	constexpr bool operator<= (const Vector& v) const { return !!(LengthSquared() <= v.LengthSquared()); }
	template<typename T> constexpr bool operator> (T fl) const { return !!(LengthSquared() > (fl * fl)); }
	template<typename T> constexpr bool operator>= (T fl) const { return !!(LengthSquared() >= (fl * fl)); }
	constexpr bool operator> (const Vector& v) const { return !!(LengthSquared() > v.LengthSquared()); }
	constexpr bool operator>= (const Vector& v) const { return !!(LengthSquared() >= v.LengthSquared()); }

	constexpr bool IsZero() const
	{
		return (x > -FLT_EPSILON && x < FLT_EPSILON&&
			y > -FLT_EPSILON && y < FLT_EPSILON&&
			z > -FLT_EPSILON && z < FLT_EPSILON);
	}

	explicit constexpr operator bool() const { return !IsZero(); }	// Can be placed in if() now.

	bool IsNaN() const
	{
		return std::isnan(x) || std::isnan(y) || std::isnan(z);
	}

	Vector MakeVector() const
	{
		auto rad_pitch = (pitch * M_PI / 180.0f);
		auto rad_yaw = (yaw * M_PI / 180.0f);
		auto tmp = std::cos(rad_pitch);

		return Vector(
			-tmp * -std::cos(rad_yaw),	// x
			std::sin(rad_yaw) * tmp,	// y
			-std::sin(rad_pitch)		// z
		);
	}

	Vector VectorAngles(void) const
	{
		Vector a;
		a.pitch = 0;
		a.yaw = 0;
		a.roll = 0;

		if (y == 0 && x == 0)
		{
			a.yaw = 0;
			if (z > 0)
				a.pitch = 90;
			else
				a.pitch = 270;
		}
		else
		{
			a.yaw = vec_t(std::atan2(-y, x) * 180.0 / M_PI);
			if (a.yaw < 0)
				a.yaw += 360;

			a.yaw = 360.0f - a.yaw;	// LUNA: why???

			auto tmp = rsqrt(x * x + y * y);
			a.pitch = vec_t(std::atan(z * tmp) * 180.0 / M_PI);
			if (a.pitch < 0)
				a.pitch += 360;
		}

		return a;
	}

	Vector RotateX(float angle) const
	{
		auto a = (angle * M_PI / 180.0);
		auto c = std::cos(a);
		auto s = std::sin(a);

		return Vector(
			x,
			c * y - s * z,
			s * y + c * z
		);
	}

	Vector RotateY(float angle) const
	{
		auto a = (angle * M_PI / 180.0);
		auto c = std::cos(a);
		auto s = std::sin(a);

		return Vector(
			c * x + s * z,
			y,
			-s * x + c * z
		);
	}

	Vector RotateZ(float angle) const
	{
		auto a = (angle * M_PI / 180.0);
		auto c = std::cos(a);
		auto s = std::sin(a);

		return Vector(
			c * x - s * y,
			s * x + c * y,
			z
		);
	}

	// Members
	union { vec_t x; vec_t pitch;	};
	union { vec_t y; vec_t yaw;		};
	union { vec_t z; vec_t roll;	};
};

export inline constexpr Vector operator*(float fl, const Vector& v)
{
	return v * fl;
}

export inline constexpr auto DotProduct(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

export inline constexpr auto DotProduct2D(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y);
}

export inline constexpr auto CrossProduct(const Vector& a, const Vector& b)
{
	return Vector(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

export inline auto operator^(const Vector& a, const Vector& b)
{
	double length_ab = static_cast<double>(a.Length() * b.Length());

	if (Q_abs(length_ab) < DBL_EPSILON)
		return 0.0;

	return std::acos(DotProduct(a, b) / length_ab) * (180.0 / M_PI);
}
