/*

Created Date: May 24 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#include "precompiled.h"

using namespace vgui;

class CTeamButton : public LMImageButton
{
	DECLARE_CLASS_SIMPLE(CTeamButton, LMImageButton);

public:
	CTeamButton(Panel* parent, const char* panelName, const char* text, unsigned short iTeamTracking, Panel* pActionSignalTarget = nullptr, const char* pCmd = nullptr) : LMImageButton(parent, panelName, text, pActionSignalTarget, pCmd), m_iTeamTracking(iTeamTracking) {}
	CTeamButton(Panel* parent, const char* panelName, const wchar_t* text, unsigned short iTeamTracking, Panel* pActionSignalTarget = nullptr, const char* pCmd = nullptr) : LMImageButton(parent, panelName, text, pActionSignalTarget, pCmd), m_iTeamTracking(iTeamTracking) {}

	void Paint(void) final
	{
		BaseClass::Paint();

		Vector2D vecPos = m_vecExistingPlayerTrackerStartingPos;

		for (unsigned short i = 0; i < m_iPlayerCounts; i++)
		{
			DrawUtils::glRegularTexDrawingInit(0xFFFFFF, 0xFF);
			DrawUtils::glSetTexture(CTeamMenu::m_sPlayerCountIcon.m_iId);
			DrawUtils::Draw2DQuad(vecPos, vecPos + m_vecPawnIconScaledSize);

			vecPos.x -= MARGIN_BETWEEN_PAWN_ICONS;
			vecPos.x -= m_vecPawnIconScaledSize.width;	// Moving left.
		}
	}

	void OnThink(void) final
	{
		BaseClass::OnThink();

		m_iPlayerCounts = 0;
		for (int i = 0; i <= gEngfuncs.GetMaxClients(); i++)
		{
			if (g_PlayerInfoList[i].name == nullptr)	// existence of a player.
				continue;

			if (g_PlayerExtraInfo[i].m_iTeam != m_iTeamTracking)
				continue;

			m_iPlayerCounts++;
		}
	}

	void InvalidateLayout(bool layoutNow = false, bool reloadScheme = false) final
	{
		BaseClass::InvalidateLayout(layoutNow, reloadScheme);

		m_vecPawnIconScaledSize.width = round(m_flSparedBlankHeight * CTeamMenu::m_sPlayerCountIcon.m_flW2HRatio);
		m_vecPawnIconScaledSize.height = round(m_flSparedBlankHeight);
		m_vecExistingPlayerTrackerStartingPos = Vector2D(GetWide() - m_vecPawnIconScaledSize.width, GetWide()) - MARGIN_PAWN_ICON;	// Assume the picture is a square.
	}

	void SetTeamTracking(unsigned short iTeam)
	{
		m_iTeamTracking = Q_clamp<unsigned short>(iTeam, TEAM_UNASSIGNED, TEAM_SPECTATOR);
	}

public:
	static constexpr auto MARGIN_BETWEEN_PAWN_ICONS = 2;
	static constexpr auto MARGIN_PAWN_ICON = Vector2D(2);

private:
	unsigned short m_iPlayerCounts{ 0 };
	unsigned short m_iTeamTracking{ TEAM_UNASSIGNED };
	Vector2D m_vecExistingPlayerTrackerStartingPos{ Vector2D() };
	Vector2D m_vecPawnIconScaledSize{ Vector2D() };
};

CTeamMenu::CTeamMenu(void) : Frame(nullptr, "TeamMenu")
{
	SetTitle("", true);
	SetScheme("ClientScheme");
	SetMoveable(false);
	SetSizeable(false);

	// This element should be constructed in IClientVGUI::Start()
	// Hence this calling is gurenteed safe.
	int iScreenWidth = 0, iScreenTall = 0;
	VGUI_SURFACE->GetScreenSize(iScreenWidth, iScreenTall);
	SetBounds(MARGIN, MARGIN, iScreenWidth - MARGIN * 2, iScreenTall - MARGIN * 2 - 12);

	SetTitleBarVisible(false);
	SetProportional(true);

	SetVisible(false);

	if (!m_sPlayerCountIcon)
		m_sPlayerCountIcon.Load("sprites/Miscellaneous/SchemeNumber.dds");

	Vector2D vecTButtonPos = Vector2D(iScreenWidth, iScreenTall) / 2 - Vector2D(MARGIN_BUTTON, BUTTON_SIZE) / 2 - Vector2D(BUTTON_SIZE, 0);
	Vector2D vecCTButtonPos = Vector2D(iScreenWidth, iScreenTall) / 2 + Vector2D(MARGIN_BUTTON, -BUTTON_SIZE) / 2;

	m_pButtonT = new CTeamButton(this, "SelectT", "#LeaderMod_SB_Ter_Short", TEAM_TERRORIST, this, "jointeam 1");
	m_pButtonT->SetBounds(vecTButtonPos.x, vecTButtonPos.y, BUTTON_SIZE, BUTTON_SIZE + FONT_SIZE);
	m_pButtonT->SetVisible(true);
	m_pButtonT->SetUpImage("sprites/ClassesIcon/T/Godfather.dds");
	m_pButtonT->SetFocusImage("sprites/ClassesIcon/T/Godfather.dds");
	m_pButtonT->SetDownImage("sprites/ClassesIcon/T/Godfather.dds");
	m_pButtonT->SetDisableImage("sprites/ClassesIcon/T/Godfather.dds");
	m_pButtonT->AddGlyphSetToFont("Trajan Pro", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
	m_pButtonT->AddGlyphSetToFont("I.MingCP", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
	m_pButtonT->InvalidateLayout(true);

	m_pButtonCT = new CTeamButton(this, "SelectCT", "#LeaderMod_SB_CT_Short", TEAM_CT, this, "jointeam 2");
	m_pButtonCT->SetBounds(vecCTButtonPos.x, vecCTButtonPos.y, BUTTON_SIZE, BUTTON_SIZE + FONT_SIZE);
	m_pButtonCT->SetVisible(true);
	m_pButtonCT->SetUpImage("sprites/ClassesIcon/CT/Commander.dds");
	m_pButtonCT->SetFocusImage("sprites/ClassesIcon/CT/Commander.dds");
	m_pButtonCT->SetDownImage("sprites/ClassesIcon/CT/Commander.dds");
	m_pButtonCT->SetDisableImage("sprites/ClassesIcon/CT/Commander.dds");
	m_pButtonCT->AddGlyphSetToFont("Trajan Pro", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
	m_pButtonCT->AddGlyphSetToFont("I.MingCP", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
	m_pButtonCT->InvalidateLayout(true);

	/*m_pButtonObserver = new LMImageButton(this, "SelectS", L"SelectS", this, "jointeam 3");
	m_pButtonObserver->SetBounds(64, 64 + BUTTON_SIZE * 2, BUTTON_SIZE, BUTTON_SIZE);
	m_pButtonObserver->SetVisible(true);
	m_pButtonObserver->SetUpImage("sprites/Inventory/NVG.dds");
	m_pButtonObserver->SetFocusImage("sprites/Inventory/NVG.dds");
	m_pButtonObserver->SetDownImage("sprites/Inventory/NVG.dds");
	m_pButtonObserver->SetDisableImage("sprites/Inventory/NVG.dds");*/
}

