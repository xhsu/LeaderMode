/*

Created Date: May 24 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#pragma once

class CTeamMenu : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CTeamMenu, vgui::Frame);

public:
	CTeamMenu(void);
	virtual ~CTeamMenu(void);

	void OnCommand(const char* command) final;
	void OnThink(void) final;
	void Paint(void) final;

public:	// Custom new functions.
	void Show(bool bShow);

private:
	vgui::LMImageButton* m_pButtonT, * m_pButtonCT, * m_pButtonObserver, * m_pButtonAuto;
	unsigned short m_iHighlightedButton{ 0 };

public:
	static constexpr auto LENGTH_FRAME = 36, WIDTH_FRAME = 4, MARGIN_BETWEEN_FRAME_AND_BUTTON = 12;
	static constexpr auto MARGIN = 2, MARGIN_BUTTON = 256;
	static constexpr auto BUTTON_SIZE = 256, FONT_SIZE = 48;
	static inline vgui::image_t m_sPlayerCountIcon;
};
