/*

Remastered Date: Mar 22 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CSVD::m_usEvent = 0;
int CSVD::m_iShell = 0;

void CSVD::Precache()
{
	PRECACHE_NECESSARY_FILES(SVD);
	PRECACHE_SOUND("weapons/zoom.wav");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/svd.sc");
}

#endif

bool CSVD::Deploy()
{
	m_flAccuracy = 0.2f;
	return DefaultDeploy(SVD_VIEW_MODEL, SVD_WORLD_MODEL, SVD_DRAW, "mp5", SVD_DEPLOY_TIME);
}

void CSVD::SecondaryAttack()
{
	// this is the delay for the m_bResumeZoom.
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.3f;

	if (int(m_pPlayer->pev->fov) < 90)
	{
		m_pPlayer->pev->fov = 90;

#ifdef CLIENT_DLL
		// zoom out anim.
		g_vecGunOfsGoal = Vector();

		// manually set fade.
		gHUD::m_SniperScope.SetFadeFromBlack(5.0f, 0);
#endif
	}
	else
	{
		// get ready to zoom in.
		m_pPlayer->m_iLastZoom = 40;
		m_pPlayer->m_bResumeZoom = true;

#ifdef CLIENT_DLL
		// zoom out anim.
		g_vecGunOfsGoal = Vector(-5.2f, -6.0f, 0.25f);
#endif
	}

#ifndef CLIENT_DLL
	if (TheBots)
	{
		TheBots->OnEvent(EVENT_WEAPON_ZOOMED, m_pPlayer);
	}

	// SFX only emitted from SV.
	EMIT_SOUND(m_pPlayer->edict(), CHAN_ITEM, "weapons/zoom.wav", 0.2, 2.4);
#else
	g_flGunOfsMovingSpeed = 10.0f;
#endif

	// slow down while we zooming.
	m_pPlayer->ResetMaxSpeed();

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;
}

void CSVD::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		SVDFire(0.45f);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		SVDFire(0.15f);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		SVDFire(0.035f);
	}
	else
	{
		SVDFire(0.055f);
	}
}

void CSVD::SVDFire(float flSpread, float flCycleTime)
{
	// semi-auto is NOT full-auto with a lower fire rate.
	if (++m_iShotsFired > 1)
	{
		return;
	}

	if (m_pPlayer->pev->fov == DEFAULT_FOV)
	{
		flSpread += 0.025f;
	}

	if (m_flLastFire)
	{
		m_flAccuracy = (gpGlobals->time - m_flLastFire) * 0.3f + 0.55f;

		if (m_flAccuracy > 0.98f)
		{
			m_flAccuracy = 0.98f;
		}
	}
	else
	{
		m_flAccuracy = 0.98f;
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
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, (1 - m_flAccuracy) * flSpread, SVD_EFFECTIVE_RANGE, SVD_PENETRATION, m_pAmmoInfo->m_iBulletBehavior, SVD_DAMAGE, SVD_RANGE_MODIFER, m_pPlayer->pev, true, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	SendWeaponAnim(UTIL_SharedRandomLong(m_pPlayer->random_seed, SVD_SHOOT, SVD_SHOOT2));
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.x * 100), FALSE, FALSE);

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

	EV_FireSVD(&args);
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8f;

	m_pPlayer->m_vecVAngleShift.x -= UTIL_SharedRandomFloat(m_pPlayer->random_seed + 4, 0.95, 2.15);
	m_pPlayer->m_vecVAngleShift.y += UTIL_SharedRandomFloat(m_pPlayer->random_seed + 5, -0.75, 0.75);
}

bool CSVD::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, SVD_RELOAD, SVD_RELOAD_TIME))
	{
		m_flAccuracy = 0.2f;
		return true;
	}

	return false;
}

void CSVD::WeaponIdle()
{
	if (m_iClip)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0f;
		SendWeaponAnim(SVD_IDLE);
	}
}

float CSVD::GetMaxSpeed()
{
	return (int(m_pPlayer->pev->fov) >= DEFAULT_FOV) ? SVD_MAX_SPEED : SVD_MAX_SPEED_ZOOM;
}
