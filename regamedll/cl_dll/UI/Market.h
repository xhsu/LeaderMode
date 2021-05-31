/*

Created Date: May 31 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#pragma once


class CMarket : public vgui::Frame, public CViewportPanelHelper<CMarket>
{
	friend struct CViewportPanelHelper;

	DECLARE_CLASS_SIMPLE(CMarket, vgui::Frame);

public:
	CMarket();
	~CMarket() override {}

public:
	static constexpr auto LENGTH_FRAME = 18, WIDTH_FRAME = 2, MARGIN_BETWEEN_FRAME_AND_BUTTON = 12;
	static constexpr auto MARGIN = 2;
	static constexpr auto WPN_SPRITE_HEIGHT = 96, FONT_SIZE = 24;
	static inline int s_hFont = 0;

private:
	vgui::ScrollableEditablePanel* m_pPurchasablePanel{ nullptr };
};
