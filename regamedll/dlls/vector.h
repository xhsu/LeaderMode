/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#pragma once

#ifdef RGB
#undef RGB
#endif

#include <float.h>

inline constexpr float rsqrt(float x)
{
#ifdef __SSE__
	return _mm_rsqrt_ps(_mm_set1_ps(x))[0];
#else
	/*
	* Reference:
	* Walczyk, C.J.; Moroz, L.V.; Cie�sli �nski, J.L. Improving the Accuracy of the Fast Inverse Square Root by Modifying Newton�Raphson
	Corrections. Entropy 2021, 23, 86.
	*/

	float xhalf = 0.5f * x;
	int i = *(int*)&x;
	i = 0x5f376908 - (i >> 1);

	float y = *(float*)&i;
	y = y * (1.50087896f - xhalf * y * y);
	y = y * (1.50000057f - xhalf * y * y);	// Second iteration.

	return y;
#endif
}

// Used for many pathfinding and many other operations that are treated as planar rather than 3D.
class Vector2D
{
public:
	// Construction/destruction
	constexpr Vector2D(Vector2D&& s) = default;
	Vector2D& operator=(const Vector2D& s) = default;
	Vector2D& operator=(Vector2D&& s) = default;
	constexpr Vector2D() : x(0), y(0) {}
	constexpr Vector2D(float X, float Y) : x(X), y(Y) {}
	constexpr Vector2D(float sideLength) : width(sideLength), height(sideLength) {}
	constexpr Vector2D(const Vector2D &v) : x(v.x), y(v.y) {}
	explicit constexpr Vector2D(const vec_t rgfl[2]) : x(rgfl[0]), y(rgfl[1]) {}

	// Operators
	constexpr decltype(auto) operator-()         const { return Vector2D(-x, -y); }
	constexpr bool operator==(const Vector2D &v) const { return x == v.x && y == v.y; }
	constexpr bool operator!=(const Vector2D &v) const { return !(*this == v); }

	constexpr decltype(auto) operator+(const Vector2D &v)  const { return Vector2D(x + v.x, y + v.y); }
	constexpr decltype(auto) operator-(const Vector2D &v)  const { return Vector2D(x - v.x, y - v.y); }
	constexpr decltype(auto) operator*(const Vector2D &v)  const { return Vector2D(x * v.x, y * v.y); }
	constexpr decltype(auto) operator/(const Vector2D &v)  const { return Vector2D(x / v.x, y / v.y); }

	constexpr decltype(auto) operator+=(const Vector2D &v) { return (*this = *this + v); }
	constexpr decltype(auto) operator-=(const Vector2D &v) { return (*this = *this - v); }
	constexpr decltype(auto) operator*=(const Vector2D &v) { return (*this = *this * v); }
	constexpr decltype(auto) operator/=(const Vector2D &v) { return (*this = *this / v); }

	constexpr decltype(auto) operator*(float fl) const { return Vector2D(vec_t(x * fl), vec_t(y * fl)); }
	constexpr decltype(auto) operator/(float fl) const { return Vector2D(vec_t(x / fl), vec_t(y / fl)); }

	constexpr decltype(auto) operator=(std::nullptr_t) { return Vector2D(0, 0); }
	constexpr decltype(auto) operator*=(float fl) { return (*this = *this * fl); }
	constexpr decltype(auto) operator/=(float fl) { return (*this = *this / fl); }

	// Static methods
	static constexpr decltype(auto) Zero() { return Vector2D(0, 0); }
	static constexpr decltype(auto) I() { return Vector2D(1, 0); }
	static constexpr decltype(auto) J() { return Vector2D(0, 1); }

	// Methods
	inline void Clear() { x = 0; y = 0; }
	template<class OutputIter> inline void CopyToIter(OutputIter arr) const { *arr++ = x; *arr++ = y; }
	real_t Length() const { return Q_sqrt(X() * X() + Y() * Y()); }	// Get the vector's magnitude
	constexpr real_t LengthSquared() const { return (X() * X() + Y() * Y()); }	// Get the vector's magnitude squared
	inline constexpr real_t X() const { return static_cast<real_t>(x); }
	inline constexpr real_t Y() const { return static_cast<real_t>(y); }

	constexpr operator float*()             { return &x; } // Vectors will now automatically convert to float * when needed
	constexpr operator const float*() const { return &x; } // Vectors will now automatically convert to float * when needed

	Vector2D Normalize() const
	{
		real_t flLen = Length();
		if (flLen <= FLT_EPSILON)
			return Vector2D(0, 0);

		flLen = 1.0 / flLen;
		return Vector2D(vec_t(x * flLen), vec_t(y * flLen));
	}

