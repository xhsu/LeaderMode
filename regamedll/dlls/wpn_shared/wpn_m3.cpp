/*

Remastered Date: Mar 13 2020

*/

#include "precompiled.h"
#include "..\weapons.h"

#ifndef CLIENT_DLL

unsigned short CM3::m_usEvent = 0;
int CM3::m_iShell = 0;

void CM3::Precache()
{
	PRECACHE_MODEL("models/v_m3.mdl");
	PRECACHE_MODEL("models/w_m3.mdl");
	PRECACHE_MODEL("models/p_m3.mdl");

	PRECACHE_SOUND("weapons/m3_insertshell.wav");
	PRECACHE_SOUND("weapons/m3_pump.wav");
	PRECACHE_SOUND("weapons/reload1.wav");
	PRECACHE_SOUND("weapons/reload3.wav");

	m_usEvent = PRECACHE_EVENT(1, "events/m3.sc");
	m_iShell = PRECACHE_MODEL("models/shotgunshell.mdl");
}

#endif

void CM3::Think(void)
{
	if (m_pPlayer->m_afButtonReleased & IN_ATTACK)
		m_bAllowNextEmptySound = true;	// only one empty sound per time.
}

bool CM3::Deploy()
{
	m_bAllowNextEmptySound = true;

	return DefaultDeploy("models/v_m3.mdl", "models/p_m3.mdl", M3_DRAW, "shotgun");
}

void CM3::PostFrame(void)
{
	if (m_bInReload)
	{
		if (m_flNextInsertAnim <= gpGlobals->time && m_iClip < m_pItemInfo->m_iMaxClip)
		{
			SendWeaponAnim(M3_RELOAD);
			m_pPlayer->SetAnimation(PLAYER_RELOAD);

			m_flNextInsertAnim = gpGlobals->time + KSG12_TIME_INSERT;
		}

		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0 && m_flNextAddAmmo <= gpGlobals->time && m_iClip < m_pItemInfo->m_iMaxClip)
		{
			m_iClip++;
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

			m_flNextAddAmmo = gpGlobals->time + KSG12_TIME_INSERT;	// yeah, that's right, not KSG12_TIME_ADD_AMMO.
			// TODO: reload sfx.
		}

		if (((m_iClip >= m_pItemInfo->m_iMaxClip || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) && m_flNextInsertAnim <= gpGlobals->time)
			|| m_bSetForceStopReload || m_pPlayer->pev->button & (IN_ATTACK | IN_RUN))
		{
			SendWeaponAnim(M3_PUMP);
			m_pPlayer->m_flNextAttack = KSG12_TIME_AFTER_RELOAD;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + KSG12_TIME_AFTER_RELOAD;

			m_bInReload = false;
		}
	}
	else
		CBaseWeapon::PostFrame();	// for emergency.
}

void CM3::PrimaryAttack()
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

	m_iClip--;
	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	int iSeedOfs = m_pPlayer->FireBuckshots(KSG12_PROJECTILE_COUNT, m_pPlayer->GetGunPosition(), gpGlobals->v_forward, KSG12_CONE_VECTOR, KSG12_EFFECTIVE_RANGE, KSG12_DAMAGE, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, int(m_pPlayer->pev->punchangle.x * 100.0f), m_pPlayer->random_seed, FALSE, FALSE);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}

	// shell should be placed at SV side.
	m_pPlayer->m_flEjectBrass = gpGlobals->time + KSG12_SHELL_EJECT;
	m_pPlayer->m_iShellModelIndex = m_iShell;
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = false;
	args.bparam2 = false;
	args.ducking = gEngfuncs.pEventAPI->EV_LocalPlayerDucking();
	args.entindex = gEngfuncs.GetLocalPlayer()->index;
	args.flags = FEV_NOTHOST | FEV_RELIABLE | FEV_CLIENT;
	args.fparam1 = 0;
	args.fparam2 = 0;
	args.iparam1 = int(m_pPlayer->pev->punchangle.x * 100.0f);
	args.iparam2 = m_pPlayer->random_seed;
	args.origin = m_pPlayer->pev->origin;
	args.velocity = m_pPlayer->pev->velocity;

	EV_FireM3(&args);
#endif

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + KSG12_FIRE_INTERVAL;

	if (m_iClip != 0)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5f;
	else
		m_flTimeWeaponIdle = KSG12_FIRE_INTERVAL;

	if (m_pPlayer->pev->flags & FL_ONGROUND)
		m_pPlayer->pev->punchangle.x -= UTIL_SharedRandomLong(m_pPlayer->random_seed + iSeedOfs, 4, 6);
	else
		m_pPlayer->pev->punchangle.x -= UTIL_SharedRandomLong(m_pPlayer->random_seed + iSeedOfs, 8, 11);
}

void CM3::SecondaryAttack(void)
{
#ifdef CLIENT_DLL
	// TODO: client should have the zoom now.
#endif
}

void CM3::WeaponIdle(void)
{
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.7f;	// this KSG12 model has looping vfx.
	SendWeaponAnim(M3_IDLE);
}

bool CM3::Reload(void)
{
	if (m_iClip >= m_pItemInfo->m_iMaxClip || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return false;

	m_iShotsFired = 0;
	m_bInReload = true;
	m_pPlayer->m_flNextAttack = KSG12_TIME_START_RELOAD;
	m_flNextInsertAnim = gpGlobals->time + KSG12_TIME_START_RELOAD;
	m_flNextAddAmmo = gpGlobals->time + KSG12_TIME_ADD_AMMO + KSG12_TIME_START_RELOAD;

	SendWeaponAnim(M3_START_RELOAD);
	return true;
}

void CM3::PlayEmptySound(void)
{
	if (!m_bAllowNextEmptySound)
		return;

	m_bAllowNextEmptySound = false;
	CBaseWeapon::PlayEmptySound();
}
