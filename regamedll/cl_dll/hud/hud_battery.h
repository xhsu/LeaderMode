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
	void Reset(void);
	void InitHUDData(void);

	void MsgFunc_Battery(int& iNewArmourValue);
	void MsgFunc_ArmorType(int& iArmourType);

	float GetMaxArmour(void);

public:
	hSprite m_hSuitEmpty, m_hSuitHelmetEmpty;
	const wrect_t* m_prcSuitEmpty, *m_prcSuitHelmetEmpty;
	hSprite m_hSuitFull, m_hSuitHelmetFull;
	const wrect_t* m_prcSuitFull, *m_prcSuitHelmetFull;
	float m_flArmour;
	float m_flDrawingArmour;
	float m_fFade;
	int m_iArmorType;
	int m_iHeight;		// width of the battery innards
	float m_flLastDrawingY;	// for other HUD elements.
};
