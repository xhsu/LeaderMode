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
		if (m_flNextInsertAnim <= gpGlobals->time && m_iClip < m_pItemInfo->m_iMaxClip)
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

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
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
	m_bInZoom = !m_bInZoom;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

	if (!g_vecGunOfsGoal.LengthSquared())
	{
		g_vecGunOfsGoal = Vector(-4.5f, -5.0f, 2.0f);
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

void CM1014::WeaponIdle()
{
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
	SendWeaponAnim((m_bitsFlags & WPNSTATE_DASHING) ? M1014_DASHING : M1014_IDLE);
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

bool CM1014::HolsterStart(void)
{
	SendWeaponAnim(M1014_HOLSTER);
	m_pPlayer->m_flNextAttack = M1014_HOLSTER_TIME;
	m_bitsFlags |= WPNSTATE_HOLSTERING;

	return true;
}

void CM1014::DashStart(void)
{
	if (m_bInReload)
		m_bInReload = false;

	if (m_bInZoom || m_pPlayer->pev->fov < 90)
	{
#ifndef CLIENT_DLL
		SecondaryAttack();
#else
		g_vecGunOfsGoal = g_vecZero;
		g_flGunOfsMovingSpeed = 12.0f;
		gHUD::m_iFOV = 90;
#endif
	}

	SendWeaponAnim(M1014_DASH_ENTER);
	m_pPlayer->m_flNextAttack = M1014_DASH_ENTER_TIME;
	m_flTimeWeaponIdle = M1014_DASH_ENTER_TIME;
	m_bitsFlags |= WPNSTATE_DASHING;
}

void CM1014::DashEnd(void)
{
	if (m_pPlayer->m_flNextAttack > 0.0f && m_pPlayer->pev->weaponanim == M1014_DASH_ENTER)
	{
		// this is how much you procees to the dashing phase.
		// for example, assuming the whole length is 1.0s, you start 0.7s and decide to cancel.
		// although there's only 0.3s to the dashing phase, but turning back still requires another equally 0.7s.
		// "m_pPlayer->m_flNextAttack" is the 0.3s of full length. you need to get the rest part, i.e. the 70%.
		float flRunStartUnplayedRatio = 1.0f - m_pPlayer->m_flNextAttack / M1014_DASH_ENTER_TIME;

		// stick on the last instance in the comment: 70% * 1.0s(full length) = 0.7s, this is the time we need to turning back.
		float flRunStopTimeLeft = M1014_DASH_EXIT_TIME * flRunStartUnplayedRatio;

		// play the anim.
		SendWeaponAnim(M1014_DASH_EXIT);

#ifdef CLIENT_DLL
		// why we are using the "0.3s" here?
		// this is because the g_flTimeViewModelAnimStart actually means how much time had passed since the anim was ordered to play.
		// if we need to play 0.7s, we have to told system we only played it for 0.3s. right?
		g_flTimeViewModelAnimStart = gEngfuncs.GetClientTime() - (M1014_DASH_EXIT_TIME - flRunStopTimeLeft);
#endif

		// force everything else to wait.
		m_pPlayer->m_flNextAttack = m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle = flRunStopTimeLeft;
	}

	// if RUN_START is normally played and finished, go normal.
	else
	{
		SendWeaponAnim(M1014_DASH_EXIT);
		m_pPlayer->m_flNextAttack = M1014_DASH_EXIT_TIME;
		m_flTimeWeaponIdle = M1014_DASH_EXIT_TIME;
	}

	// either way, we have to remove this flag.
	m_bitsFlags &= ~WPNSTATE_DASHING;
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
