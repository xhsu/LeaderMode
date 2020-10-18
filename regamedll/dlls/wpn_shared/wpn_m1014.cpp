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
	PRECACHE_SOUND("weapons/m1014/m1014_insert.wav");

	m_usEvent = PRECACHE_EVENT(1, "events/m1014.sc");
	m_iShell = PRECACHE_MODEL("models/shotgunshell.mdl");
}

#else

int CM1014::CalcBodyParam(void)
{
	static BodyEnumInfo_t info[] =
	{
		{ 0, 1 },	// hands		= 0;
		{ 0, 1 },
		{ 0, 1 },	// mesh			= 2;
		{ 0, 1 },

		{ 0, 4 },	// scope		= 4;
		{ 0, 2 },	// muzzle		= 5;
		{ 0, 2 },	// laser		= 6;
	};

	switch (m_iVariation)
	{
	case Role_LeadEnforcer:
		// the lead enforcer's version contains only a muzzle compensator.

		info[4].body = 0;
		info[5].body = 1;
		info[6].body = 0;
		break;

	default:
		// by default, this weapon has:
		// reddot sight.
		// laser.

		info[4].body = 2;
		info[5].body = 0;
		info[6].body = 1;
		break;
	}

	return CalcBody(info, ARRAY_ELEM_COUNT(info));	// elements count of the info[].
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

			m_flNextAddAmmo = gpGlobals->time + M1014_TIME_INSERT;	// yeah, that's right, not M1014_TIME_ADD_AMMO.

#ifndef CLIENT_DLL
			// SFX should be played at SV
			EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_ITEM, "weapons/ksg12/ksg12_insert.wav", VOL_NORM, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 31));
#endif
		}

		if (((m_iClip >= m_pItemInfo->m_iMaxClip || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) && m_flNextInsertAnim <= gpGlobals->time)
			|| m_bSetForceStopReload || m_pPlayer->pev->button & (IN_ATTACK | IN_RUN))
		{
			SendWeaponAnim(m_bStartFromEmpty ? M1014_AFTER_RELOAD_RECHAMBER : M1014_AFTER_RELOAD);
			m_pPlayer->m_flNextAttack = m_bStartFromEmpty ? M1014_TIME_AR_REC : M1014_TIME_AFTER_RELOAD;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (m_bStartFromEmpty ? M1014_TIME_AR_REC : M1014_TIME_AFTER_RELOAD);

			m_bInReload = false;
			m_bStartFromEmpty = false;
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
	// LUNA: I don't know why, but this has to be done on SV side, or client fire anim would be override.
	SendWeaponAnim(m_bInZoom ? M1014_AIM_FIRE : M1014_FIRE);
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
	args.bparam1 = false;
	args.bparam2 = false;
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
	case Role_LeadEnforcer:
		// STEEL
		// Vector(-4.57f, -5, 2.2f)
		DefaultSteelSight(Vector(-4.5f, -5, 2), 85, 12.0f);
		break;

	default:
		// RED DOT
		DefaultSteelSight(Vector(-4.505f, -5, -0.09f), 85, 12.0f);
		break;
	}
}

bool CM1014::Reload(void)
{
	if (m_iClip >= m_pItemInfo->m_iMaxClip || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return false;

	if (m_bInZoom)
		SecondaryAttack();	// close scope when we reload.

	m_iShotsFired = 0;
	m_bInReload = true;
	m_bStartFromEmpty = !!(m_iClip <= 0);
	m_pPlayer->m_flNextAttack = M1014_TIME_START_RELOAD;
	m_flNextInsertAnim = gpGlobals->time + M1014_TIME_START_RELOAD;
	m_flNextAddAmmo = gpGlobals->time + M1014_TIME_ADD_AMMO + M1014_TIME_START_RELOAD;

	SendWeaponAnim(M1014_START_RELOAD);
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
