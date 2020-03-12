/*

Created Date: Mar 12 2020

*/

#pragma once

class CHudProgressBar : public CBaseHUDElement
{
public:
	int Init(void);
	void Reset(void);
	int Draw(float flTime);

public:
	void MsgFunc_BarTime(int& val);
	void MsgFunc_BarTime2(int& total, float& elapsedPercent);

public:
	float m_tmEnd;
	float m_tmStart;
	float m_tmNewPeriod;
	float m_tmNewElapsed;
	int m_width;
	int m_height;
	int m_x0;
	int m_y0;
	int m_x1;
	int m_y1;
};
