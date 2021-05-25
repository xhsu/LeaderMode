/*

Copied Date: May 25 2021 [Mind-Team]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#pragma once

#ifndef OPTIONS_SUB_VIDEO_H
#define OPTIONS_SUB_VIDEO_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/PropertyPage.h>
#include "Interface/IGameUIFuncs.h"
#include <vgui_controls/URLButton.h>
#include "public/modes.h"	// for vmode_t

class CCvarSlider;

// Copied from HUD.h of client
typedef struct SCREENINFO_s
{
	int		iSize;
	int		iWidth;
	int		iHeight;
	int		iFlags;
	int		iCharHeight;
	short	charWidths[256];

} SCREENINFO;

//-----------------------------------------------------------------------------
// Purpose: Video Details, Part of OptionsDialog
//-----------------------------------------------------------------------------
class COptionsSubVideo : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE(COptionsSubVideo, vgui::PropertyPage);

public:
	COptionsSubVideo(vgui::Panel* parent);
	~COptionsSubVideo();

	virtual void OnResetData();
	virtual void OnApplyChanges();
	virtual void PerformLayout();

	virtual bool RequiresRestart();

private:
	void        SetCurrentResolutionComboItem();

	MESSAGE_FUNC(OnDataChanged, "ControlModified");
	MESSAGE_FUNC_PTR_CHARPTR(OnTextChanged, "TextChanged", panel, text);
	MESSAGE_FUNC(OpenAdvanced, "OpenAdvanced");
	MESSAGE_FUNC(LaunchBenchmark, "LaunchBenchmark");
	MESSAGE_FUNC(OpenGammaDialog, "OpenGammaDialog");


	void		PrepareResolutionList();

	int m_nSelectedMode; // -1 if we are running in a nonstandard mode

	vgui::ComboBox* m_pMode;
	vgui::ComboBox* m_pWindowed;
	vgui::ComboBox* m_pAspectRatio;
	vgui::Button* m_pGammaButton;
	vgui::Button* m_pAdvanced;
	vgui::Button* m_pBenchmark;

	vgui::DHANDLE<class COptionsSubVideoAdvancedDlg> m_hOptionsSubVideoAdvancedDlg;
	vgui::DHANDLE<class CGammaDialog> m_hGammaDialog;

	bool m_bRequireRestart;
	MESSAGE_FUNC(OpenThirdPartyVideoCreditsDialog, "OpenThirdPartyVideoCreditsDialog");
	vgui::URLButton* m_pThirdPartyCredits;
	vgui::DHANDLE<class COptionsSubVideoThirdPartyCreditsDlg> m_OptionsSubVideoThirdPartyCreditsDlg;
};

#endif // OPTIONS_SUB_VIDEO_H
