#include "precompiled.h"

short g_sModelIndexLaser;		// holds the index for the laser beam
short g_sModelIndexLaserDot;	// holds the index for the laser beam dot
short g_sModelIndexFireball;	// holds the index for the fireball
short g_sModelIndexSmoke;		// holds the index for the smoke cloud
short g_sModelIndexWExplosion;	// holds the index for the underwater explosion
short g_sModelIndexBubbles;		// holds the index for the bubbles model
short g_sModelIndexBloodDrop;	// holds the sprite index for the initial blood
short g_sModelIndexBloodSpray;	// holds the sprite index for splattered blood
short g_sModelIndexSmokePuff;
short g_sModelIndexFireball2;
short g_sModelIndexFireball3;
short g_sModelIndexFireball4;
short g_sModelIndexRadio;

short int g_sModelIndexCTGhost;
short int g_sModelIndexTGhost;
short int g_sModelIndexC4Glow;

MULTIDAMAGE gMultiDamage;


// Resets the global multi damage accumulator
void ClearMultiDamage()
{
	gMultiDamage.pEntity = nullptr;
	gMultiDamage.amount = 0;
	gMultiDamage.type = 0;
}

// Inflicts contents of global multi damage register on gMultiDamage.pEntity
void ApplyMultiDamage(entvars_t *pevInflictor, entvars_t *pevAttacker)
{
	if (!gMultiDamage.pEntity)
		return;

	gMultiDamage.pEntity->TakeDamage(pevInflictor, pevAttacker, gMultiDamage.amount, gMultiDamage.type);

}

void AddMultiDamage(entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType)
{
	if (!pEntity)
		return;

	gMultiDamage.type |= bitsDamageType;

	if (pEntity != gMultiDamage.pEntity)
	{
		// UNDONE: wrong attacker!
		ApplyMultiDamage(pevInflictor, pevInflictor);
		gMultiDamage.pEntity = pEntity;
		gMultiDamage.amount = 0;
	}

	gMultiDamage.amount += flDamage;
}

void SpawnBlood(Vector vecSpot, int bloodColor, float flDamage)
{
	UTIL_BloodDrips(vecSpot, g_vecAttackDir, bloodColor, int(flDamage));
}

NOXREF int DamageDecal(CBaseEntity *pEntity, int bitsDamageType)
{
	if (pEntity)
	{
		return pEntity->DamageDecal(bitsDamageType);
	}

	return RANDOM_LONG(DECAL_GUNSHOT4, DECAL_GUNSHOT5);
}

void DecalGunshot(TraceResult *pTrace, int iBulletType, bool ClientOnly, entvars_t *pShooter, bool bHitMetal)
{
	;
}

// EjectBrass - tosses a brass shell from passed origin at passed velocity
void EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype, int entityIndex)
{
	MESSAGE_BEGIN(MSG_PVS, gmsgBrass, vecOrigin);
		WRITE_COORD(vecOrigin.x);	// origin
		WRITE_COORD(vecOrigin.y);
		WRITE_COORD(vecOrigin.z);
		WRITE_COORD(vecVelocity.x);	// velocity
		WRITE_COORD(vecVelocity.y);
		WRITE_COORD(vecVelocity.z);
		WRITE_ANGLE(rotation);
		WRITE_SHORT(model);
		WRITE_BYTE(soundtype);
		WRITE_BYTE(entityIndex);
	MESSAGE_END();
}

// Precaches the weapon and queues the weapon info for sending to clients
void UTIL_PrecacheOtherWeapon(WeaponIdType iId)
{
	CBaseWeapon* p = CBaseWeapon::Give(iId);

	if (p)
	{
		p->Precache();
		p->Kill();
	}
}

