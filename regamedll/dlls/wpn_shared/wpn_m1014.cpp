/*

Remastered Date: Sep 27 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Matoilet

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CM1014::m_usEvent = 0;
int CM1014::m_iShell = 0;

void CM1014::Precache()
{
	PRECACHE_NECESSARY_FILES(M1014);

	m_usEvent = PRECACHE_EVENT(1, "events/m1014.sc");
	m_iShell = PRECACHE_MODEL("models/shotgunshell.mdl");
}

#else

static constexpr int BOLT	= 5;
static constexpr int SIGHT	= 6;
static constexpr int LASER	= 7;
static constexpr int SHELL	= 8;

int CM1014::CalcBodyParam(void)
{
	static BodyEnumInfo_t info[] =
	{
		{ 0, 1 },	// right hand	= 0;
		{ 0, 2 },	// left hand	= 1;
		{ 0, 1 },	// right sleeve	= 2;
		{ 0, 2 },	// left sleeve	= 3;

		{ 0, 1 },	// body			= 4;

		{ 0, 2 },	// bolt			= 5;
		{ 0, 2 },	// sight		= 6;
		{ 0, 2 },	// laser		= 7;
		{ 0, 2 },	// shell		= 8;
	};

	switch (m_iVariation)
	{
	case Role_SWAT:
	case Role_Breacher:
	case Role_Sharpshooter:
		// sight
		// laser
		info[SIGHT].body = TRUE;
		info[LASER].body = TRUE;
		break;

	case Role_Medic:
	case Role_MadScientist:
		// sight
		info[SIGHT].body = TRUE;
		info[LASER].body = FALSE;
		break;

	default:
		info[SIGHT].body = FALSE;
		info[LASER].body = FALSE;
		break;
	}

	// bolt stop vfx.
	if (m_iClip <= 0 && (1 << m_pPlayer->pev->weaponanim) & BITS_BOLT_STOP_ANIM)
		info[BOLT].body = TRUE;
	else
		info[BOLT].body = FALSE;

	return CalcBody(info, _countof(info));	// elements count of the info[].
}

#endif

void CM1014::Think(void)
{
	CBaseWeapon::Think();

	if (m_pPlayer->m_afButtonReleased & IN_ATTACK)
		m_bAllowNextEmptySound = true;	// only one empty sound per time.
}

bool CM1014::Deploy()
{
	m_bAllowNextEmptySound = true;

	return DefaultDeploy(M1014_VIEW_MODEL, M1014_WORLD_MODEL, (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? M1014_DRAW_FIRST : M1014_DRAW, "m249", (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? M1014_DRAW_FIRST_TIME : M1014_DRAW_TIME);
}

void CM1014::PostFrame(void)
{
	if (m_bInReload && !(m_bitsFlags & (WPNSTATE_MELEE | WPNSTATE_QUICK_THROWING)))
	{
		if (m_flNextInsertAnim <= gpGlobals->time && m_iClip < m_pItemInfo->m_iMaxClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
		{
			SendWeaponAnim(M1014_INSERT);
			m_pPlayer->SetAnimation(PLAYER_RELOAD);

			m_flNextInsertAnim = gpGlobals->time + M1014_TIME_INSERT;
		}

		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0 && m_flNextAddAmmo <= gpGlobals->time && m_iClip < m_pItemInfo->m_iMaxClip)
		{
			m_iClip++;
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

			if (m_bStartFromEmpty)
			{
				// this needs to treat specially.
				m_flNextAddAmmo = gpGlobals->time + (M1014_TIME_START_RELOAD_FIRST - M1014_TIME_ADD_AMMO_FIRST) + (M1014_TIME_INSERT - M1014_TIME_ADD_AMMO);
				m_bStartFromEmpty = false;
			}
			else
				m_flNextAddAmmo = gpGlobals->time + M1014_TIME_INSERT;	// yeah, that's right, not M1014_TIME_ADD_AMMO.

#ifndef CLIENT_DLL
			// SFX should be played at SV
			EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_ITEM, "weapons/ksg12/ksg12_insert.wav", VOL_NORM, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 31));
#endif
		}

		if (((m_iClip >= m_pItemInfo->m_iMaxClip || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) && m_flNextInsertAnim <= gpGlobals->time)
			|| m_bSetForceStopReload || m_pPlayer->pev->button & (IN_ATTACK | IN_RUN))
		{
			SendWeaponAnim(M1014_AFTER_RELOAD);
			m_pPlayer->m_flNextAttack = M1014_TIME_AFTER_RELOAD;
			m_flTimeWeaponIdle = M1014_TIME_AFTER_RELOAD;

			m_bInReload = false;
		}
	}
	else
		CBaseWeapon::PostFrame();	// for emergency.
}

void CM1014::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15f;
		return;
	}

	if (m_iClip <= 0)
	{
		PlayEmptySound();

#ifndef CLIENT_DLL
		if (TheBots)
		{
			TheBots->OnEvent(EVENT_WEAPON_FIRED_ON_EMPTY, m_pPlayer);
		}
#endif

		Reload();
		return;
	}

	m_pPlayer->m_iWeaponVolume = M1014_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
	m_iClip--;

	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	int iSeedOfs = m_pPlayer->FireBuckshots(M1014_PROJECTILE_COUNT, m_pPlayer->GetGunPosition(), gpGlobals->v_forward, M1014_CONE_VECTOR, M1014_EFFECTIVE_RANGE, M1014_DAMAGE, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	int iAnim = 0;
	if (m_iClip > 0)
	{
		if (m_bInZoom)
			iAnim = M1014_AIM_SHOOT;
		else
			iAnim = M1014_SHOOT;
	}
	else
	{
		if (m_bInZoom)
			iAnim = M1014_AIM_SHOOT_LAST;
		else
			iAnim = M1014_SHOOT_LAST;
	}

	// LUNA: I don't know why, but this has to be done on SV side, or client fire anim would be override.
	SendWeaponAnim(iAnim);
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float*)&g_vecZero, (float*)&g_vecZero, 0, 0, int(m_pPlayer->pev->punchangle.x * 100.0f), m_pPlayer->random_seed, FALSE, FALSE);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		// HEV suit - indicate out of ammo condition
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
	args.fparam1 = 0;
	args.fparam2 = 0;
	args.iparam1 = int(m_pPlayer->pev->punchangle.x * 100.0f);
	args.iparam2 = m_pPlayer->random_seed;
	args.origin = m_pPlayer->pev->origin;
	args.velocity = m_pPlayer->pev->velocity;

	EV_FireM1014(&args);
#endif

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + M1014_FIRE_INTERVAL;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.55f;

	if (m_pPlayer->pev->flags & FL_ONGROUND)
		m_pPlayer->m_vecVAngleShift.x -= UTIL_SharedRandomLong(m_pPlayer->random_seed + iSeedOfs, 3, 5);
	else
		m_pPlayer->m_vecVAngleShift.x -= UTIL_SharedRandomLong(m_pPlayer->random_seed + iSeedOfs, 7, 10);
}

void CM1014::SecondaryAttack(void)
{
	switch (m_iVariation)
	{
	case Role_SWAT:
	case Role_Breacher:
	case Role_Sharpshooter:
	case Role_Medic:
	case Role_MadScientist:
		DefaultSteelSight(Vector(-5.32f, -2, 0.14f), 80, 8.0f);
		break;

	default:
		DefaultSteelSight(Vector(-5.28, 0, 2.10f), 85, 10.0f);
		break;
	}
}

bool CM1014::Reload(void)
{
	if (m_bInReload)
		return false;

	if (m_iClip >= m_pItemInfo->m_iMaxClip || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		// KF2 ???
		if (m_iClip <= 0 && m_pPlayer->pev->weaponanim != M1014_INSPECTION)	// inspection anim.
		{
			SendWeaponAnim(M1014_INSPECTION);
			m_flTimeWeaponIdle = M1014_INSPECTION_TIME;
		}
		else if (m_iClip > 0 && m_pPlayer->pev->weaponanim != M1014_CHECKMAG)
		{
			SendWeaponAnim(M1014_CHECKMAG);
			m_flTimeWeaponIdle = M1014_CHECKMAG_TIME;
		}

		return false;
	}

	if (m_bInZoom)
		SecondaryAttack();	// close scope when we reload.

	m_iShotsFired = 0;
	m_bInReload = true;
	m_bStartFromEmpty = !!(m_iClip <= 0);
	m_pPlayer->m_flNextAttack = 0;//m_bStartFromEmpty ? M1014_TIME_START_RELOAD_FIRST : M1014_TIME_START_RELOAD;
	m_flNextInsertAnim = gpGlobals->time + (m_bStartFromEmpty ? M1014_TIME_START_RELOAD_FIRST : M1014_TIME_START_RELOAD);
	m_flNextAddAmmo = gpGlobals->time + (m_bStartFromEmpty ? M1014_TIME_ADD_AMMO_FIRST : (M1014_TIME_ADD_AMMO + M1014_TIME_START_RELOAD));

	SendWeaponAnim(m_bStartFromEmpty ? M1014_START_RELOAD_FIRST : M1014_START_RELOAD);
	return true;
}

void CM1014::PlayEmptySound(void)
{
	if (!m_bAllowNextEmptySound)
		return;

	m_bAllowNextEmptySound = false;
	CBaseWeapon::PlayEmptySound();
}

void CM1014::PushAnim(void)
{
	CBaseWeapon::PushAnim();

	// the way that time being store is a little bit different.
	m_Stack2.m_flNextAddAmmo = gpGlobals->time - m_flNextAddAmmo;
	m_Stack2.m_flNextInsertAnim = gpGlobals->time - m_flNextInsertAnim;
}

void CM1014::PopAnim(void)
{
	CBaseWeapon::PopAnim();

	// by this, the time will look like "freezed" during the push-pop time frame.
	m_flNextAddAmmo = gpGlobals->time - m_Stack2.m_flNextAddAmmo;
	m_flNextInsertAnim = gpGlobals->time - m_Stack2.m_flNextInsertAnim;
}

DECLARE_STANDARD_RESET_MODEL_FUNC(M1014)
