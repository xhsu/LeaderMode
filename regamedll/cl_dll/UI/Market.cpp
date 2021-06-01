/*

Created Date: May 31 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#include "precompiled.h"

using namespace vgui;


std::array<std::wstring, _countof(g_rgpszWeaponCategoryNames)> g_rgwcsLocalisedWpnCtgyNames;
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
		m_iIndexInArray(indexInArray)
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
		}
		else if (ey > yMax)
		{
			flAlpha = std::clamp<float>(1.0f - float(ey - yMax) / (float)m_pParent->GetTall(), 0, 1) * 255.0;
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


public:
//	static constexpr auto SIZE_HEIGHT = 36;
	static constexpr auto FONT_SIZE = CMarket::FONT_SIZE;	// UNDONE
	static inline int s_hFont = 0;

private:
	int m_YposTracking{ 0U };
	unsigned m_iIndexInArray{ 0U };
	CMarket* m_pParent{ nullptr };
	std::wstring m_wstring;
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
	for (unsigned i = 0; i < _countof(g_rgpszWeaponCategoryNames); i++)
		g_rgwcsLocalisedWpnCtgyNames[i] = UTIL_GetLocalisation(g_rgpszWeaponCategoryNames[i]);

	// Find maxium text length in order to determind how many buttons can we place on one row.
	int iMaxLength = 0;
	for (auto& wcs : g_rgwcsLocalisedWpnCtgyNames)
	{
		int iCurLength = 0;
		gFontFuncs::GetTextSize(s_hFont, wcs.c_str(), &iCurLength, nullptr);

		if (iCurLength > iMaxLength)
			iMaxLength = iCurLength;
	}

	const auto iTotalButtonsWide = iPanelWidth - iMaxLength - MARGIN_BETWEEN_BUTTON_AND_TEXT - SIZE_SCROLL_BAR;	// Remember to spare some space for CScrollBar.

	// Exhibition panel
	m_pPurchasablePanel = new EditablePanel(this, "ExihibitionPanel");
	m_pPurchasablePanel->SetBounds(0, 0, iTotalButtonsWide, iPanelTall);

	m_rgpButtons.fill(nullptr);
	m_rgpCategoryButtons.clear();

	int x = 0, y = 0, iHorizontalShift = 0;
	for (unsigned i = 0; i < LAST_WEAPON; i++)
	{
		if ((1 << i) & g_bitsLastWeaponInCategory)
		{
			const auto index = m_rgpCategoryButtons.size();
			m_rgpCategoryButtons.push_back(new CCategoryLable(
				this,
				SharedVarArgs("CategoryLable%d", index),
				g_rgpszWeaponCategoryNames[index],
				y,
				index
			));
		}

		if (!Q_strlen(g_rgpszWeaponSprites[i]))
		{
			// Even though the sprite does not exist yet, you still have to skip the line.
			if ((1 << i) & g_bitsLastWeaponInCategory)
			{
				x = 0;
				y += WPN_SPRITE_HEIGHT + FONT_SIZE + MARGIN_BETWEEN_BUTTONS;
			}

			continue;
		}

		m_rgpButtons[i] = new LMImageButton(m_pPurchasablePanel, g_rgWpnInfo[i].m_pszInternalName, g_rgWpnInfo[i].m_pszExternalName, this);
		m_rgpButtons[i]->SetCommand("buy %s", g_rgWpnInfo[i].m_pszInternalName);
		m_rgpButtons[i]->SetVisible(true);
		m_rgpButtons[i]->SetUpImage(g_rgpszWeaponSprites[i]);
		m_rgpButtons[i]->SetFont(s_hFont);
		m_rgpButtons[i]->SetPinCorner(PIN_TOPLEFT, x, y);	// In scrollable page, you cannot use regular SetPos().
		m_rgpButtons[i]->SetSizeByImageHeight(WPN_SPRITE_HEIGHT);
		m_rgpButtons[i]->InvalidateLayout(true);

		iHorizontalShift = (m_rgpButtons[i]->GetWide() + MARGIN_BETWEEN_BUTTONS);

		// Have to use negative since negative means left shift.
		if ((x + iHorizontalShift) > iTotalButtonsWide || ((1 << i) & g_bitsLastWeaponInCategory))
		{
			x = 0;
			y += WPN_SPRITE_HEIGHT + FONT_SIZE + MARGIN_BETWEEN_BUTTONS;
		}
		else
		{
			x += iHorizontalShift;
		}
	}

	m_pPurchasablePanel->SetTall(y);	// Update tall for the elements we added.

	m_pScrollablePanel = new ScrollableEditablePanel(this, m_pPurchasablePanel, "ScrollablePanel");
	m_pScrollablePanel->SetBounds(iPanelWidth - iTotalButtonsWide, 0, iTotalButtonsWide, iPanelTall);
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
