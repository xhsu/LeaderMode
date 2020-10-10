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

void CDEagle::SecondaryAttack()
{
	m_bInZoom = !m_bInZoom;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

	if (!g_vecGunOfsGoal.LengthSquared())
	{
		g_vecGunOfsGoal = Vector(-1.93f, -0.5f, 0.15f);
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
		m_iClip ? DEAGLE_RELOAD_TIME : DEAGLE_RELOAD_EMPTY_TIME))
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

void CDEagle::WeaponIdle()
{
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
	SendWeaponAnim((m_bitsFlags & WPNSTATE_DASHING) ? DEAGLE_DASHING : DEAGLE_IDLE);
}

bool CDEagle::HolsterStart(void)
{
	SendWeaponAnim(DEAGLE_HOLSTER);
	m_pPlayer->m_flNextAttack = DEAGLE_HOLSTER_TIME;
	m_bitsFlags |= WPNSTATE_HOLSTERING;

	return true;
}

void CDEagle::DashStart(void)
{
	if (m_bInReload)
		m_bInReload = false;

	if (m_bInZoom || m_pPlayer->pev->fov < 90)
	{
#ifndef CLIENT_DLL
		SecondaryAttack();
#else
		g_vecGunOfsGoal = g_vecZero;
		g_flGunOfsMovingSpeed = 10.0f;
		gHUD::m_iFOV = 90;
#endif
	}

	SendWeaponAnim(DEAGLE_DASH_ENTER);
	m_pPlayer->m_flNextAttack = DEAGLE_DASH_ENTER_TIME;
	m_flTimeWeaponIdle = DEAGLE_DASH_ENTER_TIME;
	m_bitsFlags |= WPNSTATE_DASHING;
}

void CDEagle::DashEnd(void)
{
	if (m_pPlayer->m_flNextAttack > 0.0f && m_pPlayer->pev->weaponanim == DEAGLE_DASH_ENTER)
	{
		// this is how much you procees to the dashing phase.
		// for example, assuming the whole length is 1.0s, you start 0.7s and decide to cancel.
		// although there's only 0.3s to the dashing phase, but turning back still requires another equally 0.7s.
		// "m_pPlayer->m_flNextAttack" is the 0.3s of full length. you need to get the rest part, i.e. the 70%.
		float flRunStartUnplayedRatio = 1.0f - m_pPlayer->m_flNextAttack / DEAGLE_DASH_ENTER_TIME;

		// stick on the last instance in the comment: 70% * 1.0s(full length) = 0.7s, this is the time we need to turning back.
		float flRunStopTimeLeft = DEAGLE_DASH_EXIT_TIME * flRunStartUnplayedRatio;

		// play the anim.
		SendWeaponAnim(DEAGLE_DASH_EXIT);

#ifdef CLIENT_DLL
		// why we are using the "0.3s" here?
		// this is because the g_flTimeViewModelAnimStart actually means how much time had passed since the anim was ordered to play.
		// if we need to play 0.7s, we have to told system we only played it for 0.3s. right?
		g_flTimeViewModelAnimStart = gEngfuncs.GetClientTime() - (DEAGLE_DASH_EXIT_TIME - flRunStopTimeLeft);
#endif

		// force everything else to wait.
		m_pPlayer->m_flNextAttack = m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle = flRunStopTimeLeft;
	}

	// if RUN_START is normally played and finished, go normal.
	else
	{
		SendWeaponAnim(DEAGLE_DASH_EXIT);
		m_pPlayer->m_flNextAttack = DEAGLE_DASH_EXIT_TIME;
		m_flTimeWeaponIdle = DEAGLE_DASH_EXIT_TIME;
	}

	// either way, we have to remove this flag.
	m_bitsFlags &= ~WPNSTATE_DASHING;
}

int CDEagle::CalcBodyParam(void)
{
	static BodyEnumInfo_t info[] =
	{
		{ 0, 1 },	// right hand	= 0;
		{ 0, 2 },	// left hand	= 1;
		{ 0, 1 },	// right sleeve	= 2;
		{ 0, 2 },	// left sleeve	= 3;

		{ 0, 1 },	// pistol		= 4;
		{ 0, 1 },
		{ 0, 1 },
		{ 0, 1 },

		{ 0, 2 },	// follower		= 8;
		{ 0, 4 },	// bullets		= 9;
		{ 0, 1 },
		{ 0, 2 },	// slide_1		= 11;
		{ 0, 2 },	// slide_2		= 12;
	};

	// mag state control.
	switch (m_iClip)
	{
	case 0:	// empty mag. the follower is shown.
		info[8].body = 1;
		info[9].body = 3;
		break;

	case 1:
		info[8].body = 0;
		info[9].body = 2;
		break;

	case 2:
		info[8].body = 0;
		info[9].body = 1;
		break;

	default:	// m_iClip >= 3
		info[8].body = 0;
		info[9].body = 0;
		break;
	}

	// slide stop vfx.
	if (m_iClip <= 0 && (1 << m_pPlayer->pev->weaponanim) & BITS_SLIDE_STOP_ANIM)
	{
		info[11].body = 1;
		info[12].body = 1;
	}
	else
	{
		info[11].body = 0;
		info[12].body = 0;
	}

	// in current deagle model, there are two clips involved in normal reload anim. a full and an empty one.
	// in EMPTY reload, after we remove the empty mag, the new mag should be full of bullets.
	if (m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY)
	{
		if (m_pPlayer->m_flNextAttack < 1.257)	// in this anim, a new mag was taken out after around 0.657s. thus, 1.91f - 0.657f ~= 1.257f.
		{
			info[8].body = 0;	// full mag.
			info[9].body = 0;
		}
	}

	return CalcBody(info, 13);	// elements count of the info[].
}

DECLARE_STANDARD_RESET_MODEL_FUNC(DEagle)
