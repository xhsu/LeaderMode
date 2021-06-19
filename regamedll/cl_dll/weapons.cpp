/*

Created Date: Mar 12 2020

*/

#include "precompiled.h"

int g_runfuncs = 0;
local_state_t g_sWpnFrom;
local_state_t g_sWpnTo;
usercmd_t g_sWpnCmd;
const Vector g_vecZero = Vector(0, 0, 0);
int g_iCurViewModelAnim = 0;
WeaponIdType g_iSelectedWeapon = WEAPON_NONE;	// this means directly switch weapon. try to use gPseudoPlayer.StartSwitchingWeapon() instead!
cvar_t* cl_holdtoaim = nullptr;	// HOLD to aim vs. PRESS to aim.

//
// PSEUDO-PLAYER
//

bool g_bHoldingKnife = false;
bool g_bFreezeTimeOver = false;
bool g_bInBombZone = false;
bool g_bHoldingShield = false;
bool g_bIsBlocked = false;	// this should be override, but tell the server!

inline decltype(auto) CSGameRules(void)
{
	static std::shared_ptr<pseudo_gamerule_s> p = std::make_shared<pseudo_gamerule_s>();

	return p;
}

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
		}
		while (z > 1);
	}

	return Vector2D(x * vecSpread, y * vecSpread);
}

int CBasePlayer::FireBuckshots(ULONG cShots, const Vector& vecSrc, const Vector& vecDirShooting, const Vector2D& vecSpread, float flDistance, int iDamage, float flExponentialBase, int shared_rand)
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

bool CBasePlayer::StartSwitchingWeapon(CBaseWeapon* pSwitchingTo)
{
	if (!pSwitchingTo || pSwitchingTo->IsDead())
		return false;

	// TODO
	if ((m_pActiveItem && !m_pActiveItem->CanHolster()) /*|| !pSwitchingTo->CanDeploy()*/)
		return false;

	// you can't deploy same weapon twice.
	if (pSwitchingTo == m_pActiveItem)
		return false;

	if (m_pActiveItem)
	{
		m_pActiveItem->HolsterStart();
		m_pWpnSwitchingTo = pSwitchingTo;

		return true;
	}
	else
	{
		// no active weapon? which means we can directly deploy this one.
		g_iSelectedWeapon = pSwitchingTo->m_iId;	// Additional line at client. Investigate the necessity.
		return SwitchWeapon(pSwitchingTo);
	}
}

bool CBasePlayer::StartSwitchingWeapon(WeaponIdType iSwitchingTo)
{
	for (auto& pWeapon : CBaseWeapon::m_lstWeapons)
	{
		if (pWeapon->IsDead())
			continue;

		if (pWeapon->m_iId != iSwitchingTo)
			continue;

		StartSwitchingWeapon(pWeapon);
		return true;
	}

	return false;
}

bool CBasePlayer::SwitchWeapon(CBaseWeapon* pSwitchingTo)
{
	if (!pSwitchingTo || pSwitchingTo->IsDead())
		return false;

	if (m_pActiveItem && m_pActiveItem->IsDead())
		m_pActiveItem = nullptr;

	// TODO
	if ((m_pActiveItem && !m_pActiveItem->CanHolster()) /*|| !pSwitchingTo->CanDeploy()*/)
		return false;

	if (m_pActiveItem)
	{
		m_pActiveItem->Holstered();
	}

	m_pLastItem = m_pActiveItem;
	m_pActiveItem = pSwitchingTo;

	m_pActiveItem->Deploy();

	ResetMaxSpeed();

	return true;
}

CBaseWeapon* CBasePlayer::HasWeapons(WeaponIdType iId)
{
	for (auto& pWeapon : CBaseWeapon::m_lstWeapons)
	{
		if (pWeapon->IsDead())
			continue;

		if (pWeapon->m_iId != iId)
			continue;

		return pWeapon;
	}

	return nullptr;
}

// Add a weapon to the player (Item == Weapon == Selectable Object)
bool CBasePlayer::AddPlayerItem(CBaseWeapon* pItem)
{
	if (!pItem)
		return false;

	if (pItem->AddToPlayer(this))
	{
		// No gamerule event here.

		if (pItem->m_pItemInfo->m_iSlot == PRIMARY_WEAPON_SLOT)
			m_bHasPrimary = true;

		// get it into player's inventory.
		m_rgpPlayerItems[pItem->m_pItemInfo->m_iSlot] = pItem;
		pev->weapons |= (1 << pItem->m_iId);	// TODO: abolish this?

		// FX will be done at server side.

		// Slot info will be sent from server side

		// Shield settings can only be done from server side.

		// should we switch to this item?
		if (CSGameRules()->FShouldSwitchWeapon(this, pItem))
		{
			StartSwitchingWeapon(pItem);
		}

		gHUD::m_bitsHideHUDDisplay &= ~HIDEHUD_WEAPONS;
		return true;
	}

	return false;
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

bool pseudo_gamerule_s::FShouldSwitchWeapon(CBasePlayer* pPlayer, CBaseWeapon* pWeapon)
{
	// TODO: maybe reuse this ?
	/*if (!pWeapon->CanDeploy())
	{
		// that weapon can't deploy anyway.
		return FALSE;
	}*/

	if (!pPlayer->m_pActiveItem)
	{
		// player doesn't have an active item!
		return TRUE;
	}

	// TODO: How to get keyinfo from client side?
	// Infobuffer name: _cl_autowepswitch
//	if (!pPlayer->m_iAutoWepSwitch)
//		return FALSE;

//	if (pPlayer->m_iAutoWepSwitch == 2 && (pPlayer->m_afButtonLast & (IN_ATTACK | IN_ATTACK2)))
//		return FALSE;

	if (!pPlayer->m_pActiveItem->CanHolster())
	{
		// can't put away the active item.
		return FALSE;
	}

	if (pWeapon->m_pItemInfo->m_iWeight > pPlayer->m_pActiveItem->m_pItemInfo->m_iWeight)
		return TRUE;

	return FALSE;
}

void CBaseWeapon::TheWeaponsThink(void)
{
	for (auto iter = m_lstWeapons.begin(); iter != m_lstWeapons.end(); /*do nothing*/)
	{
		auto p = *iter;

		if (p->IsDead())
		{
			delete p;
			iter = m_lstWeapons.erase(iter);
		}
		else
		{
			// Player no longer in the game.
			if (!g_bInGameWorld)
				p->Kill();

			// Player gets killed.
			if (CL_IsDead())
				p->Kill();

			iter++;
		}
	}
}

CBaseWeapon* CBaseWeapon::Give(WeaponIdType iId, CBasePlayer* pPlayer, int iClip, unsigned bitsFlags)
{
	CBaseWeapon* p = nullptr;

	switch (iId)
	{
	case WEAPON_AK47:
		p = new CAK47;
		break;

	case WEAPON_ANACONDA:
		p = new CAnaconda;
		break;

	case WEAPON_AWP:
		p = new CAWP;
		break;

	case WEAPON_DEAGLE:
		p = new CDEagle;
		break;

	case WEAPON_M45A1:
		p = new CM45A1;
		break;

	case WEAPON_FIVESEVEN:
		p = new CFN57;
		break;

	case WEAPON_GLOCK18:
		p = new CG18C;
		break;

	case WEAPON_KSG12:
		p = new CKSG12;
		break;

	case WEAPON_M1014:
		p = new CM1014;
		break;

	case WEAPON_M4A1:
		p = new CM4A1;
		break;

	case WEAPON_MK46:
		p = new CMK46;
		break;

	case WEAPON_MP7A1:
		p = new CMP7A1;
		break;

	case WEAPON_PSG1:
		p = new CPSG1;
		break;

	case WEAPON_SCARH:
		p = new CSCARH;
		break;

	case WEAPON_SVD:
		p = new CSVD;
		break;

	case WEAPON_UMP45:
		p = new CUMP45;
		break;

	case WEAPON_USP:
		p = new CUSP;
		break;

	case WEAPON_XM8:
		p = new CXM8;
		break;

	default:
		return nullptr;
	}

	m_lstWeapons.emplace_back(p);

	p->m_iId = iId;
	p->m_iClip = iClip ? iClip : g_rgWpnInfo[iId].m_iMaxClip;
	p->m_bitsFlags = bitsFlags;
	p->m_pItemInfo = &g_rgWpnInfo[iId];
	p->m_pAmmoInfo = &g_rgAmmoInfo[g_rgWpnInfo[iId].m_iAmmoType];
	p->m_iPrimaryAmmoType = g_rgWpnInfo[iId].m_iAmmoType;
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

	if (!(m_bitsFlags & WPNSTATE_BUSY) && m_pPlayer->pev->button & IN_RUN && m_pPlayer->pev->button & IN_FORWARD && !(m_pPlayer->pev->flags & FL_DUCKING) && m_pPlayer->pev->flags & FL_ONGROUND)
	{
		DashStart();
	}

	if (m_bitsFlags & WPNSTATE_DASHING &&
		(m_pPlayer->m_afButtonReleased & IN_RUN || !(m_pPlayer->pev->button & IN_FORWARD) || m_pPlayer->pev->flags & FL_DUCKING || !(m_pPlayer->pev->flags & FL_ONGROUND)/* || m_pPlayer->pev->velocity.Length2D() < 50.0f*/)
		)
	{
		DashEnd();
	}

	// non other condition matters. it's all packed in the QTS/QTR function.
	if (m_pPlayer->m_afButtonPressed & IN_THROW)
		QuickThrowStart(m_pPlayer->m_iUsingGrenadeId);
	else if (m_pPlayer->m_afButtonReleased & IN_THROW)
		QuickThrowRelease();
}

bool CBaseWeapon::AddToPlayer(CBasePlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	m_bitsFlags |= WPNSTATE_DRAW_FIRST;

	SetVariation(g_iRoleType);
	return true;
}

template<class CWpn>
bool CBaseWeaponTemplate<CWpn>::Deploy(void)
{
	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ACCURACY_BASELINE))
		m_flAccuracy = CWpn::ACCURACY_BASELINE;

