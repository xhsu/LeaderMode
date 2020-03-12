/*

Created Date: Mar 11 2020

*/

#pragma once

class CHudFlashlight : public CBaseHUDElement
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Reset(void);

	void MsgFunc_Flashlight(bool bOn, int iBattery);
	void MsgFunc_FlashBat(int& iBattery);

private:
	hSprite m_hSprite1;
	hSprite m_hSprite2;
	hSprite m_hBeam;
	wrect_t* m_prc1;
	wrect_t* m_prc2;
	wrect_t* m_prcBeam;
	float m_flBat;
	int m_iBat;
	bool m_fOn;
	float m_fFade;
	int m_iWidth;
};