// Called by worldspawn
void WeaponsPrecache()
{
	// originally, m_rgItemInfo and m_rgAmmoInfo should be re-zero here.

	// custom items...

	// common world objects
	UTIL_PrecacheOther("item_suit");
	UTIL_PrecacheOther("item_battery");
	UTIL_PrecacheOther("item_antidote");
	UTIL_PrecacheOther("item_security");
	UTIL_PrecacheOther("item_longjump");
	UTIL_PrecacheOther("item_kevlar");
	UTIL_PrecacheOther("item_assaultsuit");
	UTIL_PrecacheOther("item_thighpack");

	// guns
	for (int i = WEAPON_NONE; i < LAST_WEAPON; i++)
		UTIL_PrecacheOtherWeapon((WeaponIdType)i);

	// knife
	BasicKnife::Precache();

	PRECACHE_MODEL(THROWABLE_VIEW_MODEL);
	PRECACHE_MODEL(C4_WORLD_MODEL);
	PRECACHE_SOUND(C4_PLACED_SFX);

	PRECACHE_SOUND("items/ammopickup1.wav");	// grenade purchasing SFX.

	// container for dropped deathmatch weapons
	UTIL_PrecacheOther("weaponbox");

	g_sModelIndexFireball   = PRECACHE_MODEL("sprites/zerogxplode.spr");	// fireball
	g_sModelIndexWExplosion = PRECACHE_MODEL("sprites/WXplo1.spr");			// underwater fireball
	g_sModelIndexSmoke      = PRECACHE_MODEL("sprites/steam1.spr");			// smoke
	g_sModelIndexBubbles    = PRECACHE_MODEL("sprites/bubble.spr");			// bubbles
	g_sModelIndexBloodSpray = PRECACHE_MODEL("sprites/bloodspray.spr");		// initial blood
	g_sModelIndexBloodDrop  = PRECACHE_MODEL("sprites/blood.spr");			// splattered blood

	g_sModelIndexSmokePuff  = PRECACHE_MODEL("sprites/smokepuff.spr");
	g_sModelIndexFireball2  = PRECACHE_MODEL("sprites/eexplo.spr");
	g_sModelIndexFireball3  = PRECACHE_MODEL("sprites/fexplo.spr");
	g_sModelIndexFireball4  = PRECACHE_MODEL("sprites/fexplo1.spr");
	g_sModelIndexRadio      = PRECACHE_MODEL("sprites/radio.spr");

	g_sModelIndexCTGhost    = PRECACHE_MODEL("sprites/b-tele1.spr");
	g_sModelIndexTGhost     = PRECACHE_MODEL("sprites/c-tele1.spr");
	g_sModelIndexC4Glow     = PRECACHE_MODEL("sprites/ledglow.spr");

	g_sModelIndexLaser      = PRECACHE_MODEL("sprites/laserbeam.spr");
	g_sModelIndexLaserDot   = PRECACHE_MODEL("sprites/laserdot.spr");

	// used by explosions
	PRECACHE_MODEL("models/grenade.mdl");
	PRECACHE_MODEL("sprites/explode1.spr");

	// used by cryogrenade
	PRECACHE_MODEL("models/glassgibs.mdl");

	PRECACHE_SOUND("weapons/debris1.wav"); // explosion aftermaths
	PRECACHE_SOUND("weapons/debris2.wav"); // explosion aftermaths
	PRECACHE_SOUND("weapons/debris3.wav"); // explosion aftermaths

	PRECACHE_SOUND("weapons/grenade_hit1.wav");	// grenade
	PRECACHE_SOUND("weapons/grenade_hit2.wav");	// grenade
	PRECACHE_SOUND("weapons/grenade_hit3.wav");	// grenade

	PRECACHE_SOUND("weapons/bullet_hit1.wav");	// hit by bullet
	PRECACHE_SOUND("weapons/bullet_hit2.wav");	// hit by bullet

	PRECACHE_SOUND("items/weapondrop1.wav");	// weapon falls to the ground
	PRECACHE_SOUND("weapons/generic_reload.wav");

	PRECACHE_SOUND("weapons/steelsight_in.wav");	// as its name indecates, steelsight.
	PRECACHE_SOUND("weapons/steelsight_out.wav");
}

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

std::list<CBaseWeapon*>	CBaseWeapon::m_lstWeapons;