	Vector2D SetLength(float newlen) const
	{
		real_t flLen = Length();
		if (flLen <= FLT_EPSILON)
			return Vector2D(0, 0);

		flLen = newlen / flLen;
		return Vector2D(vec_t(x * flLen), vec_t(y * flLen));
	}

	// LUNA: comparison with Vector3 was moved to the end of this file.
	constexpr bool operator< (real_t fl) const { return !!(LengthSquared() < (fl * fl)); }
	constexpr bool operator<= (real_t fl) const { return !!(LengthSquared() <= (fl * fl)); }
	constexpr bool operator< (const Vector2D& v) const { return !!(LengthSquared() < v.LengthSquared()); }
	constexpr bool operator<= (const Vector2D& v) const { return !!(LengthSquared() <= v.LengthSquared()); }
	constexpr bool operator> (real_t fl) const { return !!(LengthSquared() > (fl * fl)); }
	constexpr bool operator>= (real_t fl) const { return !!(LengthSquared() >= (fl * fl)); }
	constexpr bool operator> (const Vector2D& v) const { return !!(LengthSquared() > v.LengthSquared()); }
	constexpr bool operator>= (const Vector2D& v) const { return !!(LengthSquared() >= v.LengthSquared()); }

	real_t NormalizeInPlace()
	{
		real_t flLen = Length();

		if (flLen > 0.0)
		{
			x = vec_t(1.0 / flLen * x);
			y = vec_t(1.0 / flLen * y);
		}
		else
		{
			x = 1.0;
			y = 0.0;
		}

		return flLen;
	}
	constexpr bool IsZero(vec_t tolerance = FLT_EPSILON) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance);
	}

	// anti-clockwise.
	Vector2D Rotate(float angle) const
	{
		auto a = (angle * M_PI / 180.0);
		auto c = Q_cos(a);
		auto s = Q_sin(a);

		return Vector2D(c * x - s * y,
						s * x + c * y
		);
	}

	// Members
	union { vec_t x; vec_t width;	};
	union { vec_t y; vec_t height;	};
};

inline constexpr real_t DotProduct(const Vector2D &a, const Vector2D &b)
{
	return (a.X() * b.X() + a.Y() * b.Y());
}

inline constexpr Vector2D operator*(float fl, const Vector2D &v)
{
	return v * fl;
}

inline real_t operator^(const Vector2D& a, const Vector2D& b)
{
	real_t length_ab = a.Length() * b.Length();

	if (length_ab == 0.0)
		return 0.0;

	return (real_t)(Q_acos(DotProduct(a, b) / length_ab) * (180.0 / M_PI));
}

// 2x2 Matrix
class Matrix2x2
{
public:
	// Construction/destruction
	constexpr Matrix2x2(Matrix2x2&& s) = default;
	Matrix2x2& operator=(const Matrix2x2& s) = default;
	Matrix2x2& operator=(Matrix2x2&& s) = default;
	constexpr Matrix2x2() : a(1), b(0), c(0), d(1) {}
	constexpr Matrix2x2(float A, float B, float C, float D) : a(A), b(B), c(C), d(D) {}
	constexpr Matrix2x2(const Vector2D& i_hat, const Vector2D& j_hat) : a(i_hat.x), b(j_hat.x), c(i_hat.y), d(j_hat.y) {}
	Matrix2x2(const Matrix2x2& m) { *(int*)&a = *(int*)&m.a; *(int*)&b = *(int*)&m.b; *(int*)&c = *(int*)&m.c; *(int*)&d = *(int*)&m.d;}
	Matrix2x2(const float rgfl[4]) { *(int*)&a = *(int*)&rgfl[0]; *(int*)&b = *(int*)&rgfl[1]; *(int*)&c = *(int*)&rgfl[2]; *(int*)&d = *(int*)&rgfl[3];}
	Matrix2x2(const float rgfl[2][2]) { *(int*)&a = *(int*)&rgfl[0][0]; *(int*)&b = *(int*)&rgfl[0][1]; *(int*)&c = *(int*)&rgfl[1][0]; *(int*)&d = *(int*)&rgfl[1][1]; }
	Matrix2x2(float flAngle)	// by a anti-clockwise rotation.
	{
		auto rad = (flAngle * M_PI / 180.0);
		auto sine = Q_sin(rad);
		auto cosine = Q_cos(rad);

		a = cosine;	b = -sine;
		c = sine;	d = cosine;
	}

