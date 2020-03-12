/*

Created Date: Mar 11 2020

*/

#pragma once

class CHudNightVision : public CBaseHUDElement
{
public:
	void Reset(void);
	int Init(void);
	int Draw(float flTime);

public:
	void MsgFunc_NVGToggle(bool bOn);

public:
	bool m_fOn;
	int m_iAlpha;
};
