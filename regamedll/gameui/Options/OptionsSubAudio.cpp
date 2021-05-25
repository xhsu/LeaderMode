/*

Copied Date: May 25 2021 [Mind-Team]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#include "precompiled.h"

using namespace vgui;


// This member is static so that the updated audio language can be referenced during shutdown

enum SoundQuality_e
{
	SOUNDQUALITY_LOW,
	SOUNDQUALITY_MEDIUM,
	SOUNDQUALITY_HIGH,
};

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
COptionsSubAudio::COptionsSubAudio(vgui::Panel* parent) : PropertyPage(parent, NULL)
{
	m_pSFXSlider = new CCvarSlider(this, "SFXSlider", "#GameUI_SoundEffectVolume", 0.0f, 1.0f, "volume");
	m_pMusicSlider = new CCvarSlider(this, "MusicSlider", "#GameUI_MusicVolume", 0.0f, 1.0f, "MP3Volume");

	m_pCloseCaptionCombo = new ComboBox(this, "CloseCaptionCheck", 6, false);
	m_pCloseCaptionCombo->AddItem("#GameUI_NoClosedCaptions", NULL);
	m_pCloseCaptionCombo->AddItem("#GameUI_SubtitlesAndSoundEffects", NULL);
	m_pCloseCaptionCombo->AddItem("#GameUI_Subtitles", NULL);

	m_pSoundQualityCombo = new ComboBox(this, "SoundQuality", 6, false);
	m_pSoundQualityCombo->AddItem("#GameUI_High", new KeyValues("SoundQuality", "quality", SOUNDQUALITY_HIGH));
	m_pSoundQualityCombo->AddItem("#GameUI_Medium", new KeyValues("SoundQuality", "quality", SOUNDQUALITY_MEDIUM));
	m_pSoundQualityCombo->AddItem("#GameUI_Low", new KeyValues("SoundQuality", "quality", SOUNDQUALITY_LOW));

	m_pSpeakerSetupCombo = new ComboBox(this, "SpeakerSetup", 6, false);
	m_pSpeakerSetupCombo->AddItem("#GameUI_Headphones", new KeyValues("SpeakerSetup", "speakers", 0));
	m_pSpeakerSetupCombo->AddItem("#GameUI_2Speakers", new KeyValues("SpeakerSetup", "speakers", 2));
	m_pSpeakerSetupCombo->AddItem("#GameUI_4Speakers", new KeyValues("SpeakerSetup", "speakers", 4));
	m_pSpeakerSetupCombo->AddItem("#GameUI_5Speakers", new KeyValues("SpeakerSetup", "speakers", 5));
	m_pSpeakerSetupCombo->AddItem("#GameUI_7Speakers", new KeyValues("SpeakerSetup", "speakers", 7));

	m_pSpokenLanguageCombo = new ComboBox(this, "AudioSpokenLanguage", 6, false);

	LoadControlSettings("Resource\\OptionsSubAudio.res");
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
COptionsSubAudio::~COptionsSubAudio()
{
}

//-----------------------------------------------------------------------------
// Purpose: Reloads data
//-----------------------------------------------------------------------------
void COptionsSubAudio::OnResetData()
{
}

//-----------------------------------------------------------------------------
// Purpose: Applies changes
//-----------------------------------------------------------------------------
void COptionsSubAudio::OnApplyChanges()
{
}

//-----------------------------------------------------------------------------
// Purpose: Called on controls changing, enables the Apply button
//-----------------------------------------------------------------------------
void COptionsSubAudio::OnControlModified()
{
	PostActionSignal(new KeyValues("ApplyButtonEnable"));
}

//-----------------------------------------------------------------------------
// Purpose: returns true if the engine needs to be restarted
//-----------------------------------------------------------------------------
bool COptionsSubAudio::RequiresRestart()
{
	// nothing in audio requires a restart like now
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COptionsSubAudio::OnCommand(const char* command)
{
	if (!Q_stricmp(command, "TestSpeakers"))
	{
	}
	else if (!Q_stricmp(command, "ShowThirdPartyAudioCredits"))
	{
		OpenThirdPartySoundCreditsDialog();
	}

	BaseClass::OnCommand(command);
}

//-----------------------------------------------------------------------------
// Purpose: Run the test speakers map.
//-----------------------------------------------------------------------------
void COptionsSubAudio::RunTestSpeakers()
{
}

//-----------------------------------------------------------------------------
// Purpose: third-party audio credits dialog
//-----------------------------------------------------------------------------
class COptionsSubAudioThirdPartyCreditsDlg : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(COptionsSubAudioThirdPartyCreditsDlg, vgui::Frame);
public:
	COptionsSubAudioThirdPartyCreditsDlg(vgui::VPANEL hParent) : BaseClass(NULL, NULL)
	{
		// parent is ignored, since we want look like we're steal focus from the parent (we'll become modal below)

		SetTitle("#GameUI_ThirdPartyAudio_Title", true);
		SetSize(500, 200);
		LoadControlSettings("resource/OptionsSubAudioThirdPartyDlg.res");
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
void COptionsSubAudio::OpenThirdPartySoundCreditsDialog()
{
	if (!m_OptionsSubAudioThirdPartyCreditsDlg.Get())
	{
		m_OptionsSubAudioThirdPartyCreditsDlg = new COptionsSubAudioThirdPartyCreditsDlg(GetVParent());
	}
	m_OptionsSubAudioThirdPartyCreditsDlg->Activate();
}
