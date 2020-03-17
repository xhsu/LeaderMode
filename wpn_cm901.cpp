/*

Remastered Date: Mar 15 2020
By Miracle(Innocent Blue)

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CCM901::m_usEvent = 0;
int CCM901::m_iShell = 0;

void CCM901::Precache()
{
	PRECACHE_MODEL("models/v_cm901.mdl");
	PRECACHE_MODEL("models/w_cm901.mdl");
	PRECACHE_MODEL("models/p_cm901.mdl");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/cm901.sc");
}

#else


#endif

bool CCM901::Deploy()
{
	m_flAccuracy = 0.92f;

	return DefaultDeploy("models/v_cm901.mdl", "models/p_cm901.mdl", CM901_DRAW, "rifle");
}

void CCM901::SecondaryAttack()
{
	m_bInZoom = !m_bInZoom;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

	if (!g_vecGunOfsGoal.Length())
	{
		g_vecGunOfsGoal = Vector(-4.6f, -10.0f, 2.4f);
		gHUD::m_iFOV = 85;	// allow clients to predict the zoom.
	}
	else
	{
		g_vecGunOfsGoal = g_vecZero;
		gHUD::m_iFOV = 90;
	}

	// this model needs faster.
	g_flGunOfsMovingSpeed = 10.0f;
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

void CCM901::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		CM901Fire(1.2f * (1.0f - m_flAccuracy), CM901_FIRE_INTERVAL);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		CM901Fire(0.225f * (1.0f - m_flAccuracy), CM901_FIRE_INTERVAL);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		CM901Fire(0.08f * (1.0f - m_flAccuracy), CM901_FIRE_INTERVAL);
	}
	else
	{
		CM901Fire(0.1f * (1.0f - m_flAccuracy), CM901_FIRE_INTERVAL);
	}
}

void CCM901::CM901Fire(float flSpread, float flCycleTime)
{
	if (++m_iShotsFired > 1)
	{
		return;
	}

	if (m_flLastFire != 0.0f)
	{
		m_flAccuracy -= (0.3f - (gpGlobals->time - m_flLastFire)) * 0.275f;

		if (m_flAccuracy > 0.92f)
		{
			m_flAccuracy = 0.92f;
		}
		else if (m_flAccuracy < 0.6f)
		{
			m_flAccuracy = 0.6f;
		}
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

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

	m_iClip--;

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;	// due to the silencer
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, CM901_EFFECTIVE_RANGE, CM901_PENETRATION, BULLET_PLAYER_556MM, CM901_DAMAGE, CM901_RANGE_MODIFER, m_pPlayer->pev, true, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), 0, m_iClip == 0, FALSE);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = m_iClip == 0;
	args.bparam2 = false;	// silencer
	args.ducking = gEngfuncs.pEventAPI->EV_LocalPlayerDucking();
	args.entindex = gEngfuncs.GetLocalPlayer()->index;
	args.flags = FEV_NOTHOST | FEV_RELIABLE | FEV_CLIENT | FEV_GLOBAL;
	args.fparam1 = vecDir.x;
	args.fparam2 = vecDir.y;
	args.iparam1 = int(m_pPlayer->pev->punchangle.x * 100.0f);
	args.iparam2 = 0;	// should be punchangle.y. but in USP, it's unused.
	args.origin = m_pPlayer->pev->origin;
	args.velocity = m_pPlayer->pev->velocity;

	EV_FireCM901(&args);
#endif

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
	m_pPlayer->pev->punchangle.x -= 2.0f;
}

bool CCM901::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, CM901_RELOAD, CM901_RELOAD_TIME))
	{
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
		m_flAccuracy = 0.92f;
		return true;
	}

	return false;
}

void CCM901::WeaponIdle()
{
	if (m_iClip)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0f;
		SendWeaponAnim(CM901_IDLE);
	}
}
