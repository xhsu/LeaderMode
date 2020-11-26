/*

Created Date: Mar 11 2020

*/

#include "precompiled.h"

int CHudBattery::Init(void)
{
	m_flArmour = 0;
	m_fFade = 0;
	m_bitsFlags = 0;
	m_iArmorType = 0;
	m_flDrawingArmour = 0;

	gHUD::AddHudElem(this);

	return 1;
}

int CHudBattery::VidInit(void)
{
	m_HUD_suit_empty = gHUD::GetSpriteIndex("suit_empty");
	m_HUD_suit_full = gHUD::GetSpriteIndex("suit_full");

	m_HUD_suithelmet_empty = gHUD::GetSpriteIndex("suithelmet_empty");
	m_HUD_suithelmet_full = gHUD::GetSpriteIndex("suithelmet_full");

	m_iHeight = gHUD::GetSpriteRect(m_HUD_suit_full)->bottom - gHUD::GetSpriteRect(m_HUD_suit_empty)->top;
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
	const wrect_t* prcEmpty = gHUD::GetSpriteRect(spriteEmpty);
	const wrect_t* prcFull = gHUD::GetSpriteRect(spriteFull);

	int r, g, b, x, y, a;
	wrect_t rc = *prcFull;

	// battery can go from 0 to 100 so * 0.01 goes from 0 to 1
	float flPercent = m_flDrawingArmour / GetMaxArmour();
	rc.top += m_iHeight * (1.0f - flPercent);
	m_flDrawingArmour += (m_flArmour - m_flDrawingArmour) * gHUD::m_flTimeDelta * 5.0f;

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

	UnpackRGB(r, g, b, RGB_YELLOWISH);
	ScaleColors(r, g, b, a);

	x = HEALTH_BASIC_OFS;//ScreenWidth / 5;
	y = gHUD::m_Health.m_flLastDrawingY - (prcEmpty->bottom - prcEmpty->top) - HEALTH_BASIC_OFS;//ScreenHeight - gHUD::m_iFontHeight - gHUD::m_iFontHeight / 2;

	gEngfuncs.pfnSPR_Set(hSpriteEmpty, r, g, b);
	gEngfuncs.pfnSPR_DrawAdditive(0, x, y, prcEmpty);

	// Lock the Y here, since this is the highest point of this HUD.
	m_flLastDrawingY = y;

	if (rc.bottom > rc.top)
	{
		gEngfuncs.pfnSPR_Set(hSpriteFull, r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y + (rc.top - prcFull->top), &rc);
	}

	x += (prcEmpty->right - prcEmpty->left) + HEALTH_ICON_BAR_INTERSPACE;
	y += (prcEmpty->bottom - prcEmpty->top - HEALTH_BAR_WIDTH) / 2;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (m_iArmorType >= 1)
		UnpackRGB(r, g, b, RGB_SPRINGGREENISH);
	else
		UnpackRGB(r, g, b, RGB_YELLOWISH);	// we can't use scaled colour on our bar!

	glColor4f(r / 255.0, g / 255.0, b / 255.0, a / 255.0);

	float flLength = HEALTH_BAR_LENGTH;
	if (g_iRoleType == Role_SWAT)
		flLength *= 2.0f;

	DrawUtils::Draw2DLinearProgressBar(x, y, HEALTH_BAR_WIDTH, flLength, flPercent);

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	return 1;
}

void CHudBattery::Reset(void)
{
	m_flArmour = 0;
	m_flDrawingArmour = 0;
	m_iArmorType = 0;
}

void CHudBattery::InitHUDData(void)
{
	Reset();
}

void CHudBattery::MsgFunc_Battery(int& iNewArmourValue)
{
	m_bitsFlags |= HUD_ACTIVE;

	if (iNewArmourValue != m_flArmour)
	{
		m_fFade = FADE_TIME;
		m_flArmour = iNewArmourValue;

		if (iNewArmourValue <= 0)
			m_iArmorType = 0;
	}
}

void CHudBattery::MsgFunc_ArmorType(int& iArmourType)
{
	m_iArmorType = iArmourType;
}

float CHudBattery::GetMaxArmour(void)
{
	if (g_iRoleType == Role_SWAT)
		return 200.0f;

	return 100.0f;
}
