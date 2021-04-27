/*

Created Date: Nov 27 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

namespace cl
{
	bool (*MH_LoadClient)(unsigned short iVersion, const cl_extendedfunc_t* pfn) = nullptr;
	void (*S_StartSound)(int iEntity, int iChannel, sfx_t* pSFXin, Vector& vecOrigin, float flVolume, float flAttenuation, int bitsFlags, int iPitch) = nullptr;
	void (*S_StopAllSounds)(bool STFU) = nullptr;
	void (*CL_Disconnect)(void) = nullptr;
};

void GetClientCallbacks(void)
{
	HMODULE hClientDLL = GetModuleHandle("client.dll");

	if (!hClientDLL)
	{
		Sys_Error("client.dll no found!");
		return;
	}

	*(void**)&cl::MH_LoadClient = GetProcAddress(hClientDLL, "MH_LoadClient");
	*(void**)&cl::S_StartSound = GetProcAddress(hClientDLL, "S_StartSound");
	*(void**)&cl::S_StopAllSounds = GetProcAddress(hClientDLL, "S_StopAllSounds");
	*(void**)&cl::CL_Disconnect = GetProcAddress(hClientDLL, "CL_Disconnect");

	if (!cl::MH_LoadClient)
	{
		Sys_Error("client.dll export function \"MH_LoadClient\" no found!");
		return;
	}
	else if (!cl::S_StartSound)
	{
		Sys_Error("client.dll export function \"S_StartSound\" no found!");
		return;
	}
	else if (!cl::S_StopAllSounds)
	{
		Sys_Error("client.dll export function \"S_StopAllSounds\" no found!");
		return;
	}
	else if (!cl::CL_Disconnect)
	{
		Sys_Error("client.dll export function \"CL_Disconnect\" no found!");
		return;
	}
}

const char* Safe_Key_NameForBinding(const char* pszCommand)
{
	if (!engine::Key_NameForBinding)
		return "Engine Function no found!";

	if (!pszCommand)
		return "Null Command";

	static const char* psz = nullptr;
	psz = engine::Key_NameForBinding(pszCommand);	// you can't put them in the same line.

	if (!psz)
		return "UNBIND";

	return psz;
}
