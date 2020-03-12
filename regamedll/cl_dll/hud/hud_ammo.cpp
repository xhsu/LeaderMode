/*

Created Date: Mar 11 2020

*/

#include "cl_base.h"
#include "../dlls/weapontype.h"

int g_iShotsFired;

int giBucketHeight, giBucketWidth, giABHeight, giABWidth;
hSprite ghsprBuckets;

int CHudAmmo::Init(void)
{
	gHUD::AddHudElem(this);

	Reset();

	m_bitsFlags |= (HUD_ACTIVE | HUD_INTERMISSION);
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

	gWR.Init();
	gHR.Init();
	return 1;
};

int CHudAmmo::VidInit(void)
{
	m_HUD_bucket0 = gHUD::GetSpriteIndex("bucket1");
	m_HUD_selection = gHUD::GetSpriteIndex("selection");

	ghsprBuckets = gHUD::GetSprite(m_HUD_bucket0);
	giBucketWidth = gHUD::GetSpriteRect(m_HUD_bucket0).right - gHUD::GetSpriteRect(m_HUD_bucket0).left;
	giBucketHeight = gHUD::GetSpriteRect(m_HUD_bucket0).bottom - gHUD::GetSpriteRect(m_HUD_bucket0).top;

	gHR.iHistoryGap = max(gHR.iHistoryGap, gHUD::GetSpriteRect(m_HUD_bucket0).bottom - gHUD::GetSpriteRect(m_HUD_bucket0).top);
	gWR.LoadAllWeaponSprites();

	if (ScreenWidth >= 640)
	{
		giABWidth = 20;
		giABHeight = 4;
	}
	else
	{
		giABWidth = 10;
		giABHeight = 2;
	}

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
		client_sprite_t* p = gHUD::GetSpriteList(pList, "crosshair", iRes, i);

		if (p)
		{
			Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
			m_hObserverCrosshair = gEngfuncs.pfnSPR_Load(sz);
			m_rcObserverCrosshair = p->rc;
		}
		else
			m_hObserverCrosshair = NULL;
	}

	return 1;
}

