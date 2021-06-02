/*

Created Date: May 31 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#pragma once

#include <array>
#include <string>

constexpr const char* g_rgpszWeaponSprites[LAST_WEAPON] =
{
	// Placeholder
	"",

	// Pistols
	"",
	"",
	"",
	"sprites/Weapons/Deagle.dds",
	"sprites/Weapons/FiveseveN.dds",
	"sprites/Weapons/M45A1.dds",

	// Shotguns
	"",
	"sprites/Weapons/M1014.dds",
	"",

	// SMGs
	"",
	"",
	"",
	"sprites/Weapons/UMP45.dds",
	"",
	"",

	// Assault Rifles
	"",
	"sprites/Weapons/AR15.dds",
	"sprites/Weapons/SCARH.dds",
	"",//"sprites/Weapons/XM8.dds",	// UNDONE

	// Marksman Rifles
	"",
	"",
	"sprites/Weapons/L115A1.dds",
	"sprites/Weapons/PSG1.dds",

	// LMGs
	"sprites/Weapons/MK46.dds",
	"",
};

constexpr const char* g_rgpszMarketCategoryNames[] =
{
	"#LeaderMode_Pistols",
	"#LeaderMode_Shotguns",
	"#LeaderMode_SMGs",
	"#LeaderMode_ARs",
	"#LeaderMode_SRs",
	"#LeaderMode_LMGs",

	"Armour",
	"Throwables",
	"Equipments",
};

constexpr const char* g_rgpszEquipmentSprites[EQP_COUNT] =
{
	// Placeholder
	"sprites/ClassesIcon/Doraemon.dds",

	// armour
	"sprites/Inventory/Kevlar.dds",
	"sprites/Inventory/AssaultSuit.dds",

	// grenades
	"sprites/Inventory/HE.dds",
	"sprites/Inventory/Flashbang.dds",
	"sprites/Inventory/SmokeGrenade.dds",
	"sprites/Inventory/Cryo.dds",
	"sprites/Inventory/Molotov.dds",
	"sprites/Inventory/HealingGrenade.dds",
	"sprites/Inventory/NerveGas.dds",
	"sprites/Inventory/C4.dds",

	// misc
	"sprites/Inventory/Detonator.dds",
	"sprites/Inventory/NVG.dds",
	"sprites/Inventory/Flashlight.dds",
};

extern std::array<std::wstring, _countof(g_rgpszMarketCategoryNames)> g_rgwcsLocalisedCtgyNames;


class CMarket : public vgui::Frame, public CViewportPanelHelper<CMarket>
{
	friend struct CViewportPanelHelper;

	DECLARE_CLASS_SIMPLE(CMarket, vgui::Frame);

public:
	CMarket();
	~CMarket() override {}

public:
	void Paint(void) final;
	void OnCommand(const char* szCommand) final;
	void InvalidateLayout(bool layoutNow = false, bool reloadScheme = false) final;

public:
	void UpdateMarket(void);

public:
	static constexpr auto LENGTH_FRAME = 18, WIDTH_FRAME = 2, MARGIN_BETWEEN_FRAME_AND_BUTTON = 12;
	static constexpr auto MARGIN = 2, MARGIN_BETWEEN_BUTTONS = 36, MARGIN_BETWEEN_BUTTON_AND_TEXT = MARGIN_BETWEEN_BUTTONS;
	static constexpr auto WPN_SPRITE_HEIGHT = 96, FONT_SIZE = 24;
	static inline int s_hFont = 0;

public:
	std::array<vgui::LMImageButton*, LAST_WEAPON + EQP_COUNT> m_rgpButtons;
	std::vector<vgui::Button*> m_rgpCategoryButtons;
	vgui::ScrollableEditablePanel* m_pScrollablePanel{ nullptr };
	vgui::EditablePanel* m_pPurchasablePanel{ nullptr };
	int m_iTextMaxiumWidth{ 0 }, m_iExhibitionTableWidth{ 0 };
};
