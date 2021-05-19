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
#include "../public/strtools.h"
#include "../dlls/vector.h"

// Basic of Basics.
#include "../public/interface.h"

// File System
#include "../public/FileSystem.h"

// gEngfuncs
#include "../engine/APIProxy.h"

// Tier 3... Crsky, what's that?
#include "../tier3/tier3.h"

// Don't we just use a lib and a dll??
#include "../vgui_controls/Controls.h"

// general utils.
#include "../game_shared/shared_util.h"

// The Key/Signiture of GameUI.dll
#include "IGameConsole.h"
#include "IGameUI.h"
