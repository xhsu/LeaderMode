/*

Created Date: Mar 27 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#pragma once

class CUIBuyMenu : public CBaseHudElement
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float flTime) { return m_Baseboard.Draw(flTime); }
	virtual void Think(void) { m_Baseboard.Think(); }
	virtual void Reset(void) { m_Baseboard.OnNewRound(); }
	virtual void InitHUDData(void) { m_Baseboard.InitHUDData(); }
	virtual void Shutdown(void) { m_Baseboard.Shutdown(); }

public:
	CBasePanel	m_Baseboard;
};
