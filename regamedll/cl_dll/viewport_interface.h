/*

Created Date: Mar 12 2020

*/

#pragma once

extern vgui::ISurface* g_pVGuiSurface;
extern vgui::IPanel* g_pVGuiPanel;
extern vgui::IInput* g_pVGuiInput;
extern vgui::IVGui* g_pVGui;
extern vgui::ISystem* g_pVGuiSystem;
extern vgui::ISchemeManager* g_pVGuiSchemeManager;
extern vgui::ILocalize* g_pVGuiLocalize;
extern IFileSystem* g_pFileSystemInterface;

#define VGUI_INPUT		g_pVGuiInput
#define VGUI_SCHEME		g_pVGuiSchemeManager
#define VGUI_SURFACE	g_pVGuiSurface
#define VGUI_SYSTEM		g_pVGuiSystem
#define VGUI_IVGUI		g_pVGui
#define VGUI_IPANEL		g_pVGuiPanel
#define VGUI_LOCALISE	g_pVGuiLocalize
#define FILE_SYSTEM		g_pFileSystemInterface
