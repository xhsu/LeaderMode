/*

Created Date: 05 Mar 2020

*/

#pragma once

#include <string>

#define charsmax(x)		(sizeof(x) - 1)
#define wcharsmax(x)	(sizeof(x) / sizeof(wchar_t) - 1)

// some constants from MP
#define TEAM_UNASSIGNED	0
#define TEAM_TERRORIST	1
#define TEAM_CT			2
#define TEAM_SPECTATOR	3

#ifdef VectorSubtract
#undef VectorSubtract
#endif
#ifdef VectorAdd
#undef VectorAdd
#endif
#ifdef VectorCopy
#undef VectorCopy
#endif
#ifdef VectorClear
#undef VectorClear
#endif

#define RANDOM_LONG		(*gEngfuncs.pfnRandomLong)
#define RANDOM_FLOAT	(*gEngfuncs.pfnRandomFloat)

template<class VectorTypeA, class VectorTypeB> auto DotProduct(const VectorTypeA& x, const VectorTypeB& y) -> decltype((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2]) { return ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2]); }
template<class VectorTypeA, class VectorTypeB, class VectorTypeC> void VectorSubtract(const VectorTypeA& a, const VectorTypeB& b, VectorTypeC& c) { (c)[0] = (a)[0] - (b)[0]; (c)[1] = (a)[1] - (b)[1]; (c)[2] = (a)[2] - (b)[2]; }
template<class VectorTypeA, class VectorTypeB, class VectorTypeC> void VectorAdd(const VectorTypeA& a, const VectorTypeB& b, VectorTypeC& c) { (c)[0] = (a)[0] + (b)[0]; (c)[1] = (a)[1] + (b)[1]; (c)[2] = (a)[2] + (b)[2]; }
template<class VectorTypeA, class VectorTypeB> void VectorCopy(const VectorTypeA& a, VectorTypeB& b) { (b)[0] = (a)[0]; (b)[1] = (a)[1]; (b)[2] = (a)[2]; }
template<class VectorTypeA> void VectorClear(VectorTypeA& a) { a[0] = 0.0; a[1] = 0.0; a[2] = 0.0; }
template<class VectorTypeA> auto VectorLength(const VectorTypeA& a) -> decltype(sqrt(DotProduct(a, a))) { return sqrt(DotProduct(a, a)); }
template<class VectorTypeA, class ScaleType, class VectorTypeB, class VectorTypeC> void VectorMA(const VectorTypeA& a, ScaleType scale, const VectorTypeB& b, VectorTypeC& c) { ((c)[0] = (a)[0] + (scale) * (b)[0], (c)[1] = (a)[1] + (scale) * (b)[1], (c)[2] = (a)[2] + (scale) * (b)[2]); }
template<class VectorTypeA, class ScaleType, class VectorTypeB> void VectorScale(const VectorTypeA& in, ScaleType scale, VectorTypeB& out) { ((out)[0] = (in)[0] * (scale), (out)[1] = (in)[1] * (scale), (out)[2] = (in)[2] * (scale)); }
template<class VectorTypeA> void VectorInverse(VectorTypeA& x) { ((x)[0] = -(x)[0], (x)[1] = -(x)[1], (x)[2] = -(x)[2]); }
template<class VectorTypeA> void AngleVectors(const VectorTypeA& vecAngles, float* forward, float* right, float* up) { return gEngfuncs.pfnAngleVectors(vecAngles, forward, right, up); }

inline float CVAR_GET_FLOAT(const char* x) { return gEngfuncs.pfnGetCvarFloat((char*)x); }
inline char* CVAR_GET_STRING(const char* x) { return gEngfuncs.pfnGetCvarString((char*)x); }
inline cvar_s* CVAR_CREATE(const char* cv, const char* val, const int flags) { return gEngfuncs.pfnRegisterVariable((char*)cv, (char*)val, flags); }

// StudioModelRenderer utils.
void ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]);
void QuaternionMatrix(vec4_t quaternion, float (*matrix)[4]);
inline void MatrixCopy(float in[3][4], float out[3][4]) { Q_memcpy(out, in, sizeof(float) * 3 * 4); }
void VectorTransform(const float* in1, float in2[3][4], float* out);
void QuaternionSlerp(vec4_t p, vec4_t q, float t, vec4_t qt);
void AngleQuaternion(float* angles, vec4_t quaternion);

// 2x2 matrics and Vector2D
void BuildMatrix2x2ByIJ(const vec_t* i_hat, const vec_t* j_hat, vec_t(*output)[2]);
void Matrix2x2Composition(const vec_t **left, const vec_t **right, vec_t(*output)[2]);

inline void UnpackRGB(int& r, int& g, int& b, unsigned long ulRGB)
{
	r = (ulRGB & 0xFF0000) >> 16;
	g = (ulRGB & 0xFF00) >> 8;
	b = ulRGB & 0xFF;
}

inline void ScaleColors(int& r, int& g, int& b, int a)
{
	float x = (float)a / 255;

	r = (int)(r * x);
	g = (int)(g * x);
	b = (int)(b * x);
}

void UTIL_StringToVector(float* pVector, const char* pString);
int UTIL_FindEntityInMap(const char* name, float* origin, float* angle);
extern Vector g_ColorBlue;
extern Vector g_ColorRed;
extern Vector g_ColorGreen;
extern Vector g_ColorYellow;
extern Vector g_ColorGrey;
float* GetClientColor(int clientIndex);
hSprite LoadSprite(const char* pszName);
bool CalcScreen(Vector& in, Vector2D& out);	// a.k.a. WorldToScreen
void UTIL_TraceLine(Vector& vecSrc, Vector& vecEnd, int traceFlags, int ignore_pe, struct pmtrace_s* ptr, int index, int hull = 2);
std::wstring UTIL_ArabicToRoman(unsigned value);
const wchar_t* UTIL_GetLocalisation(const char* szToken);	// safe version of ILocalize::Find().
void UTIL_ReplaceAll(std::string& str, const std::string& from, const std::string& to);
void UTIL_ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to);
//inline bool caseInsCharCompare(char a, char b) { return(toupper(a) == toupper(b)); }
inline bool caseInsCharCompare(wchar_t a, wchar_t b) { return(towupper(a) == towupper(b)); }
//template<typename stringTy> inline bool caseInsCharCompare(stringTy a, stringTy b) { return(std::toupper(a) == std::toupper(b)); }
template<typename stringTy> inline bool UTIL_CaseInsensitiveCompare(const stringTy& s1, const stringTy& s2) { return ((s1.size() == s2.size()) && std::equal(s1.cbegin(), s1.cend(), s2.cbegin(), caseInsCharCompare)); }
void Sys_Error(const char* fmt, ...);

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	auto size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'

	if (size <= 0)
		throw std::runtime_error("Error during formatting.");

	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
