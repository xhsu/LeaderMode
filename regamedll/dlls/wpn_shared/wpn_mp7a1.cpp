#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CMP7A1::m_usEvent = 0;
int CMP7A1::m_iShell = 0;

void CMP7A1::Precache()
{
	PRECACHE_NECESSARY_FILES(MP7A1);

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/mp7a1.sc");
}

#endif

bool CMP7A1::Deploy()
{
	m_flAccuracy = 0.2f;
	m_iShotsFired = 0;

	return DefaultDeploy(MP7A1_VIEW_MODEL, MP7A1_WORLD_MODEL, MP7A1_DRAW, "onehanded");
}

void CMP7A1::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		MP7A1Fire(0.25f * m_flAccuracy);
	}
	else if (m_bInZoom)	// decrease spread while scoping.
	{
		MP7A1Fire(0.015f * m_flAccuracy);
	}
	else
	{
		MP7A1Fire(0.03f * m_flAccuracy);
	}
}

void CMP7A1::SecondaryAttack(void)
{
	m_bInZoom = !m_bInZoom;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

	if (!g_vecGunOfsGoal.LengthSquared())
	{
		g_vecGunOfsGoal = Vector(-11.6f, -11.0f, 4.0f);
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

void CMP7A1::MP7A1Fire(float flSpread, float flCycleTime)
{
	m_iShotsFired++;

	m_flAccuracy = (float(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200.0f) + 0.55f;

	if (m_flAccuracy > 1.4f)
		m_flAccuracy = 1.4f;

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
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, MP7A1_EFFECTIVE_RANGE, MP7A1_PENETRATION, m_pAmmoInfo->m_iBulletBehavior,
		MP7A1_DAMAGE, MP7A1_RANGE_MODIFER, m_pPlayer->pev, false, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	SendWeaponAnim(UTIL_SharedRandomLong(m_pPlayer->random_seed, MP7A1_SHOOT1, MP7A1_SHOOT3));
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), 5, FALSE);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
	//SERVER_PRINT(SharedVarArgs("SV:[seed: %d] v_angle: %f, %f, %f\n", m_pPlayer->random_seed, m_pPlayer->pev->v_angle.x, m_pPlayer->pev->v_angle.y, m_pPlayer->pev->v_angle.z));
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = 5;		// ???
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

	EV_FireMP7A1(&args);
	//gEngfuncs.pfnConsolePrint(SharedVarArgs("CL: [seed: %d] v_angle: %f, %f, %f\n", m_pPlayer->random_seed, m_pPlayer->pev->v_angle.x, m_pPlayer->pev->v_angle.y, m_pPlayer->pev->v_angle.z));
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		KickBack(1.1, 0.5, 0.35, 0.045, 4.5, 3.5, 6);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		KickBack(0.8, 0.4, 0.2, 0.03, 3.0, 2.5, 7);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		KickBack(0.7, 0.35, 0.125, 0.025, 2.5, 2.0, 10);
	}
	else
	{
		KickBack(0.725, 0.375, 0.15, 0.025, 2.75, 2.25, 9);
	}
}

bool CMP7A1::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, MP7A1_RELOAD, MP7A1_RELOAD_TIME))
	{
		m_flAccuracy = 0.2f;
		return true;
	}

	return false;
}

void CMP7A1::WeaponIdle()
{
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
	SendWeaponAnim(MP7A1_IDLE1);
}