#ifdef CLIENT_DLL
	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(SHELL_MODEL))
		m_iShell<CWpn> = gEngfuncs.pEventAPI->EV_FindModelIndex(CWpn::SHELL_MODEL);
#endif

	m_iShotsFired = 0;

	return DefaultDeploy(
		CWpn::VIEW_MODEL,
		CWpn::WORLD_MODEL,
		(m_bitsFlags & WPNSTATE_DRAW_FIRST) ? CWpn::DRAW_FIRST : CWpn::DEPLOY,
		CWpn::POSTURE,
		(m_bitsFlags & WPNSTATE_DRAW_FIRST) ? CWpn::DRAW_FIRST_TIME : CWpn::DEPLOY_TIME
	);
}

void CBaseWeapon::PostFrame()
{
	int usableButtons = CL_ButtonBits();//m_pPlayer->pev->button;

	// if we should be holster, then just do it. stop everything else.
	if (m_bitsFlags & WPNSTATE_HOLSTERING)
	{
		m_pPlayer->SwitchWeapon(m_pPlayer->m_pWpnSwitchingTo);
		return;
	}

	// we can't do anything during dash.
	if (m_bitsFlags & WPNSTATE_DASHING)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
			WeaponIdle();

		return;
	}

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
			ResetModel();	// you have to switch from knife model to gun model.
			PopAnim();		// then you may resume you anim.
		}
		else
		{
			// or, holster & re-deploy our gun.
			Holstered();
			Deploy();
		}

		// wait for at least one frame.
		return;
	}

	// the handle of WPNSTATE_QUICK_THROWING
	if (m_bitsFlags & WPNSTATE_QUICK_THROWING)
	{
		if (m_bitsFlags & WPNSTATE_QT_EXIT)
		{
			// remove all flags.
			m_bitsFlags &= ~(WPNSTATE_QUICK_THROWING | WPNSTATE_QT_RELEASE | WPNSTATE_QT_SHOULD_SPAWN | WPNSTATE_QT_EXIT);

			switch (m_pPlayer->m_iUsingGrenadeId)
			{
			case EQP_C4:	// left handed model.
			case EQP_DETONATOR:
				gSecViewModelMgr.m_bVisible = false;
				SetLeftHand(true);
				break;

			default:
				// back to our weapon.
				Holstered();
				Deploy();
				break;
			}

			// don't do this on CL side.
			// we don't have CSGameRules() on client.
			//m_pPlayer->ResetUsingEquipment();

			// wait for 1 frame.
			return;
		}

		// throw the grenade.
		else if (m_bitsFlags & WPNSTATE_QT_SHOULD_SPAWN)
		{
			float flTime = TIME_GR_IDLE_LOOP;
			switch (m_pPlayer->m_iUsingGrenadeId)
			{
			case EQP_HEGRENADE:
			case EQP_CRYOGRENADE:
			case EQP_INCENDIARY_GR:
			case EQP_FLASHBANG:
				flTime = TIME_QT_THROWING_FAR - TIME_SP_QT_THROWING_FAR;	// the post-throwing time.
				break;

			case EQP_SMOKEGRENADE:
			case EQP_GAS_GR:
			case EQP_HEALING_GR:
				flTime = TIME_QT_THROWING_SOFT - TIME_SP_QT_THROWING_SOFT;
				break;

			case EQP_C4:
				flTime = C4_TIME_THROW - C4_TIME_THROW_SPAWN;
				break;	// unlike SV, we can safely use the original line here.

			case EQP_DETONATOR:
				flTime = C4_TIME_DET_ANIM - C4_TIME_DETONATE;
				break;	// unlike SV, we can safely use the original line here.

			case EQP_FLASHLIGHT:	// Flashlight should never get you here. But, anyway...
			default:
				return;	// how did he get here???
			}

			// we should remove a grenade from inventory... but no, not here.
			// it would cause some trouble regarding Bombard(CSkillInfiniteGrenade) skill.
			// reduced one from client, but not on SV. And we are lack of synchronizing method.

			// we should spawn a grenade at this line... on SV.
			// at client, we do nothing.

			m_bitsFlags |= WPNSTATE_QT_EXIT;
			m_pPlayer->m_flNextAttack = flTime;
		}

		// play the release anim.
		else if (m_bitsFlags & WPNSTATE_QT_RELEASE)
		{
			int iAnim = GR_IDLE;
			float flTime = TIME_GR_IDLE_LOOP;
			switch (m_pPlayer->m_iUsingGrenadeId)
			{
			case EQP_HEGRENADE:
			case EQP_CRYOGRENADE:
			case EQP_INCENDIARY_GR:
			case EQP_FLASHBANG:
				iAnim = QT_THROWING_FAR;
				flTime = TIME_SP_QT_THROWING_FAR;
				break;

			case EQP_SMOKEGRENADE:
			case EQP_GAS_GR:
			case EQP_HEALING_GR:
				iAnim = QT_THROWING_SOFT;
				flTime = TIME_SP_QT_THROWING_SOFT;
				break;

			case EQP_C4:	// special treatment.
			{
				iAnim = C4_THROW;
				Vector vecSrc = m_pPlayer->GetGunPosition();
				Vector vecEnd = vecSrc + 32.0f * (m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle).MakeVector();

				pmtrace_t tr;
				UTIL_TraceLine(vecSrc, vecEnd, PM_WORLD_ONLY, -1, &tr, gEngfuncs.GetLocalPlayer()->index);

				// if you near a wall that much...
				if (tr.fraction < 1.0f)
					iAnim = C4_PLACE;

				m_bitsFlags |= WPNSTATE_QT_SHOULD_SPAWN;
				gSecViewModelMgr.SetAnim(iAnim);
				m_pPlayer->m_flNextAttack = C4_TIME_THROW_SPAWN;

				return;
			}

			default:
				return;	// how did he get here???
			}

			m_bitsFlags |= WPNSTATE_QT_SHOULD_SPAWN;
			SendWeaponAnim(iAnim);
			m_pPlayer->m_flNextAttack = flTime;
		}

		// hold and do nothing.
		else
		{
		}

		return;
	}

	if (m_bitsFlags & (WPNSTATE_NO_LHAND | WPNSTATE_AUTO_LAND_UP) && m_pPlayer->m_flNextAttack <= 0.0f)
	{
		gSecViewModelMgr.m_bVisible = false;
		SetLeftHand(true);	// make LHAND back up.

		m_bitsFlags &= ~WPNSTATE_AUTO_LAND_UP;
		return;	// skip this frame.
	}

	// Return zoom level back to previous zoom level before we fired a shot.
	// It could also be used in entering a scope with a delay.
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

	// complete the magazine-based reload.
	if (m_bInReload && m_pPlayer->m_flNextAttack <= UTIL_WeaponTimeBase())
	{
		int j = Q_min(m_pItemInfo->m_iMaxClip - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);

		// Add them to the clip
		m_iClip += j;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= j;

		// not reloaded from empty? extra 1 bullet.
		if (!(m_bitsFlags & WPNSTATE_RELOAD_EMPTY) && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0)
		{
			m_iClip++;
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
		}

		m_bInReload = false;
		m_bitsFlags &= ~WPNSTATE_RELOAD_EMPTY;	// remove it anyway.
	}

	// CL EXCLUSIVE!!
	// Muzzle block check. Prevent players from hiding or peaking between covers/murder holes.
	// That's a DISHONOUR behavior. I want to stop it.
	if (m_flBlockCheck <= gpGlobals->time)
	{
		pmtrace_t tr;
		Vector vecMuzzle = g_pViewEnt->attachment[0]; // this is always the muzzle.
		Vector vecLastMuzzle = g_pparams.vieworg + g_pparams.forward * m_vecBlockOffset.x + g_pparams.right * m_vecBlockOffset.y + g_pparams.up * m_vecBlockOffset.z;

		// LUNA: After moving ItemPostFrame() of client.dll into HUD_Frame(), occurationally g_pViewEnt->attachment[] snap to NaN.
		if (vecMuzzle.IsNaN())
			vecMuzzle = g_rgvecViewModelAttachments[0];	// Using backup database.

		// Why we have to use offsets?
		// If you directly Trace from player eyes to muzzle, once the BLOCK_UP anim is played, the muzzle origin will change due to this new animation.
		// Hence, the BLOCKED flag will imminently be removed because of the BLOCK_UP anim. Which leads to BLOCK_DOWN to be played.
		// It would become a "twitch" on screen.
		if (m_vecBlockOffset.LengthSquared() < 0.1f)
			vecLastMuzzle = vecMuzzle;	// first use, avoid bug.

		/*UTIL_TraceLine(g_pparams.vieworg, vecMuzzle, PM_STUDIO_BOX, -1, &tr, m_pPlayer->index, 2);*/

		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(g_pparams.vieworg, vecLastMuzzle, PM_STUDIO_BOX, -1, &tr);

		// stick into enemy's face?
		int iEntIndex = PM_GetPhysEntInfo(tr.ent);	// the TR result does not directly implies ENTINDEX() in edict or cl_entity. it needs a conversion.
		if (iEntIndex < gEngfuncs.GetMaxClients() && iEntIndex > 0)
			if (g_PlayerExtraInfo[iEntIndex].m_iTeam != g_iTeam)
				tr.fraction = 1.0f;	// "nothing happens here, let's shoot!"

		// the BLOCKED condition is a player attribute.
		bool save = g_bIsBlocked;
		g_bIsBlocked = !!(tr.fraction < 1);

		if (!save && g_bIsBlocked)	// a new blocked situation.
		{
			vecMuzzle -= g_pparams.vieworg;	// become a offset first.
			m_vecBlockOffset.x = DotProduct(vecMuzzle, g_pparams.forward);
			m_vecBlockOffset.y = DotProduct(vecMuzzle, g_pparams.right);
			m_vecBlockOffset.z = DotProduct(vecMuzzle, g_pparams.up);
		}

		// check interval.
		m_flBlockCheck = gpGlobals->time + 0.05f;
	}

	// handle block situation.
	if ((m_pPlayer->m_afButtonPressed | m_pPlayer->m_afButtonReleased) & IN_BLOCK)
	{
		// let the idle function handle it.
		// we cannot let the block anims interfere other normal anims.
		m_flTimeWeaponIdle = -1;
	}

	// LUNA: there are some problems regarding client prediction.
	// sometimes, the client side m_flNextPrimaryAttack and m_flNextSecondaryAttack would be wirely re-zero and induce multiple bullet hole VFX bug.
	// thus, I decide to use message instead. (gmsgShoot and gmsgSteelSight)
	// UPDATE Mar 25: I managed to fix PrimAttack. However, due to many server-exclusive entity, the steelsight still can't be predict on client side.
	// UPDATE Oct 30: Secondary attack (aim) fixed.

