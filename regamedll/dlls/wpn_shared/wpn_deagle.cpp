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

#else

void CDEagle::Think(void)
{
	CBaseWeapon::Think();

	// just keep updating model during empty-reload.
	if (m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY)
	{
		g_pViewEnt->curstate.body = CalcBodyParam();
	}
}

#endif

bool CDEagle::Deploy()
{
	m_flAccuracy = 0.9f;

	// this anim is slide-moving.
	if (m_iClip <= 0)
		m_bitsFlags &= ~WPNSTATE_DRAW_FIRST;

	return DefaultDeploy(DEagle_VIEW_MODEL,
		DEagle_WORLD_MODEL,
		(m_bitsFlags & WPNSTATE_DRAW_FIRST) ? DEAGLE_DRAW_FIRST : DEAGLE_DRAW,
		"onehanded",
		(m_bitsFlags & WPNSTATE_DRAW_FIRST) ? DEAGLE_DRAW_FIRST_TIME : DEAGLE_DRAW_TIME);
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

	m_pPlayer->m_iWeaponVolume = DEAGLE_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, DEAGLE_EFFECTIVE_RANGE, DEAGLE_PENETRATION, m_iPrimaryAmmoType, DEAGLE_DAMAGE, DEAGLE_RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	SendWeaponAnim(m_iClip == 0 ? DEAGLE_SHOOT_EMPTY : DEAGLE_SHOOT);	// LUNA: I don't know why, but this has to be done on SV side, or client fire anim would be override.
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
	if (DefaultReload(m_pItemInfo->m_iMaxClip,
		m_iClip ? DEAGLE_RELOAD : DEAGLE_RELOAD_EMPTY,
		m_iClip ? DEAGLE_RELOAD_TIME : DEAGLE_RELOAD_EMPTY_TIME,
		0.65f))
	{
		m_flAccuracy = 0.9f;
		return true;
	}

	// KF2 ???
	if (m_pPlayer->pev->weaponanim != DEAGLE_CHECK_MAGAZINE)
	{
		if (m_bInReload)
			SecondaryAttack();

		SendWeaponAnim(DEAGLE_CHECK_MAGAZINE);
		m_flTimeWeaponIdle = DEAGLE_CHECKMAG_TIME;
	}

	return false;
}

int CDEagle::CalcBodyParam(void)
{
	static BodyEnumInfo_t info[] =
	{
		{ 0, 1 },	// right hand	= 0;
		{ 0, 2 },	// left hand	= 1;
		{ 0, 1 },	// right sleeve	= 2;
		{ 0, 2 },	// left sleeve	= 3;

		{ 0, 2 },	// slide_1		= 4;
		{ 0, 2 },	// slide_2		= 5;
		{ 0, 4 },	// bullets		= 6;
		{ 0, 2 },	// laser		= 7;
		
	};

	// mag state control.
	switch (m_iClip)
	{
	case 0:	// empty mag. the follower is shown.
		info[6].body = 3;
		break;

	case 1:
		info[6].body = 2;
		break;

	case 2:
		info[6].body = 1;
		break;

	default:	// m_iClip >= 3
		info[6].body = 0;
		break;
	}

	// slide stop vfx.
	if (m_iClip <= 0 && (1 << m_pPlayer->pev->weaponanim) & BITS_SLIDE_STOP_ANIM)
	{
		info[4].body = 1;
		info[5].body = 1;
	}
	else
	{
		info[4].body = 0;
		info[5].body = 0;
	}

	switch (m_iVariation)
	{
	case Role_Sharpshooter:
		// the sharpshooter version of DEagle contains a laser.
		info[7].body = 1;
		break;

	default:
		info[7].body = 0;
		break;
	}

	// in current deagle model, there are two clips involved in normal reload anim. a full and an empty one.
	// in EMPTY reload, after we remove the empty mag, the new mag should be full of bullets.
	if (m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY)
	{
		if (m_pPlayer->m_flNextAttack < 1.257)	// in this anim, a new mag was taken out after around 0.657s. thus, 1.91f - 0.657f ~= 1.257f.
		{
			info[6].body = 0;	// full mag.
		}
	}

	return CalcBody(info, ARRAY_ELEM_COUNT(info));	// elements count of the info[].
}

DECLARE_STANDARD_RESET_MODEL_FUNC(DEagle)
