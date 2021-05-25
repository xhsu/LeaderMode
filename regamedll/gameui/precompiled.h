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

// default game dir.
#define LM_DEF_GAMEDIR	"leadermode"

#pragma once

#include "../public/basetypes.h"
#include "../engine/maintypes.h"
#include "../engine/archtypes.h"
#include "../tier0/dbg.h"	// Keep it at last forever. LUNA: But why?
#include "../tier1/strtools.h"
#include "../dlls/vector.h"

#include <string.h>
#include <locale.h>
#include <string>
#include <array>

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

// VGUI 2
#include "vgui_controls/pch_vgui_controls.h"
#include "PanelListPanel.h"	// Another type of PanelListPanel, different from VGUI2 one. Don't mixed them up!

// Interfaces
#include "Interface/IGameUIFuncs.h"
#include "Interface/ICommandLine.h"
#include "Interface/IEngineVGui.h"
#include "Interface/IBaseUI.h"
#include "Interface/IVGuiDLL.h"

// general utils.
#include "../game_shared/shared_util.h"
#include "../tier1/utlmemory.h"
#include "../tier1/utlbuffer.h"
#include "../tier1/characterset.h"

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

// Save manu status
#include "ScriptObject.h"

// Create Multiplayer Game
#include "CreateMultiplayerGameDialog/CreateMultiplayerGameBotPage.h"
#include "CreateMultiplayerGameDialog/CreateMultiplayerGameDialog.h"
#include "CreateMultiplayerGameDialog/CreateMultiplayerGameGameplayPage.h"
#include "CreateMultiplayerGameDialog/CreateMultiplayerGameServerPage.h"

// Options
#include "Options/OptionsDialog.h"
#include "Options/OptionsSubAudio.h"
#include "Options/OptionsSubKeyboard.h"
#include "Options/OptionsSubMouse.h"
#include "Options/OptionsSubVideo.h"

// Multiplayer net
#include "common/netapi.h"
#include "common/netadr.h"

// Server Browser
#include "OfflineMode.h"	// Are we online?
#include "ServerBrowser/BaseGamesPage.h"
#include "ServerBrowser/DialogAddServer.h"
#include "ServerBrowser/DialogGameInfo.h"
#include "ServerBrowser/DialogServerPassword.h"
#include "ServerBrowser/FavoriteGames.h"
#include "ServerBrowser/FriendsGames.h"
#include "ServerBrowser/HistoryGames.h"
#include "ServerBrowser/InternetGames.h"
#include "ServerBrowser/LanGames.h"
#include "ServerBrowser/ServerBrowser.h"
#include "ServerBrowser/ServerBrowserDialog.h"
#include "ServerBrowser/ServerContextMenu.h"
#include "ServerBrowser/ServerListCompare.h"
#include "ServerBrowser/SpectateGames.h"
#include "ServerBrowser/VACBannedConnRefusedDialog.h"


// General global vars declaration.
extern cl_enginefunc_t gEngfuncs;
extern IKeyValuesSystem* (*KeyValuesSystem)(void);
extern IGameUIFuncs* gameuifuncs;
extern ICommandLine* (*CommandLine)(void);
extern unsigned int (*LoadDDS)(const char* szFile, int* iWidth, int* iHeight);
