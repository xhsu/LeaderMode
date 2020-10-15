/*

Remastered Date: Mar 28 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Matoilet

*/

#include "precompiled.h"


#ifndef CLIENT_DLL

unsigned short CMK46::m_usEvent = 0;
int CMK46::m_iShell = 0;

void CMK46::Precache()
{
	PRECACHE_NECESSARY_FILES(MK46);

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/mk46.sc");
}

#else

void CMK46::Think(void)
{
	CBaseWeapon::Think();

	// just keep updating model during empty-reload.
	if (m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY)
	{
		g_pViewEnt->curstate.body = CalcBodyParam();
	}
}

#endif

bool CMK46::Deploy()
{
	m_flAccuracy = 0.2f;
	m_iShotsFired = 0;

	return DefaultDeploy(MK46_VIEW_MODEL, MK46_WORLD_MODEL, (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? MK46_DRAW_FIRST : MK46_DRAW, "m249", (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? MK46_DRAW_FIRST_TIME : MK46_DEPLOY_TIME);
}

void CMK46::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		MK46Fire(0.045F + (0.5F * m_flAccuracy));
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
	{
		MK46Fire(0.045F + (0.095F * m_flAccuracy));
	}
	else if (m_bInZoom)	// decrease spread while scoping.
	{
		MK46Fire(0.015f * m_flAccuracy);
	}
	else
	{
		MK46Fire(0.03F * m_flAccuracy);
	}
}

void CMK46::MK46Fire(float flSpread, float flCycleTime)
{
	m_iShotsFired++;
	m_bDelayRecovery = true;

	m_flAccuracy = (float(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 175.0f) + 0.4f;

	if (m_flAccuracy > 0.9f)
		m_flAccuracy = 0.9f;

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

	m_pPlayer->m_iWeaponVolume = MK46_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, MK46_EFFECTIVE_RANGE, MK46_PENETRATION, m_iPrimaryAmmoType, MK46_DAMAGE, MK46_RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	int seq = UTIL_SharedRandomFloat(m_pPlayer->random_seed, MK46_SHOOT1, MK46_SHOOT3);
	if (!m_bInZoom)
		seq = MK46_SHOOT_UNSCOPE;

	SendWeaponAnim(seq);
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), m_bInZoom, FALSE);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = m_bInZoom;
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

	EV_FireMK46(&args);
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.6f;

	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		KickBack(1.8, 0.65, 0.45, 0.125, 5.0, 3.5, 8);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		KickBack(1.1, 0.5, 0.3, 0.06, 4.0, 3.0, 8);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		KickBack(0.75, 0.325, 0.25, 0.025, 3.5, 2.5, 9);
	}
	else
	{
		KickBack(0.8, 0.35, 0.3, 0.03, 3.75, 3.0, 9);
	}
}

bool CMK46::Reload()
{
	if (m_iClip < 13)	// at this point, we shall use full_reload. (accroading to model)
	{
		m_iClip = 0;
	}

	if (DefaultReload(m_pItemInfo->m_iMaxClip, m_iClip ? MK46_RELOAD : MK46_RELOAD_EMPTY, m_iClip ? MK46_RELOAD_TIME : MK46_RELOAD_EMPTY_TIME))
	{
		m_flAccuracy = 0.2f;
		return true;
	}

	return false;
}

int CMK46::CalcBodyParam(void)
{
	BodyEnumInfo_t info[] =
	{
		{ 0, 2 },	// hands		= 0;
		{ 0, 1 },

		{ 0, 1 },	// weapon		= 2;
		{ 0, 1 },
		{ 0, 1 },

		{ 0, 16 },	// bullets		= 5;
		{ 0, 6 },	// optical sight= 6;
		{ 0, 4 },	// muzzle		= 7;
		{ 0, 2 },	// laser		= 8;
	};

	// by default, this weapon has:
	// red dot optical sight.
	// laser.

	info[6].body = 3;
	info[8].body = 1;

	// as the magazine is getting lesser, this number is getting bigger. (later model)
	info[5].body = Q_clamp(16 - m_iClip, 0, 15);

	// in EMPTY reload, after we remove the empty mag, the new mag should be full of bullets.
	if (m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY)
	{
		if (m_pPlayer->m_flNextAttack < 3.77f)	// in this anim, a new mag was taken out after around 2.46. thus, 6.23 - 2.46 ~= 3.77f.
		{
			info[5].body = 0;	// a full loaded bullets chain.
		}
	}

	return CalcBody(info, sizeof(info) / sizeof(BodyEnumInfo_t));	// elements count of the info[].
}

DECLARE_STANDARD_RESET_MODEL_FUNC(MK46)
