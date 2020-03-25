/*

Remastered Date: Mar 25 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CFN57::m_usEvent = 0;
int CFN57::m_iShell = 0;

void CFN57::Precache()
{
	PRECACHE_NECESSARY_FILES(FN57);

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/fiveseven.sc");
}

#endif

bool CFN57::Deploy()
{
	m_flAccuracy = 0.92f;

	return DefaultDeploy(FN57_VIEW_MODEL, FN57_WORLD_MODEL, FIVESEVEN_DRAW, "onehanded", FIVESEVEN_DEPLOY_TIME);
}

void CFN57::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		FiveSevenFire(1.5f * (1.0f - m_flAccuracy));
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		FiveSevenFire(0.255f * (1.0f - m_flAccuracy));
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		FiveSevenFire(0.075f * (1.0f - m_flAccuracy));
	}
	else
	{
		FiveSevenFire(0.15f * (1.0f - m_flAccuracy));
	}
}

void CFN57::SecondaryAttack()
{
	m_bInZoom = !m_bInZoom;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

	if (!g_vecGunOfsGoal.LengthSquared())
	{
		g_vecGunOfsGoal = Vector(-10.9f, -10.0f, 2.9f);
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

void CFN57::FiveSevenFire(float flSpread, float flCycleTime)
{
	if (++m_iShotsFired > 1)
	{
		return;
	}

	if (m_bInZoom)	// decrease spread while scoping.
		flSpread *= 0.5f;

	if (m_flLastFire != 0.0f)
	{
		m_flAccuracy -= (0.275f - (gpGlobals->time - m_flLastFire)) * 0.25f;

		if (m_flAccuracy > 0.92f)
		{
			m_flAccuracy = 0.92f;
		}
		else if (m_flAccuracy < 0.725f)
		{
			m_flAccuracy = 0.725f;
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

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, FIVESEVEN_EFFECTIVE_RANGE, FIVESEVEN_PENETRATION, m_iPrimaryAmmoType, FIVESEVEN_DAMAGE, FIVESEVEN_RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	int seq = UTIL_SharedRandomFloat(m_pPlayer->random_seed, FIVESEVEN_SHOOT1, FIVESEVEN_SHOOT2);
	if (m_iClip == 0)
		seq = FIVESEVEN_SHOOT_EMPTY;

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

	EV_Fire57(&args);
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

	m_pPlayer->m_vecVAngleShift.x -= 1.25f;	// lesser recoil than other pistols.
}

bool CFN57::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, FIVESEVEN_RELOAD, FIVESEVEN_RELOAD_TIME))
	{
		m_flAccuracy = 0.92f;
		return true;
	}

	return false;
}

void CFN57::WeaponIdle()
{
	if (m_iClip)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3.0625f;	// LUNA: what's this accurate number for?
		SendWeaponAnim(FIVESEVEN_IDLE);
	}
}

DECLARE_STANDARD_RESET_MODEL_FUNC(FN57)
