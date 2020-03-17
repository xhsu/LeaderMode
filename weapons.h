/*

Created Date: Mar 12 2020

*/

#pragma once

#include "../dlls/weapons.h"	// only import this.

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
};

struct pseudo_global_vars_s	// pseudo globalvars_t
{
	double	time;
	Vector	v_forward;
	Vector	v_right;
	Vector	v_up;
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
	int		random_seed;
	int		m_rgAmmo[AMMO_MAXTYPE];
	bool	m_bResumeZoom;
	int		m_iLastZoom;
	float	m_flEjectBrass;
	int		m_iShellModelIndex;
	float	m_flNextAttack;
	bool	m_bCanShoot;
	char	m_szAnimExtention[128];
	int		m_afButtonLast;
	int		m_afButtonPressed;
	int		m_afButtonReleased;
	CBaseWeapon* m_pActiveItem;
	CBaseWeapon* m_pLastItem;

public:
	void	SetAnimation(PLAYER_ANIM playerAnim)	{}
	Vector	GetGunPosition(void);
	Vector	FireBullets3(Vector vecSrc, Vector vecDirShooting, float flSpread, float flDistance, int iPenetration, int iBulletType, int iDamage, float flRangeModifier, std::shared_ptr<pseudo_ent_var_s> pevAttacker, bool bPistol, int shared_rand);
	int		FireBuckshots(ULONG cShots, const Vector& vecSrc, const Vector& vecDirShooting, const Vector& vecSpread, float flDistance, int iDamage, int shared_rand);	// returns the offset of shared_rand.
};

extern int g_runfuncs;
extern double g_flGameTime;
extern std::shared_ptr<pseudo_global_vars_s> gpGlobals;
extern const Vector g_vecZero;
extern CBaseWeapon* g_rgpClientWeapons[LAST_WEAPON];

extern bool g_bHoldingKnife;
extern bool g_bFreezeTimeOver;
extern bool g_bInBombZone;
extern bool g_bHoldingShield;

// pseudo-utils
void UTIL_MakeVectors(const Vector& vec);

// export func
void HUD_PostRunCmd2(local_state_t* from, local_state_t* to, usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed);