int CHudAmmo::Draw(float flTime)
{
	static wrect_t nullrc;
	int a, x, y, r, g, b;
	int AmmoWidth;

	if (gEngfuncs.IsSpectateOnly())
	{
		if (g_iUser1 != OBS_IN_EYE)
		{
			gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
			return 1;
		}

		WEAPON* pw = gWR.GetWeapon(WEAPON_AWP);

		if (pw->hZoomedCrosshair)
		{
			gEngfuncs.pfnSetCrosshair(pw->hZoomedCrosshair, pw->rcZoomedCrosshair, 255, 255, 255);
			return 1;
		}

		gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
		DrawCrosshair(flTime, 0);
		return 1;
	}

	if (gHUD::m_iIntermission)
	{
		gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
		return 1;
	}

	if (g_iUser1 != OBS_IN_EYE)
	{
		if (!(gHUD::m_iWeaponBits & (1 << (WEAPON_SUIT))))
			return 1;

		if ((gHUD::m_bitsHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)))
			return 1;

		DrawWList(flTime);

		gHR.DrawAmmoHistory(flTime);

		if (!(m_bitsFlags & HUD_ACTIVE))
			return 0;
	}

	if (!m_pWeapon)
		return 0;

	WEAPON* pw = m_pWeapon;

	if (gHUD::m_iFOV >= 55)
	{
		if (pw->iId != WEAPON_SVD && pw->iId != WEAPON_AWP && pw->iId != WEAPON_M200 && pw->iId != WEAPON_M14EBR)
		{
			if (!(gHUD::m_bitsHideHUDDisplay & HIDEHUD_CROSSHAIR))
				DrawCrosshair(flTime, pw->iId);
		}
	}

	if (g_iUser1 != OBS_IN_EYE)
	{
		if ((pw->iAmmoType < 0) && (pw->iAmmo2Type < 0))
			return 0;

		int iFlags = DHN_DRAWZERO;

		AmmoWidth = gHUD::GetSpriteRect(gHUD::m_HUD_number_0).right - gHUD::GetSpriteRect(gHUD::m_HUD_number_0).left;

		a = Q_max(MIN_ALPHA, int(m_fFade));

		if (m_fFade > 0)
			m_fFade -= (gHUD::m_flTimeDelta * 20);

		UnpackRGB(r, g, b, RGB_YELLOWISH);
		ScaleColors(r, g, b, a);

		y = ScreenHeight - gHUD::m_iFontHeight - gHUD::m_iFontHeight / 2;

		if (m_pWeapon->iAmmoType > 0)
		{
			int iIconWidth = m_pWeapon->rcAmmo.right - m_pWeapon->rcAmmo.left;

			if (pw->iClip >= 0)
			{
				x = ScreenWidth - (8 * AmmoWidth) - iIconWidth;
				x = gHUD::DrawHudNumber(x, y, iFlags | DHN_3DIGITS, pw->iClip, r, g, b);

				wrect_t rc;
				rc.top = 0;
				rc.left = 0;
				rc.right = AmmoWidth;
				rc.bottom = 100;

				int iBarWidth = AmmoWidth / 10;
				x += AmmoWidth / 2;

				UnpackRGB(r, g, b, RGB_YELLOWISH);
				gEngfuncs.pfnFillRGBA(x, y, iBarWidth, gHUD::m_iFontHeight, r, g, b, a);

				x += iBarWidth + AmmoWidth / 2;

				ScaleColors(r, g, b, a);
				x = gHUD::DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gWR.CountAmmo(pw->iAmmoType), r, g, b);
			}
			else
			{
				x = ScreenWidth - 4 * AmmoWidth - iIconWidth;
				x = gHUD::DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gWR.CountAmmo(pw->iAmmoType), r, g, b);
			}

			int iOffset = (m_pWeapon->rcAmmo.bottom - m_pWeapon->rcAmmo.top) / 8;
			gEngfuncs.pfnSPR_Set(m_pWeapon->hAmmo, r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y - iOffset, &m_pWeapon->rcAmmo);
		}

		if (pw->iAmmo2Type > 0)
		{
			int iIconWidth = m_pWeapon->rcAmmo2.right - m_pWeapon->rcAmmo2.left;

			if ((pw->iAmmo2Type != 0) && (gWR.CountAmmo(pw->iAmmo2Type) > 0))
			{
				y -= gHUD::m_iFontHeight + gHUD::m_iFontHeight / 4;
				x = ScreenWidth - 4 * AmmoWidth - iIconWidth;
				x = gHUD::DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gWR.CountAmmo(pw->iAmmo2Type), r, g, b);

				gEngfuncs.pfnSPR_Set(m_pWeapon->hAmmo2, r, g, b);

				int iOffset = (m_pWeapon->rcAmmo2.bottom - m_pWeapon->rcAmmo2.top) / 8;
				gEngfuncs.pfnSPR_DrawAdditive(0, x, y - iOffset, &m_pWeapon->rcAmmo2);
			}
		}
	}

	return 1;
}

void CHudAmmo::Think(void)
{
	if (gHUD::m_fPlayerDead)
		return;

	if (gHUD::m_iWeaponBits != gWR.iOldWeaponBits)
	{
		gWR.iOldWeaponBits = gHUD::m_iWeaponBits;

		for (int i = MAX_WEAPONS - 1; i > 0; i--)
		{
			WEAPON* p = gWR.GetWeapon(i);

			if (p)
			{
				if (gHUD::m_iWeaponBits & (1 << p->iId))
					gWR.PickupWeapon(p);
				else
					gWR.DropWeapon(p);
			}
		}
	}

	if (!gpActiveSel)
		return;

	if (gHUD::m_iKeyBits & IN_ATTACK)
	{
		if (gpActiveSel != (WEAPON*)1)
		{
			if (Q_strlen(gpActiveSel->szExtraName))
				gEngfuncs.pfnServerCmd(gpActiveSel->szExtraName);
			else
				gEngfuncs.pfnServerCmd(gpActiveSel->szName);

			g_weaponselect = gpActiveSel->iId;
		}

		gpLastSel = gpActiveSel;
		gpActiveSel = NULL;
		gHUD::m_iKeyBits &= ~IN_ATTACK;

		gEngfuncs.pfnPlaySoundByName("common/wpn_select.wav", VOL_NORM);
	}
}

