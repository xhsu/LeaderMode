/*

Created Date: May 19 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL
	Advisor		- Crsky

*/

#ifdef UNICODE
#undef UNICODE
#endif // UNICODE

#pragma once

#include "../public/basetypes.h"
#include "../engine/maintypes.h"
#include "../engine/archtypes.h"
#include "../public/tier0/dbg.h"	// Keep it at last forever. LUNA: But why?
#include "../public/strtools.h"
#include "../dlls/vector.h"

#include <string.h>
#include <locale.h>

// Basic of Basics.
#include "../public/interface.h"

// File System
#include <Interface/IFileSystem.h>

// gEngfuncs
#include "../engine/APIProxy.h"

// Tier 3... Crsky, what's that?
#include "../tier3/tier3.h"

// Cvar
#include "cvardef.h"

// Keys
#include "keydefs.h"

// Fuck these stupit windows pre-define.
#ifdef PostMessage
#undef PostMessage
#endif

// Don't we just use a lib and a dll??
#include "Interface/IGameUIFuncs.h"
#include "Interface/IEngineVGui.h"
#include "Interface/IVGuiDLL.h"
#include "Interface/IServerBrowser.h"
#include "Interface/ICommandLine.h"
#include "VGUI/IInput.h"
#include "VGUI/IScheme.h"
#include "VGUI/IVGui.h"
#include "VGUI/ISurface.h"
#include "VGUI/ILocalize.h"
#include "public/KeyValues.h"

#include "vgui_controls/Button.h"
#include "VGUI/KeyCode.h"
#include "vgui_controls/Menu.h"
#include "vgui_controls/TextEntry.h"
#include "vgui_controls/RichText.h"
#include "vgui_controls/MessageMap.h"
#include "vgui_controls/Controls.h"
#include "vgui_controls/MenuItem.h"
#include "vgui_controls/AnimationController.h"
#include "vgui_controls/QueryBox.h"
#include <vgui_controls/Frame.h>
#include <vgui_controls/HTML.h>
#include "vgui_controls/BitmapImagePanel.h"
#include "vgui_controls/ProgressBar.h"

// general utils.
#include "../game_shared/shared_util.h"
#include "../public/utlmemory.h"
#include "../public/utlbuffer.h"
#include "../public/characterset.h"

// Basical elements: a panel on the back, a console to show something.
#include "BasePanel.h"
#include "GameConsoleDialog.h"

// The Key/Signiture of GameUI.dll
#include "IGameConsole.h"
#include "IGameUI.h"

// Main menu
#include "ModInfo.h"
#include "BackgroundMenuButton.h"
#include "GameMenu.h"
#include "LoadingDialog.h"


// General global vars declaration.
extern cl_enginefunc_t gEngfuncs;
extern IKeyValuesSystem* (*KeyValuesSystem)(void);
extern IGameUIFuncs* gameuifuncs;
extern ICommandLine* (*CommandLine)(void);
