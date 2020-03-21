/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#pragma once

// debug macro
//#define RANDOM_SEED_CALIBRATION 1

class CBasePlayer;
class CWeaponBox;

const float MAX_NORMAL_BATTERY    = 100.0f;
const float MAX_DIST_RELOAD_SOUND = 512.0f;

#define ITEM_FLAG_NONE				0
#define ITEM_FLAG_SELECTONEMPTY     1
#define ITEM_FLAG_NOAUTORELOAD      2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY 4
#define ITEM_FLAG_LIMITINWORLD      8
#define ITEM_FLAG_EXHAUSTIBLE       16 // A player can totally exhaust their ammo supply and lose this weapon

#define WEAPON_IS_ONTARGET          0x40

// the maximum amount of ammo each weapon's clip can hold
#define WEAPON_NOCLIP               -1

#define LOUD_GUN_VOLUME             1000
#define NORMAL_GUN_VOLUME           600
#define QUIET_GUN_VOLUME            200

#define BRIGHT_GUN_FLASH            512
#define NORMAL_GUN_FLASH            256
#define DIM_GUN_FLASH               128

#define BIG_EXPLOSION_VOLUME        2048
#define NORMAL_EXPLOSION_VOLUME     1024
#define SMALL_EXPLOSION_VOLUME      512

#define WEAPON_ACTIVITY_VOLUME      64

// spawn flags
#define SF_DETONATE                 BIT(0) // Grenades flagged with this will be triggered when the owner calls detonateSatchelCharges

#define ARMOR_NONE		0.0f	// No armor
#define ARMOR_KEVLAR	1.0f	// Body vest only
#define ARMOR_VESTHELM	2.0f	// Vest and helmet

#ifndef CLIENT_DLL	// client.dll doesn't need these.
struct MULTIDAMAGE
{
	CBaseEntity *pEntity;
	float amount;
	int type;
};
#endif

#include "ammo.h"
#include "weapontype.h"
#include <list>

// something can place on your hand.
class CBaseWeapon
{
public:
	// avoid the complex memset();
	void* operator new(size_t size)
	{
		return calloc(1, size);
	}
	void operator delete(void* ptr)
	{
		free(ptr);
	}

protected:	// you can remove weapons only via its manager function.
	CBaseWeapon() {}
	virtual ~CBaseWeapon() {}

public:
	static	void			TheWeaponsThink	(void);
	static	CBaseWeapon*	Give(WeaponIdType iId, CBasePlayer* pPlayer = nullptr, int iClip = 0, unsigned bitsFlags = 0);

public:
	static std::list<CBaseWeapon*>	m_lstWeapons;

public:
	WeaponIdType	m_iId;
	const ItemInfo*	m_pItemInfo;
	const AmmoInfo*	m_pAmmoInfo;
	float			m_flNextPrimaryAttack;
	float			m_flNextSecondaryAttack;
	float			m_flTimeWeaponIdle;
	unsigned		m_bitsFlags;
	int				m_iClip;
	bool			m_bInReload;
	int				m_iShotsFired;
	float			m_flDecreaseShotsFired;
	bool			m_bDirection;
	float			m_flAccuracy;	// TODO: this should be remove later.
	float			m_flLastFire;
	AmmoIdType		m_iPrimaryAmmoType;			// "primary" ammo index into players m_rgAmmo[]
	AmmoIdType		m_iSecondaryAmmoType;		// "secondary" ammo index into players m_rgAmmo[]
	bool			m_bInZoom;

	struct	// this structure is for anim push and pop. it save & restore weapon state.
	{
		// on player.
		int		m_iSequence;
		float	m_flNextAttack;
		float	m_flEjectBrass;
		int		m_iShellModelIndex;

