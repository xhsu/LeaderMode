/*

Copied Date: May 22 2021 [CS:BTE Alpha]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#include "precompiled.h"

using namespace vgui;

enum BotGUITeamType : BYTE
{
	BOT_GUI_TEAM_RANDOM = 0,
	BOT_GUI_TEAM_CT = 1,
	BOT_GUI_TEAM_T = 2
};

static const char* joinTeamArg[] = { "any", "ct", "t", NULL };

enum BotGUIChatterType : BYTE
{
	BOT_GUI_CHATTER_NORMAL = 0,
	BOT_GUI_CHATTER_MINIMAL = 1,
	BOT_GUI_CHATTER_RADIO = 2,
	BOT_GUI_CHATTER_OFF = 3
};

static const char* chatterArg[] = { "normal", "minimal", "radio", "off", NULL };

extern void UTIL_StripInvalidCharacters(char* pszInput);

void CCreateMultiplayerGameBotPage::SetJoinTeamCombo(const char* team)
{
	if (team)
	{
		for (int i = 0; joinTeamArg[i]; ++i)
		{
			if (!Q_stricmp(team, joinTeamArg[i]))
			{
				m_joinTeamCombo->ActivateItemByRow(i);
				return;
			}
		}
	}
	else
		m_joinTeamCombo->ActivateItemByRow(BOT_GUI_TEAM_RANDOM);
}

void CCreateMultiplayerGameBotPage::SetChatterCombo(const char* chatter)
{
	if (chatter)
	{
		for (int i = 0; chatterArg[i]; ++i)
		{
			if (!Q_stricmp(chatter, chatterArg[i]))
			{
				m_chatterCombo->ActivateItemByRow(i);
				return;
			}
		}
	}
	else
		m_joinTeamCombo->ActivateItemByRow(BOT_GUI_CHATTER_NORMAL);
}

CCreateMultiplayerGameBotPage::CCreateMultiplayerGameBotPage(vgui::Panel* parent, const char* name, KeyValues* botKeys) : PropertyPage(parent, name)
{
	m_pSavedData = botKeys;

	m_allowRogues = new CCvarToggleCheckButton(this, "BotAllowRogueCheck", "", "bot_allow_rogues");
	m_allowPistols = new CCvarToggleCheckButton(this, "BotAllowPistolsCheck", "", "bot_allow_pistols");
	m_allowShotguns = new CCvarToggleCheckButton(this, "BotAllowShotgunsCheck", "", "bot_allow_shotguns");
	m_allowSubmachineGuns = new CCvarToggleCheckButton(this, "BotAllowSubmachineGunsCheck", "", "bot_allow_sub_machine_guns");
	m_allowRifles = new CCvarToggleCheckButton(this, "BotAllowRiflesCheck", "", "bot_allow_rifles");
	m_allowMachineGuns = new CCvarToggleCheckButton(this, "BotAllowMachineGunsCheck", "", "bot_allow_machine_guns");
	m_allowGrenades = new CCvarToggleCheckButton(this, "BotAllowGrenadesCheck", "", "bot_allow_grenades");
	m_allowSnipers = new CCvarToggleCheckButton(this, "BotAllowSnipersCheck", "", "bot_allow_snipers");

	m_joinAfterPlayer = new CCvarToggleCheckButton(this, "BotJoinAfterPlayerCheck", "", "bot_join_after_player");

	m_deferToHuman = new CCvarToggleCheckButton(this, "BotDeferToHumanCheck", "", "bot_defer_to_human");

	m_joinTeamCombo = new ComboBox(this, "BotJoinTeamCombo", 3, false);
	m_joinTeamCombo->AddItem("#Cstrike_Random", NULL);
	m_joinTeamCombo->AddItem("#Cstrike_ScoreBoard_CT", NULL);
	m_joinTeamCombo->AddItem("#Cstrike_ScoreBoard_Ter", NULL);

	m_chatterCombo = new ComboBox(this, "BotChatterCombo", 4, false);
	m_chatterCombo->AddItem("#Cstrike_Bot_Chatter_Normal", NULL);
	m_chatterCombo->AddItem("#Cstrike_Bot_Chatter_Minimal", NULL);
	m_chatterCombo->AddItem("#Cstrike_Bot_Chatter_Radio", NULL);
	m_chatterCombo->AddItem("#Cstrike_Bot_Chatter_Off", NULL);

	m_prefixEntry = new TextEntry(this, "BotPrefixEntry");
	m_profileEntry = new TextEntry(this, "BotProfileEntry");

	LoadControlSettings("Resource/CreateMultiplayerGameBotPage.res");
	//m_profileEntry->SetBounds(132, 20, 128, 24);
	//m_profileEntry->SetAutoResize(PIN_TOPLEFT, AUTORESIZE_NO, 132, 20, 0, 0);

	m_joinAfterPlayer->SetSelected(botKeys->GetInt("bot_join_after_player", 1));
	m_allowRogues->SetSelected(botKeys->GetInt("bot_allow_rogues", 1));
	m_allowPistols->SetSelected(botKeys->GetInt("bot_allow_pistols", 1));
	m_allowShotguns->SetSelected(botKeys->GetInt("bot_allow_shotguns", 1));
	m_allowSubmachineGuns->SetSelected(botKeys->GetInt("bot_allow_sub_machine_guns", 1));
	m_allowMachineGuns->SetSelected(botKeys->GetInt("bot_allow_machine_guns", 1));
	m_allowRifles->SetSelected(botKeys->GetInt("bot_allow_rifles", 1));
	m_allowSnipers->SetSelected(botKeys->GetInt("bot_allow_snipers", 1));
	m_allowGrenades->SetSelected(botKeys->GetInt("bot_allow_grenades", 1));
	m_deferToHuman->SetSelected(botKeys->GetInt("bot_defer_to_human", 1));

	SetJoinTeamCombo(botKeys->GetString("bot_join_team", "any"));
	SetChatterCombo(botKeys->GetString("bot_chatter", "normal"));

	const char* prefix = botKeys->GetString("bot_prefix");

	if (prefix)
		SetControlString("BotPrefixEntry", prefix);

	const char* profile = botKeys->GetString("bot_profile_db");

	if (profile)
		SetControlString("BotProfileEntry", profile);
}

CCreateMultiplayerGameBotPage::~CCreateMultiplayerGameBotPage(void)
{
}

void CCreateMultiplayerGameBotPage::OnResetChanges(void)
{
}

void UpdateValue(KeyValues* data, const char* cvarName, int value)
{
	data->SetInt(cvarName, value);
}

void UpdateValue(KeyValues* data, const char* cvarName, const char* value)
{
	data->SetString(cvarName, value);
}

void CCreateMultiplayerGameBotPage::OnApplyChanges(void)
{
	UpdateValue(m_pSavedData, "bot_join_after_player", m_joinAfterPlayer->IsSelected());
	UpdateValue(m_pSavedData, "bot_allow_rogues", m_allowRogues->IsSelected());
	UpdateValue(m_pSavedData, "bot_allow_pistols", m_allowPistols->IsSelected());
	UpdateValue(m_pSavedData, "bot_allow_shotguns", m_allowShotguns->IsSelected());
	UpdateValue(m_pSavedData, "bot_allow_sub_machine_guns", m_allowSubmachineGuns->IsSelected());
	UpdateValue(m_pSavedData, "bot_allow_machine_guns", m_allowMachineGuns->IsSelected());
	UpdateValue(m_pSavedData, "bot_allow_rifles", m_allowRifles->IsSelected());
	UpdateValue(m_pSavedData, "bot_allow_snipers", m_allowSnipers->IsSelected());
	UpdateValue(m_pSavedData, "bot_allow_grenades", m_allowGrenades->IsSelected());

	UpdateValue(m_pSavedData, "bot_defer_to_human", m_deferToHuman->IsSelected());
	UpdateValue(m_pSavedData, "bot_join_team", joinTeamArg[m_joinTeamCombo->GetActiveItem()]);
	UpdateValue(m_pSavedData, "bot_chatter", chatterArg[m_chatterCombo->GetActiveItem()]);

	char entryBuffer[256];
	m_prefixEntry->GetText(entryBuffer, sizeof(entryBuffer));
	UpdateValue(m_pSavedData, "bot_prefix", entryBuffer);

	m_profileEntry->GetText(entryBuffer, sizeof(entryBuffer));
	UpdateValue(m_pSavedData, "bot_profile_db", entryBuffer);
}

char* CCreateMultiplayerGameBotPage::GetBOTCommandBuffer(void)
{
	char entryBuffer[256], profile[256];
	m_profileEntry->GetText(entryBuffer, sizeof(entryBuffer));

	if (Q_strlen(entryBuffer))
		Q_slprintf(profile, "BotProfile_%s.db", entryBuffer);
	else
		Q_slprintf(profile, "BotProfile.db");

	m_prefixEntry->GetText(entryBuffer, sizeof(entryBuffer));

	static char buffer[1024];
	Q_slprintf(buffer,

		"bot_join_after_player %d\n"
		"bot_allow_rogues %d\n"

		"bot_allow_pistols %d\n"
		"bot_allow_shotguns %d\n"
		"bot_allow_sub_machine_guns %d\n"
		"bot_allow_machine_guns %d\n"
		"bot_allow_rifles %d\n"
		"bot_allow_snipers %d\n"
		"bot_allow_grenades %d\n"
		"bot_allow_shield 0\n"

		"bot_defer_to_human %d\n"
		"bot_join_team \"%s\"\n"
		"bot_prefix \"%s\"\n"
		"bot_chatter \"%s\"\n"
		"bot_profile_db \"%s\"\n",

		m_joinAfterPlayer->IsSelected(),
		m_allowRogues->IsSelected(),

		m_allowPistols->IsSelected(),
		m_allowShotguns->IsSelected(),
		m_allowSubmachineGuns->IsSelected(),
		m_allowMachineGuns->IsSelected(),
		m_allowRifles->IsSelected(),
		m_allowSnipers->IsSelected(),
		m_allowGrenades->IsSelected(),

		m_deferToHuman->IsSelected(),
		joinTeamArg[m_joinTeamCombo->GetActiveItem()],
		entryBuffer,	// bot_prefix
		chatterArg[m_chatterCombo->GetActiveItem()],
		profile);	// bot_profile_db

	return buffer;
}
