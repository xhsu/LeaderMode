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
#include "../public/FileSystem.h"

// gEngfuncs
#include "../engine/APIProxy.h"

// Tier 3... Crsky, what's that?
#include "../tier3/tier3.h"

// Don't we just use a lib and a dll??
#include "cl_dll/Interface/IGameUIFuncs.h"
#include "cl_dll/Interface/IEngineVGui.h"
#include "cl_dll/Interface/IVGuiDLL.h"
#include "cl_dll/Interface/VGUI/IInput.h"
#include "cl_dll/Interface/VGUI/IScheme.h"
#include "cl_dll/Interface/VGUI/IVGui.h"
#include "cl_dll/Interface/VGUI/ISurface.h"
#include "cl_dll/Interface/VGUI/ILocalize.h"
#include "public/KeyValues.h"

#include "vgui_controls/Button.h"
#include "cl_dll/Interface/VGUI/KeyCode.h"
#include "vgui_controls/Menu.h"
#include "vgui_controls/TextEntry.h"
#include "vgui_controls/RichText.h"
#include "FileSystem.h"
#include "vgui_controls/MessageMap.h"
#include "vgui_controls/Controls.h"

// general utils.
#include "../game_shared/shared_util.h"
#include "../public/utlmemory.h"
#include "../public/utlbuffer.h"
#include "../public/characterset.h"

// The Key/Signiture of GameUI.dll
#include "IGameConsole.h"
#include "IGameUI.h"

#include "BasePanel.h"
#include "GameConsoleDialog.h"



// General global vars declaration.
extern cl_enginefunc_t gEngfuncs;
extern IKeyValuesSystem* (*KeyValuesSystem)(void);