		// on weapon.
		float	m_flNextPrimaryAttack;
		float	m_flNextSecondaryAttack;
		float	m_flTimeWeaponIdle;

#ifdef CLIENT_DLL
		// for model. these will be applied on g_pViewEnt.
		float	m_flTimeAnimStarted;
		float	m_flFramerate;
		float	m_flFrame;
#endif
	}
	m_Stack;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	EntityHandle<CBasePlayer>	m_pPlayer;		// one of these two must be valid. or this weapon will be removed.
	EntityHandle<CWeaponBox>	m_pWeaponBox;
	int		m_iClientClip;
	int		m_iClientWeaponState;
#else
public:	// CL exclusive variables.
	CBasePlayer*	m_pPlayer;	// local pseudo-player
	bool			m_bServerInZoom;
	int				m_iServerShotsFired;
#endif

public:	// basic logic funcs
	virtual void	Think			(void);		// called by PlayerPreThink.
	virtual bool	AddToPlayer		(CBasePlayer* pPlayer);	// should only be called by CBasePlayer::AddPlayerItem();
	virtual bool	Deploy			(void) { return false; }	// called when attempting to take it out.
	virtual void	PostFrame		(void);		// called by PlayerPostThink.
	virtual void	PrimaryAttack	(void) {}	// IN_MOUSE1
	virtual void	SecondaryAttack	(void) {}	// IN_MOUSE2
	virtual void	WeaponIdle		(void) {}	// constantly called when nothing else to do.
	virtual bool	Reload			(void) { return false; }	// you know what it is, right?
	virtual bool	Melee			(void);		// quick knife.
	virtual bool	QuickThrow		(WeaponIdType iId) { return false; };	// quick grenade (maybe something else in the future?).
	virtual bool	Holster			(bool bTrial = false);		// called when attempting to put it off. bTrial means only testing whether weapon can be holster.
	virtual bool	Drop			(void **ppWeaponBoxReturned = nullptr);		// called when attempting to drop it on ground. ppWeaponBoxReturned is the CWeaponBox to be returned. (NOT avaliable on client side.)
	virtual bool	Kill			(void);		// called when attempting to remove it from your inventory.

#ifndef CLIENT_DLL
public:	// SV exclusive functions.
	virtual void	UpdateClientData(void);
	virtual void	Precache		(void) {}
#endif

public:	// util funcs
	inline	bool	IsDead			(void) { return !!(m_bitsFlags & WPNSTATE_DEAD); }
	virtual	float	GetMaxSpeed		(void) { return 260.0f; }
	virtual	bool	AddPrimaryAmmo	(int iCount);	// fill in clip first, then bpammo.
	inline	bool	IsPistol		(void) { return m_pItemInfo->m_iSlot == PISTOL_SLOT; }
	virtual	bool	DefaultDeploy	(const char* szViewModel, const char* szWeaponModel, int iAnim, const char* szAnimExt);
	virtual	void	SendWeaponAnim	(int iAnim, int iBody = 0, bool bSkipLocal = true);
	virtual	void	PlayEmptySound	(void);
	virtual	bool	DefaultReload	(int iClipSize, int iAnim, float fDelay);
	virtual	void	ReloadSound		(void);
	virtual void	PushAnim		(void);
	virtual void	PopAnim			(void);
	inline	bool	CanHolster		(void) { return Holster(true); }	// smells, looks and tastes like a duck...
	virtual	bool	CanDrop			(void) { return true; }
	virtual void	KickBack		(float up_base, float lateral_base, float up_modifier, float lateral_modifier, float up_max, float lateral_max, int direction_change);	// recoil
};




constexpr float USP_MAX_SPEED       = 250.0f;
constexpr float USP_DAMAGE          = 30.0f;
constexpr float USP_RANGE_MODIFER   = 0.79f;
constexpr float USP_RELOAD_TIME     = 2.66f;	// TODO: model needs to adjust.
constexpr float USP_FIRE_INTERVAL	= 0.15f;
constexpr float	USP_EFFECTIVE_RANGE = 4096.0f;
constexpr int	USP_PENETRATION		= 1;	// 1 means it can't penetrate anything.

enum usp_e
{
	USP_IDLE,
	USP_SHOOT1,
	USP_SHOOT2,
	USP_SHOOT3,
	USP_SHOOT_EMPTY,
	USP_RELOAD,
	USP_DRAW,
	USP_ATTACH_SILENCER,
	USP_UNSIL_IDLE,
	USP_UNSIL_SHOOT1,
	USP_UNSIL_SHOOT2,
	USP_UNSIL_SHOOT3,
	USP_UNSIL_SHOOT_EMPTY,
	USP_UNSIL_RELOAD,
	USP_UNSIL_DRAW,
	USP_DETACH_SILENCER,
};

