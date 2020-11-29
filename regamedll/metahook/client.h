/*

Created Date: Nov 27 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#define CLIENT_EXTENDED_FUNCS_API_VERSION	1

typedef struct
{
	unsigned short	version;

	ENGFUNC_LoadTGA pfnLoadTGA;
	ENGFUNC_Key_NameForBinding pfnKey_NameForBinding;
}
cl_extendedfunc_t;

typedef bool (*FUNC_GetExtFuncs)	(cl_extendedfunc_t* pStructReturned);
