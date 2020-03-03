#include "precompiled.h"

LINK_ENTITY_TO_CLASS(weapon_glock18, CGLOCK18)

void CGLOCK18::Spawn()
{
	Precache();

	m_iId = WEAPON_GLOCK18;
	SET_MODEL(edict(), "models/w_glock18.mdl");

	m_iWeaponState &= ~WPNSTATE_SHIELD_DRAWN;
	m_iDefaultAmmo = iinfo()->m_iMaxClip;
	m_bBurstFire = false;

	m_iGlock18ShotsFired = 0;
	m_flGlock18Shoot = 0;
	m_flAccuracy = 0.9f;

	// Get ready to fall down
	FallInit();

	// extend
	CBasePlayerWeapon::Spawn();
}

void CGLOCK18::Precache()
{
	PRECACHE_MODEL("models/v_glock18.mdl");
	PRECACHE_MODEL("models/w_glock18.mdl");
	PRECACHE_MODEL("models/shield/v_shield_glock18.mdl");

	PRECACHE_SOUND("weapons/glock18-1.wav");
	PRECACHE_SOUND("weapons/glock18-2.wav");
	PRECACHE_SOUND("weapons/clipout1.wav");
	PRECACHE_SOUND("weapons/clipin1.wav");
	PRECACHE_SOUND("weapons/sliderelease1.wav");
	PRECACHE_SOUND("weapons/slideback1.wav");
	PRECACHE_SOUND("weapons/357_cock1.wav");
	PRECACHE_SOUND("weapons/de_clipin.wav");
	PRECACHE_SOUND("weapons/de_clipout.wav");

	m_iShellId = m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usFireGlock18 = PRECACHE_EVENT(1, "events/glock18.sc");
}

BOOL CGLOCK18::Deploy()
{
	m_iWeaponState &= ~WPNSTATE_SHIELD_DRAWN;

	m_bBurstFire = false;
	m_iGlock18ShotsFired = 0;
	m_flGlock18Shoot = 0;
	m_flAccuracy = 0.9f;
	m_fMaxSpeed = GLOCK18_MAX_SPEED;

	m_pPlayer->m_bShieldDrawn = false;

	if (m_pPlayer->HasShield())
	{
		return DefaultDeploy("models/shield/v_shield_glock18.mdl", "models/shield/p_shield_glock18.mdl", GLOCK18_SHIELD_DRAW, "shieldgun", UseDecrement() != FALSE);
	}
	else if (RANDOM_LONG(0, 1))
	{
		return DefaultDeploy("models/v_glock18.mdl", "models/p_glock18.mdl", GLOCK18_DRAW, "onehanded", UseDecrement() != FALSE);
	}

	return DefaultDeploy("models/v_glock18.mdl", "models/p_glock18.mdl", GLOCK18_DRAW2, "onehanded", UseDecrement() != FALSE);
}

void CGLOCK18::SecondaryAttack()
{
	ShieldSecondaryFire(GLOCK18_SHIELD_UP, GLOCK18_SHIELD_DOWN);
}

void CGLOCK18::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		GLOCK18Fire(1.0 * (1 - m_flAccuracy), 0.05f, FALSE);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		GLOCK18Fire(0.165 * (1 - m_flAccuracy), 0.05f, FALSE);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		GLOCK18Fire(0.075 * (1 - m_flAccuracy), 0.05f, FALSE);
	}
	else
	{
		GLOCK18Fire(0.1 * (1 - m_flAccuracy), 0.05f, FALSE);
	}
}

void CGLOCK18::GLOCK18Fire(float flSpread, float flCycleTime, BOOL bFireBurst)
{
	Vector vecAiming, vecSrc, vecDir;
	int flag;

	if (bFireBurst)
	{
		m_iGlock18ShotsFired = 0;
	}

	// LUNA: G18C is allowed to fire in FULL AUTO.

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

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	// non-silenced
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	vecSrc = m_pPlayer->GetGunPosition();
	vecAiming = gpGlobals->v_forward;

	float flBaseDamage = GLOCK18_DAMAGE;
	vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, 8192, 1, BULLET_PLAYER_9MM, flBaseDamage, GLOCK18_RANGE_MODIFER, m_pPlayer->pev, true, m_pPlayer->random_seed);

#ifdef CLIENT_WEAPONS
	flag = FEV_NOTHOST;
#else
	flag = 0;
#endif

	PLAYBACK_EVENT_FULL(flag, m_pPlayer->edict(), m_usFireGlock18, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), m_iClip == 0, FALSE);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(flCycleTime);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5f;

	if (bFireBurst)
	{
		// Fire off the next two rounds
		m_iGlock18ShotsFired++;
		m_flGlock18Shoot = gpGlobals->time + 0.1f;
	}

	ResetPlayerShieldAnim();

	// LUNA: copied from MAC10 (PM9).
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		KickBack(1.3, 0.55, 0.4, 0.05, 4.75, 3.75, 5);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		KickBack(0.9, 0.45, 0.25, 0.035, 3.5, 2.75, 7);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		KickBack(0.75, 0.4, 0.175, 0.03, 2.75, 2.5, 10);
	}
	else
	{
		KickBack(0.775, 0.425, 0.2, 0.03, 3.0, 2.75, 9);
	}
}

void CGLOCK18::Reload()
{
	int iResult;

	if (m_pPlayer->HasShield())
		iResult = GLOCK18_SHIELD_RELOAD;
	else
		iResult = GLOCK18_RELOAD;

	if (DefaultReload(iinfo()->m_iMaxClip, iResult, GLOCK18_RELOAD_TIME))
	{
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
		m_flAccuracy = 0.9;
	}
}

void CGLOCK18::WeaponIdle()
{
	int iAnim;
	float flRand;

	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
	{
		return;
	}

	if (m_pPlayer->HasShield())
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;

		if (m_iWeaponState & WPNSTATE_SHIELD_DRAWN)
		{
			SendWeaponAnim(GLOCK18_SHIELD_IDLE, UseDecrement() != FALSE);
		}
	}
	// only idle if the slid isn't back
	else if (m_iClip)
	{
		flRand = RANDOM_FLOAT(0, 1);

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

		SendWeaponAnim(iAnim, UseDecrement() != FALSE);
	}
}
