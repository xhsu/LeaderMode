/*

Created Date: Mar 12 2020

*/

#include "precompiled.h"

int g_runfuncs = 0;
double g_flGameTime = 0;
const Vector g_vecZero = Vector(0, 0, 0);
int g_iCurViewModelAnim = 0;
CBaseWeapon* g_pCurWeapon = nullptr;
WeaponIdType g_iSelectedWeapon = WEAPON_NONE;

CBasePlayer gPseudoPlayer;
std::shared_ptr<pseudo_global_vars_s> gpGlobals;

CBaseWeapon* g_rgpClientWeapons[LAST_WEAPON];

//
// PSEUDO-PLAYER
//

bool g_bHoldingKnife = false;
bool g_bFreezeTimeOver = false;
bool g_bInBombZone = false;
bool g_bHoldingShield = false;

// the construction function of pseudo player and gpGlobals
CBasePlayer::CBasePlayer()
{
	pev = std::make_shared<pseudo_ent_var_s>();	// safe, auto-managed pointer.
}

Vector CBasePlayer::GetGunPosition()
{
	return pev->origin + pev->view_ofs;
}

Vector CBasePlayer::FireBullets3(Vector vecSrc, Vector vecDirShooting, float flSpread, float flDistance, int iPenetration, int iBulletType, int iDamage, float flRangeModifier, std::shared_ptr<pseudo_ent_var_s> pevAttacker, bool bPistol, int shared_rand)
{
	float x, y, z;

	if (pevAttacker)
	{
		x = UTIL_SharedRandomFloat(shared_rand, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 1, -0.5, 0.5);
		y = UTIL_SharedRandomFloat(shared_rand + 2, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 3, -0.5, 0.5);
	}
	else
	{
		do
		{
			x = gEngfuncs.pfnRandomFloat(-0.5, 0.5) + gEngfuncs.pfnRandomFloat(-0.5, 0.5);
			y = gEngfuncs.pfnRandomFloat(-0.5, 0.5) + gEngfuncs.pfnRandomFloat(-0.5, 0.5);
			z = x * x + y * y;
		}
		while (z > 1);
	}

	return Vector(x * flSpread, y * flSpread, 0);
}

int CBasePlayer::FireBuckshots(ULONG cShots, const Vector& vecSrc, const Vector& vecDirShooting, const Vector& vecSpread, float flDistance, int iDamage, int shared_rand)
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
		}
		while (z > 1);
	}

	return iSeedOfs;
}

void CBasePlayer::ResetMaxSpeed()
{
	// default speed.
	float speed = 240;

	if (m_pActiveItem)
	{
		// Get player speed from selected weapon
		speed = m_pActiveItem->GetMaxSpeed();
	}

	pev->maxspeed = speed;
}

//
// PSEUDO-WEAPON
//
BOOL CanAttack(float attack_time, float curtime, BOOL isPredicted)
{
#ifdef CLIENT_WEAPONS
	if (!isPredicted)
#else
	if (1)
#endif
	{
		return (attack_time <= curtime) ? TRUE : FALSE;
	}
	else
	{
		return (attack_time <= 0.0f) ? TRUE : FALSE;
	}
}

CBaseWeapon* CBaseWeapon::Give(WeaponIdType iId, CBasePlayer* pPlayer, int iClip, unsigned bitsFlags)
{
	CBaseWeapon* p = nullptr;

	switch (iId)
	{
	case WEAPON_AWP:
		p = new CAWP;
		break;

	case WEAPON_USP:
		p = new CUSP;
		break;

	case WEAPON_KSG12:
		p = new CKSG12;
		break;

	case WEAPON_CM901:
		p = new CCM901;
		break;

	default:
		return nullptr;
	}

	p->m_iId = iId;
	p->m_iClip = iClip ? iClip : g_rgItemInfo[iId].m_iMaxClip;
	p->m_bitsFlags = bitsFlags;
	p->m_pItemInfo = &g_rgItemInfo[iId];
	p->m_pAmmoInfo = &g_rgAmmoInfo[g_rgItemInfo[iId].m_iAmmoType];
	p->m_iPrimaryAmmoType = g_rgItemInfo[iId].m_iAmmoType;
	p->m_iSecondaryAmmoType = AMMO_NONE;
	p->m_pPlayer = pPlayer;

	// no call of AddToPlayer() here. it should be called from CBasePlayer::AddPlayerItem.

	return p;
}

