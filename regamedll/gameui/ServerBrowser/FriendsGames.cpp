/*

Copied Date: May 22 2021 [CS:BTE Alpha]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#include "precompiled.h"

using namespace vgui;

CFriendsGames::CFriendsGames(vgui::Panel *parent) : CBaseGamesPage(parent, "FriendsGames", eFriendsServer)
{
	m_iServerRefreshCount = 0;

	if (!IsSteamGameServerBrowsingEnabled())
	{
		m_pGameList->SetEmptyListText("#ServerBrowser_OfflineMode");
		m_pConnect->SetEnabled(false);
		m_pRefreshAll->SetEnabled(false);
		m_pRefreshQuick->SetEnabled(false);
		m_pAddServer->SetEnabled(false);
		m_pFilter->SetEnabled(false);
	}
}

CFriendsGames::~CFriendsGames(void)
{
}

bool CFriendsGames::SupportsItem(InterfaceItem_e item)
{
	switch (item)
	{
		case FILTERS: return true;
	}

	return false;
}

void CFriendsGames::RefreshComplete(HServerListRequest hRequest, EMatchMakingServerResponse response)
{
	SetRefreshing(false);

	m_pGameList->SortList();
	m_iServerRefreshCount = 0;

	if (IsSteamGameServerBrowsingEnabled())
	{
		m_pGameList->SetEmptyListText("#ServerBrowser_NoFriendsServers");
	}
}

void CFriendsGames::OnOpenContextMenu(int itemID)
{
	if (!m_pGameList->GetSelectedItemsCount())
		return;

	int serverID = m_pGameList->GetItemData(m_pGameList->GetSelectedItem(0))->userData;

	CServerContextMenu *menu = ServerBrowserDialog().GetContextMenu(m_pGameList);
	menu->ShowMenu(this, serverID, true, true, true, true);
}
