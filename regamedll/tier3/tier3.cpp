#include <precompiled.h>

//-----------------------------------------------------------------------------
// These tier3 libraries must be set by any users of this library.
// They can be set by calling ConnectTier3Libraries.
// It is hoped that setting this, and using this library will be the common mechanism for
// allowing link libraries to access tier3 library interfaces
//-----------------------------------------------------------------------------
IFileSystem *g_pFullFileSystem = NULL;
IKeyValuesSystem *g_pKeyValuesSystem = NULL;
vgui::ISurface *g_pVGuiSurface = NULL;
vgui::IInput *g_pVGuiInput = NULL;
vgui::IVGui *g_pVGui = NULL;
vgui::IPanel *g_pVGuiPanel = NULL;
vgui::ILocalize *g_pVGuiLocalize = NULL;
vgui::ISchemeManager *g_pVGuiSchemeManager = NULL;
vgui::ISystem *g_pVGuiSystem = NULL;


//-----------------------------------------------------------------------------
// Call this to connect to all tier 3 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
//-----------------------------------------------------------------------------
void ConnectTier3Libraries(CreateInterfaceFn *pFactoryList, int nFactoryCount)
{
	for (int i = 0; i < nFactoryCount; i++)
	{
		if (!g_pFullFileSystem)
		{
			g_pFullFileSystem = (IFileSystem *)pFactoryList[i](FILESYSTEM_INTERFACE_VERSION, NULL);
		}
		if (!g_pKeyValuesSystem)
		{
			g_pKeyValuesSystem = (IKeyValuesSystem *)pFactoryList[i](KEYVALUESSYSTEM_INTERFACE_VERSION, NULL);
		}
		if (!g_pVGuiSurface)
		{
			g_pVGuiSurface = (vgui::ISurface *)pFactoryList[i](VGUI_SURFACE_INTERFACE_VERSION, NULL);
		}
		if (!g_pVGuiInput)
		{
			g_pVGuiInput = (vgui::IInput *)pFactoryList[i](VGUI_INPUT_INTERFACE_VERSION, NULL);
		}
		if (!g_pVGui)
		{
			g_pVGui = (vgui::IVGui *)pFactoryList[i](VGUI_IVGUI_INTERFACE_VERSION, NULL);
		}
		if (!g_pVGuiPanel)
		{
			g_pVGuiPanel = (vgui::IPanel *)pFactoryList[i](VGUI_PANEL_INTERFACE_VERSION, NULL);
		}
		if (!g_pVGuiLocalize)
		{
			g_pVGuiLocalize = (vgui::ILocalize *)pFactoryList[i](VGUI_LOCALIZE_INTERFACE_VERSION, NULL);
		}
		if (!g_pVGuiSchemeManager)
		{
			g_pVGuiSchemeManager = (vgui::ISchemeManager *)pFactoryList[i](VGUI_SCHEME_INTERFACE_VERSION, NULL);
		}
		if (!g_pVGuiSystem)
		{
			g_pVGuiSystem = (vgui::ISystem *)pFactoryList[i](VGUI_SYSTEM_INTERFACE_VERSION, NULL);
		}
	}
}

void DisconnectTier3Libraries(void)
{
	g_pVGuiSurface = NULL;
	g_pVGuiInput = NULL;
	g_pVGui = NULL;
	g_pVGuiPanel = NULL;
	g_pVGuiLocalize = NULL;
	g_pVGuiSchemeManager = NULL;
	g_pVGuiSystem = NULL;
}
