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

	// awp magnum
	UTIL_PrecacheOtherWeapon(WEAPON_AWP);

	UTIL_PrecacheOtherWeapon(WEAPON_SVD);
	UTIL_PrecacheOtherWeapon(WEAPON_AK47);
	UTIL_PrecacheOtherWeapon(WEAPON_M200);

	// m249
	UTIL_PrecacheOtherWeapon(WEAPON_MK46);

	UTIL_PrecacheOtherWeapon(WEAPON_M4A1);
	UTIL_PrecacheOtherWeapon(WEAPON_SCARL);
	UTIL_PrecacheOtherWeapon(WEAPON_ACR);
	UTIL_PrecacheOtherWeapon(WEAPON_M14EBR);
	UTIL_PrecacheOtherWeapon(WEAPON_CM901);
	UTIL_PrecacheOtherWeapon(WEAPON_QBZ95);

	// shotgun
	UTIL_PrecacheOtherWeapon(WEAPON_KSG12);
	UTIL_PrecacheOtherWeapon(WEAPON_STRIKER);

	UTIL_PrecacheOtherWeapon(WEAPON_USP);
	UTIL_PrecacheOtherWeapon(WEAPON_PM9);
	UTIL_PrecacheOtherWeapon(WEAPON_UMP45);

	UTIL_PrecacheOtherWeapon(WEAPON_FIVESEVEN);
	UTIL_PrecacheOtherWeapon(WEAPON_P90);

	// deagle
	UTIL_PrecacheOtherWeapon(WEAPON_DEAGLE);

	// p228
	UTIL_PrecacheOtherWeapon(WEAPON_ANACONDA);

	// knife
	UTIL_PrecacheOtherWeapon(WEAPON_KNIFE);
	BasicKnife::Precache();

	UTIL_PrecacheOtherWeapon(WEAPON_GLOCK18);
	UTIL_PrecacheOtherWeapon(WEAPON_MP5N);
	UTIL_PrecacheOtherWeapon(WEAPON_MP7A1);
	UTIL_PrecacheOtherWeapon(WEAPON_P99);

	UTIL_PrecacheOtherWeapon(WEAPON_FLASHBANG);
	UTIL_PrecacheOtherWeapon(WEAPON_HEGRENADE);
	UTIL_PrecacheOtherWeapon(WEAPON_SMOKEGRENADE);

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
		if ((*i)->IsDead())
		{
			m_lstWeapons.erase(i++);  // alternatively, i = m_lstWeapons.erase(i);
		}
		else
		{
			auto pWeapon = (*i);

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
		if (pPlayer->m_rgpPlayerItems[g_rgItemInfo[iId].m_iSlot] != nullptr)	// this player already got one in this slot!
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

	case WEAPON_CM901:
		p = new CCM901;
		break;

	case WEAPON_KSG12:
		p = new CKSG12;
		break;

	case WEAPON_MP7A1:
		p = new CMP7A1;
		break;

	case WEAPON_QBZ95:
		p = new CQBZ95;
		break;

	case WEAPON_USP:
		p = new CUSP;
		break;

	default:
		return nullptr;
	}

	m_lstWeapons.emplace_back(p);

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

		UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

		Vector vecUp = RANDOM_FLOAT(100, 150) * gpGlobals->v_up;
		Vector vecRight = RANDOM_FLOAT(50, 70) * gpGlobals->v_right;

		Vector vecShellVelocity = (m_pPlayer->pev->velocity + vecRight + vecUp) + gpGlobals->v_forward * 25;
		int soundType = (m_iId == WEAPON_STRIKER || m_iId == WEAPON_KSG12) ? 2 : 1;

		EjectBrass(m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -9 + gpGlobals->v_forward * 16,
			vecShellVelocity, m_pPlayer->pev->angles.yaw, m_pPlayer->m_iShellModelIndex, soundType, m_pPlayer->entindex());
	}
}