#ifdef CLIENT_PREDICT_AIM
	if (!(usableButtons & IN_BLOCK) && (	// you cannot aim if you are blocked.
		(!cl_holdtoaim->value && usableButtons & IN_ATTACK2 && m_flNextSecondaryAttack <= UTIL_WeaponTimeBase()) ||	// PRESS to aim
		(cl_holdtoaim->value && ((m_pPlayer->m_afButtonPressed & IN_ATTACK2 && !m_bInZoom) || (m_pPlayer->m_afButtonReleased & IN_ATTACK2 && m_bInZoom)) )	// HOLD to aim
		))	// UseDecrement()
	{
		SecondaryAttack();

		// only cancel this flag in PRESS mode.
		if (!cl_holdtoaim->value)
			m_pPlayer->pev->button &= ~IN_ATTACK2;
	}
	else
#endif
#ifdef CLIENT_PREDICT_PRIM_ATK
		if ((m_pPlayer->pev->button & IN_ATTACK) && CanAttack(m_flNextPrimaryAttack, UTIL_WeaponTimeBase(), TRUE) && !(usableButtons & IN_BLOCK))	// UseDecrement()
	{
		// Can't shoot during the freeze period
		// Neither can you if blocked.
		if (m_pPlayer->m_bCanShoot)
		{
			PrimaryAttack();
		}
	}
	else
#endif
		if ((m_pPlayer->pev->button & IN_RELOAD) && m_pItemInfo->m_iMaxClip != WEAPON_NOCLIP && !m_bInReload && m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		Reload();
	}
	else if (!(usableButtons & IN_ATTACK))	// no fire buttons down
	{
		// The following code prevents the player from tapping the firebutton repeatedly
		// to simulate full auto and retaining the single shot accuracy of single fire
		if (m_bDelayRecovery)
		{
			m_bDelayRecovery = false;

			if (m_iShotsFired > 15)
				m_iShotsFired = 15;

			m_flDecreaseShotsFired = gpGlobals->time + 0.4;
		}

		// if it's a semi-auto weapon then set the shots fired to 0 after the player releases a button
		if (IsSemiautoWeapon(m_iId))
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

	// remove the first draw flag here. why? make sure player watch it all through.
	if (m_bitsFlags & WPNSTATE_DRAW_FIRST)
		m_bitsFlags &= ~WPNSTATE_DRAW_FIRST;

	// catch all
	if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
	{
		WeaponIdle();
	}
}

template<typename CWpn>
bool CBaseWeaponTemplate<CWpn>::Reload(void)
{
	if constexpr (!IsTubularMag<CWpn>)
	{
		return DefaultMagReload();
	}

	return CBaseWeapon::Reload();
}

bool CBaseWeapon::Melee(void)
{
	// you just.. can't do this.
	if (m_bitsFlags & WPNSTATE_BUSY)
		return false;

	if (m_bInZoom)
		SecondaryAttack();

	// Save the current state of anim.
	PushAnim();

	// Reduced from BaseKnife::Deploy() and Slash()
	m_bitsFlags |= WPNSTATE_MELEE;	// mark for further process.

	m_pPlayer->pev->fov = DEFAULT_FOV;
	m_pPlayer->m_iLastZoom = DEFAULT_FOV;
	m_pPlayer->m_bResumeZoom = false;

	Q_strlcpy(m_pPlayer->m_szAnimExtention, "knife");
	g_pViewEnt->model = gEngfuncs.CL_LoadModel("models/weapons/v_knife.mdl", &m_pPlayer->pev->viewmodel);

	SendWeaponAnim(KNIFE_QUICK_SLASH, false);
	m_pPlayer->m_flNextAttack = KNIFE_QUICK_SLASH_TIME;

	return true;
}

