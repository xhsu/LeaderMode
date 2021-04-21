/*

Created Date: Nov 27 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#define CLIENT_EXTENDED_FUNCS_API_VERSION	2

typedef struct
{
	unsigned short	version;

	ENGFUNC_LoadTGA pfnLoadTGA;
	ENGFUNC_Key_NameForBinding pfnKey_NameForBinding;
	ENGFUNC_S_LoadSound pfnS_LoadSound;
	ENGFUNC_S_StartSound pfnS_StartStaticSound;
	ENGFUNC_S_StartSound pfnS_StartDynamicSound;
	ENGFUNC_S_StopAllSounds pfnS_StopAllSounds;
	ENGFUNC_Cache_Check pfnCache_Check;
}
cl_extendedfunc_t;

#ifndef CLIENT_DLL

// Obvious these are not for client.dll
void GetClientCallbacks(void);
const char* Safe_Key_NameForBinding(const char* pszCommand);

namespace cl
{
	extern bool (*MH_LoadClient)(unsigned short iVersion, const cl_extendedfunc_t* pfn);
	extern void (*S_StartSound)(int iEntity, int iChannel, sfx_t* pSFXin, Vector& vecOrigin, float flVolume, float flAttenuation, int bitsFlags, int iPitch);
	extern void (*S_StopAllSounds)(bool STFU);
};
#endif
