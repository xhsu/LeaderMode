/*

Created Date: May 24 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#pragma once

class CTeamMenu : public vgui::Frame, public CViewportPanelHelper<CTeamMenu>
{
	DECLARE_CLASS_SIMPLE(CTeamMenu, vgui::Frame);

public:
	CTeamMenu(void);
	virtual ~CTeamMenu(void);

	void OnCommand(const char* command) final;
	void OnThink(void) final;
	void Paint(void) final;
	void PaintBackground(void) final;

private:
	vgui::LMImageButton* m_pButtonT, * m_pButtonCT, * m_pButtonObserver;
	vgui::image_t m_CTImage, m_TImage;
	CPanelAnimationVar(Color, m_CTColor, "m_CTColor", "255,255,255,128");
	CPanelAnimationVar(Color, m_TColor, "m_TColor", "255,255,255,128");
	bool m_bPendingOnCT : 1;
	bool m_bPendingOnT : 1;

public:
	static constexpr auto LENGTH_FRAME = 36, WIDTH_FRAME = 4, MARGIN_BETWEEN_FRAME_AND_BUTTON = 12;
	static constexpr auto MARGIN = 2, MARGIN_BUTTON = 256;
	static constexpr auto BUTTON_SIZE = 256, FONT_SIZE = 48;
	static constexpr auto BUTTON_FADING_TIME = 0.5f;
	static constexpr auto BUTTON_IDLE_COLOR = Color(0xFFFFFF, 0x7F), BUTTON_CT_COLOR = Color(RGB_CT_COLOUR, 0xFF), BUTTON_T_COLOR = Color(RGB_T_COLOUR, 0xFF);
	static inline vgui::image_t m_sPlayerCountIcon;
};
