/*

Copied Date: May 21 2021 [Mind-Team]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <vgui_controls/Panel.h>
#include "tri.h"

// Helper template class
template <class CControl>
struct CViewportPanelHelper
{
	// As long as this class has no VFT, you can use static_cast. Otherwise, use dynamic_cast.
	CViewportPanelHelper() : m_pAnotherSelf(static_cast<CControl*>(this)) {}

	//-----------------------------------------------------------------------------
	// Purpose: Paint an additional frame outside the button.
	//-----------------------------------------------------------------------------
	template<class T> void DrawMouseHoveredEffects(T* pButton) const
	{
		int x = 0, y = 0, w = 0, h = 0;
		pButton->GetPos(x, y);
		pButton->GetSize(w, h);
		x -= CControl::MARGIN_BETWEEN_FRAME_AND_BUTTON + CControl::WIDTH_FRAME;
		y -= CControl::MARGIN_BETWEEN_FRAME_AND_BUTTON + CControl::WIDTH_FRAME;

		DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, 0xFF);

		// Top-left
		DrawUtils::Draw2DQuadNoTex(x, y, x + CControl::LENGTH_FRAME, y + CControl::WIDTH_FRAME);
		DrawUtils::Draw2DQuadNoTex(x, y, x + CControl::WIDTH_FRAME, y + CControl::LENGTH_FRAME);

		// Top-right
		pButton->GetPos(x, y);
		x += w + CControl::MARGIN_BETWEEN_FRAME_AND_BUTTON + CControl::WIDTH_FRAME;
		y -= CControl::MARGIN_BETWEEN_FRAME_AND_BUTTON + CControl::WIDTH_FRAME;
		DrawUtils::Draw2DQuadNoTex(x, y, x - CControl::LENGTH_FRAME, y + CControl::WIDTH_FRAME);
		DrawUtils::Draw2DQuadNoTex(x, y, x - CControl::WIDTH_FRAME, y + CControl::LENGTH_FRAME);

		DrawUtils::glRegularPureColorDrawingExit();
	}

	void Show(bool bShow)
	{
		if (m_pAnotherSelf->IsVisible() == bShow)	// The 'BaseClass' should be defined by the class on which this template applies.
			return;

		if (bShow)
		{
			m_pAnotherSelf->Activate();

			if (m_pAnotherSelf->GetParent()->IsVisible())
			{
				m_pAnotherSelf->SetMouseInputEnabled(true);
				IN_DeactivateMouse();
			}
		}
		else
		{
			m_pAnotherSelf->SetVisible(false);

			// Only do this when IClientVGUI is visible.
			// Or this line will break the pause screen.
			if (m_pAnotherSelf->GetParent()->IsVisible())
			{
				m_pAnotherSelf->SetMouseInputEnabled(false);
				IN_ActivateMouse();
			}
		}
	}

private:
	CControl* m_pAnotherSelf{ nullptr };
};

// UI Control
#include "UI/LM_ImageButton.h"

// My own elements.
#include "UI/TeamSelection.h"
#include "UI/RoleSelection.h"

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
	void HideAllVGUIMenus(void);

protected:
	void PaintBackground(void) final;
	void OnCommand(const char* szCommand) final;

public:
	CTeamMenu* m_pTeamMenu{ nullptr };
	CRoleMenu* m_pRoleMenu{ nullptr };
};

}

extern vgui::CViewPort *g_pViewport;

#endif
