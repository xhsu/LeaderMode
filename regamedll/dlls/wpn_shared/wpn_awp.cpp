/*

Remastered Date: Mar 20 2020
Reflesh Date: Nov 02 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Miracle(Innocent Blue)
Sound - iDkGK

*/

#include "precompiled.h"

#ifdef CLIENT_DLL

static constexpr int MAGAZINE = 9;
static constexpr int EMPTY = 3;
static constexpr int FULL = 0;
static constexpr int TWO_LEFT = 1;
static constexpr int ONE_LEFT = 2;

static constexpr int MUZZLE = 7;
static constexpr int SILENCER = 2;
static constexpr int BREAKER = 1;

static constexpr int PIN = 8;
static constexpr int UP = 0;
static constexpr int DOWN = 1;

int CAWP::CalcBodyParam(void)
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

		{ 0, 3 },	// muzzle		= 7;
		{ 0, 2 },	// pin			= 8;
		{ 0, 4 },	// magazine		= 9;
	};

	// mag state control.
	switch (m_iClip)
	{
	case 0:	// empty mag. the follower is shown.
		info[MAGAZINE].body = EMPTY;
		break;

	case 1:
		info[MAGAZINE].body = ONE_LEFT;
		break;

	case 2:
		info[MAGAZINE].body = TWO_LEFT;
		break;

	default:	// m_iClip >= 3
		info[MAGAZINE].body = FULL;
		break;
	}

	// only consider variation when not morphing..
	switch (m_iVariation)
	{
	case Role_Sharpshooter:
		// muzzle breaker

		info[MUZZLE].body = BREAKER;
		break;

	case Role_Assassin:
		// silencer
		info[MUZZLE].body = SILENCER;
		break;

	default:
		info[MUZZLE].body = FALSE;
		break;
	}

	// pin downed vfx.
	if ((m_iClip <= 0 || !m_bChamberCleared) && (1 << m_pPlayer->pev->weaponanim) & BITS_PIN_UNINVOLVED_ANIM)
		info[PIN].body = DOWN;
	else
		info[PIN].body = UP;

	return CalcBody(info, _countof(info));
}

bool CAWP::StudioEvent(const mstudioevent_s* pEvent)
{
	switch (pEvent->event)
	{
	case 9527:	// Custom script.

		if (m_QCScript.empty())	// Initialize script.
		{
			std::vector<std::string> rgszTokens;
			UTIL_Split(pEvent->options, rgszTokens, ";");

			for (auto& token : rgszTokens)
			{
				std::vector<std::string> rgszTokens2;
				UTIL_Split(token, rgszTokens2, "=");

				if (rgszTokens2.size() != 2)
					continue;

				for (auto& token2 : rgszTokens2)
				{
					ltrim(token2);
					rtrim(token2);
				}

				if (rgszTokens2[0] == "m_bChamberCleared")
				{
					auto p = new QCScript::CValueAssignment(&m_bChamberCleared, rgszTokens2[1] == "true" ? true : false);
					m_QCScript.push_back(p);
				}
			}
		}
		else
			QCScript::Run(m_QCScript);

		return true;

	default:
		return false;
	}
}

#endif

void CAWP::PrimaryAttack()
{
	// no rechamber, not shoot.
	if (!m_bChamberCleared)
	{
		// unscope during this anim.
		if (m_pPlayer->pev->fov != DEFAULT_FOV)
			SecondaryAttack();

		SendWeaponAnim(RECHAMBER);
		m_pPlayer->m_flNextAttack = RECHAMBER_TIME;
		m_flTimeWeaponIdle = RECHAMBER_TIME;	// prevent anim instant break.

		return;
	}

	// PRE: use 1 to compare whether it is the last shot.
	BaseClass::DefaultShoot(GetSpread(), m_iClip == 1 ? SHOOT_LAST_TIME : FIRE_INTERVAL);

	// POST: unzoom. suggested by InnocentBlue.
	// don't do it unless bullets still left.
	if (m_pPlayer->pev->fov != DEFAULT_FOV)
		SecondaryAttack();

	// since m_flNextPrimaryAttack is involved in DefaultScopeSight(), we have to place a limit.
	// although m_flNextSecondaryAttack is set in DefaultShoot(), but actually after the SecondaryAttack() above, it's already invalid.
	m_flNextSecondaryAttack = m_iClip == 0 ? SHOOT_LAST_TIME : FIRE_INTERVAL;
}

float CAWP::GetSpread(void)
{
	float flSpread = DefaultSpread(SPREAD_BASELINE, 0.25f, 0.75f, 20, 50);

	if (m_pPlayer->pev->fov >= DEFAULT_FOV)
		flSpread *= 20;	// additional 2000% penalty for unscope shooting.

	return flSpread;
}

void CAWP::ApplyClientFPFiringVisual(const Vector2D& vSpread)
{
#ifdef CLIENT_DLL
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	EV_MuzzleFlash();
	EV_PlayShootAnim();

	EV_HLDM_CreateSmoke(3, 0.5, Color(20, 20, 20), EV_PISTOL_SMOKE, false, 35);
	EV_HLDM_CreateSmoke(40, 0.5, Color(15, 15, 15), EV_WALL_PUFF, false, 35);
	EV_HLDM_CreateSmoke(80, 0.5, Color(10, 10, 10), EV_WALL_PUFF, false, 35);

	auto vecSrc = EV_GetGunPosition(
		gEngfuncs.GetLocalPlayer()->index,
		gEngfuncs.pEventAPI->EV_LocalPlayerDucking(),
		m_pPlayer->pev->origin
	);

	::EV_PlayGunFire2(vecSrc + gpGlobals->v_forward * 10.0f, FIRE_SFX, m_iVariation == Role_Assassin ? NORMAL_GUN_VOLUME : GUN_VOLUME);
#endif // CLIENT_DLL
}

void CAWP::ApplyRecoil(void)
{
	m_pPlayer->m_vecVAngleShift.x -= 12.0f;
	m_pPlayer->m_vecVAngleShift.y -= UTIL_SharedRandomFloat(m_pPlayer->random_seed + PENETRATION, -2.0f, 2.0f);
}

void CAWP::ApplyClientTPFiringVisual(event_args_s* args)
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

	Vector vecSrc = EV_GetGunPosition(args->entindex, args->ducking, args->origin);

	// original goldsrc api: VOL = 1.0, ATTN = 0.28
	::EV_PlayGunFire2(vecSrc + forward * 10.0f, FIRE_SFX, iVariation == Role_Assassin ? NORMAL_GUN_VOLUME : GUN_VOLUME);

	::EV_HLDM_FireBullets(idx,
		forward, right, up,
		1, args->origin, forward,
		vSpread, EFFECTIVE_RANGE, g_rgWpnInfo[WEAPON_AWP].m_iAmmoType,
		PENETRATION,
		iSeed
	);
#endif // CLIENT_DLL

}
