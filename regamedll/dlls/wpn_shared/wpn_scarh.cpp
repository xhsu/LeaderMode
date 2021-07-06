/*

Remastered Date: Mar 29 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Matoilet

*/

#include "precompiled.h"

void CSCARH::SecondaryAttack()
{
	switch (m_iVariation)
	{
	case Role_Sharpshooter:
		// ACOG
		DefaultSteelSight(Vector(-3.62f, -5, 0.5f), 55, 8.0f);
		break;

	default:
		// HOLO
		DefaultSteelSight(Vector(-3.725F, -2, 0.615F), 85, 8.0f);
		break;
	}
}

float CSCARH::GetSpread(void)
{
	m_flAccuracy = (float(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 220.0f) + ACCURACY_BASELINE;

	if (m_flAccuracy > 1.0f)
		m_flAccuracy = 1.0f;

	return DefaultSpread(SPREAD_BASELINE * m_flAccuracy, 0.1f, 0.75f, 2.0f, 5.0f);
}

#ifdef CLIENT_DLL

SCE_FIELD LHAND	= 0;
SCE_FIELD MAGAZINE = 6;
SCE_FIELD STEEL_SIGHT = 7;
SCE_FIELD SCOPES = 8;
SCE_FIELD UNDERBARRELS = 9;
SCE_FIELD NAIL_SHELL = 10;
SCE_FIELD MUZZLE = 11;
SCE_FIELD LASER = 12;

// Hands
SCE_FIELD VISIBLE = 0;
SCE_FIELD INVISIBLE = 1;

// Magazine
SCE_FIELD FULL = 0;
SCE_FIELD EMPTY = 1;

// Steel sight
SCE_FIELD FLIP_UP = 0;
SCE_FIELD FLIP_DOWN = 1;
SCE_FIELD NO_STEEL_SIGHT = 2;

// Scopes
SCE_FIELD NONE = 0;
SCE_FIELD ROUND_RED_DOT = 1;
SCE_FIELD HOLOGRAPHIC = 2;
SCE_FIELD OPENED_RED_DOT = 3;
SCE_FIELD ACOG = 4;
SCE_FIELD MATOILET_SPECIAL_SCOPE_THAT_I_DONT_KNOW_WHAT_IT_IS = 5;

// Underbarrels
SCE_FIELD EGLM = 1;
SCE_FIELD M870MCS = 2;
SCE_FIELD XM26 = 3;

// Nail status
SCE_FIELD NEW = 0;
SCE_FIELD DISPOSED_SHELL = 1;

// Muzzle
SCE_FIELD SILENCER = 1;
SCE_FIELD COMPENSATOR = 2;
SCE_FIELD FLASH_HIDER = 3;

int CSCARH::CalcBodyParam(void)
{
	BodyEnumInfo_t info[] =
	{
		{ 0, 2 },	// hands		= 0;
		{ 0, 1 },
		{ 0, 1 },

		{ 0, 1 },	// rifle		= 3;
		{ 0, 1 },
		{ 0, 1 },

		{ 0, 2 },	// magazine		= 6;
		{ 0, 3 },	// steel sight	= 7;
		{ 0, 6 },	// scopes		= 8;
		{ 0, 4 },	// underbarrels	= 9;
		{ 0, 2 },	// nail/shell	= 10;
		{ 0, 4 },	// muzzle		= 11;
		{ 0, 2 },	// laser		= 12;
	};

	switch (m_iVariation)
	{
	case Role_Sharpshooter:
		// the sharpshooter's version contains a ACOG and a laser.
		// removed steel sight.
		// ACOG.
		// Recoil compensator.
		// laser.

		info[STEEL_SIGHT].body = NO_STEEL_SIGHT;
		info[SCOPES].body = ACOG;
		info[MUZZLE].body = COMPENSATOR;
		info[LASER].body = TRUE;
		break;

	default:
		// by default, this weapon has:
		// filpped down steel sight.
		// holographic sight.

		info[STEEL_SIGHT].body = FLIP_DOWN;
		info[SCOPES].body = HOLOGRAPHIC;
		break;
	}

	if (!m_iClip)
		info[MAGAZINE].body = EMPTY;	// empty mag.

	// in EMPTY reload, after we remove the empty mag, the new mag should be full of bullets.
	if (m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY)
	{
		if (m_pPlayer->m_flNextAttack < 2.18F)	// in this anim, a new mag was taken out after around 0.3s. thus, 2.9F - 0.72f ~= 2.18f.
		{
			info[MAGAZINE].body = FULL;	// full mag.
		}
	}

	return CalcBody(info, _countof(info));	// elements count of the info[].
}

void CSCARH::Think(void)
{
	CBaseWeapon::Think();

	// just keep updating model during empty-reload.
	if (m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY)
	{
		g_pViewEnt->curstate.body = CalcBodyParam();
	}
}

#endif

int CSCARH::AcquireShootAnim()
{
	if (!m_iClip)
		return SHOOT_LAST;

	return UTIL_SharedRandomLong(m_pPlayer->random_seed, SHOOT1, SHOOT3);
}

void CSCARH::ApplyClientFPFiringVisual(const Vector2D& vSpread)
{
#ifdef CLIENT_DLL
	auto idx = gEngfuncs.GetLocalPlayer()->index;
	bool bDucking = gEngfuncs.pEventAPI->EV_LocalPlayerDucking();

	auto vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors(vecAngles);

	EV_MuzzleFlash();

	// first personal gun smoke.
	EV_HLDM_CreateSmoke(3, 0.2, Color(18, 18, 18), EV_RIFLE_SMOKE, false, 35);

	// shoot anim.
	EV_PlayShootAnim();

	// Shell stuff.
	Vector ShellVelocity, ShellOrigin;
	if (cl_righthand->value == 0)
		EV_GetDefaultShellInfo(ShellVelocity, ShellOrigin, 20.0, -8.0, -10.0);
	else
		EV_GetDefaultShellInfo(ShellVelocity, ShellOrigin, 20.0, -8.0, 10.0);

	ShellVelocity *= 1.65;
	ShellVelocity.z -= 120;

	EV_EjectBrass(ShellOrigin, ShellVelocity, vecAngles.yaw, m_iShell<CSCARH>, TE_BOUNCE_SHELL, 15);

	// Ballistic calculation.
	EV_HLDM_FireBullets(vSpread);

	// Original GoldSrc API: VOL = 1.0, ATTN = 0.4
	EV_PlayGunFire2();
#endif // CLIENT_DLL
}

void CSCARH::ApplyRecoil(void)
{
	if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		KickBack(1.0, 0.45, 0.28, 0.04, 4.25, 2.5, 7);
	}
	else if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		KickBack(1.25, 0.45, 0.22, 0.18, 6.0, 4.0, 5);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		KickBack(0.6, 0.35, 0.2, 0.0125, 3.7, 2.0, 10);
	}
	else
	{
		KickBack(0.625, 0.375, 0.25, 0.0125, 4.0, 2.25, 9);
	}
}

void CSCARH::ApplyClientTPFiringVisual(event_args_s* args)
{
#ifdef CLIENT_DLL
	bool bClipGreaterThanNaught = args->bparam1;
	bool bInZoom = args->bparam2;
	int idx = args->entindex;
	Vector2D vSpread(args->fparam1, args->fparam2);
	int iSeed = args->iparam1;
	RoleTypes iVariation = (RoleTypes)args->iparam2;

	Vector forward, right, up;
	AngleVectors(args->angles, forward, right, up);

	Vector ShellVelocity, ShellOrigin;
	::EV_GetDefaultShellInfo(args->entindex, args->ducking, args->origin, args->velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0);
	EV_EjectBrass(ShellOrigin, ShellVelocity, args->angles.yaw, m_iShell<CSCARH>, TE_BOUNCE_SHELL, 15);

	// original goldsrc api: VOL = 1.0, ATTN = 0.4
	::EV_PlayGunFire2(args->origin + forward * 10.0f, FIRE_SFX, GUN_VOLUME);

	::EV_HLDM_FireBullets(idx,
		forward, right, up,
		1, args->origin, forward,
		vSpread, EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_SCARH].m_iAmmoType,
		PENETRATION,
		iSeed
	);
#endif
}
