/*

Created Date: Apr 29 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

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
	float	health;

	int		iuser1;	// observer mode
	int		iuser2;	// first target
	int		iuser3;	// second target
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
	bool	m_bCanShoot;
	int		m_afButtonLast;
	int		m_afButtonPressed;
	int		m_afButtonReleased;
	RoleTypes m_iRoleType;
	Vector	m_vecVAngleShift;
	EquipmentIdType m_iUsingGrenadeId;
	WeaponIdType m_iWpnSwitchingTo;	// use this instead of g_iSelectedWeapon.
	std::array<bool, EQP_COUNT>	m_rgbHasEquipment;
	short	m_iTeam;

public:
	void	SetAnimation(PLAYER_ANIM playerAnim) {}
	Vector	GetGunPosition(void);
	Vector2D FireBullets3(Vector vecSrc, Vector vecDirShooting, float vecSpread, float flDistance, int iPenetration, AmmoIdType iBulletType, int iDamage, float flRangeModifier, int shared_rand);
	int		FireBuckshots(ULONG cShots, const Vector& vecSrc, const Vector& vecDirShooting, const Vector& vecSpread, float flDistance, int iDamage, float flExponentialBase, int shared_rand);	// returns the offset of shared_rand.
	void	ResetMaxSpeed(void);
	int		GetGrenadeInventory(EquipmentIdType iId);
	int*	GetGrenadeInventoryPointer(EquipmentIdType iId);
	void	ResetUsingEquipment(void);
	void	Radio(const char* psz1, const char* psz2) {}
	bool	StartSwitchingWeapon(WeaponIdType iId);	// play normal holster anim.
};

extern CBasePlayer gLocalPlayer;
extern pseudo_global_vars_s* gpGlobals;

extern float g_flPlayerSpeed;

void HUD_PostRunCmd2(local_state_t* from, local_state_t* to, usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed);
