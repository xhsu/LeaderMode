/*

Remastered Date: Mar 25 2020
Reflesh Date: Oct 23 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)
Sound - iDkGK

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CFN57::m_usEvent = 0;
int CFN57::m_iShell = 0;

void CFN57::Precache()
{
	BaseClass::Precache();
	PRECACHE_SOUND(FIRE_SFX_SIL);

	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/fiveseven.sc");
}

#else

static constexpr int SLIDE = 5;
static constexpr int LASER = 8;
static constexpr int SILENCER = 9;
static constexpr int SIGHT = 10;

int CFN57::CalcBodyParam(void)
{
	static BodyEnumInfo_t info[] =
	{
		{ 0, 1 },	// right hand	= 0;
		{ 0, 2 },	// left hand	= 1;
		{ 0, 1 },	// right sleeve	= 2;
		{ 0, 2 },	// left sleeve	= 3;

		{ 0, 1 },	// pistol		= 4;

		{ 0, 2 },	// slide		= 5;
		{ 0, 4 },	// bullets		= 6;

		{ 0, 1 },	// pistol		= 7;

		{ 0, 2 },	// laser		= 8;
		{ 0, 2 },	// silencer		= 9;
		{ 0, 3 },	// sight		= 10;

		{ 0, 1 },	// reflect		= 11;
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

	// variation
	switch (m_iVariation)
	{
	case Role_SWAT:
	case Role_Medic:
	case Role_MadScientist:
		// sight
		info[LASER].body = FALSE;
		info[SILENCER].body = FALSE;
		info[SIGHT].body = TRUE;
		break;

	case Role_Sharpshooter:
		// laser
		// sight
		info[LASER].body = TRUE;
		info[SILENCER].body = FALSE;
		info[SIGHT].body = TRUE;
		break;

	case Role_Assassin:
		// silencer
		// sight
		info[LASER].body = FALSE;
		info[SILENCER].body = TRUE;
		info[SIGHT].body = TRUE;
		break;

	default:
		// nothing
		info[LASER].body = FALSE;
		info[SILENCER].body = FALSE;
		info[SIGHT].body = FALSE;
		break;
	}

	// slide stop vfx.
	if (m_iClip <= 0 && (1 << m_pPlayer->pev->weaponanim) & BITS_SLIDE_STOP_ANIM)
	{
		info[SLIDE].body = 1;

		// also move the sight, if it exists.
		if (info[SIGHT].body == TRUE)
			info[SIGHT].body = 2;
	}
	else
		info[SLIDE].body = 0;

	return CalcBody(info, _countof(info));	// elements count of the info[].
}

#endif

bool CFN57::Deploy()
{
	// this anim is slide-moving.
	if (m_iClip <= 0)
		m_bitsFlags &= ~WPNSTATE_DRAW_FIRST;

	return BaseClass::Deploy();
}

void CFN57::SecondaryAttack()
{
	switch (m_iVariation)
	{
	case Role_SWAT:
	case Role_Sharpshooter:
	case Role_Medic:
	case Role_MadScientist:
	case Role_Assassin:
		// Electronic Sight
		DefaultSteelSight(Vector(-2.98f, -2, 1.88f), 75, 8.0f);
		break;

	default:
		// Steel Sight
		DefaultSteelSight(Vector(-2.98f, 0, 2.2f), 85, 8.0f);
		break;
	}
}

float CFN57::GetSpread(void)
{
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

	return DefaultSpread(SPREAD_BASELINE * (1.0f - m_flAccuracy), 0.1f, 0.75f, 2.0f, 5.0f);
}

void CFN57::FiveSevenFire(float flSpread, float flCycleTime)
{
	if (++m_iShotsFired > 1)
	{
		return;
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

	m_pPlayer->m_iWeaponVolume = m_iVariation == Role_Assassin ? QUIET_GUN_VOLUME : GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;

	Vector2D vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, EFFECTIVE_RANGE, PENETRATION, m_iPrimaryAmmoType, DAMAGE, RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	int iAnim = 0;
	if (m_iClip > 0)
	{
		if (m_bInZoom)
			iAnim = AIM_SHOOT;
		else
			iAnim = SHOOT;
	}
	else
	{
		if (m_bInZoom)
			iAnim = AIM_SHOOT_LAST;
		else
			iAnim = SHOOT_LAST;
	}

	SendWeaponAnim(iAnim);	// LUNA: I don't know why, but this has to be done on SV side, or client fire anim would be override.
	PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE | FEV_SERVER | FEV_GLOBAL, m_pPlayer->edict(), m_usEvent, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), m_iVariation == Role_Assassin, false);	//// on non-host sending, the bparam1 is reserved for silencer status.

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}
#else
	static event_args_t args;
	Q_memset(&args, NULL, sizeof(args));

	args.angles = m_pPlayer->pev->v_angle;
	args.bparam1 = m_iClip > 0;	// on host ev sending, the bparams are used for first-personal shooting anim.
	args.bparam2 = m_bInZoom;
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
	if (DefaultReload(m_pItemInfo->m_iMaxClip,
		m_iClip ? RELOAD : RELOAD_EMPTY,
		m_iClip ? RELOAD_TIME : RELOAD_EMPTY_TIME,
		m_iClip ? 0.48f : 0.47f))
	{
		m_flAccuracy = ACCURACY_BASELINE;
		return true;
	}

	// KF2 ???
	if (m_pPlayer->pev->weaponanim != CHECK_MAGAZINE)
	{
		if (m_bInZoom)
			SecondaryAttack();

		SendWeaponAnim(CHECK_MAGAZINE);
		m_flTimeWeaponIdle = CHECK_MAGAZINE_TIME;
	}

	return false;
}
