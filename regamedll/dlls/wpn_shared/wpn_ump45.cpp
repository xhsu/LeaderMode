/*

Remastered Date: Mar 25 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CUMP45::m_usEvent = 0;
int CUMP45::m_iShell = 0;

void CUMP45::Precache()
{
	PRECACHE_NECESSARY_FILES(UMP45);

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/ump45.sc");
}

#else

static constexpr int BULLETS = 8;
static constexpr int SIGHT = 10;
static constexpr int LASER = 11;
static constexpr int MUZZLE = 12;
static constexpr int SELECTOR = 13;

static constexpr int SILENCER = 1;
static constexpr int HOLOGRAPHIC = 1;

int CUMP45::CalcBodyParam(void)
{
	static BodyEnumInfo_t info[] =
	{
		{ 0, 1 },	// right hand	= 0;
		{ 0, 2 },	// left hand	= 1;
		{ 0, 1 },	// right sleeve	= 2;
		{ 0, 2 },	// left sleeve	= 3;

		{ 0, 1 },	// weapon		= 4;
		{ 0, 1 },
		{ 0, 1 },
		{ 0, 1 },

		{ 0, 11 },	// bullets		= 8;

		{ 0, 1 },

		{ 0, 2 },	// sight		= 10;
		{ 0, 2 },	// laser		= 11;
		{ 0, 2 },	// muzzle		= 12;
		{ 0, 2 },	// selector		= 13;
	};

	switch (m_iVariation)
	{
	case Role_SWAT:
	case Role_Medic:
	case Role_MadScientist:
		// holographic
		info[SIGHT].body = HOLOGRAPHIC;
		info[MUZZLE].body = FALSE;
		info[LASER].body = FALSE;
		break;

	case Role_Sharpshooter:
		// holography, laser
		info[SIGHT].body = HOLOGRAPHIC;
		info[MUZZLE].body = FALSE;
		info[LASER].body = TRUE;
		break;

	case Role_Assassin:
		// holography, silencer
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
	info[BULLETS].body = Q_clamp(10 - m_iClip, 0, 10);

	return CalcBody(info, _countof(info));	// elements count of the info[].
}

#endif

bool CUMP45::Deploy()
{
	m_flAccuracy = 0.4f;

	return DefaultDeploy(UMP45_VIEW_MODEL, UMP45_WORLD_MODEL, (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? UMP45_DRAW_FIRST : UMP45_DRAW, "m249", (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? UMP45_DRAW_FIRST_TIME : UMP45_DRAW_TIME);
}

void CUMP45::SecondaryAttack(void)
{
	switch (m_iVariation)
	{
	case Role_SWAT:
	case Role_Medic:
	case Role_MadScientist:
	case Role_Sharpshooter:
	case Role_Assassin:
		// holography
		DefaultSteelSight(Vector(-2.575f, 0, 0.015f), 80);
		break;

	default:
		// steel sight
		DefaultSteelSight(Vector(-2.57f, 0, 0.69f), 85);
		break;
	}
}

float CUMP45::GetSpread(void)
{
	m_flAccuracy = ((m_iShotsFired * m_iShotsFired) / 210) + 0.4f;

	if (m_flAccuracy > 1.0f)
		m_flAccuracy = 1.0f;

	return DefaultSpread(UMP45_SPREAD_BASELINE * m_flAccuracy, 0.1f, 0.75f, 1.0f, 5.0f);	// no additional penalty on dashing.
}

void CUMP45::UMP45Fire(float flSpread, float flCycleTime)
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

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, UMP45_EFFECTIVE_RANGE, UMP45_PENETRATION, m_iPrimaryAmmoType, UMP45_DAMAGE, UMP45_RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	SendWeaponAnim(m_bInZoom ? UMP45_SHOOT_AIM : UMP45_SHOOT);
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), m_bInZoom, m_iVariation == Role_Assassin);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = m_bInZoom;
	args.bparam2 = m_iVariation == Role_Assassin;
	args.ducking = gEngfuncs.pEventAPI->EV_LocalPlayerDucking();
	args.entindex = gEngfuncs.GetLocalPlayer()->index;
	args.flags = FEV_NOTHOST | FEV_RELIABLE | FEV_CLIENT | FEV_GLOBAL;
	args.fparam1 = vecDir.x;
	args.fparam2 = vecDir.y;
	args.iparam1 = int(m_pPlayer->pev->punchangle.x * 100.0f);
	args.iparam2 = int(m_pPlayer->pev->punchangle.y * 100.0f);
	args.origin = m_pPlayer->pev->origin;
	args.velocity = m_pPlayer->pev->velocity;

	EV_FireUMP45(&args);
#endif

	m_pPlayer->m_iWeaponVolume = UMP45_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		KickBack(0.125, 0.65, 0.55, 0.0475, 5.5, 4.0, 10);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		KickBack(0.55, 0.3, 0.225, 0.03, 3.5, 2.5, 10);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		KickBack(0.25, 0.175, 0.125, 0.02, 2.25, 1.25, 10);
	}
	else
	{
		KickBack(0.275, 0.2, 0.15, 0.0225, 2.5, 1.5, 10);
	}
}

bool CUMP45::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip,
		m_iClip ? UMP45_RELOAD : UMP45_RELOAD_EMPTY,
		m_iClip ? UMP45_RELOAD_TIME : UMP45_RELOAD_EMPTY_TIME,
		m_iClip ? 0.7667f : 0.4667f))
	{
		m_flAccuracy = 0.4f;
		return true;
	}

	// KF2 ???
	if (m_pPlayer->pev->weaponanim != UMP45_CHECKMAG)
	{
		if (m_bInZoom)
			SecondaryAttack();

		SendWeaponAnim(UMP45_CHECKMAG);
		m_flTimeWeaponIdle = UMP45_CHECKMAG_TIME;
	}

	return false;
}

DECLARE_STANDARD_RESET_MODEL_FUNC(UMP45)
