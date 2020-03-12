/*

Created Date: Mar 11 2020

*/

#pragma once

class CHudBattery : public CBaseHUDElement
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);

	void MsgFunc_Battery(int& iNewArmourValue);
	void MsgFunc_ArmorType(int& iArmourType);

public:
	int m_HUD_suit_empty, m_HUD_suit_full;
	int m_HUD_suithelmet_empty, m_HUD_suithelmet_full;
	int m_iBat;
	float m_fFade;
	int m_iArmorType;
	int m_iHeight;		// width of the battery innards
};