class CUSP : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void) { return USP_MAX_SPEED; }

public:	// new functions
	void USPFire(float flSpread, float flCycleTime);
};

const float MP5N_MAX_SPEED     = 250.0f;
const float MP5N_DAMAGE        = 26.0f;
const float MP5N_RANGE_MODIFER = 0.84f;
const float MP5N_RELOAD_TIME   = 2.63f;

enum mp5n_e
{
	MP5N_IDLE1,
	MP5N_RELOAD,
	MP5N_DRAW,
	MP5N_SHOOT1,
	MP5N_SHOOT2,
	MP5N_SHOOT3,
};

const float SG552_MAX_SPEED      = 235.0f;
const float SG552_MAX_SPEED_ZOOM = 200.0f;
const float SG552_DAMAGE         = 33.0f;
const float SG552_RANGE_MODIFER  = 0.955f;
const float SG552_RELOAD_TIME    = 3.0f;

enum sg552_e
{
	SG552_IDLE1,
	SG552_RELOAD,
	SG552_DRAW,
	SG552_SHOOT1,
	SG552_SHOOT2,
	SG552_SHOOT3,
};

const float AK47_MAX_SPEED     = 221.0f;
const float AK47_DAMAGE        = 36.0f;
const float AK47_RANGE_MODIFER = 0.98f;
const float AK47_RELOAD_TIME   = 2.45f;

enum ak47_e
{
	AK47_IDLE1,
	AK47_RELOAD,
	AK47_DRAW,
	AK47_SHOOT1,
	AK47_SHOOT2,
	AK47_SHOOT3,
};

const float AUG_MAX_SPEED     = 240.0f;
const float AUG_DAMAGE        = 32.0f;
const float AUG_RANGE_MODIFER = 0.96f;
const float AUG_RELOAD_TIME   = 3.3f;

enum aug_e
{
	AUG_IDLE1,
	AUG_RELOAD,
	AUG_DRAW,
	AUG_SHOOT1,
	AUG_SHOOT2,
	AUG_SHOOT3,
};

constexpr float AWP_MAX_SPEED		= 210.0f;
constexpr float AWP_MAX_SPEED_ZOOM	= 150.0f;
constexpr float AWP_DAMAGE			= 115.0f;
constexpr float AWP_RANGE_MODIFER	= 0.99f;
constexpr float	AWP_RELOAD_TIME		= 2.48f;
constexpr float	AWP_DEPLOY_TIME		= 1.39f;
constexpr float AWP_FIRE_INTERVAL	= 1.5f;
constexpr float AWP_TIME_SHELL_EJ	= 0.85f;
constexpr int	AWP_PENETRATION		= 3;
constexpr float	AWP_EFFECTIVE_RANGE = 8192.0f;

enum awp_e
{
	AWP_IDLE,
	AWP_SHOOT1,
	AWP_SHOOT2,
	AWP_SHOOT3,
	AWP_RELOAD,
	AWP_DRAW,
};

class CAWP : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual float GetMaxSpeed		(void);

public:	// new funcs
	void AWPFire(float flSpread, float flCycleTime);
};

const float DEAGLE_MAX_SPEED     = 250.0f;
const float DEAGLE_DAMAGE        = 54.0f;
const float DEAGLE_RANGE_MODIFER = 0.81f;
const float DEAGLE_RELOAD_TIME   = 2.2f;

enum deagle_e
{
	DEAGLE_IDLE1,
	DEAGLE_SHOOT1,
	DEAGLE_SHOOT2,
	DEAGLE_SHOOT_EMPTY,
	DEAGLE_RELOAD,
	DEAGLE_DRAW,
};

const float FLASHBANG_MAX_SPEED        = 250.0f;
const float FLASHBANG_MAX_SPEED_SHIELD = 180.0f;

enum flashbang_e
{
	FLASHBANG_IDLE,
	FLASHBANG_PULLPIN,
	FLASHBANG_THROW,
	FLASHBANG_DRAW,
};

