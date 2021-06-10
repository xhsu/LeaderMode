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

#include "game_shared/shared_util.h"

#pragma once

// debug macro
//#define RANDOM_SEED_CALIBRATION 1
//#define CHECKING_NEXT_PRIM_ATTACK_SYNC	1
#define CLIENT_PREDICT_PRIM_ATK	1
#define CLIENT_PREDICT_AIM	1

// util macro
#define PRECACHE_NECESSARY_FILES(x)	PRECACHE_MODEL(x##_VIEW_MODEL);	\
									PRECACHE_MODEL(x##_WORLD_MODEL);	\
									PRECACHE_SOUND(x##_FIRE_SFX)

#ifndef CLIENT_DLL
#define DECLARE_STANDARD_RESET_MODEL_FUNC(x)	void C##x::ResetModel(void)	\
												{	\
													m_pPlayer->pev->viewmodel = MAKE_STRING(x##_VIEW_MODEL);	\
													m_pPlayer->pev->weaponmodel = MAKE_STRING(x##_WORLD_MODEL);	\
												}
#else
#define DECLARE_STANDARD_RESET_MODEL_FUNC(x)	void C##x::ResetModel(void)	\
												{	\
													g_pViewEnt->model = gEngfuncs.CL_LoadModel(x##_VIEW_MODEL, &m_pPlayer->pev->viewmodel);	\
												}
#endif

class CBaseEntity;
class CBasePlayer;
class CWeaponBox;

using pcchar = const char* const;

constexpr float MAX_NORMAL_BATTERY    = 100.0f;
constexpr float MAX_DIST_RELOAD_SOUND = 512.0f;

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
#include "player_classes.h"
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
	CBaseWeapon() noexcept {}
	CBaseWeapon(const CBaseWeapon& s) = default;
	CBaseWeapon(CBaseWeapon&& s) = default;
	CBaseWeapon& operator=(const CBaseWeapon& s) = default;
	CBaseWeapon& operator=(CBaseWeapon&& s) = default;
	virtual ~CBaseWeapon() {}

public:
	static	void			TheWeaponsThink	(void);
	static	CBaseWeapon*	Give(WeaponIdType iId, CBasePlayer* pPlayer = nullptr, int iClip = 0, unsigned bitsFlags = 0);

public:
	static std::list<CBaseWeapon*>	m_lstWeapons;

public:
	WeaponIdType	m_iId					{ WEAPON_NONE };
	const WeaponInfo* m_pItemInfo			{ &g_rgWpnInfo[WEAPON_NONE] };
	const AmmoInfo* m_pAmmoInfo				{ &g_rgAmmoInfo[AMMO_NONE] };
	float			m_flNextPrimaryAttack	{ 0.0f };
	float			m_flNextSecondaryAttack	{ 0.0f };
	float			m_flTimeWeaponIdle		{ 0.0f };
	unsigned		m_bitsFlags				{ 0 };
	int				m_iClip					{ 0 };
	bool			m_bInReload				{ false };
	int				m_iShotsFired			{ 0 };
	float			m_flDecreaseShotsFired	{ 0.0f };
	bool			m_bDirection			{ false };
	float			m_flAccuracy			{ 0.0f };	// TODO: this should be remove later.
	float			m_flLastFire			{ 0.0f };
	AmmoIdType		m_iPrimaryAmmoType		{ AMMO_NONE };			// "primary" ammo index into players m_rgAmmo[]
	AmmoIdType		m_iSecondaryAmmoType	{ AMMO_NONE };		// "secondary" ammo index into players m_rgAmmo[]
	bool			m_bInZoom				{ false };
	RoleTypes		m_iVariation			{ Role_UNASSIGNED };	// weapons suppose to variegate accroading to their owner.
	bool			m_bDelayRecovery		{ false };

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
	m_Stack
	{ 0, 0.0f, 0.0f, 0,
		0.0f, 0.0f, 0.0f
#ifndef CLIENT_DLL
	};
#else
		, 0.0f, 0.0f, 0.0f };
#endif

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	EntityHandle<CBasePlayer>	m_pPlayer;		// one of these two must be valid. or this weapon will be removed.
	EntityHandle<CWeaponBox>	m_pWeaponBox;
	int		m_iClientClip		{ 0 };
	int		m_iClientWeaponState{ 0 };
#else
public:	// CL exclusive variables.
	CBasePlayer*	m_pPlayer		{ nullptr };	// local pseudo-player
	float			m_flBlockCheck	{ 0.0f };
	Vector			m_vecBlockOffset{ g_vecZero };
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
	virtual bool	QuickThrowStart	(EquipmentIdType iId);	// quick grenade (maybe something else in the future?).
	virtual bool	QuickThrowRelease(void);	// triggered when +qtg button released.
	virtual bool	AlterAct		(void) { return false; }	// special use. for instance, XM8 "morph".
	virtual bool	HolsterStart	(void);		// play holster anim, initialize holstering.
	virtual void	Holstered		(void);		// majorlly reset the weapon data. no visual stuff.
	virtual	void	DashStart		(void) { m_bitsFlags |= WPNSTATE_DASHING; }		// called when system thinks it's time to dash.
	virtual void	DashEnd			(void) { m_bitsFlags &= ~WPNSTATE_DASHING; }		// called when system thinks it's time to stop dash.
	virtual bool	Drop			(void **ppWeaponBoxReturned = nullptr);		// called when attempting to drop it on ground. ppWeaponBoxReturned is the CWeaponBox to be returned. (NOT avaliable on client side.)
	virtual bool	Kill			(void);		// called when attempting to remove it from your inventory.

#ifndef CLIENT_DLL
public:	// SV exclusive functions.
	virtual void	UpdateClientData(void);
	virtual void	Precache		(void) {}
#else
public:	// CL xclusive functions.
	virtual	bool	UsingInvertedVMDL(void)		{ return true; }	// by default, original CS/CZ vmdls are inverted displaying.
	virtual int		CalcBodyParam(void)			{ return 0; }		// allow user to varient weapon body.
	virtual	void	UpdateBobParameters(void);						// in which you may set some bob parameters.
#endif

public:	// basic API and behaviour for weapons.
	bool	DefaultDeploy	(const char* szViewModel, const char* szWeaponModel, int iAnim, const char* szAnimExt, float flDeployTime = 0.75f);
	void	DefaultIdle		(int iDashingAnim, int iIdleAnim = 0, float flDashLoop = 20.0f);
	bool	DefaultReload	(int iClipSize, int iAnim, float flTotalDelay, float flSoftDelay = 0.5f);
	bool	DefaultHolster	(int iHolsterAnim, float flHolsterDelay);
	void	DefaultSteelSight(const Vector& vecOfs, int iFOV, float flDriftingSpeed = 10.0f, float flNextSecondaryAttack = 0.3f);
	void	DefaultScopeSight(const Vector& vecOfs, int iFOV, float flEnterScopeDelay = 0.25f, float flFadeFromBlack = 5.0f, float flDriftingSpeed = 10.0f, float flNextSecondaryAttack = 0.3f);
	void	DefaultDashStart(int iEnterAnim, float flEnterTime);
	void	DefaultDashEnd	(int iEnterAnim, float flEnterTime, int iExitAnim, float flExitTime);
	bool	DefaultSetLHand	(bool bAppear, int iLHandUpAnim, float flLHandUpTime, int iLHandDownAnim, float flLHandDownTime);
	void	DefaultBlock	(int iEnterAnim, float flEnterTime, int iExitAnim, float flExitTime);
	float	DefaultSpread	(float flBaseline, float flAimingMul, float flDuckingMul, float flWalkingMul, float flJumpingMul);

public:	// util funcs
	inline	bool	IsDead			(void) { return !!(m_bitsFlags & WPNSTATE_DEAD); }
	virtual	float	GetMaxSpeed		(void) { return 260.0f; }
	virtual	bool	AddPrimaryAmmo	(int iCount);	// fill in clip first, then bpammo.
	inline	bool	IsPistol		(void) { return m_pItemInfo->m_iSlot == PISTOL_SLOT; }
	virtual	void	SendWeaponAnim	(int iAnim, bool bSkipLocal = true);
	virtual	void	PlayEmptySound	(void);
	virtual	void	ReloadSound		(void);
	virtual void	PushAnim		(void);
	virtual void	PopAnim			(void);
	virtual	bool	CanHolster		(void);	// smells, looks and tastes like a duck...
	virtual	bool	CanDrop			(void) { return true; }
	virtual void	KickBack		(float up_base, float lateral_base, float up_modifier, float lateral_modifier, float up_max, float lateral_max, int direction_change);	// recoil
	virtual void	ResetModel		(void) { }	// used after Melee() and QuickThrowRelease().
	virtual bool	SetVariation	(RoleTypes iType) { m_iVariation = iType; return true; }
	virtual bool	SetLeftHand		(bool bAppear) { return false; }
	virtual void	PlayBlockAnim	(void) { }
	virtual float	GetSpread		(void) { return 0.0f; }
};

// Declare detectors
GENERATE_HAS_MEMBER(ACCURACY_BASELINE);

// General template.
template <class CWpn>
class CBaseWeaponTemplate : public CBaseWeapon
{
protected:
	using BaseClass = CBaseWeaponTemplate<CWpn>;

public:	// basic logic funcs
	bool	Deploy		(void) override	{ m_flAccuracy = CWpn::ACCURACY_BASELINE; m_iShotsFired = 0; return DefaultDeploy(CWpn::VIEW_MODEL, CWpn::WORLD_MODEL, (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? CWpn::DRAW_FIRST : CWpn::DEPLOY, CWpn::POSTURE, (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? CWpn::DRAW_FIRST_TIME : CWpn::DEPLOY_TIME); }
	void	WeaponIdle	(void) override { return DefaultIdle(CWpn::DASHING); }
	bool	HolsterStart(void) override	{ return DefaultHolster(CWpn::HOLSTER, CWpn::HOLSTER_TIME); }
	void	DashStart	(void) override { return DefaultDashStart(CWpn::DASH_ENTER, CWpn::DASH_ENTER_TIME); }
	void	DashEnd		(void) override { return DefaultDashEnd(CWpn::DASH_ENTER, CWpn::DASH_ENTER_TIME, CWpn::DASH_EXIT, CWpn::DASH_EXIT_TIME); }

public:
	float	GetMaxSpeed		(void) override { return CWpn::MAX_SPEED; }
	void	ResetModel		(void) override;
	bool	SetLeftHand		(bool bAppear) override { return DefaultSetLHand(bAppear, CWpn::LHAND_UP, CWpn::LHAND_UP_TIME, CWpn::LHAND_DOWN, CWpn::LHAND_DOWN_TIME); }
	void	PlayBlockAnim	(void) override { return DefaultBlock(CWpn::BLOCK_UP, CWpn::BLOCK_UP_TIME, CWpn::BLOCK_DOWN, CWpn::BLOCK_DOWN_TIME); }
};



#define USP_VIEW_MODEL		"models/weapons/v_usp.mdl"
#define USP_WORLD_MODEL		"models/weapons/w_usp.mdl"
#define USP_FIRE_SFX		"weapons/usp/usp_fire.wav"

constexpr float USP_MAX_SPEED       = 250.0f;
constexpr float USP_DAMAGE          = 32;
constexpr float USP_RANGE_MODIFER   = 1.187260896;	// 80% damage @650 inches.
constexpr float USP_DEPLOY_TIME		= 0.34f;
constexpr float USP_RELOAD_TIME		= 1.87f;
constexpr float USP_FIRE_INTERVAL	= 0.15f;
constexpr float	USP_EFFECTIVE_RANGE = 4096.0f;
constexpr int	USP_PENETRATION		= 1;	// 1 means it can't penetrate anything.
constexpr float	USP_SPREAD_BASELINE	= 1.2f;

enum usp_e
{
	USP_IDLE,
	USP_SHOOT1,
	USP_SHOOT2,
	USP_SHOOT3,
	USP_SHOOT_EMPTY,
	USP_RELOAD,
	USP_DRAW,
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
	virtual void	PrimaryAttack	(void) { return USPFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void) { return USP_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new functions
	void USPFire(float flSpread, float flCycleTime = USP_FIRE_INTERVAL);
};

#define MP5N_VIEW_MODEL		"models/weapons/v_mp5.mdl"
#define MP5N_WORLD_MODEL	"models/weapons/w_mp5.mdl"
#define MP5N_FIRE_SFX		"weapons/mp5/mp5_fire.wav"

constexpr float MP5N_MAX_SPEED     = 250.0f;
constexpr float MP5N_DAMAGE        = 26.0f;
constexpr float MP5N_RANGE_MODIFER = 1.124618621;	// 80% damage @950 inches.
constexpr float MP5N_RELOAD_TIME   = 2.63f;

enum mp5n_e
{
	MP5N_IDLE1,
	MP5N_RELOAD,
	MP5N_DRAW,
	MP5N_SHOOT1,
	MP5N_SHOOT2,
	MP5N_SHOOT3,
};

#define SCARH_VIEW_MODEL	"models/weapons/v_scarh.mdl"
#define SCARH_WORLD_MODEL	"models/weapons/w_scarl.mdl"
#define SCARH_FIRE_SFX		"weapons/SCARH/mk17_shoot.wav"

class CSCARH : public CBaseWeaponTemplate<CSCARH>
{
public:	// Constants / Database
	enum scarh_anim_e
	{
		IDLE,
		SHOOT1,
		SHOOT2,
		SHOOT3,
		SHOOT_LAST,
		SHOOT_ATTACHMENTS,
		M870MCS_PUMP,
		RELOAD,
		RELOAD_EMPTY,
		EGLM_RELOAD,
		XM26_RELOAD,
		XM26_RELOAD_EMPTY,
		M870MCS_RELOAD_START,
		M870MCS_RELOAD_FIRST_INSERT,
		M870MCS_RELOAD_INSERT,
		M870MCS_RELOAD_END,
		M870MCS_RELOAD_END_EMPTY,
		DRAW_FIRST,
		DEPLOY,
		JUMP,
		CHECK_MAGAZINE,
		SWITCH_SELECTOR,
		HOLSTER,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_UP,
		LHAND_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
	};

	static constexpr pcchar	VIEW_MODEL			= "models/weapons/v_scarh.mdl";
	static constexpr pcchar	WORLD_MODEL			= "models/weapons/w_scarl.mdl";
	static constexpr pcchar	FIRE_SFX			= "weapons/SCARH/mk17_shoot.wav";
	static constexpr pcchar POSTURE				= "mp5";
	static constexpr float	MAX_SPEED			= 235.0f;
	static constexpr float	DAMAGE				= 54;
	static constexpr float	RANGE_MODIFER		= 1.057371263;	// 80% damage @2000 inches.
	static constexpr float	DEPLOY_TIME			= 0.97F;
	static constexpr float	DRAW_FIRST_TIME		= 2.61F;
	static constexpr float	RELOAD_TIME			= 2.6f;
	static constexpr float	RELOAD_EMPTY_TIME	= 2.9f;
	static constexpr float	CHECK_MAGAZINE_TIME	= 3.06F;
	static constexpr float	HOLSTER_TIME		= 0.74F;
	static constexpr float	DASH_ENTER_TIME		= 0.485F;
	static constexpr float	DASH_EXIT_TIME		= 0.485F;
	static constexpr float	BLOCK_UP_TIME		= 0.3333f;
	static constexpr float	BLOCK_DOWN_TIME		= 0.3939f;
	static constexpr float	LHAND_UP_TIME		= 0.5526f;
	static constexpr float	LHAND_DOWN_TIME		= 0.4210f;
	static constexpr float	RPM					= 550.0f;
	static constexpr int	PENETRATION			= 2;
	static constexpr float	EFFECTIVE_RANGE		= 8192.0f;
	static constexpr int	GUN_VOLUME			= NORMAL_GUN_VOLUME;
	static constexpr float	SPREAD_BASELINE		= 0.15f;
	static constexpr float	ACCURACY_BASELINE	= 0.25f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual void	PrimaryAttack	(void) { return SCARHFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void SCARHFire(float flSpread, float flCycleTime = (60.0f / RPM));
};

#define AK47_VIEW_MODEL		"models/weapons/v_ak47.mdl"
#define AK47_WORLD_MODEL	"models/weapons/w_ak47.mdl"
#define AK47_FIRE_SFX		"weapons/ak47/ak47_fire.wav"

constexpr float AK47_MAX_SPEED			= 221.0f;
constexpr float AK47_DAMAGE				= 48;
constexpr float AK47_RANGE_MODIFER		= 1.077217345;	// 80% damage @1500 inches.
constexpr float AK47_RELOAD_TIME		= 2.44f;
constexpr float AK47_RPM				= 600.0f;
constexpr float	AK47_EFFECTIVE_RANGE	= 8192.0f;
constexpr int	AK47_PENETRATION		= 2;
constexpr float	AK47_SPREAD_BASELINE	= 0.4f;

enum ak47_e
{
	AK47_IDLE1,
	AK47_RELOAD,
	AK47_DRAW,
	AK47_SHOOT1,
	AK47_SHOOT2,
	AK47_SHOOT3,
};

class CAK47 : public CBaseWeapon
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
	virtual void	PrimaryAttack	(void) { return AK47Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void) { return AK47_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new functions
	void AK47Fire(float flSpread, float flCycleTime = (60.0f / AK47_RPM));
};

#define XM8_VIEW_MODEL	"models/weapons/v_xm8.mdl"
#define XM8_WORLD_MODEL	"models/weapons/w_xm8.mdl"
#define XM8_FIRE_SFX	"weapons/xm8/xm8_shoot.wav"

class CXM8 : public CBaseWeaponTemplate<CXM8>
{
public:	// Constants / Database
	enum xm8_anim_e
	{
		IDLE = 0,
		FIRE,
		FIRE_LAST,
		FIRE_AIM,
		FIRE_AIM_LAST,
		RELOAD,
		RELOAD_EMPTY,
		DRAW_FIRST,
		DEPLOY,
		HOLSTER,
		CHECK_MAGAZINE,
		SELECTOR_SEMI,
		SELECTOR_AUTO,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_DOWN,
		LHAND_UP,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
		SWITCH_TO_SHARPSHOOTER,
		SWITCH_TO_CARBINE
	};

	static constexpr pcchar	VIEW_MODEL				= "models/weapons/v_xm8.mdl";
	static constexpr pcchar	WORLD_MODEL				= "models/weapons/w_xm8.mdl";
	static constexpr pcchar	FIRE_SFX				= "weapons/xm8/xm8_shoot.wav";
	static constexpr pcchar POSTURE					= "carbine";
	static constexpr float	MAX_SPEED				= 240.0f;
	static constexpr float	MAX_SPEED_ZOOM			= 150.0f;
	static constexpr float	DAMAGE					= 32.0f;
	static constexpr float	RANGE_MODIFER			= 1.057371263;	// 80% damage @2000 inches
	static constexpr float	RELOAD_TIME				= 2.33f;
	static constexpr float	RELOAD_EMPTY_TIME		= 3.03f;
	static constexpr float	DRAW_FIRST_TIME			= 1.3F;
	static constexpr float	DEPLOY_TIME				= 0.7F;
	static constexpr float	HOLSTER_TIME			= 0.7F;
	static constexpr float	CHECKMAG_TIME			= 2.2667F;
	static constexpr float	DASH_ENTER_TIME			= 0.8F;
	static constexpr float	DASH_EXIT_TIME			= 0.533F;
	static constexpr float	TO_SHARPSHOOTER_TIME	= 8.8F;
	static constexpr float	TO_CARBIN_TIME			= 8.8F;
	static constexpr float	BLOCK_UP_TIME			= 0.5333f;
	static constexpr float	BLOCK_DOWN_TIME			= 0.5333f;
	static constexpr float	LHAND_UP_TIME			= 0.5333f;
	static constexpr float	LHAND_DOWN_TIME			= 0.5333f;
	static constexpr float	RPM						= 800.0f;
	static constexpr int	PENETRATION				= 2;
	static constexpr float	EFFECTIVE_RANGE			= 8192.0f;
	static constexpr int	GUN_VOLUME				= NORMAL_GUN_VOLUME;
	static constexpr float	SPREAD_BASELINE			= 0.1f;
	static constexpr float	ACCURACY_BASELINE		= 0.25f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual void	PostFrame		(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual bool	AlterAct		(void);

public:	// util funcs
	virtual float	GetMaxSpeed		(void);
	virtual float	GetSpread		(void);

public:	// new functions
	void XM8Fire(float flSpread, float flCycleTime = (60.0f / RPM));
};

#define AWP_VIEW_MODEL	"models/weapons/v_awp.mdl"
#define AWP_WORLD_MODEL	"models/weapons/w_awp.mdl"
#define AWP_FIRE_SFX	"weapons/l115a3/l115a3_fire.wav"

class CAWP : public CBaseWeaponTemplate<CAWP>
{
public:	// Constants / Database
	enum awp_anim_e
	{
		IDLE = 0,
		SHOOT_REC,
		RECHAMBER,
		SHOOT_LAST,
		RELOAD,
		RELOAD_EMPTY,
		DEPLOY,
		DRAW_FIRST,
		HOLSTER,
		CHECK_MAGAZINE,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_UP,
		LHAND_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT
	};

	static constexpr pcchar	VIEW_MODEL			= "models/weapons/v_awp.mdl";
	static constexpr pcchar	WORLD_MODEL			= "models/weapons/w_awp.mdl";
	static constexpr pcchar	FIRE_SFX			= "weapons/l115a3/l115a3_fire.wav";
	static constexpr pcchar POSTURE				= "rifle";
	static constexpr float	MAX_SPEED			= 210.0f;
	static constexpr float	MAX_SPEED_ZOOM		= 150.0f;
	static constexpr float	DAMAGE				= 125.0f;
	static constexpr float	RANGE_MODIFER		= 1.047585759;	// 80% damage @2400 inches.
	static constexpr float	FIRE_INTERVAL		= 1.5f;
	static constexpr float	FIRE_LAST_INV		= 0.5666f;
	static constexpr float	TIME_SHELL_EJ		= 0.666F;
	static constexpr float	TIME_RECHAMBER		= 1.2F;
	static constexpr float	TIME_REC_SHELL_EJ	= 0.3667F;
	static constexpr float	RELOAD_TIME			= 3.566F;
	static constexpr float	RELOAD_EMPTY_TIME	= 4.5F;
	static constexpr float	RELOAD_EMPTY_SHELL	= 0.4333f;
	static constexpr float	DEPLOY_TIME			= 0.733F;
	static constexpr float	DRAW_FIRST_TIME		= 1.533F;
	static constexpr float	HOLSTER_TIME		= 0.7333F;
	static constexpr float	CHECKMAG_TIME		= 2.0333F;
	static constexpr float	BLOCK_UP_TIME		= 0.5333F;
	static constexpr float	BLOCK_DOWN_TIME		= 0.5333F;
	static constexpr float	LHAND_UP_TIME		= 0.7F;
	static constexpr float	LHAND_DOWN_TIME		= 0.7F;
	static constexpr float	DASH_ENTER_TIME		= 0.4667F;
	static constexpr float	DASH_EXIT_TIME		= 0.4667F;
	static constexpr int	PENETRATION			= 3;
	static constexpr float	EFFECTIVE_RANGE		= 8192.0f;
	static constexpr int	GUN_VOLUME			= BIG_EXPLOSION_VOLUME;
	static constexpr float	SPREAD_BASELINE		= 0.001f;
	static constexpr float	ACCURACY_BASELINE	= 0;	// Dummy, unused.

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void);
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam(void);
#endif

	// Pin static anims.
	static constexpr int BITS_PIN_UNINVOLVED_ANIM = (1 << IDLE) |
													(1 << DEPLOY) |
													(1 << RELOAD) |	// normal reload could not bypass this rechamber flag.
													(1 << HOLSTER) |
													(1 << CHECK_MAGAZINE) |
													(1 << LHAND_DOWN) | (1 << LHAND_UP) |
													(1 << BLOCK_DOWN) | (1 << BLOCK_UP) |
													(1 << DASH_ENTER) | (1 << DASHING) | (1 << DASH_EXIT);

	// Rechamber acceptable anims.
	static constexpr int BITS_RECHAMBER_ANIM = (1 << RECHAMBER) | (1 << SHOOT_REC) | (1 << RELOAD_EMPTY);

public: // shared new vars.
	float m_flTimeChamberCleared;

public:	// basic logic funcs
	virtual void	Think			(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void)	{ return DefaultScopeSight(Vector(-6.2f, -2, 1.1f), 25); }
	virtual bool	Reload			(void);
	virtual bool	HolsterStart	(void);

public:	// util funcs
	virtual float	GetMaxSpeed		(void);
	virtual float	GetSpread		(void);

public:	// new funcs
	void AWPFire(float flSpread, float flCycleTime = FIRE_INTERVAL);
};

#define DEagle_VIEW_MODEL	"models/weapons/v_deagle.mdl"
#define DEagle_WORLD_MODEL	"models/weapons/w_deagle.mdl"
#define DEagle_FIRE_SFX		"weapons/deagle/deagle_fire.wav"

class CDEagle : public CBaseWeaponTemplate<CDEagle>
{
public:	// Constants / Database
	enum deagle_anim_e
	{
		IDLE = 0,
		SHOOT,
		SHOOT_EMPTY,
		RELOAD,
		RELOAD_EMPTY,
		DEPLOY,
		DRAW_FIRST,
		HOLSTER,
		CHECK_MAGAZINE,
		LHAND_DOWN,
		LHAND_UP,
		BLOCK_UP,
		BLOCK_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
		SH_RELOAD,
		SH_RELOAD_EMPTY,
		SH_DASH_ENTER,
		SH_DASHING,
		SH_DASH_EXIT,
	};

	static constexpr pcchar	VIEW_MODEL			= "models/weapons/v_deagle.mdl";
	static constexpr pcchar	WORLD_MODEL			= "models/weapons/w_deagle.mdl";
	static constexpr pcchar	FIRE_SFX			= "weapons/deagle/deagle_fire.wav";
	static constexpr pcchar	POSTURE				= "onehanded";
	static constexpr float	MAX_SPEED			= 245.0f;
	static constexpr float	DAMAGE				= 60;
	static constexpr float	RANGE_MODIFER		= 1.106751211;	// 80% damage @1100 inches.
	static constexpr float	RELOAD_TIME			= 1.97f;
	static constexpr float	RELOAD_EMPTY_TIME	= 2.02f;
	static constexpr float	DEPLOY_TIME			= 0.7f;
	static constexpr float	DRAW_FIRST_TIME		= 1.8f;
	static constexpr float	HOLSTER_TIME		= 0.7f;
	static constexpr float	CHECKMAG_TIME		= 2.32f;
	static constexpr float	BLOCK_UP_TIME		= 0.5333F;
	static constexpr float	BLOCK_DOWN_TIME		= 0.5333F;
	static constexpr float	LHAND_UP_TIME		= 0.6666F;
	static constexpr float	LHAND_DOWN_TIME		= 0.6666F;
	static constexpr float	DASH_ENTER_TIME		= 0.8667f;
	static constexpr float	DASH_EXIT_TIME		= 0.3667f;
	static constexpr float	SH_RELOAD_TIME		= 2.2333f;
	static constexpr float	SH_RELOAD_EMPTY_TIME= 2.7f;
	static constexpr float	SH_DASH_ENTER_TIME	= 0.7f;
	static constexpr float	SH_DASH_EXIT_TIME	= 0.7f;
	static constexpr float	FIRE_INTERVAL		= 0.225f;
	static constexpr int	PENETRATION			= 2;
	static constexpr float	EFFECTIVE_RANGE		= 4096.0f;
	static constexpr int	GUN_VOLUME			= BIG_EXPLOSION_VOLUME;
	static constexpr float	SPREAD_BASELINE		= 4;
	static constexpr float	ACCURACY_BASELINE	= 0.9f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam	(void);
#endif

	// Slide stop available anims.
	static constexpr int BITS_SLIDE_STOP_ANIM =	(1 << IDLE) |
												(1 << DEPLOY) |
												(1 << HOLSTER) |
												(1 << CHECK_MAGAZINE) |
												(1 << LHAND_DOWN) | (1 << LHAND_UP) |
												(1 << BLOCK_DOWN) | (1 << BLOCK_UP) |
												(1 << DASH_ENTER) | (1 << DASHING) | (1 << DASH_EXIT) |
												(1 << SH_DASH_ENTER) | (1 << SH_DASHING) | (1 << SH_DASH_EXIT);

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return DEagleFire(GetSpread()); }
	virtual void	SecondaryAttack	(void)	{ return DefaultSteelSight(Vector(-1.905f, -2, 1.1f), 85); }
	virtual bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void DEagleFire(float flSpread, float flCycleTime = FIRE_INTERVAL);
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

#define SVD_VIEW_MODEL	"models/weapons/v_svd.mdl"
#define SVD_WORLD_MODEL	"models/weapons/w_svd.mdl"
#define SVD_FIRE_SFX	"weapons/svd/svd_fire.wav"

constexpr float SVD_MAX_SPEED		= 210.0f;
constexpr float SVD_MAX_SPEED_ZOOM	= 150.0f;
constexpr float SVD_DAMAGE			= 115;
constexpr float SVD_RANGE_MODIFER	= 1.077137738;	// 70% damage @2400 inches.
constexpr float SVD_DEPLOY_TIME		= 1.13f;
constexpr float SVD_RELOAD_TIME		= 3.2f;
constexpr float SVD_FIRE_INTERVAL	= 0.25f;
constexpr int	SVD_PENETRATION		= 3;
constexpr float	SVD_EFFECTIVE_RANGE	= 8192.0f;
constexpr float	SVD_SPREAD_BASELINE = 0.0251f;

enum svd_e
{
	SVD_IDLE,
	SVD_SHOOT,
	SVD_SHOOT2,
	SVD_RELOAD,
	SVD_DRAW,
};

class CSVD : public CBaseWeapon
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
	virtual void	PrimaryAttack	(void)	{ return SVDFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void);
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void)	{ return DefaultSpread(SVD_SPREAD_BASELINE, 0.25f, 0.75f, 2.0f, 5.0f); }

public:	// new functions
	void SVDFire(float flSpread, float flCycleTime = SVD_FIRE_INTERVAL);
};

#define G18C_VIEW_MODEL		"models/weapons/v_glock18.mdl"
#define G18C_WORLD_MODEL	"models/w_glock18.mdl"	// FIXME
#define G18C_FIRE_SFX		"weapons/glock18/glock18_fire.wav"

constexpr float GLOCK18_MAX_SPEED		= 245.0f;
constexpr float GLOCK18_DAMAGE			= 17;
constexpr float GLOCK18_RANGE_MODIFER	= 1.187260896;	// 80% damage @650 inches.
constexpr float GLOCK18_DEPLOY_TIME		= 0.34f;
constexpr float GLOCK18_RELOAD_TIME		= 1.87f;
constexpr float GLOCK18_RPM				= 1200.0f;
constexpr int	GLOCK18_PENETRATION		= 1;
constexpr float	GLOCK18_EFFECTIVE_RANGE	= 4096.0f;
constexpr float	GLOCK18_SPREAD_BASELINE = 0.33f;	// don't mul it my 10 like other pistols. IT'S A DE FACTO SMG.

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

class CG18C : public CBaseWeapon
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
	virtual void	PrimaryAttack	(void)	{ return GLOCK18Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual	float	GetMaxSpeed		(void)	{ return GLOCK18_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new functions
	void GLOCK18Fire(float flSpread, float flCycleTime = 60.0f / GLOCK18_RPM);
};

#define THROWABLE_VIEW_MODEL	"models/weapons/v_throwable.mdl"

constexpr float TIME_GR_IDLE_LOOP			= (11.0f / 3.0f);
constexpr float TIME_GR_QT_READY			= 0.86f;
constexpr float TIME_GR_QT_READY_2			= 0.86f;
constexpr float TIME_FB_QT_READY			= 0.96f;
constexpr float TIME_SG_QT_READY			= 0.96f;
constexpr float TIME_QT_THROWING_FAR		= 0.78f;
constexpr float TIME_SP_QT_THROWING_FAR		= 0.25f;
constexpr float TIME_QT_THROWING_LONGBOW	= 0.79f;
constexpr float TIME_SP_QT_THROWING_LB		= 0.18f;
constexpr float TIME_QT_THROWING_SOFT		= 0.79f;
constexpr float TIME_SP_QT_THROWING_SOFT	= 0.21f;

enum throwable_e
{
	GR_IDLE = 0,

	GR_QT_READY = 12,
	GR_QT_READY_2 = 25,

	FB_QT_READY = 38,

	SG_QT_READY = 51,

	QT_THROWING_FAR = 52,
	QT_THROWING_LONGBOW = 53,
	QT_THROWING_SOFT = 54,
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

#define MK46_VIEW_MODEL		"models/weapons/v_mk46.mdl"
#define MK46_WORLD_MODEL	"models/weapons/w_mk46.mdl"
#define MK46_FIRE_SFX		"weapons/mk46/mk46_fire.wav"

class CMK46 : public CBaseWeaponTemplate<CMK46>
{
public:	// Constants / Database
	enum mk46_anim_e
	{
		IDLE,
		SHOOT_UNSCOPE,
		SHOOT1,
		SHOOT2,
		SHOOT3,
		RELOAD_EMPTY,
		RELOAD,
		DRAW_FIRST,
		DEPLOY,
		JUMP,
		HOLSTER,
		INSPECTION,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_UP,
		LHAND_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
	};

	static constexpr pcchar	VIEW_MODEL		= "models/weapons/v_mk46.mdl";
	static constexpr pcchar	WORLD_MODEL		= "models/weapons/w_mk46.mdl";
	static constexpr pcchar	FIRE_SFX		= "weapons/mk46/mk46_fire.wav";
	static constexpr pcchar	POSTURE			= "m249";
	static constexpr float	MAX_SPEED		= 210.0f;
	static constexpr float	DAMAGE			= 32.0f;
	static constexpr float	RANGE_MODIFER	= 1.077217345;	// 80% damage @1500 inches.
	static constexpr float	DEPLOY_TIME		= 1.0F;
	static constexpr float	DRAW_FIRST_TIME	= 1.68F;
	static constexpr float	RELOAD_TIME		= 6.767f;
	static constexpr float	RELOAD_EMPTY_TIME= 6.232f;
	static constexpr float	HOLSTER_TIME	= 0.6F;
	static constexpr float	INSPECTION_TIME	= 1.5556f;
	static constexpr float	BLOCK_UP_TIME	= 0.525f;
	static constexpr float	BLOCK_DOWN_TIME	= 0.6f;
	static constexpr float	LHAND_UP_TIME	= 0.7333f;
	static constexpr float	LHAND_DOWN_TIME	= 0.7333f;
	static constexpr float	DASH_ENTER_TIME	= 0.68F;
	static constexpr float	DASH_EXIT_TIME	= 0.68F;
	static constexpr float	RPM				= 750.0f;
	static constexpr int	PENETRATION		= 2;
	static constexpr float	EFFECTIVE_RANGE	= 8192.0f;
	static constexpr int	GUN_VOLUME		= NORMAL_GUN_VOLUME;
	static constexpr float	SPREAD_BASELINE	= 0.3f;
	static constexpr float	ACCURACY_BASELINE= 0.35f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual void	PrimaryAttack	(void)	{ return MK46Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void MK46Fire(float flSpread, float flCycleTime = (60.0f / RPM));
};

#define KSG12_VIEW_MODEL	"models/weapons/v_ksg12.mdl"
#define KSG12_WORLD_MODEL	"models/weapons/w_ksg12.mdl"
#define KSG12_FIRE_SFX		"weapons/ksg12/ksg12_fire.wav"

constexpr float KSG12_MAX_SPEED			= 230.0f;
constexpr float KSG12_DAMAGE			= 20.0f;
constexpr int	KSG12_PROJECTILE_COUNT	= 9;
constexpr float	KSG12_EFFECTIVE_RANGE	= 3000.0f;
constexpr float KSG12_FIRE_INTERVAL		= 0.95f;
constexpr float KSG12_SHELL_EJECT		= 0.66f;
constexpr float KSG12_TIME_START_RELOAD = 0.566f;
constexpr float KSG12_TIME_INSERT		= 0.75f;
constexpr float KSG12_TIME_ADD_AMMO		= 0.3f;
constexpr float KSG12_TIME_AFTER_RELOAD = 1.033f;
const	Vector	KSG12_CONE_VECTOR		= Vector(0.0675f, 0.0675f, 0.0f); // special shotgun spreads
constexpr float KSG12_RANGE_MODIFIER	= 1.172793196; // 80% damage @700 inches.

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
	virtual	float	GetMaxSpeed		(void)	{ return KSG12_MAX_SPEED; }
	virtual	void	PlayEmptySound	(void);
	virtual void	PushAnim		(void);
	virtual void	PopAnim			(void);
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void)	{ return KSG12_CONE_VECTOR.x; }
};

#define M4A1_VIEW_MODEL		"models/weapons/v_m4a1.mdl"
#define M4A1_WORLD_MODEL	"models/weapons/w_m4a1.mdl"
#define M4A1_FIRE_SFX		"weapons/AR15/ar15_shoot.wav"

class CM4A1 : public CBaseWeaponTemplate<CM4A1>
{
public:	// Constants / Database
	enum m4a1_anim_e
	{
		IDLE,
		SHOOT_BACKWARD,
		SHOOT_LEFTWARD,
		SHOOT_RIGHTWARD,
		SHOOT_ATTACHMENTS,
		M870MCS_PUMP,
		RELOAD,
		RELOAD_EMPTY,
		M203_RELOAD,
		XM26_RELOAD,
		XM26_RELOAD_EMPTY,
		M870MCS_RELOAD_START,
		M870MCS_RELOAD_FIRST_INSERT,
		M870MCS_RELOAD_INSERT,
		M870MCS_RELOAD_END,
		M870MCS_RELOAD_END_EMPTY,
		DRAW_FIRST,
		DEPLOY,
		JUMP,
		CHECK_MAGAZINE,
		SWITCH_SELECTOR,
		HOLSTER,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_UP,
		LHAND_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
	};

	static constexpr pcchar	VIEW_MODEL			= "models/weapons/v_m4a1.mdl";
	static constexpr pcchar	WORLD_MODEL			= "models/weapons/w_m4a1.mdl";
	static constexpr pcchar	FIRE_SFX			= "weapons/AR15/ar15_shoot.wav";
	static constexpr pcchar	POSTURE				= "rifle";
	static constexpr float	MAX_SPEED			= 230.0f;
	static constexpr float	DAMAGE				= 32.0f;
	static constexpr float	RANGE_MODIFER		= 1.072221173;	// 80% damage @1600 inches.
	static constexpr float	DEPLOY_TIME			= 0.743f;
	static constexpr float	DRAW_FIRST_TIME		= 2.45f;
	static constexpr float	RELOAD_TIME			= 2.033f;
	static constexpr float	RELOAD_EMPTY_TIME	= 2.6f;
	static constexpr float	CHECK_MAGAZINE_TIME	= 3.06f;
	static constexpr float	HOLSTER_TIME		= 0.6f;
	static constexpr float	BLOCK_UP_TIME		= 0.3333f;
	static constexpr float	BLOCK_DOWN_TIME		= 0.3333f;
	static constexpr float	LHAND_UP_TIME		= 0.5526f;
	static constexpr float	LHAND_DOWN_TIME		= 0.4210f;
	static constexpr float	DASH_ENTER_TIME		= 0.485f;
	static constexpr float	DASH_EXIT_TIME		= 0.485f;
	static constexpr float	RPM					= 700.0f;	// 700~950 RPM
	static constexpr int	PENETRATION			= 2;
	static constexpr float	EFFECTIVE_RANGE		= 8192.0f;
	static constexpr int	GUN_VOLUME			= NORMAL_GUN_VOLUME;
	static constexpr float	SPREAD_BASELINE		= 0.14f;
	static constexpr float	ACCURACY_BASELINE	= 0.25f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual void	Think			(void);
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual void	PrimaryAttack	(void)	{ return M4A1Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void M4A1Fire(float flSpread, float flCycleTime = (60.0f / RPM));
};

#define PM9_VIEW_MODEL	"models/weapons/v_pm9.mdl"
#define PM9_WORLD_MODEL	"models/weapons/w_pm9.mdl"
#define PM9_FIRE_SFX	"weapons/pm9/pm9_fire.wav"

const float MAC10_MAX_SPEED     = 250.0f;
const float MAC10_DAMAGE        = 29.0f;
const float MAC10_RANGE_MODIFER = 1.149658245;	// 80% damage @800 inches.
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

#define Anaconda_VIEW_MODEL		"models/weapons/v_anaconda.mdl"
#define Anaconda_WORLD_MODEL	"models/weapons/w_anaconda.mdl"
#define Anaconda_FIRE_SFX		"weapons/anaconda/anaconda_fire.wav"

constexpr float ANACONDA_MAX_SPEED			= 250.0f;
constexpr float ANACONDA_DAMAGE				= 55;
constexpr float ANACONDA_RANGE_MODIFER		= 1.052022439;	// 80% damage @2200 inches.
constexpr float ANACONDA_RELOAD_TIME		= 2.68f;
constexpr float ANACONDA_DEPLOY_TIME		= 0.367f;	// this gun has a extremely short deploy time.
constexpr float ANACONDA_FIRE_INTERVAL		= 0.15f;
constexpr int	ANACONDA_PENETRATION		= 1;
constexpr float	ANACONDA_EFFECTIVE_RANGE	= 4096.0f;
constexpr float ANACONDA_SPREAD_BASELINE	= 0.24;

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
	virtual void	PrimaryAttack	(void) { return AnacondaFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual float	GetMaxSpeed		(void) { return ANACONDA_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new funcs
	void AnacondaFire(float flSpread, float flCycleTime = ANACONDA_FIRE_INTERVAL);
};

#define P90_VIEW_MODEL	"models/weapons/v_p90.mdl"
#define P90_WORLD_MODEL	"models/weapons/w_p90.mdl"
#define P90_FIRE_SFX	"weapons/p90/p90_fire.wav"

const float P90_MAX_SPEED     = 245.0f;
const float P90_DAMAGE        = 24;
const float P90_RANGE_MODIFER = 1.106751211;	// 80% damage @1100 inches.
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

#define M200_VIEW_MODEL		"models/weapons/v_m200.mdl"
#define M200_WORLD_MODEL	"models/weapons/w_m200.mdl"
#define M200_FIRE_SFX		"weapons/m200/m200_fire.wav"

const float SCOUT_MAX_SPEED      = 260.0f;
const float SCOUT_MAX_SPEED_ZOOM = 220.0f;
const float SCOUT_DAMAGE         = 100;
const float SCOUT_RANGE_MODIFER  = 1.061766218;	// 75% damage @2400 inches.
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

#define MP7A1_VIEW_MODEL	"models/weapons/v_mp7a1.mdl"
#define MP7A1_WORLD_MODEL	"models/weapons/w_mp7a1.mdl"
#define MP7A1_FIRE_SFX		"weapons/mp7a1/mp7a1_fire.wav"

constexpr float MP7A1_MAX_SPEED			= 250.0f;
constexpr float MP7A1_DAMAGE			= 22;
constexpr float MP7A1_RANGE_MODIFER		= 1.106751211;	// 80% damage @1100 inches.
constexpr float MP7A1_RELOAD_TIME		= 2.575f;
constexpr float MP7A1_RPM				= 850.0f;
constexpr int	MP7A1_PENETRATION		= 1;
constexpr float	MP7A1_EFFECTIVE_RANGE	= 8192.0f;
constexpr float MP7A1_SPREAD_BASELINE	= 0.2f;

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
	virtual void	PrimaryAttack	(void) { return MP7A1Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);
	virtual void	WeaponIdle		(void);

public:	// util funcs
	virtual float	GetMaxSpeed		(void) { return MP7A1_MAX_SPEED; }
	virtual void	ResetModel		(void);
	virtual float	GetSpread		(void);

public:	// new funcs
	void MP7A1Fire(float flSpread, float flCycleTime = (60.0f / MP7A1_RPM));
};

#define M1014_VIEW_MODEL	"models/weapons/v_m1014.mdl"
#define M1014_WORLD_MODEL	"models/weapons/w_m1014.mdl"
#define M1014_FIRE_SFX		"weapons/m1014/m1014_fire.wav"

class CM1014 : public CBaseWeaponTemplate<CM1014>
{
public:	// Constants / Database
	enum m1014_anim_e
	{
		IDLE,
		SHOOT,
		SHOOT_LAST,
		AIM_SHOOT,
		AIM_SHOOT_LAST,
		START_RELOAD,
		START_RELOAD_FIRST,
		INSERT,
		AFTER_RELOAD,
		DRAW_FIRST,
		DEPLOY,
		HOLSTER,
		CHECKMAG,
		INSPECTION,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_UP,
		LHAND_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
	};

	static constexpr pcchar	VIEW_MODEL				= "models/weapons/v_m1014.mdl";
	static constexpr pcchar	WORLD_MODEL				= "models/weapons/w_m1014.mdl";
	static constexpr pcchar	FIRE_SFX				= "weapons/m1014/m1014_fire.wav";
	static constexpr pcchar	POSTURE					= "mp5";
	static constexpr float	MAX_SPEED				= 240.0f;
	static constexpr float	DAMAGE					= 20.0f;
	static constexpr int	PROJECTILE_COUNT		= 6;
	static constexpr float	EFFECTIVE_RANGE			= 3048.0f;
	static constexpr float	FIRE_INTERVAL			= 0.25f;
	static constexpr float	TIME_START_RELOAD		= 0.8f;
	static constexpr float	TIME_START_RELOAD_FIRST	= 2.333f;
	static constexpr float	TIME_INSERT				= 0.4f;
	static constexpr float	TIME_ADD_AMMO			= 0.2f;
	static constexpr float	TIME_ADD_AMMO_FIRST		= 1.0667f;
	static constexpr float	TIME_INSERT_SFX			= 0.2f;
	static constexpr float	TIME_SIDELOAD_SFX		= 1.0f;
	static constexpr float	TIME_AFTER_RELOAD		= 0.7667f;
	static constexpr float	DRAW_FIRST_TIME			= 1.9f;
	static constexpr float	DEPLOY_TIME				= 0.833f;
	static constexpr float	HOLSTER_TIME			= 0.833f;
	static constexpr float	CHECKMAG_TIME			= 2.3f;
	static constexpr float	INSPECTION_TIME			= 1.8f;
	static constexpr float	BLOCK_UP_TIME			= 0.366F;
	static constexpr float	BLOCK_DOWN_TIME			= 0.366F;
	static constexpr float	LHAND_UP_TIME			= 0.7f;
	static constexpr float	LHAND_DOWN_TIME			= 0.7f;
	static constexpr float	DASH_ENTER_TIME			= 0.366F;
	static constexpr float	DASH_EXIT_TIME			= 0.366F;
	static constexpr Vector	CONE_VECTOR				= Vector(0.0725, 0.0725, 0.0); // special shotgun spreads
	static constexpr int	GUN_VOLUME				= LOUD_GUN_VOLUME;
	static constexpr float	RANGE_MODIFIER			= 1.172793196;	// 80% damage @700 inches.
	static constexpr float	ACCURACY_BASELINE = 0;	// Dummy, unused.

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam	(void);
#endif

public:
	// Bolt stop available anims.
	static constexpr int BITS_BOLT_STOP_ANIM =	(1 << IDLE) |
												(1 << DEPLOY) |
												(1 << HOLSTER) |
												(1 << INSPECTION) |
												(1 << BLOCK_UP) | (1 << BLOCK_DOWN) |
												(1 << LHAND_UP) | (1 << LHAND_DOWN) |
												(1 << DASH_ENTER) | (1 << DASHING) | (1 << DASH_EXIT);


public:
	bool	m_bAllowNextEmptySound;
	float	m_flNextInsertAnim;
	float	m_flNextAddAmmo;
	float	m_flNextInsertionSFX;
	bool	m_bSetForceStopReload;
	bool	m_bStartFromEmpty;

	struct	// shotgun needs to expand these push-pop stuff a little bit.
	{
		float	m_flNextInsertAnim;
		float	m_flNextAddAmmo;
		float	m_flNextInsertionSFX;
	}
	m_Stack2;

public:	// basic logic funcs
	virtual void	Think			(void);
	virtual bool	Deploy			(void);
	virtual void	PostFrame		(void);
	virtual void	PrimaryAttack	(void);
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);

public:	// util funcs
	virtual	void	PlayEmptySound	(void);
	virtual void	PushAnim		(void);
	virtual void	PopAnim			(void);
	virtual float	GetSpread		(void)	{ return CONE_VECTOR.x; }
};

#define M45A1_VIEW_MODEL	"models/weapons/v_m45a1.mdl"
#define M45A1_WORLD_MODEL	"models/w_elite.mdl"	// FIXME, BUGBUG
#define M45A1_FIRE_SFX		"weapons/m45a1/m45a1_fire.wav"
#define M45A1_FIRE_SFX_SIL	"weapons/m45a1/m45a1_fire_sil.wav"

class CM45A1 : public CBaseWeaponTemplate<CM45A1>
{
public:	// Constants / Database
	enum m45a1_anim_e
	{
		IDLE = 0,
		SHOOT,
		SHOOT_LAST,
		AIM_SHOOT_A,
		AIM_SHOOT_B,
		AIM_SHOOT_LAST,
		RELOAD_EMPTY,
		RELOAD,
		DRAW_FIRST,
		DEPLOY,
		JUMP,
		HOLSTER,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_DOWN,
		LHAND_UP,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
		SH_RELOAD_EMPTY,
		SH_RELOAD,
		SH_DASH_ENTER,
		SH_DASHING,
		SH_DASH_EXIT,
		CHECK_MAGAZINE
	};

	static constexpr pcchar	VIEW_MODEL			= "models/weapons/v_m45a1.mdl";
	static constexpr pcchar	WORLD_MODEL			= "models/w_elite.mdl";	// FIXME, BUGBUG
	static constexpr pcchar	FIRE_SFX			= "weapons/m45a1/m45a1_fire.wav";
	static constexpr pcchar	FIRE_SFX_SIL		= "weapons/m45a1/m45a1_fire_sil.wav";
	static constexpr pcchar	POSTURE				= "onehanded";
	static constexpr float	MAX_SPEED			= 250.0f;
	static constexpr float	RELOAD_EMPYT_TIME	= 2.6f;
	static constexpr float	RELOAD_TIME			= 2.166f;
	static constexpr float	DRAW_FIRST_TIME		= 1.067F;
	static constexpr float	DEPLOY_TIME			= 0.7F;
	static constexpr float	HOLSTER_TIME		= 0.5F;
	static constexpr float	DASH_ENTER_TIME		= 0.5333F;
	static constexpr float	DASH_EXIT_TIME		= 0.5333F;
	static constexpr float	BLOCK_UP_TIME		= 0.4f;
	static constexpr float	BLOCK_DOWN_TIME		= 0.4333f;
	static constexpr float	LHAND_UP_TIME		= 0.4f;
	static constexpr float	LHAND_DOWN_TIME		= 0.3333f;
	static constexpr float	SH_RELOAD_EMPTY_TIME= 2.833F;
	static constexpr float	SH_RELOAD_TIME		= 2.1667F;
	static constexpr float	SH_DASH_ENTER_TIME	= 0.33333F;
	static constexpr float	SH_DASH_EXIT_TIME	= 0.65F;
	static constexpr float	CHECK_MAGAZINE_TIME	= 2.06667F;
	static constexpr float	DAMAGE				= 36.0f;
	static constexpr float	RANGE_MODIFER		= 1.131980371;	// 80% damage @900 inches.
	static constexpr float	FIRE_INTERVAL		= 0.2f;
	static constexpr int	PENETRATION			= 1;
	static constexpr float	EFFECTIVE_RANGE		= 4096.0f;
	static constexpr int	GUN_VOLUME			= NORMAL_GUN_VOLUME;
	static constexpr float	SPREAD_BASELINE		= 0.9f;
	static constexpr float	ACCURACY_BASELINE	= 0.88f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void);
#else
public:	// CL exclusive functions.
	virtual int		CalcBodyParam(void);
#endif

	// Slide stop available anims.
	static constexpr int BITS_SLIDE_STOP_ANIM = (1 << IDLE) |
												(1 << DEPLOY) |
												(1 << HOLSTER) |
												(1 << CHECK_MAGAZINE) |
												(1 << LHAND_DOWN) | (1 << LHAND_UP) |
												(1 << BLOCK_DOWN) | (1 << BLOCK_UP) |
												(1 << DASH_ENTER) | (1 << DASHING) | (1 << DASH_EXIT) |
												(1 << SH_DASH_ENTER) | (1 << SH_DASHING) | (1 << SH_DASH_EXIT);

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void)	{ return M45A1Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void M45A1Fire(float flSpread, float flCycleTime = FIRE_INTERVAL);
};

#define FN57_VIEW_MODEL		"models/weapons/v_fiveseven.mdl"
#define FN57_WORLD_MODEL	"models/weapons/w_fiveseven.mdl"
#define FN57_FIRE_SFX		"weapons/fiveseven/fiveseven_fire.wav"
#define FN57_FIRE_SIL_SFX	"weapons/fiveseven/fiveseven_fire-sil.wav"

class CFN57 : public CBaseWeaponTemplate<CFN57>
{
public:	// Constants / Database
	enum fiveseven_anim_e
	{
		IDLE = 0,
		SHOOT,
		SHOOT_LAST,
		AIM_SHOOT,
		AIM_SHOOT_LAST,
		RELOAD,
		RELOAD_EMPTY,
		DEPLOY,
		DRAW_FIRST,
		HOLSTER,
		CHECK_MAGAZINE,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_UP,
		LHAND_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
		SH_RELOAD,
		SH_RELOAD_EMPTY,
		SH_DASH_ENTER,
		SH_DASHING,
		SH_DASH_EXIT,
	};

	static constexpr pcchar	VIEW_MODEL			= "models/weapons/v_fiveseven.mdl";
	static constexpr pcchar	WORLD_MODEL			= "models/weapons/w_fiveseven.mdl";
	static constexpr pcchar	FIRE_SFX			= "weapons/fiveseven/fiveseven_fire.wav";
	static constexpr pcchar	FIRE_SFX_SIL		= "weapons/fiveseven/fiveseven_fire-sil.wav";
	static constexpr pcchar	POSTURE				= "onehanded";
	static constexpr float	MAX_SPEED			= 250.0f;
	static constexpr float	DAMAGE				= 24;
	static constexpr float	RANGE_MODIFER		= 1.149658245;	// 80% damage @800 inches.
	static constexpr float	RELOAD_TIME			= 1.94F;
	static constexpr float	RELOAD_EMPTY_TIME	= 1.86F;
	static constexpr float	DRAW_FIRST_TIME		= 1.56F;
	static constexpr float	DEPLOY_TIME			= 0.7F;
	static constexpr float	HOLSTER_TIME		= 0.7F;
	static constexpr float	CHECKMAG_TIME		= 2.033F;
	static constexpr float	DASH_ENTER_TIME		= 0.8F;
	static constexpr float	DASH_EXIT_TIME		= 0.37F;
	static constexpr float	BLOCK_UP_TIME		= 0.4666f;
	static constexpr float	BLOCK_DOWN_TIME		= 0.4666f;
	static constexpr float	LHAND_UP_TIME		= 0.2666f;
	static constexpr float	LHAND_DOWN_TIME		= 0.2666f;
	static constexpr float	FIRE_INTERVAL		= 0.132f;
	static constexpr float	EFFECTIVE_RANGE		= 4096.0f;
	static constexpr int	PENETRATION			= 1;
	static constexpr int	GUN_VOLUME			= NORMAL_GUN_VOLUME;
	static constexpr float	SPREAD_BASELINE		= 0.4f;
	static constexpr float	ACCURACY_BASELINE	= 0.92f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void);
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam(void);
#endif

	// Slide stop available anims.
	static constexpr int BITS_SLIDE_STOP_ANIM = (1 << IDLE) |
												(1 << DEPLOY) |
												(1 << HOLSTER) |
												(1 << CHECK_MAGAZINE) |
												(1 << LHAND_DOWN) | (1 << LHAND_UP) |
												(1 << BLOCK_DOWN) | (1 << BLOCK_UP) |
												(1 << DASH_ENTER) | (1 << DASHING) | (1 << DASH_EXIT) |
												(1 << SH_DASH_ENTER) | (1 << SH_DASHING) | (1 << SH_DASH_EXIT);

public:	// basic logic funcs
	virtual bool	Deploy			(void);
	virtual void	PrimaryAttack	(void) { return FiveSevenFire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void FiveSevenFire(float flSpread, float flCycleTime = FIRE_INTERVAL);
};

#define UMP45_VIEW_MODEL	"models/weapons/v_ump45.mdl"
#define UMP45_WORLD_MODEL	"models/weapons/w_ump45.mdl"	
#define UMP45_FIRE_SFX		"weapons/ump45/ump45_fire.wav"
#define UMP45_FIRE_SIL_SFX	"weapons/ump45/ump45_fire.wav"	// FIXME

class CUMP45 : public CBaseWeaponTemplate<CUMP45>
{
public:	// Constants / Database
	enum ump45_anim_e
	{
		IDLE,
		SHOOT,
		SHOOT_AIM,
		RELOAD,
		RELOAD_EMPTY,
		DEPLOY,
		DRAW_FIRST,
		HOLSTER,
		CHECKMAG,
		TO_SEMI,
		TO_AUTO,
		BLOCK_UP,
		BLOCK_DOWN,
		LHAND_DOWN,
		LHAND_UP,
		DASH_ENTER,
		DASHING,
		DASH_EXIT
	};

	static constexpr auto	VIEW_MODEL		= "models/weapons/v_ump45.mdl";
	static constexpr auto	WORLD_MODEL		= "models/weapons/w_ump45.mdl";
	static constexpr auto	FIRE_SFX		= "weapons/ump45/ump45_fire.wav";
	static constexpr auto	FIRE_SIL_SFX	= "weapons/ump45/ump45_fire.wav";	// FIXME
	static constexpr auto	POSTURE			= "m249";
	static constexpr float	MAX_SPEED		= 250.0f;
	static constexpr float	DAMAGE			= 32;
	static constexpr float	RANGE_MODIFER	= 1.149658245;	// 80% damage @800 inches.
	static constexpr float	RELOAD_TIME		= 2.8667F;
	static constexpr float	RELOAD_EMPTY_TIME = 3.0667F;
	static constexpr float	DEPLOY_TIME		= 0.6333F;
	static constexpr float	DRAW_FIRST_TIME	= 1.2F;
	static constexpr float	HOLSTER_TIME	= 0.6333F;
	static constexpr float	CHECKMAG_TIME	= 3.6F;
	static constexpr float	BLOCK_UP_TIME	= 0.3667F;
	static constexpr float	BLOCK_DOWN_TIME	= 0.3667F;
	static constexpr float	LHAND_DOWN_TIME	= 0.7F;
	static constexpr float	LHAND_UP_TIME	= 0.7F;
	static constexpr float	DASH_ENTER_TIME	= 0.3667F;
	static constexpr float	DASH_EXIT_TIME	= 0.3667F;
	static constexpr float	RPM				= 600.0f;
	static constexpr float	EFFECTIVE_RANGE	= 8192.0f;
	static constexpr int	PENETRATION		= 1;
	static constexpr float	SPREAD_BASELINE	= 0.15f;
	static constexpr int	GUN_VOLUME		= NORMAL_GUN_VOLUME;
	static constexpr float	ACCURACY_BASELINE= 0.4f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache		(void);
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam	(void);
#endif

public:	// basic logic funcs
	virtual void	PrimaryAttack	(void)	{ return UMP45Fire(GetSpread()); }
	virtual void	SecondaryAttack	(void);
	virtual	bool	Reload			(void);

public:	// util funcs
	virtual float	GetSpread		(void);

public:	// new functions
	void UMP45Fire(float flSpread, float flCycleTime = 60.0f / RPM);
};

#define PSG1_VIEW_MODEL		"models/weapons/v_psg1.mdl"
#define PSG1_WORLD_MODEL	"models/weapons/w_psg1.mdl"
#define PSG1_FIRE_SFX		"weapons/m14ebr/m14ebr_fire.wav"	// UNDONE

class CPSG1 : public CBaseWeapon
{
public:	// Constants / Database
	enum psg1_anim_e
	{
		IDLE,
		SHOOT,
		RELOAD,
		RELOAD_EMPTY,
		DRAW,
		DRAW_FIRST,
		HOLSTER,
		CHECK_MAGAZINE,
		BLOCK_UP,
		BLOCK_DOWN,
		DASH_ENTER,
		DASHING,
		DASH_EXIT,
		LHAND_DOWN,
		LHAND_UP,
	};

	static constexpr auto	VIEW_MODEL			= "models/weapons/v_psg1.mdl";
	static constexpr auto	WORLD_MODEL			= "models/weapons/w_psg1.mdl";
	static constexpr auto	FIRE_SFX			= "weapons/m14ebr/m14ebr_fire.wav";	// UNDONE
	static constexpr auto	POSTURE				= "rifle";
	static constexpr float	MAX_SPEED			= 210.0f;
	static constexpr float	MAX_SPEED_ZOOM		= 150.0f;
	static constexpr float	DAMAGE				= 90.0f;
	static constexpr float	RANGE_MODIFER		= 1.03443782f;	// 85% damage @2400 inches.
	static constexpr float	FIRE_INTERVAL		= 0.125f;
	static constexpr float	FIRE_ANIMTIME		= 0.4333f;
	static constexpr float	RELOAD_TIME			= 3.0667F;
	static constexpr float	RELOAD_EMPTY_TIME	= 3.8667F;
	static constexpr float	DEPLOY_TIME			= 0.833F;
	static constexpr float	DRAW_FIRST_TIME		= 2.033F;
	static constexpr float	HOLSTER_TIME		= 0.8333F;
	static constexpr float	CHECK_MAGAZINE_TIME	= 3.0667F;
	static constexpr float	BLOCK_UP_TIME		= 0.3667F;
	static constexpr float	BLOCK_DOWN_TIME		= 0.3667F;
	static constexpr float	LHAND_UP_TIME		= 0.4333F;
	static constexpr float	LHAND_DOWN_TIME		= 0.4333F;
	static constexpr float	DASH_ENTER_TIME		= 0.3667F;
	static constexpr float	DASH_EXIT_TIME		= 0.3667F;
	static constexpr int	PENETRATION			= 2;
	static constexpr float	EFFECTIVE_RANGE		= 8192.0f;
	static constexpr int	GUN_VOLUME			= BIG_EXPLOSION_VOLUME;
	static constexpr float	SPREAD_BASELINE		= 0.0025f;
	static constexpr float	ACCURACY_BASELINE	= 0.2f;

#ifndef CLIENT_DLL
public:	// SV exclusive variables.
	static unsigned short m_usEvent;
	static int m_iShell;

public:	// SV exclusive functions.
	virtual void	Precache(void) final;
#else
public:	// CL exclusive functions.
	virtual	bool	UsingInvertedVMDL(void) final { return false; }	// Model designed by InnocentBlue is not inverted.
	virtual int		CalcBodyParam(void) final;
#endif

public:	// basic logic funcs
	void	PrimaryAttack	(void) final { return PSG1Fire(GetSpread()); }
	void	SecondaryAttack	(void) final { return DefaultScopeSight(Vector(-4.9f, 5, 0.5f), 25); }
	bool	Reload			(void) final;
	bool	HolsterStart	(void) final;

public:	// util funcs
	float	GetMaxSpeed		(void) final;
	float	GetSpread		(void) final;

public:	// new funcs
	void PSG1Fire(float flSpread, float flCycleTime = FIRE_INTERVAL);
};

#define C4_VIEW_MODEL	"models/weapons/v_c4.mdl"
#define C4_WORLD_MODEL	"models/weapons/w_c4.mdl"
#define C4_PLACED_SFX	"zombieriot/plant.wav"

constexpr float C4_TIME_DRAW			= 0.7f;
constexpr float C4_TIME_HOLSTER			= 0.7f;
constexpr float C4_TIME_THROW			= 1.0333f;
constexpr float C4_TIME_THROW_SPAWN		= 0.5f;
constexpr float C4_TIME_PLACE			= 1.0333f;
constexpr float C4_TIME_PLACE_SPAWN		= 0.5f;
constexpr float C4_TIME_DET_ANIM		= 1.0333f;
constexpr float C4_TIME_DETONATE		= 0.4f;
constexpr float C4_TIME_PICKUP			= 0.7f;
constexpr float C4_TIME_PICKUP_HAPPEN	= 0.3333f;

enum c4_e
{
	C4_IDLE = 0,
	C4_DRAW,
	C4_HOLSTER,
	C4_THROW,
	C4_PLACE,
	C4_DETONATE,
	C4_PICKUP,
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
