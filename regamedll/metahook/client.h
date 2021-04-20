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

typedef bool (*FUNC_GetExtFuncs)	(cl_extendedfunc_t* pStructReturned);
