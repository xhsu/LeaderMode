/*

Created Date: 26 Dec 2020

*/

#include "precompiled.h"

vgui::ISurfaceV26* VGUI_SURFACE = nullptr;


int Initialize(struct cl_enginefuncs_s* pEnginefuncs, int iVersion)
{
	bool result = !!gExportfuncs.Initialize(pEnginefuncs, iVersion);

	// only doing this if the client loaded successfully.
	if (result)
	{
		// load vgui surface for create new texture id.
		CreateInterfaceFn EngineCreateInterface = g_pMetaHookAPI->GetEngineFactory();
		VGUI_SURFACE = (vgui::ISurfaceV26*)EngineCreateInterface(VGUI_SURFACEV26_INTERFACE_VERSION, nullptr);
	}

	return result;
}
