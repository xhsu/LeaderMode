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

Vector g_ColorBlue = Vector(0.6, 0.8, 1.0);
Vector g_ColorRed = Vector(1.0, 0.25, 0.25);
Vector g_ColorGreen = Vector(0.6, 1.0, 0.6);
Vector g_ColorYellow = Vector(1.0, 0.7, 0.0);
Vector g_ColorGrey = Vector(0.8, 0.8, 0.8);

float* GetClientColor(int clientIndex)
{
	switch (g_PlayerExtraInfo[clientIndex].m_iTeam)
	{
	case TEAM_TERRORIST:  return g_ColorRed;
	case TEAM_CT:         return g_ColorBlue;
	case TEAM_SPECTATOR:
	case TEAM_UNASSIGNED: return g_ColorYellow;
	case 4:               return g_ColorGreen;
	default:              return g_ColorGrey;
	}
}

hSprite LoadSprite(const char* pszName)
{
	int i;
	char sz[256];

	if (ScreenWidth < 640)
		i = 320;
	else
		i = 640;

	Q_snprintf(sz, charsmax(sz), pszName, i);
	return gEngfuncs.pfnSPR_Load(sz);
}

// from view.cpp
extern Vector v_origin, v_angles;

bool CalcScreen(Vector& in, Vector2D& out)
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
