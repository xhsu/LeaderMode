/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

int g_iShotsFired = 0;

int CHudCrosshair::Init(void)
{
	gHUD::AddHudElem(this);

	Reset();

	m_bitsFlags |= HUD_ACTIVE;
	m_bObserverCrosshair = false;
	m_flCrosshairDistance = 0;
	m_iAmmoLastCheck = 0;
	m_iAlpha = 200;
	m_R = 50;
	m_G = 250;
	m_B = 50;
	m_cvarR = -1;
	m_cvarG = -1;
	m_cvarB = -1;
	m_iCurrentCrosshair = 0;
	m_bAdditive = 1;
	m_iCrosshairScaleBase = 1024;
	m_szLastCrosshairColor[0] = 0;
	m_flLastCalcTime = 0;

	Q_memset(&m_rghScopes, NULL, sizeof(m_rghScopes));
	Q_memset(&m_rgrcScopes, NULL, sizeof(m_rgrcScopes));

	return 1;
};

int CHudCrosshair::VidInit(void)
{
	int i;
	client_sprite_t* pList = gEngfuncs.pfnSPR_GetList("sprites/observer.txt", &i);

	if (pList)
	{
		int iRes;

		if (ScreenWidth < 640)
			iRes = 320;
		else
			iRes = 640;

		char sz[128];
		client_sprite_t* p = gHUD::GetSpriteFromList(pList, "crosshair", iRes, i);

		if (p)
		{
			Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
			m_hObserverCrosshair = gEngfuncs.pfnSPR_Load(sz);
			m_rcObserverCrosshair = p->rc;
		}
		else
			m_hObserverCrosshair = NULL;
	}

	// get the sniper rifles scopes
	pList = gEngfuncs.pfnSPR_GetList("sprites/scopes.txt", &i);

	if (pList)
	{
		char sz[128];
		client_sprite_t* p = nullptr;

		for (int j = 0; j < LAST_WEAPON; j++)
		{
			if (!g_rgWpnInfo[j].m_pszInternalName || g_rgWpnInfo[j].m_pszInternalName[0] == '\0')
				continue;

			p = gHUD::GetSpriteFromList(pList, g_rgWpnInfo[j].m_pszInternalName, 640, i);

			if (p)
			{
				Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
				m_rghScopes[j] = gEngfuncs.pfnSPR_Load(sz);
				m_rgrcScopes[j] = p->rc;
			}
			else
				m_rghScopes[j] = NULL;
		}
	}

	return 1;
}

int CHudCrosshair::Draw(float flTime)
{
	static wrect_t nullrc;

	// in steel sight calibrating mode.
	if (cl_gun_ofs[0]->value || cl_gun_ofs[1]->value || cl_gun_ofs[2]->value)
	{
		gEngfuncs.pfnFillRGBA(ScreenWidth / 2 - 1, ScreenHeight / 2 - 1, 2, 2, 255, 255, 255, 255);
	}

	if (gEngfuncs.IsSpectateOnly())
	{
		if (g_iUser1 != OBS_IN_EYE)
		{
			gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
			return 1;
		}

		gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
		DrawCrosshair(flTime, WEAPON_NONE);
		return 1;
	}

	if (gHUD::m_bIntermission)
	{
		gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
		return 1;
	}

	if (!g_pCurWeapon)
		return 0;

	if (gHUD::m_iFOV >= 40)
	{
		if (g_pCurWeapon->m_iId != WEAPON_SVD && g_pCurWeapon->m_iId != WEAPON_AWP && g_pCurWeapon->m_iId != WEAPON_SRS && g_pCurWeapon->m_iId != WEAPON_PSG1)
		{
			if (!(gHUD::m_bitsHideHUDDisplay & HIDEHUD_CROSSHAIR))
				DrawCrosshair(flTime, g_pCurWeapon->m_iId);
		}
	}

	if (gHUD::m_iFOV <= 40 && m_rghScopes[g_pCurWeapon->m_iId])	// have a customised scope? draw it.
	{
		gEngfuncs.pfnSetCrosshair(m_rghScopes[g_pCurWeapon->m_iId], m_rgrcScopes[g_pCurWeapon->m_iId], 255, 255, 255);
	}
	else
	{
		gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
	}

	return 1;
}

