/*

Copied Date: May 21 2021 [CS:BTE Alpha]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL
	Advisor		- Crsky

*/

#include "precompiled.h"

using namespace vgui;

CBackgroundMenuButton::CBackgroundMenuButton(vgui::Panel* parent, const char* name) : BaseClass(parent, name, "")
{
	m_pImage = NULL;
	m_pMouseOverImage = NULL;
}

CBackgroundMenuButton::~CBackgroundMenuButton(void)
{
}

void CBackgroundMenuButton::SetVisible(bool state)
{
	BaseClass::SetVisible(true);
}

void CBackgroundMenuButton::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetFgColor(Color(255, 255, 255, 255));
	SetBgColor(Color(0, 0, 0, 0));
	SetDefaultColor(Color(255, 255, 255, 255), Color(0, 0, 0, 0));
	SetArmedColor(Color(255, 255, 0, 255), Color(0, 0, 0, 0));
	SetDepressedColor(Color(255, 255, 0, 255), Color(0, 0, 0, 0));
	SetContentAlignment(Label::a_west);
	SetBorder(NULL);
	SetDefaultBorder(NULL);
	SetDepressedBorder(NULL);
	SetKeyFocusBorder(NULL);
	SetTextInset(0, 0);
	SetAlpha(0);

	SetArmedSound("UI/buttonrollover.wav");
	SetDepressedSound("UI/buttonclick.wav");
	SetReleasedSound("UI/buttonclickrelease.wav");
}

void CBackgroundMenuButton::OnKillFocus(void)
{
	BaseClass::OnKillFocus();
}

void CBackgroundMenuButton::OnCommand(const char* command)
{
	BaseClass::OnCommand(command);
}