void CBaseWeapon::TheWeaponsThink(void)
{
	std::list<CBaseWeapon*>::iterator i = m_lstWeapons.begin();

	while (i != m_lstWeapons.end())
	{
		auto pWeapon = (*i);

		if (pWeapon->IsDead())
		{
			delete pWeapon;	// destory pointer, otherwise it would become a wild pointer.
			m_lstWeapons.erase(i++);  // alternatively, i = m_lstWeapons.erase(i);
		}
		else
		{
			// the owner is vanished.
			if (!pWeapon->m_pPlayer.IsValid() && !pWeapon->m_pWeaponBox.IsValid())
				pWeapon->Kill();

			// the pointer is missing on the player side.
			if (pWeapon->m_pPlayer.IsValid() && pWeapon->m_pPlayer->m_rgpPlayerItems[pWeapon->m_pItemInfo->m_iSlot] != pWeapon)
				pWeapon->Kill();

			// the pointer is missing on the weaponbox side.
			if (pWeapon->m_pWeaponBox.IsValid() && pWeapon->m_pWeaponBox->m_rgpPlayerItems[pWeapon->m_pItemInfo->m_iSlot] != pWeapon)
				pWeapon->Kill();

			++i;
		}
	}
}

CBaseWeapon* CBaseWeapon::Give(WeaponIdType iId, CBasePlayer* pPlayer, int iClip, unsigned bitsFlags)
{
	CBaseWeapon* p = nullptr;

	if (!FNullEnt(pPlayer))
	{
		if (pPlayer->m_rgpPlayerItems[g_rgWpnInfo[iId].m_iSlot] != nullptr)	// this player already got one in this slot!
			return nullptr;
	}

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

		UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

		Vector vecUp = RANDOM_FLOAT(100, 150) * gpGlobals->v_up;
		Vector vecRight = RANDOM_FLOAT(50, 70) * gpGlobals->v_right;

		Vector vecShellVelocity = (m_pPlayer->pev->velocity + vecRight + vecUp) + gpGlobals->v_forward * 25;
		int soundType = (m_iId == WEAPON_M1014 || m_iId == WEAPON_KSG12 || m_iId == WEAPON_AA12) ? 2 : 1;

		// never worry about those origin settings on first personal VFX.
		// the message will detect whether it belongs to a local player, if so, replace the origin here with g_pViewEnt->attachments[1].
		EjectBrass(m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -9 + gpGlobals->v_forward * 16,
			vecShellVelocity, m_pPlayer->pev->angles.yaw, m_pPlayer->m_iShellModelIndex, soundType, m_pPlayer->entindex());
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
	// how can I add to someone who didn't even exist?
	if (FNullEnt(pPlayer))
		return false;

	if (pPlayer->m_rgpPlayerItems[g_rgWpnInfo[m_iId].m_iSlot] != nullptr)	// this player already got one in this slot!
		return false;

	m_pWeaponBox = nullptr;	// make the weaponbox disown me.
	m_pPlayer = pPlayer;
	m_bitsFlags |= WPNSTATE_DRAW_FIRST;	// play draw_first anim.

	SetVariation(pPlayer->m_iRoleType);
	return true;
}