bool CBaseWeapon::AddToPlayer(CBasePlayer* pPlayer)
{
	// how can I add to someone who didn't even exist?
	if (FNullEnt(pPlayer))
		return false;

	if (pPlayer->m_rgpPlayerItems[g_rgItemInfo[m_iId].m_iSlot] != nullptr)	// this player already got one in this slot!
		return false;

	m_pWeaponBox = nullptr;	// make the weaponbox disown me.
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
		MESSAGE_BEGIN(MSG_ONE, gmsgSteelSight, nullptr, m_pPlayer->pev);
		WRITE_BYTE(m_bInZoom);
		MESSAGE_END();

		SecondaryAttack();
		m_pPlayer->pev->button &= ~IN_ATTACK2;
	}
	else if ((m_pPlayer->pev->button & IN_ATTACK) && CanAttack(m_flNextPrimaryAttack, UTIL_WeaponTimeBase(), TRUE))	// UseDecrement()
	{
		// Can't shoot during the freeze period
		// Always allow firing in single player
		if ((m_pPlayer->m_bCanShoot && CSGameRules()->IsMultiplayer() && !CSGameRules()->IsFreezePeriod()) || !CSGameRules()->IsMultiplayer())
		{
			// prediction code is unusable for full-automatic weapon. I have to use this instead.
			// UNDONE: perhaps I need to send a angle along with?
			MESSAGE_BEGIN(MSG_ONE, gmsgShoot, nullptr, m_pPlayer->pev);
			WRITE_SHORT(m_pPlayer->random_seed);
			MESSAGE_END();

			PrimaryAttack();
			m_flNextPrimaryAttack *= m_pPlayer->WeaponFireIntervalModifier(this);	// passive skill applied.
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

	if (m_bInZoom)
		SecondaryAttack();

	// save what we are doing right now.
	PushAnim();

	// then, swing the knife.
	BasicKnife::Deploy(this);
	BasicKnife::Swing();
	return true;
}

bool CBaseWeapon::Holster(bool bTrial)
{
	if (m_bitsFlags & WPNSTATE_MELEE)	// you can't holster while meleeing.
		return false;

	// if this is only a trial like original CanHolster(), let's finish here.
	if (bTrial)
		return true;

	m_bInReload = false;
	m_bInZoom = false;

	m_pPlayer->pev->viewmodel = 0;
	m_pPlayer->pev->weaponmodel = 0;

	m_pPlayer->m_flEjectBrass = 0;	// prevents famous AWP bug.

	return true;
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

bool CBaseWeapon::DefaultDeploy(const char* szViewModel, const char* szWeaponModel, int iAnim, const char* szAnimExt)
{
	// TODO
	/*if (!CanDeploy())
		return FALSE;*/

	m_pPlayer->pev->viewmodel = MAKE_STRING(szViewModel);
	m_pPlayer->pev->weaponmodel = MAKE_STRING(szWeaponModel);

	Q_strlcpy(m_pPlayer->m_szAnimExtention, szAnimExt);
	SendWeaponAnim(iAnim);

	m_pPlayer->m_flNextAttack = 0.75f;
	m_flTimeWeaponIdle = 1.5f;
	m_flDecreaseShotsFired = gpGlobals->time;

	m_pPlayer->pev->fov = DEFAULT_FOV;
	m_pPlayer->m_iLastZoom = DEFAULT_FOV;
	m_pPlayer->m_bResumeZoom = false;
	m_pPlayer->m_vecVAngleShift = g_vecZero;

	return TRUE;
}

void CBaseWeapon::SendWeaponAnim(int iAnim, int iBody, bool bSkipLocal)
{
	m_pPlayer->pev->weaponanim = iAnim;

#ifdef CLIENT_WEAPONS
	if (bSkipLocal && ENGINE_CANSKIP(m_pPlayer->edict()))
		return;
#endif

	MESSAGE_BEGIN(MSG_ONE, SVC_WEAPONANIM, nullptr, m_pPlayer->pev);
	WRITE_BYTE(iAnim);		// sequence number
	WRITE_BYTE(iBody);		// weaponmodel bodygroup.
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
	case WEAPON_P99:
	case WEAPON_FIVESEVEN:
		EMIT_SOUND(m_pPlayer->edict(), CHAN_WEAPON, "weapons/dryfire_pistol.wav", 0.8, ATTN_NORM);
		break;
	default:
		EMIT_SOUND(m_pPlayer->edict(), CHAN_WEAPON, "weapons/dryfire_rifle.wav", 0.8, ATTN_NORM);
		break;
	}
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
			if (m_iId == WEAPON_STRIKER || m_iId == WEAPON_KSG12)
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
}
