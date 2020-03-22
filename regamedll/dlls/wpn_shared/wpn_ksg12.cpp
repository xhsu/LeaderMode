/*

Remastered Date: Mar 13 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CKSG12::m_usEvent = 0;
int CKSG12::m_iShell = 0;

void CKSG12::Precache()
{
	PRECACHE_MODEL("models/weapons/v_ksg12.mdl");
	PRECACHE_MODEL("models/weapons/w_ksg12.mdl");
	PRECACHE_MODEL("models/weapons/p_ksg12.mdl");

	PRECACHE_SOUND("weapons/m3_insertshell.wav");
	PRECACHE_SOUND("weapons/m3_pump.wav");
	PRECACHE_SOUND("weapons/reload1.wav");
	PRECACHE_SOUND("weapons/reload3.wav");

	m_usEvent = PRECACHE_EVENT(1, "events/ksg12.sc");
	m_iShell = PRECACHE_MODEL("models/shotgunshell.mdl");
}

#endif

void CKSG12::Think(void)
{
	CBaseWeapon::Think();

	if (m_pPlayer->m_afButtonReleased & IN_ATTACK)
		m_bAllowNextEmptySound = true;	// only one empty sound per time.
}

bool CKSG12::Deploy()
{
	m_bAllowNextEmptySound = true;

	return DefaultDeploy("models/weapons/v_ksg12.mdl", "models/weapons/p_ksg12.mdl", KSG12_DRAW, "shotgun");
}

void CKSG12::PostFrame(void)
{
	if (m_bInReload && !(m_bitsFlags & WPNSTATE_MELEE))
	{
		if (m_flNextInsertAnim <= gpGlobals->time && m_iClip < m_pItemInfo->m_iMaxClip)
		{
			SendWeaponAnim(KSG12_INSERT);
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
			SendWeaponAnim(KSG12_AFTER_RELOAD);
			m_pPlayer->m_flNextAttack = KSG12_TIME_AFTER_RELOAD;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + KSG12_TIME_AFTER_RELOAD;

			m_bInReload = false;
		}
	}
	else
		CBaseWeapon::PostFrame();	// for emergency.
}

void CKSG12::PrimaryAttack()
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
	SendWeaponAnim(UTIL_SharedRandomLong(m_pPlayer->random_seed, KSG12_FIRE1, KSG12_FIRE2));	// LUNA: I don't know why, but this has to be done on SV side, or client fire anim would be override.
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, int(m_pPlayer->pev->punchangle.x * 100.0f), m_pPlayer->random_seed, FALSE, FALSE);

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
	args.flags = FEV_NOTHOST | FEV_RELIABLE | FEV_CLIENT | FEV_GLOBAL;
	args.fparam1 = 0;
	args.fparam2 = 0;
	args.iparam1 = int(m_pPlayer->pev->punchangle.x * 100.0f);
	args.iparam2 = m_pPlayer->random_seed;
	args.origin = m_pPlayer->pev->origin;
	args.velocity = m_pPlayer->pev->velocity;

	EV_FireKSG12(&args);
#endif

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + KSG12_FIRE_INTERVAL;

	if (m_iClip != 0)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5f;
	else
		m_flTimeWeaponIdle = KSG12_FIRE_INTERVAL;

	if (m_pPlayer->pev->flags & FL_ONGROUND)
		m_pPlayer->m_vecVAngleShift.x -= UTIL_SharedRandomLong(m_pPlayer->random_seed + iSeedOfs, 4, 6);
	else
		m_pPlayer->m_vecVAngleShift.x -= UTIL_SharedRandomLong(m_pPlayer->random_seed + iSeedOfs, 8, 11);
}

void CKSG12::SecondaryAttack(void)
{
	m_bInZoom = !m_bInZoom;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

	if (!g_vecGunOfsGoal.LengthSquared())
	{
		g_vecGunOfsGoal = Vector(-10.15f, -7.5f, 3.1f);
		gHUD::m_iFOV = 85;	// allow clients to predict the zoom.
	}
	else
	{
		g_vecGunOfsGoal = g_vecZero;
		gHUD::m_iFOV = 90;
	}

	// this model needs faster.
	g_flGunOfsMovingSpeed = 12.0f;
#else
	// just zoom a liiiiittle bit.
	// this doesn't suffer from the same bug where the gunofs does, since the FOV was actually sent from SV.
	if (m_bInZoom)
	{
		m_pPlayer->pev->fov = 85;
		EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, "weapons/steelsight_in.wav", 0.75f, ATTN_STATIC);
	}
	else
	{
		m_pPlayer->pev->fov = 90;
		EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, "weapons/steelsight_out.wav", 0.75f, ATTN_STATIC);
	}
#endif
}

void CKSG12::WeaponIdle(void)
{
	if (m_bInZoom)	// the idle anim would sabortage steel sight.
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.7f;	// this KSG12 model has looping vfx.
	SendWeaponAnim(KSG12_IDLE);
}

bool CKSG12::Reload(void)
{
	if (m_iClip >= m_pItemInfo->m_iMaxClip || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return false;

	if (m_bInZoom)
		SecondaryAttack();	// close scope when we reload.

	m_iShotsFired = 0;
	m_bInReload = true;
	m_pPlayer->m_flNextAttack = KSG12_TIME_START_RELOAD;
	m_flNextInsertAnim = gpGlobals->time + KSG12_TIME_START_RELOAD;
	m_flNextAddAmmo = gpGlobals->time + KSG12_TIME_ADD_AMMO + KSG12_TIME_START_RELOAD;

	SendWeaponAnim(KSG12_START_RELOAD);
	return true;
}

void CKSG12::PlayEmptySound(void)
{
	if (!m_bAllowNextEmptySound)
		return;

	m_bAllowNextEmptySound = false;
	CBaseWeapon::PlayEmptySound();
}

void CKSG12::PushAnim(void)
{
	CBaseWeapon::PushAnim();

	// the way that time being store is a little bit different.
	m_Stack2.m_flNextAddAmmo	= gpGlobals->time - m_flNextAddAmmo;
	m_Stack2.m_flNextInsertAnim	= gpGlobals->time - m_flNextInsertAnim;
}

void CKSG12::PopAnim(void)
{
	CBaseWeapon::PopAnim();

	// by this, the time will look like "freezed" during the push-pop time frame.
	m_flNextAddAmmo		= gpGlobals->time - m_Stack2.m_flNextAddAmmo;
	m_flNextInsertAnim	= gpGlobals->time - m_Stack2.m_flNextInsertAnim;
}