void CBaseWeapon::PostFrame()
{
	int usableButtons = m_pPlayer->pev->button;

	// if we should be holster, then just do it. stop everything else.
	if (m_bitsFlags & WPNSTATE_HOLSTERING)
	{
		m_pPlayer->SwitchWeapon(m_pPlayer->GetItemById(m_pPlayer->m_iWpnSwitchingTo));
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
				UTIL_HideSecondaryVMDL(m_pPlayer);
				SetLeftHand(true);
				break;

			default:
				// back to our weapon.
				Holstered();
				Deploy();
				break;
			}

			// check inventory of current equipment.
			m_pPlayer->ResetUsingEquipment();

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

			case EQP_C4:	// treat specially.
			{
				m_pPlayer->SetAnimation(PLAYER_ATTACK1);
				CGrenade::C4(m_pPlayer);

				m_bitsFlags |= WPNSTATE_QT_EXIT;
				m_pPlayer->m_flNextAttack = C4_TIME_THROW - C4_TIME_THROW_SPAWN;

				if (*m_pPlayer->GetGrenadeInventoryPointer(EQP_C4))
					(*m_pPlayer->GetGrenadeInventoryPointer(EQP_C4))--;

				return;
			}

			case EQP_DETONATOR:
			{
				CGrenade::C4_Detonate(m_pPlayer);

				m_bitsFlags |= WPNSTATE_QT_EXIT;
				m_pPlayer->m_flNextAttack = C4_TIME_DET_ANIM - C4_TIME_DETONATE;

				return;
			}

			case EQP_FLASHLIGHT:	// Flashlight should never get you here. But, anyway...
			default:
				return;	// how did he get here???
			}

			// reduce the grenade inventory
			if (*m_pPlayer->GetGrenadeInventoryPointer(m_pPlayer->m_iUsingGrenadeId))
				(*m_pPlayer->GetGrenadeInventoryPointer(m_pPlayer->m_iUsingGrenadeId))--;

			m_pPlayer->ThrowGrenade(m_pPlayer->m_iUsingGrenadeId);
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

				TraceResult tr;
				UTIL_TraceLine(vecSrc, vecEnd, ignore_monsters, m_pPlayer->edict(), &tr);

				// if you near a wall that much...
				if (tr.flFraction < 1.0f)
					iAnim = C4_PLACE;

				m_bitsFlags |= WPNSTATE_QT_SHOULD_SPAWN;
				UTIL_SendSecondaryVMDLAnim(m_pPlayer, iAnim);
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

		// hold and do nothing. except C4.
		else
		{
		}

		return;
	}

	if (m_bitsFlags & (WPNSTATE_NO_LHAND | WPNSTATE_AUTO_LAND_UP) && m_pPlayer->m_flNextAttack <= 0.0f)
	{
		UTIL_HideSecondaryVMDL(m_pPlayer);
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

	// handle block situation.
	if ((m_pPlayer->m_afButtonPressed | m_pPlayer->m_afButtonReleased) & IN_BLOCK)
	{
		// let the idle function handle it.
		// we cannot let the block anims interfere other normal anims.
		m_flTimeWeaponIdle = -1;
	}

	if (!(usableButtons & IN_BLOCK) && (	// you cannot aim if you are blocked.
		(!m_pPlayer->m_bHoldToAim && usableButtons & IN_ATTACK2 && m_flNextSecondaryAttack <= UTIL_WeaponTimeBase()) ||	// PRESS to aim
		(m_pPlayer->m_bHoldToAim && ((m_pPlayer->m_afButtonPressed & IN_ATTACK2 && !m_bInZoom) || (m_pPlayer->m_afButtonReleased & IN_ATTACK2 && m_bInZoom)) )	// HOLD to aim
		))	// UseDecrement()
	{
#ifndef CLIENT_PREDICT_AIM
		MESSAGE_BEGIN(MSG_ONE, gmsgSteelSight, nullptr, m_pPlayer->pev);
		WRITE_BYTE(m_bInZoom);
		MESSAGE_END();
#endif

		SecondaryAttack();

		// only cancel this flag in PRESS mode.
		if (!m_pPlayer->m_bHoldToAim)
			m_pPlayer->pev->button &= ~IN_ATTACK2;
	}
	else if ((m_pPlayer->pev->button & IN_ATTACK) && CanAttack(m_flNextPrimaryAttack, UTIL_WeaponTimeBase(), TRUE) && !(usableButtons & IN_BLOCK))	// UseDecrement()
	{
		// Can't shoot during the freeze period, but always allow firing in single player.
		// Neither can you if blocked.
		if ((m_pPlayer->m_bCanShoot && CSGameRules()->IsMultiplayer() && !CSGameRules()->IsFreezePeriod()) || !CSGameRules()->IsMultiplayer())
		{
#ifndef CLIENT_PREDICT_PRIM_ATK
			// prediction code is unusable for full-automatic weapon. I have to use this instead.
			// UNDONE: perhaps I need to send a angle along with?
			MESSAGE_BEGIN(MSG_ONE, gmsgShoot, nullptr, m_pPlayer->pev);
			WRITE_SHORT(m_pPlayer->random_seed);
			MESSAGE_END();
#endif
			PrimaryAttack();
			m_flNextPrimaryAttack *= m_pPlayer->WeaponFireIntervalModifier(this);	// passive skill applied.
		}
	}
	else if ((m_pPlayer->pev->button & IN_RELOAD) && m_pItemInfo->m_iMaxClip != WEAPON_NOCLIP && !m_bInReload && m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
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

bool CBaseWeapon::Melee(void)
{
	// you just.. can't do this.
	if (m_bitsFlags & WPNSTATE_BUSY)
		return false;

	if (m_bInZoom)
		SecondaryAttack();

	// save what we are doing right now.
	PushAnim();

	// then, swing the knife.
	BasicKnife::Deploy(this);
	BasicKnife::Swing();
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
	const char *pszViewModel = THROWABLE_VIEW_MODEL, *pszWeaponModel = "";	// remember, you have to use the constant string.
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
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/weapons/w_c4.mdl");

		Q_strlcpy(m_pPlayer->m_szAnimExtention, "grenade");
		SetLeftHand(false);	// holster L hand. The freeze time (m_flNextAttack) should be included.

		UTIL_SetSecondaryVMDL(m_pPlayer, SSZ_C4_VMDL, C4_DRAW);

		goto TAG_C4_SKIPPING;
	}

	case EQP_DETONATOR:
	{
		m_bitsFlags |= WPNSTATE_QUICK_THROWING | WPNSTATE_QT_RELEASE | WPNSTATE_QT_SHOULD_SPAWN;	// consider detonator is already "released" on start.
		m_pPlayer->m_iUsingGrenadeId = iId;
		// no 3rd personal model setting needed.

		SetLeftHand(false);	// holster L hand. The freeze time (m_flNextAttack) should be included.
		m_pPlayer->m_flNextAttack = C4_TIME_DETONATE;	// however, we have to overwrite it.

		UTIL_SetSecondaryVMDL(m_pPlayer, SSZ_C4_VMDL, C4_DETONATE);

		goto TAG_C4_SKIPPING;
	}

	case EQP_FLASHLIGHT:
	{
		// No bit flag to set.
		m_pPlayer->m_iUsingGrenadeId = iId;

		// Nothing to do but turn it on.
		if (m_pPlayer->FlashlightIsOn())
			m_pPlayer->FlashlightTurnOff();
		else
			m_pPlayer->FlashlightTurnOn();

		// Nothing to set, or cooldown.
		return true;
	}

	default:
		return false;	// how did he get here???
	}

	m_bitsFlags |= WPNSTATE_QUICK_THROWING;
	m_pPlayer->m_iUsingGrenadeId = iId;
	m_pPlayer->pev->viewmodel = MAKE_STRING(pszViewModel);
	m_pPlayer->pev->weaponmodel = MAKE_STRING(pszWeaponModel);

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

	m_bInReload = false;
	m_bInZoom = false;

	m_pPlayer->pev->viewmodel = 0;
	m_pPlayer->pev->weaponmodel = 0;
	m_pPlayer->pev->fov = DEFAULT_FOV;
}

