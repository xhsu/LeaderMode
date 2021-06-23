/*

Created Date: Mar 12 2020

*/

#pragma once

#include "../dlls/weapons.h"	// only import this.

// util macros
#define IS_AIMING	(g_pCurWeapon && (g_pCurWeapon->m_bInZoom || gHUD::m_iFOV < 90))
#define IS_DASHING	(g_pCurWeapon && g_pCurWeapon->m_bitsFlags & WPNSTATE_DASHING)

// copied from mp.dll::player.h
enum PLAYER_ANIM
{
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
	PLAYER_ATTACK2,
	PLAYER_FLINCH,
	PLAYER_LARGE_FLINCH,
	PLAYER_RELOAD,
	PLAYER_HOLDBOMB
};

struct pseudo_ent_var_s	// pseudo entvars_t
{
	int		flags;
	Vector	velocity;
	Vector	angles;
	Vector	v_angle;
	Vector	punchangle;
	int		effects;
	Vector	origin;
	Vector	view_ofs;
	int		button;
	float	fov;
	int		viewmodel;
	int		weaponmodel;
	int		weaponanim;
	int		deadflag;
	int		waterlevel;
	int		maxspeed;
	uint	weapons;
};

struct pseudo_global_vars_s	// pseudo globalvars_t
{
	double	time;
	double	frametime;
	Vector	v_forward;
	Vector	v_right;
	Vector	v_up;
};

struct pseudo_gamerule_s	// pseudo CHalfLifeMultiplay
{
	bool FShouldSwitchWeapon(CBasePlayer* pPlayer, CBaseWeapon* pWeapon);
};

class CBasePlayer	// pseudo player
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
	CBasePlayer();
	virtual ~CBasePlayer() {}

public:
	std::shared_ptr<pseudo_ent_var_s> pev;
	int		m_iWeaponVolume;
	int		m_iWeaponFlash;
	uint32	random_seed;
	bool	m_bResumeZoom : 1;
	int		m_iLastZoom;
	float	m_flEjectBrass;
	int		m_iShellModelIndex;
	float	m_flNextAttack;
	bool	m_bCanShoot : 1;
	char	m_szAnimExtention[128];
	int		m_afButtonLast;
	int		m_afButtonPressed;
	int		m_afButtonReleased;
	CBaseWeapon* m_pActiveItem;
	CBaseWeapon* m_pLastItem;
	Vector	m_vecVAngleShift;
	EquipmentIdType m_iUsingGrenadeId;
	CBaseWeapon* m_pWpnSwitchingTo;	// use this instead of g_iSelectedWeapon.
	RoleTypes m_iRoleType;
	std::array<bool, EQP_COUNT> m_rgbHasEquipment;
	std::array<int, AMMO_MAXTYPE> m_rgAmmo;
	bool	m_bHasPrimary : 1;
	std::array<CBaseWeapon*, MAX_ITEM_TYPES> m_rgpPlayerItems;

public:
	void	SetAnimation(PLAYER_ANIM playerAnim)	{}
	Vector	GetGunPosition(void);
	Vector2D FireBullets3(Vector vecSrc, Vector vecDirShooting, float vecSpread, float flDistance, int iPenetration, AmmoIdType iBulletType, int iDamage, float flRangeModifier, int shared_rand);
	int		FireBuckshots(ULONG cShots, const Vector& vecSrc, const Vector& vecDirShooting, const Vector2D& vecSpread, float flDistance, int iDamage, float flExponentialBase, int shared_rand);	// returns the offset of shared_rand.
	void	ResetMaxSpeed(void);
	int		GetGrenadeInventory(EquipmentIdType iId);
	int*	GetGrenadeInventoryPointer(EquipmentIdType iId);
	void	ResetUsingEquipment(void);
	void	Radio(const char* psz1, const char* psz2) {}
	bool	StartSwitchingWeapon(CBaseWeapon* pSwitchingTo);	// play normal holster anim.
	bool	StartSwitchingWeapon(WeaponIdType iSwitchingTo);	// play normal holster anim.
	bool	SwitchWeapon(CBaseWeapon* pSwitchingTo);	// skip holster anim.
	CBaseWeapon* HasPlayerItem(WeaponIdType iId);
	bool	AddPlayerItem(CBaseWeapon* pItem);
};

extern int g_runfuncs;
extern local_state_t g_sWpnFrom;
extern local_state_t g_sWpnTo;
extern usercmd_t g_sWpnCmd;
inline std::shared_ptr<pseudo_global_vars_s> gpGlobals;
extern const Vector g_vecZero;
inline CBaseWeapon* g_pCurWeapon;
inline CBasePlayer gPseudoPlayer;
extern WeaponIdType g_iSelectedWeapon;	// this means directly switch weapon. try to use gPseudoPlayer.StartSwitchingWeapon() instead!
extern cvar_t* cl_holdtoaim;

extern bool g_bHoldingKnife;
extern bool g_bFreezeTimeOver;
extern bool g_bInBombZone;
extern bool g_bHoldingShield;
extern bool g_bIsBlocked;	// this should be override, but tell the server!

// pseudo-utils
void UTIL_MakeVectors(const Vector& vec);

// export func
void HUD_WeaponsPostThink(local_state_s* from, local_state_s* to, usercmd_t* cmd, double time, unsigned int random_seed);
void HUD_PostRunCmd2(local_state_t* from, local_state_t* to, usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed);
void Wpn_Init();
