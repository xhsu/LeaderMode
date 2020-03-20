/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#define WEAPONLIST_GAP				5
#define WEAPONLIST_ACTIVE_WPN_OFS	15
#define WEAPONLIST_DRIFT_SPEED		15.0f

#define WEAPONLIST_EMPTY_SFX		"common/wpn_hudon.wav"
#define WEAPONLIST_WHEEL_SFX		"common/wpn_moveselect.wav"
#define WEAPONLIST_SELECT_SFX		"common/wpn_select.wav"

#define AMMOBAR_WIDTH	5

class CHudWeaponList : public CBaseHUDElement
{
public:
	enum Phase
	{
		MOVING_OUT,
		MOVING_IN,
	};

public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float flTime);
	virtual void Think(void) {}
	virtual void Reset(void);
	virtual void InitHUDData(void) {}
	virtual void Shutdown(void) {}

	void MsgFunc_SetSlot(WeaponIdType iId, int iSlot);

public:
	hSprite m_rghWeaponSprite[LAST_WEAPON];
	wrect_t	m_rgrcWeaponSprite[LAST_WEAPON];
	hSprite m_rghHighlightedWeaponSprite[LAST_WEAPON];
	wrect_t	m_rgrcHighlightedWeaponSprite[LAST_WEAPON];

	WeaponIdType m_rgiWeapons[MAX_ITEM_TYPES];
	Vector2D m_rgvecCurCoord[MAX_ITEM_TYPES];
	Vector2D m_rgvecDestCoord[MAX_ITEM_TYPES];
	hSprite* m_rgphList[MAX_ITEM_TYPES];
	wrect_t* m_rgprcList[MAX_ITEM_TYPES];

	float m_flAlpha;
	Phase m_iPhase;
	WeaponIdType m_iLastWpnId;
};