bool CBaseWeapon::Drop(void** ppWeaponBoxReturned)
{
	// LUNA: you cannot reject drop request here. it depents on player/other functions.

	// LUNA: you can't call it here. You have to wait it called in pWeaponBox->PackWeapon(). or this would result in packing weapon failure.
	//RemovePlayerItem(pWeapon);

	UTIL_MakeVectors(m_pPlayer->pev->angles);

	CWeaponBox* pWeaponBox = (CWeaponBox*)CBaseEntity::Create("weaponbox", m_pPlayer->pev->origin + gpGlobals->v_forward * 10, m_pPlayer->pev->angles, m_pPlayer->edict());
	pWeaponBox->pev->angles.x = 0;
	pWeaponBox->pev->angles.z = 0;
	pWeaponBox->SetThink(&CWeaponBox::Kill);
	pWeaponBox->pev->nextthink = gpGlobals->time + item_staytime.value;
	pWeaponBox->PackWeapon(this);	// !this call would detach weapon from CBasePlayer databse and attack it to CWeaponBox database.

	if (m_pPlayer->IsAlive())	// drop by intense
		pWeaponBox->pev->velocity = m_pPlayer->pev->velocity + gpGlobals->v_forward * 300 + gpGlobals->v_forward * 100;	// this velocity would be override soon.
	else
	{
		pWeaponBox->pev->velocity = m_pPlayer->pev->velocity * 0.85f;	// drop due to death.
	}

	// if this item is using bpammo instead of magzine, we should pack all player bpammo.
	if (m_pItemInfo->m_bitsFlags & ITEM_FLAG_EXHAUSTIBLE)
	{
		if (m_iPrimaryAmmoType > AMMO_NONE && m_iPrimaryAmmoType < AMMO_MAXTYPE)
		{
			pWeaponBox->GiveAmmo(m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType], m_iPrimaryAmmoType);
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
		}
	}

	const char* modelname = CWeaponBox::GetCSModelName(m_iId);
	if (modelname)
	{
		pWeaponBox->SetModel(modelname);
	}

	// if the caller is inquiring CWeaponBox, give it to him.
	if (ppWeaponBoxReturned != nullptr)
	{
		(*ppWeaponBoxReturned) = pWeaponBox;
	}

	// have to re-zero m_pPlayer.
	// otherwise, since CBasePlayer::RemovePlayerItem() had it removed from m_rgpPlayerItems[], CBaseWeapons::WeaponsThink() would consider this is an error weapon.
	m_pPlayer = nullptr;

	return true;
}

