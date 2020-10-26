/*

Created Date: Oct 26 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#pragma once

enum SharedString
{
	SSZ_M1014_INSERT_SFX,
	SSZ_M1014_SIDELOAD_SFX,
	SSZ_C4_VMDL,

	SSZ_COUNT
};

//extern const char* g_rgpszSharedString[SSZ_COUNT];
extern std::array<const char*, SSZ_COUNT> g_rgpszSharedString;
