/*

Copied Date: May 22 2021 [CS:BTE Alpha]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#include "precompiled.h"

using namespace vgui;

CCreateMultiplayerGameDialog::CCreateMultiplayerGameDialog(vgui::Panel* parent) : PropertyDialog(parent, "CreateMultiplayerGameDialog")
{
	SetSize(348, 460);

	SetTitle("#GameUI_CreateServer", true);
	SetOKButtonText("#GameUI_Start");

	m_pSavedData = new KeyValues("ServerConfig");

	if (m_pSavedData)
	{
		m_pSavedData->LoadFromFile(g_pFullFileSystem, "cfg/ServerConfig.vdf");

		const char* startMap = m_pSavedData->GetString("map", "");

		if (startMap[0])
			m_pServerPage->SetMap(startMap);
	}

	m_pServerPage = new CCreateMultiplayerGameServerPage(this, "ServerPage");
	m_pGameplayPage = new CCreateMultiplayerGameGameplayPage(this, "GameplayPage");
	m_pBotPage = new CCreateMultiplayerGameBotPage(this, "BotPage", m_pSavedData);

	AddPage(m_pServerPage, "#GameUI_Server");
	AddPage(m_pGameplayPage, "#GameUI_Game");
	AddPage(m_pBotPage, "#GameUI_CPUPlayerOptions");

	m_pServerPage->EnableBots(m_pSavedData);
}

CCreateMultiplayerGameDialog::~CCreateMultiplayerGameDialog(void)
{
	if (m_pSavedData)
	{
		m_pSavedData->deleteThis();
		m_pSavedData = NULL;
	}
}

bool CCreateMultiplayerGameDialog::OnOK(bool applyOnly)
{
	BaseClass::OnOK(applyOnly);

	char szMapName[64], szHostName[64], szPassword[64];
	const char* pszMapName = m_pServerPage->GetMapName();

	if (!pszMapName)
		return false;

	Q_strlcpy(szMapName, pszMapName);
	Q_strlcpy(szHostName, m_pGameplayPage->GetHostName());
	Q_strlcpy(szPassword, m_pGameplayPage->GetPassword());

	if (m_pSavedData)
	{
		if (m_pServerPage->IsRandomMapSelected())
			m_pSavedData->SetString("map", "");
		else
			m_pSavedData->SetString("map", szMapName);

		m_pSavedData->SaveToFile(g_pFullFileSystem, "cfg/ServerConfig.vdf");
	}

	SetVisible(false);

	char szMapCommand[1024];
	Q_slprintf(szMapCommand, "disconnect\nsv_lan 1\nsetmaster disable\nmaxplayers %i\nsv_password \"%s\"\nhostname \"%s\"\ncd fadeout\nmap %s\n", m_pGameplayPage->GetMaxPlayers(), szPassword, szHostName, szMapName);
	gEngfuncs.pfnClientCmd(szMapCommand);

	char* botCmdBuf = m_pServerPage->GetBOTCommandBuffer();

	if (botCmdBuf)
		gEngfuncs.pfnClientCmd(botCmdBuf);

	if (m_pBotPage)
		gEngfuncs.pfnClientCmd(m_pBotPage->GetBOTCommandBuffer());

	m_pGameplayPage->SaveValues();
	return true;
}
