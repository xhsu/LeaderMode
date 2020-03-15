/*

Created Date: Mar 11 2020

*/

#pragma once

#define ACCURACY_AIR				(1 << 0) // accuracy depends on FL_ONGROUND
#define ACCURACY_SPEED				(1 << 1)
#define ACCURACY_DUCK				(1 << 2) // more accurate when ducking
#define ACCURACY_MULTIPLY_BY_14		(1 << 3) // accuracy multiply to 1.4
#define ACCURACY_MULTIPLY_BY_14_2	(1 << 4) // accuracy multiply to 1.4

typedef struct
{
	unsigned char r, g, b, a;
}
RGBA;

class CHudAmmo : public CBaseHUDElement
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Think(void);
	void Reset(void);
	int DrawWList(float flTime);

	void CalculateCrosshairColor(void);
	void CalculateCrosshairDrawMode(void);
	void CalculateCrosshairSize(void);

	int DrawCrosshair(float flTime, int weaponid);
	int DrawCrosshairEx(float flTime, int weaponid, int iBarSize, float flCrosshairDistance, bool bAdditive, int r, int g, int b, int a);

	void Adjust_Crosshair(void);

	void MsgFunc_CurWeapon(int& iState, int& iId, int& iClip);
	void MsgFunc_HideWeapon(int& bits);
	void MsgFunc_Crosshair(bool bDrawn);

public:
	float m_fFade;
	RGBA m_rgba;
	WEAPON* m_pWeapon;
	int m_HUD_bucket0;
	int m_HUD_selection;
	hSprite m_hObserverCrosshair;
	wrect_t m_rcObserverCrosshair;
	int m_bObserverCrosshair;
	int m_iAmmoLastCheck;
	float m_flCrosshairDistance;
	int m_iAlpha;
	int m_R;
	int m_G;
	int m_B;
	int m_cvarR;
	int m_cvarG;
	int m_cvarB;
	int m_iCurrentCrosshair;
	bool m_bAdditive;
	int m_iCrosshairScaleBase;
	char m_szLastCrosshairColor[32];
	float m_flLastCalcTime;
};

extern int g_iShotsFired;
