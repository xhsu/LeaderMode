/*

Created Date: Oct 8 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CXM8::m_usEvent = 0;
int CXM8::m_iShell = 0;

void CXM8::Precache()
{
	PRECACHE_NECESSARY_FILES(XM8);

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/xm8.sc");
}

#else

int CXM8::CalcBodyParam(void)
{
	static BodyEnumInfo_t info[] =
	{
		{ 0, 1 },	// right hand	= 0;
		{ 0, 2 },	// left hand	= 1;
		{ 0, 1 },	// right sleeve	= 2;
		{ 0, 2 },	// left sleeve	= 3;

		{ 0, 1 },	// weapon		= 4;
		{ 0, 1 },
		{ 0, 1 },
		{ 0, 1 },

		{ 0, 2 },	// bullets		= 8;

		{ 0, 1 },
		{ 0, 1 },
		{ 0, 1 },

		{ 0, 2 },	// barrel_1		= 12;
		{ 0, 2 },	// barrel_2		= 13;

		{ 0, 1 },
		{ 0, 1 },
		{ 0, 1 },

		{ 0, 2 },	// optical sight= 17;
		{ 0, 2 },	// steel sight	= 18;
		{ 0, 2 },	// selector		= 19;
	};

	// this model requires all parts set to 0 in order to play changing anim.
	if (m_bitsFlags & WPNSTATE_XM8_CHANGING)
	{
		info[12].body = 0;
		info[13].body = 0;
		info[17].body = 0;
		info[18].body = 0;
	}
	else
	{
		// only consider variation when not morphing..
		switch (m_iVariation)
		{
		case Role_Sharpshooter:
			// the sharpshooter's version contains a XM8 sharpshooter optical sight.
			// filpped down steel sight.

			info[12].body = 1;
			info[13].body = 1;
			info[17].body = 0;
			info[18].body = 1;
			break;

		default:
			// by default, this weapon has no optical sight.
			// filpped up steel sight.

			info[12].body = 0;
			info[13].body = 0;
			info[17].body = 1;
			info[18].body = 0;
			break;
		}
	}

	if (!m_iClip)
		info[8].body = 1;	// empty mag.

	// in EMPTY reload, after we remove the empty mag, the new mag should be full of bullets.
	if (m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY)
	{
		if (m_pPlayer->m_flNextAttack < 2.03F)	// in this anim, a new mag was taken out after around 1.0s. thus, 3.03f - 1.0f ~= 2.03f.
		{
			info[8].body = 0;	// full mag.
		}
	}

	return CalcBody(info, _countof(info));
}

void CXM8::Think(void)
{
	CBaseWeapon::Think();

	// this model requires all parts set to 0 in order to play changing anim.
	if ((m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY) || (m_bitsFlags & WPNSTATE_XM8_CHANGING))
	{
		g_pViewEnt->curstate.body = CalcBodyParam();
	}
}

#endif

bool CXM8::Deploy()
{
	m_flAccuracy = 0.2f;
	m_iShotsFired = 0;

	return DefaultDeploy(XM8_VIEW_MODEL, XM8_WORLD_MODEL, (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? XM8_DRAW_FIRST : XM8_DRAW, "carbine", (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? XM8_DRAW_FIRST_TIME : XM8_DRAW_TIME);
}

void CXM8::PostFrame(void)
{
	if (m_bitsFlags & WPNSTATE_XM8_CHANGING)
	{
		if (m_iVariation == Role_Sharpshooter)
			SetVariation(Role_UNASSIGNED);
		else
			SetVariation(Role_Sharpshooter);

		m_bitsFlags &= ~WPNSTATE_XM8_CHANGING;
	}

	// semi-auto for sharpshooter mode.
	// release IN_ATTACK1 would clear fire flag instantly.
	if (m_iVariation == Role_Sharpshooter && !(m_pPlayer->pev->button & IN_ATTACK))
	{
		m_iShotsFired = 0;
		m_bDelayRecovery = false;
	}

	return CBaseWeapon::PostFrame();
}

void CXM8::SecondaryAttack()
{
	switch (m_iVariation)
	{
	case Role_Sharpshooter:
		// OPTICAL.
		//DefaultSteelSight(Vector(-3.06f, -2, 1.142f), 50, 8.0f);
		DefaultScopeSight(Vector(-6.12f, -4, 2.284f), 40);
		break;

	default:
		// STEEL.
		DefaultSteelSight(Vector(-6.12f, -4, 2.70f), 85, 8.0f);
		break;
	}
}

void CXM8::PrimaryAttack()
{
	float flInterval = 60.0f / XM8_RPM;

	// slower fire interval for sharpshooter mode.
	if (m_iVariation == Role_Sharpshooter)
		flInterval = 0.2f;

	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		XM8Fire(0.035f + (0.4f * m_flAccuracy), flInterval);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
	{
		XM8Fire(0.035f + (0.07f * m_flAccuracy), flInterval);
	}
	else if (m_bInZoom)	// decrease spread while scoping.
	{
		XM8Fire(0.01f * m_flAccuracy, flInterval);
	}
	else
	{
		XM8Fire(0.02f * m_flAccuracy, flInterval);
	}
}

void CXM8::XM8Fire(float flSpread, float flCycleTime)
{
	m_iShotsFired++;
	m_bDelayRecovery = true;

	// semi-auto for sharpshooter mode.
	if (m_iVariation == Role_Sharpshooter && m_iShotsFired > 1)
	{
		return;
	}

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

	m_pPlayer->m_iWeaponVolume = XM8_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, XM8_EFFECTIVE_RANGE, XM8_PENETRATION, m_iPrimaryAmmoType, XM8_DAMAGE, XM8_RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	int iAnim = XM8_FIRE;
	
	if (m_bInZoom)
	{
		iAnim = XM8_FIRE_AIM;

		if (m_iClip <= 0)
			iAnim = XM8_FIRE_AIM_LAST;
	}
	else if (m_iClip <= 0)
		iAnim = XM8_FIRE_LAST;

	SendWeaponAnim(iAnim);
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), m_iClip <= 0, m_bInZoom);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = !!(m_iClip <= 0);	// empty clip?
	args.bparam2 = m_bInZoom;			// in zoom?
	args.ducking = gEngfuncs.pEventAPI->EV_LocalPlayerDucking();
	args.entindex = gEngfuncs.GetLocalPlayer()->index;
	args.flags = FEV_NOTHOST | FEV_RELIABLE | FEV_CLIENT | FEV_GLOBAL;
	args.fparam1 = vecDir.x;
	args.fparam2 = vecDir.y;
	args.iparam1 = int(m_pPlayer->pev->punchangle.x * 100.0f);
	args.iparam2 = int(m_pPlayer->pev->punchangle.y * 100.0f);
	args.origin = m_pPlayer->pev->origin;
	args.velocity = m_pPlayer->pev->velocity;

	EV_FireXM8(&args);
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

bool CXM8::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip,
		m_iClip ? XM8_RELOAD : XM8_RELOAD_EMPTY,
		m_iClip ? XM8_RELOAD_TIME : XM8_RELOAD_EMPTY_TIME,
		m_iClip ? 0.7f : 0.7f))
	{
		m_flAccuracy = 0.2f;
		return true;
	}

	// KF2 ???
	if (m_pPlayer->pev->weaponanim != XM8_CHECK_MAGAZINE)
	{
		if (m_bInReload)
			SecondaryAttack();

		SendWeaponAnim(XM8_CHECK_MAGAZINE);
		m_flTimeWeaponIdle = XM8_CHECKMAG_TIME;
	}

	return false;
}

bool CXM8::AlterAct(void)
{
	if (m_pPlayer->m_iRoleType != Role_Sharpshooter)
		return false;

	if (m_bitsFlags & WPNSTATE_BUSY)
		return false;

	SendWeaponAnim(m_iVariation == Role_Sharpshooter ? XM8_SWITCH_TO_CARBINE : XM8_SWITCH_TO_SHARPSHOOTER);
	m_pPlayer->m_flNextAttack = m_iVariation == Role_Sharpshooter ? XM8_TO_CARBIN_TIME : XM8_TO_SHARPSHOOTER_TIME;
	m_flTimeWeaponIdle = m_iVariation == Role_Sharpshooter ? XM8_TO_CARBIN_TIME : XM8_TO_SHARPSHOOTER_TIME;
	m_bitsFlags |= WPNSTATE_XM8_CHANGING;

	return true;
}

DECLARE_STANDARD_RESET_MODEL_FUNC(XM8)
