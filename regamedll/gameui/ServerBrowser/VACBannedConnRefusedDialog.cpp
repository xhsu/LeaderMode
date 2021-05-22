/*

Copied Date: May 22 2021 [CS:BTE Alpha]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#include "precompiled.h"

using namespace vgui;

CVACBannedConnRefusedDialog::CVACBannedConnRefusedDialog(VPANEL hVParent, const char *name) : BaseClass(NULL, name)
{
	SetParent(hVParent);
	SetSize(480, 220);
	SetSizeable(false);

	LoadControlSettings("servers/VACBannedConnRefusedDialog.res");
	MoveToCenterOfScreen();
}