	// Operators
	constexpr decltype(auto) operator~()			const { return Matrix2x2(d, -b, -c, a); }	// Inverse matrix
	constexpr bool operator==(const Matrix2x2& m)	const { return a == m.a && b == m.b && c == m.c && d == m.d; }
	constexpr bool operator!=(const Matrix2x2& m)	const { return !(*this == m); }

	constexpr decltype(auto) operator+(const Matrix2x2& m) const { return Matrix2x2(a + m.a, b + m.b, c + m.c, d + m.d); }
	constexpr decltype(auto) operator-(const Matrix2x2& m) const { return Matrix2x2(a - m.a, b - m.b, c - m.c, d - m.d); }
	constexpr decltype(auto) operator*(const Matrix2x2& m) const { return Matrix2x2(a * m.a + b * m.c, a * m.b + b * m.d, c * m.a + d * m.c, c * m.b + d * m.d); }

	constexpr decltype(auto) operator+=(const Matrix2x2& m) { return (*this = *this + m); }
	constexpr decltype(auto) operator-=(const Matrix2x2& m) { return (*this = *this - m); }
	constexpr decltype(auto) operator*=(const Matrix2x2& m) { return (*this = *this * m); }

	constexpr decltype(auto) operator=(std::nullptr_t) { return Matrix2x2(0, 0, 0, 0); }
	constexpr decltype(auto) operator*(float fl) const { return Matrix2x2(a * fl, b * fl, c * fl, d * fl); }
	constexpr decltype(auto) operator/(float fl) const { return Matrix2x2(a / fl, b / fl, c / fl, d / fl); }

	constexpr decltype(auto) operator*=(float fl) { return (*this = *this * fl); }
	constexpr decltype(auto) operator/=(float fl) { return (*this = *this / fl); }

	constexpr operator float* () { return &a; }
	constexpr operator const float* () const { return &a; }

	// Methods
	void Clear() { a = 0; b = 0; c = 0; d = 0; }
	static constexpr decltype(auto) Identity()	{ return Matrix2x2(1, 0, 0, 1); }
	static constexpr decltype(auto) Zero()		{ return Matrix2x2(0, 0, 0, 0); }

	// Members
	vec_t a{ 1 }, b{ 0 }, c{ 0 }, d{ 1 };	// Started with identity matrix.
};

inline constexpr Matrix2x2 operator*(float fl, const Matrix2x2& m)
{
	return m * fl;
}

inline constexpr Vector2D operator*(const Matrix2x2& m, const Vector2D& v)
{
	/*
	| a  b | | x |   | ax + by |
	|      | |   | = |         |
	| c  d | | y |   | cx + dy |
	*/

	return Vector2D(
		m.a * v.x + m.b * v.y,
		m.c * v.x + m.d * v.y
	);
}

// 3x3 Matrix
class Matrix3x3
{
public:
	// Construction/destruction
	constexpr Matrix3x3() noexcept {}
	constexpr Matrix3x3(const Matrix3x3& s) = default;
	constexpr Matrix3x3(Matrix3x3&& s) = default;
	Matrix3x3& operator=(const Matrix3x3& s) = default;
	Matrix3x3& operator=(Matrix3x3&& s) = default;
	//~Matrix3x3() {}

	Matrix3x3(const float rgfl[3][3]) { memcpy(&dat, &rgfl, sizeof(dat)); }
	constexpr Matrix3x3(float a, float b, float c, float d, float e, float f, float g, float h, float i)
	{
		dat[0][0] = a; dat[0][1] = b; dat[0][2] = c;
		dat[1][0] = d; dat[1][1] = e; dat[1][2] = f;
		dat[2][0] = g; dat[2][1] = h; dat[2][2] = i;
	}

