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

class CHudWeaponList : public CBaseHudElement
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
	virtual void InitHUDData(void);
	virtual void Shutdown(void) {}

	void MsgFunc_SetSlot(WeaponIdType iId, int iSlot);

public:
	std::array<hSprite, LAST_WEAPON> m_rghWeaponSprite;
	std::array<wrect_t, LAST_WEAPON> m_rgrcWeaponSprite;
	std::array<hSprite, LAST_WEAPON> m_rghHighlightedWeaponSprite;
	std::array<wrect_t, LAST_WEAPON> m_rgrcHighlightedWeaponSprite;

	std::array<WeaponIdType, MAX_ITEM_TYPES> m_rgiWeapons;
	std::array<Vector2D, MAX_ITEM_TYPES> m_rgvecCurCoord;
	std::array<Vector2D, MAX_ITEM_TYPES> m_rgvecDestCoord;
	std::array<hSprite*, MAX_ITEM_TYPES> m_rgphList;
	std::array<wrect_t*, MAX_ITEM_TYPES> m_rgprcList;

	float m_flAlpha;
	Phase m_iPhase;
	WeaponIdType m_iLastWpnId;

	int m_hClipFont;
	int m_hAmmoFont;
	std::array<float, MAX_ITEM_TYPES> m_rgflFontAlpha;
	std::array<Vector2D, MAX_ITEM_TYPES> m_rgvecFontCoord;	// UNDONE
};