bool CBaseWeapon::QuickThrowStart(EquipmentIdType iId)
{
	if (m_bitsFlags & WPNSTATE_BUSY)
		return false;

	if (!m_pPlayer->GetGrenadeInventory(iId) && !m_pPlayer->m_rgbHasEquipment[iId])
		return false;

	if (m_bInZoom)
		SecondaryAttack();

	int iAnim = GR_IDLE;
	float flTime = TIME_GR_IDLE_LOOP;
	const char* pszViewModel = THROWABLE_VIEW_MODEL;	// constant string is not required on CL side.
	switch (iId)
	{
	case EQP_HEGRENADE:
	case EQP_CRYOGRENADE:
	case EQP_INCENDIARY_GR:
		iAnim = GR_QT_READY;
		flTime = TIME_GR_QT_READY;
		break;

	case EQP_FLASHBANG:
		iAnim = FB_QT_READY;
		flTime = TIME_FB_QT_READY;
		break;

	case EQP_SMOKEGRENADE:
	case EQP_GAS_GR:
	case EQP_HEALING_GR:
		iAnim = SG_QT_READY;
		flTime = TIME_SG_QT_READY;
		break;

	case EQP_C4:	// this one is really special.
	{
		m_bitsFlags |= WPNSTATE_QUICK_THROWING;
		m_pPlayer->m_iUsingGrenadeId = iId;
		// no 3rd personal model setting here, as below.

		Q_strlcpy(m_pPlayer->m_szAnimExtention, "grenade");
		SetLeftHand(false);	// holster L hand. The freeze time (m_flNextAttack) should be included.

		// totally different from SV.
		gSecViewModelMgr.m_bVisible = true;
		gSecViewModelMgr.SetModel(g_rgpszSharedString[SSZ_C4_VMDL]);
		gSecViewModelMgr.SetAnim(C4_DRAW);

		goto TAG_C4_SKIPPING;
	}

	case EQP_DETONATOR:
	{
		m_bitsFlags |= WPNSTATE_QUICK_THROWING | WPNSTATE_QT_RELEASE | WPNSTATE_QT_SHOULD_SPAWN;	// consider detonator is already "released" on start.
		m_pPlayer->m_iUsingGrenadeId = iId;
		// no 3rd personal model setting needed.

		SetLeftHand(false);	// holster L hand. The freeze time (m_flNextAttack) should be included.
		m_pPlayer->m_flNextAttack = C4_TIME_DETONATE;	// however, we have to overwrite it.

		// totally different from SV.
		gSecViewModelMgr.m_bVisible = true;
		gSecViewModelMgr.SetModel(g_rgpszSharedString[SSZ_C4_VMDL]);
		gSecViewModelMgr.SetAnim(C4_DETONATE);

		goto TAG_C4_SKIPPING;
	}

	case EQP_FLASHLIGHT:
	{
		// Nothing to do at client side.
		m_pPlayer->m_iUsingGrenadeId = iId;
		return true;
	}

	default:
		return false;	// how did he get here???
	}

	m_bitsFlags |= WPNSTATE_QUICK_THROWING;
	m_pPlayer->m_iUsingGrenadeId = iId;
	g_pViewEnt->model = gEngfuncs.CL_LoadModel(pszViewModel, &m_pPlayer->pev->viewmodel);

	Q_strlcpy(m_pPlayer->m_szAnimExtention, "grenade");
	SendWeaponAnim(iAnim);

	m_pPlayer->m_flNextAttack = flTime;
	m_flTimeWeaponIdle = flTime + 0.75f;
TAG_C4_SKIPPING:
	m_flDecreaseShotsFired = gpGlobals->time;
	m_bInReload = false;	// the reload has to stop this time.

	m_pPlayer->pev->fov = DEFAULT_FOV;
	m_pPlayer->m_iLastZoom = DEFAULT_FOV;
	m_pPlayer->m_bResumeZoom = false;
	m_pPlayer->m_vecVAngleShift = g_vecZero;

	return true;
}

bool CBaseWeapon::QuickThrowRelease(void)
{
	if (!(m_bitsFlags & WPNSTATE_QUICK_THROWING))
		return false;	// what are you doing here then?

	m_bitsFlags |= WPNSTATE_QT_RELEASE;
	return true;
}

bool CBaseWeapon::HolsterStart(void)
{
	// most weapon models still has no holster anim. therefore, we have nothing else todo but reset our data and leave.
	SendWeaponAnim(0);
	m_pPlayer->m_flNextAttack = 0.01f;
	m_bitsFlags |= WPNSTATE_HOLSTERING;

	return true;
}

void CBaseWeapon::Holstered(void)
{
	// if user is insist to holster weapon, we should allow that.
	// LUNA: no longer check CanHolster() before Holster().

	m_bInZoom = false;
	g_vecGunOfsGoal = g_vecZero;
	g_flGunOfsMovingSpeed = 10.0f;

	m_bInReload = false;

	m_pPlayer->pev->viewmodel = 0;
	m_pPlayer->pev->weaponmodel = 0;
	m_pPlayer->pev->fov = DEFAULT_FOV;
}

bool CBaseWeapon::Drop(void** ppWeaponBoxReturned)
{
	// the client site have no CWeaponBox.
	if (ppWeaponBoxReturned)
		(*ppWeaponBoxReturned) = nullptr;

	// It is equivlent to have this weapon removed.
	Kill();

	return true;
}

bool CBaseWeapon::Kill(void)
{
	m_pPlayer = nullptr;

	m_bitsFlags |= WPNSTATE_DEAD;	// Mark for death.
	return true;
}

void CBaseWeapon::UpdateBobParameters(void)
{
	// normally we don'touch any Omega stuff.
	g_flGunBobOmegaModifier = 1.0;

	// less shake when scoping.
	if (m_bInZoom)
		g_flGunBobAmplitudeModifier = 0.35;

	// common sense: running will cause your hands shake more.
	else if (m_bitsFlags & WPNSTATE_DASHING)
		g_flGunBobAmplitudeModifier = 10.0;

	// normal walking shake.
	else
		g_flGunBobAmplitudeModifier = 0.75;
}

bool CBaseWeapon::DefaultDeploy(const char* szViewModel, const char* szWeaponModel, int iAnim, const char* szAnimExt, float flDeployTime)
{
	// TODO
	/*if (!CanDeploy())
		return false;*/

	g_pViewEnt->model = gEngfuncs.CL_LoadModel(szViewModel, &m_pPlayer->pev->viewmodel);

	Q_strlcpy(m_pPlayer->m_szAnimExtention, szAnimExt);
	SendWeaponAnim(iAnim);

	m_pPlayer->m_flNextAttack = flDeployTime;
	m_flTimeWeaponIdle = flDeployTime + 0.75f;
	m_flDecreaseShotsFired = gpGlobals->time;
	m_bitsFlags &= ~WPNSTATE_HOLSTERING;	// remove this marker on deploy. !

	m_pPlayer->pev->fov = DEFAULT_FOV;
	m_pPlayer->m_iLastZoom = DEFAULT_FOV;
	m_pPlayer->m_bResumeZoom = false;
	m_pPlayer->m_vecVAngleShift = g_vecZero;

	return true;
}

template<typename CWpn>
int CBaseWeaponTemplate<CWpn>::DefaultShoot(void)  requires(IsShotgun<CWpn>)
{
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	return m_pPlayer->FireBuckshots(
		CWpn::PROJECTILE_COUNT,
		m_pPlayer->GetGunPosition(),
		gpGlobals->v_forward,
		CWpn::CONE_VECTOR,
		CWpn::EFFECTIVE_RANGE,
		CWpn::DAMAGE,
		CWpn::RANGE_MODIFIER,
		m_pPlayer->random_seed
	);
}

template<typename CWpn>
Vector2D CBaseWeaponTemplate<CWpn>::DefaultShoot(float flSpread, float flCycleTime)  requires(IS_MEMBER_PRESENTED_CPP20_W(SPREAD_BASELINE))
{
#pragma region Semiauto check.
	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ATTRIB_SEMIAUTO))
	{
		if (++m_iShotsFired > 1)
		{
			return Vector2D::Zero();
		}
	}
#pragma endregion

#pragma region Check input variables
	if (flSpread < 0.0f)
		flSpread = This()->GetSpread();

	if (flCycleTime < 0.0f)
	{
		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(RPM))
		{
			flCycleTime = 60.0f / CWpn::RPM;
		}
		else if constexpr (IS_MEMBER_PRESENTED_CPP20_W(FIRE_INTERVAL))
		{
			flCycleTime = CWpn::FIRE_INTERVAL;
		}
		else
		{
			COMPILING_ERROR("One of two fire interval attrib must be provided: \"RPM\" or \"FIRE_INTERVAL\".");
		}
	}
#pragma endregion

#pragma region Underwater check.
	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ATTRIB_NO_FIRE_UNDERWATER))
	{
		if (m_pPlayer->pev->waterlevel == 3)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15f;
			return Vector2D::Zero();
		}
	}
#pragma endregion

#pragma region Magazine check.
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

		return Vector2D::Zero();
	}
#pragma endregion

	m_iClip--;

#pragma region Server side visual effects
	This()->ApplyServerFiringVisual();
#pragma endregion

#pragma region Fire bullets.
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	auto vSpread = m_pPlayer->FireBullets3(
		m_pPlayer->GetGunPosition(),
		gpGlobals->v_forward,
		flSpread,
		CWpn::EFFECTIVE_RANGE,
		CWpn::PENETRATION,
		m_iPrimaryAmmoType,
		CWpn::DAMAGE,
		CWpn::RANGE_MODIFER,
		m_pPlayer->random_seed
	);
#pragma endregion

#pragma region Notify all clients that this weapon has fired.
	This()->PlaybackEvent(vSpread);
#pragma endregion

#pragma region Client visual effects
	// If this code is running at client side, by default we have to call first personal VFX.
	// The third personal VFX can only be refer in the events.
	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ApplyClientFPFiringVisual))
	{
		This()->ApplyClientFPFiringVisual(vSpread);	// Allow an override of CWpn's version.
	}
