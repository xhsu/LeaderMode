/*

Created Date: Mar 11 2020

*/

#pragma once

#define MONEY_FADE_TIME		5.0f
#define MONEY_FASTFADE_TIME	(MONEY_FADE_TIME / 2.0f)

class CHudAccountBalance : public CBaseHUDElement
{
public:
	int Init(void);
	int VidInit(void);
	void Reset(void);
	int Draw(float flTime);

public:
	void MsgFunc_Money(int& iNewMoney, bool bShowDelta);
	void MsgFunc_BlinkAcct(int& iValue);

public:
	int m_iAccount;
	int m_HUD_dollar;
	int m_HUD_minus;
	int m_HUD_plus;
	int m_iAccountDelta;
	float m_fFade;
	float m_fFadeFast;
	bool m_bShowDelta;
	int m_iBlinkCount;
	float m_fBlinkTime;
};
