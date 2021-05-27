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

		if (m_iPlayerCounts <= 5)	//  Designed max player per team. But sometimes...
		{
			for (unsigned short i = 0; i < m_iPlayerCounts; i++)
			{
				DrawUtils::glRegularTexDrawingInit(0xFFFFFF, GetAlpha());
				DrawUtils::glSetTexture(CTeamMenu::m_sPlayerCountIcon.m_iId);
				DrawUtils::Draw2DQuad(vecPos, vecPos + m_vecPawnIconScaledSize);

				vecPos.x -= MARGIN_BETWEEN_PAWN_ICONS;
				vecPos.x -= m_vecPawnIconScaledSize.width;	// Moving left.
			}
		}
		else
		{
			// Draw a pawn icon first.
			DrawUtils::glRegularTexDrawingInit(0xFFFFFF, GetAlpha());
			DrawUtils::glSetTexture(CTeamMenu::m_sPlayerCountIcon.m_iId);
			DrawUtils::Draw2DQuad(vecPos, vecPos + m_vecPawnIconScaledSize);

			vecPos.x -= m_iRomanNumberWidth;

			// Display roman number indicator.
			gFontFuncs::DrawSetTextFont(_font);
			gFontFuncs::DrawSetTextColor(0x0, GetAlpha());
			gFontFuncs::DrawSetTextPos(vecPos);
			gFontFuncs::DrawPrintText(m_wcsRomanNumber.c_str());
		}
	}

	void OnThink(void) final
	{
		BaseClass::OnThink();

		auto iLastPlayerCounts = m_iPlayerCounts;
		m_iPlayerCounts = 0;
		for (int i = 0; i <= gEngfuncs.GetMaxClients(); i++)
		{
			if (g_PlayerInfoList[i].name == nullptr)	// existence of a player.
				continue;

			if (g_PlayerExtraInfo[i].m_iTeam != m_iTeamTracking)
				continue;

			m_iPlayerCounts++;
		}

		if (m_iPlayerCounts > 5 && iLastPlayerCounts != m_iPlayerCounts)	// Only updates text when needed.
		{
			m_wcsRomanNumber = UTIL_ArabicToRoman(m_iPlayerCounts) + L" × ";
			gFontFuncs::GetTextSize(_font, m_wcsRomanNumber.c_str(), &m_iRomanNumberWidth, nullptr);
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
	static constexpr auto MARGIN_BETWEEN_ROMAN_NUM_PAWN_ICON = MARGIN_BETWEEN_PAWN_ICONS;

private:
	unsigned short m_iPlayerCounts{ 0 };
	unsigned short m_iTeamTracking{ TEAM_UNASSIGNED };
	Vector2D m_vecExistingPlayerTrackerStartingPos{ Vector2D() };
	Vector2D m_vecPawnIconScaledSize{ Vector2D() };
	std::wstring m_wcsRomanNumber{ L"\0" };
	int m_iRomanNumberWidth{ 0 };
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
	SetBounds(MARGIN, MARGIN, iScreenWidth - MARGIN * 2, iScreenTall - MARGIN * 2);

	int iPanelWidth = 0, iPanelTall = 0;
	GetSize(iPanelWidth, iPanelTall);

	SetTitleBarVisible(false);
	SetProportional(true);

	SetVisible(false);

	if (!m_sPlayerCountIcon)
		m_sPlayerCountIcon.Load("sprites/Miscellaneous/SchemeNumber.dds");

	m_CTBackgrounds[0].Load("sprites/ClassesIcon/Portraits/CommanderMonochrome.dds");
	m_CTBackgrounds[1].Load("sprites/ClassesIcon/Portraits/CommanderColored.dds");
	m_TERBackgrounds[0].Load("sprites/ClassesIcon/Portraits/GodfatherMonochrome.dds");
	m_TERBackgrounds[1].Load("sprites/ClassesIcon/Portraits/GodfatherColored.dds");

#pragma region Team Buttons Setup
	Vector2D vecTButtonPos = Vector2D(iPanelWidth, iPanelTall) / 2 - Vector2D(MARGIN_BUTTON, BUTTON_SIZE) / 2 - Vector2D(BUTTON_SIZE, 0);
	Vector2D vecCTButtonPos = Vector2D(iPanelWidth, iPanelTall) / 2 + Vector2D(MARGIN_BUTTON, -BUTTON_SIZE) / 2;
	Vector2D vecSpecButtonPos = Vector2D(iPanelWidth - BUTTON_SIZE / 2 - 16, 16);

	m_pButtonT = new CTeamButton(this, "SelectT", "#LeaderMod_SB_Ter_Short", TEAM_TERRORIST, this, "jointeam 1\njoinclass 6\n");
	m_pButtonT->SetBounds(vecTButtonPos.x, vecTButtonPos.y, BUTTON_SIZE, BUTTON_SIZE + FONT_SIZE);
	m_pButtonT->SetVisible(true);
	m_pButtonT->SetUpImage("sprites/ClassesIcon/T/Godfather.dds");
	m_pButtonT->AddGlyphSetToFont("Trajan Pro", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
	m_pButtonT->AddGlyphSetToFont("I.MingCP", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
	m_pButtonT->InvalidateLayout(true);

	m_pButtonCT = new CTeamButton(this, "SelectCT", "#LeaderMod_SB_CT_Short", TEAM_CT, this, "jointeam 2\njoinclass 6\n");
	m_pButtonCT->SetBounds(vecCTButtonPos.x, vecCTButtonPos.y, BUTTON_SIZE, BUTTON_SIZE + FONT_SIZE);
	m_pButtonCT->SetVisible(true);
	m_pButtonCT->SetUpImage("sprites/ClassesIcon/CT/Commander.dds");
	m_pButtonCT->AddGlyphSetToFont("Trajan Pro", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
	m_pButtonCT->AddGlyphSetToFont("I.MingCP", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
	m_pButtonCT->InvalidateLayout(true);

	m_pButtonObserver = new CTeamButton(this, "SelectSpec", "#LeaderMod_SB_Spec_Short", TEAM_CT, this, "jointeam 6");
	m_pButtonObserver->SetBounds(vecSpecButtonPos.x, vecSpecButtonPos.y, BUTTON_SIZE / 2, BUTTON_SIZE / 2 + FONT_SIZE);
	m_pButtonObserver->SetVisible(true);
	m_pButtonObserver->SetUpImage("sprites/Inventory/NVG.dds");
	m_pButtonObserver->AddGlyphSetToFont("Trajan Pro", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
	m_pButtonObserver->AddGlyphSetToFont("I.MingCP", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
	m_pButtonObserver->InvalidateLayout(true);
#pragma endregion
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

	// Why I have to do this? Kind of buggy...
	if (IsVisible())
		SetMouseInputEnabled(true);

	m_flBackgroundAlphaGoals.fill(128);
	m_vecBackgroundColorGoals.fill(Vector(1, 1, 1));

	if (m_pButtonCT->IsPendingSelected())
	{
		m_iHighlightedButton = TEAM_CT;
		m_vecBackgroundColorGoals[TEAM_CT] = VEC_CT_COLOUR;
	}
	else if (m_pButtonT->IsPendingSelected())
	{
		m_iHighlightedButton = TEAM_TERRORIST;
		m_vecBackgroundColorGoals[TEAM_TERRORIST] = VEC_T_COLOUR;
	}
	else
		m_iHighlightedButton = TEAM_UNASSIGNED;

	m_flBackgroundAlphaGoals[m_iHighlightedButton] = 255;

	// Since the size of these arrays are the same, we can place them in a same loop.
	for (unsigned i = 0; i < m_flBackgroundAlphas.size(); i++)
	{
		m_flBackgroundAlphas[i] += (m_flBackgroundAlphaGoals[i] - m_flBackgroundAlphas[i]) * g_flClientTimeDelta * 10;
		m_vecBackgroundColors[i] += (m_vecBackgroundColorGoals[i] - m_vecBackgroundColors[i]) * g_flClientTimeDelta * 10.0;
	}
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

void CTeamMenu::PaintBackground(void)
{
	SetBgColor(Color(0, 0, 0, 96));

	BaseClass::PaintBackground();

	int iFrameTall = GetTall(), iFrameWide = GetWide();
	image_t* pTerTex = &m_TERBackgrounds[0];//&m_TERBackgrounds[m_iHighlightedButton == TEAM_TERRORIST ? 1 : 0];
	image_t* pCTTex = &m_CTBackgrounds[0];//&m_CTBackgrounds[m_iHighlightedButton == TEAM_CT ? 1 : 0];
	int iDrawTall = round(iFrameTall * 0.65);

	// T
	DrawUtils::glRegularTexDrawingInit(m_vecBackgroundColors[TEAM_TERRORIST], m_flBackgroundAlphas[TEAM_TERRORIST] / 255.0);
	DrawUtils::glSetTexture(pTerTex->m_iId);
	DrawUtils::Draw2DQuad(0, iFrameTall - iDrawTall, pTerTex->CalculateWidthByDefinedHeight(iDrawTall), iFrameTall);

	// CT
	DrawUtils::glSetColor(m_vecBackgroundColors[TEAM_CT], m_flBackgroundAlphas[TEAM_CT] / 255.0);
	DrawUtils::glSetTexture(pCTTex->m_iId);
	DrawUtils::Draw2DQuad(iFrameWide - pCTTex->CalculateWidthByDefinedHeight(iDrawTall), iFrameTall - iDrawTall, iFrameWide, iFrameTall);
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