void CHudAmmo::Reset(void)
{
	m_fFade = 0;
	m_iAlpha = 120;
	m_bitsFlags |= HUD_ACTIVE;

	gpActiveSel = NULL;
	gHUD::m_bitsHideHUDDisplay = 0;

	gWR.Reset();
	gHR.Reset();
}

int DrawBar(int x, int y, int width, int height, float f)
{
	int r, g, b;

	if (f < 0)
		f = 0;

	if (f > 1)
		f = 1;

	if (f)
	{
		int w = f * width;

		if (w <= 0)
			w = 1;

		UnpackRGB(r, g, b, RGB_GREENISH);
		gEngfuncs.pfnFillRGBA(x, y, w, height, r, g, b, 255);
		x += w;
		width -= w;
	}

	UnpackRGB(r, g, b, RGB_YELLOWISH);
	gEngfuncs.pfnFillRGBA(x, y, width, height, r, g, b, 128);
	return (x + width);
}

void DrawAmmoBar(WEAPON* p, int x, int y, int width, int height)
{
	if (!p)
		return;

	if (p->iAmmoType != -1)
	{
		if (!gWR.CountAmmo(p->iAmmoType))
			return;

		float f = (float)gWR.CountAmmo(p->iAmmoType) / (float)p->iMax1;
		x = DrawBar(x, y, width, height, f);

		if (p->iAmmo2Type != -1)
		{
			f = (float)gWR.CountAmmo(p->iAmmo2Type) / (float)p->iMax2;
			x += 5;

			DrawBar(x, y, width, height, f);
		}
	}
}

int CHudAmmo::DrawWList(float flTime)
{
	int r, g, b, x, y, a, i;

	if (!gpActiveSel)
		return 0;

	int iActiveSlot;

	if (gpActiveSel == (WEAPON*)1)
		iActiveSlot = -1;
	else
		iActiveSlot = gpActiveSel->iSlot;

	// UNDONE : UNDONE_RADAR
	x = /*gHUD::m_Radar.GetRadarSize() + */10;
	y = 10;

	if (iActiveSlot > 0)
	{
		if (!gWR.GetFirstPos(iActiveSlot))
		{
			gpActiveSel = (WEAPON*)1;
			iActiveSlot = -1;
		}
	}

	for (i = 0; i < MAX_WEAPON_SLOTS; i++)
	{
		int iWidth;

		UnpackRGB(r, g, b, RGB_YELLOWISH);

		if (iActiveSlot == i)
			a = 255;
		else
			a = 192;

		ScaleColors(r, g, b, 255);
		gEngfuncs.pfnSPR_Set(gHUD::GetSprite(m_HUD_bucket0 + i), r, g, b);

		if (i == iActiveSlot)
		{
			WEAPON* p = gWR.GetFirstPos(iActiveSlot);

			if (p)
				iWidth = p->rcActive.right - p->rcActive.left;
			else
				iWidth = giBucketWidth;
		}
		else
			iWidth = giBucketWidth;

		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &gHUD::GetSpriteRect(m_HUD_bucket0 + i));
		x += iWidth + 5;
	}

	// UNDONE : UNDONE_RADAR
	a = 128;
	x = /*gHUD::m_Radar.GetRadarSize() + */10;

	for (i = 0; i < MAX_WEAPON_SLOTS; i++)
	{
		y = giBucketHeight + 10;

		if (i == iActiveSlot)
		{
			WEAPON* p = gWR.GetFirstPos(i);
			int iWidth = giBucketWidth;

			if (p)
				iWidth = p->rcActive.right - p->rcActive.left;

			for (int iPos = 0; iPos < MAX_WEAPON_POSITIONS; iPos++)
			{
				p = gWR.GetWeaponSlot(i, iPos);

				if (!p || !p->iId)
					continue;

				UnpackRGB(r, g, b, RGB_YELLOWISH);

				if (gWR.HasAmmo(p))
				{
					ScaleColors(r, g, b, 192);
				}
				else
				{
					UnpackRGB(r, g, b, RGB_REDISH);
					ScaleColors(r, g, b, 128);
				}

				if (gpActiveSel == p)
				{
					gEngfuncs.pfnSPR_Set(p->hActive, r, g, b);
					gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &p->rcActive);

					gEngfuncs.pfnSPR_Set(gHUD::GetSprite(m_HUD_selection), r, g, b);
					gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &gHUD::GetSpriteRect(m_HUD_selection));
				}
				else
				{
					gEngfuncs.pfnSPR_Set(p->hInactive, r, g, b);
					gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &p->rcInactive);
				}

				DrawAmmoBar(p, x + giABWidth / 2, y, giABWidth, giABHeight);
				y += p->rcActive.bottom - p->rcActive.top + 5;
			}

			x += iWidth + 5;
		}
		else
		{
			UnpackRGB(r, g, b, RGB_YELLOWISH);

			for (int iPos = 0; iPos < MAX_WEAPON_POSITIONS; iPos++)
			{
				WEAPON* p = gWR.GetWeaponSlot(i, iPos);

				if (!p || !p->iId)
					continue;

				if (gWR.HasAmmo(p))
				{
					UnpackRGB(r, g, b, RGB_YELLOWISH);
					a = 128;
				}
				else
				{
					UnpackRGB(r, g, b, RGB_REDISH);
					a = 96;
				}

				gEngfuncs.pfnFillRGBA(x, y, giBucketWidth, giBucketHeight, r, g, b, a);
				y += giBucketHeight + 5;
			}

			x += giBucketWidth + 5;
		}
	}

	return 1;
}