const float G3SG1_MAX_SPEED      = 210.0f;
const float G3SG1_MAX_SPEED_ZOOM = 150.0f;
const float G3SG1_DAMAGE         = 80.0f;
const float G3SG1_RANGE_MODIFER  = 0.98f;
const float G3SG1_RELOAD_TIME    = 3.5f;

enum g3sg1_e
{
	G3SG1_IDLE,
	G3SG1_SHOOT,
	G3SG1_SHOOT2,
	G3SG1_RELOAD,
	G3SG1_DRAW,
};

const float GLOCK18_MAX_SPEED     = 250.0f;
const float GLOCK18_DAMAGE        = 25.0f;
const float GLOCK18_RANGE_MODIFER = 0.75f;
const float GLOCK18_RELOAD_TIME   = 2.2f;

enum glock18_e
{
	GLOCK18_IDLE1,
	GLOCK18_IDLE2,
	GLOCK18_IDLE3,
	GLOCK18_SHOOT,
	GLOCK18_SHOOT2,
	GLOCK18_SHOOT3,
	GLOCK18_SHOOT_EMPTY,
	GLOCK18_RELOAD,
	GLOCK18_DRAW,
	GLOCK18_HOLSTER,
	GLOCK18_ADD_SILENCER,
	GLOCK18_DRAW2,
	GLOCK18_RELOAD2,
};

const float HEGRENADE_MAX_SPEED        = 250.0f;
const float HEGRENADE_MAX_SPEED_SHIELD = 180.0f;

enum hegrenade_e
{
	HEGRENADE_IDLE,
	HEGRENADE_PULLPIN,
	HEGRENADE_THROW,
	HEGRENADE_DRAW,
};

constexpr float KNIFE_BODYHIT_VOLUME   = 128.0f;
constexpr float KNIFE_WALLHIT_VOLUME   = 512.0f;
constexpr float KNIFE_MAX_SPEED        = 250.0f;
constexpr float KNIFE_MAX_SPEED_SHIELD = 180.0f;
constexpr float KNIFE_QUICK_SLASH_TIME = 0.939f;
constexpr float KNIFE_QUICK_SLASH_DMG  = 55.0f;

enum knife_e
{
	KNIFE_IDLE,
	KNIFE_QUICK_SLASH = 22,	// model from HLMW.
};

namespace BasicKnife
{
#ifndef CLIENT_DLL	// SV exclusive namespace.

	// player currently running this code.
	extern EntityHandle<CBasePlayer>	m_pPlayer;

	// the weapon which calling quick slash.
	extern CBaseWeapon* m_pWeapon;

	void Precache();
	bool Deploy(CBaseWeapon* pWeapon);
	void Swing();
#endif
};

const float M249_MAX_SPEED     = 220.0f;
const float M249_DAMAGE        = 32.0f;
const float M249_RANGE_MODIFER = 0.97f;
const float M249_RELOAD_TIME   = 4.7f;

enum m249_e
{
	M249_IDLE1,
	M249_SHOOT1,
	M249_SHOOT2,
	M249_RELOAD,
	M249_DRAW,
};

constexpr float KSG12_MAX_SPEED			= 230.0f;
constexpr float KSG12_DAMAGE			= 20.0f;
constexpr int	KSG12_PROJECTILE_COUNT	= 9;
constexpr float	KSG12_EFFECTIVE_RANGE	= 3000.0f;
constexpr float KSG12_FIRE_INTERVAL		= 0.9f;
constexpr float KSG12_SHELL_EJECT		= 0.66f;
constexpr float KSG12_TIME_START_RELOAD = 0.566f;
constexpr float KSG12_TIME_INSERT		= 0.5f;
constexpr float KSG12_TIME_ADD_AMMO		= 0.45f;
constexpr float KSG12_TIME_AFTER_RELOAD = 1.033f;
const	Vector	KSG12_CONE_VECTOR		= Vector(0.0675f, 0.0675f, 0.0f); // special shotgun spreads

enum ksg12_e
{
	KSG12_IDLE,
	KSG12_FIRE1,
	KSG12_FIRE2,
	KSG12_INSERT,
	KSG12_AFTER_RELOAD,
	KSG12_START_RELOAD,
	KSG12_DRAW,
	KSG12_HOLSTER,
};

