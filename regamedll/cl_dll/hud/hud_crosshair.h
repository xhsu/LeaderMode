/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#define ACCURACY_AIR				(1 << 0) // accuracy depends on FL_ONGROUND
#define ACCURACY_SPEED				(1 << 1)
#define ACCURACY_DUCK				(1 << 2) // more accurate when ducking
#define ACCURACY_MULTIPLY_BY_14		(1 << 3) // accuracy multiply to 1.4

extern int g_iShotsFired;

class CHudCrosshair : public CBaseHudElement
{
public:
	hSprite m_hObserverCrosshair;
	wrect_t m_rcObserverCrosshair;
	hSprite	m_rghScopes[LAST_WEAPON];
	wrect_t	m_rgrcScopes[LAST_WEAPON];
	int m_iCurrentCrosshair;
	bool m_bAdditive;
	int m_iCrosshairScaleBase;
	char m_szLastCrosshairColor[32];
	float m_flLastCalcTime;
	int m_iAlpha;
	int m_R;
	int m_G;
	int m_B;
	int m_cvarR;
	int m_cvarG;
	int m_cvarB;
	int m_bObserverCrosshair;
	int m_iAmmoLastCheck;
	float m_flCrosshairDistance;

public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float flTime);
	virtual void Reset(void);

	void CalculateCrosshairColor(void);
	void CalculateCrosshairDrawMode(void);
	void CalculateCrosshairSize(void);
	int DrawCrosshair(float flTime, WeaponIdType weaponid);
	int DrawCrosshairEx(float flTime, WeaponIdType weaponid, int iBarSize, float flCrosshairDistance, bool bAdditive, int r, int g, int b, int a);
	void Adjust_Crosshair(void);

	void MsgFunc_Crosshair(bool bDrawn);
	void MsgFunc_HideWeapon(int& bits);
};

int GetWeaponAccuracyFlags(WeaponIdType iId);