	// Static Methods
	static constexpr decltype(auto) Identity()
	{
		return Matrix3x3(
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		);
	}
	static constexpr decltype(auto) Zero()
	{
		return Matrix3x3(
			0, 0, 0,
			0, 0, 0,
			0, 0, 0
		);
	}
	static constexpr decltype(auto) IJ(const Vector2D& i, const Vector2D& j)
	{
		return Matrix3x3(
			i.x, j.x, 0,
			i.y, j.y, 0,
			0,   0,   1
		);
	}
	static decltype(auto) Rotation2D(float flAngle)
	{
		auto rad = (flAngle * M_PI / 180.0);
		auto sine = Q_sin(rad);
		auto cosine = Q_cos(rad);

		return Matrix3x3(
			cosine,	-sine,	0,
			sine,	cosine,	0,
			0,		0,		1
		);
	}
	static constexpr decltype(auto) Translation2D(const Vector2D& v)
	{
		return Matrix3x3(
			1, 0, v.x,
			0, 1, v.y,
			0, 0, 1
		);
	}
	static constexpr decltype(auto) Translation2D(float x, float y)
	{
		return Matrix3x3(
			1, 0, x,
			0, 1, y,
			0, 0, 1
		);
	}
	static constexpr decltype(auto) Stretch2D(float x, float y)
	{
		return Matrix3x3(
			x, 0, 0,
			0, y, 0,
			0, 0, 1
		);
	}
	static constexpr decltype(auto) Stretch2D(float k)
	{
		return Matrix3x3(
			k, 0, 0,
			0, k, 0,
			0, 0, 1
		);
	}
	static constexpr decltype(auto) Squeeze2D(float x, float y)
	{
		return Matrix3x3(
			1.0f / x,	0,			0,
			0,			1.0f / y,	0,
			0,			0,			1
		);
	}
	static constexpr decltype(auto) Squeeze2D(float k)
	{
		return Matrix3x3(
			1.0f / k,	0,			0,
			0,			1.0f / k,	0,
			0,			0,			1
		);
	}

	// Operators
	constexpr decltype(auto) operator~() const	// Inverse matrix
	{
		vec_t invdet = 1.0f / Det();

		Matrix3x3 mxInv;
		mxInv.dat[0][0] = (dat[1][1] * dat[2][2] - dat[2][1] * dat[1][2]) * invdet;
		mxInv.dat[0][1] = (dat[0][2] * dat[2][1] - dat[0][1] * dat[2][2]) * invdet;
		mxInv.dat[0][2] = (dat[0][1] * dat[1][2] - dat[0][2] * dat[1][1]) * invdet;
		mxInv.dat[1][0] = (dat[1][2] * dat[2][0] - dat[1][0] * dat[2][2]) * invdet;
		mxInv.dat[1][1] = (dat[0][0] * dat[2][2] - dat[0][2] * dat[2][0]) * invdet;
		mxInv.dat[1][2] = (dat[1][0] * dat[0][2] - dat[0][0] * dat[1][2]) * invdet;
		mxInv.dat[2][0] = (dat[1][0] * dat[2][1] - dat[2][0] * dat[1][1]) * invdet;
		mxInv.dat[2][1] = (dat[2][0] * dat[0][1] - dat[0][0] * dat[2][1]) * invdet;
		mxInv.dat[2][2] = (dat[0][0] * dat[1][1] - dat[1][0] * dat[0][1]) * invdet;

		return mxInv;
	}

	bool operator==(const Matrix3x3& m) const { return !memcmp(&dat, &m.dat, sizeof(dat)); }
	bool operator!=(const Matrix3x3& m) const { return !(*this == m); }

	constexpr decltype(auto) operator+(const Matrix3x3& m) const
	{
		Matrix3x3 mx;

		for (size_t i = 0; i < 3; i++)
		{
			mx.dat[i][0] = dat[i][0] + m.dat[i][0];
			mx.dat[i][1] = dat[i][1] + m.dat[i][1];
			mx.dat[i][2] = dat[i][2] + m.dat[i][2];
		}

		return mx;
	}
	constexpr decltype(auto) operator-(const Matrix3x3& m) const
	{
		Matrix3x3 mx;

		for (size_t i = 0; i < 3; i++)
		{
			mx.dat[i][0] = dat[i][0] - m.dat[i][0];
			mx.dat[i][1] = dat[i][1] - m.dat[i][1];
			mx.dat[i][2] = dat[i][2] - m.dat[i][2];
		}

		return mx;
	}
	constexpr decltype(auto) operator*(const Matrix3x3& m) const
	{
		Matrix3x3 mx;

		mx.dat[0][0] = dat[0][0] * m.dat[0][0] + dat[0][1] * m.dat[1][0] + dat[0][2] * m.dat[2][0];
		mx.dat[0][1] = dat[0][0] * m.dat[0][1] + dat[0][1] * m.dat[1][1] + dat[0][2] * m.dat[2][1];
		mx.dat[0][2] = dat[0][0] * m.dat[0][2] + dat[0][1] * m.dat[1][2] + dat[0][2] * m.dat[2][2];
		mx.dat[1][0] = dat[1][0] * m.dat[0][0] + dat[1][1] * m.dat[1][0] + dat[1][2] * m.dat[2][0];
		mx.dat[1][1] = dat[1][0] * m.dat[0][1] + dat[1][1] * m.dat[1][1] + dat[1][2] * m.dat[2][1];
		mx.dat[1][2] = dat[1][0] * m.dat[0][2] + dat[1][1] * m.dat[1][2] + dat[1][2] * m.dat[2][2];
		mx.dat[2][0] = dat[2][0] * m.dat[0][0] + dat[2][1] * m.dat[1][0] + dat[2][2] * m.dat[2][0];
		mx.dat[2][1] = dat[2][0] * m.dat[0][1] + dat[2][1] * m.dat[1][1] + dat[2][2] * m.dat[2][1];
		mx.dat[2][2] = dat[2][0] * m.dat[0][2] + dat[2][1] * m.dat[1][2] + dat[2][2] * m.dat[2][2];

		return mx;
	}

