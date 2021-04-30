/*

Created Date: Apr 29 2021

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

CBasePlayer gLocalPlayer;
pseudo_global_vars_s* gpGlobals = new pseudo_global_vars_s;	// Fuck the smart pointer. It cannot be create on start.
float g_flPlayerSpeed = 0.0f;

// From view.cpp
extern Vector v_angles;

// the construction function of pseudo player and gpGlobals
CBasePlayer::CBasePlayer()
{
	pev = std::make_shared<pseudo_ent_var_s>();	// safe, auto-managed pointer.
}

Vector CBasePlayer::GetGunPosition()
{
	return pev->origin + pev->view_ofs;
}

Vector2D CBasePlayer::FireBullets3(Vector vecSrc, Vector vecDirShooting, float vecSpread, float flDistance, int iPenetration, AmmoIdType iBulletType, int iDamage, float flRangeModifier, int shared_rand)
{
	float x, y, z;

	if (pev)
	{
		x = UTIL_SharedRandomFloat(shared_rand, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 1, -0.5, 0.5);
		y = UTIL_SharedRandomFloat(shared_rand + 2, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 3, -0.5, 0.5);

		// print for debug.
#ifdef RANDOM_SEED_CALIBRATION
		gEngfuncs.pfnConsolePrint(SharedVarArgs("CL:[seed: %d] x: %f, y: %f\n", shared_rand, x, y));
#endif
	}
	else
	{
		do
		{
			x = gEngfuncs.pfnRandomFloat(-0.5, 0.5) + gEngfuncs.pfnRandomFloat(-0.5, 0.5);
			y = gEngfuncs.pfnRandomFloat(-0.5, 0.5) + gEngfuncs.pfnRandomFloat(-0.5, 0.5);
			z = x * x + y * y;
		} 		while (z > 1);
	}

	return Vector2D(x * vecSpread, y * vecSpread);
}

int CBasePlayer::FireBuckshots(ULONG cShots, const Vector& vecSrc, const Vector& vecDirShooting, const Vector& vecSpread, float flDistance, int iDamage, float flExponentialBase, int shared_rand)
{
	int iSeedOfs = 0;	// keep track how many times we used the shared_rand.

	for (ULONG iShot = 1; iShot <= cShots; iShot++)
	{
		// get circular gaussian spread
		float x, y, z;

		do
		{
			x = UTIL_SharedRandomFloat(shared_rand + iSeedOfs, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + iSeedOfs + 1, -0.5, 0.5);
			y = UTIL_SharedRandomFloat(shared_rand + iSeedOfs + 2, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + iSeedOfs + 3, -0.5, 0.5);
			z = x * x + y * y;

			// we used 4 times, thus we plus 4.
			iSeedOfs += 4;
		} 		while (z > 1);
	}

	return iSeedOfs;
}

void CBasePlayer::ResetMaxSpeed()
{
	// default speed.
	float speed = 240;

	// WPN_UNDONE
	//if (m_pActiveItem)
	//{
	//	// Get player speed from selected weapon
	//	speed = m_pActiveItem->GetMaxSpeed();
	//}

	pev->maxspeed = speed;
}

int CBasePlayer::GetGrenadeInventory(EquipmentIdType iId)
{
	return *GetGrenadeInventoryPointer(iId);
}

int* CBasePlayer::GetGrenadeInventoryPointer(EquipmentIdType iId)
{
	return &m_rgAmmo[GetAmmoIdOfEquipment(iId)];
}

void CBasePlayer::ResetUsingEquipment(void)
{
	// the stock is ok.
	if (GetGrenadeInventory(m_iUsingGrenadeId) > 0)
		return;

	AmmoIdType iAmmoId = AMMO_NONE;
	EquipmentIdType iCandidate = EQP_NONE;

	// start the searching from the first.
	for (int i = EQP_NONE; i < EQP_COUNT; i++)
	{
		iAmmoId = GetAmmoIdOfEquipment((EquipmentIdType)i);

		if (!iAmmoId || m_rgAmmo[iAmmoId] <= 0)
			continue;

		iCandidate = (EquipmentIdType)i;
		break;
	}

	m_iUsingGrenadeId = iCandidate;
}

bool CBasePlayer::StartSwitchingWeapon(WeaponIdType iId)
{
	// WPN_UNDONE
	return false;
}

// Name says it!
// Override stupid Xash(or even GoldSrc?) bug with overwriting
// already predicted values, like maxspeed or punchangle vector
#define _CS16CLIENT_TAKE_PREDICTED_INFO_FOR_WEAPON_PREDICTION

#ifdef _CS16CLIENT_TAKE_PREDICTED_INFO_FOR_WEAPON_PREDICTION
#define STATE to
#else
#define STATE from
#endif

/*
=====================
HUD_PostRunCmd

Client calls this during prediction, after it has moved the player and updated any info changed into to->
time is the current client clock based on prediction
cmd is the command that caused the movement, etc
runfuncs is 1 if this is the first time we've predicted this command.  If so, sounds and effects should play, otherwise, they should
be ignored
=====================
*/
void HUD_PostRunCmd2(local_state_t* from, local_state_t* to, usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
{
	int i;
	int buttonsChanged;
	static int lasthealth;
	int flags;

	// Get current clock
	gpGlobals->time = time;

	// these vars have to be obtained nomatter what.
	gLocalPlayer.pev->waterlevel = from->client.waterlevel;
	gLocalPlayer.pev->health = to->client.health;

	// For random weapon events, use this seed to seed random # generator
	gLocalPlayer.random_seed = random_seed;

	// Get old buttons from previous state.
	gLocalPlayer.m_afButtonLast = from->playerstate.oldbuttons;

	// Which buttsons chave changed
	buttonsChanged = (gLocalPlayer.m_afButtonLast ^ cmd->buttons);	// These buttons have changed this frame

	// Debounced button codes for pressed/released
	// The changed ones still down are "pressed"
	gLocalPlayer.m_afButtonPressed = buttonsChanged & cmd->buttons;
	// The ones not down are "released"
	gLocalPlayer.m_afButtonReleased = buttonsChanged & (~cmd->buttons);

	// Set player variables that weapons code might check/alter
	gLocalPlayer.pev->button = cmd->buttons;

	gLocalPlayer.pev->deadflag = from->client.deadflag;
	gLocalPlayer.pev->waterlevel = from->client.waterlevel;
	gLocalPlayer.pev->maxspeed = STATE->client.maxspeed; //!!! Taking "to"
	gLocalPlayer.pev->punchangle = STATE->client.punchangle; //!!! Taking "to"
	gLocalPlayer.pev->fov = gHUD::m_iFOV;	// 11072020 LUNA: from->client.fov is broken, keeping give me 0.
	gLocalPlayer.pev->weaponanim = from->client.weaponanim;
	gLocalPlayer.pev->viewmodel = from->client.viewmodel;

	gLocalPlayer.pev->flags = from->client.flags;
	gLocalPlayer.pev->velocity = from->client.velocity;
	g_flPlayerSpeed = gLocalPlayer.pev->velocity.Length();

	cl_entity_t* pplayer = gEngfuncs.GetLocalPlayer();
	if (pplayer)
	{
		gLocalPlayer.pev->origin = STATE->client.origin; //!!! Taking "to"
		gLocalPlayer.pev->angles = pplayer->angles;
		gLocalPlayer.pev->v_angle = v_angles;	// from view.cpp
	}

	flags = from->client.iuser3;
	gLocalPlayer.m_bCanShoot = (flags & PLAYER_CAN_SHOOT) != 0;
	g_bFreezeTimeOver = !(flags & PLAYER_FREEZE_TIME_OVER);

	// Assume that we are not going to switch weapons
	to->client.m_iId = from->client.m_iId;

	// Copy in results of prediction code
	to->client.viewmodel = gLocalPlayer.pev->viewmodel;
	to->client.fov = gLocalPlayer.pev->fov;
	to->client.weaponanim = gLocalPlayer.pev->weaponanim;
	to->client.maxspeed = gLocalPlayer.pev->maxspeed;
	to->client.punchangle = gLocalPlayer.pev->punchangle;

	to->client.iuser3 = flags;

	// m_flNextAttack is now part of the weapons, but is part of the player instead
	to->client.m_flNextAttack -= cmd->msec / 1000.0f;
	if (to->client.m_flNextAttack < -0.001)
	{
		to->client.m_flNextAttack = -0.001;
	}

	to->client.fov = g_lastFOV;

	if (runfuncs)
	{
		CounterStrike_SetSequence(to->playerstate.sequence, to->playerstate.gaitsequence);
		CounterStrike_SetOrientation(to->playerstate.origin, cmd->viewangles);
	}
}
