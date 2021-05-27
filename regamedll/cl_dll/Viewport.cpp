/*

Copied Date: May 21 2021 [Mind-Team]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#include "precompiled.h"

using namespace vgui;

CViewPort::CViewPort(void)
{
	m_pTeamMenu = new CTeamMenu();
	m_pTeamMenu->SetParent(this);

	m_pRoleMenu = new CRoleMenu();
	m_pRoleMenu->SetParent(this);
}

CViewPort::~CViewPort(void)
{
}

void vgui::CViewPort::OnActivateClientUI(void)
{
	this->SetVisible(true);

	if (IsAnyClientUIUsingMouse())
	{
		SetMouseInputEnabled(true);
	}
	else
	{
		IN_ActivateMouse();
	}
}

void vgui::CViewPort::OnHideClientUI(void)
{
	this->SetVisible(false);

	IN_DeactivateMouse();
}

bool vgui::CViewPort::IsAnyClientUIUsingMouse(void)
{
	return (m_pTeamMenu->IsVisible() || m_pRoleMenu->IsVisible());
}

void vgui::CViewPort::HideAllVGUIMenus(void)
{
	m_pRoleMenu->Show(false);
	m_pTeamMenu->Show(false);
}

void CViewPort::PaintBackground(void)
{
	//int swide, stall;
	//g_pVGuiSurface->GetScreenSize(swide, stall);
	//g_pVGuiSurface->DrawSetColor(0, 0, 60, 128);
	//g_pVGuiSurface->DrawFilledRect(0, 0, swide, stall);
}

void CViewPort::OnCommand(const char* szCommand)
{
	//if (!Q_strcmp(szCommand, "showteam"))
	//	m_pTeamMenu->Show(!m_pTeamMenu->IsVisible());
}
