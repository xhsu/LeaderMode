/*

Created Date: Mar 11 2020

*/

#include "cl_base.h"

int CHudBattery::Init(void)
{
	m_iBat = 0;
	m_fFade = 0;
	m_bitsFlags = 0;
	m_iArmorType = 0;

	gHUD::AddHudElem(this);

	return 1;
}

int CHudBattery::VidInit(void)
{
	m_HUD_suit_empty = gHUD::GetSpriteIndex("suit_empty");
	m_HUD_suit_full = gHUD::GetSpriteIndex("suit_full");

	m_HUD_suithelmet_empty = gHUD::GetSpriteIndex("suithelmet_empty");
	m_HUD_suithelmet_full = gHUD::GetSpriteIndex("suithelmet_full");

	m_iHeight = gHUD::GetSpriteRect(m_HUD_suit_full).bottom - gHUD::GetSpriteRect(m_HUD_suit_empty).top;
	m_fFade = 0;
	return 1;
}

int CHudBattery::Draw(float flTime)
{
	if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_HEALTH) || g_iUser1)
		return 1;

	int spriteEmpty, spriteFull;

	if (m_iArmorType >= 1)
	{
		spriteEmpty = m_HUD_suithelmet_empty;
		spriteFull = m_HUD_suithelmet_full;
	}
	else
	{
		spriteEmpty = m_HUD_suit_empty;
		spriteFull = m_HUD_suit_full;
	}

	hSprite hSpriteEmpty = gHUD::GetSprite(spriteEmpty);
	hSprite hSpriteFull = gHUD::GetSprite(spriteFull);
	wrect_t* prcEmpty = &gHUD::GetSpriteRect(spriteEmpty);
	wrect_t* prcFull = &gHUD::GetSpriteRect(spriteFull);

	int r, g, b, x, y, a;
	wrect_t rc = *prcFull;

	// battery can go from 0 to 100 so * 0.01 goes from 0 to 1
	rc.top += m_iHeight * ((float)(100 - (Q_min(100, m_iBat))) * 0.01f);

	UnpackRGB(r, g, b, RGB_YELLOWISH);

	if (!(gHUD::m_iWeaponBits & (1 << (WEAPON_SUIT))))
		return 1;

	if (m_fFade)
	{
		if (m_fFade > FADE_TIME)
			m_fFade = FADE_TIME;

		m_fFade -= (gHUD::m_flTimeDelta * 20);

		if (m_fFade <= 0)
		{
			a = 100;
			m_fFade = 0;
		}

		a = MIN_ALPHA + (m_fFade / FADE_TIME) * 128;

	}
	else
		a = MIN_ALPHA;

	ScaleColors(r, g, b, a);

	y = ScreenHeight - gHUD::m_iFontHeight - gHUD::m_iFontHeight / 2;
	x = ScreenWidth / 5;

	gEngfuncs.pfnSPR_Set(hSpriteEmpty, r, g, b);
	gEngfuncs.pfnSPR_DrawAdditive(0, x, y, prcEmpty);

	if (rc.bottom > rc.top)
	{
		gEngfuncs.pfnSPR_Set(hSpriteFull, r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y + (rc.top - prcFull->top), &rc);
	}

	x += (prcEmpty->right - prcEmpty->left);
	x = gHUD::DrawHudNumber(x, y, DHN_3DIGITS | DHN_DRAWZERO, m_iBat, r, g, b);

	return 1;
}

void CHudBattery::MsgFunc_Battery(int& iNewArmourValue)
{
	m_bitsFlags |= HUD_ACTIVE;

	if (iNewArmourValue != m_iBat)
	{
		m_fFade = FADE_TIME;
		m_iBat = iNewArmourValue;

		if (iNewArmourValue <= 0)
			m_iArmorType = 0;
	}
}

void CHudBattery::MsgFunc_ArmorType(int& iArmourType)
{
	m_iArmorType = iArmourType;
}
