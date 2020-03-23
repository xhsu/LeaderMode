/*

Remastered Date: Mar 22 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CACR::m_usEvent = 0;
int CACR::m_iShell = 0;

void CACR::Precache()
{
	PRECACHE_NECESSARY_FILES(ACR);

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/acr.sc");
}

#endif

bool CACR::Deploy()
{
	m_flAccuracy = 0.2f;
	m_iShotsFired = 0;

	return DefaultDeploy(ACR_VIEW_MODEL, ACR_WORLD_MODEL, ACR_DRAW, "carbine");
}

void CACR::SecondaryAttack()
{
	m_bInZoom = !m_bInZoom;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

	if (!g_vecGunOfsGoal.LengthSquared())
	{
		g_vecGunOfsGoal = Vector(-18.0f, -20.0f, 2.0f);
		gHUD::m_iFOV = 75;	// allow clients to predict the zoom.
	}
	else
	{
		g_vecGunOfsGoal = g_vecZero;
		gHUD::m_iFOV = 90;
	}

	g_flGunOfsMovingSpeed = 14.0f;
#else
	// just zoom a liiiiittle bit.
	// this doesn't suffer from the same bug where the gunofs does, since the FOV was actually sent from SV.
	if (m_bInZoom)
	{
		m_pPlayer->pev->fov = 75;
		EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, "weapons/steelsight_in.wav", 0.75f, ATTN_STATIC);
	}
	else
	{
		m_pPlayer->pev->fov = 90;
		EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, "weapons/steelsight_out.wav", 0.75f, ATTN_STATIC);
	}
#endif
}

void CACR::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		ACRFire(0.035f + (0.4f * m_flAccuracy));
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
	{
		ACRFire(0.035f + (0.07f * m_flAccuracy));
	}
	else if (m_bInZoom)	// decrease spread while scoping.
	{
		ACRFire(0.01f * m_flAccuracy);
	}
	else
	{
		ACRFire(0.02f * m_flAccuracy);
	}
}

void CACR::ACRFire(float flSpread, float flCycleTime)
{
	m_iShotsFired++;

	m_flAccuracy = (float(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 215.0f) + 0.3f;

	if (m_flAccuracy > 1.0f)
		m_flAccuracy = 1.0f;

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

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, ACR_EFFECTIVE_RANGE, ACR_PENETRATION, m_iPrimaryAmmoType, ACR_DAMAGE, ACR_RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	SendWeaponAnim(UTIL_SharedRandomFloat(m_pPlayer->random_seed, ACR_SHOOT1, ACR_SHOOT3));
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

	EV_FireACR(&args);
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9f;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		KickBack(1.0, 0.45, 0.275, 0.05, 4.0, 2.5, 7);
	}
	else if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		KickBack(1.25, 0.45, 0.22, 0.18, 5.5, 4.0, 5);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		KickBack(0.575, 0.325, 0.2, 0.011, 3.25, 2.0, 8);
	}
	else
	{
		KickBack(0.625, 0.375, 0.25, 0.0125, 3.5, 2.25, 8);
	}
}

bool CACR::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, ACR_RELOAD, ACR_RELOAD_TIME))
	{
		m_flAccuracy = 0.0f;
		return true;
	}

	return false;
}

void CACR::WeaponIdle()
{
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
	SendWeaponAnim(ACR_IDLE1);
}

DECLARE_STANDARD_RESET_MODEL_FUNC(ACR)