void CHudCrosshair::Reset(void)
{
	m_iAlpha = 120;
	m_bitsFlags |= HUD_ACTIVE;

	gHUD::m_bitsHideHUDDisplay = 0;
}

void CHudCrosshair::CalculateCrosshairColor(void)
{
	const char* value = cl_crosshair_color->string;

	if (value && Q_strcmp(value, m_szLastCrosshairColor))
	{
		int cvarR, cvarG, cvarB;
		const char* token;
		const char* data = value;

		data = SharedParse(data);
		token = SharedGetToken();

		if (token)
		{
			cvarR = Q_atoi(token);

			data = SharedParse(data);
			token = SharedGetToken();

			if (token)
			{
				cvarG = Q_atoi(token);

				data = SharedParse(data);
				token = SharedGetToken();

				if (token)
				{
					cvarB = Q_atoi(token);

					if (m_cvarR != cvarR || m_cvarG != cvarG || m_cvarB != cvarB)
					{
						int r, g, b;

						r = Q_clamp(cvarR, 0, 255);
						g = Q_clamp(cvarG, 0, 255);
						b = Q_clamp(cvarB, 0, 255);

						m_R = r;
						m_G = g;
						m_B = b;
						m_cvarR = cvarR;
						m_cvarG = cvarG;
						m_cvarB = cvarB;
					}

					Q_strlcpy(m_szLastCrosshairColor, value);
				}
			}
		}
	}
}

void CHudCrosshair::CalculateCrosshairDrawMode(void)
{
	float value = cl_crosshair_translucent->value;

	if (value == 0)
		m_bAdditive = false;
	else if (value == 1)
		m_bAdditive = true;
	else
		gEngfuncs.Con_Printf("usage: cl_crosshair_translucent <1|0>\n");
}

void CHudCrosshair::CalculateCrosshairSize(void)
{
	const char* value = cl_crosshair_size->string;

	if (!value)
		return;

	int size = Q_atoi(value);

	if (size)
	{
		if (size > 3)
			size = -1;
	}
	else
	{
		if (Q_strcmp(value, "0"))
			size = -1;
	}

	if (!Q_stricmp(value, "auto"))
		size = 0;
	else if (!Q_stricmp(value, "small"))
		size = 1;
	else if (!Q_stricmp(value, "medium"))
		size = 2;
	else if (!Q_stricmp(value, "large"))
		size = 3;

	if (size == -1)
	{
		gEngfuncs.Con_Printf("usage: cl_crosshair_size <auto|small|medium|large>\n");
		return;
	}

	switch (size)
	{
	case 0:
	{
		if (ScreenWidth < 640)
			m_iCrosshairScaleBase = 1024;
		else if (ScreenWidth < 1024)
			m_iCrosshairScaleBase = 800;
		else
			m_iCrosshairScaleBase = 640;

		break;
	}

	case 1:
	{
		m_iCrosshairScaleBase = 1024;
		break;
	}

	case 2:
	{
		m_iCrosshairScaleBase = 800;
		break;
	}

	case 3:
	{
		m_iCrosshairScaleBase = 640;
		break;
	}
	}
}

// UNDONE, TODO
// finish this list for new weapons. VECTOR, PSG1 and RPD.

int GetWeaponAccuracyFlags(WeaponIdType iId)
{
	int result = 0;

	if (iId > WEAPON_NONE && iId < LAST_WEAPON)
	{
		switch (iId)
		{
		case WEAPON_XM8:
		case WEAPON_MK46:
		case WEAPON_SCARH:
		case WEAPON_AK47:
		case WEAPON_P90:
			result = ACCURACY_AIR | ACCURACY_SPEED;
			break;

		case WEAPON_GLOCK18:
		case WEAPON_USP:
		case WEAPON_ANACONDA:
		case WEAPON_FIVESEVEN:
		case WEAPON_DEAGLE:
			result = ACCURACY_AIR | ACCURACY_SPEED | ACCURACY_DUCK;
			break;

		case WEAPON_MAC10:
		case WEAPON_UMP45:
		case WEAPON_MP5N:
		case WEAPON_MP7A1:
			result = ACCURACY_AIR;
			break;

		case WEAPON_M4A1:
			result = ACCURACY_AIR | ACCURACY_SPEED | ACCURACY_MULTIPLY_BY_14;
			break;
		}
	}

	return result;
}