class CKSG12 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:
	bool	m_bAllowNextEmptySound;
	float	m_flNextInsertAnim;
	float	m_flNextAddAmmo;
	bool	m_bSetForceStopReload;

	struct	// shotgun needs to expand these push-pop stuff a little bit.
	{
		float	m_flNextInsertAnim;
		float	m_flNextAddAmmo;
	}
	m_Stack2;

public:	// basic logic funcs
	virtual void	Think			(void);
	virtual bool	Deploy			(void);
	virtual void	PostFrame		(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual void	WeaponIdle		(void);
	virtual	bool	Reload			(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void) { return KSG12_MAX_SPEED; }
	virtual	void	PlayEmptySound	(void);
	virtual void	PushAnim		(void);
	virtual void	PopAnim			(void);
};

const float M4A1_MAX_SPEED         = 230.0f;
const float M4A1_DAMAGE            = 32.0f;
const float M4A1_DAMAGE_SIL        = 33.0f;
const float M4A1_RANGE_MODIFER     = 0.97f;
const float M4A1_RANGE_MODIFER_SIL = 0.95f;
const float M4A1_RELOAD_TIME       = 3.05f;

enum m4a1_e
{
	M4A1_IDLE,
	M4A1_SHOOT1,
	M4A1_SHOOT2,
	M4A1_SHOOT3,
	M4A1_RELOAD,
	M4A1_DRAW,
	M4A1_ATTACH_SILENCER,
	M4A1_UNSIL_IDLE,
	M4A1_UNSIL_SHOOT1,
	M4A1_UNSIL_SHOOT2,
	M4A1_UNSIL_SHOOT3,
	M4A1_UNSIL_RELOAD,
	M4A1_UNSIL_DRAW,
	M4A1_DETACH_SILENCER,
};

const float MAC10_MAX_SPEED     = 250.0f;
const float MAC10_DAMAGE        = 29.0f;
const float MAC10_RANGE_MODIFER = 0.82f;
const float MAC10_RELOAD_TIME   = 3.15f;

enum mac10_e
{
	MAC10_IDLE1,
	MAC10_RELOAD,
	MAC10_DRAW,
	MAC10_SHOOT1,
	MAC10_SHOOT2,
	MAC10_SHOOT3,
};

constexpr float ANACONDA_MAX_SPEED			= 250.0f;
constexpr float ANACONDA_DAMAGE				= 64.0f;
constexpr float ANACONDA_RANGE_MODIFER		= 0.8f;
constexpr float ANACONDA_RELOAD_TIME		= 2.68f;
constexpr float ANACONDA_DEPLOY_TIME		= 0.367f;	// a really quick-ready gun.
constexpr float ANACONDA_FIRE_INTERVAL		= 0.15f;
constexpr int	ANACONDA_PENETRATION		= 1;
constexpr float	ANACONDA_EFFECTIVE_RANGE	= 4096.0f;

enum anaconda_e
{
	ANACONDA_IDLE,
	ANACONDA_SHOOT1,
	ANACONDA_SHOOT2,
	ANACONDA_SHOOT3,
	ANACONDA_SHOOT_EMPTY,
	ANACONDA_RELOAD,
	ANACONDA_DRAW,
};

class CAnaconda : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive variables.
	float m_flShellRain;

public:	// we need some CL-exclusive VFX.
	virtual void	Think			(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual float GetMaxSpeed		(void) { return ANACONDA_MAX_SPEED; }

public:	// new funcs
	void AnacondaFire(float flSpread, float flCycleTime);
};

const float P90_MAX_SPEED     = 245.0f;
const float P90_DAMAGE        = 21.0f;
const float P90_RANGE_MODIFER = 0.885f;
const float P90_RELOAD_TIME   = 3.4f;

enum p90_e
{
	P90_IDLE1,
	P90_RELOAD,
	P90_DRAW,
	P90_SHOOT1,
	P90_SHOOT2,
	P90_SHOOT3,
};

const float SCOUT_MAX_SPEED      = 260.0f;
const float SCOUT_MAX_SPEED_ZOOM = 220.0f;
const float SCOUT_DAMAGE         = 75.0f;
const float SCOUT_RANGE_MODIFER  = 0.98f;
const float SCOUT_RELOAD_TIME    = 2.0f;

