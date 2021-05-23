/*

Copied Date: May 22 2021 [CS:BTE Alpha]

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky

*/

#ifndef CREATEMULTIPLAYERGAMEDIALOG_H
#define CREATEMULTIPLAYERGAMEDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyDialog.h>

class CCreateMultiplayerGameServerPage;
class CCreateMultiplayerGameGameplayPage;
class CCreateMultiplayerGameBotPage;

class CCreateMultiplayerGameDialog : public vgui::PropertyDialog
{
	DECLARE_CLASS_SIMPLE(CCreateMultiplayerGameDialog, vgui::PropertyDialog);

public:
	CCreateMultiplayerGameDialog(vgui::Panel* parent);
	~CCreateMultiplayerGameDialog(void);

protected:
	virtual bool OnOK(bool applyOnly);

private:
	CCreateMultiplayerGameServerPage* m_pServerPage;
	CCreateMultiplayerGameGameplayPage* m_pGameplayPage;
	CCreateMultiplayerGameBotPage* m_pBotPage;
	KeyValues* m_pSavedData;
};

#endif
