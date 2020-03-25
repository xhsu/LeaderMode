/*

Created Date: Mar 11 2020

*/

#pragma once

class CHudRoundTimer : public CBaseHudElement
{
public:
	int Init(void);
	int VidInit(void);
	void Reset(void);
	int Draw(float flTime);

public:
	void MsgFunc_RoundTime(int& iTime);

public:
	float m_flTimeEnd;
	int m_HUD_stopwatch;
	int m_HUD_colon;
	float m_fFade;
	float m_flNewPeriod;
	float m_flNextToggle;
	bool m_bPanicColor;
	int m_closestRight;
};