	constexpr decltype(auto) operator+=(const Matrix3x3& m) { return (*this = *this + m); }
	constexpr decltype(auto) operator-=(const Matrix3x3& m) { return (*this = *this - m); }
	constexpr decltype(auto) operator*=(const Matrix3x3& m) { return (*this = *this * m); }

	constexpr decltype(auto) operator=(std::nullptr_t) { return Zero(); }
	constexpr decltype(auto) operator*(float fl) const
	{
		Matrix3x3 mx;

		for (size_t i = 0; i < 3; i++)
		{
			mx.dat[i][0] = dat[i][0] * fl;
			mx.dat[i][1] = dat[i][1] * fl;
			mx.dat[i][2] = dat[i][2] * fl;
		}

		return mx;
	}
	constexpr decltype(auto) operator/(float fl) const
	{
		Matrix3x3 mx;

		for (size_t i = 0; i < 3; i++)
		{
			mx.dat[i][0] = dat[i][0] / fl;
			mx.dat[i][1] = dat[i][1] / fl;
			mx.dat[i][2] = dat[i][2] / fl;
		}

		return mx;
	}

	constexpr decltype(auto) operator*=(float fl) { return (*this = *this * fl); }
	constexpr decltype(auto) operator/=(float fl) { return (*this = *this / fl); }

	constexpr operator float* () { return &dat[0][0]; }
	constexpr operator const float* () const { return &dat[0][0]; }

	// Methods
	constexpr vec_t Det(void) const
	{
		return	dat[0][0] * (dat[1][1] * dat[2][2] - dat[2][1] * dat[1][2]) -
				dat[0][1] * (dat[1][0] * dat[2][2] - dat[1][2] * dat[2][0]) +
				dat[0][2] * (dat[1][0] * dat[2][1] - dat[1][1] * dat[2][0]);
	}

	// Members
	vec_t dat[3][3]
	{
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	};
};

inline constexpr Matrix3x3 operator*(float fl, const Matrix3x3& m)
{
	return m * fl;
}

inline constexpr Vector2D operator*(const Matrix3x3& m, const Vector2D& v)
{
	/*
	| a  b  u | | x |   | ax + by + u |
	| c  d  v | | y | = | cx + dy + v |
	| 0  0  1 | | 1 |   |      1      |

	For the practice sake, we just ignore the additional [1] from the last row.
	*/

	return Vector2D(
		m.dat[0][0] * v.x + m.dat[0][1] * v.y + m.dat[0][2],
		m.dat[1][0] * v.x + m.dat[1][1] * v.y + m.dat[1][2]
	);
}

inline constexpr float Q_fabs2(float f1)	// constexpr version of Q_fabs
{
	return f1 > 0.0 ? f1 : -f1;
}

// 3D Vector
// Same data-layout as engine's vec3_t, which is a vec_t[3]
class Vector
{
public:
	// Construction/destruction
	constexpr Vector(Vector&& s) = default;
	Vector& operator=(const Vector& s) = default;
	Vector& operator=(Vector&& s) = default;
	constexpr Vector() : x(0), y(0), z(0) {}
	constexpr Vector(vec_t X, vec_t Y, vec_t Z) : x(X), y(Y), z(Z) {}
	constexpr Vector(const Vector2D& v2d, vec_t Z) : x(v2d.x), y(v2d.y), z(Z) {}
	constexpr Vector(const Vector &v) : x(v.x), y(v.y), z(v.z) {}
	constexpr Vector(const vec_t rgfl[3]) : x(rgfl[0]), y(rgfl[1]), z(rgfl[2]) {}
	explicit constexpr Vector(unsigned long ulRGB) : x(((ulRGB & 0xFF0000) >> 16) / 255.0), y(((ulRGB & 0xFF00) >> 8) / 255.0), z((ulRGB & 0xFF) / 255.0) {}

	// Operators
	constexpr decltype(auto) operator-()       const { return Vector(-x, -y, -z); }
	constexpr bool operator==(const Vector &v) const { return Q_fabs2(x - v.x) <= FLT_EPSILON && Q_fabs2(y - v.y) <= FLT_EPSILON && Q_fabs2(z - v.z) <= FLT_EPSILON; }
	constexpr bool operator!=(const Vector &v) const { return !(*this == v); }

