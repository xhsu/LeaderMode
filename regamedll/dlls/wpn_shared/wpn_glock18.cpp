/*

Remastered Date: Mar 25 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CG18C::m_usEvent = 0;
int CG18C::m_iShell = 0;

void CG18C::Precache()
{
	PRECACHE_NECESSARY_FILES(G18C);

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/glock18.sc");
}

#endif

bool CG18C::Deploy()
{
	m_flAccuracy = 0.9f;

	return DefaultDeploy(G18C_VIEW_MODEL, G18C_WORLD_MODEL, GLOCK18_DRAW, "onehanded", GLOCK18_DEPLOY_TIME);
}

void CG18C::SecondaryAttack()
{
	m_bInZoom = !m_bInZoom;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

	if (!g_vecGunOfsGoal.LengthSquared())
	{
		g_vecGunOfsGoal = Vector(-11, -10, 2.4f);
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

void CG18C::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		GLOCK18Fire(1.0f * (1.0f - m_flAccuracy));
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		GLOCK18Fire(0.165f * (1.0f - m_flAccuracy));
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		GLOCK18Fire(0.075f * (1.0f - m_flAccuracy));
	}
	else
	{
		GLOCK18Fire(0.1f * (1.0f - m_flAccuracy));
	}
}

void CG18C::GLOCK18Fire(float flSpread, float flCycleTime)
{
	// LUNA: G18C is allowed to fire in FULL AUTO.

	if (m_bInZoom)
		flSpread *= 0.5f;

	if (m_flLastFire)
	{
		// Mark the time of this shot and determine the accuracy modifier based on the last shot fired...
		m_flAccuracy -= (0.325f - (gpGlobals->time - m_flLastFire)) * 0.275f;

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

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	// non-silenced
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, GLOCK18_EFFECTIVE_RANGE, GLOCK18_PENETRATION, m_iPrimaryAmmoType, GLOCK18_DAMAGE, GLOCK18_RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	int seq = UTIL_SharedRandomFloat(m_pPlayer->random_seed, GLOCK18_SHOOT, GLOCK18_SHOOT3);
	if (m_iClip == 0)
		seq = GLOCK18_SHOOT_EMPTY;

	SendWeaponAnim(seq);	// LUNA: I don't know why, but this has to be done on SV side, or client fire anim would be override.
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), m_iClip == 0, FALSE);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		// HEV suit - indicate out of ammo condition
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

	EV_Fireglock18(&args);
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5f;

	// tweaked from PM9/MAC10
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		KickBack(1.8, 0.55, 0.4, 0.05, 4.75, 3.75, 5);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		KickBack(1.0, 0.45, 0.25, 0.035, 3.5, 2.75, 7);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		KickBack(0.8, 0.4, 0.175, 0.03, 2.75, 2.5, 10);
	}
	else
	{
		KickBack(0.825, 0.425, 0.2, 0.03, 3.0, 2.75, 9);
	}
}

bool CG18C::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, GLOCK18_RELOAD, GLOCK18_RELOAD_TIME))
	{
		m_flAccuracy = 0.9f;
		return true;
	}

	return false;
}

void CG18C::WeaponIdle()
{
	// only idle if the slid isn't back
	if (m_iClip)
	{
		auto flRand = RANDOM_FLOAT(0, 1);
		int iAnim = 0;

		if (flRand <= 0.3f)
		{
			iAnim = GLOCK18_IDLE3;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0625f;
		}
		else if (flRand <= 0.6f)
		{
			iAnim = GLOCK18_IDLE1;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.75f;
		}
		else
		{
			iAnim = GLOCK18_IDLE2;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5f;
		}

		SendWeaponAnim(iAnim);
	}
}

DECLARE_STANDARD_RESET_MODEL_FUNC(G18C)
