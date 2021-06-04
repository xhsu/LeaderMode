/*

Created Date: May 31 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#include "precompiled.h"

using namespace vgui;


std::array<std::wstring, _countof(g_rgpszMarketCategoryNames)> g_rgwcsLocalisedCtgyNames;
static int iDummy = 0;


//class CExhibitionPanel : public EditablePanel, public CViewportPanelHelper<CExhibitionPanel>
//{
//	DECLARE_CLASS_SIMPLE(CExhibitionPanel, EditablePanel);
//
//};

class CCategoryLable : public Button	// Inherit from vgui::Button instead of LMImageButton.
{
	DECLARE_CLASS_SIMPLE(CCategoryLable, Button);

public:
	CCategoryLable(Panel* parent, const char* text, int YposTracking, unsigned indexInArray) :
		BaseClass(parent, SharedVarArgs("CategoryLable%d", s_iInstanceCount++), text, parent),
		m_YposTracking(YposTracking),
		m_pParent(dynamic_cast<CMarket*>(parent)),
		m_iIndexInArray(indexInArray),
		m_bShowingSeparator(false)
	{
		static wchar_t string[128];
		GetText(string, sizeof(string));
		m_wstring = string;

		// Theoretically it is already initialized. But who knows...
		InitializeFont();

		// Automatically initialize XPos here.
		int iTall = 0, xPos = 0;
		gFontFuncs::GetTextSize(s_hFont, string, &m_iTextLength, &iTall);	// Actually the tall is just the font tall. With consideration of \n symbol.
		xPos = 0 + (m_pParent->GetWide() - m_pParent->m_iExhibitionTableWidth - m_iTextLength) / 2;	// Centered.

		// Move upward a bit in favor of text printing.
		m_YposTracking -= iTall + CMarket::MARGIN_BETWEEN_BUTTONS;

		SetBounds(xPos, m_YposTracking, m_iTextLength, iTall);

		SetVisible(true);
		SetEnabled(true);
	}

	void OnThink(void) final
	{
		// Drift button according to scroll status.
		int x = 0, y = 0;
		GetPos(x, y);	// Shouldn't use dummmy here. It would be modify later.

		int /*ex = 0, */ey = m_YposTracking;
		//m_pParent->m_pPurchasablePanel->GetPos(ex, ey);
		m_pParent->m_pPurchasablePanel->LocalToScreen(/*ex*/iDummy, ey);	// Remember, this function would modify its input. DO NOT directly put m_YposTracking in.
		m_pParent->ScreenToLocal(iDummy, ey);	// Recentered to parent's origin.

		// Clamp Y.
		int yMin = m_iIndexInArray * (FONT_SIZE + HEIGHT_SEPARATOR);
		int yMax = m_pParent->GetTall() - (m_pParent->m_rgpCategoryButtons.size() - m_iIndexInArray) * (FONT_SIZE + HEIGHT_SEPARATOR);

		SetPos(x, std::clamp(ey, yMin, yMax));

		// Calculate alpha.
		float flAlpha = 255;

		if (ey < yMin)	// The desired line is above.
		{
			flAlpha = std::clamp<float>(1.0f - float(yMin - ey) / (float)m_pParent->GetTall(), 0, 1) * 255.0;

			if (m_bShowingSeparator)
			{
				GetAnimationController()->RunAnimationCommand(this, "m_iSeparatorAlpha", 0, 0.0f, 0.5, AnimationController::INTERPOLATOR_DEACCEL);
				m_bShowingSeparator = false;
			}
		}
		else if (ey > yMax)
		{
			flAlpha = std::clamp<float>(1.0f - float(ey - yMax) / (float)m_pParent->GetTall(), 0, 1) * 255.0;

			if (m_bShowingSeparator)
			{
				GetAnimationController()->RunAnimationCommand(this, "m_iSeparatorAlpha", 0, 0.0f, 0.5, AnimationController::INTERPOLATOR_DEACCEL);
				m_bShowingSeparator = false;
			}
		}
		else
		{
			if (!m_bShowingSeparator)
			{
				GetAnimationController()->RunAnimationCommand(this, "m_iSeparatorAlpha", 255 /*flAlpha must be 255 here.*/, 0.0f, 0.5, AnimationController::INTERPOLATOR_DEACCEL);
				m_bShowingSeparator = true;
			}
		}

		SetAlpha(flAlpha * m_pParent->m_flAlpha);	// Don't forget the global alpha factor!

		// Hovering effect.
		if (m_bShowingShortSprtr && !IsArmed())
		{
			GetAnimationController()->RunAnimationCommand(this, "m_iShortSeparatorLength", 0, 0, 0.2, AnimationController::INTERPOLATOR_DEACCEL);
			m_bShowingShortSprtr = false;
		}
		else if (!m_bShowingShortSprtr && IsArmed())
		{
			GetAnimationController()->RunAnimationCommand(this, "m_iShortSeparatorLength", m_iTextLength, 0, 0.2, AnimationController::INTERPOLATOR_DEACCEL);
			m_bShowingShortSprtr = true;
		}
	}

	void PaintBackground(void) final { /* Draw no background*/ }

	void Paint(void) override
	{
		if (!ShouldPaint())
			return;

		if (m_wstring.empty())
			return;

		auto flTextAlpha = GetAlpha();

		if (m_iShortSeparatorLength >= 1)	// At least 1 pixel.
		{
			DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, m_pParent->GetAlpha());	// Have to highlight.
			DrawUtils::Draw2DQuadNoTex(0, FONT_SIZE, m_iShortSeparatorLength, FONT_SIZE + HEIGHT_SEPARATOR);
			DrawUtils::glRegularPureColorDrawingExit();

			// Highlight myself if mouse is hovering around.
			auto flDelta = 255.0f - flTextAlpha;
			flTextAlpha = std::clamp<float>(flTextAlpha + m_iShortSeparatorLength / (float)m_iTextLength * flDelta, 0, 255);
		}

		gFontFuncs::DrawSetTextFont(s_hFont);
		gFontFuncs::DrawSetTextPos(0, 0);	// Drawing always centered at (0, 0)
		gFontFuncs::DrawSetTextColor(0xFFFFFF, flTextAlpha * m_pParent->m_flAlpha);
		gFontFuncs::DrawPrintText(m_wstring.c_str());

		if (m_iSeparatorAlpha > FLT_EPSILON)	// Can't use the m_bShowingSeparator to determind. It would cause sudden disappear of separator.
		{
			// Don't know why, but this have to kept update. Can't place it in Constructor.
			// Not work even in ApplySchemeSettings().
			m_iSeparatorLength = m_pParent->GetWide() - ScrollableEditablePanel::WIDTH_SCROLLBAR;	// Don't forget the scroll bar!
			m_pParent->LocalToScreen(m_iSeparatorLength, iDummy);
			this->ScreenToLocal(m_iSeparatorLength, iDummy);

			DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, m_iSeparatorAlpha * m_pParent->m_flAlpha);
			DrawUtils::Draw2DQuadNoTex(0, FONT_SIZE, m_iSeparatorLength, FONT_SIZE + HEIGHT_SEPARATOR);
			DrawUtils::glRegularPureColorDrawingExit();
		}
	}

	void PaintBorder(void) final { /* Draw no border*/ }

	void ApplySchemeSettings(IScheme* pScheme) final
	{
		BaseClass::ApplySchemeSettings(pScheme);

		// Unset button border
		SetDefaultBorder(nullptr);
		SetDepressedBorder(nullptr);
		SetKeyFocusBorder(nullptr);
	}

	void DoClick(void) final	// TODO: change to void OnMousePressed(MouseCode code)?
	{
		BaseClass::DoClick();

		// Scroll to the designated location.
		//m_pParent->m_pScrollablePanel->m_pScrollBar->SetValue(m_YposTracking);
		int iPos = 0;

		if (m_iIndexInArray != 0)
		{
			// Why?
			// Because the separator appears along with the last row of this category.
			// What if this category has multiple rows?
			// Hence the fast-est way to do it is to locate it by the previous lable.
			auto pLableButton = dynamic_cast<CCategoryLable*>(m_pParent->m_rgpCategoryButtons[m_iIndexInArray - 1U]);
			iPos = pLableButton->m_YposTracking + FONT_SIZE;
		}
		else
			iPos = 0;	// The top.

		auto iRangeWindow = m_pParent->m_pScrollablePanel->m_pScrollBar->GetRangeWindow();	// Range window: after set the the pos, the windows range below is actual visible.
		int iMax = 0; m_pParent->m_pScrollablePanel->m_pScrollBar->GetRange(iDummy, iMax);	// Hence the real valid scroll bar pos is smaller then its total range[a, b]

		iPos = std::clamp(iPos, 0, Q_max(0, iMax - iRangeWindow));	// Although the silder will clamp by itself, but the scroll animation will waste time on the calculation of these invalid numbers.

		GetAnimationController()->RunAnimationCommand(m_pParent, "m_iScrollPanelPos", iPos, 0, 0.25, AnimationController::INTERPOLATOR_DEACCEL);
	}

	static inline int InitializeFont()
	{
		if (!s_hFont)
		{
			s_hFont = gFontFuncs::CreateFont();
			gFontFuncs::AddGlyphSetToFont(s_hFont, "Trajan Pro", FONT_SIZE, FW_BOLD, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
			gFontFuncs::AddGlyphSetToFont(s_hFont, "I.MingCP", FONT_SIZE, FW_BOLD, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
		}

		return s_hFont;
	}


public:
//	static constexpr auto SIZE_HEIGHT = 36;
	static constexpr auto FONT_SIZE = 32;
	static constexpr auto HEIGHT_SEPARATOR = 2;
	static inline int s_hFont = 0;
	static inline int s_iInstanceCount = 0;

private:
	int m_YposTracking{ 0U };
	unsigned m_iIndexInArray{ 0U };
	CMarket* m_pParent{ nullptr };
	std::wstring m_wstring;
	int m_iTextLength{ 0U };
	bool m_bShowingSeparator : 1;
	CPanelAnimationVar(float, m_iSeparatorAlpha, "m_iSeparatorAlpha", "0");
	bool m_bShowingShortSprtr : 1;
	CPanelAnimationVar(float, m_iShortSeparatorLength, "m_iShortSeparatorLength", "0");	// This one is for hover effect.
	int m_iSeparatorLength{ 0 };
};

class CBuyMenuButton : public LMImageButton
{
	DECLARE_CLASS_SIMPLE(CBuyMenuButton, LMImageButton);

	using BaseClass::BaseClass;

public:
	void OnThink(void) final
	{
		BaseClass::OnThink();

		if (!m_pGrandpa)
			m_pGrandpa = dynamic_cast<ScrollableEditablePanel*>(GetParent()->GetParent());	// CBuyMenuButton => m_pPurchasablePanel => m_pScrollablePanel

		if (!m_pMarket)
			m_pMarket = dynamic_cast<CMarket*>(m_pGrandpa->GetParent());	// CBuyMenuButton => m_pPurchasablePanel => m_pScrollablePanel => CMarket

		// Calculate alpha.
		float flAlpha = 255;

		int x = 0, y = 0;
		GetPos(x, y);
		GetParent()->LocalToScreen(x, y);
		m_pGrandpa->ScreenToLocal(x, y);

		auto flTall = (float)GetTall();
		auto yMax = (float)m_pGrandpa->GetTall() - flTall;

		if (y < 0)
			flAlpha = std::clamp<float>(1.0f - float(0 - y) / flTall, 0, 1) * 255.0;
		else if (y > yMax)
			flAlpha = std::clamp<float>(1.0f - float(y - yMax) / flTall, 0, 1) * 255.0;

		SetAlpha(flAlpha * m_pMarket->m_flAlpha);	// Don't forget the global alpha factor!
	}

	/*
	* What to do here?
	* 1. Make buttons calculate alpha by it position.
	* 2. Store its item index. Hide itself for wrong role.
	*	a. Maybe I can create a separator at the beginning of next category instead of the end of current category?
	*/

private:
	ScrollableEditablePanel* m_pGrandpa{ nullptr };
	CMarket* m_pMarket{ nullptr };
};


CMarket::CMarket() : BaseClass(nullptr, "Market")
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

	if (!s_hFont)
	{
		s_hFont = gFontFuncs::CreateFont();
		gFontFuncs::AddGlyphSetToFont(s_hFont, "Trajan Pro", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
		gFontFuncs::AddGlyphSetToFont(s_hFont, "I.MingCP", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
	}

	// Localize text first.
	for (unsigned i = 0; i < _countof(g_rgpszMarketCategoryNames); i++)
		g_rgwcsLocalisedCtgyNames[i] = UTIL_GetLocalisation(g_rgpszMarketCategoryNames[i]);

	// Find maxium text length in order to determind how many buttons can we place on one row.
	m_iTextMaxiumWidth = 0;
	auto hLableFont = CCategoryLable::InitializeFont();

	for (auto& wcs : g_rgwcsLocalisedCtgyNames)
	{
		int iCurLength = 0;
		gFontFuncs::GetTextSize(hLableFont, wcs.c_str(), &iCurLength, nullptr);	// We should use lable class font.

		if (iCurLength > m_iTextMaxiumWidth)
			m_iTextMaxiumWidth = iCurLength;
	}

	m_iExhibitionTableWidth = iPanelWidth - m_iTextMaxiumWidth - MARGIN_BETWEEN_BUTTON_AND_TEXT - ScrollableEditablePanel::WIDTH_SCROLLBAR;	// Remember to spare some space for CScrollBar.

	// Exhibition panel
	m_pPurchasablePanel = new EditablePanel(this, "ExihibitionPanel");
	m_pPurchasablePanel->SetBounds(0, 0, m_iExhibitionTableWidth, iPanelTall);

	m_rgpButtons.fill(nullptr);
	m_rgpCategoryButtons.reserve(16);	// Just... a guess...

	int x = 0, y = WIDTH_FRAME + MARGIN_BETWEEN_FRAME_AND_BUTTON, iHorizontalShift = 0;	// Spare some space for hover effect.
	unsigned iCurCtgyLable = 0, iItemAddedInCurCtgy = 0;
	for (unsigned i = 0; i < LAST_WEAPON; i++)
	{
		if (!Q_strlen(g_rgpszWeaponSprites[i]) || g_rgRoleWeaponsAccessibility[g_iRoleType][i] == WPN_F)
		{
			// Even though the sprite does not exist yet, you still have to skip the line.
			// Only if there were no item added in this category.
			if ((1 << i) & g_bitsLastWeaponInCategory && iItemAddedInCurCtgy)
			{
				x = 0;
				y += WPN_SPRITE_HEIGHT + FONT_SIZE + MARGIN_BETWEEN_BUTTONS;
			}

			goto LAB_ADD_LABLE_BUTTON;
		}

		m_rgpButtons[i] = new CBuyMenuButton(
			m_pPurchasablePanel,
			g_rgWpnInfo[i].m_pszInternalName,
			g_rgWpnInfo[i].m_pszExternalName,
			this,
			SharedVarArgs("buyweapon %s", g_rgWpnInfo[i].m_pszInternalName)
		);

		m_rgpButtons[i]->SetVisible(true);
		m_rgpButtons[i]->SetUpImage(g_rgpszWeaponSprites[i]);
		m_rgpButtons[i]->SetFont(s_hFont);
		m_rgpButtons[i]->SetPinCorner(PIN_TOPLEFT, x, y);	// In scrollable page, you cannot use regular SetPos().
		m_rgpButtons[i]->SetSizeByImageHeight(WPN_SPRITE_HEIGHT);
		m_rgpButtons[i]->InvalidateLayout(true);

		iHorizontalShift = (m_rgpButtons[i]->GetWide() + MARGIN_BETWEEN_BUTTONS);
		iItemAddedInCurCtgy++;

		// Have to use negative since negative means left shift.
		if ((x + iHorizontalShift) > m_iExhibitionTableWidth || ((1 << i) & g_bitsLastWeaponInCategory))
		{
			x = 0;
			y += WPN_SPRITE_HEIGHT + FONT_SIZE + MARGIN_BETWEEN_BUTTONS;
		}
		else
		{
			x += iHorizontalShift;
		}

	LAB_ADD_LABLE_BUTTON:;
		if ((1 << i) & g_bitsLastWeaponInCategory)
		{
			if (iItemAddedInCurCtgy)	// Skip the lable if none of its item is enlisted.
			{
				m_rgpCategoryButtons.push_back(new CCategoryLable(
					this,
					g_rgpszMarketCategoryNames[iCurCtgyLable],
					y,
					m_rgpCategoryButtons.size()
				));
			}

			iCurCtgyLable++;
			iItemAddedInCurCtgy = 0;	// Re-zero after each category lable is added.
		}
	}

	x = 0;
	iItemAddedInCurCtgy = 0;
	bool bShouldAddCategory = false;

	for (unsigned i = LAST_WEAPON + 1, j = 1; i < LAST_WEAPON + EQP_COUNT; i++, j++)
	{
		bShouldAddCategory = j == EQP_ASSAULT_SUIT || j == EQP_C4 || j == EQP_FLASHLIGHT;

		if (g_rgRoleEquipmentsAccessibility[g_iRoleType][j] == WPN_F)
		{
			// Even though the sprite does not exist yet, you still have to skip the line.
			// Only if there were no item added in this category.
			if (bShouldAddCategory && iItemAddedInCurCtgy)
			{
				x = 0;
				y += WPN_SPRITE_HEIGHT + FONT_SIZE + MARGIN_BETWEEN_BUTTONS;
			}

			goto LAB_ADD_LABLE_BUTTON2;
		}

		m_rgpButtons[i] = new CBuyMenuButton(
			m_pPurchasablePanel,
			g_rgEquipmentInfo[j].m_pszInternalName,
			g_rgEquipmentInfo[j].m_pszExternalName,
			this,
			SharedVarArgs("buyeqp %s", g_rgEquipmentInfo[j].m_pszInternalName)
		);

		m_rgpButtons[i]->SetVisible(true);
		m_rgpButtons[i]->SetUpImage(g_rgpszEquipmentSprites[j]);
		m_rgpButtons[i]->SetFont(s_hFont);
		m_rgpButtons[i]->SetPinCorner(PIN_TOPLEFT, x, y);	// In scrollable page, you cannot use regular SetPos().
		m_rgpButtons[i]->SetSizeByImageHeight(WPN_SPRITE_HEIGHT);
		m_rgpButtons[i]->InvalidateLayout(true);

		iHorizontalShift = (m_rgpButtons[i]->GetWide() + MARGIN_BETWEEN_BUTTONS);
		iItemAddedInCurCtgy++;

		// Have to use negative since negative means left shift.
		if ((x + iHorizontalShift) > m_iExhibitionTableWidth || bShouldAddCategory)
		{
			x = 0;
			y += WPN_SPRITE_HEIGHT + FONT_SIZE + MARGIN_BETWEEN_BUTTONS;
		}
		else
		{
			x += iHorizontalShift;
		}

	LAB_ADD_LABLE_BUTTON2:;
		if (bShouldAddCategory)
		{
			if (iItemAddedInCurCtgy)
			{
				m_rgpCategoryButtons.push_back(new CCategoryLable(
					this,
					g_rgpszMarketCategoryNames[iCurCtgyLable],
					y,
					m_rgpCategoryButtons.size()
				));
			}

			iCurCtgyLable++;
			iItemAddedInCurCtgy = 0;
		}
	}

	m_pPurchasablePanel->SetTall(y);	// Update tall for the elements we added.

	m_pScrollablePanel = new ScrollableEditablePanel(this, m_pPurchasablePanel, "ScrollablePanel");
	m_pScrollablePanel->SetBounds(iPanelWidth - m_iExhibitionTableWidth, 0, m_iExhibitionTableWidth, iPanelTall);
	m_pScrollablePanel->m_pScrollBar->SetVisible(false);
}

void CMarket::Paint(void)
{
	BaseClass::Paint();

	for (auto& pButton : m_rgpButtons)
	{
		if (!pButton)
			continue;

		if (pButton->IsPendingSelected())
			DrawMouseHoveredEffects(pButton);
	}
}

void CMarket::OnThink(void)
{
	BaseClass::OnThink();

	// Why I have to do this? Kind of buggy...
	//if (IsVisible() && m_flTimeShouldTurnInvisible <= 0)
	//	SetMouseInputEnabled(true);

	// Out of buyzone? No more.
	if (IsVisible() && !CHudStatusIcons::IconExists("buy"))
		Show(false);

	if (m_flTimeShouldTurnInvisible > 0 && m_flTimeShouldTurnInvisible < g_flClientTime)
	{
		BaseClass::SetVisible(false);	// Bypass the override one.
		m_flTimeShouldTurnInvisible = -1;
	}

	m_pScrollablePanel->m_pScrollBar->SetValue(m_iScrollPanelPos);
	SetAlpha(m_flAlpha * 255.0f);

	SetBgColor(Color(0, 0, 0, m_flAlpha * ALPHA_BACKGROUND));
}

void CMarket::OnCommand(const char* szCommand)
{
	Show(false);

	if (Q_strstr(szCommand, "buyweapon"))
		gEngfuncs.pfnClientCmd(szCommand);
}

void CMarket::Show(bool bShow)
{
	if (IsVisible() == bShow)
		return;

	if (bShow)
	{
		Activate();
		GetAnimationController()->RunAnimationCommand(this, "m_flAlpha", 1, 0, TIME_FADING, AnimationController::INTERPOLATOR_DEACCEL);

		if (GetParent()->IsVisible())
		{
			SetMouseInputEnabled(true);
			IN_DeactivateMouse();
		}
	}
	else
	{
		// Fade out.
		m_flTimeShouldTurnInvisible = g_flClientTime + TIME_FADING;
		GetAnimationController()->RunAnimationCommand(this, "m_flAlpha", 0, 0, TIME_FADING, AnimationController::INTERPOLATOR_DEACCEL);

		// Only do this when IClientVGUI is visible.
		// Or this line will break the pause screen.
		if (GetParent()->IsVisible())
		{
			SetMouseInputEnabled(false);
			IN_ActivateMouse();
		}
	}
}

void CMarket::OnKeyCodeTyped(KeyCode code)
{
	BaseClass::OnKeyCodeTyped(code);
	Show(false);	// Hide on any typing.
}

void CMarket::UpdateMarket(void)
{
	// Purge the original lable array.
	for (auto& p : m_rgpCategoryButtons)
	{
		//p->MarkForDeletion();
		p->DeletePanel();
	}

	m_rgpCategoryButtons.clear();

	for (auto& p : m_rgpButtons)
	{
		if (!p)
			continue;

		p->SetVisible(false);
		p->SetPinCorner(PIN_TOPLEFT, 0, 0);
	}

	// Find the "fallbacked" last item in category.
	constexpr std::array<std::array<BYTE, 2>, 9> rgiWeapon =
	{ {	// Fuck C++14, we needs double brackets now.
		{FIRST_PISTOL, LAST_PISTOL},
		{FIRST_SHOTGUN, LAST_SHOTGUN},
		{FIRST_SMG, LAST_SMG},
		{FIRST_AR, LAST_AR},
		{FIRST_SR, LAST_SR},
		{FIRST_LMG, LAST_LMG},

		{FIRST_ARMOUR, LAST_ARMOUR},
		{FIRST_GR, LAST_GR},
		{FIRST_MISC, LAST_MISC},
	} };

	std::array<int, 9> rgiFallbackedLastItem{};
	rgiFallbackedLastItem.fill(-1);

	for (unsigned i = 0; i < rgiWeapon.size(); i++)
	{
		if (i < 6)
		{
			for (unsigned j = rgiWeapon[i][1]; j >= rgiWeapon[i][0]; j--)
			{
				if (m_rgpButtons[j] && g_rgRoleWeaponsAccessibility[g_iRoleType][j] != WPN_F)
				{
					rgiFallbackedLastItem[i] = j;
					break;
				}
			}
		}

		// Equipments need special treatment.
		else
		{
			for (unsigned j = rgiWeapon[i][1]; j >= rgiWeapon[i][0]; j--)
			{
				if (m_rgpButtons[j + LAST_WEAPON] && g_rgRoleEquipmentsAccessibility[g_iRoleType][j] != WPN_F)
				{
					// Kept the offseted status.
					rgiFallbackedLastItem[i] = j + LAST_WEAPON;
					break;
				}
			}
		}
	}

	// Weapons loop.
	int x = 0, y = WIDTH_FRAME + MARGIN_BETWEEN_FRAME_AND_BUTTON, iHorizontalShift = 0;	// Spare some space for hover effect.
	for (unsigned i = 0; i < LAST_WEAPON; i++)
	{
		auto pButton = m_rgpButtons[i];//dynamic_cast<CBuyMenuButton*>(m_rgpButtons[i]);
		auto iter = std::find(rgiFallbackedLastItem.begin(), rgiFallbackedLastItem.end(), i);
		auto bIsLastItem = iter != rgiFallbackedLastItem.end();

		if (!pButton || g_rgRoleWeaponsAccessibility[g_iRoleType][i] == WPN_F)
		{
			// Even though the sprite does not exist yet, you still have to skip the line.
			if (bIsLastItem)
			{
				x = 0;
				y += WPN_SPRITE_HEIGHT + FONT_SIZE + MARGIN_BETWEEN_BUTTONS;
			}

			if (pButton)
				pButton->SetVisible(false);	// Hide unavailable weapons.

			goto LAB_ADD_LABLE_BUTTON2;
		}

		pButton->SetVisible(true);
		pButton->SetPinCorner(PIN_TOPLEFT, x, y);	// In scrollable page, you cannot use regular SetPos().
		pButton->SetPos(x, y);

		iHorizontalShift = (pButton->GetWide() + MARGIN_BETWEEN_BUTTONS);

		// Have to use negative since negative means left shift.
		if ((x + iHorizontalShift) > m_iExhibitionTableWidth || bIsLastItem)
		{
			x = 0;
			y += WPN_SPRITE_HEIGHT + FONT_SIZE + MARGIN_BETWEEN_BUTTONS;
		}
		else
		{
			x += iHorizontalShift;
		}

	LAB_ADD_LABLE_BUTTON2:;
		if (bIsLastItem)
		{
			const auto index = iter - rgiFallbackedLastItem.begin();	// From iterator to index.

			m_rgpCategoryButtons.push_back(new CCategoryLable(
				this,
				g_rgpszMarketCategoryNames[index],
				y,
				m_rgpCategoryButtons.size()
			));
		}
	}

	// Equipments loop.
	x = 0;
	for (unsigned i = LAST_WEAPON + 1, j = 1; i < m_rgpButtons.size(); i++, j++)
	{
		auto pButton = m_rgpButtons[i];//dynamic_cast<CBuyMenuButton*>(m_rgpButtons[i]);
		auto iter = std::find(rgiFallbackedLastItem.begin(), rgiFallbackedLastItem.end(), i);
		auto bIsLastItem = iter != rgiFallbackedLastItem.end();

		if (!pButton || g_rgRoleEquipmentsAccessibility[g_iRoleType][j] == WPN_F)
		{
			// Even though the sprite does not exist yet, you still have to skip the line.
			if (bIsLastItem)
			{
				x = 0;
				y += WPN_SPRITE_HEIGHT + FONT_SIZE + MARGIN_BETWEEN_BUTTONS;
			}

			if (pButton)
				pButton->SetVisible(false);	// Hide unavailable weapons.

			goto LAB_ADD_LABLE_BUTTON3;
		}

		pButton->SetVisible(true);
		pButton->SetPinCorner(PIN_TOPLEFT, x, y);	// In scrollable page, you cannot use regular SetPos().
		pButton->SetPos(x, y);

		iHorizontalShift = (m_rgpButtons[i]->GetWide() + MARGIN_BETWEEN_BUTTONS);

		// Have to use negative since negative means left shift.
		if ((x + iHorizontalShift) > m_iExhibitionTableWidth || bIsLastItem)
		{
			x = 0;
			y += WPN_SPRITE_HEIGHT + FONT_SIZE + MARGIN_BETWEEN_BUTTONS;
		}
		else
		{
			x += iHorizontalShift;
		}

	LAB_ADD_LABLE_BUTTON3:;
		if (bIsLastItem)
		{
			const auto index = iter - rgiFallbackedLastItem.begin();	// From iterator to index.

			m_rgpCategoryButtons.push_back(new CCategoryLable(
				this,
				g_rgpszMarketCategoryNames[index],
				y,
				m_rgpCategoryButtons.size()
			));
		}
	}

	m_pPurchasablePanel->SetTall(y);	// Update tall for the elements we added.

	InvalidateLayout(true);
}
