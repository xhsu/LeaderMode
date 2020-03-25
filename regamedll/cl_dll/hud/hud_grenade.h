/*

Created Date: Mar 23 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Matoilet

*/

#pragma once

class CHudGrenade : public CBaseHudElement
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float flTime);
	virtual void Think(void) {}
	virtual void Reset(void) {}
	virtual void InitHUDData(void) {}
	virtual void Shutdown(void) {}

public:
	hSprite m_rghGrenadeIcons[EQP_COUNT];
	wrect_t m_rgrcGrenadeIcons[EQP_COUNT];
};
