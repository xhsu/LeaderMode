//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
//=============================================================================

#ifndef IGAMECONSOLE_H
#define IGAMECONSOLE_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

struct Color;

//-----------------------------------------------------------------------------
// Purpose: interface to game/dev console
//-----------------------------------------------------------------------------
class IGameConsole : public IBaseInterface
{
public:
	IGameConsole();
	~IGameConsole();

public:
	// activates the console, makes it visible and brings it to the foreground
	virtual void Activate();

	virtual void Initialize();

	// hides the console
	virtual void Hide();

	// clears the console
	virtual void Clear();

	// return true if the console has focus
	virtual bool IsConsoleVisible();

	// prints a message to the console
	virtual void Printf(const char *format, ...);

	// printes a debug message to the console
	virtual void DPrintf(const char *format, ...);

	virtual void SetParent( int parent );

	// LUNA: Can I add some members?
public:
	bool m_bInitialized{ false };
	CGameConsoleDialog* m_pConsole{ nullptr };
};

#define GAMECONSOLE_INTERFACE_VERSION "GameConsole003"

// You should instanclize it yourself.
extern IGameConsole& GameConsole();

#endif // IGAMECONSOLE_H
