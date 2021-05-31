/*

Created Date: 07 Mar 2020

*/

#include "precompiled.h"

void ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
}

void QuaternionMatrix(vec4_t quaternion, float (*matrix)[4])
{
	matrix[0][0] = 1.0 - 2.0 * quaternion[1] * quaternion[1] - 2.0 * quaternion[2] * quaternion[2];
	matrix[1][0] = 2.0 * quaternion[0] * quaternion[1] + 2.0 * quaternion[3] * quaternion[2];
	matrix[2][0] = 2.0 * quaternion[0] * quaternion[2] - 2.0 * quaternion[3] * quaternion[1];

	matrix[0][1] = 2.0 * quaternion[0] * quaternion[1] - 2.0 * quaternion[3] * quaternion[2];
	matrix[1][1] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[2] * quaternion[2];
	matrix[2][1] = 2.0 * quaternion[1] * quaternion[2] + 2.0 * quaternion[3] * quaternion[0];

	matrix[0][2] = 2.0 * quaternion[0] * quaternion[2] + 2.0 * quaternion[3] * quaternion[1];
	matrix[1][2] = 2.0 * quaternion[1] * quaternion[2] - 2.0 * quaternion[3] * quaternion[0];
	matrix[2][2] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[1] * quaternion[1];
}

void VectorTransform(const float* in1, float in2[3][4], float* out)
{
	out[0] = DotProduct(in1, in2[0]) + in2[0][3];
	out[1] = DotProduct(in1, in2[1]) + in2[1][3];
	out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}

void QuaternionSlerp(vec4_t p, vec4_t q, float t, vec4_t qt)
{
	int i;
	float omega, cosom, sinom, sclp, sclq;

	float a = 0;
	float b = 0;

	for (i = 0; i < 4; i++)
	{
		a += (p[i] - q[i]) * (p[i] - q[i]);
		b += (p[i] + q[i]) * (p[i] + q[i]);
	}

	if (a > b)
	{
		for (i = 0; i < 4; i++)
			q[i] = -q[i];
	}

	cosom = p[0] * q[0] + p[1] * q[1] + p[2] * q[2] + p[3] * q[3];

	if ((1.0 + cosom) > 0.000001)
	{
		if ((1.0 - cosom) > 0.000001)
		{
			omega = acos(cosom);
			sinom = sin(omega);
			sclp = sin((1.0 - t) * omega) / sinom;
			sclq = sin(t * omega) / sinom;
		}
		else
		{
			sclp = 1.0 - t;
			sclq = t;
		}

		for (i = 0; i < 4; i++)
			qt[i] = sclp * p[i] + sclq * q[i];
	}
	else
	{
		qt[0] = -q[1];
		qt[1] = q[0];
		qt[2] = -q[3];
		qt[3] = q[2];
		sclp = sin((1.0 - t) * (0.5 * M_PI));
		sclq = sin(t * (0.5 * M_PI));

		for (i = 0; i < 3; i++)
			qt[i] = sclp * p[i] + sclq * qt[i];
	}
}

void AngleQuaternion(float* angles, vec4_t quaternion)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[2] * 0.5;
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * 0.5;
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * 0.5;
	sr = sin(angle);
	cr = cos(angle);

	quaternion[0] = sr * cp * cy - cr * sp * sy;
	quaternion[1] = cr * sp * cy + sr * cp * sy;
	quaternion[2] = cr * cp * sy - sr * sp * cy;
	quaternion[3] = cr * cp * cy + sr * sp * sy;
}

void BuildMatrix2x2ByIJ(const vec_t* i_hat, const vec_t* j_hat, vec_t(*output)[2])
{
	output[0][0] = i_hat[0];
	output[0][1] = j_hat[0];
	output[1][0] = i_hat[1];
	output[1][1] = j_hat[1];
}

void Matrix2x2Composition(const vec_t** left, const vec_t** right, vec_t(*output)[2])
{
	output[0][0] = left[0][0] * right[0][0] + left[0][1] * right[1][0];
	output[0][1] = left[0][0] * right[0][1] + left[0][1] * right[1][1];
	output[1][0] = left[1][0] * right[0][0] + left[1][1] * right[1][0];
	output[1][1] = left[1][0] * right[0][1] + left[1][1] * right[1][1];
}

