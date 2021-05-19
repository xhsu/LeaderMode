/*

Created Date: May 19 2021

Modern Warfare Dev Team
	Advisor		- Crsky

*/

#include "precompiled.h"

IKeyValuesSystem *g_pKeyValuesSystem = nullptr;

void (__fastcall *g_pfnCKeyValuesSystem__RegisterSizeofKeyValues)(void *pthis, int edx, int size) = 0;
void *(__fastcall *g_pfnCKeyValuesSystem__AllocKeyValuesMemory)(void *pthis, int edx, int size) = 0;
void (__fastcall *g_pfnCKeyValuesSystem__FreeKeyValuesMemory)(void *pthis, int edx, void *pMem) = 0;
HKeySymbol (__fastcall *g_pfnCKeyValuesSystem__GetSymbolForString)(void *pthis, int edx, const char *name) = 0;
const char *(__fastcall *g_pfnCKeyValuesSystem__GetStringForSymbol)(void *pthis, int edx, HKeySymbol symbol) = 0;
HLocalized (__fastcall *g_pfnCKeyValuesSystem__GetLocalizedFromANSI)(void *pthis, int edx, const char *string) = 0;
const char *(__fastcall *g_pfnCKeyValuesSystem__GetANSIFromLocalized)(void *pthis, int edx, HLocalized l) = 0;
void (__fastcall *g_pfnCKeyValuesSystem__AddKeyValuesToMemoryLeakList)(void *pthis, int edx, void *pMem, HKeySymbol name) = 0;
void (_fastcall *g_pfnCKeyValuesSystem__RemoveKeyValuesFromMemoryLeakList)(void *pthis, int edx, void *pMem) = 0;

class CKeyValuesSystem : public IKeyValuesSystem
{
public:
	virtual void RegisterSizeofKeyValues(int size);
	virtual void *AllocKeyValuesMemory(int size);
	virtual void FreeKeyValuesMemory(void *pMem);
	virtual HKeySymbol GetSymbolForString(const char *name);
	virtual const char *GetStringForSymbol(HKeySymbol symbol);
	virtual HLocalized GetLocalizedFromANSI(const char *string);
	virtual const char *GetANSIFromLocalized(HLocalized l);
	virtual void AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name);
	virtual void RemoveKeyValuesFromMemoryLeakList(void *pMem);
};

void CKeyValuesSystem::RegisterSizeofKeyValues(int size)
{
	return g_pfnCKeyValuesSystem__RegisterSizeofKeyValues(this, 0, size);
}

void *CKeyValuesSystem::AllocKeyValuesMemory(int size)
{
	return calloc(1, size);
}
void CKeyValuesSystem::FreeKeyValuesMemory(void *pMem)
{
	return free(pMem);
}

HKeySymbol CKeyValuesSystem::GetSymbolForString(const char *name)
{
	return g_pfnCKeyValuesSystem__GetSymbolForString(this, 0, name);
}

const char *CKeyValuesSystem::GetStringForSymbol(HKeySymbol symbol)
{
	return g_pfnCKeyValuesSystem__GetStringForSymbol(this, 0, symbol);
}

HLocalized CKeyValuesSystem::GetLocalizedFromANSI(const char *string)
{
	return g_pfnCKeyValuesSystem__GetLocalizedFromANSI(this, 0, string);
}

const char *CKeyValuesSystem::GetANSIFromLocalized(HLocalized l)
{
	return g_pfnCKeyValuesSystem__GetANSIFromLocalized(this, 0, l);
}

void CKeyValuesSystem::AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name)
{
	return g_pfnCKeyValuesSystem__AddKeyValuesToMemoryLeakList(this, 0, pMem, name);
}

void CKeyValuesSystem::RemoveKeyValuesFromMemoryLeakList(void *pMem)
{
	return g_pfnCKeyValuesSystem__RemoveKeyValuesFromMemoryLeakList(this, 0, pMem);
}

void KeyValuesSystem_InstallHook()
{
	CSysModule* m_hVGuiModule = Sys_LoadModule("vgui2.dll");
	CreateInterfaceFn vguiFactory = Sys_GetFactory(m_hVGuiModule);

	g_pKeyValuesSystem = (IKeyValuesSystem*)vguiFactory(KEYVALUESSYSTEM_INTERFACE_VERSION, NULL);

	CKeyValuesSystem KeyValuesSystem;
	DWORD *pVFTable = *(DWORD **)&KeyValuesSystem;

	//g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 1, (void *)pVFTable[1], (void *&)g_pfnCKeyValuesSystem__RegisterSizeofKeyValues);
	g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 2, (void *)pVFTable[2], (void *&)g_pfnCKeyValuesSystem__AllocKeyValuesMemory);
	g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 3, (void *)pVFTable[3], (void *&)g_pfnCKeyValuesSystem__FreeKeyValuesMemory);
	//g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 4, (void *)pVFTable[4], (void *&)g_pfnCKeyValuesSystem__GetSymbolForString);
	//g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 5, (void *)pVFTable[5], (void *&)g_pfnCKeyValuesSystem__GetStringForSymbol);
	//g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 6, (void *)pVFTable[6], (void *&)g_pfnCKeyValuesSystem__GetLocalizedFromANSI);
	//g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 7, (void *)pVFTable[7], (void *&)g_pfnCKeyValuesSystem__GetANSIFromLocalized);
	//g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 8, (void *)pVFTable[8], (void *&)g_pfnCKeyValuesSystem__AddKeyValuesToMemoryLeakList);
	//g_pMetaHookAPI->VFTHook(g_pKeyValuesSystem, 0, 9, (void *)pVFTable[9], (void *&)g_pfnCKeyValuesSystem__RemoveKeyValuesFromMemoryLeakList);
}

IKeyValuesSystem* /* DLL_EXPORT for GameUI.dll */ GetKeyValueSystem(void)
{
	return g_pKeyValuesSystem;
}