	constexpr decltype(auto) operator=(std::nullptr_t) { return Vector(0, 0, 0); }

	constexpr decltype(auto) operator+(const Vector &v) const { return Vector(x + v.x, y + v.y, z + v.z); }
	constexpr decltype(auto) operator-(const Vector &v) const { return Vector(x - v.x, y - v.y, z - v.z); }
	constexpr decltype(auto) operator+=(const Vector &v) { return (*this = *this + v); }
	constexpr decltype(auto) operator-=(const Vector &v) { return (*this = *this - v); }

	constexpr decltype(auto) operator*(float fl) const { return Vector(vec_t(x * fl), vec_t(y * fl), vec_t(z * fl)); }
	constexpr decltype(auto) operator/(float fl) const { return Vector(vec_t(x / fl), vec_t(y / fl), vec_t(z / fl)); }
	constexpr decltype(auto) operator*=(float fl) { return (*this = *this * fl); }
	constexpr decltype(auto) operator/=(float fl) { return (*this = *this / fl); }

	// Static methods
	static constexpr decltype(auto) Zero() { return Vector(0, 0, 0); }
	static constexpr decltype(auto) I() { return Vector(1, 0, 0); }
	static constexpr decltype(auto) J() { return Vector(0, 1, 0); }
	static constexpr decltype(auto) K() { return Vector(0, 0, 1); }

	// Methods
	inline constexpr real_t X() const { return static_cast<real_t>(x); }
	inline constexpr real_t Y() const { return static_cast<real_t>(y); }
	inline constexpr real_t Z() const { return static_cast<real_t>(z); }

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

	real_t Length() const { return Q_sqrt(X() * X() + Y() * Y() + Z() * Z()); }	// Get the vector's magnitude
	constexpr real_t LengthSquared() const { return (X() * X() + Y() * Y() + Z() * Z()); }	// Get the vector's magnitude squared
	real_t Length2D() const { return Q_sqrt(X() * X() + Y() * Y()); }	// Get the vector's magnitude, but only consider its X and Y component
	constexpr real_t Length2DSquared() const { return (X() * X() + Y() * Y()); }

	constexpr operator float*()             { return &x; } // Vectors will now automatically convert to float * when needed
	constexpr operator const float*() const { return &x; } // Vectors will now automatically convert to float * when needed

	// for out precision normalize
	Vector Normalize() const
	{
		real_t flLen = Length();
		if (Q_fabs(flLen) <= FLT_EPSILON || std::isnan(flLen))
			return Vector(0, 0, 1);

		flLen = 1.0 / flLen;
		return Vector(vec_t(x * flLen), vec_t(y * flLen), vec_t(z * flLen));
	}

	constexpr Vector2D Make2D() const
	{
		Vector2D Vec2;
		*(int *)&Vec2.x = *(int *)&x;
		*(int *)&Vec2.y = *(int *)&y;
		return Vec2;
	}

	constexpr bool operator< (real_t fl) const { return !!(LengthSquared() < fl * fl); }
	constexpr bool operator<= (real_t fl) const { return !!(LengthSquared() <= fl * fl); }
	constexpr bool operator< (const Vector& v) const { return !!(LengthSquared() < v.LengthSquared()); }
	constexpr bool operator<= (const Vector& v) const { return !!(LengthSquared() <= v.LengthSquared()); }
	constexpr bool operator> (real_t fl) const { return !!(LengthSquared() > fl * fl); }
	constexpr bool operator>= (real_t fl) const { return !!(LengthSquared() >= fl * fl); }
	constexpr bool operator> (const Vector& v) const { return !!(LengthSquared() > v.LengthSquared()); }
	constexpr bool operator>= (const Vector& v) const { return !!(LengthSquared() >= v.LengthSquared()); }

	template<typename T = real_t>
	constexpr T NormalizeInPlace()
	{
		T flLen = Length();

		if (Q_fabs(flLen) > FLT_EPSILON && !std::isnan(flLen))
		{
			x = vec_t(1.0 / flLen * x);
			y = vec_t(1.0 / flLen * y);
			z = vec_t(1.0 / flLen * z);
		}
		else
		{
			x = 0;
			y = 0;
			z = 1;
		}

		return flLen;
	}

	constexpr bool IsZero() const
	{
		return (x > -FLT_EPSILON && x < FLT_EPSILON &&
			y > -FLT_EPSILON && y < FLT_EPSILON &&
			z > -FLT_EPSILON && z < FLT_EPSILON);
	}

