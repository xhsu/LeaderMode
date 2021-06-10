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

static constexpr int BARREL1 = 12;
static constexpr int BARREL2 = 13;
static constexpr int SIGHT = 17;
static constexpr int LASER = 18;
static constexpr int MUZZLE = 19;
static constexpr int STEELSIGHT = 20;

static constexpr int OPTIC = 0;
static constexpr int FLIPPED_UP = 0;
static constexpr int CARBINE = 0;
static constexpr int SHARPSHOOTER = 1;
static constexpr int NONE = 1;
static constexpr int SILENCER = 1;
static constexpr int HOLOGRAPHIC = 2;

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

		{ 0, 3 },	// sight		= 17;
		{ 0, 2 },	// laser		= 18;
		{ 0, 2 },	// muzzle		= 19;
		{ 0, 2 },	// steel sight	= 20;
		{ 0, 2 },	// selector		= 21;
	};

	// this model requires all parts set to 0 in order to play changing anim.
	if (m_bitsFlags & WPNSTATE_XM8_CHANGING)
	{
		info[BARREL1].body = CARBINE;
		info[BARREL2].body = CARBINE;
		info[SIGHT].body = OPTIC;
		info[STEELSIGHT].body = FLIPPED_UP;
	}
	else
	{
		// only consider variation when not morphing..
		switch (m_iVariation)
		{
		case Role_SWAT:
		case Role_Medic:
		case Role_MadScientist:
			// electronic sight
			info[BARREL1].body = CARBINE;
			info[BARREL2].body = CARBINE;
			info[SIGHT].body = HOLOGRAPHIC;
			info[LASER].body = FALSE;
			info[MUZZLE].body = FALSE;
			info[STEELSIGHT].body = NONE;
			break;

		case Role_Assassin:
			// silencer
			// electronic sight
			info[BARREL1].body = CARBINE;
			info[BARREL2].body = CARBINE;
			info[SIGHT].body = HOLOGRAPHIC;
			info[LASER].body = FALSE;
			info[MUZZLE].body = SILENCER;
			info[STEELSIGHT].body = NONE;
			break;

		case Role_Sharpshooter:
			// sharpshooter barrel.
			// optic
			// laser

			info[BARREL1].body = SHARPSHOOTER;
			info[BARREL2].body = SHARPSHOOTER;
			info[SIGHT].body = OPTIC;
			info[LASER].body = TRUE;
			info[MUZZLE].body = FALSE;
			info[STEELSIGHT].body = NONE;
			break;

		default:
			// by default, this weapon has no optical sight.
			// filpped up steel sight.

			info[BARREL1].body = CARBINE;
			info[BARREL2].body = CARBINE;
			info[SIGHT].body = NONE;
			info[LASER].body = FALSE;
			info[MUZZLE].body = FALSE;
			info[STEELSIGHT].body = FLIPPED_UP;
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
	m_flAccuracy = 0.25f;
	m_iShotsFired = 0;

	return DefaultDeploy(VIEW_MODEL, WORLD_MODEL, (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? DRAW_FIRST : DRAW, "carbine", (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? DRAW_FIRST_TIME : DRAW_TIME);
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
	case Role_SWAT:
	case Role_Medic:
	case Role_MadScientist:
	case Role_Assassin:
		// ELECTRONIC.
		DefaultSteelSight(Vector(-6.12f, -4, 2.05f), 80, 8.0f);
		break;

	case Role_Sharpshooter:
		// OPTICAL.
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
	float flInterval = 60.0f / RPM;

	// slower fire interval for sharpshooter mode.
	if (m_iVariation == Role_Sharpshooter)
		flInterval = 0.2f;

	XM8Fire(GetSpread(), flInterval);
}

float CXM8::GetSpread(void)
{
	m_flAccuracy = (float(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 215.0f) + 0.25f;

	if (m_flAccuracy > 1.0f)
		m_flAccuracy = 1.0f;

	return DefaultSpread(SPREAD_BASELINE * m_flAccuracy, 0.1f, 0.75f, 2.0f, 5.0f);
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

	m_pPlayer->m_iWeaponVolume = GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, EFFECTIVE_RANGE, PENETRATION, m_iPrimaryAmmoType, DAMAGE, RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	int iAnim = FIRE;
	
	if (m_bInZoom)
	{
		iAnim = FIRE_AIM;

		if (m_iClip <= 0)
			iAnim = FIRE_AIM_LAST;
	}
	else if (m_iClip <= 0)
		iAnim = FIRE_LAST;

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
		m_iClip ? RELOAD : RELOAD_EMPTY,
		m_iClip ? RELOAD_TIME : RELOAD_EMPTY_TIME,
		m_iClip ? 0.7f : 0.7f))
	{
		m_flAccuracy = 0.25f;
		return true;
	}

	// KF2 ???
	if (m_pPlayer->pev->weaponanim != CHECK_MAGAZINE)
	{
		if (m_bInZoom)
			SecondaryAttack();

		SendWeaponAnim(CHECK_MAGAZINE);
		m_flTimeWeaponIdle = CHECKMAG_TIME;
	}

	return false;
}

bool CXM8::AlterAct(void)
{
	if (m_pPlayer->m_iRoleType != Role_Sharpshooter)
		return false;

	if (m_bitsFlags & WPNSTATE_BUSY)
		return false;

	SendWeaponAnim(m_iVariation == Role_Sharpshooter ? SWITCH_TO_CARBINE : SWITCH_TO_SHARPSHOOTER);
	m_pPlayer->m_flNextAttack = m_iVariation == Role_Sharpshooter ? TO_CARBIN_TIME : TO_SHARPSHOOTER_TIME;
	m_flTimeWeaponIdle = m_iVariation == Role_Sharpshooter ? TO_CARBIN_TIME : TO_SHARPSHOOTER_TIME;
	m_bitsFlags |= WPNSTATE_XM8_CHANGING;

	return true;
}
