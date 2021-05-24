//===== Copyright ?1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#include "precompiled.h"

using namespace vgui;


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CGameConsoleDialog::CGameConsoleDialog() : BaseClass( NULL, "GameConsole", false )
{
	AddActionSignalTarget( this );
}


//-----------------------------------------------------------------------------
// Purpose: generic vgui command handler
//-----------------------------------------------------------------------------
void CGameConsoleDialog::OnCommand(const char *command)
{
	if ( !Q_stricmp( command, "Close" ) )
	{
	}

	BaseClass::OnCommand(command);
}


//-----------------------------------------------------------------------------
// HACK: Allow F key bindings to operate even when typing in the text entry field
//-----------------------------------------------------------------------------
void CGameConsoleDialog::OnKeyCodeTyped(KeyCode code)
{
	BaseClass::OnKeyCodeTyped(code);

	// check for processing
	if ( m_pConsolePanel->TextEntryHasFocus() )
	{
		// HACK: Allow F key bindings to operate even here
		if ( code >= KEY_F1 && code <= KEY_F12 )
		{
		}
	}
}

//-----------------------------------------------------------------------------
// Submits a command
//-----------------------------------------------------------------------------
void CGameConsoleDialog::OnCommandSubmitted( const char *pCommand )
{
	if (!Q_strncmp(pCommand, "options", 7))
	{
		BasePanel()->OnOpenOptionsDialog();
		return;
	}
	else if (!Q_strncmp(pCommand, "server", 6))
	{
		BasePanel()->OnOpenCreateMultiplayerGameDialog();
		return;
	}

	gEngfuncs.pfnClientCmd( (char *)pCommand );
}


//-----------------------------------------------------------------------------
// Submits a command
//-----------------------------------------------------------------------------
void CGameConsoleDialog::OnClosedByHittingTilde()
{
}
