/*

Created Date: May 26 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#include "precompiled.h"

using namespace vgui;

static constexpr const char* s_rgpszRoleButtonInternalNames[ROLE_COUNT] =
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

static constexpr const char* s_rgpszRoleLocalisationKeys[ROLE_COUNT] =
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

static constexpr const char* s_rgpszRoleIntroLocalisationKeys[ROLE_COUNT] =
{
	"#LeaderMod_Role_UNASSIGNED_Intro",

	"#LeaderMod_Role_Commander_Intro",
	"#LeaderMod_Role_SWAT_Intro",
	"#LeaderMod_Role_Breacher_Intro",
	"#LeaderMod_Role_Sharpshooter_Intro",
	"#LeaderMod_Role_Medic_Intro",

	"#LeaderMod_Role_Godfather_Intro",
	"#LeaderMod_Role_LeadEnforcer_Intro",
	"#LeaderMod_Role_MadScientist_Intro",
	"#LeaderMod_Role_Assassin_Intro",
	"#LeaderMod_Role_Arsonist_Intro",
};

static constexpr const char* s_rgpszRolePortraitFile[ROLE_COUNT] =
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

class CRoleButton : public LMImageButton
{
	DECLARE_CLASS_SIMPLE(CRoleButton, LMImageButton);

public:
	CRoleButton(Panel* parent, const char* panelName, const char* text, RoleTypes iRoleTracking, Panel* pActionSignalTarget = nullptr, const char* pCmd = nullptr) : BaseClass(parent, panelName, text, pActionSignalTarget, pCmd), m_iRoleTracking(iRoleTracking) { Q_wcslcpy(m_wcsText, _string.c_str()); _string = L"\0"; UpdateRoleIntroText(); }
	CRoleButton(Panel* parent, const char* panelName, const wchar_t* text, RoleTypes iRoleTracking, Panel* pActionSignalTarget = nullptr, const char* pCmd = nullptr) : BaseClass(parent, panelName, text, pActionSignalTarget, pCmd), m_iRoleTracking(iRoleTracking) { Q_wcslcpy(m_wcsText, _string.c_str()); _string = L"\0"; UpdateRoleIntroText(); }

	void OnThink(void) final
	{
		BaseClass::OnThink();

		SetAlpha(m_flAlpha);	// Sync constantly. This var is handled by vgui::AnimationController.

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
				// Update info only if it is not previously turned off.
				if (IsEnabled())
				{
					SetEnabled(false);
					GetAnimationController()->RunAnimationCommand(this, "m_flAlpha", 128, 0.0f, 0.5, AnimationController::INTERPOLATOR_DEACCEL);

					auto msg = VGUI_LOCALISE->Find("#LeaderMod_RoleSel_CastBy");
					if (msg)
						VGUI_LOCALISE->ConstructString(m_wcsText, sizeof(m_wcsText), msg, 1, UTF8ToUnicode(g_PlayerInfoList[i].name));	// Have to be unicode to be able to construct string.
				}
				
				return;
			}
		}

		// No found: avaliable to choose.
		if (!IsEnabled())
		{
			GetAnimationController()->RunAnimationCommand(this, "m_flAlpha", 255, 0.0f, 0.5, AnimationController::INTERPOLATOR_DEACCEL);
			SetEnabled(true);

			// Restoration.
			Q_wcslcpy(m_wcsText, UTIL_GetLocalisation(s_rgpszRoleLocalisationKeys[m_iRoleTracking]));
		}
	}

	void Paint(void) final
	{
		BaseClass::Paint();

		if (!ShouldPaint())
			return;

		auto iWidth = GetImageWidth(), iHeight = (int)_upImage.CalculateHeightByDefinedWidth(iWidth);
		auto iButtonTall = GetTall(), iButtonWide = GetWide();
		auto iAlpha = GetAlpha();

		auto vPos = Vector2D(iWidth, iHeight);
		DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, iAlpha);

		// Second part of white block.
		DrawUtils::Draw2DQuadNoTex(vPos, Vector2D(iButtonWide, iButtonTall));

		// Black board at the top-right.
		vPos = Vector2D(iWidth, 0);
		DrawUtils::glSetColor(0x0, iAlpha / 2);
		DrawUtils::Draw2DQuadNoTex(vPos, Vector2D(iButtonWide, iButtonTall - m_flSparedBlankHeight));

		DrawUtils::glRegularPureColorDrawingExit();

		// Draw text. This is the reason why the original _string was abolished.
		gFontFuncs::DrawSetTextFont(_font);
		gFontFuncs::DrawSetTextColor(0x0, iAlpha);
		gFontFuncs::DrawSetTextPos(BaseClass::MARGIN_TEXT, iHeight);
		gFontFuncs::DrawPrintText(m_wcsText);

		// Draw introduction text.
		gFontFuncs::DrawSetTextFont(m_iIntroFont);
		gFontFuncs::DrawSetTextColor(0xFFFFFF, iAlpha);
		gFontFuncs::DrawSetTextPos(iWidth + BaseClass::MARGIN_TEXT, 0);
		gFontFuncs::DrawPrintText(m_wcsIntroText.c_str());
	}

	int GetImageWidth(void) final
	{
		return CRoleMenu::BUTTON_SIZE;
	}

	inline void UpdateRoleIntroText(void)
	{
		if (!m_iIntroFont)
		{
			m_iIntroFont = gFontFuncs::CreateFont();
			gFontFuncs::AddGlyphSetToFont(m_iIntroFont, "Cambria", FONT_INTRO_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
			gFontFuncs::AddGlyphSetToFont(m_iIntroFont, "TW-Kai", FONT_INTRO_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
		}

		m_wcsIntroText = UTIL_GetLocalisation(s_rgpszRoleIntroLocalisationKeys[m_iRoleTracking]);

		// We need to clamp text.
		// However, different language have to treat differently.
		if (Q_stristr(g_szLanguage, "chinese") ||
			!Q_stricmp(g_szLanguage, "korean") ||
			!Q_stricmp(g_szLanguage, "japanese"))
		{
			gFontFuncs::ClampTextWidthCJK(m_wcsIntroText, m_iIntroFont, CRoleMenu::INTRO_REGION_WIDTH - BaseClass::MARGIN_TEXT);
		}
		else
		{
			gFontFuncs::ClampTextWidthROW(m_wcsIntroText, m_iIntroFont, CRoleMenu::INTRO_REGION_WIDTH - BaseClass::MARGIN_TEXT);
		}
	}

public:
	static constexpr auto FONT_INTRO_SIZE = 24;

private:
	CPanelAnimationVar(float, m_flAlpha, "m_flAlpha", "255");
	RoleTypes m_iRoleTracking{ Role_UNASSIGNED };
	wchar_t m_wcsText[256]{ L"\0" };	// Why abolish origin _string from base class? We have to draw this after second part of white block.
	int m_iIntroFont{ 0 };
	std::wstring m_wcsIntroText{ L"\0" };
};

CRoleMenu::CRoleMenu(void) : BaseClass(nullptr, "RoleMenu")
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
	SetKeyBoardInputEnabled(false);

	std::array<Vector2D, ROLE_COUNT> rgvPos;
	constexpr auto MARGIN_PERCENTAGE_MIDDLE_PORTRAITS_H = 1.0 / 3.0;
	constexpr auto MARGIN_PERCENTAGE_MIDDLE_PORTRAITS_V = 1.0 / 5.0;
	int iMarginBetweenMiddlePortraitsH = Q_max<double>(round((iPanelWidth * MARGIN_PERCENTAGE_MIDDLE_PORTRAITS_H) / 3.0), 36);	// 3 interspaces
	int iMarginBetweenMiddlePortraitsV = Q_max<double>(round((iPanelTall * MARGIN_PERCENTAGE_MIDDLE_PORTRAITS_V) / 2.0), 36);	// only 2 interspaces this time.

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
		CLASS_PORTRAITS[i].Load(s_rgpszRolePortraitFile[i]);

		m_rgpButtons[i] = new CRoleButton(this, s_rgpszRoleButtonInternalNames[i], s_rgpszRoleLocalisationKeys[i], (RoleTypes)i, this);
		m_rgpButtons[i]->SetCommand("role %d", i);
		m_rgpButtons[i]->SetBounds(rgvPos[i].x, rgvPos[i].y, BUTTON_SIZE + INTRO_REGION_WIDTH, BUTTON_SIZE + FONT_SIZE);
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
	// Why I have to do this? Kind of buggy...
	if (IsVisible())
		SetMouseInputEnabled(true);

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
	//Show(false);

	if (Q_strstr(command, "role"))
		gEngfuncs.pfnServerCmd(command);
	else
		BaseClass::OnCommand(command);
}

void CRoleMenu::OnKeyCodeTyped(KeyCode code)
{
	BaseClass::OnKeyCodeTyped(code);
	Show(false);	// Hide on any typing.
}

void CRoleMenu::PaintBackground(void)
{
	SetBgColor(Color(0, 0, 0, 96));

	BaseClass::PaintBackground();
}
