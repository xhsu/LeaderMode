/*

Remastered Date: Mar 21 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CAnaconda::m_usEvent = 0;
int CAnaconda::m_iShell = 0;

void CAnaconda::Precache()
{
	PRECACHE_NECESSARY_FILES(ANACONDA);

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/anaconda.sc");
}

#else

void CAnaconda::Think(void)
{
	if (m_flShellRain > 0.0f && m_flShellRain <= gEngfuncs.GetClientTime())
	{
		m_flShellRain = 0;

		for (int i = 0; i < m_pItemInfo->m_iMaxClip; i++)
			EV_EjectBrass(g_pViewEnt->attachment[1] + Vector(RANDOM_FLOAT(-10, 10), RANDOM_FLOAT(-10, 10), RANDOM_FLOAT(-10, 0)), m_pPlayer->pev->velocity * 0.75f, m_pPlayer->pev->angles.yaw, g_iPShell, TE_BOUNCE_SHELL);
	}
}

#endif

bool CAnaconda::Deploy()
{
	if (DefaultDeploy(ANACONDA_VIEW_MODEL, ANACONDA_WORLD_MODEL, ANACONDA_DRAW, "onehanded", ANACONDA_DEPLOY_TIME))
	{
#ifdef CLIENT_DLL
		// reset this when switching gun.
		m_flShellRain = 0;
#endif
		m_flAccuracy = 0.9f;
		return true;
	}

	return false;
}

void CAnaconda::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		AnacondaFire(1.5f * (1.0f - m_flAccuracy));
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		AnacondaFire(0.255f * (1.0f - m_flAccuracy));
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		AnacondaFire(0.075f * (1.0f - m_flAccuracy));
	}
	else
	{
		AnacondaFire(0.15 * (1.0f - m_flAccuracy));
	}
}

void CAnaconda::SecondaryAttack()
{
	m_bInZoom = !m_bInZoom;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

	if (!g_vecGunOfsGoal.LengthSquared())
	{
		g_vecGunOfsGoal = Vector(-9.61f, -5.0f, 3.0f);
		gHUD::m_iFOV = 85;	// allow clients to predict the zoom.
	}
	else
	{
		g_vecGunOfsGoal = g_vecZero;
		gHUD::m_iFOV = 90;
	}

	// this model needs faster.
	g_flGunOfsMovingSpeed = 13.0f;
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

void CAnaconda::AnacondaFire(float flSpread, float flCycleTime)
{
	if (++m_iShotsFired > 1)
	{
		return;
	}

	if (m_bInZoom)	// decrease spread while scoping.
		flSpread *= 0.5f;

	if (m_flLastFire != 0.0f)
	{
		m_flAccuracy -= (0.325f - (gpGlobals->time - m_flLastFire)) * 0.3f;

		if (m_flAccuracy > 0.9f)
		{
			m_flAccuracy = 0.9f;
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

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = BIG_EXPLOSION_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, ANACONDA_EFFECTIVE_RANGE, ANACONDA_PENETRATION, m_pAmmoInfo->m_iBulletBehavior, ANACONDA_DAMAGE, ANACONDA_RANGE_MODIFER, m_pPlayer->pev, true, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	int seq = UTIL_SharedRandomFloat(m_pPlayer->random_seed, ANACONDA_SHOOT1, ANACONDA_SHOOT2);
	if (m_iClip == 0)
		seq = ANACONDA_SHOOT_EMPTY;

	SendWeaponAnim(seq);	// LUNA: I don't know why, but this has to be done on SV side, or client fire anim would be override.
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), m_iClip == 0, FALSE);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = m_iClip == 0;
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

	EV_FireAnaconda(&args);
#endif

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
	m_pPlayer->m_vecVAngleShift.x -= 2;
}

bool CAnaconda::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, ANACONDA_RELOAD, ANACONDA_RELOAD_TIME))
	{
		m_flAccuracy = 0.9f;

#ifdef CLIENT_DLL
		m_flShellRain = gEngfuncs.GetClientTime() + 0.95f;
#endif

		return true;
	}

	return false;
}

void CAnaconda::WeaponIdle()
{
	if (m_iClip)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0625f;
		SendWeaponAnim(ANACONDA_IDLE);
	}
}
