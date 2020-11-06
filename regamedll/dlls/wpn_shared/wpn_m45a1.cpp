/*

Created Date: Oct 17 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Matoilet

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CM45A1::m_usEvent = 0;
int CM45A1::m_iShell = 0;

void CM45A1::Precache()
{
	PRECACHE_NECESSARY_FILES(M45A1);

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/m45a1.sc");
}

#else

static constexpr int SLIDE = 6;
static constexpr int SIGHT = 7;
static constexpr int MUZZLE = 8;
static constexpr int LASER = 9;

static constexpr int SILENCER = 1;
static constexpr int BREAKER = 2;

int CM45A1::CalcBodyParam(void)
{
	static BodyEnumInfo_t info[] =
	{
		{ 0, 2 },	// left hand	= 0;
		{ 0, 1 },	// right hand	= 1;
		{ 0, 1 },	// sleeve		= 2;

		{ 0, 1 },	// weapon		= 3;
		{ 0, 1 },
		{ 0, 1 },

		{ 0, 2 },	// slide		= 6;
		{ 0, 3 },	// sight		= 7;
		{ 0, 3 },	// muzzle		= 8;
		{ 0, 2 },	// laser		= 9;
	};

	// variation
	switch (m_iVariation)
	{
	case Role_SWAT:
	case Role_Medic:
	case Role_MadScientist:
		// sight
		info[LASER].body = FALSE;
		info[MUZZLE].body = FALSE;
		info[SIGHT].body = TRUE;
		break;

	case Role_Sharpshooter:
		// laser
		// sight
		info[LASER].body = TRUE;
		info[MUZZLE].body = FALSE;
		info[SIGHT].body = TRUE;
		break;

	case Role_Godfather:
	case Role_LeadEnforcer:
		// muzzle breaker
		info[LASER].body = FALSE;
		info[MUZZLE].body = BREAKER;
		info[SIGHT].body = FALSE;
		break;

	case Role_Assassin:
		// silencer
		// sight
		info[LASER].body = FALSE;
		info[MUZZLE].body = SILENCER;
		info[SIGHT].body = TRUE;
		break;

	default:
		// nothing
		info[LASER].body = FALSE;
		info[MUZZLE].body = FALSE;
		info[SIGHT].body = FALSE;
		break;
	}

	// slide stop vfx.
	if (m_iClip <= 0 && (1 << m_pPlayer->pev->weaponanim) & BITS_SLIDE_STOP_ANIM)
	{
		info[SLIDE].body = TRUE;

		// also move the sight, if it exists.
		if (info[SIGHT].body == TRUE)
			info[SIGHT].body = 2;
	}
	else
		info[SLIDE].body = 0;

	// TODO: checkmag anim & body parts.

	return CalcBody(info, _countof(info));	// elements count of the info[].
}

#endif

bool CM45A1::Deploy()
{
	m_flAccuracy = 0.88f;

	// this anim is slide-moving.
	if (m_iClip <= 0)
		m_bitsFlags &= ~WPNSTATE_DRAW_FIRST;

	return DefaultDeploy(
		M45A1_VIEW_MODEL, M45A1_WORLD_MODEL,
		(m_bitsFlags & WPNSTATE_DRAW_FIRST) ? M45A1_DRAW_FIRST : M45A1_DRAW,
		"onehanded",
		(m_bitsFlags & WPNSTATE_DRAW_FIRST) ? M45A1_DRAW_FIRST_TIME : M45A1_DRAW_TIME);
}

void CM45A1::SecondaryAttack()
{
	switch (m_iVariation)
	{
	case Role_SWAT:
	case Role_Sharpshooter:
	case Role_Medic:
	case Role_MadScientist:
	case Role_Assassin:
		// Electronic Sight
		DefaultSteelSight(Vector(-2.71f, -5, 1.45f), 75, 8.0f);
		break;

	default:
		// Steel Sight
		DefaultSteelSight(Vector(-2.71f, -5, 2.13f), 85, 8.0f);
		break;
	}
}

void CM45A1::M45A1Fire(float flSpread, float flCycleTime)
{
	if (++m_iShotsFired > 1)
	{
		return;
	}

	if (m_flLastFire)
	{
		m_flAccuracy -= (0.325f - (gpGlobals->time - m_flLastFire)) * 0.275f;

		if (m_flAccuracy > 0.88f)
		{
			m_flAccuracy = 0.88f;
		}
		else if (m_flAccuracy < 0.55f)
		{
			m_flAccuracy = 0.55f;
		}
	}

	m_flLastFire = gpGlobals->time;

	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;

#ifndef CLIENT_DLL
		if (TheBots)
		{
			TheBots->OnEvent(EVENT_WEAPON_FIRED_ON_EMPTY, m_pPlayer);
		}
#endif

		return;
	}

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

	m_iClip--;
	m_pPlayer->m_iWeaponVolume = M45A1_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, M45A1_EFFECTIVE_RANGE, M45A1_PENETRATION, m_iPrimaryAmmoType, M45A1_DAMAGE, M45A1_RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL

	int iAnim = 0;
	if (m_iClip > 0)
	{
		if (m_bInZoom)
			iAnim = UTIL_SharedRandomLong(m_pPlayer->random_seed, M45A1_AIM_SHOOT_A, M45A1_AIM_SHOOT_B);
		else
			iAnim = M45A1_SHOOT;
	}
	else
	{
		if (m_bInZoom)
			iAnim = M45A1_AIM_SHOOT_LAST;
		else
			iAnim = M45A1_SHOOT_LAST;
	}

	SendWeaponAnim(iAnim);	// LUNA: I don't know why, but this has to be done on SV side, or client fire anim would be override.
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), m_iVariation == Role_Assassin, FALSE);	// on non-host sending, the bparam1 is reserved for silencer status.

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = m_iClip > 0;	// on host ev sending, the bparams are used for first-personal shooting anim.
	args.bparam2 = m_bInZoom;
	args.ducking = gEngfuncs.pEventAPI->EV_LocalPlayerDucking();
	args.entindex = gEngfuncs.GetLocalPlayer()->index;
	args.flags = FEV_NOTHOST | FEV_RELIABLE | FEV_CLIENT | FEV_GLOBAL;
	args.fparam1 = vecDir.x;
	args.fparam2 = vecDir.y;
	args.iparam1 = int(m_pPlayer->pev->punchangle.x * 100.0f);
	args.iparam2 = int(m_pPlayer->pev->punchangle.y * 100.0f);
	args.origin = m_pPlayer->pev->origin;
	args.velocity = m_pPlayer->pev->velocity;

	EV_FireM45A1(&args);
#endif

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
	m_pPlayer->m_vecVAngleShift.x -= 2.0f;
}

bool CM45A1::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip,
		m_iClip ? M45A1_RELOAD : M45A1_RELOAD_EMPTY,
		m_iClip ? M45A1_RELOAD_TIME : M45A1_RELOAD_EMPYT_TIME,
		m_iClip ? 0.566f : 0.5f))
	{
		m_flAccuracy = 0.88f;
		return true;
	}

	// TODO: add a magcheck anim.

	return false;
}

DECLARE_STANDARD_RESET_MODEL_FUNC(M45A1)
