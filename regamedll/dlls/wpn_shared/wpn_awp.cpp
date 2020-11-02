/*

Remastered Date: Mar 20 2020
Reflesh Date: Nov 02 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CAWP::m_usEvent = 0;
int CAWP::m_iShell = 0;

void CAWP::Precache()
{
	PRECACHE_NECESSARY_FILES(AWP);

	m_iShell = PRECACHE_MODEL("models/rshell_big.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/awp.sc");
}

#else

static constexpr int MAGAZINE = 9;
static constexpr int EMPTY = 3;
static constexpr int FULL = 0;
static constexpr int TWO_LEFT = 1;
static constexpr int ONE_LEFT = 2;

static constexpr int MUZZLE = 7;
static constexpr int SILENCER = 2;
static constexpr int BREAKER = 1;

int CAWP::CalcBodyParam(void)
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

		{ 0, 3 },	// muzzle		= 7;
		{ 0, 2 },	// pin			= 8;
		{ 0, 4 },	// magazine		= 9;
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

	case 2:
		info[MAGAZINE].body = TWO_LEFT;
		break;

	default:	// m_iClip >= 3
		info[MAGAZINE].body = FULL;
		break;
	}

	// only consider variation when not morphing..
	switch (m_iVariation)
	{
	case Role_Sharpshooter:
		// muzzle breaker

		info[MUZZLE].body = BREAKER;
		break;

	case Role_Assassin:
		// silencer
		info[MUZZLE].body = SILENCER;
		break;

	default:
		info[MUZZLE].body = FALSE;
		break;
	}

	return CalcBody(info, _countof(info));
}

#endif

bool CAWP::Deploy()
{
	return DefaultDeploy(AWP_VIEW_MODEL, AWP_WORLD_MODEL,
						(m_bitsFlags & WPNSTATE_DRAW_FIRST) ? AWP_DRAW_FIRST : AWP_DRAW,
						"rifle",
						(m_bitsFlags & WPNSTATE_DRAW_FIRST) ? AWP_DRAW_FIRST_TIME : AWP_DEPLOY_TIME);
}

void CAWP::SecondaryAttack()
{
	DefaultScopeSight(Vector(-6.3f, -5.0f, 1.6f), 25);
}

void CAWP::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		AWPFire(0.85f);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
	{
		AWPFire(0.25f);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 10)
	{
		AWPFire(0.1f);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		AWPFire(0.0f);
	}
	else
	{
		AWPFire(0.001f);
	}
}

void CAWP::AWPFire(float flSpread, float flCycleTime)
{
	if (m_pPlayer->pev->fov != DEFAULT_FOV)
	{
		m_pPlayer->m_bResumeZoom = true;
		m_pPlayer->m_iLastZoom = m_pPlayer->pev->fov;

		// reset a fov
		m_pPlayer->pev->fov = DEFAULT_FOV;
	}
	// If we are not zoomed in, the bullet diverts more.
	else
	{
		flSpread += 0.08f;
	}

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

	m_pPlayer->m_iWeaponVolume = BIG_EXPLOSION_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, AWP_EFFECTIVE_RANGE, AWP_PENETRATION, m_iPrimaryAmmoType, AWP_DAMAGE, AWP_RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	SendWeaponAnim(UTIL_SharedRandomLong(m_pPlayer->random_seed, AWP_SHOOT1, AWP_SHOOT3));
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.x * 100), FALSE, FALSE);
	
	m_pPlayer->m_flEjectBrass = gpGlobals->time + AWP_TIME_SHELL_EJ;
	m_pPlayer->m_iShellModelIndex = m_iShell;

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = false;	// unused
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

	EV_FireAWP(&args);
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
	m_pPlayer->m_vecVAngleShift.x -= 4.0f;
	m_pPlayer->m_vecVAngleShift.y -= UTIL_SharedRandomFloat(m_pPlayer->random_seed + AWP_PENETRATION, -2.0f, 2.0f);
}

bool CAWP::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, AWP_RELOAD, AWP_RELOAD_TIME))
	{
#ifndef CLIENT_DLL
		// VFX corespounding to model anim.
		// we can only do this on SV side.
		m_pPlayer->m_flEjectBrass = gpGlobals->time + 3.2f;	// rechamber portion of reload anim.
		m_pPlayer->m_iShellModelIndex = m_iShell;
#endif
		return true;
	}

	// KF2 ???
	if (m_pPlayer->pev->weaponanim != AWP_CHECK_MAGAZINE)
	{
		if (m_bInReload)
			SecondaryAttack();

		SendWeaponAnim(AWP_CHECK_MAGAZINE);
		m_flTimeWeaponIdle = AWP_CHECKMAG_TIME;
	}

	return false;
}

float CAWP::GetMaxSpeed()
{
	if (int(m_pPlayer->pev->fov) == DEFAULT_FOV)
		return AWP_MAX_SPEED;

	// Slower speed when zoomed in.
	return AWP_MAX_SPEED_ZOOM;
}

DECLARE_STANDARD_RESET_MODEL_FUNC(AWP)