enum scout_e
{
	SCOUT_IDLE,
	SCOUT_SHOOT,
	SCOUT_SHOOT2,
	SCOUT_RELOAD,
	SCOUT_DRAW,
};

const float SMOKEGRENADE_MAX_SPEED        = 250.0f;
const float SMOKEGRENADE_MAX_SPEED_SHIELD = 180.0f;

enum smokegrenade_e
{
	SMOKEGRENADE_IDLE,
	SMOKEGRENADE_PINPULL,
	SMOKEGRENADE_THROW,
	SMOKEGRENADE_DRAW,
};

constexpr float MP7A1_MAX_SPEED			= 250.0f;
constexpr float MP7A1_DAMAGE			= 20.0f;
constexpr float MP7A1_RANGE_MODIFER		= 0.85f;
constexpr float MP7A1_RELOAD_TIME		= 2.1f;
constexpr float MP7A1_RPM				= 850.0f;
constexpr int	MP7A1_PENETRATION		= 1;
constexpr float	MP7A1_EFFECTIVE_RANGE	= 8192.0f;

enum mp7a1_e
{
	MP7A1_IDLE1,
	MP7A1_RELOAD,
	MP7A1_DRAW,
	MP7A1_SHOOT1,
	MP7A1_SHOOT2,
	MP7A1_SHOOT3,
};

class CMP7A1 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#endif

public:
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual float	GetMaxSpeed		(void) { return MP7A1_MAX_SPEED; }

public:	// new funcs
	void MP7A1Fire(float flSpread, float flCycleTime);
};

const float XM1014_MAX_SPEED   = 240.0f;
const float XM1014_DAMAGE      = 20.0f;
const Vector XM1014_CONE_VECTOR = Vector(0.0725, 0.0725, 0.0); // special shotgun spreads

enum xm1014_e
{
	XM1014_IDLE,
	XM1014_FIRE1,
	XM1014_FIRE2,
	XM1014_RELOAD,
	XM1014_PUMP,
	XM1014_START_RELOAD,
	XM1014_DRAW,
};

const float ELITE_MAX_SPEED     = 250.0f;
const float ELITE_RELOAD_TIME   = 4.5f;
const float ELITE_DAMAGE        = 36.0f;
const float ELITE_RANGE_MODIFER = 0.75f;

enum elite_e
{
	ELITE_IDLE,
	ELITE_IDLE_LEFTEMPTY,
	ELITE_SHOOTLEFT1,
	ELITE_SHOOTLEFT2,
	ELITE_SHOOTLEFT3,
	ELITE_SHOOTLEFT4,
	ELITE_SHOOTLEFT5,
	ELITE_SHOOTLEFTLAST,
	ELITE_SHOOTRIGHT1,
	ELITE_SHOOTRIGHT2,
	ELITE_SHOOTRIGHT3,
	ELITE_SHOOTRIGHT4,
	ELITE_SHOOTRIGHT5,
	ELITE_SHOOTRIGHTLAST,
	ELITE_RELOAD,
	ELITE_DRAW,
};

const float FIVESEVEN_MAX_SPEED     = 250.0f;
const float FIVESEVEN_DAMAGE        = 20.0f;
const float FIVESEVEN_RANGE_MODIFER = 0.885f;
const float FIVESEVEN_RELOAD_TIME   = 2.7f;

enum fiveseven_e
{
	FIVESEVEN_IDLE,
	FIVESEVEN_SHOOT1,
	FIVESEVEN_SHOOT2,
	FIVESEVEN_SHOOT_EMPTY,
	FIVESEVEN_RELOAD,
	FIVESEVEN_DRAW,
};

const float UMP45_MAX_SPEED     = 250.0f;
const float UMP45_DAMAGE        = 30.0f;
const float UMP45_RANGE_MODIFER = 0.82f;
const float UMP45_RELOAD_TIME   = 3.5f;

enum ump45_e
{
	UMP45_IDLE1,
	UMP45_RELOAD,
	UMP45_DRAW,
	UMP45_SHOOT1,
	UMP45_SHOOT2,
	UMP45_SHOOT3,
};

