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
		//gEngfuncs.pfnSetMouseEnable(true);
		//VGUI_SURFACE->LockCursor();

	}
	else
	{
		gHUD::m_UI_BuyMenu.m_bitsFlags |= IN_ACTIVE;
		IN_DeactivateMouse();
		//gEngfuncs.pfnSetMouseEnable(false);
		//VGUI_SURFACE->UnlockCursor();
	}
}

int CUIBuyMenu::Init(void)
{
	m_bitsFlags = 0;

	gEngfuncs.pfnAddCommand("buy", CommandFunc_Buy);

	// add children here.
	m_Baseboard.AddChild(&m_Button_M4A1);

	gHUD::AddHudElem(this);
	return m_Baseboard.Initialize();
}

int CUIBuyMenu::VidInit(void)
{
	int hFont = gFontFuncs.CreateFont();
	gFontFuncs.AddGlyphSetToFont(hFont, "HYTangLiF", 20, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0xFFFF);

	// baseboard
	m_Baseboard.m_bitsFlags |= HUD_ACTIVE;
	m_Baseboard.SetAlpha(192);

	m_Baseboard.m_vecCoord.x = float(ScreenWidth) / 4.0f / 2.0f;
	m_Baseboard.m_vecCoord.y = float(ScreenHeight) / 4.0f / 2.0f;

	m_Baseboard.m_flWidth = float(ScreenWidth) * 3.0f / 4.0f;
	m_Baseboard.m_flHeight = float(ScreenHeight) * 3.0f / 4.0f;

	// M4A1 button.
	m_Button_M4A1.m_bitsFlags |= HUD_ACTIVE;
	m_Button_M4A1.m_Text.SetANSI("M4A1 Sopmod II");
	m_Button_M4A1.m_Text.SetFont(gHUD::m_hCambriaFont, 24);
	m_Button_M4A1.m_Text.SetColour(0xFFFFFF, 255);
	m_Button_M4A1.m_Background.SetColour(0xCCCCCC, 160);
	m_Button_M4A1.SetSize(192, 36);	// place it behind the m_Text settings.
	m_Button_M4A1.SetGap(2);	// place it bebind the SetSize().
	m_Button_M4A1.SetColour(0xFFFFFF, 255);
	m_Button_M4A1.SetCommand(SharedVarArgs("buyweapon %d\n", WEAPON_M4A1));

	return m_Baseboard.VidInit();
}