void CBaseWeapon::Think(void)
{
	if (m_pPlayer->m_flEjectBrass != 0 && m_pPlayer->m_flEjectBrass <= gpGlobals->time)
	{
		m_pPlayer->m_flEjectBrass = 0;
		// we do nothing on client side.
		// that's because the brass ejected is visible globally, thus, it should be managed by SV.
	}
}

bool CBaseWeapon::AddToPlayer(CBasePlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	return true;
}

void CBaseWeapon::PostFrame()
{
	int usableButtons = m_pPlayer->pev->button;

	// since the call of BasicKnife::Swing() block the normal PostFrame() calls, this must be afterwards.
	// so the only thing we need to do here is to resume everything back to normal.
	if (m_bitsFlags & WPNSTATE_MELEE)
	{
		// we can't used a Holster() function for namespace BasicKnife{}, because it is used by all players.
		// we should just remove the melee flag for our weapon.
		m_bitsFlags &= ~WPNSTATE_MELEE;

		// if the player was reloading, then we should back to reload.
		if (m_bInReload)
		{
			Holster();	// the default Holster() would remove m_bInReload flag. Thus we have to do this.
			Deploy();

			PopAnim();
			m_bInReload = true;
		}
		else
		{
			// or, holster & re-deploy our gun.
			Holster();
			Deploy();
		}

		// wait for at least one frame.
		return;
	}

	// Return zoom level back to previous zoom level before we fired a shot.
	// This is used only for the AWP and Scout
	if (m_flNextPrimaryAttack <= UTIL_WeaponTimeBase())
	{
		if (m_pPlayer->m_bResumeZoom)
		{
			// return the fade level in zoom.
			m_pPlayer->pev->fov = m_pPlayer->m_iLastZoom;
			m_pPlayer->m_bResumeZoom = false;

			// we have additional thing to do on client site.
			gHUD::m_SniperScope.SetFadeFromBlack(5.0f);
		}
	}

	if (m_bInReload && m_pPlayer->m_flNextAttack <= UTIL_WeaponTimeBase())
	{
		// complete the reload.
		int j = Q_min(m_pItemInfo->m_iMaxClip - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);

		// Add them to the clip
		m_iClip += j;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= j;

		m_bInReload = false;
	}

	if ((usableButtons & IN_ATTACK2) && m_flNextSecondaryAttack <= UTIL_WeaponTimeBase())	// UseDecrement()
	{
		SecondaryAttack();
		m_pPlayer->pev->button &= ~IN_ATTACK2;
	}
	else if ((m_pPlayer->pev->button & IN_ATTACK) && CanAttack(m_flNextPrimaryAttack, UTIL_WeaponTimeBase(), TRUE))	// UseDecrement()
	{
		// Can't shoot during the freeze period
		// Always allow firing in single player
		if (m_pPlayer->m_bCanShoot)
		{
			PrimaryAttack();
		}
	}
	else if ((m_pPlayer->pev->button & IN_RELOAD) && m_pItemInfo->m_iMaxClip != WEAPON_NOCLIP && !m_bInReload && m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		Reload();
	}
	else if (!(usableButtons & (IN_ATTACK | IN_ATTACK2)))	// no fire buttons down
	{
		// The following code prevents the player from tapping the firebutton repeatedly
		// to simulate full auto and retaining the single shot accuracy of single fire
		if (m_iShotsFired > 15)
		{
			m_iShotsFired = 15;
		}

		m_flDecreaseShotsFired = gpGlobals->time + 0.4f;

		// if it's a pistol then set the shots fired to 0 after the player releases a button
		if (IsSecondaryWeapon(m_iId) && m_iId != WEAPON_GLOCK18)
		{
			m_iShotsFired = 0;
		}
		else
		{
			if (m_iShotsFired > 0 && m_flDecreaseShotsFired < gpGlobals->time)
			{
				m_flDecreaseShotsFired = gpGlobals->time + 0.0225f;
				m_iShotsFired--;
			}
		}


		if (!(m_bitsFlags & WPNSTATE_SHIELD_DRAWN))
		{
			// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
			if (!m_iClip && !(m_pItemInfo->m_bitsFlags & ITEM_FLAG_NOAUTORELOAD) && m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
			{
				Reload();
				return;
			}
		}
	}

	// catch all
	if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
	{
		WeaponIdle();
	}
}

bool CBaseWeapon::Melee(void)
{
	// you just.. can't do this.
	if (m_iId == WEAPON_KNIFE || m_bitsFlags & WPNSTATE_MELEE)
		return false;

	// do nothing melee-ish action on client side.
	// this has to be executed from SV.

	// but we should push-pop anims.
	PushAnim();
	return true;
}

bool CBaseWeapon::Holster(bool bTrial)
{
	if (m_bitsFlags & WPNSTATE_MELEE)	// you can't holster while meleeing.
		return false;

	// if this is only a trial like original CanHolster(), let's finish here.
	if (bTrial)
		return true;

	m_bInZoom = false;
	g_vecGunOfsGoal = g_vecZero;
	g_flGunOfsMovingSpeed = 10.0f;

	m_bInReload = false;
	m_pPlayer->pev->viewmodel = 0;
	m_pPlayer->pev->weaponmodel = 0;

	return true;
}

bool CBaseWeapon::Drop(void** ppWeaponBoxReturned)
{
	// the client site have no CWeaponBox.
	if (ppWeaponBoxReturned)
		(*ppWeaponBoxReturned) = nullptr;

	// we have nothing to do on client side.
	// you can't set m_pPlayer to be null here. since only one player existed at client.dll.

	return true;
}

bool CBaseWeapon::Kill(void)
{
	m_bitsFlags |= WPNSTATE_DEAD;
	return true;
}

bool CBaseWeapon::AddPrimaryAmmo(int iCount)
{
	return false;
}

bool CBaseWeapon::DefaultDeploy(const char* szViewModel, const char* szWeaponModel, int iAnim, const char* szAnimExt)
{
	// TODO
	/*if (!CanDeploy())
		return FALSE;*/

	gEngfuncs.CL_LoadModel(szViewModel, &m_pPlayer->pev->viewmodel);

	Q_strlcpy(m_pPlayer->m_szAnimExtention, szAnimExt);
	SendWeaponAnim(iAnim);

	m_pPlayer->m_flNextAttack = 0.75f;
	m_flTimeWeaponIdle = 1.5f;
	m_flDecreaseShotsFired = gpGlobals->time;

	m_pPlayer->pev->fov = DEFAULT_FOV;
	m_pPlayer->m_iLastZoom = DEFAULT_FOV;
	m_pPlayer->m_bResumeZoom = false;

	return TRUE;
}

void CBaseWeapon::SendWeaponAnim(int iAnim, int iBody, bool bSkipLocal)
{
	g_iCurViewModelAnim = iAnim;

	m_pPlayer->pev->weaponanim = iAnim;
	gEngfuncs.pfnWeaponAnim(iAnim, iBody);

	// save the time for the renderer.
	g_flTimeViewModelAnimStart = gEngfuncs.GetClientTime();
}

void CBaseWeapon::PlayEmptySound()
{
	// this should be played by Server.
}

bool CBaseWeapon::DefaultReload(int iClipSize, int iAnim, float fDelay)
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return false;

	int j = Q_min(iClipSize - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);
	if (!j)
	{
		return false;
	}

	if (m_bInZoom)
		SecondaryAttack();	// close scope when we reload.

	m_pPlayer->m_flNextAttack = fDelay;

	ReloadSound();
	SendWeaponAnim(iAnim);

	m_bInReload = true;
	m_flTimeWeaponIdle = fDelay + 0.5f;

	return true;
}

