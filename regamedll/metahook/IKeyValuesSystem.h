#ifndef VSTDLIB_IKEYVALUESSYSTEM_H
#define VSTDLIB_IKEYVALUESSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

// handle to a KeyValues key name symbol
typedef int HKeySymbol;
#define INVALID_KEY_SYMBOL (-1)

typedef int HLocalized;
#define INVALID_LOCALIZED (-1)

//-----------------------------------------------------------------------------
// Purpose: Interface to shared data repository for KeyValues (included in vgui_controls.lib)
//			allows for central data storage point of KeyValues symbol table
//-----------------------------------------------------------------------------
class IKeyValuesSystem : public IBaseInterface
{
public:
	// registers the size of the KeyValues in the specified instance
	// so it can build a properly sized memory pool for the KeyValues objects
	// the sizes will usually never differ but this is for versioning safety
	virtual void RegisterSizeofKeyValues(int size) = 0;

	// allocates/frees a KeyValues object from the shared mempool
	virtual void* AllocKeyValuesMemory(int size) = 0;
	virtual void FreeKeyValuesMemory(void* pMem) = 0;

	// symbol table access (used for key names)
	virtual HKeySymbol GetSymbolForString(const char* name) = 0;
	virtual const char* GetStringForSymbol(HKeySymbol symbol) = 0;

	// LUNA: New funcs???
	virtual HLocalized GetLocalizedFromANSI(const char *string) = 0;
	virtual const char *GetANSIFromLocalized(HLocalized l) = 0;
	virtual void AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name) = 0;
	virtual void RemoveKeyValuesFromMemoryLeakList(void *pMem) = 0;
};

// Added by LUNA.
extern void KeyValuesSystem_InstallHook();
extern IKeyValuesSystem* g_pKeyValuesSystem;

#define KEYVALUESSYSTEM_INTERFACE_VERSION "KeyValues003"
#endif
