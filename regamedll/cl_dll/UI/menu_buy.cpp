/*

Created Date: Mar 27 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#include "precompiled.h"

void CommandFunc_Buy(void)
{
	if (gHUD::m_UI_BuyMenu.m_bitsFlags & IN_ACTIVE)
	{
		gHUD::m_UI_BuyMenu.m_bitsFlags &= ~IN_ACTIVE;
		IN_ActivateMouse();
		gEngfuncs.pfnSetMouseEnable(true);
	}
	else
	{
		gHUD::m_UI_BuyMenu.m_bitsFlags |= IN_ACTIVE;
		IN_DeactivateMouse();
		gEngfuncs.pfnSetMouseEnable(false);
	}
}

int CUIBuyMenu::Init(void)
{
	m_bitsFlags = 0;

	gEngfuncs.pfnAddCommand("buy", CommandFunc_Buy);

	gHUD::AddHudElem(this);
	return m_Baseboard.Initialize();
}

int CUIBuyMenu::VidInit(void)
{
	m_Baseboard.m_bitsFlags |= HUD_ACTIVE;
	m_Baseboard.SetAlpha(192);

	m_Baseboard.m_vecCoord.x = float(ScreenWidth) / 4.0f / 2.0f;
	m_Baseboard.m_vecCoord.y = float(ScreenHeight) / 4.0f / 2.0f;

	m_Baseboard.m_flWidth = float(ScreenWidth) * 3.0f / 4.0f;
	m_Baseboard.m_flHeight = float(ScreenHeight) * 3.0f / 4.0f;

	return m_Baseboard.VidInit();
}