#pragma endregion

#pragma region Apply time defer to next attacks.
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(FIRE_ANIMTIME))
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + CWpn::FIRE_ANIMTIME;
	}
	else
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flCycleTime + 2.0f;
	}
#pragma endregion

#pragma region Apply recoils.
	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ApplyRecoil))
	{
		This()->ApplyRecoil();
	}
#pragma endregion

	return vSpread;
}

void CBaseWeapon::DefaultIdle(int iDashingAnim, int iIdleAnim, float flDashLoop)
{
#ifdef CLIENT_DLL
	UpdateBobParameters();
#endif
	// the priority of these anims:
	// 1. Running first. You can't be BLOCKED during a RUN.
	// 2. Block. You can't aimming if you are blocked.
	// 3. Aim.

	if (m_bitsFlags & WPNSTATE_DASHING)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flDashLoop;
		SendWeaponAnim(iDashingAnim);
	}
	else if ((m_pPlayer->m_afButtonPressed | m_pPlayer->m_afButtonReleased) & IN_BLOCK
		|| (m_pPlayer->pev->button & IN_BLOCK && m_pPlayer->pev->weaponanim == iIdleAnim))
	{
		// you can't aim during a BLOCK section.
		if (m_bInZoom || m_pPlayer->pev->fov != DEFAULT_FOV)
			SecondaryAttack();

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
		PlayBlockAnim();
	}
	else if (m_pPlayer->pev->weaponanim != iIdleAnim)
	{
		SendWeaponAnim(iIdleAnim);
	}
}

bool CBaseWeapon::DefaultReload(int iClipSize, int iAnim, float flTotalDelay, float flSoftDelay)
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return false;

	if (m_iClip >= iClipSize)
		return false;

	// exit scope
	if (m_bInZoom || int(m_pPlayer->pev->fov) != DEFAULT_FOV)
		SecondaryAttack();	// close scope when we reload.

	// 3rd personal anim & SFX
	m_pPlayer->SetAnimation(PLAYER_RELOAD);
	ReloadSound();

	// reset accuracy data
	m_iShotsFired = 0;

	// pause weapon actions
	m_pPlayer->m_flNextAttack = flTotalDelay - flSoftDelay;
	m_flTimeWeaponIdle = flTotalDelay;
	m_flNextPrimaryAttack = flTotalDelay;
	m_flNextSecondaryAttack = flTotalDelay - flSoftDelay;
	m_bInReload = true;

	// 1st personal anim
	SendWeaponAnim(iAnim);

	// it's currently useless.. but let's do it anyway.
	if (!m_iClip)
		m_bitsFlags |= WPNSTATE_RELOAD_EMPTY;

	return true;
}

template<typename CWpn>
bool CBaseWeaponTemplate<CWpn>::DefaultMagReload(void) requires(!IsTubularMag<CWpn>)
{
	bool bReloadEntered = false;
	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(RELOAD_SOFT_DELAY_TIME) && IS_MEMBER_PRESENTED_CPP20_W(RELOAD_EMPTY_SOFT_DELAY_TIME))
	{
		bReloadEntered = DefaultReload(
			WInfo()->m_iMaxClip,
			m_iClip ? CWpn::RELOAD : CWpn::RELOAD_EMPTY,
			m_iClip ? CWpn::RELOAD_TIME : CWpn::RELOAD_EMPTY_TIME,
			m_iClip ? CWpn::RELOAD_SOFT_DELAY_TIME : CWpn::RELOAD_EMPTY_SOFT_DELAY_TIME
		);
	}
	else if constexpr (IS_MEMBER_PRESENTED_CPP20_W(RELOAD_SOFT_DELAY_TIME))	// Only one of them presented.
	{
		bReloadEntered = DefaultReload(
			WInfo()->m_iMaxClip,
			m_iClip ? CWpn::RELOAD : CWpn::RELOAD_EMPTY,
			m_iClip ? CWpn::RELOAD_TIME : CWpn::RELOAD_EMPTY_TIME,
			CWpn::RELOAD_SOFT_DELAY_TIME
		);
	}
	else
	{
		bReloadEntered = DefaultReload(
			WInfo()->m_iMaxClip,
			m_iClip ? CWpn::RELOAD : CWpn::RELOAD_EMPTY,
			m_iClip ? CWpn::RELOAD_TIME : CWpn::RELOAD_EMPTY_TIME
		);
	}

	if (bReloadEntered)
	{
		if constexpr (IS_MEMBER_PRESENTED_CPP20_W(ACCURACY_BASELINE))
			m_flAccuracy = CWpn::ACCURACY_BASELINE;

		return true;
	}

	// KF2 style inspection when you press R.
	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(INSPECTION))
	{
		if (m_pPlayer->pev->weaponanim != CWpn::INSPECTION)
		{
			if (m_bInZoom)
				SecondaryAttack();

			SendWeaponAnim(CWpn::INSPECTION);
			m_flTimeWeaponIdle = CWpn::INSPECTION_TIME;
		}
	}

	return false;
}

bool CBaseWeapon::DefaultHolster(int iHolsterAnim, float flHolsterDelay)
{
	// no m_flEjectBrass re-zero for player on client side.

	SendWeaponAnim(iHolsterAnim);
	m_pPlayer->m_flNextAttack = flHolsterDelay;
	m_bitsFlags |= WPNSTATE_HOLSTERING;

	return true;
}

void CBaseWeapon::DefaultSteelSight(const Vector& vecOfs, int iFOV, float flDriftingSpeed, float flNextSecondaryAttack)
{
	m_bInZoom = !m_bInZoom;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flNextSecondaryAttack;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + flNextSecondaryAttack;	// set/reset the gun bob amp.

#ifdef CLIENT_DLL
	// due to some logic problem, we actually cannot use m_bInZoom here.
	// it would be override.

	if (!g_vecGunOfsGoal.LengthSquared())
	{
		g_vecGunOfsGoal = vecOfs;
		gHUD::m_iFOV = iFOV;	// allow clients to predict the zoom.
	}
	else
	{
		g_vecGunOfsGoal = g_vecZero;
		gHUD::m_iFOV = 90;
	}

	// this model needs faster.
	g_flGunOfsMovingSpeed = flDriftingSpeed;
#else
	// just zoom a liiiiittle bit.
	// this doesn't suffer from the same bug where the gunofs does, since the FOV was actually sent from SV.
	if (m_bInZoom)
	{
		m_pPlayer->pev->fov = iFOV;
		EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, "weapons/steelsight_in.wav", 0.75f, ATTN_STATIC);
	}
	else
	{
		m_pPlayer->pev->fov = 90;
		EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, "weapons/steelsight_out.wav", 0.75f, ATTN_STATIC);
	}
#endif
}

void CBaseWeapon::DefaultScopeSight(const Vector& vecOfs, int iFOV, float flEnterScopeDelay, float flFadeFromBlack, float flDriftingSpeed, float flNextSecondaryAttack)
{
	// this is the delay for the m_bResumeZoom.
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + flEnterScopeDelay;

	if (int(m_pPlayer->pev->fov) < 90)
	{
		m_pPlayer->pev->fov = 90;

#ifdef CLIENT_DLL
		// zoom out anim.
		g_vecGunOfsGoal = g_vecZero;

		// manually set fade.
		gHUD::m_SniperScope.SetFadeFromBlack(flFadeFromBlack, 0);
#endif
	}
	else
	{
		// get ready to zoom in.
		m_pPlayer->m_iLastZoom = iFOV;
		m_pPlayer->m_bResumeZoom = true;

#ifdef CLIENT_DLL
		// zoom in anim.
		g_vecGunOfsGoal = vecOfs;
#endif
	}

#ifndef CLIENT_DLL
	if (TheBots)
	{
		TheBots->OnEvent(EVENT_WEAPON_ZOOMED, m_pPlayer);
	}

	// SFX only emitted from SV.
	EMIT_SOUND(m_pPlayer->edict(), CHAN_ITEM, "weapons/zoom.wav", 0.2, 2.4);
#else
	g_flGunOfsMovingSpeed = flDriftingSpeed;
#endif

	// slow down while we zooming.
	m_pPlayer->ResetMaxSpeed();

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flNextSecondaryAttack;
}

void CBaseWeapon::DefaultDashStart(int iEnterAnim, float flEnterTime)
{
	if (m_bInReload)
		m_bInReload = false;

	if (m_bInZoom || m_pPlayer->pev->fov < DEFAULT_FOV)
	{
#ifndef CLIENT_DLL
		SecondaryAttack();
#else
		g_vecGunOfsGoal = g_vecZero;
		g_flGunOfsMovingSpeed = 10.0f;
		gHUD::m_iFOV = 90;
#endif
	}

	SendWeaponAnim(iEnterAnim);
	m_pPlayer->m_flNextAttack = flEnterTime;
	m_flTimeWeaponIdle = flEnterTime;
	m_bitsFlags |= WPNSTATE_DASHING;
}