const float SG550_MAX_SPEED      = 210.0f;
const float SG550_MAX_SPEED_ZOOM = 150.0f;
const float SG550_DAMAGE         = 70.0f;
const float SG550_RANGE_MODIFER  = 0.98f;
const float SG550_RELOAD_TIME    = 3.35f;

enum sg550_e
{
	SG550_IDLE,
	SG550_SHOOT,
	SG550_SHOOT2,
	SG550_RELOAD,
	SG550_DRAW,
};

const float GALIL_MAX_SPEED     = 240.0f;
const float GALIL_DAMAGE        = 30.0f;
const float GALIL_RANGE_MODIFER = 0.98f;
const float GALIL_RELOAD_TIME   = 2.45f;

enum galil_e
{
	GALIL_IDLE1,
	GALIL_RELOAD,
	GALIL_DRAW,
	GALIL_SHOOT1,
	GALIL_SHOOT2,
	GALIL_SHOOT3,
};

const float CM901_FIRE_INTERVAL = 0.0857f;
const float CM901_EFFECTIVE_RANGE = 8000.0f;
const float CM901_PENETRATION = 0.2f;
const float CM901_MAX_SPEED = 240.0f;
const float CM901_DAMAGE = 30.0f;
const float CM901_RANGE_MODIFER = 0.98f;
const float CM901_RELOAD_TIME = 2.459;

enum cm901_e
{
	CM901_IDLE,
	CM901_RELOAD,
	CM901_DRAW,
	CM901_SHOOT1,
	CM901_SHOOT2,
	CM901_SHOOT3,
};

class CCM901 : public CBaseWeapon
{
#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void);
#endif

public:	// basic logic funcs
	virtual bool	Deploy(void);
	virtual void	PrimaryAttack(void);
	virtual void	SecondaryAttack(void);
	virtual	bool	Reload(void);
	virtual void	WeaponIdle(void);

public:	// util funcs
	virtual	float	GetMaxSpeed(void) { return CM901_MAX_SPEED; }

public:	// new functions
	void CM901Fire(float flSpread, float flCycleTime);
};

const float FAMAS_MAX_SPEED     = 240.0f;
const float FAMAS_RELOAD_TIME   = 3.3f;
const float FAMAS_DAMAGE        = 30.0f;
const float FAMAS_DAMAGE_BURST  = 34.0f;
const float FAMAS_RANGE_MODIFER = 0.96f;

enum famas_e
{
	FAMAS_IDLE1,
	FAMAS_RELOAD,
	FAMAS_DRAW,
	FAMAS_SHOOT1,
	FAMAS_SHOOT2,
	FAMAS_SHOOT3,
};

#ifndef CLIENT_DLL
extern short g_sModelIndexLaser;
extern short g_sModelIndexLaserDot;

extern short g_sModelIndexFireball;
extern short g_sModelIndexSmoke;
extern short g_sModelIndexWExplosion;
extern short g_sModelIndexBubbles;
extern short g_sModelIndexBloodDrop;
extern short g_sModelIndexBloodSpray;
extern short g_sModelIndexSmokePuff;
extern short g_sModelIndexFireball2;
extern short g_sModelIndexFireball3;
extern short g_sModelIndexFireball4;
extern short g_sModelIndexCTGhost;
extern short g_sModelIndexTGhost;
extern short g_sModelIndexC4Glow;

extern short g_sModelIndexRadio;
extern MULTIDAMAGE gMultiDamage;

void WeaponsPrecache();
void FindHullIntersection(const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity);
void AnnounceFlashInterval(float interval, float offset = 0);

void ClearMultiDamage();
void ApplyMultiDamage(entvars_t *pevInflictor, entvars_t *pevAttacker);
void AddMultiDamage(entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType);
void SpawnBlood(Vector vecSpot, int bloodColor, float flDamage);
int DamageDecal(CBaseEntity *pEntity, int bitsDamageType);
void DecalGunshot(TraceResult *pTrace, int iBulletType, bool ClientOnly, entvars_t *pShooter, bool bHitMetal);
void EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype, int entityIndex);
void UTIL_PrecacheOtherWeapon(WeaponIdType iId);
BOOL CanAttack(float attack_time, float curtime, BOOL isPredicted);
#endif
