/*

Created Date: Nov 27 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

ENGFUNC_LoadTGA g_pfnLoadTGA = nullptr;
ENGFUNC_Key_NameForBinding g_pfnKey_NameForBinding = nullptr;

void SearchEngineFunctions(void)
{
	g_pfnLoadTGA = (ENGFUNC_LoadTGA)g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, LOAD_TGA_SIG, sizeof(LOAD_TGA_SIG) - 1U);
	if (!g_pfnLoadTGA)
		g_pfnLoadTGA = (ENGFUNC_LoadTGA)g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, LOAD_TGA_SIG_NEW, sizeof(LOAD_TGA_SIG_NEW) - 1U);

	g_pfnKey_NameForBinding = (ENGFUNC_Key_NameForBinding)g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, KEY_NAME_FOR_BINDING_SIG, sizeof(KEY_NAME_FOR_BINDING_SIG) - 1U);
}
