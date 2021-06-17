/*

Remastered Date: Mar 13 2020
Remastered II Date: Jun 17 2021

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)
Sound - iDkGK
Sprite - HL&CL

*/

#include "precompiled.h"


float CUSP::GetSpread(void)
{
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

	return DefaultSpread(SPREAD_BASELINE * (1.0f - m_flAccuracy), 0.1f, 0.75f, 2.0f, 5.0f);
}

void CUSP::PlaybackEvent(const Vector2D& vSpread)
{
#ifndef CLIENT_DLL
	PLAYBACK_EVENT_FULL(
		FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL,
		m_pPlayer->edict(),
		m_usEvent<CUSP>,
		0,
		(float*)&g_vecZero, (float*)&g_vecZero,
		vSpread.x, vSpread.y,
		int(m_pPlayer->pev->punchangle.x * 100),
		int(m_pPlayer->pev->punchangle.y * 100),
		m_iClip == 0,
		false	// unused.
	);
#endif
}

void CUSP::ApplyClientFPFiringVisual(const Vector2D& vSpread)
{
#ifdef CLIENT_DLL
	auto idx = gEngfuncs.GetLocalPlayer()->index;
	bool bDucking = gEngfuncs.pEventAPI->EV_LocalPlayerDucking();

	auto vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors(vecAngles);

	EV_MuzzleFlash();

	// first personal gun smoke.
	Vector smoke_origin = g_pViewEnt->attachment[0] - gpGlobals->v_forward * 3.0f;
	float base_scale = RANDOM_FLOAT(0.1, 0.25);

	EV_HLDM_CreateSmoke(smoke_origin, gpGlobals->v_forward, 0, base_scale, 7, 7, 7, EV_PISTOL_SMOKE, m_pPlayer->pev->velocity, false, 35);
	EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], gpGlobals->v_forward, 20, base_scale + 0.1, 10, 10, 10, EV_WALL_PUFF, m_pPlayer->pev->velocity, false, 35);
	EV_HLDM_CreateSmoke(g_pViewEnt->attachment[0], gpGlobals->v_forward, 40, base_scale, 13, 13, 13, EV_WALL_PUFF, m_pPlayer->pev->velocity, false, 35);

	// shoot anim.
	SendWeaponAnim(AcquireShootAnim());

	Vector ShellVelocity, ShellOrigin;
	if (!cl_righthand->value)
		EV_GetDefaultShellInfo(idx, bDucking, m_pPlayer->pev->origin, m_pPlayer->pev->velocity, ShellVelocity, ShellOrigin, gpGlobals->v_forward, gpGlobals->v_right, gpGlobals->v_up, 36.0, -14.0, -14.0);
	else
		EV_GetDefaultShellInfo(idx, bDucking, m_pPlayer->pev->origin, m_pPlayer->pev->velocity, ShellVelocity, ShellOrigin, gpGlobals->v_forward, gpGlobals->v_right, gpGlobals->v_up, 36.0, -14.0, 14.0);

	ShellOrigin = g_pViewEnt->attachment[1];	// use the weapon attachment instead.
	ShellVelocity *= 0.5;
	ShellVelocity.z += 45.0;

	EV_EjectBrass(ShellOrigin, ShellVelocity, vecAngles.yaw, m_iShell<CUSP>, TE_BOUNCE_SHELL, 5);

	Vector vecSrc = EV_GetGunPosition(idx, bDucking, m_pPlayer->pev->origin);

	// original API: vol = 1.0, attn = 0.8, pitch = 87~105
	EV_PlayGunFire(vecSrc + gpGlobals->v_forward * 10.0f, FIRE_SFX, QUIET_GUN_VOLUME, 1.0, RANDOM_LONG(87, 105));

	EV_HLDM_FireBullets(idx,
		gpGlobals->v_forward, gpGlobals->v_right, gpGlobals->v_up,
		1, vecSrc, gpGlobals->v_forward,
		vSpread, EFFECTIVE_RANGE, m_iPrimaryAmmoType,
		PENETRATION);
#endif
}

void CUSP::ApplyRecoil(void)
{
	m_pPlayer->pev->punchangle.x -= 2.0f;
}

bool CUSP::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip, RELOAD, RELOAD_TIME))
	{
		m_flAccuracy = ACCURACY_BASELINE;
		return true;
	}

	return false;
}

void CUSP::ApplyClientTPFiringVisual(struct event_args_s* args)
{
#ifdef CLIENT_DLL
	bool silencer_on = !args->bparam2;	// useless
	bool empty = !args->bparam1;
	int idx = args->entindex;
	Vector origin(args->origin);
	Vector angles(
		args->iparam1 / 100.0f + args->angles[0],
		args->iparam2 / 100.0f + args->angles[1],
		args->angles[2]
	);
	Vector velocity(args->velocity);

	Vector forward, right, up;
	AngleVectors(angles, forward, right, up);

	Vector ShellVelocity, ShellOrigin;
	EV_GetDefaultShellInfo(args->entindex, args->ducking, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0);
	EV_EjectBrass(ShellOrigin, ShellVelocity, angles.yaw, m_iShell<CUSP>, TE_BOUNCE_SHELL, 5);

	Vector vecSrc = EV_GetGunPosition(args->entindex, args->ducking, origin);
	Vector2D vSpread = Vector2D(args->fparam1, args->fparam2);

	// original API: vol = 1.0, attn = 0.8, pitch = 87~105
	EV_PlayGunFire(vecSrc + forward * 10.0f, FIRE_SFX, QUIET_GUN_VOLUME, 1.0, RANDOM_LONG(87, 105));

	EV_HLDM_FireBullets(idx,
		forward, right, up,
		1, vecSrc, forward,
		vSpread, EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_USP].m_iAmmoType,
		PENETRATION);
#endif
}