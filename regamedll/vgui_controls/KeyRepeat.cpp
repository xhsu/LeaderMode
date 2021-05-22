//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "../public/basetypes.h"
#include "../engine/maintypes.h"
#include "../engine/archtypes.h"
#include "../tier1/strtools.h"
#include "../dlls/vector.h"

#include "public/interface.h"
#include "vgui_controls/pch_vgui_controls.h"
#include <vgui_controls/keyrepeat.h>

// memdbgon must be the last include file in a .cpp file

using namespace vgui;

vgui::KeyCode g_iCodesForAliases[FM_NUM_KEYREPEAT_ALIASES] = 
{
	KEY_XBUTTON_UP,
	KEY_XBUTTON_DOWN,
	KEY_XBUTTON_LEFT,
	KEY_XBUTTON_RIGHT,
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CKeyRepeatHandler::KeyDown( vgui::KeyCode code )
{
	int iIndex = GetIndexForCode(code);
	if ( iIndex == -1 )
		return;

	if ( m_bAliasDown[ iIndex ] )
		return;

	Reset();
	m_bAliasDown[ iIndex ] = true;
	m_flNextKeyRepeat = system()->GetCurrentTime() + 0.4;
	m_bHaveKeyDown = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CKeyRepeatHandler::KeyUp( vgui::KeyCode code )
{
	int iIndex = GetIndexForCode(code);
	if ( iIndex == -1 )
		return;

	m_bAliasDown[ GetIndexForCode(code) ] = false;

	m_bHaveKeyDown = false;
	for ( int i = 0; i < FM_NUM_KEYREPEAT_ALIASES; i++ )
	{
		if ( m_bAliasDown[i] )
		{
			m_bHaveKeyDown = true;
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
vgui::KeyCode CKeyRepeatHandler::KeyRepeated(void)
{
	if (IsPC())
		return KEY_NONE;

	if (!m_bHaveKeyDown)
		return KEY_NONE;

	if (m_flNextKeyRepeat < system()->GetCurrentTime())
	{
		for (int i = 0; i < FM_NUM_KEYREPEAT_ALIASES; i++)
		{
			if (m_bAliasDown[i])
			{
				m_flNextKeyRepeat = system()->GetCurrentTime() + m_flRepeatTimes[i];
				return g_iCodesForAliases[i];
			}
		}
	}

	return KEY_NONE;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CKeyRepeatHandler::SetKeyRepeatTime( vgui::KeyCode code, float flRepeat )
{
	int iIndex = GetIndexForCode(code);
	Assert( iIndex != -1 );
	m_flRepeatTimes[ iIndex ] = flRepeat;
}

int vgui::CKeyRepeatHandler::GetIndexForCode(vgui::KeyCode code)
{
	switch (code)
	{
	case KEY_XBUTTON_DOWN:
	case KEY_XSTICK1_DOWN:
		return KR_ALIAS_DOWN; break;
	case KEY_XBUTTON_UP:
	case KEY_XSTICK1_UP:
		return KR_ALIAS_UP; break;
	case KEY_XBUTTON_LEFT:
	case KEY_XSTICK1_LEFT:
		return KR_ALIAS_LEFT; break;
	case KEY_XBUTTON_RIGHT:
	case KEY_XSTICK1_RIGHT:
		return KR_ALIAS_RIGHT; break;
	default:
		break;
	}
	return -1;
}
