/*

Remastered Date: Mar 28 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Mesh/Animation - Matoilet
Animation - Innocent Blue
Sound - iDkGK

*/

#include "precompiled.h"


#ifndef CLIENT_DLL

unsigned short CMK46::m_usEvent = 0;
int CMK46::m_iShell = 0;

void CMK46::Precache()
{
	PRECACHE_NECESSARY_FILES(MK46);

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/mk46.sc");
}

#else

static constexpr int	BULLETS = 6;
static constexpr int	SIGHT = 7;
static constexpr int	MUZZLE = 8;
static constexpr int	LASER = 9;

static constexpr int	REDDOT = 3;
static constexpr int	ROUNDED = 1;
static constexpr int	HOLOGRAPHIC = 2;
static constexpr int	ACOG = 4;
static constexpr int	BREAKER = 2;
static constexpr int	SILENCER = 1;

int CMK46::CalcBodyParam(void)
{
	static BodyEnumInfo_t info[] =
	{
		{ 0, 2 },	// hands		= 0;
		{ 0, 1 },
		{ 0, 1 },	// sleeve		= 2;

		{ 0, 1 },	// weapon		= 3;
		{ 0, 1 },
		{ 0, 1 },

		{ 0, 16 },	// bullets		= 6;
		{ 0, 6 },	// optical sight= 7;
		{ 0, 4 },	// muzzle		= 8;
		{ 0, 2 },	// laser		= 9;
	};

	switch (m_iVariation)
	{
	case Role_Commander:
	case Role_Godfather:
		// holographic
		info[SIGHT].body = HOLOGRAPHIC;
		info[MUZZLE].body = FALSE;
		info[LASER].body = FALSE;
		break;

	case Role_SWAT:
		// openned red dot.
		info[SIGHT].body = REDDOT;
		info[MUZZLE].body = FALSE;
		info[LASER].body = FALSE;
		break;

	case Role_Medic:
	case Role_MadScientist:
		// rounded red dot.
		info[SIGHT].body = ROUNDED;
		info[MUZZLE].body = FALSE;
		info[LASER].body = FALSE;
		break;

	case Role_Sharpshooter:
		// laser
		// acog
		info[SIGHT].body = ACOG;
		info[MUZZLE].body = FALSE;
		info[LASER].body = TRUE;
		break;

	case Role_LeadEnforcer:
		// breaker
		info[SIGHT].body = FALSE;
		info[MUZZLE].body = BREAKER;
		info[LASER].body = FALSE;
		break;

	case Role_Assassin:
		// holographic
		// silencer
		info[SIGHT].body = HOLOGRAPHIC;
		info[MUZZLE].body = SILENCER;
		info[LASER].body = FALSE;
		break;

	default:
		info[SIGHT].body = FALSE;
		info[MUZZLE].body = FALSE;
		info[LASER].body = FALSE;
		break;
	}

	// as the magazine is getting lesser, this number is getting bigger. (later sub-model)
	info[BULLETS].body = Q_clamp(16 - m_iClip, 0, 15);

	// in EMPTY reload, after we remove the empty mag, the new mag should be full of bullets.
	if (m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY)
	{
		if (m_pPlayer->m_flNextAttack < 3.77f)	// in this anim, a new mag was taken out after around 2.46. thus, 6.23 - 2.46 ~= 3.77f.
		{
			info[BULLETS].body = 0;	// a full loaded bullets chain.
		}
	}

	return CalcBody(info, _countof(info));	// elements count of the info[].
}

void CMK46::Think(void)
{
	CBaseWeapon::Think();

	// just keep updating model during empty-reload.
	if (m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY)
	{
		g_pViewEnt->curstate.body = CalcBodyParam();
	}
}

#endif

void CMK46::SecondaryAttack(void)
{
	switch (m_iVariation)
	{
	case Role_Commander:
	case Role_Godfather:
	case Role_Assassin:
		DefaultSteelSight(Vector(-4.02f, -4, 0.505f), 80, 7.5F);
		break;

	case Role_SWAT:
		DefaultSteelSight(Vector(-4.04f, -4, -0.015f), 80, 10.0f);
		break;

	case Role_Medic:
	case Role_MadScientist:
		DefaultSteelSight(Vector(-4.025f, -4, 0.045f), 80, 7.5F);
		break;

	case Role_Sharpshooter:
		DefaultSteelSight(Vector(-4.007f, -5, 0.7f), 55, 7.5F);
		break;

	default:
		DefaultSteelSight(Vector(-4.02f, -4, 2.03f), 85, 7.5F);
		break;
	}
}

float CMK46::GetSpread(void)
{
	m_flAccuracy = (float(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 175.0f) + ACCURACY_BASELINE;

	if (m_flAccuracy > 0.9f)
		m_flAccuracy = 0.9f;

	return DefaultSpread(SPREAD_BASELINE * m_flAccuracy, 0.1f, 0.75f, 2.0f, 5.0f);
}

void CMK46::MK46Fire(float flSpread, float flCycleTime)
{
	m_iShotsFired++;
	m_bDelayRecovery = true;

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

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, EFFECTIVE_RANGE, PENETRATION, m_iPrimaryAmmoType, DAMAGE, RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	int seq = UTIL_SharedRandomFloat(m_pPlayer->random_seed, SHOOT1, SHOOT3);
	if (!m_bInZoom)
		seq = SHOOT_UNSCOPE;

	SendWeaponAnim(seq);
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), m_bInZoom, FALSE);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = m_bInZoom;
	args.bparam2 = false;	// unused
	args.ducking = gEngfuncs.pEventAPI->EV_LocalPlayerDucking();
	args.entindex = gEngfuncs.GetLocalPlayer()->index;
	args.flags = FEV_NOTHOST | FEV_RELIABLE | FEV_CLIENT | FEV_GLOBAL;
	args.fparam1 = vecDir.x;
	args.fparam2 = vecDir.y;
	args.iparam1 = int(m_pPlayer->pev->punchangle.x * 100.0f);
	args.iparam2 = int(m_pPlayer->pev->punchangle.y * 100.0f);
	args.origin = m_pPlayer->pev->origin;
	args.velocity = m_pPlayer->pev->velocity;

	EV_FireMK46(&args);
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.6f;

	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		KickBack(1.8, 0.65, 0.45, 0.125, 5.0, 3.5, 8);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		KickBack(1.1, 0.5, 0.3, 0.06, 4.0, 3.0, 8);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		KickBack(0.75, 0.325, 0.25, 0.025, 3.5, 2.5, 9);
	}
	else
	{
		KickBack(0.8, 0.35, 0.3, 0.03, 3.75, 3.0, 9);
	}
}

bool CMK46::Reload()
{
	if (m_iClip < 13 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)	// at this point, we shall use full_reload. (accroading to model)
	{
		m_iClip = 0;
	}

	if (DefaultReload(m_pItemInfo->m_iMaxClip,
		m_iClip ? RELOAD : RELOAD_EMPTY,
		m_iClip ? RELOAD_TIME : RELOAD_EMPTY_TIME,
		1.16f))
	{
		m_flAccuracy = ACCURACY_BASELINE;
		return true;
	}

	// KF2 ???
	if (m_pPlayer->pev->weaponanim != INSPECTION)
	{
		if (m_bInZoom)
			SecondaryAttack();

		SendWeaponAnim(INSPECTION);
		m_flTimeWeaponIdle = INSPECTION_TIME;
	}

	return false;
}
