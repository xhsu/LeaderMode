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

// Helper template class. Use it with classes inherit from vgui::Panel.
template <class CControl>
struct CViewportPanelHelper
{
	//-----------------------------------------------------------------------------
	// Purpose: Paint an additional frame outside the button.
	//-----------------------------------------------------------------------------
	template<class T> void DrawMouseHoveredEffects(T* pButton)
	{
		int x = 0, y = 0, w = 0, h = 0;
		pButton->LocalToScreen(x, y);
		Panel()->ScreenToLocal(x, y);
		pButton->GetSize(w, h);
		x -= CControl::MARGIN_BETWEEN_FRAME_AND_BUTTON + CControl::WIDTH_FRAME;
		y -= CControl::MARGIN_BETWEEN_FRAME_AND_BUTTON + CControl::WIDTH_FRAME;

		DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, 0xFF);

		// Top-left
		DrawUtils::Draw2DQuadNoTex(x, y, x + CControl::LENGTH_FRAME, y + CControl::WIDTH_FRAME);
		DrawUtils::Draw2DQuadNoTex(x, y, x + CControl::WIDTH_FRAME, y + CControl::LENGTH_FRAME);

		// Top-right
		x = y = 0;	// Parameters of LocalToScreen() are both inputs and outputs. Rezero here means we wants the top-left point.
		pButton->LocalToScreen(x, y);
		Panel()->ScreenToLocal(x, y);	// Why doing this? It is because sometimes the panel you drawing this effect is not the button's parent.
		x += w + CControl::MARGIN_BETWEEN_FRAME_AND_BUTTON + CControl::WIDTH_FRAME;
		y -= CControl::MARGIN_BETWEEN_FRAME_AND_BUTTON + CControl::WIDTH_FRAME;
		DrawUtils::Draw2DQuadNoTex(x, y, x - CControl::LENGTH_FRAME, y + CControl::WIDTH_FRAME);
		DrawUtils::Draw2DQuadNoTex(x, y, x - CControl::WIDTH_FRAME, y + CControl::LENGTH_FRAME);

		DrawUtils::glRegularPureColorDrawingExit();
	}

	//-----------------------------------------------------------------------------
	// Purpose: Unlock mouse and display the panel.
	//-----------------------------------------------------------------------------
	virtual void Show(bool bShow)
	{
		if (Panel()->IsVisible() == bShow)	// The 'BaseClass' should be defined by the class on which this template applies.
			return;

		if (bShow)
		{
			Panel()->Activate();

			if (Panel()->GetParent()->IsVisible())
			{
				Panel()->SetMouseInputEnabled(true);
				IN_DeactivateMouse();
			}
		}
		else
		{
			Panel()->SetVisible(false);

			// Only do this when IClientVGUI is visible.
			// Or this line will break the pause screen.
			if (Panel()->GetParent()->IsVisible())
			{
				Panel()->SetMouseInputEnabled(false);
				IN_ActivateMouse();
			}
		}
	}

private:
	CControl* _pThis{ nullptr };
	inline CControl* Panel()
	{
		if (!_pThis)
			// As long as this class has no VFT, you can use static_cast. Otherwise, use dynamic_cast.
			_pThis = dynamic_cast<CControl*>(this);

		return _pThis;
	}
};

// UI Control
#include "UI/LM_ImageButton.h"

// My own elements.
#include "UI/TeamSelection.h"
#include "UI/RoleSelection.h"
#include "UI/Market.h"

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
	void OnTick(void) final;

public:
	CTeamMenu* m_pTeamMenu{ nullptr };
	CRoleMenu* m_pRoleMenu{ nullptr };
	CMarket* m_pMarketMenu{ nullptr };
};

}

extern vgui::CViewPort *g_pViewport;

#endif