int CHudCrosshair::DrawCrosshair(float flTime, WeaponIdType weaponid)
{
	int iDistance;
	int iDeltaDistance;
	int iWeaponAccuracyFlags;
	int iBarSize;
	float flCrosshairDistance;

	switch (weaponid)
	{
	case WEAPON_ANACONDA:
	case WEAPON_FIVESEVEN:
	case WEAPON_USP:
	case WEAPON_GLOCK18:
	case WEAPON_AWP:
	case WEAPON_DEAGLE:
	{
		iDistance = 8;
		iDeltaDistance = 3;
		break;
	}

	case WEAPON_MP5N:
	{
		iDistance = 6;
		iDeltaDistance = 2;
		break;
	}

	case WEAPON_KSG12:
	{
		iDistance = 8;
		iDeltaDistance = 6;
		break;
	}

	case WEAPON_SVD:
	{
		iDistance = 6;
		iDeltaDistance = 4;
		break;
	}

	case WEAPON_AK47:
	{
		iDistance = 4;
		iDeltaDistance = 4;
		break;
	}

	case WEAPON_MP7A1:
	case WEAPON_P90:
	{
		iDistance = 7;
		iDeltaDistance = 3;
		break;
	}

	case WEAPON_M1014:
	{
		iDistance = 9;
		iDeltaDistance = 4;
		break;
	}

	case WEAPON_MAC10:
	{
		iDistance = 9;
		iDeltaDistance = 3;
		break;
	}

	case WEAPON_XM8:
	{
		iDistance = 3;
		iDeltaDistance = 3;
		break;
	}

	case WEAPON_C4:
	case WEAPON_UMP45:
	case WEAPON_MK46:
	{
		iDistance = 6;
		iDeltaDistance = 3;
		break;
	}

	case WEAPON_SRS:
	case WEAPON_PSG1:
	case WEAPON_SCARH:
	{
		iDistance = 5;
		iDeltaDistance = 3;
		break;
	}

	default:
	{
		iDistance = 4;
		iDeltaDistance = 3;
		break;
	}
	}

	iWeaponAccuracyFlags = GetWeaponAccuracyFlags(weaponid);

	if (iWeaponAccuracyFlags != 0 && cl_dynamiccrosshair && cl_dynamiccrosshair->value != 0.0 && !(gHUD::m_bitsHideHUDDisplay & HIDEHUD_CROSSHAIR))
	{
		if ((g_iPlayerFlags & FL_ONGROUND) || !(iWeaponAccuracyFlags & ACCURACY_AIR))
		{
			if ((g_iPlayerFlags & FL_DUCKING) && (iWeaponAccuracyFlags & ACCURACY_DUCK))
			{
				iDistance *= 0.5;
			}
			else
			{
				float flLimitSpeed;

				switch (weaponid)
				{
				case WEAPON_SRS:
				case WEAPON_PSG1:
				case WEAPON_MAC10:
				case WEAPON_MP5N:
				case WEAPON_KSG12:
				case WEAPON_DEAGLE:
				case WEAPON_SCARH:
				{
					flLimitSpeed = 140;
					break;
				}

				default:
				{
					flLimitSpeed = 0;
					break;
				}
				}

				if (g_flPlayerSpeed > flLimitSpeed && (iWeaponAccuracyFlags & 2))
					iDistance *= 1.5;
			}
		}
		else
			iDistance *= 2;

		if (iWeaponAccuracyFlags & ACCURACY_MULTIPLY_BY_14)
			iDistance *= 1.4;
	}

	if (g_iShotsFired > m_iAmmoLastCheck)
	{
		m_flCrosshairDistance += iDeltaDistance;
		m_iAlpha -= 40;

		if (m_flCrosshairDistance > 15)
			m_flCrosshairDistance = 15;

		if (m_iAlpha < 120)
			m_iAlpha = 120;
	}
	else
	{
		m_flCrosshairDistance -= (0.013 * m_flCrosshairDistance) + 0.1;
		m_iAlpha += 2;
	}

	if (g_iShotsFired > 600)
		g_iShotsFired = 1;

	m_iAmmoLastCheck = g_iShotsFired;

	if (iDistance > m_flCrosshairDistance)
		m_flCrosshairDistance = iDistance;

	iBarSize = (int)((m_flCrosshairDistance - (float)iDistance) * 0.5) + 5;

	if (gHUD::m_flTime > m_flLastCalcTime + 1)
	{
		CalculateCrosshairColor();
		CalculateCrosshairDrawMode();
		CalculateCrosshairSize();

		m_flLastCalcTime = gHUD::m_flTime;
	}

	flCrosshairDistance = m_flCrosshairDistance;

	if (m_iCrosshairScaleBase != ScreenWidth)
	{
		flCrosshairDistance *= (float)(ScreenWidth) / m_iCrosshairScaleBase;
		iBarSize = (float)(ScreenWidth * iBarSize) / m_iCrosshairScaleBase;
	}

	// when we scope in, remove the crosshair.
	if (gHUD::m_iFOV <= 40)
		m_iAlpha = 0;	// hide crosshair when we are using sniper scope.
	else if (gHUD::m_iLastFOVDiff > 0)
	{
		if (gHUD::m_iFOV >= 90)	// scoping out
			m_iAlpha = float(m_iAlpha) * (1.0f - Q_abs(gHUD::m_flDisplayedFOV - float(gHUD::m_iFOV)) / float(gHUD::m_iLastFOVDiff));
		else
			m_iAlpha = float(m_iAlpha) * (Q_abs(gHUD::m_flDisplayedFOV - float(gHUD::m_iFOV)) / float(gHUD::m_iLastFOVDiff));
	}

	m_iAlpha = Q_clamp(m_iAlpha, 0, 255);

	if (gHUD::m_NightVision.m_fOn)
		DrawCrosshairEx(flTime, weaponid, iBarSize, flCrosshairDistance, false, 250, 50, 50, m_iAlpha);
	else
		DrawCrosshairEx(flTime, weaponid, iBarSize, flCrosshairDistance, m_bAdditive, m_R, m_G, m_B, m_iAlpha);

	return 1;
}

