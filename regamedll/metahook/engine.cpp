/*

Created Date: Nov 27 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

namespace engine
{
	ENGFUNC_LoadTGA LoadTGA = nullptr;
	ENGFUNC_Key_NameForBinding Key_NameForBinding = nullptr;
	ENGFUNC_Cache_Check Cache_Check = nullptr;
};

void SearchEngineFunctions(void)
{
	*(void**)&engine::LoadTGA = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, LOAD_TGA_SIG, sizeof(LOAD_TGA_SIG) - 1U);
	if (!engine::LoadTGA)
		*(void**)&engine::LoadTGA = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, LOAD_TGA_SIG_NEW, sizeof(LOAD_TGA_SIG_NEW) - 1U);
	if (!engine::LoadTGA)
		Sys_Error("Function \"LoadTGA\" no found!\nEngine buildnum %d unsupported!", g_dwEngineBuildnum);

	*(void**)&engine::Key_NameForBinding = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, KEY_NAME_FOR_BINDING_SIG, sizeof(KEY_NAME_FOR_BINDING_SIG) - 1U);
	if (!engine::Key_NameForBinding)
		Sys_Error("Function \"Key_NameForBinding\" no found!\nEngine buildnum %d unsupported!", g_dwEngineBuildnum);

	*(void**)&engine::Cache_Check = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, Cache_Check_SIG, sizeof(Cache_Check_SIG) - 1U);
	if (!engine::Cache_Check)
		Sys_Error("Function \"Cache_Check\" no found!\nEngine buildnum %d unsupported!", g_dwEngineBuildnum);
}