void CBaseWeapon::ReloadSound()
{
	// this should be playered by Server.
}

void CBaseWeapon::PushAnim(void)
{
	cl_entity_t* pViewEntity = gEngfuncs.GetViewModel();

	m_Stack.m_flEjectBrass			= m_pPlayer->m_flEjectBrass - gpGlobals->time;
	m_Stack.m_flFrame				= pViewEntity->curstate.frame;
	m_Stack.m_flFramerate			= pViewEntity->curstate.framerate;
	m_Stack.m_flNextAttack			= m_pPlayer->m_flNextAttack;
	m_Stack.m_flNextPrimaryAttack	= m_flNextPrimaryAttack;
	m_Stack.m_flNextSecondaryAttack	= m_flNextSecondaryAttack;
	m_Stack.m_flTimeAnimStarted		= gEngfuncs.GetClientTime() - g_flTimeViewModelAnimStart;
	m_Stack.m_flTimeWeaponIdle		= m_flTimeWeaponIdle;
	m_Stack.m_iSequence				= pViewEntity->curstate.sequence;
	m_Stack.m_iShellModelIndex		= m_pPlayer->m_iShellModelIndex;
}

void CBaseWeapon::PopAnim(void)
{
	// invalid pop.
	if (m_Stack.m_iSequence < 0)
		return;

	cl_entity_t* pViewEntity = gEngfuncs.GetViewModel();

	m_pPlayer->m_flEjectBrass		= m_Stack.m_flEjectBrass + gpGlobals->time;
	pViewEntity->curstate.frame		= m_Stack.m_flFrame;
	pViewEntity->curstate.framerate	= m_Stack.m_flFramerate;
	m_pPlayer->m_flNextAttack		= m_Stack.m_flNextAttack;
	m_flNextPrimaryAttack			= m_Stack.m_flNextPrimaryAttack;
	m_flNextSecondaryAttack			= m_Stack.m_flNextSecondaryAttack;
	g_flTimeViewModelAnimStart		= gEngfuncs.GetClientTime() - m_Stack.m_flTimeAnimStarted;
	m_flTimeWeaponIdle				= m_Stack.m_flTimeWeaponIdle;
	pViewEntity->curstate.sequence	= m_Stack.m_iSequence;	// you have to set all these 3 places at CL side.
	m_pPlayer->pev->weaponanim		= m_Stack.m_iSequence;
	g_iCurViewModelAnim				= m_Stack.m_iSequence;
	m_pPlayer->m_iShellModelIndex	= m_Stack.m_iShellModelIndex;

	// LUNA: I don't know why, but execute this can prevent anim-restart over. This has to be done on both side.
	gEngfuncs.pfnWeaponAnim(m_Stack.m_iSequence, 0);

	// clear old data, mark for invalid.
	Q_memset(&m_Stack, NULL, sizeof(m_Stack));
	m_Stack.m_iSequence = -1;
}

