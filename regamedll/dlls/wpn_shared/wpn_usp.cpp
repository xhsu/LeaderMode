#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CUSP::m_usEvent = 0;
int CUSP::m_iShell = 0;

void CUSP::Precache()
{
	PRECACHE_MODEL("models/v_usp.mdl");
	PRECACHE_MODEL("models/w_usp.mdl");
	PRECACHE_MODEL("models/p_usp.mdl");

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/usp.sc");
}

#else


#endif

bool CUSP::Deploy()
{
	m_flAccuracy = 0.92f;

	return DefaultDeploy("models/v_usp.mdl", "models/p_usp.mdl", USP_UNSIL_DRAW, "onehanded");
}

void CUSP::SecondaryAttack()
{
#ifdef CLIENT_DLL
	// TODO: client should have the zoom now.
#endif
}

void CUSP::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		USPFire(1.2f * (1.0f - m_flAccuracy), USP_FIRE_INTERVAL);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		USPFire(0.225f * (1.0f - m_flAccuracy), USP_FIRE_INTERVAL);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		USPFire(0.08f * (1.0f - m_flAccuracy), USP_FIRE_INTERVAL);
	}
	else
	{
		USPFire(0.1f * (1.0f - m_flAccuracy), USP_FIRE_INTERVAL);
	}
}

void CUSP::USPFire(float flSpread, float flCycleTime)
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

	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, 4096, 1, BULLET_PLAYER_45ACP, USP_DAMAGE, USP_RANGE_MODIFER, m_pPlayer->pev, true, m_pPlayer->random_seed);

#ifdef CLIENT_WEAPONS
	int flag = FEV_NOTHOST;
#else
	int flag = 0;
#endif // CLIENT_WEAPONS

#ifndef CLIENT_DLL
	PLAYBACK_EVENT_FULL(flag, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), 0, m_iClip == 0, FALSE);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
#endif

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;
	m_pPlayer->pev->punchangle.x -= 2.0f;
}

bool CUSP::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, USP_UNSIL_RELOAD, USP_RELOAD_TIME))
	{
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
		m_flAccuracy = 0.92f;
		return true;
	}

	return false;
}

void CUSP::WeaponIdle()
{
	if (m_iClip)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0f;
		SendWeaponAnim(USP_UNSIL_IDLE);
	}
}