	bool IsNaN() const
	{
		return std::isnan(x) || std::isnan(y) || std::isnan(z);
	}

	Vector MakeVector() const
	{
		auto rad_pitch = (pitch * M_PI / 180.0f);
		auto rad_yaw = (yaw * M_PI / 180.0f);
		auto tmp = Q_cos(rad_pitch);

		return Vector(	vec_t(-tmp * -Q_cos(rad_yaw)),	// x
						vec_t(Q_sin(rad_yaw) * tmp),	// y
						vec_t(-Q_sin(rad_pitch))		// z
		);
	}

	constexpr Vector VectorAngles(void) const
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
			a.yaw = vec_t(Q_atan2(-y, x) * 180.0 / M_PI);
			if (a.yaw < 0)
				a.yaw += 360;

			a.yaw = 360.0f - a.yaw;	// LUNA: why???

			auto tmp = Q_sqrt(x * x + y * y);
			a.pitch = vec_t(Q_atan2(z, tmp) * 180.0 / M_PI);
			if (a.pitch < 0)
				a.pitch += 360;
		}

		return a;
	}

	Vector RotateX(float angle) const
	{
		auto a = (angle * M_PI / 180.0);
		auto c = Q_cos(a);
		auto s = Q_sin(a);

		return Vector(	x,
						c * y - s * z,
						s * y + c * z
		);
	}

	Vector RotateY(float angle) const
	{
		auto a = (angle * M_PI / 180.0);
		auto c = Q_cos(a);
		auto s = Q_sin(a);

		return Vector(	c * x + s * z,
						y,
						-s * x + c * z
		);
	}

	Vector RotateZ(float angle) const
	{
		auto a = (angle * M_PI / 180.0);
		auto c = Q_cos(a);
		auto s = Q_sin(a);

		return Vector(	c * x - s * y,
						s * x + c * y,
						z
		);
	}

	constexpr Vector HSV() const
	{
		Vector	out;

		double min = r < g ? r : g;
		min = min < b ? min : b;

		double max = r > g ? r : g;
		max = max > b ? max : b;

		out.v = max;	// v
		double delta = max - min;
		if (delta < DBL_EPSILON)
		{
			out.s = 0;
			out.h = NAN;	// undefined
			return out;
		}

		if (max > 0.0)	// NOTE: if Max is == 0, this divide would cause a crash
		{
			out.s = (delta / max);	// s
		}
		else
		{
			// if max is 0, then r = g = b = 0              
			// s = 0, h is undefined
			out.s = 0.0;
			out.h = NAN;	// its now undefined
			return out;
		}

		if (r >= max)					// > is bogus, just keeps compilor happy
			out.h = (g - b) / delta;	// between yellow & magenta
		else if (g >= max)
			out.h = 2.0 + (b - r) / delta;	// between cyan & yellow
		else
			out.h = 4.0 + (r - g) / delta;	// between magenta & cyan

		out.h *= 60.0;	// degrees

		if (out.h < 0.0)
			out.h += 360.0;

		return out;
	}


	constexpr Vector RGB()	const
	{
		Vector	out;

		if (s <= 0.0)	// < is bogus, just shuts up warnings
		{
			out.r = v;
			out.g = v;
			out.b = v;
			return out;
		}

		double hh = h;
		if (hh >= 360.0)
			hh = 0.0;
		hh /= 60.0;

		long i = (long)hh;
		double ff = hh - i;
		double p = v * (1.0 - s);
		double q = v * (1.0 - (s * ff));
		double t = v * (1.0 - (s * (1.0 - ff)));

		switch (i) {
		case 0:
			out.r = v;
			out.g = t;
			out.b = p;
			break;
		case 1:
			out.r = q;
			out.g = v;
			out.b = p;
			break;
		case 2:
			out.r = p;
			out.g = v;
			out.b = t;
			break;

		case 3:
			out.r = p;
			out.g = q;
			out.b = v;
			break;
		case 4:
			out.r = t;
			out.g = p;
			out.b = v;
			break;
		case 5:
		default:
			out.r = v;
			out.g = p;
			out.b = q;
			break;
		}

		return out;
	}

	// Members
	union { vec_t x; vec_t pitch;	vec_t r;	vec_t h; };
	union { vec_t y; vec_t yaw;		vec_t g;	vec_t s; };
	union { vec_t z; vec_t roll;	vec_t b;	vec_t v; };
};

inline constexpr Vector operator*(float fl, const Vector &v)
{
	return v * fl;
}

