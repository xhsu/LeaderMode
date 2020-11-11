/*

Remastered Date: Mar 29 2020

Modern Warfare Dev Team
Code - Luna the Reborn
Model - Matoilet

*/

#include "precompiled.h"

#ifndef CLIENT_DLL

unsigned short CSCARH::m_usEvent = 0;
int CSCARH::m_iShell = 0;

void CSCARH::Precache()
{
	PRECACHE_NECESSARY_FILES(SCARH);

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usEvent = PRECACHE_EVENT(1, "events/scarh.sc");
}

#else

int CSCARH::CalcBodyParam(void)
{
	BodyEnumInfo_t info[] =
	{
		{ 0, 2 },	// hands		= 0;
		{ 0, 1 },
		{ 0, 1 },

		{ 0, 1 },	// rifle		= 3;
		{ 0, 1 },
		{ 0, 1 },

		{ 0, 2 },	// magazine		= 6;
		{ 0, 3 },	// steel sight	= 7;
		{ 0, 6 },	// scopes		= 8;
		{ 0, 4 },	// attachments	= 9;
		{ 0, 2 },	// nail/shell	= 10;
		{ 0, 4 },	// muzzle		= 11;
		{ 0, 2 },	// laser		= 12;
	};

	switch (m_iVariation)
	{
	case Role_Sharpshooter:
		// the sharpshooter's version contains a ACOG and a laser.
		// removed steel sight.
		// ACOG.
		// Recoil compensator.
		// laser.

		info[7].body = 2;
		info[8].body = 4;
		info[11].body = 2;
		info[12].body = 1;
		break;

	default:
		// by default, this weapon has:
		// filpped down steel sight.
		// holographic sight.

		info[7].body = 1;
		info[8].body = 2;
		break;
	}

	if (!m_iClip)
		info[6].body = 1;	// empty mag.

	// in EMPTY reload, after we remove the empty mag, the new mag should be full of bullets.
	if (m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY)
	{
		if (m_pPlayer->m_flNextAttack < 2.18F)	// in this anim, a new mag was taken out after around 0.3s. thus, 2.9F - 0.72f ~= 2.18f.
		{
			info[6].body = 0;	// full mag.
		}
	}

	return CalcBody(info, _countof(info));	// elements count of the info[].
}

void CSCARH::Think(void)
{
	CBaseWeapon::Think();

	// just keep updating model during empty-reload.
	if (m_bInReload && m_bitsFlags & WPNSTATE_RELOAD_EMPTY)
	{
		g_pViewEnt->curstate.body = CalcBodyParam();
	}
}

#endif

bool CSCARH::Deploy()
{
	m_flAccuracy = 0.25f;
	m_iShotsFired = 0;
	return DefaultDeploy(SCARH_VIEW_MODEL, SCARH_WORLD_MODEL, (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? SCARH_DRAW_FIRST : SCARH_DEPLOY, "mp5", (m_bitsFlags & WPNSTATE_DRAW_FIRST) ? SCARH_DRAW_FIRST_TIME : SCARH_DEPLOY_TIME);
}

void CSCARH::SecondaryAttack()
{
	switch (m_iVariation)
	{
	case Role_Sharpshooter:
		// ACOG
		DefaultSteelSight(Vector(-3.62f, -5, 0.5f), 55, 8.0f);
		break;

	default:
		// HOLO
		DefaultSteelSight(Vector(-3.725F, -2, 0.615F), 85, 8.0f);
		break;
	}
}

float CSCARH::GetSpread(void)
{
	m_flAccuracy = (float(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 220.0f) + 0.25f;

	if (m_flAccuracy > 1.0f)
		m_flAccuracy = 1.0f;

	return DefaultSpread(SCARH_SPREAD_BASELINE * m_flAccuracy, 0.1f, 0.75f, 2.0f, 5.0f);
}

void CSCARH::SCARHFire(float flSpread, float flCycleTime)
{
	m_iShotsFired++;
	m_bDelayRecovery = true;

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

	m_pPlayer->m_iWeaponVolume = SCARH_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	auto vecSrc = m_pPlayer->GetGunPosition();
	auto vecAiming = gpGlobals->v_forward;

	auto vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, SCARH_EFFECTIVE_RANGE, SCARH_PENETRATION, m_iPrimaryAmmoType, SCARH_DAMAGE, SCARH_RANGE_MODIFER, m_pPlayer->random_seed);

#ifndef CLIENT_DLL
	int seq = UTIL_SharedRandomLong(m_pPlayer->random_seed, SCARH_SHOOT1, SCARH_SHOOT3);
	if (m_iClip == 0)
		seq = SCARH_SHOOT_LAST;

	SendWeaponAnim(seq);
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
	args.bparam1 = m_iClip == 0;	// originally it was 5. I changed it to whether it's empty.
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

	EV_FireSCARH(&args);
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.0f;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
	{
		KickBack(1.0, 0.45, 0.28, 0.04, 4.25, 2.5, 7);
	}
	else if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		KickBack(1.25, 0.45, 0.22, 0.18, 6.0, 4.0, 5);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		KickBack(0.6, 0.35, 0.2, 0.0125, 3.7, 2.0, 10);
	}
	else
	{
		KickBack(0.625, 0.375, 0.25, 0.0125, 4.0, 2.25, 9);
	}
}

bool CSCARH::Reload()
{
	if (DefaultReload(m_pItemInfo->m_iMaxClip,
		m_iClip ? SCARH_RELOAD : SCARH_RELOAD_EMPTY,
		m_iClip ? SCARH_RELOAD_TIME : SCARH_RELOAD_EMPTY_TIME,
		m_iClip ? 0.69f : 0.54f))
	{
		m_flAccuracy = 0.25f;
		return true;
	}

	// KF2 ???
	if (m_pPlayer->pev->weaponanim != SCARH_CHECK_MAGAZINE)
	{
		if (m_bInZoom)
			SecondaryAttack();

		SendWeaponAnim(SCARH_CHECK_MAGAZINE);
		m_flTimeWeaponIdle = SCARH_CHECK_MAGAZINE_TIME;
	}

	return false;
}

DECLARE_STANDARD_RESET_MODEL_FUNC(SCARH)
