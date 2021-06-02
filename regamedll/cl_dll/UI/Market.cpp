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
	CCategoryLable(Panel* parent, const char* panelName, const char* text, int YposTracking, unsigned indexInArray) :
		BaseClass(parent, panelName, text, parent),
		m_YposTracking(YposTracking),
		m_pParent(dynamic_cast<CMarket*>(parent)),
		m_iIndexInArray(indexInArray),
		m_bShowingSeparator(false)
	{
		static wchar_t string[128];
		GetText(string, sizeof(string));
		m_wstring = string;

		if (!s_hFont)
		{
			s_hFont = gFontFuncs::CreateFont();
			gFontFuncs::AddGlyphSetToFont(s_hFont, "Trajan Pro", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x0, 0x2E7F);
			gFontFuncs::AddGlyphSetToFont(s_hFont, "I.MingCP", FONT_SIZE, FW_NORMAL, 1, 0, FONTFLAG_ANTIALIAS, 0x2E80, 0xFFFF);
		}

		// Automatically initialize XPos here.
		int iLength = 0, iTall = 0, xPos = 0;
		gFontFuncs::GetTextSize(s_hFont, string, &iLength, &iTall);	// Actually the tall is just the font tall. With consideration of \n symbol.
		xPos = 0 + (m_pParent->GetWide() - m_pParent->m_iExhibitionTableWidth - iLength) / 2;	// Centered.

		// Move upward a bit in favor of text printing.
		m_YposTracking -= iTall + CMarket::MARGIN_BETWEEN_BUTTONS;

		SetBounds(xPos, m_YposTracking, iLength, iTall);

		// Separator length setup.
		m_iSeparatorLength = m_pParent->GetWide() - ScrollableEditablePanel::WIDTH_SCROLLBAR;	// Don't forget the scroll bar!
		m_pParent->LocalToScreen(m_iSeparatorLength, iDummy);
		this->ScreenToLocal(m_iSeparatorLength, iDummy);
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
		int yMin = m_iIndexInArray * FONT_SIZE;
		int yMax = m_pParent->GetTall() - (m_pParent->m_rgpCategoryButtons.size() - m_iIndexInArray) * FONT_SIZE;

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

		SetAlpha(flAlpha);
	}

	void PaintBackground(void) final { /* Draw no background*/ }

	void Paint(void) override
	{
		if (!ShouldPaint())
			return;

		if (m_wstring.empty())
			return;

		gFontFuncs::DrawSetTextFont(s_hFont);
		gFontFuncs::DrawSetTextPos(0, 0);	// Drawing always centered at (0, 0)
		gFontFuncs::DrawSetTextColor(0xFFFFFF, GetAlpha());
		gFontFuncs::DrawPrintText(m_wstring.c_str());

		if (m_iSeparatorAlpha > FLT_EPSILON)	// Can't use the m_bShowingSeparator to determind. It would cause sudden disappear of separator.
		{
			DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, m_iSeparatorAlpha);
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

	void DoClick(void) final
	{
		BaseClass::DoClick();

		// Scroll to the designated location.
		m_pParent->m_pScrollablePanel->m_pScrollBar->SetValue(m_YposTracking);
	}


public:
//	static constexpr auto SIZE_HEIGHT = 36;
	static constexpr auto FONT_SIZE = CMarket::FONT_SIZE;	// UNDONE
	static constexpr auto HEIGHT_SEPARATOR = 2;
	static inline int s_hFont = 0;

private:
	int m_YposTracking{ 0U };
	unsigned m_iIndexInArray{ 0U };
	CMarket* m_pParent{ nullptr };
	std::wstring m_wstring;
	bool m_bShowingSeparator : 1;
	CPanelAnimationVar(float, m_iSeparatorAlpha, "m_iSeparatorAlpha", "0");
	int m_iSeparatorLength{ 0 };
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
	for (auto& wcs : g_rgwcsLocalisedCtgyNames)
	{
		int iCurLength = 0;
		gFontFuncs::GetTextSize(s_hFont, wcs.c_str(), &iCurLength, nullptr);

		if (iCurLength > m_iTextMaxiumWidth)
			m_iTextMaxiumWidth = iCurLength;
	}

	m_iExhibitionTableWidth = iPanelWidth - m_iTextMaxiumWidth - MARGIN_BETWEEN_BUTTON_AND_TEXT - ScrollableEditablePanel::WIDTH_SCROLLBAR;	// Remember to spare some space for CScrollBar.

	// Exhibition panel
	m_pPurchasablePanel = new EditablePanel(this, "ExihibitionPanel");
	m_pPurchasablePanel->SetBounds(0, 0, m_iExhibitionTableWidth, iPanelTall);

	m_rgpButtons.fill(nullptr);
	m_rgpCategoryButtons.reserve(16);	// Just... a guess...

	int x = 0, y = 0, iHorizontalShift = 0;
	for (unsigned i = 0; i < LAST_WEAPON; i++)
	{
		if (!Q_strlen(g_rgpszWeaponSprites[i]))
		{
			// Even though the sprite does not exist yet, you still have to skip the line.
			if ((1 << i) & g_bitsLastWeaponInCategory)
			{
				x = 0;
				y += WPN_SPRITE_HEIGHT + FONT_SIZE + MARGIN_BETWEEN_BUTTONS;
			}

			goto LAB_ADD_LABLE_BUTTON;
		}

		m_rgpButtons[i] = new LMImageButton(
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
			const auto index = m_rgpCategoryButtons.size();

			m_rgpCategoryButtons.push_back(new CCategoryLable(
				this,
				SharedVarArgs("CategoryLable%d", index),
				g_rgpszMarketCategoryNames[index],
				y,
				index
			));
		}
	}

	bool bShouldAddCategory = false;
	for (unsigned i = LAST_WEAPON+1, j=0; i < LAST_WEAPON + EQP_COUNT; i++,j++)
	{
		bShouldAddCategory = j == EQP_ASSAULT_SUIT || j == EQP_C4 || j == EQP_FLASHLIGHT;

		m_rgpButtons[i] = new LMImageButton(
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

		if (bShouldAddCategory)
		{
			const auto index = m_rgpCategoryButtons.size();

			m_rgpCategoryButtons.push_back(new CCategoryLable(
				this,
				SharedVarArgs("CategoryLable%d", index),
				g_rgpszMarketCategoryNames[index],
				y,
				index
			));
		}
	}

	m_pPurchasablePanel->SetTall(y);	// Update tall for the elements we added.

	m_pScrollablePanel = new ScrollableEditablePanel(this, m_pPurchasablePanel, "ScrollablePanel");
	m_pScrollablePanel->SetBounds(iPanelWidth - m_iExhibitionTableWidth, 0, m_iExhibitionTableWidth, iPanelTall);
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

void CMarket::OnCommand(const char* szCommand)
{
	Show(false);

	if (Q_strstr(szCommand, "buyweapon"))
		gEngfuncs.pfnClientCmd(szCommand);
}

void CMarket::InvalidateLayout(bool layoutNow, bool reloadScheme)
{
	BaseClass::InvalidateLayout(layoutNow, reloadScheme);

	UpdateMarket();
}

void CMarket::UpdateMarket(void)
{
}
