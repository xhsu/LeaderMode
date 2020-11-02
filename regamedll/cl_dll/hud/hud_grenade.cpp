/*

Created Date: Mar 23 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Matoilet

*/

#include "precompiled.h"

int CHudGrenade::Init(void)
{
	gHUD::AddHudElem(this);

	Q_memset(&m_rghGrenadeIcons, NULL, sizeof(m_rghGrenadeIcons));
	Q_memset(&m_rgrcGrenadeIcons, NULL, sizeof(m_rgrcGrenadeIcons));

	m_bitsFlags = HUD_ACTIVE;
	return 1;
}

int CHudGrenade::VidInit(void)
{
	// this HUD is actually depent on gHUD::m_Ammo.
	// therefore, this HUD must be loaded AFTER gHUD::m_Ammo.

	// reset our database.
	Q_memset(&m_rghGrenadeIcons, NULL, sizeof(m_rghGrenadeIcons));
	Q_memset(&m_rgrcGrenadeIcons, NULL, sizeof(m_rgrcGrenadeIcons));

	// find ammo id first, and we can get a SPR from gHUD::m_Ammo.
	AmmoIdType iAmmoId = AMMO_NONE;
	for (int i = EQP_NONE; i < EQP_COUNT; i++)
	{
		iAmmoId = GetAmmoIdOfEquipment((EquipmentIdType)i);

		if (iAmmoId)
		{
			m_rghGrenadeIcons[i] = gHUD::m_Ammo.m_rghAmmoSprite[iAmmoId];
			m_rgrcGrenadeIcons[i] = gHUD::m_Ammo.m_rgrcAmmoSprite[iAmmoId];
		}
		else
		{
			auto p = gHUD::GetSpriteFromList(gHUD::m_Ammo.m_pTxtList, g_rgEquipmentInfo[i].m_pszInternalName, 640, gHUD::m_Ammo.m_iTxtListCount);
			gHUD::GetSprite(p, m_rghGrenadeIcons[i], m_rgrcGrenadeIcons[i]);
		}
	}

	return 1;
}

int CHudGrenade::Draw(float flTime)
{
	int x = ScreenWidth / 2;
	int y = ScreenHeight - 24;	// 24 is the normal height of a GR icon.

	int r = 255, g = 255, b = 255;
	int iAlpha = 255, iAlphaStep = 255;
	int iIconWidth = 24;

	AmmoIdType iAmmoId = AMMO_NONE;

	for (int i = EQP_NONE; i < EQP_COUNT; i++)
	{
		// we can't draw voidness. TODO: maybe a default icon later??
		if (!m_rghGrenadeIcons[i])
			continue;

		iAmmoId = GetAmmoIdOfEquipment((EquipmentIdType)i);	// no grenade, no drawing.
		if ((!iAmmoId || gPseudoPlayer.m_rgAmmo[iAmmoId] <= 0) && !gPseudoPlayer.m_rgbHasEquipment[i])	// Alternatively, if this is a special usable item, you can take it on your hand.
			continue;

		y = ScreenHeight - (m_rgrcGrenadeIcons[i].bottom - m_rgrcGrenadeIcons[i].top);	// Y is not a constant, it depents on icon.
		iIconWidth = m_rgrcGrenadeIcons[i].right - m_rgrcGrenadeIcons[i].left;

		iAlphaStep = 255 / Q_max(gPseudoPlayer.m_rgAmmo[iAmmoId], 1);	// CAREFUL! don't divide it by naught!

		for (iAlpha = 255; iAlpha > 0; iAlpha -= iAlphaStep)
		{
			UnpackRGB(r, g, b, gPseudoPlayer.m_iUsingGrenadeId == i ? 0xFFFFFF : RGB_YELLOWISH);
			ScaleColors(r, g, b, iAlpha);

			gEngfuncs.pfnSPR_Set(m_rghGrenadeIcons[i], r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &m_rgrcGrenadeIcons[i]);
			x += iIconWidth / 2;
		}

		// for the last one, give some space for next set of icons.
		x += iIconWidth;
	}

	return 1;
}
