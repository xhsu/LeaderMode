/*

Created Date: 26 Dec 2020

*/

#include "precompiled.h"

vgui::ISurfaceV26* VGUI_SURFACE = nullptr;	// Have to use V26 on MH, or it will never be created successfully.
cl_enginefuncs_s gEngfuncs;


int Initialize(struct cl_enginefuncs_s* pEnginefuncs, int iVersion)
{
	// Copy for ourselves first.
	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefuncs_s));

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
