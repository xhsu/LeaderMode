/*

Copied Date: May 25 2021 [Mind-Team]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#include "precompiled.h"

using namespace vgui;


//-----------------------------------------------------------------------------
// Purpose: aspect ratio mappings (for normal/widescreen combo)
//-----------------------------------------------------------------------------
struct RatioToAspectMode_t
{
	int anamorphic;
	float aspectRatio;
};
RatioToAspectMode_t g_RatioToAspectModes[] =
{
	{	0,		4.0f / 3.0f },
	{	1,		16.0f / 9.0f },
	{	2,		16.0f / 10.0f },
	{	2,		1.0f },
};

struct AAMode_t
{
	int m_nNumSamples;
	int m_nQualityLevel;
};

//-----------------------------------------------------------------------------
// Purpose: list of valid dx levels
//-----------------------------------------------------------------------------
int g_DirectXLevels[] =
{
	70,
	80,
	81,
	90,
	95,
};

//-----------------------------------------------------------------------------
// Purpose: returns the string name of a given dxlevel
//-----------------------------------------------------------------------------
void GetNameForDXLevel(int dxlevel, char* name, int bufferSize)
{
	if (dxlevel == 95)
	{
		Q_snprintf(name, bufferSize, "DirectX v9.0+");
	}
	else
	{
		Q_snprintf(name, bufferSize, "DirectX v%.1f", dxlevel / 10.0f);
	}
}

//-----------------------------------------------------------------------------
// Purpose: returns the aspect ratio mode number for the given resolution
//-----------------------------------------------------------------------------
int GetScreenAspectMode(int width, int height)
{
	float aspectRatio = (float)width / (float)height;

	// just find the closest ratio
	float closestAspectRatioDist = 99999.0f;
	int closestAnamorphic = 0;
	for (int i = 0; i < ARRAYSIZE(g_RatioToAspectModes); i++)
	{
		float dist = fabs(g_RatioToAspectModes[i].aspectRatio - aspectRatio);
		if (dist < closestAspectRatioDist)
		{
			closestAspectRatioDist = dist;
			closestAnamorphic = g_RatioToAspectModes[i].anamorphic;
		}
	}

	return closestAnamorphic;
}

//-----------------------------------------------------------------------------
// Purpose: returns the string name of the specified resolution mode
//-----------------------------------------------------------------------------
void GetResolutionName(vmode_t* mode, char* sz, int sizeofsz)
{
	if (mode->width == 1280 && mode->height == 1024)
	{
		// LCD native monitor resolution gets special case
		Q_snprintf(sz, sizeofsz, "%i x %i (LCD)", mode->width, mode->height);
	}
	else
	{
		Q_snprintf(sz, sizeofsz, "%i x %i", mode->width, mode->height);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Gamma-adjust dialog
//-----------------------------------------------------------------------------
class CGammaDialog : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CGammaDialog, vgui::Frame);
public:
	CGammaDialog(vgui::VPANEL hParent) : BaseClass(NULL, "OptionsSubVideoGammaDlg")
	{
		// parent is ignored, since we want look like we're steal focus from the parent (we'll become modal below)
		SetTitle("#GameUI_AdjustGamma_Title", true);
		SetSize(400, 260);
		SetDeleteSelfOnClose(true);

		m_pGammaSlider = new CCvarSlider(this, "Gamma", "#GameUI_Gamma", 1.8f, 3.0f, "gamma");
		m_pGammaLabel = new Label(this, "Gamma label", "#GameUI_Gamma");
		m_pGammaEntry = new TextEntry(this, "GammaEntry");

		Button* ok = new Button(this, "OKButton", "#vgui_ok");
		ok->SetCommand(new KeyValues("OK"));

		LoadControlSettings("resource/OptionsSubVideoGammaDlg.res");
		MoveToCenterOfScreen();
		SetSizeable(false);

		m_pGammaSlider->SetTickCaptions("#GameUI_Light", "#GameUI_Dark");
	}

	MESSAGE_FUNC_PTR(OnGammaChanged, "SliderMoved", panel)
	{
		if (panel == m_pGammaSlider)
		{
			m_pGammaSlider->ApplyChanges();
		}
	}

	virtual void Activate()
	{
		BaseClass::Activate();
		m_flOriginalGamma = m_pGammaSlider->GetValue();
		UpdateGammaLabel();
	}

	MESSAGE_FUNC(OnOK, "OK")
	{
		// make the gamma stick
		m_flOriginalGamma = m_pGammaSlider->GetValue();
		Close();
	}

	virtual void OnClose()
	{
		// reset to the original gamma
		m_pGammaSlider->SetValue(m_flOriginalGamma);
		m_pGammaSlider->ApplyChanges();
		BaseClass::OnClose();
	}

	void OnKeyCodeTyped(KeyCode code)
	{
		// force ourselves to be closed if the escape key it pressed
		if (code == KEY_ESCAPE)
		{
			Close();
		}
		else
		{
			BaseClass::OnKeyCodeTyped(code);
		}
	}

	MESSAGE_FUNC_PTR(OnControlModified, "ControlModified", panel)
	{
		// the HasBeenModified() check is so that if the value is outside of the range of the
		// slider, it won't use the slider to determine the display value but leave the
		// real value that we determined in the constructor
		if (panel == m_pGammaSlider && m_pGammaSlider->HasBeenModified())
		{
			UpdateGammaLabel();
		}
	}

	MESSAGE_FUNC_PTR(OnTextChanged, "TextChanged", panel)
	{
		if (panel == m_pGammaEntry)
		{
			char buf[64];
			m_pGammaEntry->GetText(buf, 64);

			float fValue = (float)atof(buf);
			if (fValue >= 1.0)
			{
				m_pGammaSlider->SetSliderValue(fValue);
				PostActionSignal(new KeyValues("ApplyButtonEnable"));
			}
		}
	}

	void UpdateGammaLabel()
	{
		char buf[64];
		Q_snprintf(buf, sizeof(buf), " %.1f", m_pGammaSlider->GetSliderValue());
		m_pGammaEntry->SetText(buf);
	}


private:
	CCvarSlider* m_pGammaSlider;
	vgui::Label* m_pGammaLabel;
	vgui::TextEntry* m_pGammaEntry;
	float				m_flOriginalGamma;
};


//-----------------------------------------------------------------------------
// Purpose: advanced keyboard settings dialog
//-----------------------------------------------------------------------------
class COptionsSubVideoAdvancedDlg : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(COptionsSubVideoAdvancedDlg, vgui::Frame);
public:
	COptionsSubVideoAdvancedDlg(vgui::Panel* parent) : BaseClass(parent, "OptionsSubVideoAdvancedDlg")
	{
		SetTitle("#GameUI_VideoAdvanced_Title", true);
		SetSize(260, 400);

		m_pDXLevel = new ComboBox(this, "dxlabel", 6, false);

		m_pModelDetail = new ComboBox(this, "ModelDetail", 6, false);
		m_pModelDetail->AddItem("#gameui_low", NULL);
		m_pModelDetail->AddItem("#gameui_medium", NULL);
		m_pModelDetail->AddItem("#gameui_high", NULL);

		m_pTextureDetail = new ComboBox(this, "TextureDetail", 6, false);
		m_pTextureDetail->AddItem("#gameui_low", NULL);
		m_pTextureDetail->AddItem("#gameui_medium", NULL);
		m_pTextureDetail->AddItem("#gameui_high", NULL);
		m_pTextureDetail->AddItem("#gameui_ultra", NULL);

		// Build list of MSAA and CSAA modes, based upon those which are supported by the device
		//
		// The modes that we've seen in the wild to date are as follows (in perf order, fastest to slowest)
		//
		//								2x	4x	6x	8x	16x	8x	16xQ
		//		Texture/Shader Samples	1	1	1	1	1	1	1
		//		Stored Color/Z Samples	2	4	6	4	4	8	8
		//		Coverage Samples		2	4	6	8	16	8	16
		//		MSAA or CSAA			M	M	M	C	C	M	C
		//
		//	The CSAA modes are nVidia only (added in the G80 generation of GPUs)
		//
		m_nNumAAModes = 0;
		m_pAntialiasingMode = new ComboBox(this, "AntialiasingMode", 10, false);
		m_pAntialiasingMode->AddItem("#GameUI_None", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 1;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;

		m_pAntialiasingMode->AddItem("#GameUI_2X", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 2;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;

		m_pAntialiasingMode->AddItem("#GameUI_4X", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 4;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;

		m_pAntialiasingMode->AddItem("#GameUI_6X", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 6;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;

		m_pAntialiasingMode->AddItem("#GameUI_8X_CSAA", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 4;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 2;
		m_nNumAAModes++;

		m_pAntialiasingMode->AddItem("#GameUI_16X_CSAA", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 4;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 4;
		m_nNumAAModes++;

		m_pAntialiasingMode->AddItem("#GameUI_8X", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 8;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;

		m_pAntialiasingMode->AddItem("#GameUI_16XQ_CSAA", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 8;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 2;
		m_nNumAAModes++;

		m_pFilteringMode = new ComboBox(this, "FilteringMode", 6, false);
		m_pFilteringMode->AddItem("#GameUI_Bilinear", NULL);
		m_pFilteringMode->AddItem("#GameUI_Trilinear", NULL);
		m_pFilteringMode->AddItem("#GameUI_Anisotropic2X", NULL);
		m_pFilteringMode->AddItem("#GameUI_Anisotropic4X", NULL);
		m_pFilteringMode->AddItem("#GameUI_Anisotropic8X", NULL);
		m_pFilteringMode->AddItem("#GameUI_Anisotropic16X", NULL);

		m_pShadowDetail = new ComboBox(this, "ShadowDetail", 6, false);
		m_pShadowDetail->AddItem("#gameui_low", NULL);
		m_pShadowDetail->AddItem("#gameui_medium", NULL);
		m_pShadowDetail->AddItem("#gameui_high", NULL);

		m_pHDR = new ComboBox(this, "HDR", 6, false);
		m_pHDR->AddItem("#GameUI_hdr_level0", NULL);
		m_pHDR->AddItem("#GameUI_hdr_level1", NULL);
		m_pHDR->AddItem("#GameUI_hdr_level2", NULL);
		m_pHDR->AddItem("#GameUI_hdr_level3", NULL);

		m_pWaterDetail = new ComboBox(this, "WaterDetail", 6, false);
		m_pWaterDetail->AddItem("#gameui_noreflections", NULL);
		m_pWaterDetail->AddItem("#gameui_reflectonlyworld", NULL);
		m_pWaterDetail->AddItem("#gameui_reflectall", NULL);

		m_pVSync = new ComboBox(this, "VSync", 2, false);
		m_pVSync->AddItem("#gameui_disabled", NULL);
		m_pVSync->AddItem("#gameui_enabled", NULL);

		m_pShaderDetail = new ComboBox(this, "ShaderDetail", 6, false);
		m_pShaderDetail->AddItem("#gameui_low", NULL);
		m_pShaderDetail->AddItem("#gameui_high", NULL);

		m_pColorCorrection = new ComboBox(this, "ColorCorrection", 2, false);
		m_pColorCorrection->AddItem("#gameui_disabled", NULL);
		m_pColorCorrection->AddItem("#gameui_enabled", NULL);

		m_pMotionBlur = new ComboBox(this, "MotionBlur", 2, false);
		m_pMotionBlur->AddItem("#gameui_disabled", NULL);
		m_pMotionBlur->AddItem("#gameui_enabled", NULL);

		LoadControlSettings("resource/OptionsSubVideoAdvancedDlg.res");
		MoveToCenterOfScreen();
		SetSizeable(false);



		MarkDefaultSettingsAsRecommended();

		m_bUseChanges = false;
	}

	virtual void Activate()
	{
		BaseClass::Activate();

		input()->SetAppModalSurface(GetVPanel());

		if (!m_bUseChanges)
		{
			// reset the data
			OnResetData();
		}
	}

	void SetComboItemAsRecommended(vgui::ComboBox* combo, int iItem)
	{
		// get the item text
		wchar_t text[512];
		combo->GetItemText(iItem, text, sizeof(text));

		// append the recommended flag
		wchar_t newText[512];
		_snwprintf(newText, sizeof(newText) / sizeof(wchar_t), L"%s *", text);

		// reset
		combo->UpdateItem(iItem, newText, NULL);
	}

	int FindMSAAMode(int nAASamples, int nAAQuality)
	{
		// Run through the AA Modes supported by the device
		for (int nAAMode = 0; nAAMode < m_nNumAAModes; nAAMode++)
		{
			// If we found the mode that matches what we're looking for, return the index
			if ((m_nAAModes[nAAMode].m_nNumSamples == nAASamples) && (m_nAAModes[nAAMode].m_nQualityLevel == nAAQuality))
			{
				return nAAMode;
			}
		}

		return 0;	// Didn't find what we're looking for, so no AA
	}

	MESSAGE_FUNC_PTR(OnTextChanged, "TextChanged", panel)
	{
		if (panel == m_pDXLevel && RequiresRestart())
		{
			// notify the user that this will require a disconnect
			QueryBox* box = new QueryBox("#GameUI_SettingRequiresDisconnect_Title", "#GameUI_SettingRequiresDisconnect_Info");
			box->AddActionSignalTarget(this);
			box->SetCancelCommand(new KeyValues("ResetDXLevelCombo"));
			box->DoModal();
		}
	}

	MESSAGE_FUNC(OnGameUIHidden, "GameUIHidden")	// called when the GameUI is hidden
	{
		Close();
	}

	MESSAGE_FUNC(ResetDXLevelCombo, "ResetDXLevelCombo")
	{

		// Reset HDR too
	}

	MESSAGE_FUNC(OK_Confirmed, "OK_Confirmed")
	{
		m_bUseChanges = true;
		Close();
	}

	void MarkDefaultSettingsAsRecommended()
	{
		// Pull in data from dxsupport.cfg database (includes fine-grained per-vendor/per-device config data)

		// Read individual values from keyvalues which came from dxsupport.cfg database

		// Only recommend a dxlevel if there is more than one available


		// Map desired mode to list item number







	}

	void ApplyChangesToConVar(const char* pConVarName, int value)
	{
		char szCmd[256];
		Q_snprintf(szCmd, sizeof(szCmd), "%s %d\n", pConVarName, value);
		gEngfuncs.pfnClientCmd(szCmd);
	}

	virtual void ApplyChanges()
	{
		if (!m_bUseChanges)
			return;


		// reset everything tied to the filtering mode, then the switch sets the appropriate one

		// Set the AA convars according to the menu item chosen







	}

	virtual void OnResetData()
	{






		// Map convar to item on AA drop-down





		// get current hardware dx support level

		// get installed version
	}

	virtual void OnCommand(const char* command)
	{
		if (!Q_stricmp(command, "OK"))
		{
			if (RequiresRestart())
			{
				// Bring up the confirmation dialog
				QueryBox* box = new QueryBox("#GameUI_SettingRequiresDisconnect_Title", "#GameUI_SettingRequiresDisconnect_Info");
				box->AddActionSignalTarget(this);
				box->SetOKCommand(new KeyValues("OK_Confirmed"));
				box->SetCancelCommand(new KeyValues("ResetDXLevelCombo"));
				box->DoModal();
				box->MoveToFront();
				return;
			}

			m_bUseChanges = true;
			Close();
		}
		else
		{
			BaseClass::OnCommand(command);
		}
	}

	void OnKeyCodeTyped(KeyCode code)
	{
		// force ourselves to be closed if the escape key it pressed
		if (code == KEY_ESCAPE)
		{
			Close();
		}
		else
		{
			BaseClass::OnKeyCodeTyped(code);
		}
	}

	bool RequiresRestart()
	{


		// HDR changed?
		return false;
	}

private:
	bool m_bUseChanges;
	vgui::ComboBox* m_pModelDetail, * m_pTextureDetail, * m_pAntialiasingMode, * m_pFilteringMode;
	vgui::ComboBox* m_pShadowDetail, * m_pHDR, * m_pWaterDetail, * m_pVSync, * m_pShaderDetail;
	vgui::ComboBox* m_pColorCorrection;
	vgui::ComboBox* m_pMotionBlur;
	vgui::ComboBox* m_pDXLevel;

	int m_nNumAAModes;
	AAMode_t m_nAAModes[16];
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
COptionsSubVideo::COptionsSubVideo(vgui::Panel* parent) : PropertyPage(parent, NULL)
{
	m_bRequireRestart = false;

	m_pGammaButton = new Button(this, "GammaButton", "#GameUI_AdjustGamma");
	m_pGammaButton->SetCommand(new KeyValues("OpenGammaDialog"));
	m_pMode = new ComboBox(this, "Resolution", 8, false);
	m_pAspectRatio = new ComboBox(this, "AspectRatio", 6, false);
	m_pAdvanced = new Button(this, "AdvancedButton", "#GameUI_AdvancedEllipsis");
	m_pAdvanced->SetCommand(new KeyValues("OpenAdvanced"));
	m_pBenchmark = new Button(this, "BenchmarkButton", "#GameUI_LaunchBenchmark");
	m_pBenchmark->SetCommand(new KeyValues("LaunchBenchmark"));
	m_pThirdPartyCredits = new URLButton(this, "ThirdPartyVideoCredits", "#GameUI_ThirdPartyTechCredits");
	m_pThirdPartyCredits->SetCommand(new KeyValues("OpenThirdPartyVideoCreditsDialog"));

	char pszAspectName[3][64];
	wchar_t* unicodeText = g_pVGuiLocalize->Find("#GameUI_AspectNormal");
	g_pVGuiLocalize->ConvertUnicodeToANSI(unicodeText, pszAspectName[0], 32);
	unicodeText = g_pVGuiLocalize->Find("#GameUI_AspectWide16x9");
	g_pVGuiLocalize->ConvertUnicodeToANSI(unicodeText, pszAspectName[1], 32);
	unicodeText = g_pVGuiLocalize->Find("#GameUI_AspectWide16x10");
	g_pVGuiLocalize->ConvertUnicodeToANSI(unicodeText, pszAspectName[2], 32);

	int iNormalItemID = m_pAspectRatio->AddItem(pszAspectName[0], NULL);
	int i16x9ItemID = m_pAspectRatio->AddItem(pszAspectName[1], NULL);
	int i16x10ItemID = m_pAspectRatio->AddItem(pszAspectName[2], NULL);

	SCREENINFO screen;
	screen.iSize = sizeof(screen);
	gEngfuncs.pfnGetScreenInfo(&screen);

	int iAspectMode = GetScreenAspectMode(screen.iWidth, screen.iHeight);
	switch (iAspectMode)
	{
	default:
	case 0:
		m_pAspectRatio->ActivateItem(iNormalItemID);
		break;
	case 1:
		m_pAspectRatio->ActivateItem(i16x9ItemID);
		break;
	case 2:
		m_pAspectRatio->ActivateItem(i16x10ItemID);
		break;
	}

	m_pWindowed = new vgui::ComboBox(this, "DisplayModeCombo", 6, false);
	m_pWindowed->AddItem("#GameUI_Fullscreen", NULL);
	m_pWindowed->AddItem("#GameUI_Windowed", NULL);

	LoadControlSettings("Resource\\OptionsSubVideo.res");

	// Moved down here so we can set the Drop down's 
	// menu state after the default (disabled) value is loaded
	PrepareResolutionList();

	// only show the benchmark button if they have the benchmark map
}

//-----------------------------------------------------------------------------
// Purpose: Generates resolution list
//-----------------------------------------------------------------------------
void COptionsSubVideo::PrepareResolutionList()
{
	// get the currently selected resolution
	char sz[256];
	m_pMode->GetText(sz, 256);
	int currentWidth = 0, currentHeight = 0;
	sscanf(sz, "%i x %i", &currentWidth, &currentHeight);

	// Clean up before filling the info again.
	m_pMode->DeleteAllItems();
	m_pAspectRatio->SetItemEnabled(1, false);
	m_pAspectRatio->SetItemEnabled(2, false);

	// get full video mode list
	vmode_t* plist = NULL;
	int count = 0;
	gameuifuncs->GetVideoModes(&plist, &count);


	bool bWindowed = (m_pWindowed->GetActiveItem() > 0);
	SCREENINFO screen;
	screen.iSize = sizeof(screen);
	gEngfuncs.pfnGetScreenInfo(&screen);

	// iterate all the video modes adding them to the dropdown
	bool bFoundWidescreen = false;
	int selectedItemID = -1;
	for (int i = 0; i < count; i++, plist++)
	{
		char sz[256];
		GetResolutionName(plist, sz, sizeof(sz));

		// don't show modes bigger than the desktop for windowed mode

		int itemID = -1;
		int iAspectMode = GetScreenAspectMode(plist->width, plist->height);
		if (iAspectMode > 0)
		{
			m_pAspectRatio->SetItemEnabled(iAspectMode, true);
			bFoundWidescreen = true;
		}

		// filter the list for those matching the current aspect
		if (iAspectMode == m_pAspectRatio->GetActiveItem())
		{
			itemID = m_pMode->AddItem(sz, NULL);
		}

		// try and find the best match for the resolution to be selected
		if (plist->width == currentWidth && plist->height == currentHeight)
		{
			selectedItemID = itemID;
		}
		else if (selectedItemID == -1 && plist->width == screen.iWidth && plist->height == screen.iHeight)
		{
			selectedItemID = itemID;
		}
	}

	// disable ratio selection if we can't display widescreen.

	m_nSelectedMode = selectedItemID;

	if (selectedItemID != -1)
	{
		m_pMode->ActivateItem(selectedItemID);
	}
	else
	{
		char sz[256];
		sprintf(sz, "%d x %d", screen.iWidth, screen.iHeight);
		m_pMode->SetText(sz);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
COptionsSubVideo::~COptionsSubVideo()
{
	if (m_hOptionsSubVideoAdvancedDlg.Get())
	{
		m_hOptionsSubVideoAdvancedDlg->MarkForDeletion();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubVideo::OnResetData()
{
	m_bRequireRestart = false;


	// reset UI elements

	// reset gamma control
	m_pGammaButton->SetEnabled(true);


	SetCurrentResolutionComboItem();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubVideo::SetCurrentResolutionComboItem()
{
	vmode_t* plist = NULL;
	int count = 0;
	gameuifuncs->GetVideoModes(&plist, &count);

	SCREENINFO screen;
	screen.iSize = sizeof(screen);
	gEngfuncs.pfnGetScreenInfo(&screen);

	int resolution = -1;
	for (int i = 0; i < count; i++, plist++)
	{
		if (plist->width == screen.iWidth &&
			plist->height == screen.iHeight)
		{
			resolution = i;
			break;
		}
	}

	if (resolution != -1)
	{
		char sz[256];
		GetResolutionName(plist, sz, sizeof(sz));
		m_pMode->SetText(sz);
	}
}

//-----------------------------------------------------------------------------
// Purpose: restarts the game
//-----------------------------------------------------------------------------
void COptionsSubVideo::OnApplyChanges()
{

	// apply advanced options
	if (m_hOptionsSubVideoAdvancedDlg.Get())
	{
		m_hOptionsSubVideoAdvancedDlg->ApplyChanges();
	}

	// resolution


	// windowed

	// make sure there is a change

	// apply changes

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubVideo::PerformLayout()
{
	BaseClass::PerformLayout();

}

//-----------------------------------------------------------------------------
// Purpose: enables apply button on data changing
//-----------------------------------------------------------------------------
void COptionsSubVideo::OnTextChanged(Panel* pPanel, const char* pszText)
{
	if (pPanel == m_pMode)
	{
		SCREENINFO screen;
		screen.iSize = sizeof(SCREENINFO);
		gEngfuncs.pfnGetScreenInfo(&screen);

		m_nSelectedMode = m_pMode->GetActiveItem();

		int w = 0, h = 0;
		sscanf(pszText, "%i x %i", &w, &h);
		if (screen.iWidth != w || screen.iHeight != h)
		{
			OnDataChanged();
		}
	}
	else if (pPanel == m_pAspectRatio)
	{
		PrepareResolutionList();
	}
	else if (pPanel == m_pWindowed)
	{
		PrepareResolutionList();
		OnDataChanged();
	}
}

//-----------------------------------------------------------------------------
// Purpose: enables apply button
//-----------------------------------------------------------------------------
void COptionsSubVideo::OnDataChanged()
{
	PostActionSignal(new KeyValues("ApplyButtonEnable"));
}

//-----------------------------------------------------------------------------
// Purpose: Checks to see if the changes requires a restart to take effect
//-----------------------------------------------------------------------------
bool COptionsSubVideo::RequiresRestart()
{
	if (m_hOptionsSubVideoAdvancedDlg.Get()
		&& m_hOptionsSubVideoAdvancedDlg->RequiresRestart())
	{
		return true;
	}

	// make sure there is a change
	return m_bRequireRestart;
}

//-----------------------------------------------------------------------------
// Purpose: Opens advanced video mode options dialog
//-----------------------------------------------------------------------------
void COptionsSubVideo::OpenAdvanced()
{
	if (!m_hOptionsSubVideoAdvancedDlg.Get())
	{
		m_hOptionsSubVideoAdvancedDlg = new COptionsSubVideoAdvancedDlg(GetParent()); // we'll parent this to the OptionsDialog directly
	}

	m_hOptionsSubVideoAdvancedDlg->Activate();
}

//-----------------------------------------------------------------------------
// Purpose: Opens gamma-adjusting dialog
//-----------------------------------------------------------------------------
void COptionsSubVideo::OpenGammaDialog()
{
	if (!m_hGammaDialog.Get())
	{
		m_hGammaDialog = new CGammaDialog(GetVParent());
	}

	m_hGammaDialog->Activate();
}

//-----------------------------------------------------------------------------
// Purpose: Opens benchmark dialog
//-----------------------------------------------------------------------------
void COptionsSubVideo::LaunchBenchmark()
{
}

//-----------------------------------------------------------------------------
// Purpose: third-party audio credits dialog
//-----------------------------------------------------------------------------
class COptionsSubVideoThirdPartyCreditsDlg : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(COptionsSubVideoThirdPartyCreditsDlg, vgui::Frame);
public:
	COptionsSubVideoThirdPartyCreditsDlg(vgui::VPANEL hParent) : BaseClass(NULL, NULL)
	{
		// parent is ignored, since we want look like we're steal focus from the parent (we'll become modal below)

		SetTitle("#GameUI_ThirdPartyVideo_Title", true);
		SetSize(500, 200);
		LoadControlSettings("resource/OptionsSubVideoThirdPartyDlg.res");
		MoveToCenterOfScreen();
		SetSizeable(false);
		SetDeleteSelfOnClose(true);
	}

	virtual void Activate()
	{
		BaseClass::Activate();

		input()->SetAppModalSurface(GetVPanel());
	}

	void OnKeyCodeTyped(KeyCode code)
	{
		// force ourselves to be closed if the escape key it pressed
		if (code == KEY_ESCAPE)
		{
			Close();
		}
		else
		{
			BaseClass::OnKeyCodeTyped(code);
		}
	}
};

//-----------------------------------------------------------------------------
// Purpose: Open third party audio credits dialog
//-----------------------------------------------------------------------------
void COptionsSubVideo::OpenThirdPartyVideoCreditsDialog()
{
	if (!m_OptionsSubVideoThirdPartyCreditsDlg.Get())
	{
		m_OptionsSubVideoThirdPartyCreditsDlg = new COptionsSubVideoThirdPartyCreditsDlg(GetVParent());
	}
	m_OptionsSubVideoThirdPartyCreditsDlg->Activate();
}
