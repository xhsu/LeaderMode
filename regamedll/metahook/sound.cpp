/*

Created Date: Apr 21 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

namespace engine
{
	ENGFUNC_S_StartSound S_StartStaticSound = nullptr;
	ENGFUNC_S_StartSound S_StartDynamicSound = nullptr;
	ENGFUNC_S_LoadSound S_LoadSound = nullptr;
	ENGFUNC_S_StopAllSounds S_StopAllSounds = nullptr;
};

void S_StartStaticSound(int iEntity, int iChannel, sfx_t* pSFXin, Vector& vecOrigin, float flVolume, float flAttenuation, int bitsFlags, int iPitch)
{
	cl::S_StartSound(iEntity, iChannel, pSFXin, vecOrigin, flVolume, flAttenuation, bitsFlags, iPitch);
}

void S_StartDynamicSound(int iEntity, int iChannel, sfx_t* pSFXin, Vector& vecOrigin, float flVolume, float flAttenuation, int bitsFlags, int iPitch)
{
	cl::S_StartSound(iEntity, iChannel, pSFXin, vecOrigin, flVolume, flAttenuation, bitsFlags, iPitch);
}

void S_StopAllSounds(bool STFU)
{
	cl::S_StopAllSounds(STFU);
}

void Sound_InstallHook()
{
	*(void**)&engine::S_StartDynamicSound = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, S_StartDynamicSound_SIG, sizeof(S_StartDynamicSound_SIG) - 1U);
	if (engine::S_StartDynamicSound)
		g_pMetaHookAPI->InlineHook(engine::S_StartDynamicSound, S_StartDynamicSound, (void*&)engine::S_StartDynamicSound);
	else
		Sys_Error("Function \"S_StartDynamicSound\" no found!\nEngine buildnum %d unsupported!", g_dwEngineBuildnum);

	*(void**)&engine::S_StartStaticSound = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, S_StartStaticSound_SIG, sizeof(S_StartStaticSound_SIG) - 1U);
	if (engine::S_StartStaticSound)
		g_pMetaHookAPI->InlineHook(engine::S_StartStaticSound, S_StartStaticSound, (void*&)engine::S_StartStaticSound);
	else
		Sys_Error("Function \"S_StartStaticSound\" no found!\nEngine buildnum %d unsupported!", g_dwEngineBuildnum);

	*(void**)&engine::S_LoadSound = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, S_LoadSound_SIG, sizeof(S_LoadSound_SIG) - 1U);
	if (!engine::S_LoadSound)
		Sys_Error("Function \"S_LoadSound\" no found!\nEngine buildnum %d unsupported!", g_dwEngineBuildnum);

	*(void**)&engine::S_StopAllSounds = g_pMetaHookAPI->SearchPattern((void*)g_dwEngineBase, g_dwEngineSize, S_StopAllSounds_SIG, sizeof(S_StopAllSounds_SIG) - 1U);
	if (engine::S_StopAllSounds)
		g_pMetaHookAPI->InlineHook(engine::S_StopAllSounds, S_StopAllSounds, (void*&)engine::S_StopAllSounds);
	else
		Sys_Error("Function \"S_StopAllSounds\" no found!\nEngine buildnum %d unsupported!", g_dwEngineBuildnum);
}
