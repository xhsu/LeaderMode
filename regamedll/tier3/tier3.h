#ifndef TIER3_H
#define TIER3_H

#include <filesystem.h>
#include <../metahook/IKeyValuesSystem.h>
#include <../cl_dll/Interface/VGUI/VGUI.h>
#include <../cl_dll/Interface/VGUI/ISurface.h>
#include <../cl_dll/Interface/VGUI/IInput.h>
#include <../cl_dll/Interface/VGUI/IVGui.h>
#include <../cl_dll/Interface/VGUI/IPanel.h>
#include <../cl_dll/Interface/VGUI/ILocalize.h>
#include <../cl_dll/Interface/VGUI/IScheme.h>
#include <../cl_dll/Interface/VGUI/ISystem.h>

//-----------------------------------------------------------------------------
// These tier3 libraries must be set by any users of this library.
// They can be set by calling ConnectTier3Libraries.
// It is hoped that setting this, and using this library will be the common mechanism for
// allowing link libraries to access tier3 library interfaces
//-----------------------------------------------------------------------------
extern IFileSystem *g_pFullFileSystem;
extern IKeyValuesSystem *g_pKeyValuesSystem;
extern vgui::ISurface *g_pVGuiSurface;
extern vgui::IInput *g_pVGuiInput;
extern vgui::IVGui *g_pVGui;
extern vgui::IPanel *g_pVGuiPanel;
extern vgui::ILocalize *g_pVGuiLocalize;
extern vgui::ISchemeManager *g_pVGuiSchemeManager;
extern vgui::ISystem *g_pVGuiSystem;

//-----------------------------------------------------------------------------
// Call this to connect to/disconnect from all tier 3 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
//-----------------------------------------------------------------------------
void ConnectTier3Libraries(CreateInterfaceFn *pFactoryList, int nFactoryCount);
void DisconnectTier3Libraries(void);

#endif