bool CBaseWeapon::Kill(void)
{
	if (m_pPlayer.IsValid())
	{
		m_pPlayer->RemovePlayerItem(this);
		m_pPlayer = nullptr;
	}

	if (m_pWeaponBox.IsValid())
	{
		if (m_pWeaponBox->m_rgpPlayerItems[m_pItemInfo->m_iSlot] == this)
			m_pWeaponBox->m_rgpPlayerItems[m_pItemInfo->m_iSlot] = nullptr;

		m_pWeaponBox = nullptr;
	}

	m_bitsFlags |= WPNSTATE_DEAD;
	return true;
}

void CBaseWeapon::UpdateClientData(void)
{
	bool bSend = false;
	int state = 0;

	if (m_pPlayer->m_pActiveItem == this)
	{
		if (m_pPlayer->m_fOnTarget)
			state = WEAPON_IS_ONTARGET;
		else
			state = 1;
	}

	if (!m_pPlayer->m_bClientWeaponUpToDate)
		bSend = true;

	if (this == m_pPlayer->m_pActiveItem || this == m_pPlayer->m_pClientActiveItem)
	{
		if (m_pPlayer->m_pActiveItem != m_pPlayer->m_pClientActiveItem)
			bSend = true;
	}

	if (m_iClip != m_iClientClip || state != m_iClientWeaponState || int(m_pPlayer->pev->fov) != m_pPlayer->m_iClientFOV)
		bSend = true;

	if (bSend)
	{
		// deleted by LUNA: gmsgCurWeapon

		m_iClientClip = m_iClip;
		m_iClientWeaponState = state;
		m_pPlayer->m_bClientWeaponUpToDate = true;
	}
}

bool CBaseWeapon::AddPrimaryAmmo(int iCount)
{
	CBaseEntity* pEntity = nullptr;
	if (m_pPlayer.IsValid())	// player has the priority of having the ammo. although it's impossible that both m_pPlayer and m_pWeaponBox are valid, it must be wrong.
		pEntity = m_pPlayer;
	else if (m_pWeaponBox.IsValid())
		pEntity = m_pWeaponBox;

	if (!pEntity)	// this is a dead weapon. SOON.
		return false;

	if (iCount <= 0)
		return false;

	if (m_iPrimaryAmmoType <= 0 || m_iPrimaryAmmoType >= AMMO_MAXTYPE)	// wrongful ammo type!
		return false;

	bool bGotAmmo = false;

	if (m_pItemInfo->m_iMaxClip < 1)
	{
		m_iClip = WEAPON_NOCLIP;
		bGotAmmo = pEntity->GiveAmmo(iCount, m_iPrimaryAmmoType);
	}
	else if (m_iClip < m_pItemInfo->m_iMaxClip)
	{
		if (m_iClip + iCount <= m_pItemInfo->m_iMaxClip)	// not enough or just fill.
		{
			m_iClip += iCount;
		}
		else
		{
			iCount = m_iClip + iCount - m_pItemInfo->m_iMaxClip;
			m_iClip = m_pItemInfo->m_iMaxClip;

			bGotAmmo = pEntity->GiveAmmo(iCount, m_iPrimaryAmmoType);
		}
	}
	else
	{
		bGotAmmo = pEntity->GiveAmmo(iCount, m_iPrimaryAmmoType);
	}

	if (pEntity->IsPlayer())	// only a player can make this sound.
		EMIT_SOUND(pEntity->edict(), CHAN_ITEM, "items/9mmclip1.wav", VOL_NORM, ATTN_NORM);

	return bGotAmmo;
}