int CHudCrosshair::DrawCrosshairEx(float flTime, WeaponIdType weaponid, int iBarSize, float flCrosshairDistance, bool bAdditive, int r, int g, int b, int a)
{
	bool bDrawPoint = false;
	bool bDrawCircle = false;
	bool bDrawCross = false;

	void (*pfnFillRGBA)(int x, int y, int w, int h, int r, int g, int b, int a) = (bAdditive == false) ? gEngfuncs.pfnFillRGBABlend : gEngfuncs.pfnFillRGBA;

	switch ((int)cl_crosshair_type->value)
	{
	case 1:
	{
		bDrawPoint = true;
		bDrawCross = true;
		break;
	}

	case 2:
	{
		bDrawPoint = true;
		bDrawCircle = true;
		break;
	}

	case 3:
	{
		bDrawPoint = true;
		break;
	}

	default:
	{
		bDrawCross = true;
		break;
	}
	}

	if (bDrawCircle)
	{
		float radius = (iBarSize / 2) + flCrosshairDistance;
		int count = (int)((cos(M_PI / 4) * radius) + 0.5);

		for (int i = 0; i < count; i++)
		{
			int size = sqrt((radius * radius) - (float)(i * i));

			pfnFillRGBA((ScreenWidth / 2) + i, (ScreenHeight / 2) + size, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) + i, (ScreenHeight / 2) - size, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) - i, (ScreenHeight / 2) + size, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) - i, (ScreenHeight / 2) - size, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) + size, (ScreenHeight / 2) + i, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) + size, (ScreenHeight / 2) - i, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) - size, (ScreenHeight / 2) + i, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) - size, (ScreenHeight / 2) - i, 1, 1, r, g, b, a);
		}
	}

	if (bDrawPoint)
		pfnFillRGBA((ScreenWidth / 2) - 1, (ScreenHeight / 2) - 1, 3, 3, r, g, b, a);

	if (bDrawCross)
	{
		pfnFillRGBA((ScreenWidth / 2) - (int)flCrosshairDistance - iBarSize + 1, ScreenHeight / 2, iBarSize, 1, r, g, b, a);
		pfnFillRGBA((ScreenWidth / 2) + (int)flCrosshairDistance, ScreenHeight / 2, iBarSize, 1, r, g, b, a);
		pfnFillRGBA(ScreenWidth / 2, (ScreenHeight / 2) - (int)flCrosshairDistance - iBarSize + 1, 1, iBarSize, r, g, b, a);
		pfnFillRGBA(ScreenWidth / 2, (ScreenHeight / 2) + (int)flCrosshairDistance, 1, iBarSize, r, g, b, a);
	}

	return 1;
}

