#include "BasePanel.h"
#include "EngineInterface.h"
#include <itextureapi.h>

CBasePanel::CBasePanel() : vgui::Panel(NULL, "BaseGameUIPanel")
{
}

void CBasePanel::PaintBackground(void)
{
	int wide, tall;
	GetSize(wide, tall);

	const char *level = gEngfuncs.pfnGetLevelName();

	if (*level)
	{
		vgui::surface()->DrawSetColor(0, 0, 0, 128);
		vgui::surface()->DrawFilledRect(0, 0, wide, tall);
	}
	else
	{
		vgui::surface()->DrawSetColor(0, 0, 0, 255);
		vgui::surface()->DrawFilledRect(0, 0, wide, tall);
	}
}

void CBasePanel::RunFrame(void)
{
	const char *level = gEngfuncs.pfnGetLevelName();

	if (IsVisible() && !*level)
	{
		//surface()->PaintTraverse(GetVPanel());
		PaintBackground();
	}
}