bool CBaseWeapon::DefaultDeploy(const char* szViewModel, const char* szWeaponModel, int iAnim, const char* szAnimExt, float flDeployTime)
{
	// TODO
	/*if (!CanDeploy())
		return false;*/

	m_pPlayer->pev->viewmodel = MAKE_STRING(szViewModel);
	m_pPlayer->pev->weaponmodel = MAKE_STRING(szWeaponModel);

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

bool CBaseWeapon::DefaultHolster(int iHolsterAnim, float flHolsterDelay)
{
	m_pPlayer->m_flEjectBrass = 0;	// prevents famous AWP-shell-ejecting bug.

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

template<class CWpn>
void CBaseWeapon::DefaultDashStart(void)
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

	SendWeaponAnim(CWpn::DASH_ENTER);
	m_pPlayer->m_flNextAttack = CWpn::DASH_ENTER_TIME;
	m_flTimeWeaponIdle = CWpn::DASH_ENTER_TIME;
	m_bitsFlags |= WPNSTATE_DASHING;
}

template<class CWpn>
void CBaseWeapon::DefaultDashEnd(void)
{
	if (m_pPlayer->m_flNextAttack > 0.0f && m_pPlayer->pev->weaponanim == CWpn::DASH_ENTER)
	{
		// this is how much you procees to the dashing phase.
		// for example, assuming the whole length is 1.0s, you start 0.7s and decide to cancel.
		// although there's only 0.3s to the dashing phase, but turning back still requires another equally 0.7s.
		// "m_pPlayer->m_flNextAttack" is the 0.3s of full length. you need to get the rest part, i.e. the 70%.
		float flRunStartUnplayedRatio = 1.0f - m_pPlayer->m_flNextAttack / CWpn::DASH_ENTER_TIME;

		// stick on the last instance in the comment: 70% * 1.0s(full length) = 0.7s, this is the time we need to turning back.
		float flRunStopTimeLeft = CWpn::DASH_EXIT_TIME * flRunStartUnplayedRatio;

		// play the anim.
		SendWeaponAnim(CWpn::DASH_EXIT);

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
		SendWeaponAnim(CWpn::DASH_EXIT);
		m_pPlayer->m_flNextAttack = CWpn::DASH_EXIT_TIME;
		m_flTimeWeaponIdle = CWpn::DASH_EXIT_TIME;
	}

	// either way, we have to remove this flag.
	m_bitsFlags &= ~WPNSTATE_DASHING;

#ifdef CLIENT_DLL
	// remove the exaggerating shaking vfx.
	g_flGunBobAmplitudeModifier = 1.0f;
#endif
}

template<class CWpn>
bool CBaseWeapon::DefaultSetLHand(bool bAppear)
{
	if (bAppear && m_bitsFlags & WPNSTATE_NO_LHAND)
	{
		SendWeaponAnim(CWpn::LHAND_UP);
		m_pPlayer->m_flNextAttack = CWpn::LHAND_UP_TIME;
		m_flTimeWeaponIdle = CWpn::LHAND_UP_TIME;
		m_bitsFlags &= ~WPNSTATE_NO_LHAND;

		return true;
	}
	else if (!(m_bitsFlags & WPNSTATE_NO_LHAND))
	{
		SendWeaponAnim(CWpn::LHAND_DOWN);
		m_pPlayer->m_flNextAttack = CWpn::LHAND_DOWN_TIME;
		m_flTimeWeaponIdle = CWpn::LHAND_DOWN_TIME;
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
		SendWeaponAnim(iEnterAnim);
	}

	// therefore, you should not keep calling it in Think() or Frame().
	else if (!bBlocked && m_pPlayer->pev->weaponanim != iExitAnim)
	{
		SendWeaponAnim(iExitAnim);
	}
}

void CBaseWeapon::SendWeaponAnim(int iAnim, bool bSkipLocal)
{
	m_pPlayer->pev->weaponanim = iAnim;

#ifdef CLIENT_WEAPONS
	if (bSkipLocal && ENGINE_CANSKIP(m_pPlayer->edict()))
		return;
#endif

	MESSAGE_BEGIN(MSG_ONE, SVC_WEAPONANIM, nullptr, m_pPlayer->pev);
	WRITE_BYTE(iAnim);		// sequence number
	WRITE_BYTE(0);			// weaponmodel bodygroup. it doesn't matter at SV side, since the client will override it anyway.
	MESSAGE_END();
}

void CBaseWeapon::PlayEmptySound()
{
	switch (m_iId)
	{
	case WEAPON_USP:
	case WEAPON_GLOCK18:
	case WEAPON_ANACONDA:
	case WEAPON_DEAGLE:
	case WEAPON_M45A1:
	case WEAPON_FIVESEVEN:
		EMIT_SOUND(m_pPlayer->edict(), CHAN_WEAPON, "weapons/dryfire_pistol.wav", 0.8, ATTN_NORM);
		break;
	default:
		EMIT_SOUND(m_pPlayer->edict(), CHAN_WEAPON, "weapons/dryfire_rifle.wav", 0.8, ATTN_NORM);
		break;
	}
}

void CBaseWeapon::ReloadSound()
{
	CBasePlayer* pPlayer = nullptr;
	while ((pPlayer = UTIL_FindEntityByClassname(pPlayer, "player")))
	{
		if (pPlayer->IsDormant())
			continue;

		if (pPlayer == m_pPlayer)
			continue;

		if (pPlayer->m_iObserverLastMode == OBS_IN_EYE && pPlayer->m_hObserverTarget.IsValid() && pPlayer->m_hObserverTarget->entindex() == m_pPlayer->entindex())	// avoid the sfx send to the observer.
			continue;

		float distance = (m_pPlayer->pev->origin - pPlayer->pev->origin).Length();
		if (distance <= MAX_DIST_RELOAD_SOUND)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgReloadSound, nullptr, pPlayer->pev);
			WRITE_BYTE(int((1.0f - (distance / MAX_DIST_RELOAD_SOUND)) * 255.0f));
			if (m_iId == WEAPON_M1014 || m_iId == WEAPON_KSG12)
				WRITE_BYTE(0);
			else
				WRITE_BYTE(1);
			MESSAGE_END();
		}
	}
}