void CHudAmmo::CalculateCrosshairColor(void)
{
	char* value = cl_crosshair_color->string;

	if (value && Q_strcmp(value, m_szLastCrosshairColor))
	{
		int cvarR, cvarG, cvarB;
		char* token;
		char* data = value;

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

void CHudAmmo::CalculateCrosshairDrawMode(void)
{
	float value = cl_crosshair_translucent->value;

	if (value == 0)
		m_bAdditive = false;
	else if (value == 1)
		m_bAdditive = true;
	else
		gEngfuncs.Con_Printf("usage: cl_crosshair_translucent <1|0>\n");
}

void CHudAmmo::CalculateCrosshairSize(void)
{
	char* value = cl_crosshair_size->string;

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

int GetWeaponAccuracyFlags(int iWeaponID)
{
	int flags;

	switch (iWeaponID)
	{
	case WEAPON_PM9:
	case WEAPON_UMP45:
	case WEAPON_MP5N:
	case WEAPON_MP7A1:
	{
		flags = 1;
		break;
	}

	case WEAPON_ACR:
	case WEAPON_CM901:
	case WEAPON_MK46:
	case WEAPON_SCARL:
	case WEAPON_M4A1:
	case WEAPON_AK47:
	case WEAPON_P90:
	{
		flags = 3;
		break;
	}

	case WEAPON_USP:
	case WEAPON_GLOCK18:
	case WEAPON_ANACONDA:
	case WEAPON_FIVESEVEN:
	case WEAPON_DEAGLE:
	{
		flags = 7;
		break;
	}

	case WEAPON_QBZ95:
	{
		flags = (g_iWeaponFlags & 0x10) < 1 ? 3 : 19;
		break;
	}

	default:
	{
		flags = 0;
		break;
	}
	}

	return flags;
}

int CHudAmmo::DrawCrosshair(float flTime, int weaponid)
{
	int iDistance;
	int iDeltaDistance;
	int iWeaponAccuracyFlags;
	int iBarSize;
	float flCrosshairDistance;

	switch (weaponid)
	{
	case WEAPON_ANACONDA:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_FIVESEVEN:
	case WEAPON_USP:
	case WEAPON_GLOCK18:
	case WEAPON_AWP:
	case WEAPON_FLASHBANG:
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
	case WEAPON_KNIFE:
	case WEAPON_P90:
	{
		iDistance = 7;
		iDeltaDistance = 3;
		break;
	}

	case WEAPON_STRIKER:
	{
		iDistance = 9;
		iDeltaDistance = 4;
		break;
	}

	case WEAPON_PM9:
	{
		iDistance = 9;
		iDeltaDistance = 3;
		break;
	}

	case WEAPON_ACR:
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

	case WEAPON_M200:
	case WEAPON_M14EBR:
	case WEAPON_SCARL:
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
		if ((g_iPlayerFlags & FL_ONGROUND) || !(iWeaponAccuracyFlags & 1))
		{
			if ((g_iPlayerFlags & FL_DUCKING) && (iWeaponAccuracyFlags & 4))
			{
				iDistance *= 0.5;
			}
			else
			{
				float flLimitSpeed;

				switch (weaponid)
				{
				case WEAPON_PM9:
				case WEAPON_M14EBR:
				case WEAPON_CM901:
				case WEAPON_MP5N:
				case WEAPON_KSG12:
				case WEAPON_DEAGLE:
				case WEAPON_SCARL:
				{
					flLimitSpeed = 140;
					break;
				}

				case WEAPON_KNIFE:
				{
					flLimitSpeed = 170;
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

		if (iWeaponAccuracyFlags & 8)
			iDistance *= 1.4;

		if (iWeaponAccuracyFlags & 0x10)
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

	if (m_iAlpha > 255)
		m_iAlpha = 255;

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

	// UNDONE : NVG
	/*if (gHUD::m_NightVision.m_fOn)
		DrawCrosshairEx(flTime, weaponid, iBarSize, flCrosshairDistance, false, 250, 50, 50, m_iAlpha);
	else
		DrawCrosshairEx(flTime, weaponid, iBarSize, flCrosshairDistance, m_bAdditive, m_R, m_G, m_B, m_iAlpha);*/

	return 1;
}

int CHudAmmo::DrawCrosshairEx(float flTime, int weaponid, int iBarSize, float flCrosshairDistance, bool bAdditive, int r, int g, int b, int a)
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

void CHudAmmo::Adjust_Crosshair(void)
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

void CHudAmmo::MsgFunc_CurWeapon(int& iState, int& iId, int& iClip)
{
	static wrect_t nullrc;
	int fOnTarget = FALSE;

	if (iState > 1)
		fOnTarget = TRUE;

	if (iId < 1 && !m_bObserverCrosshair)
	{
		gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
		return;
	}

	if (g_iUser1 != OBS_IN_EYE)
	{
		if ((iId == -1) && (iClip == -1))
		{
			gHUD::m_fPlayerDead = true;
			gpActiveSel = NULL;
			return;
		}

		gHUD::m_fPlayerDead = false;
	}

	WEAPON* pWeapon = gWR.GetWeapon(iId);

	if (!pWeapon)
		return;

	if (iClip < -1)
		pWeapon->iClip = abs(iClip);
	else
		pWeapon->iClip = iClip;

	if (iState == 0)
		return;

	m_pWeapon = pWeapon;

	if (gHUD::m_iFOV >= 90)
	{
		gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
	}
	else
	{
		if (fOnTarget && m_pWeapon->hZoomedAutoaim)
			gEngfuncs.pfnSetCrosshair(m_pWeapon->hZoomedAutoaim, m_pWeapon->rcZoomedAutoaim, 255, 255, 255);
		else
			gEngfuncs.pfnSetCrosshair(m_pWeapon->hZoomedCrosshair, m_pWeapon->rcZoomedCrosshair, 255, 255, 255);
	}

	m_fFade = 200.0f;
	m_bitsFlags |= HUD_ACTIVE;
	m_bObserverCrosshair = false;
}

void CHudAmmo::MsgFunc_HideWeapon(int& bits)
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
			{
				gpActiveSel = NULL;
				gEngfuncs.pfnSetCrosshair(0, nullrc, 0, 0, 0);
			}
		}
		else
		{
			if (m_pWeapon)
				gEngfuncs.pfnSetCrosshair(m_pWeapon->hCrosshair, m_pWeapon->rcCrosshair, 255, 255, 255);

			m_bObserverCrosshair = false;
		}
	}
}

void CHudAmmo::MsgFunc_Crosshair(bool bDrawn)
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
