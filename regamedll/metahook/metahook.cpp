/*

Created Date: Mar 22 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

cl_exportfuncs_t gExportfuncs;
mh_interface_t* g_pInterface;
metahook_api_t* g_pMetaHookAPI;
mh_enginesave_t* g_pMetaSave;

DWORD g_dwEngineBase = 0;
DWORD g_dwEngineSize = 0;
DWORD g_dwEngineBuildnum = 0;

void IPlugins::Init(metahook_api_t* pAPI, mh_interface_t* pInterface, mh_enginesave_t* pSave)
{
	g_pInterface = pInterface;
	g_pMetaHookAPI = pAPI;
	g_pMetaSave = pSave;
}

void IPlugins::Shutdown(void)
{
}

void IPlugins::LoadEngine(void)
{
	g_dwEngineBuildnum = g_pMetaHookAPI->GetEngineBuildnum();
	g_dwEngineBase = g_pMetaHookAPI->GetEngineBase();
	g_dwEngineSize = g_pMetaHookAPI->GetEngineSize();

	// load hooks.
	LargerTexture_InstallHook();
	FullBLight_InstallHook();
	Sound_InstallHook();
	KeyValuesSystem_InstallHook();

	// Search engine function for client.dll
	SearchEngineFunctions();
}

void IPlugins::LoadClient(cl_exportfuncs_t* pExportFunc)
{
	// in client.dll, this function should be called in gExportfuncs.Initialize().
	Dxt_Initialization();

	// client.dll initialize.
	Q_memcpy(&gExportfuncs, pExportFunc, sizeof(gExportfuncs));

	// Hooks
	pExportFunc->Initialize = &Initialize;

	// Load custom client export functions.
	GetClientCallbacks();

	cl_extendedfunc_t table =
	{
		CLIENT_EXTENDED_FUNCS_API_VERSION,

		engine::LoadTGA,
		&Safe_Key_NameForBinding,
		engine::S_LoadSound,
		engine::S_StartStaticSound,
		engine::S_StartDynamicSound,
		engine::S_StopAllSounds,
		engine::Cache_Check,
		engine::CL_Disconnect,
	};

	cl::MH_LoadClient(CLIENT_EXTENDED_FUNCS_API_VERSION, &table);
}

void IPlugins::ExitGame(int iResult)
{
}

EXPOSE_SINGLE_INTERFACE(IPlugins, IPlugins, METAHOOK_PLUGIN_API_VERSION);
