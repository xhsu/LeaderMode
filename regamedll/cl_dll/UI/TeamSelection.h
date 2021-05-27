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
	void PaintBackground(void) final;

public:	// Custom new functions.
	void Show(bool bShow);

private:
	vgui::LMImageButton* m_pButtonT, * m_pButtonCT, * m_pButtonObserver, * m_pButtonAuto;
	unsigned short m_iHighlightedButton{ 0 };
	std::array<vgui::image_t, 2> m_CTBackgrounds, m_TERBackgrounds;
	std::array<float, 4> m_flBackgroundAlphas, m_flBackgroundAlphaGoals;	// both calculated in bytes.
	std::array<Vector, 4> m_vecBackgroundColors, m_vecBackgroundColorGoals;	// HEX code.

public:
	static constexpr auto LENGTH_FRAME = 36, WIDTH_FRAME = 4, MARGIN_BETWEEN_FRAME_AND_BUTTON = 12;
	static constexpr auto MARGIN = 2, MARGIN_BUTTON = 256;
	static constexpr auto BUTTON_SIZE = 256, FONT_SIZE = 48;
	static inline vgui::image_t m_sPlayerCountIcon;
};