void CBaseWeapon::DefaultDashEnd(int iEnterAnim, float flEnterTime, int iExitAnim, float flExitTime)
{
	if (m_pPlayer->m_flNextAttack > 0.0f && m_pPlayer->pev->weaponanim == iEnterAnim)
	{
		// this is how much you procees to the dashing phase.
		// for example, assuming the whole length is 1.0s, you start 0.7s and decide to cancel.
		// although there's only 0.3s to the dashing phase, but turning back still requires another equally 0.7s.
		// "m_pPlayer->m_flNextAttack" is the 0.3s of full length. you need to get the rest part, i.e. the 70%.
		float flRunStartUnplayedRatio = 1.0f - m_pPlayer->m_flNextAttack / flEnterTime;

		// stick on the last instance in the comment: 70% * 1.0s(full length) = 0.7s, this is the time we need to turning back.
		float flRunStopTimeLeft = flExitTime * flRunStartUnplayedRatio;

		// play the anim.
		SendWeaponAnim(iExitAnim);

#ifdef CLIENT_DLL
		// why we are using the "0.3s" here?
		// this is because the g_flTimeViewModelAnimStart actually means how much time had passed since the anim was ordered to play.
		// if we need to play 0.7s, we have to told system we only played it for 0.3s. right?
		g_flTimeViewModelAnimStart = gEngfuncs.GetClientTime() - (flExitTime - flRunStopTimeLeft);
#endif

		// force everything else to wait.
		m_pPlayer->m_flNextAttack = m_flNextPrimaryAttack = m_flNextSecondaryAttack = m_flTimeWeaponIdle = flRunStopTimeLeft;
	}

	// if RUN_START is normally played and finished, go normal.
	else
	{
		SendWeaponAnim(iExitAnim);
		m_pPlayer->m_flNextAttack = flExitTime;
		m_flTimeWeaponIdle = flExitTime;
	}

	// either way, we have to remove this flag.
	m_bitsFlags &= ~WPNSTATE_DASHING;

#ifdef CLIENT_DLL
	// remove the exaggerating shaking vfx.
	g_flGunBobAmplitudeModifier = 1.0f;
#endif
}

bool CBaseWeapon::DefaultSetLHand(bool bAppear, int iLHandUpAnim, float flLHandUpTime, int iLHandDownAnim, float flLHandDownTime)
{
	if (bAppear && m_bitsFlags & WPNSTATE_NO_LHAND)
	{
		SendWeaponAnim(iLHandUpAnim);
		m_pPlayer->m_flNextAttack = flLHandUpTime;
		m_flTimeWeaponIdle = flLHandUpTime;
		m_bitsFlags &= ~WPNSTATE_NO_LHAND;

		return true;
	}
	else if (!(m_bitsFlags & WPNSTATE_NO_LHAND))
	{
		SendWeaponAnim(iLHandDownAnim);
		m_pPlayer->m_flNextAttack = flLHandDownTime;
		m_flTimeWeaponIdle = flLHandDownTime;
		m_bitsFlags |= WPNSTATE_NO_LHAND;

		return true;
	}

	return false;
}

void CBaseWeapon::DefaultBlock(int iEnterAnim, float flEnterTime, int iExitAnim, float flExitTime)
{
	// we are using "play anim mid-way" method.
	// go check CBaseWeapon::DefaultDashEnd() for detailed commentary.
	bool bBlocked = !!(m_pPlayer->pev->button & IN_BLOCK);

	if (bBlocked && m_pPlayer->pev->weaponanim != iEnterAnim)
	{
		// time to play iEnterAnim.
		if (m_pPlayer->pev->weaponanim == iExitAnim)
		{
			float flAlreadyPlayed = gEngfuncs.GetClientTime() - g_flTimeViewModelAnimStart;

			SendWeaponAnim(iEnterAnim);

			if (flAlreadyPlayed >= flExitTime)
				return;

			float flAlreadyPlayedRatio = flAlreadyPlayed / flExitTime;

			g_flTimeViewModelAnimStart -= flEnterTime * (1.0f - flAlreadyPlayedRatio);
		}
		else
		{
			SendWeaponAnim(iEnterAnim);
		}
	}

	// therefore, you should not keep calling it in Think() or Frame().
	else if (!bBlocked && m_pPlayer->pev->weaponanim != iExitAnim)
	{
		if (m_pPlayer->pev->weaponanim == iEnterAnim)
		{
			float flAlreadyPlayed = gEngfuncs.GetClientTime() - g_flTimeViewModelAnimStart;

			SendWeaponAnim(iExitAnim);

			if (flAlreadyPlayed >= flEnterTime)
				return;

			float flAlreadyPlayedRatio = flAlreadyPlayed / flEnterTime;

			g_flTimeViewModelAnimStart -= flExitTime * (1.0f - flAlreadyPlayedRatio);
		}
		else
		{
			SendWeaponAnim(iExitAnim);
		}
	}
}

float CBaseWeapon::DefaultSpread(float flBaseline, float flAimingMul, float flDuckingMul, float flWalkingMul, float flJumpingMul)
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
		flBaseline *= flJumpingMul;

	if (m_pPlayer->pev->velocity.Length2D() > 0)	// z speed does not included.
		flBaseline *= flWalkingMul;

	if (m_pPlayer->pev->flags & FL_DUCKING)
		flBaseline *= flDuckingMul;

	if (m_bInZoom || m_pPlayer->pev->fov < DEFAULT_FOV)
		flBaseline *= flAimingMul;

	return flBaseline;	// it's already be modified.
}

template<typename CWpn>
WeaponIdType CBaseWeaponTemplate<CWpn>::Id(void)
{
	// Pistols
	if constexpr (std::is_same_v<CWpn, CG18C>)
	{
		return WEAPON_GLOCK18;
	}
	else if constexpr (std::is_same_v<CWpn, CUSP>)
	{
		return WEAPON_USP;
	}
	else if constexpr (std::is_same_v<CWpn, CAnaconda>)
	{
		return WEAPON_ANACONDA;
	}
	else if constexpr (std::is_same_v<CWpn, CDEagle>)
	{
		return WEAPON_DEAGLE;
	}
	else if constexpr (std::is_same_v<CWpn, CFN57>)
	{
		return WEAPON_FIVESEVEN;
	}
	else if constexpr (std::is_same_v<CWpn, CM45A1>)
	{
		return WEAPON_M45A1;
	}

	// Shotguns
	else if constexpr (std::is_same_v<CWpn, CKSG12>)
	{
		return WEAPON_KSG12;
	}
	else if constexpr (std::is_same_v<CWpn, CM1014>)
	{
		return WEAPON_M1014;
	}
	//else if constexpr (std::is_same_v<CWpn, CAA12>)
	//{
	//	return WEAPON_AA12;
	//}

	// SMGs
	else if constexpr (std::is_same_v<CWpn, CMP7A1>)
	{
		return WEAPON_MP7A1;
	}
	//else if constexpr (std::is_same_v<CWpn, CMAC10>)
	//{
	//	return WEAPON_MAC10;
	//}
	//else if constexpr (std::is_same_v<CWpn, CMP5N>)
	//{
	//	return WEAPON_MP5N;
	//}
	else if constexpr (std::is_same_v<CWpn, CUMP45>)
	{
		return WEAPON_UMP45;
	}
	//else if constexpr (std::is_same_v<CWpn, CP90>)
	//{
	//	return WEAPON_P90;
	//}
	//else if constexpr (std::is_same_v<CWpn, CKrissVector>)
	//{
	//	return WEAPON_VECTOR;
	//}

	// Assault Rifles
	else if constexpr (std::is_same_v<CWpn, CAK47>)
	{
		return WEAPON_AK47;
	}
	else if constexpr (std::is_same_v<CWpn, CM4A1>)
	{
		return WEAPON_M4A1;
	}
	else if constexpr (std::is_same_v<CWpn, CSCARH>)
	{
		return WEAPON_SCARH;
	}
	else if constexpr (std::is_same_v<CWpn, CXM8>)
	{
		return WEAPON_XM8;
	}

	// Sniper Rifles
	//else if constexpr (std::is_same_v<CWpn, CSRS>)
	//{
	//	return WEAPON_SRS;
	//}
	else if constexpr (std::is_same_v<CWpn, CSVD>)
	{
		return WEAPON_SVD;
	}
	else if constexpr (std::is_same_v<CWpn, CAWP>)
	{
		return WEAPON_AWP;
	}
	else if constexpr (std::is_same_v<CWpn, CPSG1>)
	{
		return WEAPON_PSG1;
	}

	// LMGs
	else if constexpr (std::is_same_v<CWpn, CMK46>)
	{
		return WEAPON_MK46;
	}
	//else if constexpr (std::is_same_v<CWpn, CRPD>)
	//{
	//	return WEAPON_RPD;
	//}

	else
	{
		COMPILING_ERROR("Unregistered weapon class presented!");
	}
}

