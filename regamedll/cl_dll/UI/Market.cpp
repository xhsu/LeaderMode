/*

Created Date: May 31 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#include "precompiled.h"

using namespace vgui;


CMarket::CMarket() : BaseClass(nullptr, "Market")
{
	SetTitle("", true);
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);

	// This element should be constructed in IClientVGUI::Start()
	// Hence this calling is gurenteed safe.
	int iScreenWidth = 0, iScreenTall = 0;
	VGUI_SURFACE->GetScreenSize(iScreenWidth, iScreenTall);
	SetBounds(MARGIN, MARGIN, iScreenWidth - MARGIN * 2, iScreenTall - MARGIN * 2);

	int iPanelWidth = 0, iPanelTall = 0;
	GetSize(iPanelWidth, iPanelTall);

	SetTitleBarVisible(false);
	SetProportional(true);

	SetVisible(false);

	// We need to scroll.
	//m_pPurchasablePanel = new ScrollableEditablePanel(this, new EditablePanel(nullptr, "PurchasablePanel_EP"), "PurchasablePanel_SEP");
	//m_pPurchasablePanel->SetBounds(0, 0, iPanelWidth, iPanelTall);

	auto pButton1 = new LMImageButton(this, "AR15", L"Colt AR-15", this, "buyweapon weapon_ar15");
	pButton1->SetVisible(true);
	pButton1->SetUpImage("sprites/Weapons/AR15.dds");
	pButton1->AddGlyphSetToFont("Trajan Pro", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
	pButton1->AddGlyphSetToFont("I.MingCP", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
	pButton1->SetPos(0, 0);
	pButton1->SetSizeByImageHeight(WPN_SPRITE_HEIGHT);
	pButton1->InvalidateLayout(true);
}
