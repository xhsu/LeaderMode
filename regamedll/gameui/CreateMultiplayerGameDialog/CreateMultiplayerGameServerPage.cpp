/*

Copied Date: May 23 2021 [CS:BTE Alpha]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#include "precompiled.h"

using namespace vgui;

#define RANDOM_MAP "#GameUI_RandomMap"

CCreateMultiplayerGameServerPage::CCreateMultiplayerGameServerPage(vgui::Panel* parent, const char* name) : PropertyPage(parent, name)
{
	m_pSavedData = NULL;

	m_pMapList = new ComboBox(this, "MapList", 12, false);

	m_pEnableBotsCheck = new CheckButton(this, "EnableBotsCheck", "");
	m_pEnableBotsCheck->SetVisible(false);
	m_pEnableBotsCheck->SetEnabled(false);

	LoadControlSettings("Resource/CreateMultiplayerGameServerPage.res");

	LoadMapList();


	m_iMaxPlayers = gEngfuncs.GetMaxClients();

	if (m_iMaxPlayers <= 1)
		m_iMaxPlayers = 20;

	SetControlString("ServerNameEdit", ModInfo().GetGameDescription());
	SetControlString("PasswordEdit", gEngfuncs.pfnGetCvarString("sv_password"));

	int maxPlayersEdit = atoi(GetControlString("MaxPlayersEdit", "-1"));

	if (maxPlayersEdit <= 1)
	{
		char szBuffer[4];
		Q_slprintf(szBuffer, "%d", m_iMaxPlayers);
		szBuffer[sizeof(szBuffer) - 1] = '\0';
		SetControlString("MaxPlayersEdit", szBuffer);
	}
}

CCreateMultiplayerGameServerPage::~CCreateMultiplayerGameServerPage(void)
{
	m_vMapCache.clear();
}

void CCreateMultiplayerGameServerPage::EnableBots(KeyValues* data)
{
	m_pSavedData = data;

	int quota = data->GetInt("bot_quota", 0);
	SetControlInt("BotQuotaCombo", quota);
	m_pEnableBotsCheck->SetSelected(quota > 0);

	int difficulty = Q_clamp(data->GetInt("bot_difficulty", 0), 0, 3);

	char buttonName[64];
	Q_snprintf(buttonName, sizeof(buttonName), "SkillLevel%d", difficulty);
	vgui::RadioButton* button = dynamic_cast<vgui::RadioButton*>(FindChildByName(buttonName));

	if (button)
		button->SetSelected(true);
}

void CCreateMultiplayerGameServerPage::OnApplyChanges(void)
{
	Q_strncpy(m_szHostName, GetControlString("ServerNameEdit", "Leader Mode"), DATA_STR_LENGTH);
	Q_strncpy(m_szPassword, GetControlString("PasswordEdit", ""), DATA_STR_LENGTH);
	m_iMaxPlayers = atoi(GetControlString("MaxPlayersEdit", "9"));

	KeyValues* kv = m_pMapList->GetActiveItemUserData();
	Q_strncpy(m_szMapName, kv->GetString("mapname", ""), DATA_STR_LENGTH);

	if (m_pSavedData)
	{
		int quota = GetControlInt("BotQuotaCombo", 0);

		if (!m_pEnableBotsCheck->IsSelected())
			quota = 0;

		m_pSavedData->SetInt("bot_quota", quota);

		int difficulty = 0;

		for (int i = 0; i < 4; ++i)
		{
			char buttonName[64];
			Q_snprintf(buttonName, sizeof(buttonName), "SkillLevel%d", i);
			vgui::RadioButton* button = dynamic_cast<vgui::RadioButton*>(FindChildByName(buttonName));

			if (button)
			{
				if (button->IsSelected())
				{
					difficulty = i;
					break;
				}
			}
		}

		m_pSavedData->SetInt("bot_difficulty", difficulty);
	}
}

void CCreateMultiplayerGameServerPage::LoadMaps(const char* pszPathID)
{
	FileFindHandle_t findHandle = NULL;
	const char* pszFilename = g_pFullFileSystem->FindFirst("maps/*.bsp", &findHandle, pszPathID);

	while (pszFilename)
	{
		char mapname[256];
		Q_slprintf(mapname, "maps/%s", pszFilename);

		const char* str = Q_strstr(pszFilename, "maps");

		if (str)
			Q_strncpy(mapname, str + 5, sizeof(mapname) - 1);
		else
			Q_strncpy(mapname, pszFilename, sizeof(mapname) - 1);

		char* ext = Q_strstr(mapname, ".bsp");

		if (ext)
			*ext = 0;

		if (!Q_stricmp(ModInfo().GetGameDescription(), "Half-Life") && (mapname[0] == 'c' || mapname[0] == 't') && mapname[2] == 'a' && mapname[1] >= '0' && mapname[1] <= '5')
		{
			goto nextFile;	// C2362
		}
		else
		{
			bool found = false;

			for (auto& cachedMapName : m_vMapCache)
			{
				if (cachedMapName == mapname)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				m_vMapCache.push_back(mapname);
			}
		}

	nextFile:
		pszFilename = g_pFullFileSystem->FindNext(findHandle);
	}

	g_pFullFileSystem->FindClose(findHandle);
}

void CCreateMultiplayerGameServerPage::LoadMapList(void)
{
	m_pMapList->DeleteAllItems();
	m_pMapList->AddItem(RANDOM_MAP, new KeyValues("data", "mapname", RANDOM_MAP));

	m_vMapCache.clear();

	if (!Q_stricmp(ModInfo().GetGameDescription(), "Half-Life"))
	{
		LoadMaps(NULL);
	}
	else
	{
		LoadMaps("GAME_FALLBACK");
		LoadMaps("GAME");
	}

	std::sort(m_vMapCache.begin(), m_vMapCache.end());

	for (auto& str_mapName : m_vMapCache)
	{
		auto mapname = str_mapName.c_str();
		m_pMapList->AddItem(mapname, new KeyValues("data", "mapname", mapname));
	}

	m_pMapList->ActivateItem(0);
}

bool CCreateMultiplayerGameServerPage::IsRandomMapSelected(void)
{
	const char* mapname = m_pMapList->GetActiveItemUserData()->GetString("mapname");

	if (!Q_stricmp(mapname, RANDOM_MAP))
		return true;

	return false;
}

const char* CCreateMultiplayerGameServerPage::GetMapName(void)
{
	int count = m_pMapList->GetItemCount();

	if (count <= 1)
		return NULL;

	const char* mapname = m_pMapList->GetActiveItemUserData()->GetString("mapname");

	if (!Q_strcmp(mapname, RANDOM_MAP))
	{
		int which = gEngfuncs.pfnRandomLong(1, count - 1);
		mapname = m_pMapList->GetItemUserData(which)->GetString("mapname");
	}

	return mapname;
}

void CCreateMultiplayerGameServerPage::SetMap(const char* mapName)
{
	for (int i = 0; i < m_pMapList->GetItemCount(); i++)
	{
		if (!m_pMapList->IsItemIDValid(i))
			continue;

		if (!Q_stricmp(m_pMapList->GetItemUserData(i)->GetString("mapname"), mapName))
		{
			m_pMapList->ActivateItem(i);
			break;
		}
	}
}

void CCreateMultiplayerGameServerPage::OnCheckButtonChecked(void)
{
	SetControlEnabled("SkillLevel0", m_pEnableBotsCheck->IsSelected());
	SetControlEnabled("SkillLevel1", m_pEnableBotsCheck->IsSelected());
	SetControlEnabled("SkillLevel2", m_pEnableBotsCheck->IsSelected());
	SetControlEnabled("SkillLevel3", m_pEnableBotsCheck->IsSelected());
	SetControlEnabled("BotQuotaCombo", m_pEnableBotsCheck->IsSelected());
	SetControlEnabled("BotQuotaLabel", m_pEnableBotsCheck->IsSelected());
	SetControlEnabled("BotDifficultyLabel", m_pEnableBotsCheck->IsSelected());
}

char* CCreateMultiplayerGameServerPage::GetBOTCommandBuffer(void)
{
	if (!m_pSavedData)
		return NULL;

	int quota = GetControlInt("BotQuotaCombo", 0);

	if (!m_pEnableBotsCheck->IsSelected())
		quota = 0;

	m_pSavedData->SetInt("bot_quota", quota);

	int difficulty = 0;

	for (int i = 0; i < 4; ++i)
	{
		char buttonName[64];
		Q_snprintf(buttonName, sizeof(buttonName), "SkillLevel%d", i);
		vgui::RadioButton* button = dynamic_cast<vgui::RadioButton*>(FindChildByName(buttonName));

		if (button)
		{
			if (button->IsSelected())
			{
				difficulty = i;
				break;
			}
		}
	}

	m_pSavedData->SetInt("bot_difficulty", difficulty);

	static char buffer[1024];
	Q_slprintf(buffer, "wait\nbot_auto_vacate 1\nbot_quota %d\nbot_difficulty %d\n", quota, difficulty);
	return buffer;
}
