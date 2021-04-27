/*

Created Date: Apr 27 2021

Modern Warfare Dev Team
 - Luna the Reborn

Copied from metahook.
Credits to Nagist!

*/

#pragma once

void* MH_SearchPattern(void* pStartSearch, DWORD dwSearchLen, char* pPattern, DWORD dwPatternLen);
DWORD MH_GetModuleBase(HMODULE hModule);
DWORD MH_GetModuleSize(HMODULE hModule);
