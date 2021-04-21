/*

Created Date: Apr 20 2021

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)
Sound - iDkGK

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CPSG1::m_usEvent = 0;
int CPSG1::m_iShell = 0;

void CPSG1::Precache()
{
	PRECACHE_NECESSARY_FILES(PSG1);

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/psg1.sc");
}

#else

static constexpr unsigned LHAND = 1U;
static constexpr unsigned LSLEEVE = 3U;
static constexpr unsigned HIDE = 1U;
static constexpr unsigned SHOW = 0U;

static constexpr unsigned SCOPE = 7U;
static constexpr unsigned NO_SCOPE = 1U;
static constexpr unsigned HENSOLDT = 0U;

static constexpr unsigned MUZZLE = 8U;
static constexpr unsigned NO_MUZZLE = 0U;
static constexpr unsigned SILENCER = 1U;

static constexpr unsigned MAGAZINE = 9U;
static constexpr unsigned FULL = 0U;
static constexpr unsigned ONE_LEFT = 1U;
static constexpr unsigned EMPTY = 2U;

int CPSG1::CalcBodyParam(void)
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

		{ 0, 2 },	// scope		= 7;
		{ 0, 2 },	// muzzle		= 8;
		{ 0, 3 },	// magazine		= 9;
	};

	// mag state control.
	switch (m_iClip)
	{
	case 0:	// empty mag. the follower is shown.
		info[MAGAZINE].body = EMPTY;
		break;

	case 1:
		info[MAGAZINE].body = ONE_LEFT;
		break;

	default:	// m_iClip >= 2
		info[MAGAZINE].body = FULL;
		break;
	}

	// only consider variation when not morphing..
	switch (m_iVariation)
	{
	case Role_Sharpshooter:
		info[SCOPE].body = HENSOLDT;
		info[MUZZLE].body = NO_MUZZLE;
		break;

	case Role_Assassin:
		info[SCOPE].body = NO_SCOPE;
		info[MUZZLE].body = SILENCER;
		break;

	default:
		info[SCOPE].body = NO_SCOPE;
		info[MUZZLE].body = NO_MUZZLE;
		break;
	}

	return CalcBody(info, _countof(info));
}

#endif

bool CPSG1::Deploy()
{
	m_flAccuracy = 0.2f;
	m_iShotsFired = 0;

	return DefaultDeploy(PSG1_VIEW_MODEL, PSG1_WORLD_MODEL,
		(m_bitsFlags & WPNSTATE_DRAW_FIRST) ? DRAW_FIRST : DRAW,
		"rifle",
		(m_bitsFlags & WPNSTATE_DRAW_FIRST) ? DRAW_FIRST_TIME : DEPLOY_TIME);
}

float CPSG1::GetSpread()
{
	if (m_flLastFire != 0.0f)
	{
		m_flAccuracy = (gpGlobals->time - m_flLastFire) * 0.35f + 0.65f;

		if (m_flAccuracy > 0.98f)
		{
			m_flAccuracy = 0.98f;
		}
	}

	float flSpread = DefaultSpread(SPREAD_BASELINE * (1.0f - m_flAccuracy), 0.25f, 0.75f, 20, 50);

	if (m_pPlayer->pev->fov >= DEFAULT_FOV)
		flSpread *= 20;	// additional 2000% penalty for unscope shooting.

	return flSpread;
}

void CPSG1::PSG1Fire(float flSpread, float flCycleTime)
{
	if (++m_iShotsFired > 1)
	{
		return;
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

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, EFFECTIVE_RANGE, PENETRATION, m_iPrimaryAmmoType, DAMAGE, RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	SendWeaponAnim(SHOOT);

	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.x * 100), m_iVariation == Role_Assassin, false);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = m_iVariation == Role_Assassin;
	args.bparam2 = false;	// unused.
	args.ducking = gEngfuncs.pEventAPI->EV_LocalPlayerDucking();
	args.entindex = gEngfuncs.GetLocalPlayer()->index;
	args.flags = FEV_NOTHOST | FEV_RELIABLE | FEV_CLIENT | FEV_GLOBAL;
	args.fparam1 = vecDir.x;
	args.fparam2 = vecDir.y;
	args.iparam1 = int(m_pPlayer->pev->punchangle.x * 100.0f);
	args.iparam2 = int(m_pPlayer->pev->punchangle.y * 100.0f);
	args.origin = m_pPlayer->pev->origin;
	args.velocity = m_pPlayer->pev->velocity;

	EV_FirePSG1(&args);
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + FIRE_ANIMTIME;

	m_pPlayer->pev->punchangle.x -= UTIL_SharedRandomFloat(m_pPlayer->random_seed + PENETRATION, 0.75, 1.25) + m_pPlayer->pev->punchangle.x * 0.25;
	m_pPlayer->pev->punchangle.y += UTIL_SharedRandomFloat(m_pPlayer->random_seed + PENETRATION + 1, -0.75, 0.75);
}

bool CPSG1::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip,
		m_iClip ? RELOAD : RELOAD_EMPTY,
		m_iClip ? RELOAD_TIME : RELOAD_EMPTY_TIME,
		m_iClip ? 0.9667F : 0.6333F))
	{
		m_flAccuracy = 0.2f;
		return true;
	}

	// KF2 ???
	if (m_pPlayer->pev->weaponanim != CHECK_MAGAZINE)
	{
		if (m_bInZoom)
			SecondaryAttack();

		SendWeaponAnim(CHECK_MAGAZINE);
		m_flTimeWeaponIdle = CHECK_MAGAZINE_TIME;
	}

	return false;
}

bool CPSG1::HolsterStart()
{
	// unzoom before holster.
	if (m_pPlayer->pev->fov != DEFAULT_FOV)
		SecondaryAttack();

	// then holster.
	return DefaultHolster(HOLSTER, HOLSTER_TIME);
}

float CPSG1::GetMaxSpeed()
{
	return (m_pPlayer->pev->fov >= DEFAULT_FOV) ? MAX_SPEED : MAX_SPEED_ZOOM;
}

DECLARE_STANDARD_RESET_MODEL_FUNC(PSG1)