void UTIL_StringToVector(float* pVector, const char* pString)
{
	char* pstr, * pfront, tempString[128];
	int j;

	Q_strlcpy(tempString, pString);
	pstr = pfront = tempString;

	for (j = 0; j < 3; j++)
	{
		pVector[j] = Q_atof(pfront);

		while (*pstr && *pstr != ' ')
			pstr++;

		if (!*pstr)
			break;

		pstr++;
		pfront = pstr;
	}

	if (j < 2)
	{
		for (j = j + 1; j < 3; j++)
			pVector[j] = 0;
	}
}

int UTIL_FindEntityInMap(const char* name, float* origin, float* angle)
{
	int n, found = 0;
	char keyname[256];
	char token[1024];

	cl_entity_t* pEnt = gEngfuncs.GetEntityByIndex(0);

	if (!pEnt)
		return 0;

	if (!pEnt->model)
		return 0;

	char* data = pEnt->model->entities;

	while (data)
	{
		data = gEngfuncs.COM_ParseFile(data, token);

		if ((token[0] == '}') || (token[0] == 0))
			break;

		if (!data)
		{
			gEngfuncs.Con_DPrintf("UTIL_FindEntityInMap: EOF without closing brace\n");
			return 0;
		}

		if (token[0] != '{')
		{
			gEngfuncs.Con_DPrintf("UTIL_FindEntityInMap: expected {\n");
			return 0;
		}

		while (1)
		{
			data = gEngfuncs.COM_ParseFile(data, token);

			if (token[0] == '}')
				break;

			if (!data)
			{
				gEngfuncs.Con_DPrintf("UTIL_FindEntityInMap: EOF without closing brace\n");
				return 0;
			}

			Q_strlcpy(keyname, token);

			n = Q_strlen(keyname);

			while (n && keyname[n - 1] == ' ')
			{
				keyname[n - 1] = 0;
				n--;
			}

			data = gEngfuncs.COM_ParseFile(data, token);

			if (!data)
			{
				gEngfuncs.Con_DPrintf("UTIL_FindEntityInMap: EOF without closing brace\n");
				return 0;
			}

			if (token[0] == '}')
			{
				gEngfuncs.Con_DPrintf("UTIL_FindEntityInMap: closing brace without data");
				return 0;
			}

			if (!Q_stricmp(keyname, "classname"))
			{
				if (!Q_stricmp(token, name))
					found = 1;
			}

			if (!Q_stricmp(keyname, "angle"))
			{
				float y = Q_atof(token);

				if (y >= 0)
				{
					angle[0] = 0.0f;
					angle[1] = y;
				}
				else if ((int)y == -1)
				{
					angle[0] = -90.0f;
					angle[1] = 0.0f;;
				}
				else
				{
					angle[0] = 90.0f;
					angle[1] = 0.0f;
				}

				angle[2] = 0.0f;
			}

			if (!Q_stricmp(keyname, "angles"))
			{
				UTIL_StringToVector(angle, token);
			}

			if (!Q_stricmp(keyname, "origin"))
			{
				UTIL_StringToVector(origin, token);
			}
		}

		if (found)
			return 1;
	}

	return 0;
}

hSprite LoadSprite(const char* pszName)
{
	int i;
	char sz[256];

	if (ScreenWidth < 640)
		i = 320;
	else
		i = 640;

	Q_slprintf(sz, pszName, i);
	return gEngfuncs.pfnSPR_Load(sz);
}

// from view.cpp
extern Vector v_origin, v_angles;

