//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef IGAMEUI_H
#define IGAMEUI_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

#ifndef ESTEAMLOGINFAILURE_DEFINED
// reasons why the user can't connect to a game server
enum ESteamLoginFailure
{
	STEAMLOGINFAILURE_NONE,
	STEAMLOGINFAILURE_BADTICKET,
	STEAMLOGINFAILURE_NOSTEAMLOGIN,
	STEAMLOGINFAILURE_VACBANNED,
	STEAMLOGINFAILURE_LOGGED_IN_ELSEWHERE,
	STEAMLOGINFAILURE_CONNECTIONLOST,
	STEAMLOGINFAILURE_NOCONNECTION,
};
#define ESTEAMLOGINFAILURE_DEFINED
#endif

struct cl_enginefuncs_s;
class IBaseSystem;	

namespace vgui2
{
class Panel;
}

//-----------------------------------------------------------------------------
// Purpose: contains all the functions that the GameUI dll exports
//			GameUI_GetInterface() is exported via dll export table to get this table
//-----------------------------------------------------------------------------
class IGameUI : public IBaseInterface
{
public:
	virtual void Initialize( CreateInterfaceFn *factories, int count );
	virtual void Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, IBaseSystem *system);
	virtual void Shutdown();

	virtual int	ActivateGameUI();	// activates the menus, returns 0 if it doesn't want to handle it
	virtual int	ActivateDemoUI();	// activates the demo player, returns 0 if it doesn't want to handle it

	virtual int	HasExclusiveInput();

	virtual void RunFrame();

	virtual void ConnectToServer(const char *game, int IP, int port);
	virtual void DisconnectFromServer();
	virtual void HideGameUI();

	virtual bool IsGameUIActive();
	
	virtual void LoadingStarted(const char *resourceType, const char *resourceName);
	virtual void LoadingFinished(const char *resourceType, const char *resourceName);

	virtual void StartProgressBar(const char *progressType, int numProgressPoints);
	virtual int	 ContinueProgressBar(int progressPoint, float progressFraction);
	virtual void StopProgressBar(bool bError, const char *failureReasonIfAny, const char *extendedReason);
	virtual int  SetProgressBarStatusText(const char *statusText);
	virtual void SetSecondaryProgressBar(float progress);
	virtual void SetSecondaryProgressBarText(const char *statusText);

	virtual void ValidateCDKey(bool force, bool inConnect);

	virtual void OnDisconnectFromServer( int eSteamLoginFailure, const char *username );
};

// the interface version is the number to call GameUI_GetInterface(int interfaceNumber) with
#define GAMEUI_INTERFACE_VERSION "GameUI007"

#endif // IGAMEUI_H
