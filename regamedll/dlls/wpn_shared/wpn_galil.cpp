#include "precompiled.h"

LINK_ENTITY_TO_CLASS(weapon_galil, CGalil)

void CGalil::Spawn()
{
	Precache();

	m_iId = WEAPON_CM901;
	SET_MODEL(edict(), "models/w_galil.mdl");

	m_iDefaultAmmo = iinfo()->m_iMaxClip;

	// Get ready to fall down
	FallInit();

	// extend
	CBasePlayerWeapon::Spawn();
}

void CGalil::Precache()
{
	PRECACHE_MODEL("models/v_galil.mdl");
	PRECACHE_MODEL("models/w_galil.mdl");

	PRECACHE_SOUND("weapons/galil-1.wav");
	PRECACHE_SOUND("weapons/galil-2.wav");
	PRECACHE_SOUND("weapons/galil_clipout.wav");
	PRECACHE_SOUND("weapons/galil_clipin.wav");
	PRECACHE_SOUND("weapons/galil_boltpull.wav");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireGalil = PRECACHE_EVENT(1, "events/galil.sc");
}

BOOL CGalil::Deploy()
{
	m_flAccuracy = 0.2f;
	m_iShotsFired = 0;
	iShellOn = 1;

	return DefaultDeploy("models/v_galil.mdl", "models/p_galil.mdl", GALIL_DRAW, "ak47", UseDecrement() != FALSE);
}

void CGalil::SecondaryAttack()
{
	;
}

void CGalil::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = GetNextAttackDelay(0.15);
		return;
	}

	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		GalilFire(0.04 + (0.3 * m_flAccuracy), 0.0875, FALSE);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
	{
		GalilFire(0.04 + (0.07 * m_flAccuracy), 0.0875, FALSE);
	}
	else
	{
		GalilFire(0.0375 * m_flAccuracy, 0.0875, FALSE);
	}
}

void CGalil::GalilFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	Vector vecAiming, vecSrc, vecDir;
	int flag;

	m_bDelayFire = true;
	m_iShotsFired++;

	m_flAccuracy = ((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200) + 0.35f;

	if (m_flAccuracy > 1.25f)
		m_flAccuracy = 1.25f;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = GetNextAttackDelay(0.2);
		}

		if (TheBots)
		{
			TheBots->OnEvent(EVENT_WEAPON_FIRED_ON_EMPTY, m_pPlayer);
		}

		return;
	}

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	vecSrc = m_pPlayer->GetGunPosition();
	vecAiming = gpGlobals->v_forward;

	float flBaseDamage = GALIL_DAMAGE;
	vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, 8192, 2, BULLET_PLAYER_556MM,
		flBaseDamage, GALIL_RANGE_MODIFER, m_pPlayer->pev, false, m_pPlayer->random_seed);

#ifdef CLIENT_WEAPONS
	flag = FEV_NOTHOST;
#else
	flag = 0;
#endif

	PLAYBACK_EVENT_FULL(flag, m_pPlayer->edict(), m_usFireGalil, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 10000000), int(m_pPlayer->pev->punchangle.y * 10000000), FALSE, FALSE);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(flCycleTime);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.28f;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		KickBack(1.0, 0.45, 0.28, 0.045, 3.75, 3.0, 7);
	}
	else if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		KickBack(1.2, 0.5, 0.23, 0.15, 5.5, 3.5, 6);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		KickBack(0.6, 0.3, 0.2, 0.0125, 3.25, 2.0, 7);
	}
	else
	{
		KickBack(0.65, 0.35, 0.25, 0.015, 3.5, 2.25, 7);
	}
}

void CGalil::Reload()
{
	if (DefaultReload(iinfo()->m_iMaxClip, GALIL_RELOAD, GALIL_RELOAD_TIME))
	{
		m_pPlayer->SetAnimation(PLAYER_RELOAD);

		m_flAccuracy = 0.2f;
		m_iShotsFired = 0;
		m_bDelayFire = false;
	}
}

void CGalil::WeaponIdle()
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
		SendWeaponAnim(GALIL_IDLE1, UseDecrement() != FALSE);
	}
}
