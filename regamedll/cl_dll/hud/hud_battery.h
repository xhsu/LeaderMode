/*

Created Date: Mar 11 2020

*/

#pragma once

class CHudBattery : public CBaseHudElement
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void InitHUDData(void);

	void MsgFunc_Battery(int& iNewArmourValue);
	void MsgFunc_ArmorType(int& iArmourType);

	float GetMaxArmour(void);

public:
	int m_HUD_suit_empty, m_HUD_suit_full;
	int m_HUD_suithelmet_empty, m_HUD_suithelmet_full;
	float m_flArmour;
	float m_flDrawingArmour;
	float m_fFade;
	int m_iArmorType;
	int m_iHeight;		// width of the battery innards
};