//
// PSEUDO-UTILS
//
void UTIL_MakeVectors(const Vector& vec)
{
	gEngfuncs.pfnAngleVectors(vec, gpGlobals->v_forward, gpGlobals->v_right, gpGlobals->v_up);
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
HUD_InitClientWeapons

Set up weapons, player and functions needed to run weapons code client-side.
=====================
*/
void HUD_InitClientWeapons(void)
{
	static bool initialized = false;
	if (initialized)
		return;

	initialized = true;

	gpGlobals = std::make_shared<pseudo_global_vars_s>();

	Q_memset(&g_rgpClientWeapons, NULL, sizeof(g_rgpClientWeapons));

	g_rgpClientWeapons[WEAPON_AWP] = CBaseWeapon::Give(WEAPON_AWP, &gPseudoPlayer);
	g_rgpClientWeapons[WEAPON_CM901] = CBaseWeapon::Give(WEAPON_CM901, &gPseudoPlayer);
	g_rgpClientWeapons[WEAPON_KSG12] = CBaseWeapon::Give(WEAPON_KSG12, &gPseudoPlayer);
	g_rgpClientWeapons[WEAPON_USP] = CBaseWeapon::Give(WEAPON_USP, &gPseudoPlayer);
}

// During our weapon prediction processing, we'll need to reference some data that is part of
//  the final state passed into the postthink functionality.  We'll set this pointer and then
//  reset it to NULL as appropriate
local_state_t* g_finalstate = nullptr;

// from view.cpp
extern Vector v_angles;

/*
=====================
HUD_WeaponsPostThink

Run Weapon firing code on client
=====================
*/
void HUD_WeaponsPostThink(local_state_s* from, local_state_s* to, usercmd_t* cmd, double time, unsigned int random_seed)
{
	int i;
	int buttonsChanged;
	static int lasthealth;
	int flags;

	// initiation
	HUD_InitClientWeapons();

	// Get current clock
	gpGlobals->time = time;

	// Fill in data based on selected weapon
	if (from->client.m_iId > WEAPON_NONE && from->client.m_iId < LAST_WEAPON)
		g_pCurWeapon = g_rgpClientWeapons[from->client.m_iId];

	// Store pointer to our destination entity_state_t so we can get our origin, etc. from it
	//  for setting up events on the client
	g_finalstate = to;

	// these vars have to be obtained nomatter what.
	g_iWaterLevel = from->client.waterlevel;

	// We are not predicting the current weapon, just bow out here.
	if (!g_pCurWeapon)
		return;

	// If we are running events/etc. go ahead and see if we
	//  managed to die between last frame and this one
	// If so, run the appropriate player killed or spawn function
	if (g_runfuncs)
	{
		if (to->client.health <= 0 && lasthealth > 0)
			g_pCurWeapon->Holster();	// killed
		else if (to->client.health > 0 && lasthealth <= 0)
			g_pCurWeapon->Deploy();	// spawned

		lasthealth = to->client.health;
	}

	for (i = 0; i < LAST_WEAPON; i++)
	{
		if (!g_rgpClientWeapons[i] || g_rgpClientWeapons[i]->m_iId != i)	// something wrong with this pointer..?
			continue;

		auto pCurrent = g_rgpClientWeapons[i];
		weapon_data_t* pfrom = from->weapondata + i;

		pCurrent->m_bInReload = pfrom->m_fInReload;
		pCurrent->m_bInZoom = pfrom->m_fInSpecialReload;
		pCurrent->m_iClip = pfrom->m_iClip;
		pCurrent->m_flNextPrimaryAttack = pfrom->m_flNextPrimaryAttack;
		pCurrent->m_flNextSecondaryAttack = pfrom->m_flNextSecondaryAttack;
		pCurrent->m_flTimeWeaponIdle = pfrom->m_flTimeWeaponIdle;
		//pCurrent->m_flStartThrow = time_point_t (duration_t (pfrom->fuser2));
		//pCurrent->m_flReleaseThrow = time_point_t (duration_t (pfrom->fuser3));
		//pCurrent->m_iSwing = pfrom->iuser1;
		pCurrent->m_bitsFlags = pfrom->m_iWeaponState;
		pCurrent->m_flLastFire = pfrom->m_fAimedDamage;
		pCurrent->m_iShotsFired = pfrom->m_fInZoom;
	}

	if (from->client.vuser4.x < AMMO_NONE || from->client.vuser4.x > AMMO_MAXTYPE)
		g_pCurWeapon->m_iPrimaryAmmoType = AMMO_NONE;
	else
	{
		g_pCurWeapon->m_iPrimaryAmmoType = (AmmoIdType)int(from->client.vuser4.x);
		gPseudoPlayer.m_rgAmmo[g_pCurWeapon->m_iPrimaryAmmoType] = (int)from->client.vuser4.y;
	}

	g_iWeaponFlags = g_pCurWeapon->m_bitsFlags;

	// For random weapon events, use this seed to seed random # generator
	gPseudoPlayer.random_seed = random_seed;

	// Get old buttons from previous state.
	gPseudoPlayer.m_afButtonLast = from->playerstate.oldbuttons;

	// Which buttsons chave changed
	buttonsChanged = (gPseudoPlayer.m_afButtonLast ^ cmd->buttons);	// These buttons have changed this frame

	// Debounced button codes for pressed/released
	// The changed ones still down are "pressed"
	gPseudoPlayer.m_afButtonPressed = buttonsChanged & cmd->buttons;
	// The ones not down are "released"
	gPseudoPlayer.m_afButtonReleased = buttonsChanged & (~cmd->buttons);

	// Set player variables that weapons code might check/alter
	gPseudoPlayer.pev->button = cmd->buttons;

	gPseudoPlayer.pev->deadflag = from->client.deadflag;
	gPseudoPlayer.pev->waterlevel = from->client.waterlevel;
	gPseudoPlayer.pev->maxspeed = STATE->client.maxspeed; //!!! Taking "to"
	gPseudoPlayer.pev->punchangle = STATE->client.punchangle; //!!! Taking "to"
	gPseudoPlayer.pev->fov = from->client.fov;
	gPseudoPlayer.pev->weaponanim = from->client.weaponanim;
	gPseudoPlayer.pev->viewmodel = from->client.viewmodel;
	gPseudoPlayer.m_flNextAttack = from->client.m_flNextAttack;

	g_iPlayerFlags = gPseudoPlayer.pev->flags = from->client.flags;
	g_vPlayerVelocity = gPseudoPlayer.pev->velocity = from->client.velocity;
	g_flPlayerSpeed = gPseudoPlayer.pev->velocity.Length();

	cl_entity_t* pplayer = gEngfuncs.GetLocalPlayer();
	if (pplayer)
	{
		gPseudoPlayer.pev->origin = STATE->client.origin; //!!! Taking "to"
		gPseudoPlayer.pev->angles = pplayer->angles;
		gPseudoPlayer.pev->v_angle = v_angles;	// from view.cpp
	}

	flags = from->client.iuser3;
	g_bHoldingKnife = g_pCurWeapon->m_iId == WEAPON_KNIFE;
	gPseudoPlayer.m_bCanShoot = (flags & PLAYER_CAN_SHOOT) != 0;
	g_bFreezeTimeOver = !(flags & PLAYER_FREEZE_TIME_OVER);
	g_bInBombZone = (flags & PLAYER_IN_BOMB_ZONE) != 0;
	g_bHoldingShield = (flags & PLAYER_HOLDING_SHIELD) != 0;

	// Point to current weapon object
	if (from->client.m_iId)
		gPseudoPlayer.m_pActiveItem = g_pCurWeapon;

	// Don't go firing anything if we have died.
	// Or if we don't have a weapon model deployed
	if ((gPseudoPlayer.pev->deadflag != (DEAD_DISCARDBODY + 1)) && !CL_IsDead() && gPseudoPlayer.pev->viewmodel && !g_iUser1)
	{
		// LUNA: the weapon think would be called anyway.
		g_pCurWeapon->Think();

		if (g_bHoldingShield && g_pCurWeapon->m_bInReload && gPseudoPlayer.pev->button & IN_ATTACK2)	// fixed by referencing IDA.
		{
			gPseudoPlayer.m_flNextAttack = 0;
		}
		else if (gPseudoPlayer.m_flNextAttack <= 0.0f)
		{
			g_pCurWeapon->PostFrame();
		}
	}

	// Assume that we are not going to switch weapons
	to->client.m_iId = from->client.m_iId;

	// Now see if we issued a changeweapon command ( and we're not dead )
	if (cmd->weaponselect && (gPseudoPlayer.pev->deadflag != (DEAD_DISCARDBODY + 1)))
	{
		// Switched to a different weapon?
		if (from->weapondata[cmd->weaponselect].m_iId == cmd->weaponselect)
		{
			CBaseWeapon* pNew = g_rgpClientWeapons[cmd->weaponselect];
			if (pNew && (pNew != g_pCurWeapon))
			{
				// Put away old weapon
				if (gPseudoPlayer.m_pActiveItem)
					gPseudoPlayer.m_pActiveItem->Holster();

				gPseudoPlayer.m_pLastItem = gPseudoPlayer.m_pActiveItem;
				gPseudoPlayer.m_pActiveItem = pNew;

				// Deploy new weapon
				if (gPseudoPlayer.m_pActiveItem)
				{
					gPseudoPlayer.m_pActiveItem->Deploy();
				}

				// Update weapon id so we can predict things correctly.
				to->client.m_iId = cmd->weaponselect;
			}
		}
	}

	// Copy in results of prediction code
	to->client.viewmodel = gPseudoPlayer.pev->viewmodel;
	to->client.fov = gPseudoPlayer.pev->fov;
	to->client.weaponanim = gPseudoPlayer.pev->weaponanim;
	to->client.m_flNextAttack = gPseudoPlayer.m_flNextAttack;
	to->client.maxspeed = gPseudoPlayer.pev->maxspeed;
	to->client.punchangle = gPseudoPlayer.pev->punchangle;

	to->client.iuser3 = flags;

	// FIXME: this was working fine until the push-pop anim comes out. Why my predict code being override right on next frame?
	// Make sure that weapon animation matches what the game .dll is telling us
	//  over the wire ( fixes some animation glitches )
	if (g_runfuncs && (g_iCurViewModelAnim != to->client.weaponanim))
		// Force a fixed anim down to viewmodel. LUNA: FIXME: wired, how did that happens? it seems impossible.
		g_pCurWeapon->SendWeaponAnim(to->client.weaponanim);

	if (g_pCurWeapon->m_iPrimaryAmmoType < AMMO_MAXTYPE)
	{
		to->client.vuser4.x = g_pCurWeapon->m_iPrimaryAmmoType;
		to->client.vuser4.y = gPseudoPlayer.m_rgAmmo[g_pCurWeapon->m_iPrimaryAmmoType];
	}
	else
	{
		to->client.vuser4.x = -1.0;
		to->client.vuser4.y = 0;
	}

	for (i = 0; i < LAST_WEAPON; i++)
	{
		CBaseWeapon* pCurrent = g_rgpClientWeapons[i];

		weapon_data_t* pto = to->weapondata + i;

		if (!pCurrent)
		{
			Q_memset(pto, NULL, sizeof(weapon_data_t));
			continue;
		}

		pto->m_iClip = pCurrent->m_iClip;

		pto->m_flNextPrimaryAttack = pCurrent->m_flNextPrimaryAttack;
		pto->m_flNextSecondaryAttack = pCurrent->m_flNextSecondaryAttack;
		pto->m_flTimeWeaponIdle = pCurrent->m_flTimeWeaponIdle;

		pto->m_fInReload = pCurrent->m_bInReload;
		pto->m_fInSpecialReload = pCurrent->m_bInZoom;
		//pto->m_flNextReload = pCurrent->m_flNextReload / 1s;
		//pto->fuser2 = pCurrent->m_flStartThrow.time_since_epoch() / 1s;
		//pto->fuser3 = pCurrent->m_flReleaseThrow.time_since_epoch() / 1s;
		//pto->iuser1 = pCurrent->m_iSwing;
		pto->m_iWeaponState = pCurrent->m_bitsFlags;
		pto->m_fInZoom = pCurrent->m_iShotsFired;
		pto->m_fAimedDamage = pCurrent->m_flLastFire;

		// Decrement weapon counters, server does this at same time ( during post think, after doing everything else )
		// LUNA: is this gpGlobals->framerate ???
		pto->m_flNextReload -= cmd->msec / 1000.0f;
		pto->m_fNextAimBonus -= cmd->msec / 1000.0f;
		pto->m_flNextPrimaryAttack -= cmd->msec / 1000.0f;
		pto->m_flNextSecondaryAttack -= cmd->msec / 1000.0f;
		pto->m_flTimeWeaponIdle -= cmd->msec / 1000.0f;


		if (pto->m_flPumpTime != -9999.0f)
		{
			pto->m_flPumpTime -= cmd->msec / 1000.0f;
			if (pto->m_flPumpTime < -1.0f)
				pto->m_flPumpTime = 1.0f;
		}

		if (pto->m_fNextAimBonus < -1.0)
		{
			pto->m_fNextAimBonus = -1.0;
		}

		if (pto->m_flNextPrimaryAttack < -1.0)
		{
			pto->m_flNextPrimaryAttack = -1.0;
		}

		if (pto->m_flNextSecondaryAttack < -0.001)
		{
			pto->m_flNextSecondaryAttack = -0.001;
		}

		if (pto->m_flTimeWeaponIdle < -0.001)
		{
			pto->m_flTimeWeaponIdle = -0.001;
		}

		if (pto->m_flNextReload < -0.001)
		{
			pto->m_flNextReload = -0.001;
		}

		/*if ( pto->fuser1 < -0.001 )
		{
			pto->fuser1 = -0.001;
		}*/
	}

	// m_flNextAttack is now part of the weapons, but is part of the player instead
	to->client.m_flNextAttack -= cmd->msec / 1000.0f;
	if (to->client.m_flNextAttack < -0.001)
	{
		to->client.m_flNextAttack = -0.001;
	}

	// Wipe it so we can't use it after this frame
	g_finalstate = NULL;
}

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
	g_runfuncs = runfuncs;
	g_flGameTime = time;

	HUD_WeaponsPostThink(from, to, cmd, time, random_seed);
	to->client.fov = g_lastFOV;

	if (runfuncs)
	{
		CounterStrike_SetSequence(to->playerstate.sequence, to->playerstate.gaitsequence);
		CounterStrike_SetOrientation(to->playerstate.origin, cmd->viewangles);
	}
}

/*
=====================
CommandFunc_Melee

We are not doing SV stuff here. It just allow us to predict the anim push-pop.
=====================
*/
void CommandFunc_Melee(void)
{
	if (!CL_IsDead() && g_pCurWeapon)
	{
		g_pCurWeapon->Melee();
		gEngfuncs.pfnServerCmd("melee\n");	// forward this msg to SV for further action.
	}
}

/*
=====================
Wpn_Init
=====================
*/
void Wpn_Init()
{
	gEngfuncs.pfnAddCommand("melee", CommandFunc_Melee);
}
