/*

Created Date: Mar 26 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#pragma once

class CHudScoreboard : public CBaseHudElement
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float flTime);
	virtual void Think(void);
	virtual void Reset(void) { m_Baseboard.NewRound(); }
	virtual void InitHUDData(void) { m_Baseboard.InitHUDData(); }
	virtual void Shutdown(void) { m_Baseboard.Shutdown(); }

public:
	CBasePanel m_Baseboard;
};