bool CalcScreen(const Vector& in, Vector2D& out)
{
	Vector aim = in - v_origin;
	Vector view = v_angles.MakeVector();

	if ((view ^ aim) > (gHUD::m_flDisplayedFOV / 1.8f))	// LUNA: where did this 1.8 came from?
		return false;

	Vector newaim = aim.RotateZ(-v_angles.yaw);
	Vector tmp = newaim.RotateY(-v_angles.pitch);
	newaim = tmp.RotateX(-v_angles.roll);

	if (gHUD::m_flDisplayedFOV == 0.0f)
		return false;

	// we have to use current FOV instead of target FOV.
	float num = (((ScreenWidth / 2) / newaim[0]) * (120.0 / gHUD::m_flDisplayedFOV - 1.0 / 3.0));
	out[0] = (ScreenWidth / 2) - num * newaim[1];
	out[1] = (ScreenHeight / 2) - num * newaim[2];

	Q_clamp(out[0], 0.0f, float(ScreenWidth));
	Q_clamp(out[1], 0.0f, float(ScreenHeight));
	return true;
}

void UTIL_TraceLine(Vector& vecSrc, Vector& vecEnd, int traceFlags, int ignore_pe, struct pmtrace_s* ptr, int index, int hull)
{
	/*
	in order to have tents collide with players, we have to run the player prediction code so
	that the client has the player list. We run this code once when we detect any COLLIDEALL
	tent, then set this BOOL to true so the code doesn't get run again if there's more than
	one COLLIDEALL ent for this update. (often are).
	*/
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// index - 1 for specific player, -1 for all players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(index - 1);

	gEngfuncs.pEventAPI->EV_SetTraceHull(hull);
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, traceFlags, ignore_pe, ptr);

	// Restore state info
	gEngfuncs.pEventAPI->EV_PopPMStates();
}

struct romandata_t { unsigned int value; wchar_t const* numeral; };
constexpr romandata_t romandata[] =
{
	{1000,	L"M"},
	{900,	L"CM"},
	{500,	L"D"},
	{400,	L"CD"},
	{100,	L"C"},
	{ 90,	L"XC"},
	{ 50,	L"L"},
	{ 40,	L"XL"},
	{ 10,	L"X"},
	{ 9,	L"IX"},
	{ 5,	L"V"},
	{ 4,	L"IV"},
	{ 1,	L"I"},
	{ 0,	nullptr} // end marker
};

std::wstring UTIL_ArabicToRoman(unsigned value)
{
	std::wstring result;

	for (const romandata_t* current = romandata; current->value > 0; ++current)
	{
		while (value >= current->value)
		{
			result += current->numeral;
			value -= current->value;
		}
	}

	return result;
}

static std::unordered_map<std::string, std::wstring> s_mapTexts;

const wchar_t* UTIL_GetLocalisation(const char* szToken)
{
	if (!szToken)
		return L"NULL POINTER";

	const wchar_t* pwcs = VGUI_LOCALISE->Find(szToken);

	if (pwcs)
		return pwcs;

	auto iterator = s_mapTexts.find(szToken);

	if (iterator == s_mapTexts.end())
	{
		s_mapTexts[szToken] = ANSIToUnicode(szToken);
		return s_mapTexts[szToken].c_str();
	}
	else
		return iterator->second.c_str();
}

GLuint UTIL_VguiSurfaceNewTextureId(void)
{
	return (GLuint)VGUI_SURFACE->CreateNewTextureID();
}

std::string& strip(std::string& s, const std::string& chars = " ")
{
	s.erase(0, s.find_first_not_of(chars.c_str()));
	s.erase(s.find_last_not_of(chars.c_str()) + 1);
	return s;
}

void UTIL_Split(const std::string& s, std::vector<std::string>& tokens, const std::string& delimiters)
{
	std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
	std::string::size_type pos = s.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		tokens.push_back(s.substr(lastPos, pos - lastPos));
		lastPos = s.find_first_not_of(delimiters, pos);
		pos = s.find_first_of(delimiters, lastPos);
	}
}

void parse(std::string& s, std::unordered_map<std::string, std::string>& items)
{
	std::vector<std::string> elements;
	s.erase(0, s.find_first_not_of(" {"));
	s.erase(s.find_last_not_of("} ") + 1);
	UTIL_Split(s, elements, ",");

	for (auto iter = elements.begin(); iter != elements.end(); iter++)
	{
		std::vector<std::string> kv;
		UTIL_Split(*iter, kv, ":");

		if (kv.size() != 2)
			continue;

		items[strip(kv[0], " \"")] = strip(kv[1], " \"");
	}
}

