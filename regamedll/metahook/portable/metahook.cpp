/*

Created Date: Apr 27 2021

Modern Warfare Dev Team
 - Luna the Reborn

Copied from metahook.
Credits to Nagist!

*/

#include <winsani_in.h>
#include <windows.h>
#include <winsani_out.h>
#include <stdio.h>

void* MH_SearchPattern(void* pStartSearch, DWORD dwSearchLen, const char* pPattern, DWORD dwPatternLen)
{
	DWORD dwStartAddr = (DWORD)pStartSearch;
	DWORD dwEndAddr = dwStartAddr + dwSearchLen - dwPatternLen;

	while (dwStartAddr < dwEndAddr)
	{
		bool found = true;

		for (unsigned i = 0; i < dwPatternLen; i++)
		{
			char code = *(char*)(dwStartAddr + i);

			if (pPattern[i] != 0x2A && pPattern[i] != code)
			{
				found = false;
				break;
			}
		}

		if (found)
			return (void*)dwStartAddr;

		dwStartAddr++;
	}

	return 0;
}

DWORD MH_GetModuleBase(HMODULE hModule)
{
	MEMORY_BASIC_INFORMATION mem;

	if (!VirtualQuery(hModule, &mem, sizeof(MEMORY_BASIC_INFORMATION)))
		return 0;

	return (DWORD)mem.AllocationBase;
}

DWORD MH_GetModuleSize(HMODULE hModule)
{
	return ((IMAGE_NT_HEADERS*)((DWORD)hModule + ((IMAGE_DOS_HEADER*)hModule)->e_lfanew))->OptionalHeader.SizeOfImage;
}
