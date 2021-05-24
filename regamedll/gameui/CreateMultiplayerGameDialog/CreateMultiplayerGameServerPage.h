/*

Copied Date: May 23 2021 [CS:BTE Alpha]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#ifndef CREATEMULTIPLAYERGAMESERVERPAGE_H
#define CREATEMULTIPLAYERGAMESERVERPAGE_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/CvarToggleCheckButton.h>
#include <vector>
#include <string>

class CCreateMultiplayerGameServerPage : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE(CCreateMultiplayerGameServerPage, vgui::PropertyPage);

public:
	CCreateMultiplayerGameServerPage(vgui::Panel* parent, const char* name);
	~CCreateMultiplayerGameServerPage(void);

public:
	void SetMap(const char* name);
	bool IsRandomMapSelected(void);
	const char* GetMapName(void);

public:
	void EnableBots(KeyValues* data);
	inline int GetBotQuota(void) { return GetControlInt("BotQuotaCombo", 0); }
	inline bool GetBotsEnabled(void) { return m_pEnableBotsCheck->IsSelected(); }

protected:
	virtual void OnApplyChanges(void);

protected:
	MESSAGE_FUNC(OnCheckButtonChecked, "CheckButtonChecked");

public:
	char* GetBOTCommandBuffer(void);

private:
	void LoadMapList(void);
	void LoadMaps(const char* pszPathID);

private:
	vgui::ComboBox* m_pMapList;
	vgui::CheckButton* m_pEnableBotsCheck;
	CCvarToggleCheckButton* m_pEnableTutorCheck;
	KeyValues* m_pSavedData;

	enum { DATA_STR_LENGTH = 64 };

	char m_szHostName[DATA_STR_LENGTH];
	char m_szPassword[DATA_STR_LENGTH];
	char m_szMapName[DATA_STR_LENGTH];
	int m_iMaxPlayers;

	std::vector<std::string> m_vMapCache;
};

#endif