void CBaseWeapon::PushAnim(void)
{
	m_Stack.m_flEjectBrass			= m_pPlayer->m_flEjectBrass - gpGlobals->time;
	m_Stack.m_flNextAttack			= m_pPlayer->m_flNextAttack;
	m_Stack.m_flNextPrimaryAttack	= m_flNextPrimaryAttack;
	m_Stack.m_flNextSecondaryAttack	= m_flNextSecondaryAttack;
	m_Stack.m_flTimeWeaponIdle		= m_flTimeWeaponIdle;
	m_Stack.m_iSequence				= m_pPlayer->pev->weaponanim;
	m_Stack.m_iShellModelIndex		= m_pPlayer->m_iShellModelIndex;
}

void CBaseWeapon::PopAnim(void)
{
	// invalid pop.
	if (m_Stack.m_iSequence < 0)
		return;

	m_pPlayer->m_flEjectBrass		= m_Stack.m_flEjectBrass + gpGlobals->time;
	m_pPlayer->m_flNextAttack		= m_Stack.m_flNextAttack;
	m_flNextPrimaryAttack			= m_Stack.m_flNextPrimaryAttack;
	m_flNextSecondaryAttack			= m_Stack.m_flNextSecondaryAttack;
	m_flTimeWeaponIdle				= m_Stack.m_flTimeWeaponIdle;
	m_pPlayer->pev->weaponanim		= m_Stack.m_iSequence;
	m_pPlayer->m_iShellModelIndex	= m_Stack.m_iShellModelIndex;

	// LUNA: I don't know why, but execute this can prevent anim-restart over. This has to be done on both side.
	SendWeaponAnim(m_Stack.m_iSequence);

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
