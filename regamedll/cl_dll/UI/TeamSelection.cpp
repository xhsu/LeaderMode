/*

Created Date: May 24 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#include "precompiled.h"

using namespace vgui;

CTeamMenu::CTeamMenu(void) : Frame(nullptr, "TeamMenu")
{
	SetTitle("", true);
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);

	// This element should be constructed in IClientVGUI::Start()
	// Hence this calling is gurenteed safe.
	int iScreenWidth = 0, iScreenTall = 0;
	VGUI_SURFACE->GetScreenSize(iScreenWidth, iScreenTall);
	SetBounds(MARGIN, MARGIN, iScreenWidth - MARGIN * 2, iScreenTall - MARGIN * 2 - 12);

	SetTitleBarVisible(false);
	SetProportional(true);

	SetVisible(false);

	Vector2D vecTButtonPos = Vector2D(iScreenWidth, iScreenTall) / 2 - Vector2D(MARGIN_BUTTON, BUTTON_SIZE) / 2 - Vector2D(BUTTON_SIZE, 0);
	Vector2D vecCTButtonPos = Vector2D(iScreenWidth, iScreenTall) / 2 + Vector2D(MARGIN_BUTTON, -BUTTON_SIZE) / 2;

	m_pButtonT = new CTeamButton(this, "SelectT", L"恐怖分子", this, "jointeam 1");
	m_pButtonT->SetBounds(vecTButtonPos.x, vecTButtonPos.y, BUTTON_SIZE, BUTTON_SIZE+24);
	m_pButtonT->SetVisible(true);
	m_pButtonT->SetUpImage("sprites/ClassesIcon/T/Godfather.dds");
	m_pButtonT->SetFocusImage("sprites/ClassesIcon/T/Godfather.dds");
	m_pButtonT->SetDownImage("sprites/ClassesIcon/T/Godfather.dds");
	m_pButtonT->SetDisableImage("sprites/ClassesIcon/T/Godfather.dds");

	m_pButtonCT = new CTeamButton(this, "SelectCT", L"反恐部隊", this, "jointeam 2");
	m_pButtonCT->SetBounds(vecCTButtonPos.x, vecCTButtonPos.y, BUTTON_SIZE, BUTTON_SIZE+24);
	m_pButtonCT->SetVisible(true);
	m_pButtonCT->SetUpImage("sprites/ClassesIcon/CT/Commander.dds");
	m_pButtonCT->SetFocusImage("sprites/ClassesIcon/CT/Commander.dds");
	m_pButtonCT->SetDownImage("sprites/ClassesIcon/CT/Commander.dds");
	m_pButtonCT->SetDisableImage("sprites/ClassesIcon/CT/Commander.dds");

	/*m_pButtonObserver = new CTeamButton(this, "SelectS", L"SelectS", this, "jointeam 3");
	m_pButtonObserver->SetBounds(64, 64 + BUTTON_SIZE * 2, BUTTON_SIZE, BUTTON_SIZE);
	m_pButtonObserver->SetVisible(true);
	m_pButtonObserver->SetUpImage("sprites/Inventory/NVG.dds");
	m_pButtonObserver->SetFocusImage("sprites/Inventory/NVG.dds");
	m_pButtonObserver->SetDownImage("sprites/Inventory/NVG.dds");
	m_pButtonObserver->SetDisableImage("sprites/Inventory/NVG.dds");*/
}

CTeamMenu::~CTeamMenu(void)
{
}

void CTeamMenu::OnCommand(const char* command)
{
	Show(false);

	if (Q_strstr(command, "jointeam"))
		gEngfuncs.pfnServerCmd(command);
	else
		BaseClass::OnCommand(command);
}

void CTeamMenu::Show(bool bShow)
{
	if (BaseClass::IsVisible() == bShow)
		return;

	if (bShow)
	{
		Activate();
		SetMouseInputEnabled(true);
		IN_DeactivateMouse();
	}
	else
	{
		SetVisible(false);
		SetMouseInputEnabled(false);
		IN_ActivateMouse();
	}
}
