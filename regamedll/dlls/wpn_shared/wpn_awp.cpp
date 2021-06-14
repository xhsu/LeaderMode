/*

Remastered Date: Mar 20 2020
Reflesh Date: Nov 02 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)
Sound - iDkGK

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CAWP::m_usEvent = 0;
int CAWP::m_iShell = 0;

void CAWP::Precache()
{
	BaseClass::Precache();

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

static constexpr int PIN = 8;
static constexpr int UP = 0;
static constexpr int DOWN = 1;

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

	// pin downed vfx.
	if ((m_iClip <= 0 || m_flTimeChamberCleared) && (1 << m_pPlayer->pev->weaponanim) & BITS_PIN_UNINVOLVED_ANIM)
		info[PIN].body = DOWN;
	else
		info[PIN].body = UP;

	return CalcBody(info, _countof(info));
}

#endif

void CAWP::Think(void)
{
	if (m_flTimeChamberCleared != 0 && m_flTimeChamberCleared <= gpGlobals->time)
	{
		// you must playing designated anim to cancel this flag. also, you cannot in-scope.
		if ((1 << m_pPlayer->pev->weaponanim) & BITS_RECHAMBER_ANIM && m_pPlayer->pev->fov == DEFAULT_FOV)
		{
			// flag cleared. you may shoot now.
			m_flTimeChamberCleared = 0;
		}
		else
		{
			// sorry, you're doing something else to trick the system.
			m_flTimeChamberCleared = gpGlobals->time + 9999.0f;

			// also, block the shell vfx.
			m_pPlayer->m_flEjectBrass = 0;
		}
	}

	// make it post.
	CBaseWeapon::Think();
}

void CAWP::PrimaryAttack()
{
	// no rechamber, not shoot.
	if (m_flTimeChamberCleared)
	{
		// unscope during this anim.
		if (m_pPlayer->pev->fov != DEFAULT_FOV)
			SecondaryAttack();

		SendWeaponAnim(RECHAMBER);
		m_pPlayer->m_flNextAttack = TIME_RECHAMBER;
		m_flTimeWeaponIdle = TIME_RECHAMBER;	// prevent anim instant break.
		m_flTimeChamberCleared = gpGlobals->time + TIME_REC_SHELL_EJ;

#ifndef CLIENT_DLL
		// display the shell.
		m_pPlayer->m_flEjectBrass = gpGlobals->time + TIME_SHELL_EJ;
		m_pPlayer->m_iShellModelIndex = m_iShell;
#endif
		return;
	}

	// PRE: use 1 to compare whether it is the last shot.
	AWPFire(GetSpread(), m_iClip == 1 ? FIRE_LAST_INV : FIRE_INTERVAL);

	// POST: unzoom. suggested by InnocentBlue.
	// don't do it unless bullets still left.
	if (m_pPlayer->pev->fov != DEFAULT_FOV)
		SecondaryAttack();

	// only make shells during a normal shoot.
	// the SHOOT_LAST anim does not contain a shell ejecting behaviour.
	if (m_iClip)
	{
		m_flTimeChamberCleared = gpGlobals->time + TIME_SHELL_EJ;

#ifndef CLIENT_DLL
		m_pPlayer->m_flEjectBrass = gpGlobals->time + TIME_SHELL_EJ;
		m_pPlayer->m_iShellModelIndex = m_iShell;
#endif
	}

	// since m_flNextPrimaryAttack is involved in DefaultScopeSight(), we have to place a limit.
	// although m_flNextSecondaryAttack is set in AWPFire(), but actually after the SecondaryAttack() above, it's already invalid.
	m_flNextSecondaryAttack = m_iClip == 1 ? FIRE_LAST_INV : FIRE_INTERVAL;
}

void CAWP::AWPFire(float flSpread, float flCycleTime)
{
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
	if (m_iClip > 0)
		SendWeaponAnim(SHOOT_REC);
	else
		SendWeaponAnim(SHOOT_LAST);

	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.x * 100), m_iClip > 0, m_iVariation == Role_Assassin);
	
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = m_iClip > 0;
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

	EV_FireAWP(&args);
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flCycleTime;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + flCycleTime;	// LUNA: ultimate prevention against evil players.

	m_pPlayer->m_vecVAngleShift.x -= 12.0f;
	m_pPlayer->m_vecVAngleShift.y -= UTIL_SharedRandomFloat(m_pPlayer->random_seed + PENETRATION, -2.0f, 2.0f);
}

bool CAWP::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip,
		m_iClip ? RELOAD : RELOAD_EMPTY,
		m_iClip ? RELOAD_TIME : RELOAD_EMPTY_TIME,
		m_iClip ? 1.066f : 0.6f))
	{
#ifndef CLIENT_DLL
		if (!m_iClip)
		{
			// only the RELOAD_EMPTY involves a rechamber action.
			m_pPlayer->m_flEjectBrass = gpGlobals->time + RELOAD_EMPTY_SHELL;
			m_pPlayer->m_iShellModelIndex = m_iShell;
		}
		else
#endif
			// this should be decide on both side.
			// you can't use reload to avoid rechamber.
			if (m_iClip > 0 && m_flTimeChamberCleared)
				m_flTimeChamberCleared = gpGlobals->time + 9999.0f;

		return true;
	}

	// KF2 ???
	if (m_pPlayer->pev->weaponanim != CHECK_MAGAZINE)
	{
		if (m_pPlayer->pev->fov < DEFAULT_FOV)
			SecondaryAttack();

		SendWeaponAnim(CHECK_MAGAZINE);
		m_flTimeWeaponIdle = CHECKMAG_TIME;
	}

	return false;
}

bool CAWP::HolsterStart(void)
{
	// unzoom before holster.
	if (m_pPlayer->pev->fov != DEFAULT_FOV)
		SecondaryAttack();

	// then holster.
	return DefaultHolster(HOLSTER, HOLSTER_TIME);
}

float CAWP::GetMaxSpeed()
{
	if (int(m_pPlayer->pev->fov) == DEFAULT_FOV)
		return MAX_SPEED;

	// Slower speed when zoomed in.
	return MAX_SPEED_ZOOM;
}

float CAWP::GetSpread(void)
{
	float flSpread = DefaultSpread(SPREAD_BASELINE, 0.25f, 0.75f, 20, 50);

	if (m_pPlayer->pev->fov >= DEFAULT_FOV)
		flSpread *= 20;	// additional 2000% penalty for unscope shooting.

	return flSpread;
}
