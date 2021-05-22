/*

Copied Date: May 22 2021 [CS:BTE Alpha]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#include "precompiled.h"

CSpectateGames::CSpectateGames(vgui::Panel *parent) : CInternetGames(parent, "SpectateGames", eSpectatorServer)
{
}

void CSpectateGames::GetNewServerList(void)
{
	m_vecServerFilters.AddToTail(MatchMakingKeyValuePair_t("proxy", "1"));

	BaseClass::GetNewServerList();
}
