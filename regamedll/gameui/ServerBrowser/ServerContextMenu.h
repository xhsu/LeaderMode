/*

Copied Date: May 22 2021 [CS:BTE Alpha]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#ifndef SERVERCONTEXTMENU_H
#define SERVERCONTEXTMENU_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Menu.h>

class CServerContextMenu : public vgui::Menu
{
public:
	CServerContextMenu(vgui::Panel *parent);
	~CServerContextMenu();

public:
	void ShowMenu(vgui::Panel *target, unsigned int serverID, bool showConnect, bool showViewGameInfo, bool showRefresh, bool showAddToFavorites);
};

#endif