template<class CWpn>
float CBaseWeaponTemplate<CWpn>::GetMaxSpeed(void)
{
	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(MAX_SPEED_ZOOM))
	{
		// Slower speed when zoomed in.
		if (std::roundf(m_pPlayer->pev->fov) < DEFAULT_FOV)
			return CWpn::MAX_SPEED_ZOOM;
	}

	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(MAX_SPEED))
	{
		return CWpn::MAX_SPEED;
	}
	else
	{
		return CBaseWeapon::GetMaxSpeed();
	}
}

bool CBaseWeapon::AddPrimaryAmmo(int iCount)
{
	return false;
}

void CBaseWeapon::SendWeaponAnim(int iAnim, bool bSkipLocal)
{
	g_iCurViewModelAnim = iAnim;

	m_pPlayer->pev->weaponanim = iAnim;
	gEngfuncs.pfnWeaponAnim(iAnim, CalcBodyParam());

	// save the time for the renderer.
	g_flTimeViewModelAnimStart = gEngfuncs.GetClientTime();
}

void CBaseWeapon::PlayEmptySound()
{
	// this should be played by Server.
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
	gEngfuncs.pfnWeaponAnim(m_Stack.m_iSequence, CalcBodyParam());

	// clear old data, mark for invalid.
	Q_memset(&m_Stack, NULL, sizeof(m_Stack));
	m_Stack.m_iSequence = -1;
}

bool CBaseWeapon::CanHolster(void)
{
	if (m_pPlayer->m_flNextAttack <= 0.0f && m_bitsFlags & WPNSTATE_HOLSTERING)	// the holster is completed.
		return true;

	if (m_bitsFlags & WPNSTATE_BUSY)
		return false;

	return true;
}

void CBaseWeapon::KickBack(float up_base, float lateral_base, float up_modifier, float lateral_modifier, float up_max, float lateral_max, int direction_change)
{
	real_t flKickUp;
	float flKickLateral;

	if (m_iShotsFired == 1)
	{
		flKickUp = up_base;
		flKickLateral = lateral_base;
	}
	else
	{
		flKickUp = float(m_iShotsFired) * up_modifier + up_base;
		flKickLateral = float(m_iShotsFired) * lateral_modifier + lateral_base;
	}

	m_pPlayer->m_vecVAngleShift.x -= flKickUp;

	if (m_pPlayer->m_vecVAngleShift.x < -up_max)
	{
		m_pPlayer->m_vecVAngleShift.x = -up_max;
	}

	if (m_bDirection)
	{
		m_pPlayer->m_vecVAngleShift.y += flKickLateral;

		if (m_pPlayer->m_vecVAngleShift.y > lateral_max)
			m_pPlayer->m_vecVAngleShift.y = lateral_max;
	}
	else
	{
		m_pPlayer->m_vecVAngleShift.y -= flKickLateral;

		if (m_pPlayer->m_vecVAngleShift.y < -lateral_max)
			m_pPlayer->m_vecVAngleShift.y = -lateral_max;
	}

	if (!UTIL_SharedRandomLong(m_pPlayer->random_seed, 0, direction_change))
	{
		m_bDirection = !m_bDirection;
	}

	if (m_bInZoom)
		m_pPlayer->m_vecVAngleShift *= 0.5f;
}

template<class CWpn>
void CBaseWeaponTemplate<CWpn>::ResetModel(void)
{
#ifndef CLIENT_DLL
	m_pPlayer->pev->viewmodel = MAKE_STRING(CWpn::VIEW_MODEL);
	m_pPlayer->pev->weaponmodel = MAKE_STRING(CWpn::WORLD_MODEL);
#else
	g_pViewEnt->model = gEngfuncs.CL_LoadModel(CWpn::VIEW_MODEL, &m_pPlayer->pev->viewmodel);
#endif
}

//template<typename CWpn>
//inline void CBaseWeaponTemplate<CWpn>::RegisterEvent(void) requires(HasEvent<CWpn>)
//{
//#ifdef CLIENT_DLL
//	gEngfuncs.pfnHookEvent(CWpn::EVENT_FILE, CWpn::ApplyClientTPFiringVisual);
//#endif
//}

template<typename CWpn>
void CBaseWeaponTemplate<CWpn>::ApplyServerFiringVisual(void)
{
	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(GUN_FLASH))
	{
		m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
		m_pPlayer->m_iWeaponFlash = CWpn::GUN_FLASH;
	}

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	if constexpr (IS_MEMBER_PRESENTED_CPP20_W(GUN_VOLUME))
	{
		m_pPlayer->m_iWeaponVolume = CWpn::GUN_VOLUME;
	}
	else
	{
		m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	}
}

template<typename CWpn>
void CBaseWeaponTemplate<CWpn>::PlaybackEvent(const Vector2D& vSpread)
{
	// Nothing to do here.
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

	for (auto& p : CBaseWeapon::m_lstWeapons)
		p->Kill();

	CBaseWeapon::m_lstWeapons.clear();
}

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
	using hr_clock = std::chrono::high_resolution_clock;

	CBaseWeapon::TheWeaponsThink();	// Call it no matter what.

	if (!from || !to || !cmd || !g_bInGameWorld)
		return;

//	int i;
	int buttonsChanged;
	static int lasthealth;
	int flags;
//	static auto last_hr_clock = hr_clock::now();	// init only.

	// initiation
	HUD_InitClientWeapons();

	// Get current clock
	gpGlobals->time = g_flClientTime;
	gpGlobals->frametime = g_flClientTimeDelta;

	//auto dur = hr_clock::now() - last_hr_clock;
	//last_hr_clock = hr_clock::now();
	//gpGlobals->time = std::chrono::time_point_cast<std::chrono::microseconds>(last_hr_clock).count() / 1000000.0;
	//gpGlobals->frametime = std::chrono::duration_cast<std::chrono::microseconds>(dur).count() / 1000000.0;	// Method offered by Crsky.

	// Fill in data based on selected weapon
//	if (from->client.m_iId > WEAPON_NONE && from->client.m_iId < LAST_WEAPON)
//		g_pCurWeapon = g_rgpClientWeapons[from->client.m_iId];
	g_pCurWeapon = gPseudoPlayer.m_pActiveItem;

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
			g_pCurWeapon->Holstered();	// player is killed. holster your weapon data-ly is enough.
		else if (to->client.health > 0 && lasthealth <= 0)
			g_pCurWeapon->Deploy();	// player is spawned

		lasthealth = to->client.health;
	}

	//for (i = 0; i < LAST_WEAPON; i++)
	//{
	//	if (!g_rgpClientWeapons[i] || g_rgpClientWeapons[i]->m_iId != i)	// something wrong with this pointer..?
	//		continue;

	//	auto pCurrent = g_rgpClientWeapons[i];
	//	weapon_data_t* pfrom = from->weapondata + i;

	//	pCurrent->m_bInReload = pfrom->m_fInReload;
	//	pCurrent->m_bInZoom = pfrom->m_fInSpecialReload;	// not directly used.
	//	pCurrent->m_iClip = pfrom->m_iClip;
	//	pCurrent->m_flNextPrimaryAttack = pfrom->m_flNextPrimaryAttack;
	//	pCurrent->m_flNextSecondaryAttack = pfrom->m_flNextSecondaryAttack;
	//	pCurrent->m_flTimeWeaponIdle = pfrom->m_flTimeWeaponIdle;
	//	//pCurrent->m_flStartThrow = time_point_t (duration_t (pfrom->fuser2));
	//	//pCurrent->m_flReleaseThrow = time_point_t (duration_t (pfrom->fuser3));
	//	//pCurrent->m_iSwing = pfrom->iuser1;
	//	pCurrent->m_bitsFlags = pfrom->m_iWeaponState;
	//	pCurrent->m_flLastFire = pfrom->m_fAimedDamage;
	//	pCurrent->m_iShotsFired = pfrom->m_fInZoom;	// not directly used.
	//}

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
	gPseudoPlayer.m_afButtonLast = gPseudoPlayer.pev->button;

	// Refresh button.
	gPseudoPlayer.pev->button = CL_ButtonBits();

	// Which buttsons chave changed
	buttonsChanged = (gPseudoPlayer.m_afButtonLast ^ gPseudoPlayer.pev->button);	// These buttons have changed this frame

	// Debounced button codes for pressed/released
	// The changed ones still down are "pressed"
	gPseudoPlayer.m_afButtonPressed = buttonsChanged & gPseudoPlayer.pev->button;
	// The ones not down are "released"
	gPseudoPlayer.m_afButtonReleased = buttonsChanged & (~gPseudoPlayer.pev->button);

	// Set player variables that weapons code might check/alter

	gPseudoPlayer.pev->deadflag = from->client.deadflag;
	gPseudoPlayer.pev->waterlevel = from->client.waterlevel;
	gPseudoPlayer.pev->maxspeed = STATE->client.maxspeed; //!!! Taking "to"
	gPseudoPlayer.pev->punchangle = STATE->client.punchangle; //!!! Taking "to"
	gPseudoPlayer.pev->fov = gHUD::m_iFOV;	// 11072020 LUNA: from->client.fov is broken, keeping give me 0.