inline constexpr real_t DotProduct(const Vector &a, const Vector &b)
{
	return (a.X() * b.X() + a.Y() * b.Y() + a.Z() * b.Z());
}

inline constexpr real_t DotProduct2D(const Vector& a, const Vector& b)
{
	return (a.X() * b.X() + a.Y() * b.Y());
}

inline constexpr real_t DotProduct2D(const Vector2D &a, const Vector2D &b)
{
	return (a.X() * b.X() + a.Y() * b.Y());
}

inline constexpr Vector CrossProduct(const Vector &a, const Vector &b)
{
	return Vector(
		a.Y() * b.Z() - a.Z() * b.Y(),
		a.Z() * b.X() - a.X() * b.Z(),
		a.X() * b.Y() - a.Y() * b.X()
	);
}

inline real_t operator^(const Vector& a, const Vector& b)
{
	real_t length_ab = a.Length() * b.Length();

	if (length_ab == 0.0)
		return 0.0;

	return (real_t)(Q_acos(DotProduct(a, b) / length_ab) * (180.0 / M_PI));
}

class Quaternion
{
	using qtn_t = double;

public:
	constexpr Quaternion(Quaternion&& s) = default;
	Quaternion& operator=(const Quaternion& s) = default;
	Quaternion& operator=(Quaternion&& s) = default;
	constexpr Quaternion() : a(1), b(0), c(0), d(0) {}
	constexpr Quaternion(qtn_t A, qtn_t B, qtn_t C, qtn_t D) : a(A), b(B), c(C), d(D) {}

	// Static member
	static inline constexpr decltype(auto) Zero() { return Quaternion(0, 0, 0, 0); }
	static inline constexpr decltype(auto) Identity() { return Quaternion(1, 0, 0, 0); }
	static inline decltype(auto) VA(const Vector& v, qtn_t angle_in_degree)
	{
		auto angle = angle_in_degree * M_PI / 180.0;
		auto cosine = Q_cos(0.5 * angle);
		auto sine = Q_sin(0.5 * angle);

		return Quaternion(cosine, v.x * sine, v.y * sine, v.z * sine);
	}

	// Properties
	inline constexpr decltype(auto) Norm() const { return 1.0 / rsqrt(a * a + b * b + c * c + d * d); }
	inline constexpr decltype(auto) Conjugate() const { return Quaternion(a, -b, -c, -d); }
	inline constexpr decltype(auto) Versor() const { return *this / Norm(); }
	inline constexpr decltype(auto) Reciprocal() const { return Conjugate() / (Norm() * Norm()); }
	inline constexpr decltype(auto) Real() const { return a; }
	inline constexpr decltype(auto) Pure() const { return Vector(b, c, d); }

	// Method
	constexpr decltype(auto) operator*(const Quaternion& q) const { return Quaternion(a * q.a - b * q.b - c * q.c - d * q.d, a * q.a + b * q.b + c * q.c - d * q.d, a * q.a - b * q.b + c * q.c + d * q.d, a * q.a + b * q.b - c * q.c + d * q.d); }
	constexpr decltype(auto) operator*=(const Quaternion& q) { return (*this = *this * q); }
	template<typename NumType> constexpr decltype(auto) operator/(NumType x) const { return Quaternion(a / x, b / x, c / x, d / x); }
	template<typename NumType> constexpr decltype(auto) operator/=(NumType x) { return (*this = *this / x); }

	constexpr decltype(auto) operator*(const Vector& v) const { return v + ((CrossProduct(Pure(), v) * a) + CrossProduct(Pure(), CrossProduct(Pure(), v))) * 2.0f; }	// Rotate a vector by this quaternion.

	Vector Euler() const
	{
		Vector vecAngles;

		vecAngles.roll = Q_atan2(2.0 * (a * b + c * d), a * a - b * b - c * c + d * d);
		vecAngles.pitch = -asin(2.0 * (b * d - a * c));
		vecAngles.yaw = Q_atan2(2.0 * (a * d + b * c), a * a + b * b - c * c - d * d);

		// Rad to Deg
		vecAngles *= 180.0 / M_PI;

		return vecAngles;
	}

	inline constexpr Matrix3x3 M3x3() const
	{
		return Matrix3x3	(a * a + b * b - c * c - d * d,	2.0 * (b * c - a * d),			2.0 * (b * d + a * c),
							2.0 * (b * c + a * d),			a * a - b * b + c * c - d * d,	2.0 * (c * d - a * b),
							2.0 * (b * d - a * c),			2.0 * (c * d + a * b),			a * a - b * b - c * c + d * d);
	}

public:	// Members
	qtn_t a, b, c, d;	// w, x, y, z
};
