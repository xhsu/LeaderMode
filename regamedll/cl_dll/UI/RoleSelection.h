/*

Created Date: May 26 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#pragma once


class CRoleMenu : public vgui::Frame, public CViewportPanelHelper<CRoleMenu>
{
	friend struct CViewportPanelHelper;

	DECLARE_CLASS_SIMPLE(CRoleMenu, vgui::Frame);

public:
	CRoleMenu(void);
	virtual ~CRoleMenu(void);

public:
	void OnThink(void) final;
	void Paint(void) final;
	void OnCommand(const char* command) final;
	void OnKeyCodeTyped(vgui::KeyCode code) final;

private:
	std::array<vgui::LMImageButton*, ROLE_COUNT> m_rgpButtons;
	unsigned short m_iLastTeam{ 65535 };
	RoleTypes m_iSelectedRole{ ROLE_COUNT };	// Initialize with an error value.

public:
	static constexpr auto LENGTH_FRAME = 18, WIDTH_FRAME = 2, MARGIN_BETWEEN_FRAME_AND_BUTTON = 12;
	static constexpr auto MARGIN = 2;
	static constexpr auto BUTTON_SIZE = 128, FONT_SIZE = 24, INTRO_REGION_WIDTH = 192;
	static inline std::array<vgui::image_t, ROLE_COUNT> CLASS_PORTRAITS;
};