//	gPseudoPlayer.pev->weaponanim = from->client.weaponanim;
//	gPseudoPlayer.pev->viewmodel = from->client.viewmodel;
//	gPseudoPlayer.m_flNextAttack = from->client.m_flNextAttack;
	gPseudoPlayer.m_iRoleType = g_iRoleType;	// synchronize local player role.

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
	g_bHoldingKnife = !!(g_pCurWeapon->m_bitsFlags & WPNSTATE_MELEE);
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
	// g_iSelectedWeapon was assign to cmd->weaponselect in input.cpp
	// this is a weapon switching prediction.
	//if (cmd->weaponselect && (gPseudoPlayer.pev->deadflag != (DEAD_DISCARDBODY + 1)))
	//{
	//	// Switched to a different weapon?
	//	if (from->weapondata[cmd->weaponselect].m_iId == cmd->weaponselect)
	//	{
	//		CBaseWeapon* pNew = g_rgpClientWeapons[cmd->weaponselect];
	//		if (pNew && (pNew != g_pCurWeapon))
	//		{
	//			// Put away old weapon
	//			if (gPseudoPlayer.m_pActiveItem)
	//				gPseudoPlayer.m_pActiveItem->Holstered();

	//			gPseudoPlayer.m_pLastItem = gPseudoPlayer.m_pActiveItem;
	//			gPseudoPlayer.m_pActiveItem = pNew;

	//			// Deploy new weapon
	//			if (gPseudoPlayer.m_pActiveItem)
	//			{
	//				gPseudoPlayer.m_pActiveItem->Deploy();
	//			}

	//			// Update weapon id so we can predict things correctly.
	//			to->client.m_iId = cmd->weaponselect;
	//		}
	//	}
	//}

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
//	if (g_runfuncs && (g_iCurViewModelAnim != to->client.weaponanim))
		// Force a fixed anim down to viewmodel. LUNA: FIXME: wired, how did that happens? it seems impossible.
//		g_pCurWeapon->SendWeaponAnim(to->client.weaponanim);

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

	//for (i = 0; i < LAST_WEAPON; i++)
	//{
	//	CBaseWeapon* pCurrent = g_rgpClientWeapons[i];

	//	weapon_data_t* pto = to->weapondata + i;

	//	if (!pCurrent)
	//	{
	//		Q_memset(pto, NULL, sizeof(weapon_data_t));
	//		continue;
	//	}

	//	// Decrement weapon counters, server does this at same time ( during post think, after doing everything else )
	//	// LUNA: is this gpGlobals->framerate ???
	//	pCurrent->m_flNextPrimaryAttack -= gpGlobals->frametime;	// LUNA: NEVER use cmd->msec / 1000.0f
	//	pCurrent->m_flNextSecondaryAttack -= gpGlobals->frametime;
	//	pCurrent->m_flTimeWeaponIdle -= gpGlobals->frametime;

	//	if (pCurrent->m_flNextPrimaryAttack < -1.0)
	//		pCurrent->m_flNextPrimaryAttack = -1.0;

	//	if (pCurrent->m_flNextSecondaryAttack < -0.001)
	//		pCurrent->m_flNextSecondaryAttack = -0.001;

	//	if (pCurrent->m_flTimeWeaponIdle < -0.001)
	//		pCurrent->m_flTimeWeaponIdle = -0.001;

	//	pto->m_iClip = pCurrent->m_iClip;

	//	pto->m_flNextPrimaryAttack = pCurrent->m_flNextPrimaryAttack;
	//	pto->m_flNextSecondaryAttack = pCurrent->m_flNextSecondaryAttack;
	//	pto->m_flTimeWeaponIdle = pCurrent->m_flTimeWeaponIdle;

	//	pto->m_fInReload = pCurrent->m_bInReload;
	//	pto->m_fInSpecialReload = pCurrent->m_bInZoom;
	//	//pto->m_flNextReload = pCurrent->m_flNextReload / 1s;
	//	//pto->fuser2 = pCurrent->m_flStartThrow.time_since_epoch() / 1s;
	//	//pto->fuser3 = pCurrent->m_flReleaseThrow.time_since_epoch() / 1s;
	//	//pto->iuser1 = pCurrent->m_iSwing;
	//	pto->m_iWeaponState = pCurrent->m_bitsFlags;
	//	pto->m_fInZoom = pCurrent->m_iShotsFired;
	//	pto->m_fAimedDamage = pCurrent->m_flLastFire;
	//}

	for (auto& pCurrent : CBaseWeapon::m_lstWeapons)
	{
		pCurrent->m_flNextPrimaryAttack -= gpGlobals->frametime;	// LUNA: NEVER use cmd->msec / 1000.0f
		pCurrent->m_flNextSecondaryAttack -= gpGlobals->frametime;
		pCurrent->m_flTimeWeaponIdle -= gpGlobals->frametime;

		if (pCurrent->m_flNextPrimaryAttack < -1.0)
			pCurrent->m_flNextPrimaryAttack = -1.0;

		if (pCurrent->m_flNextSecondaryAttack < -0.001)
			pCurrent->m_flNextSecondaryAttack = -0.001;

		if (pCurrent->m_flTimeWeaponIdle < -0.001)
			pCurrent->m_flTimeWeaponIdle = -0.001;
	}

#ifdef CHECKING_NEXT_PRIM_ATTACK_SYNC
	if (g_pCurWeapon && g_pCurWeapon->m_flNextPrimaryAttack > 0.0f)
		gEngfuncs.pfnConsolePrint(SharedVarArgs("[Client] m_flNextPrimaryAttack: %f\n", g_pCurWeapon->m_flNextPrimaryAttack));
#endif

	// m_flNextAttack is now part of the weapons, but is part of the player instead
//	to->client.m_flNextAttack -= gpGlobals->frametime;
//	if (to->client.m_flNextAttack < -0.001)
//	{
//		to->client.m_flNextAttack = -0.001;
//	}

	gPseudoPlayer.m_flNextAttack -= gpGlobals->frametime;
	if (gPseudoPlayer.m_flNextAttack < -0.001)
		gPseudoPlayer.m_flNextAttack = -0.001;
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
	// LUNA: Credits to Crsky.
	// If you run the client.dll standalone, the cmd->msec is UTTERLY wrong.
	// You will have to calculate it manually.
	// On server side, manually calculate the change of gpGlobals->time.
	// On client side, manually calculate the change of "time" parameter from ExportFunc::HUD_PostRunCmd().

	//Q_memcpy(from, &g_sWpnFrom, sizeof(local_state_t));
	//Q_memcpy(to, &g_sWpnTo, sizeof(local_state_t));
	//Q_memcpy(cmd, &g_sWpnCmd, sizeof(usercmd_s));

	g_sWpnFrom = *from;
	g_sWpnTo = *to;
	g_sWpnCmd = *cmd;
	g_runfuncs = runfuncs;

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
CommandFunc_AlterAct
=====================
*/
void CommandFunc_AlterAct(void)
{
	// only pass to server when it is allowed in client.
	if (g_pCurWeapon && g_pCurWeapon->AlterAct())
		gEngfuncs.pfnServerCmd("changemode\n");
}

/*
=====================
Wpn_Init
=====================
*/
void Wpn_Init()
{
	cl_holdtoaim = gEngfuncs.pfnRegisterVariable("cl_holdtoaim", "0", FCVAR_ARCHIVE | FCVAR_CLIENTDLL | FCVAR_USERINFO);

	gEngfuncs.pfnAddCommand("melee", CommandFunc_Melee);
	gEngfuncs.pfnAddCommand("changemode", CommandFunc_AlterAct);
}
