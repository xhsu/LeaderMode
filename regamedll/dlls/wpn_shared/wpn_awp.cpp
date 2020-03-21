/*

Remastered Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CAWP::m_usEvent = 0;
int CAWP::m_iShell = 0;

void CAWP::Precache()
{
	PRECACHE_MODEL("models/weapons/v_awp.mdl");
	PRECACHE_MODEL("models/weapons/w_awp.mdl");
	PRECACHE_MODEL("models/weapons/p_awp.mdl");

	PRECACHE_SOUND("weapons/awp1.wav");
	PRECACHE_SOUND("weapons/boltpull1.wav");
	PRECACHE_SOUND("weapons/boltup.wav");
	PRECACHE_SOUND("weapons/boltdown.wav");
	PRECACHE_SOUND("weapons/zoom.wav");
	PRECACHE_SOUND("weapons/awp_deploy.wav");
	PRECACHE_SOUND("weapons/awp_clipin.wav");
	PRECACHE_SOUND("weapons/awp_clipout.wav");

	m_iShell = PRECACHE_MODEL("models/rshell_big.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/awp.sc");
}

#endif

bool CAWP::Deploy()
{
	if (DefaultDeploy("models/weapons/v_awp.mdl", "models/weapons/p_awp.mdl", AWP_DRAW, "rifle"))
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + AWP_DEPLOY_TIME;
		m_flNextPrimaryAttack = m_pPlayer->m_flNextAttack;
		m_flNextSecondaryAttack = m_pPlayer->m_flNextAttack;

#ifndef CLIENT_DLL
		// VFX corespounding to model anim.
		// we can only do this on SV side.
		m_pPlayer->m_flEjectBrass = gpGlobals->time + 0.43f;
		m_pPlayer->m_iShellModelIndex = m_iShell;
#endif

		return true;
	}

	return false;
}

void CAWP::SecondaryAttack()
{
	// this is the delay for the m_bResumeZoom.
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.25f;

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
		g_vecGunOfsGoal = Vector(-6.3f, -5.0f, 1.6f);
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

void CAWP::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		AWPFire(0.85, AWP_FIRE_INTERVAL);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
	{
		AWPFire(0.25, AWP_FIRE_INTERVAL);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 10)
	{
		AWPFire(0.1, AWP_FIRE_INTERVAL);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		AWPFire(0.0, AWP_FIRE_INTERVAL);
	}
	else
	{
		AWPFire(0.001, AWP_FIRE_INTERVAL);
	}
}

void CAWP::AWPFire(float flSpread, float flCycleTime)
{
	if (m_pPlayer->pev->fov != DEFAULT_FOV)
	{
		m_pPlayer->m_bResumeZoom = true;
		m_pPlayer->m_iLastZoom = m_pPlayer->pev->fov;

		// reset a fov
		m_pPlayer->pev->fov = DEFAULT_FOV;
	}
	// If we are not zoomed in, the bullet diverts more.
	else
	{
		flSpread += 0.08f;
	}

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

	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, AWP_EFFECTIVE_RANGE, AWP_PENETRATION, m_pAmmoInfo->m_iBulletBehavior, AWP_DAMAGE, AWP_RANGE_MODIFER, m_pPlayer->pev, true, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	SendWeaponAnim(UTIL_SharedRandomLong(m_pPlayer->random_seed, AWP_SHOOT1, AWP_SHOOT3));
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.x * 100), FALSE, FALSE);
	
	m_pPlayer->m_flEjectBrass = gpGlobals->time + AWP_TIME_SHELL_EJ;
	m_pPlayer->m_iShellModelIndex = m_iShell;

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

	EV_FireAWP(&args);
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
	m_pPlayer->m_vecVAngleShift.x -= 4.0f;
	m_pPlayer->m_vecVAngleShift.y -= UTIL_SharedRandomFloat(m_pPlayer->random_seed + AWP_PENETRATION, -2.0f, 2.0f);
}

bool CAWP::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, AWP_RELOAD, AWP_RELOAD_TIME))
	{
		m_pPlayer->SetAnimation(PLAYER_RELOAD);

#ifndef CLIENT_DLL
		// VFX corespounding to model anim.
		// we can only do this on SV side.
		m_pPlayer->m_flEjectBrass = gpGlobals->time + 1.91f;
		m_pPlayer->m_iShellModelIndex = m_iShell;
#endif

		if (m_pPlayer->pev->fov != DEFAULT_FOV)
		{
			m_pPlayer->pev->fov = 10;

			SecondaryAttack();
		}

		return true;
	}

	return false;
}

void CAWP::WeaponIdle()
{
	if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase() && m_iClip)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0f;
		SendWeaponAnim(AWP_IDLE);
	}
}

float CAWP::GetMaxSpeed()
{
	if (int(m_pPlayer->pev->fov) == DEFAULT_FOV)
		return AWP_MAX_SPEED;

	// Slower speed when zoomed in.
	return AWP_MAX_SPEED_ZOOM;
}
