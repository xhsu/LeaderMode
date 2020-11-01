/*

Created Date: Mar 11 2020

*/

#include "precompiled.h"



int CHudAmmo::Init(void)
{
	gHUD::AddHudElem(this);

	Reset();

	Q_memset(&m_rghAmmoSprite, NULL, sizeof(m_rghAmmoSprite));
	Q_memset(&m_rgrcAmmoSprite, NULL, sizeof(m_rgrcAmmoSprite));

	m_bitsFlags &= ~HUD_ACTIVE;	// LUNA: this is now abolished.
	return 1;
};

int CHudAmmo::VidInit(void)
{
	m_pTxtList = gEngfuncs.pfnSPR_GetList("sprites/ammo.txt", &m_iTxtListCount);

	if (m_pTxtList)
	{
		char sz[128];
		client_sprite_t* p = nullptr;

		for (int j = 0; j < AMMO_MAXTYPE; j++)
		{
			if (!g_rgAmmoInfo[j].m_pszName || g_rgAmmoInfo[j].m_pszName[0] == '\0')
				continue;

			p = gHUD::GetSpriteFromList(m_pTxtList, g_rgAmmoInfo[j].m_pszName, 640, m_iTxtListCount);

			if (p)
			{
				Q_snprintf(sz, charsmax(sz), "sprites/%s.spr", p->szSprite);
				m_rghAmmoSprite[j] = gEngfuncs.pfnSPR_Load(sz);
				m_rgrcAmmoSprite[j] = p->rc;
			}
			else
				m_rghAmmoSprite[j] = NULL;
		}
	}

	return 1;
}

int CHudAmmo::Draw(float flTime)
{
	static wrect_t nullrc;

	if (gEngfuncs.IsSpectateOnly())
		return 0;

	if (g_iUser1 != OBS_IN_EYE)
	{
		if (!(gHUD::m_iWeaponBits & (1 << (WEAPON_SUIT))))
			return 1;

		if ((gHUD::m_bitsHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)))
			return 1;
	}

	if (!g_pCurWeapon)
		return 0;

	if (g_iUser1 != OBS_IN_EYE)
	{
		int a, x, y, r, g, b;

		if (g_pCurWeapon->m_iPrimaryAmmoType <= AMMO_NONE || g_pCurWeapon->m_iPrimaryAmmoType >= AMMO_MAXTYPE)
			return 0;

		// reset the alpha to notify player that number was updated.
		if (m_iLastDrawnBpAmmo != gPseudoPlayer.m_rgAmmo[g_pCurWeapon->m_iPrimaryAmmoType] || m_iLastDrawnClip != g_pCurWeapon->m_iClip)
		{
			m_flAlpha = 200.0f;
			m_iLastDrawnBpAmmo = gPseudoPlayer.m_rgAmmo[g_pCurWeapon->m_iPrimaryAmmoType];
			m_iLastDrawnClip = g_pCurWeapon->m_iClip;
		}

		int iFlags = DHN_DRAWZERO;
		int AmmoWidth = gHUD::GetSpriteRect(gHUD::m_HUD_number_0).right - gHUD::GetSpriteRect(gHUD::m_HUD_number_0).left;

		a = Q_max(MIN_ALPHA, int(m_flAlpha));

		// fade out.
		if (m_flAlpha > 0)
			m_flAlpha -= (gHUD::m_flTimeDelta * 20);

		UnpackRGB(r, g, b, RGB_YELLOWISH);
		ScaleColors(r, g, b, a);

		y = ScreenHeight - gHUD::m_iFontHeight - gHUD::m_iFontHeight / 2;

		int iIconWidth = m_rgrcAmmoSprite[g_pCurWeapon->m_iPrimaryAmmoType].right - m_rgrcAmmoSprite[g_pCurWeapon->m_iPrimaryAmmoType].left;

		if (g_pCurWeapon->m_iClip >= 0)
		{
			x = ScreenWidth - (8 * AmmoWidth) - iIconWidth;
			x = gHUD::DrawHudNumber(x, y, iFlags | DHN_3DIGITS, g_pCurWeapon->m_iClip, r, g, b);

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
			x = gHUD::DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gPseudoPlayer.m_rgAmmo[g_pCurWeapon->m_iPrimaryAmmoType], r, g, b);
		}
		else
		{
			x = ScreenWidth - 4 * AmmoWidth - iIconWidth;
			x = gHUD::DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gPseudoPlayer.m_rgAmmo[g_pCurWeapon->m_iPrimaryAmmoType], r, g, b);
		}

		int iOffset = (m_rgrcAmmoSprite[g_pCurWeapon->m_iPrimaryAmmoType].bottom - m_rgrcAmmoSprite[g_pCurWeapon->m_iPrimaryAmmoType].top) / 8;
		gEngfuncs.pfnSPR_Set(m_rghAmmoSprite[g_pCurWeapon->m_iPrimaryAmmoType], r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y - iOffset, &m_rgrcAmmoSprite[g_pCurWeapon->m_iPrimaryAmmoType]);
	}

	return 1;
}

void CHudAmmo::Reset(void)
{
	m_flAlpha = 255;
	m_bitsFlags &= ~HUD_ACTIVE;	// LUNA: this is now abolished.
	m_iLastDrawnBpAmmo = -1;
	m_iLastDrawnClip = -1;
}