CTeamMenu::~CTeamMenu(void)
{
}

void CTeamMenu::OnCommand(const char* command)
{
	Show(false);

	if (Q_strstr(command, "jointeam"))
		gEngfuncs.pfnServerCmd(command);
	else
		BaseClass::OnCommand(command);
}

void CTeamMenu::OnThink(void)
{
	BaseClass::OnThink();

	if (m_pButtonCT->IsPendingSelected())
		m_iHighlightedButton = TEAM_CT;
	else if (m_pButtonT->IsPendingSelected())
		m_iHighlightedButton = TEAM_TERRORIST;
	else
		m_iHighlightedButton = TEAM_UNASSIGNED;
}

void CTeamMenu::Paint(void)
{
	BaseClass::Paint();

	CTeamButton* pButton = nullptr;
	switch (m_iHighlightedButton)
	{
	case TEAM_CT:
		pButton = dynamic_cast<CTeamButton*>(m_pButtonCT);
		break;

	case TEAM_TERRORIST:
		pButton = dynamic_cast<CTeamButton*>(m_pButtonT);
		break;

	default:
		return;	// No additional drawing.
	}

	if (!pButton)
		return;

	// Purpose: Paint an additional frame outside the button.
	int x = 0, y = 0, w = 0, h = 0;
	pButton->GetPos(x, y);
	pButton->GetSize(w, h);
	x -= MARGIN_BETWEEN_FRAME_AND_BUTTON + WIDTH_FRAME;
	y -= MARGIN_BETWEEN_FRAME_AND_BUTTON + WIDTH_FRAME;

	DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, 0xFF);

	// Top-left
	DrawUtils::Draw2DQuadNoTex(x, y, x + LENGTH_FRAME, y + WIDTH_FRAME);
	DrawUtils::Draw2DQuadNoTex(x, y, x + WIDTH_FRAME, y + LENGTH_FRAME);

	// Top-right
	pButton->GetPos(x, y);
	x += w + MARGIN_BETWEEN_FRAME_AND_BUTTON + WIDTH_FRAME;
	y -= MARGIN_BETWEEN_FRAME_AND_BUTTON + WIDTH_FRAME;
	DrawUtils::Draw2DQuadNoTex(x, y, x - LENGTH_FRAME, y + WIDTH_FRAME);
	DrawUtils::Draw2DQuadNoTex(x, y, x - WIDTH_FRAME, y + LENGTH_FRAME);

	DrawUtils::glRegularPureColorDrawingExit();
}

void CTeamMenu::Show(bool bShow)
{
	if (BaseClass::IsVisible() == bShow)
		return;

	if (bShow)
	{
		Activate();

		if (GetParent()->IsVisible())
		{
			SetMouseInputEnabled(true);
			IN_DeactivateMouse();
		}
	}
	else
	{
		SetVisible(false);

		// Only do this when IClientVGUI is visible.
		// Or this line will break the pause screen.
		if (GetParent()->IsVisible())
		{
			SetMouseInputEnabled(false);
			IN_ActivateMouse();
		}
	}
}
