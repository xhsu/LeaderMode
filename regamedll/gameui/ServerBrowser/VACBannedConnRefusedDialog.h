/*

Copied Date: May 22 2021 [CS:BTE Alpha]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#ifndef VACBANNEDCONNREFUSED_H
#define VACBANNEDCONNREFUSED_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>

class CVACBannedConnRefusedDialog : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CVACBannedConnRefusedDialog, vgui::Frame);

public:
	CVACBannedConnRefusedDialog(vgui::VPANEL hVParent, const char *name);
};

#endif