bool UTIL_EntityValid(const cl_entity_t* pEntity)	// Incomplete.
{
	bool bNotInPVS = (Q_abs(gEngfuncs.GetLocalPlayer()->curstate.messagenum - pEntity->curstate.messagenum) > 15);

	if (pEntity && pEntity->model && pEntity->model->name && !bNotInPVS)
		return true;

	return false;
}

static int iDummy = 0;

void ISurface_ClampTextWidthROW(wchar_t* text, size_t size, vgui::HFont font, int iWidth)
{
	const auto count = size / sizeof(wchar_t);
	const auto space_width = VGUI_SURFACE->GetCharacterWidth(font, L' ');
	wchar_t* result = (wchar_t*)malloc(size);
	memset(result, L'\0', size);

	// replace all '\n' with ' '
	for (unsigned i = 0; i < count; i++)
	{
		if (text[i] == L'\n')
			text[i] = L' ';
	}

	int iCurLineWidth = 0, iWordLength = 0;
	wchar_t* pwc = nullptr, * pt = nullptr;

	pwc = wcstok_s(text, L" ", &pt);
	while (pwc != nullptr)
	{
		VGUI_SURFACE->GetTextSize(font, pwc, iWordLength, iDummy);

		if (iCurLineWidth + iWordLength > iWidth)
		{
			wcscat_s(result, count, L"\n");
			iCurLineWidth = 0;
		}
		else if (wcslen(result))	// only place a new line if something already in it.
		{
			wcscat_s(result, count, L" ");
			iCurLineWidth += space_width;
		}

		wcscat_s(result, count, pwc);	// Concatenate the word anyway.
		iCurLineWidth += iWordLength;

		pwc = wcstok_s(nullptr, L" ", &pt);
	}

	memcpy(text, result, size);
	free(result);
}

bool ISurface_ClampTextWidthROW(std::wstring& string, vgui::HFont font, int iWidth)
{
	size_t size = string.length() * sizeof(wchar_t) * 2U;	// Additional space should be reserved.
	wchar_t* copy = (wchar_t*)malloc(size);

	if (copy == nullptr)
		return false;

	memset(copy, L'\0', size);
	memcpy(copy, string.c_str(), size);

	ISurface_ClampTextWidthROW(copy, size, font, iWidth);

	string = copy;
	free(copy);
	return true;
}

void ISurface_ClampTextWidthCJK(wchar_t* text, size_t size, vgui::HFont font, int iWidth)
{
	auto count = size / sizeof(wchar_t);

	// clear all '\n'
	for (unsigned i = 0; i < count; i++)
	{
		if (text[i] == L'\n')
			memmove(&text[i], &text[i + 1], size - (i + 1) * sizeof(wchar_t));
	}

	int iCurLineWidth = 0, iCurCharacterWidth = 0;
	for (unsigned i = 0; i < count; i++)
	{
		iCurCharacterWidth = VGUI_SURFACE->GetCharacterWidth(font, text[i]);

		if (iCurLineWidth + iCurCharacterWidth > iWidth)
		{
			memmove(&text[i + 1], &text[i], size - (i + 1) * sizeof(wchar_t));
			text[i] = L'\n';
			iCurLineWidth = 0;
		}
		else
			iCurLineWidth += iCurCharacterWidth;
	}
}

bool ISurface_ClampTextWidthCJK(std::wstring& string, vgui::HFont font, int iWidth)
{
	size_t size = string.length() * sizeof(wchar_t) * 2U;	// Additional space should be reserved.
	wchar_t* copy = (wchar_t*)malloc(size);

	if (copy == nullptr)
		return false;

	memset(copy, L'\0', size);
	memcpy(copy, string.c_str(), size);

	ISurface_ClampTextWidthCJK(copy, size, font, iWidth);

	string = copy;
	free(copy);
	return true;
}
