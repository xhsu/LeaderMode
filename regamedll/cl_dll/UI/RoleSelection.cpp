/*

Created Date: May 26 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#include "precompiled.h"

using namespace vgui;

class CRoleButton : public LMImageButton
{
	DECLARE_CLASS_SIMPLE(CRoleButton, LMImageButton);

public:
	CRoleButton(Panel* parent, const char* panelName, const char* text, RoleTypes iRoleTracking, Panel* pActionSignalTarget = nullptr, const char* pCmd = nullptr) : LMImageButton(parent, panelName, text, pActionSignalTarget, pCmd), m_iRoleTracking(iRoleTracking) {}
	CRoleButton(Panel* parent, const char* panelName, const wchar_t* text, RoleTypes iRoleTracking, Panel* pActionSignalTarget = nullptr, const char* pCmd = nullptr) : LMImageButton(parent, panelName, text, pActionSignalTarget, pCmd), m_iRoleTracking(iRoleTracking) {}

	void OnThink(void) final
	{
		BaseClass::OnThink();

		if (m_iRoleTracking == Role_UNASSIGNED)	// This role is unlimited.
			return;

		for (unsigned i = 0; i < _countof(g_PlayerExtraInfo); i++)
		{
			if (g_PlayerInfoList[i].name == nullptr)	// existence of a player.
				continue;

			if (g_PlayerExtraInfo[i].m_iTeam != g_iTeam)	// Same team?
				continue;

			if (g_PlayerExtraInfo[i].m_iRoleType == m_iRoleTracking)
			{
				SetEnabled(false);
				return;
			}
		}

		// No found: avaliable to choose.
		if (!IsEnabled())
			SetEnabled(true);
	}

private:
	float m_flAlpha{ 255.0f };	// byte
	RoleTypes m_iRoleTracking{ Role_UNASSIGNED };
};

static const char* s_pszRoleButtonInternalNames[ROLE_COUNT] =
{
	"SelectUnassigned",

	"SelectCommander",
	"SelectSWAT",
	"SelectBreacher",
	"SelectSharpshooter",
	"SelectMedic",

	"SelectGodfather",
	"SelectLeadEnforcer",
	"SelectMadScientist",
	"SelectAssassin",
	"SelectArsonist",
};

static const char* s_pszRoleLocalisationKeys[ROLE_COUNT] =
{
	"#LeaderMod_Role_UNASSIGNED",

	"#LeaderMod_Role_Commander",
	"#LeaderMod_Role_SWAT",
	"#LeaderMod_Role_Breacher",
	"#LeaderMod_Role_Sharpshooter",
	"#LeaderMod_Role_Medic",

	"#LeaderMod_Role_Godfather",
	"#LeaderMod_Role_LeadEnforcer",
	"#LeaderMod_Role_MadScientist",
	"#LeaderMod_Role_Assassin",
	"#LeaderMod_Role_Arsonist",
};

static const char* s_pszRolePortraitFile[ROLE_COUNT] =
{
	"sprites/ClassesIcon/Doraemon.dds",

	"sprites/ClassesIcon/CT/Commander.dds",
	"sprites/ClassesIcon/CT/SWAT.dds",
	"sprites/ClassesIcon/CT/Breacher.dds",
	"sprites/ClassesIcon/CT/Sharpshooter.dds",
	"sprites/ClassesIcon/CT/Medic.dds",

	"sprites/ClassesIcon/T/Godfather.dds",
	"sprites/ClassesIcon/T/LeadEnforcer.dds",
	"sprites/ClassesIcon/T/MadScientist.dds",
	"sprites/ClassesIcon/T/Assassin.dds",
	"sprites/ClassesIcon/T/Arsonist.dds",
};

CRoleMenu::CRoleMenu(void) : Frame(nullptr, "RoleMenu")
{
	SetTitle("", true);
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);

	// This element should be constructed in IClientVGUI::Start()
	// Hence this calling is gurenteed safe.
	int iScreenWidth = 0, iScreenTall = 0;
	VGUI_SURFACE->GetScreenSize(iScreenWidth, iScreenTall);
	SetBounds(MARGIN, MARGIN, iScreenWidth - MARGIN * 2, iScreenTall - MARGIN * 2);

	int iPanelWidth = 0, iPanelTall = 0;
	GetSize(iPanelWidth, iPanelTall);

	SetTitleBarVisible(false);
	SetProportional(true);

	SetVisible(false);

	std::array<Vector2D, ROLE_COUNT> rgvPos;
	constexpr auto MARGIN_PERCENTAGE_MIDDLE_PORTRAITS_H = 1.0 / 3.0;
	constexpr auto MARGIN_PERCENTAGE_MIDDLE_PORTRAITS_V = 1.0 / 5.0;
	int iMarginBetweenMiddlePortraitsH = round((iPanelWidth * MARGIN_PERCENTAGE_MIDDLE_PORTRAITS_H) / 3.0);	// 3 interspaces
	int iMarginBetweenMiddlePortraitsV = round((iPanelTall * MARGIN_PERCENTAGE_MIDDLE_PORTRAITS_V) / 2.0);	// only 2 interspaces this time.

	rgvPos[Role_Commander] = rgvPos[Role_Godfather] = Vector2D(
		iPanelWidth / 2 - BUTTON_SIZE / 2,
		iPanelTall / 2 - (BUTTON_SIZE + FONT_SIZE) / 2 - iMarginBetweenMiddlePortraitsV - (BUTTON_SIZE + FONT_SIZE)
	);

	rgvPos[Role_SWAT] = rgvPos[Role_LeadEnforcer] = Vector2D(
		iPanelWidth / 2 - iMarginBetweenMiddlePortraitsH / 2 - BUTTON_SIZE * 2 - iMarginBetweenMiddlePortraitsH,
		iPanelTall / 2 - (BUTTON_SIZE + FONT_SIZE) / 2
	);

	rgvPos[Role_Breacher] = rgvPos[Role_Arsonist] = rgvPos[Role_SWAT] + Vector2D(BUTTON_SIZE + iMarginBetweenMiddlePortraitsH, 0);
	rgvPos[Role_Sharpshooter] = rgvPos[Role_Assassin] = rgvPos[Role_Breacher] + Vector2D(BUTTON_SIZE + iMarginBetweenMiddlePortraitsH, 0);
	rgvPos[Role_Medic] = rgvPos[Role_MadScientist] = rgvPos[Role_Sharpshooter] + Vector2D(BUTTON_SIZE + iMarginBetweenMiddlePortraitsH, 0);

	rgvPos[Role_UNASSIGNED] = rgvPos[Role_Commander] + Vector2D(0, (BUTTON_SIZE + FONT_SIZE) * 2 + iMarginBetweenMiddlePortraitsV * 2);

	for (unsigned i = 0; i < ROLE_COUNT; i++)
	{
		CLASS_PORTRAITS[i].Load(s_pszRolePortraitFile[i]);

		m_rgpButtons[i] = new LMImageButton(this, s_pszRoleButtonInternalNames[i], s_pszRoleLocalisationKeys[i], this);
		m_rgpButtons[i]->SetCommand("role %d", i);
		m_rgpButtons[i]->SetBounds(rgvPos[i].x, rgvPos[i].y, BUTTON_SIZE, BUTTON_SIZE + FONT_SIZE);
		m_rgpButtons[i]->SetVisible(false);
		m_rgpButtons[i]->SetUpImage(&CLASS_PORTRAITS[i]);
		m_rgpButtons[i]->AddGlyphSetToFont("Trajan Pro", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
		m_rgpButtons[i]->AddGlyphSetToFont("I.MingCP", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
		m_rgpButtons[i]->InvalidateLayout(true);
	}
}

CRoleMenu::~CRoleMenu(void)
{
}

void CRoleMenu::OnThink(void)
{
	if (g_iTeam != m_iLastTeam)	// Update team status.
	{
		for (auto pButton : m_rgpButtons)
			pButton->SetVisible(false);

		switch (g_iTeam)
		{
		case TEAM_CT:
			for (int i = Role_Commander; i <= Role_Medic; i++)
				m_rgpButtons[i]->SetVisible(true);

			break;

		case TEAM_TERRORIST:
			for (int i = Role_Godfather; i <= Role_Arsonist; i++)
				m_rgpButtons[i]->SetVisible(true);

			break;
		}

		m_rgpButtons[Role_UNASSIGNED]->SetVisible(true);	// Always avaliable.
		m_iLastTeam = g_iTeam;
	}

	for (unsigned i = Role_UNASSIGNED; i < ROLE_COUNT; i++)	// Update highlighted button.
	{
		if (m_rgpButtons[i]->IsPendingSelected())
		{
			m_iSelectedRole = (RoleTypes)i;
			break;
		}
		else
			m_iSelectedRole = ROLE_COUNT;	// Error.
	}
}

void CRoleMenu::Paint(void)
{
	BaseClass::Paint();

	if (m_iSelectedRole < ROLE_START || m_iSelectedRole > ROLE_END)
		return;

	DrawMouseHoveredEffects(m_rgpButtons[m_iSelectedRole]);
}

void CRoleMenu::OnCommand(const char* command)
{
	Show(false);

	if (Q_strstr(command, "role"))
		gEngfuncs.pfnServerCmd(command);
	else
		BaseClass::OnCommand(command);
}
