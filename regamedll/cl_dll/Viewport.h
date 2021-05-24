/*

Copied Date: May 21 2021 [Mind-Team]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <vgui_controls/Panel.h>

// UI Control
#include "UI/LMUI_TeamButton.h"

// My own elements.
#include "UI/TeamSelection.h"

namespace vgui
{

class CViewPort : public Panel
{
	DECLARE_CLASS_SIMPLE(CViewPort, Panel);

public:
	CViewPort(void);
	virtual ~CViewPort(void);

	void OnActivateClientUI(void);
	void OnHideClientUI(void);
	bool IsAnyClientUIUsingMouse(void);

protected:
	void PaintBackground(void) final;
	void OnCommand(const char* szCommand) final;

public:
	CTeamMenu* m_pTeamMenu{ nullptr };
};

}

extern vgui::CViewPort *g_pViewport;

#endif
