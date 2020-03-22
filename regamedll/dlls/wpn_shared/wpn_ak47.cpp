/*

Remastered Date: Mar 21 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CAK47::m_usEvent = 0;
int CAK47::m_iShell = 0;

void CAK47::Precache()
{
	PRECACHE_NECESSARY_FILES(AK47);

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/ak47.sc");
}

#endif

bool CAK47::Deploy()
{
	m_flAccuracy = 0.2f;
	m_iShotsFired = 0;

	return DefaultDeploy(AK47_VIEW_MODEL, AK47_WORLD_MODEL, AK47_DRAW, "ak47");
}

void CAK47::SecondaryAttack()
{
	m_bInZoom = !m_bInZoom;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

	if (!g_vecGunOfsGoal.LengthSquared())
	{
		g_vecGunOfsGoal = Vector(-11.6f, -10.0f, 3.8f);
		gHUD::m_iFOV = 85;	// allow clients to predict the zoom.
	}
	else
	{
		g_vecGunOfsGoal = g_vecZero;
		gHUD::m_iFOV = 90;
	}

	g_flGunOfsMovingSpeed = 8.0f;
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

void CAK47::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		AK47Fire(0.04f + (0.4f * m_flAccuracy));
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
	{
		AK47Fire(0.04f + (0.07f * m_flAccuracy));
	}
	else if (m_bInZoom)	// decrease spread while scoping.
	{
		AK47Fire(0.015f * m_flAccuracy);
	}
	else
	{
		AK47Fire(0.0275f * m_flAccuracy);
	}
}

void CAK47::AK47Fire(float flSpread, float flCycleTime)
{
	m_iShotsFired++;

	m_flAccuracy = (float(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200.0f) + 0.35f;

	if (m_flAccuracy > 1.25f)
		m_flAccuracy = 1.25f;

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

	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, AK47_EFFECTIVE_RANGE, AK47_PENETRATION, m_pAmmoInfo->m_iBulletBehavior,
		AK47_DAMAGE, AK47_RANGE_MODIFER, m_pPlayer->pev, false, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	SendWeaponAnim(UTIL_SharedRandomFloat(m_pPlayer->random_seed, AK47_SHOOT1, AK47_SHOOT3));
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);

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

	EV_FireAK47(&args);
#endif

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9f;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		KickBack(1.5, 0.45, 0.225, 0.05, 6.5, 2.5, 7);
	}
	else if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		KickBack(2.0, 1.0, 0.5, 0.35, 9.0, 6.0, 5);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		KickBack(0.9, 0.35, 0.15, 0.025, 5.5, 1.5, 9);
	}
	else
	{
		KickBack(1.0, 0.375, 0.175, 0.0375, 5.75, 1.75, 8);
	}
}

bool CAK47::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, AK47_RELOAD, AK47_RELOAD_TIME))
	{
		m_flAccuracy = 0.2f;
		return true;
	}

	return false;
}

void CAK47::WeaponIdle()
{
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
	SendWeaponAnim(AK47_IDLE1);
}
