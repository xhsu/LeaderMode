/*

Created Date: Mar 11 2020

*/

#pragma once

#define BLINK_OMEGA		4.0f
#define BLINK_PERIOD	(1.0f / BLINK_OMEGA)

class CHudAccountBalance : public CBaseHUDElement
{
public:
	int Init(void);
	int VidInit(void);
	void Reset(void);
	int Draw(float flTime);
	void Think(void);

public:
	void MsgFunc_Money(int& iNewMoney);
	void MsgFunc_BlinkAcct(int& iValue);

public:
	int m_HUD_dollar;
	int m_HUD_minus;
	int m_HUD_plus;

	int m_iAccount;
	int m_iAccountDelta;
	float m_flAlpha;
	bool m_bStartFade;
	bool m_bDeltaPositiveSign;
	float m_flBlinkTime;
	Vector m_vecBlinkColourDelta;

	int m_hNumberFont;
	int m_hSignFont;
};
