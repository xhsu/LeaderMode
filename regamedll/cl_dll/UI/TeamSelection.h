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
private:
	DECLARE_CLASS_SIMPLE(CTeamMenu, vgui::Frame);

public:
	CTeamMenu(void);
	virtual ~CTeamMenu(void);

	void OnCommand(const char* command) final;

public:	// Custom new functions.
	void Show(bool bShow);


private:
	vgui::CTeamButton* m_pButtonT, * m_pButtonCT, * m_pButtonObserver, * m_pButtonAuto;

public:
	static constexpr auto MARGIN = 2, MARGIN_BUTTON = 256;
	static constexpr auto BUTTON_SIZE = 256;
};
