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

#ifdef CLIENT_DLL

SCE_FIELD LHAND		= 1;
SCE_FIELD LSLEEVE	= 3;
SCE_FIELD SLIDE		= 5;
SCE_FIELD MAGAZINE	= 6;
SCE_FIELD BARREL	= 7;
SCE_FIELD LASER		= 10;
SCE_FIELD SIGHT		= 11;

SCE_FIELD VISIBLE = 0;
SCE_FIELD INVISIBLE	= 1;
SCE_FIELD NORMAL = 0;
SCE_FIELD SLIDE_STOP = 1;
SCE_FIELD NO_BULLET = 1;
SCE_FIELD BREECHBLOCK = 1;
SCE_FIELD POS_AT_SLIDE_STOP = 2;

int CUSP::CalcBodyParam(void)
{
	static BodyEnumInfo_t info[] =
	{
		{ 0, 1 },	// right hand	= 0;
		{ 0, 2 },	// left hand	= 1;
		{ 0, 1 },	// right sleeve	= 2;
		{ 0, 2 },	// left sleeve	= 3;

		{ 0, 1 },	// weapon_1		= 4;

		{ 0, 2 },	// slide		= 5;
		{ 0, 2 },	// bullets		= 6;
		{ 0, 2 },	// barrel		= 7;
		{ 0, 1 },	// hammer		= 8;
		{ 0, 1 },	// weapon_2		= 9;
		{ 0, 2 },	// laser		= 10;
		{ 0, 3 },	// sight		= 11;
	};

	// mag state control.
	if (m_iClip > 0)
		info[MAGAZINE].body = NORMAL;
	else
		info[MAGAZINE].body = NO_BULLET;

	// variation
	switch (m_iVariation)
	{
	case Role_SWAT:
	case Role_Medic:
	case Role_MadScientist:
		// sight
		info[LASER].body = FALSE;
		info[SIGHT].body = TRUE;
		break;

	case Role_Sharpshooter:
		// laser
		// sight
		info[LASER].body = TRUE;
		info[SIGHT].body = TRUE;
		break;

	case Role_Assassin:
		// silencer
		// sight
		info[LASER].body = FALSE;
		info[SIGHT].body = TRUE;
		break;

	default:
		// nothing
		info[LASER].body = FALSE;
		info[SIGHT].body = FALSE;
		break;
	}

	// slide stop vfx.
	if (m_iClip <= 0 && (1 << m_pPlayer->pev->weaponanim) & BITS_SLIDE_STOP_ANIM)
	{
		info[SLIDE].body = SLIDE_STOP;
		info[BARREL].body = BREECHBLOCK;

		// also move the sight, if it exists.
		if (info[SIGHT].body == TRUE)
			info[SIGHT].body = POS_AT_SLIDE_STOP;
	}
	else
	{
		info[SLIDE].body = NORMAL;
		info[BARREL].body = NORMAL;
	}

	return CalcBody(info, _countof(info));	// elements count of the info[].
}

#endif

void CUSP::SecondaryAttack(void)
{
	switch (m_iVariation)
	{
	case Role_SWAT:
	case Role_Sharpshooter:
	case Role_Medic:
	case Role_MadScientist:
	case Role_Assassin:
		// Electronic Sight
		DefaultSteelSight(Vector(-2.69f, 5, 0.88), 75);
		break;

	default:
		// Steel Sight
		DefaultSteelSight(Vector(-2.7, 5, 1.4), 85);
		break;
	}
}

float CUSP::GetSpread(void)
{
	if (m_flLastFire != 0.0f)
	{
		m_flAccuracy -= (0.3f - (gpGlobals->time - m_flLastFire)) * 0.275f;

		if (m_flAccuracy > ACCURACY_BASELINE)
		{
			m_flAccuracy = ACCURACY_BASELINE;
		}
		else if (m_flAccuracy < 0.6f)
		{
			m_flAccuracy = 0.6f;
		}
	}

	return DefaultSpread(SPREAD_BASELINE * (1.0f - m_flAccuracy), 0.1f, 0.75f, 2.0f, 5.0f);
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

	::EV_HLDM_CreateSmoke(smoke_origin, gpGlobals->v_forward, 0, base_scale, 7, 7, 7, EV_PISTOL_SMOKE, m_pPlayer->pev->velocity, false, 35);
	EV_HLDM_CreateSmoke(20, base_scale + 0.1, Color(10, 10, 10), EV_WALL_PUFF, false, 35);
	EV_HLDM_CreateSmoke(40, base_scale, Color(13, 13, 13), EV_WALL_PUFF, false, 35);

	// shoot anim.
	EV_PlayShootAnim();

	Vector ShellVelocity, ShellOrigin;
	if (!cl_righthand->value)
		EV_GetDefaultShellInfo(ShellVelocity, ShellOrigin, 36.0, -14.0, -14.0);
	else
		EV_GetDefaultShellInfo(ShellVelocity, ShellOrigin, 36.0, -14.0, 14.0);

	ShellVelocity *= 0.5;
	ShellVelocity.z += 45.0;

	EV_EjectBrass(ShellOrigin, ShellVelocity, vecAngles.yaw, m_iShell<CUSP>, TE_BOUNCE_SHELL, 5);

	Vector vecSrc = EV_GetGunPosition(idx, bDucking, m_pPlayer->pev->origin);

	// original API: vol = 1.0, attn = 0.8, pitch = 87~105
	EV_PlayGunFire(vecSrc + gpGlobals->v_forward * 10.0f, FIRE_SFX, QUIET_GUN_VOLUME, 1.0, RANDOM_LONG(87, 105));

	EV_HLDM_FireBullets(vSpread);
#endif
}

void CUSP::ApplyRecoil(void)
{
	m_pPlayer->m_vecVAngleShift.x -= 2.0f;
}

void CUSP::ApplyClientTPFiringVisual(struct event_args_s* args)
{
#ifdef CLIENT_DLL
	bool bClipGreaterThanNaught = args->bparam1;
	bool bInZoom = args->bparam2;
	int idx = args->entindex;
	Vector2D vSpread(args->fparam1, args->fparam2);
	int iSeed = args->iparam1;
	RoleTypes iVariation = (RoleTypes)args->iparam2;

	Vector forward, right, up;
	AngleVectors(args->angles, forward, right, up);

	Vector ShellVelocity, ShellOrigin;
	::EV_GetDefaultShellInfo(args->entindex, args->ducking, args->origin, args->velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0);
	EV_EjectBrass(ShellOrigin, ShellVelocity, args->angles.yaw, m_iShell<CUSP>, TE_BOUNCE_SHELL, 5);

	// original API: vol = 1.0, attn = 0.8, pitch = 87~105
	EV_PlayGunFire(args->origin + forward * 10.0f, FIRE_SFX, QUIET_GUN_VOLUME, 1.0, RANDOM_LONG(87, 105));

	::EV_HLDM_FireBullets(idx,
		forward, right, up,
		1, args->origin, forward,
		vSpread, EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_USP].m_iAmmoType,
		PENETRATION,
		iSeed);
#endif
}
