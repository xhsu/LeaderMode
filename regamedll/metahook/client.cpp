/*

Created Date: Nov 27 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

const char* Safe_Key_NameForBinding(const char* pszCommand)
{
	if (!g_pfnKey_NameForBinding)
		return "Engine Function Error";

	if (!pszCommand)
		return "Null Command";

	static const char* psz = nullptr;
	psz = g_pfnKey_NameForBinding(pszCommand);	// you can't put them in the same line.

	if (!psz)
		return "UNBIND";

	return psz;
}

bool CL_GetExtendedFuncs(cl_extendedfunc_t* pStructReturned)
{
	if (!pStructReturned)
		return false;

	*pStructReturned =
	{
		CLIENT_EXTENDED_FUNCS_API_VERSION,

		g_pfnLoadTGA,
		&Safe_Key_NameForBinding,
	};

	return true;
}