void CHudCrosshair::Adjust_Crosshair(void)
{
	if (gEngfuncs.Cmd_Argc() <= 1)
		m_iCurrentCrosshair = (m_iCurrentCrosshair + 1) % 5;
	else
		m_iCurrentCrosshair = atoi(gEngfuncs.Cmd_Argv(1)) % 10;

	switch (m_iCurrentCrosshair)
	{
	default:
	{
		m_R = 50;
		m_G = 250;
		m_B = 50;
		m_bAdditive = true;
		break;
	}

	case 8:
	{
		m_R = 250;
		m_G = 250;
		m_B = 50;
		m_bAdditive = false;
		break;
	}

	case 9:
	{
		m_R = 50;
		m_G = 250;
		m_B = 250;
		m_bAdditive = false;
		break;
	}

	case 1:
	{
		m_R = 250;
		m_G = 50;
		m_B = 50;
		m_bAdditive = true;
		break;
	}

	case 2:
	{
		m_R = 50;
		m_G = 50;
		m_B = 250;
		m_bAdditive = true;
		break;
	}

	case 3:
	{
		m_R = 250;
		m_G = 250;
		m_B = 50;
		m_bAdditive = true;
		break;
	}

	case 4:
	{
		m_R = 50;
		m_G = 250;
		m_B = 250;
		m_bAdditive = true;
		break;
	}

	case 5:
	{
		m_R = 50;
		m_G = 250;
		m_B = 50;
		m_bAdditive = false;
		break;
	}

	case 6:
	{
		m_R = 250;
		m_G = 50;
		m_B = 50;
		m_bAdditive = false;
		break;
	}

	case 7:
	{
		m_R = 50;
		m_G = 50;
		m_B = 250;
		m_bAdditive = false;
		break;
	}
	}

	char cmd[256] = "";
	Q_snprintf(cmd, charsmax(cmd), "%d %d %d", m_R, m_G, m_B);
	gEngfuncs.Cvar_Set("cl_crosshair_color", cmd);

	if (m_bAdditive)
		gEngfuncs.Cvar_Set("cl_crosshair_translucent", "1");
	else
		gEngfuncs.Cvar_Set("cl_crosshair_translucent", "0");
}

void CHudCrosshair::MsgFunc_Crosshair(bool bDrawn)
{
	static wrect_t nullrc;

	if (gEngfuncs.IsSpectateOnly())
		return;

	if (gEngfuncs.pfnGetCvarFloat("cl_observercrosshair") == 0)
		return;

	if (bDrawn)
	{
		gEngfuncs.pfnSetCrosshair(m_hObserverCrosshair, m_rcObserverCrosshair, 255, 255, 255);
	}
	else
	{
		gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
	}

	m_bObserverCrosshair = true;
}

void CHudCrosshair::MsgFunc_HideWeapon(int& bits)
{
	static wrect_t nullrc;

	gHUD::m_bitsHideHUDDisplay = bits;

	if (!gEngfuncs.IsSpectateOnly())
	{
		if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_CROSSHAIR) || gHUD::m_bitsHideHUDDisplay == 0)
		{
			gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
		}
		else if (gHUD::m_bitsHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL))
		{
			if (!m_bObserverCrosshair)
				gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
		}
		else
		{
			if (g_pCurWeapon)
				gEngfuncs.pfnSetCrosshair(m_rghScopes[g_pCurWeapon->m_iId], m_rgrcScopes[g_pCurWeapon->m_iId], 255, 255, 255);

			m_bObserverCrosshair = false;
		}
	}
}
