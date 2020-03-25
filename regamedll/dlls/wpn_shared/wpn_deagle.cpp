/*

Remastered Date: Mar 22 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CDEagle::m_usEvent = 0;
int CDEagle::m_iShell = 0;

void CDEagle::Precache()
{
	PRECACHE_NECESSARY_FILES(DEagle);

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/deagle.sc");
}

#endif

bool CDEagle::Deploy()
{
	m_flAccuracy = 0.9f;

	return DefaultDeploy(DEagle_VIEW_MODEL, DEagle_WORLD_MODEL, DEAGLE_DRAW, "onehanded", DEAGLE_DEPLOY_TIME);
}

void CDEagle::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		DEagleFire(1.5f * (1.0f - m_flAccuracy));
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		DEagleFire(0.25f * (1.0f - m_flAccuracy));
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		DEagleFire(0.115f * (1.0f - m_flAccuracy));
	}
	else
	{
		DEagleFire(0.13f * (1.0f - m_flAccuracy));
	}
}

void CDEagle::SecondaryAttack()
{
	m_bInZoom = !m_bInZoom;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

	if (!g_vecGunOfsGoal.LengthSquared())
	{
		g_vecGunOfsGoal = Vector(-6.18f, -5.0f, 0.6f);
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

void CDEagle::DEagleFire(float flSpread, float flCycleTime)
{
	if (++m_iShotsFired > 1)
	{
		return;
	}

	if (m_bInZoom)	// decrease spread while scoping.
		flSpread *= 0.5f;

	if (m_flLastFire != 0.0)
	{
		m_flAccuracy -= (0.4f - (gpGlobals->time - m_flLastFire)) * 0.35f;

		if (m_flAccuracy > 0.9f)
		{
			m_flAccuracy = 0.9f;
		}
		else if (m_flAccuracy < 0.55f)
		{
			m_flAccuracy = 0.55f;
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
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, DEAGLE_EFFECTIVE_RANGE, DEAGLE_PENETRATION, m_iPrimaryAmmoType, DEAGLE_DAMAGE, DEAGLE_RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	int seq = UTIL_SharedRandomFloat(m_pPlayer->random_seed, DEAGLE_SHOOT1, DEAGLE_SHOOT2);
	if (m_iClip == 0)
		seq = DEAGLE_SHOOT_EMPTY;

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

	EV_FireDEagle(&args);
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8f;

	m_pPlayer->m_vecVAngleShift.x -= 2.5f;
}

bool CDEagle::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, DEAGLE_RELOAD, DEAGLE_RELOAD_TIME))
	{
		m_flAccuracy = 0.9f;
		return true;
	}

	return false;
}

void CDEagle::WeaponIdle()
{
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
	SendWeaponAnim(DEAGLE_IDLE1);
}

DECLARE_STANDARD_RESET_MODEL_FUNC(DEagle)
