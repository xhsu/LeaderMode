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
void EjectBrass(const Vector &vecOrigin, const Vector &vecLeft, const Vector &vecVelocity, float rotation, int model, int soundtype, int entityIndex)
{
	bool useNewBehavior = AreRunningCZero();

	MESSAGE_BEGIN(MSG_PVS, gmsgBrass, vecOrigin);
		if (!useNewBehavior)
		{
			// noxref
			WRITE_BYTE(TE_MODEL);
		}
		WRITE_COORD(vecOrigin.x);	// origin
		WRITE_COORD(vecOrigin.y);
		WRITE_COORD(vecOrigin.z);
		if (!useNewBehavior)
		{
			// noxref
			// it parses the client side, but does not use it
			WRITE_COORD(vecLeft.x);
			WRITE_COORD(vecLeft.y);
			WRITE_COORD(vecLeft.z);
		}
		WRITE_COORD(vecVelocity.x);	// velocity
		WRITE_COORD(vecVelocity.y);
		WRITE_COORD(vecVelocity.z);
		WRITE_ANGLE(rotation);
		WRITE_SHORT(model);
		WRITE_BYTE(soundtype);
		if (!useNewBehavior)
		{
			// noxref
			WRITE_BYTE(25);// 2.5 seconds
		}
		WRITE_BYTE(entityIndex);
	MESSAGE_END();
}

NOXREF void EjectBrass2(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype, entvars_t *pev)
{
	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, nullptr, pev);
		WRITE_BYTE(TE_MODEL);
		WRITE_COORD(vecOrigin.x);
		WRITE_COORD(vecOrigin.y);
		WRITE_COORD(vecOrigin.z);
		WRITE_COORD(vecVelocity.x);
		WRITE_COORD(vecVelocity.y);
		WRITE_COORD(vecVelocity.z);
		WRITE_ANGLE(rotation);
		WRITE_SHORT(model);
		WRITE_BYTE(0);
		WRITE_BYTE(5);// 0.5 seconds
	MESSAGE_END();
}

// Precaches the weapon and queues the weapon info for sending to clients
void UTIL_PrecacheOtherWeapon(const char *szClassname)
{
	edict_t *pEdict = CREATE_NAMED_ENTITY(MAKE_STRING(szClassname));
	if (FNullEnt(pEdict))
	{
		ALERT(at_console, "NULL Ent in UTIL_PrecacheOtherWeapon classname `%s`\n", szClassname);
		return;
	}

	CBasePlayerItem *pItem = GET_PRIVATE<CBasePlayerItem>(pEdict);
	if (pItem)
		pItem->Precache();

	REMOVE_ENTITY(pEdict);
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
	UTIL_PrecacheOtherWeapon("weapon_awp");

	UTIL_PrecacheOtherWeapon("weapon_g3sg1");
	UTIL_PrecacheOtherWeapon("weapon_ak47");
	UTIL_PrecacheOtherWeapon("weapon_scout");

	// m249
	UTIL_PrecacheOtherWeapon("weapon_m249");

	UTIL_PrecacheOtherWeapon("weapon_m4a1");
	UTIL_PrecacheOtherWeapon("weapon_sg552");
	UTIL_PrecacheOtherWeapon("weapon_aug");
	UTIL_PrecacheOtherWeapon("weapon_sg550");
	UTIL_PrecacheOtherWeapon("weapon_galil");
	UTIL_PrecacheOtherWeapon("weapon_famas");

	// shotgun
	UTIL_PrecacheOtherWeapon("weapon_m3");
	UTIL_PrecacheOtherWeapon("weapon_xm1014");

	UTIL_PrecacheOtherWeapon("weapon_usp");
	UTIL_PrecacheOtherWeapon("weapon_mac10");
	UTIL_PrecacheOtherWeapon("weapon_ump45");

	UTIL_PrecacheOtherWeapon("weapon_fiveseven");
	UTIL_PrecacheOtherWeapon("weapon_p90");

	// deagle
	UTIL_PrecacheOtherWeapon("weapon_deagle");

	// p228
	UTIL_PrecacheOtherWeapon("weapon_p228");

	// knife
	UTIL_PrecacheOtherWeapon("weapon_knife");

	UTIL_PrecacheOtherWeapon("weapon_glock18");
	UTIL_PrecacheOtherWeapon("weapon_mp5navy");
	UTIL_PrecacheOtherWeapon("weapon_tmp");
	UTIL_PrecacheOtherWeapon("weapon_elite");

	UTIL_PrecacheOtherWeapon("weapon_flashbang");
	UTIL_PrecacheOtherWeapon("weapon_hegrenade");
	UTIL_PrecacheOtherWeapon("weapon_smokegrenade");

	if (g_pGameRules->IsDeathmatch())
	{
		// container for dropped deathmatch weapons
		UTIL_PrecacheOther("weaponbox");
	}

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
}

TYPEDESCRIPTION CBasePlayerItem::m_SaveData[] =
{
	DEFINE_FIELD(CBasePlayerItem, m_pPlayer, FIELD_CLASSPTR),
	DEFINE_FIELD(CBasePlayerItem, m_pNext, FIELD_CLASSPTR),
	DEFINE_FIELD(CBasePlayerItem, m_iId, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CBasePlayerItem, CBaseAnimating)

TYPEDESCRIPTION CBasePlayerWeapon::m_SaveData[] =
{
	DEFINE_FIELD(CBasePlayerWeapon, m_flNextPrimaryAttack, FIELD_TIME),
	DEFINE_FIELD(CBasePlayerWeapon, m_flNextSecondaryAttack, FIELD_TIME),
	DEFINE_FIELD(CBasePlayerWeapon, m_flTimeWeaponIdle, FIELD_TIME),
	DEFINE_FIELD(CBasePlayerWeapon, m_iPrimaryAmmoType, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayerWeapon, m_iSecondaryAmmoType, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayerWeapon, m_iClip, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayerWeapon, m_iDefaultAmmo, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CBasePlayerWeapon, CBasePlayerItem)

void CBasePlayerItem::SetObjectCollisionBox()
{
	pev->absmin = pev->origin + Vector(-24, -24, 0);
	pev->absmax = pev->origin + Vector(24, 24, 16);
}

// Sets up movetype, size, solidtype for a new weapon.
void CBasePlayerItem::FallInit()
{
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_BBOX;

	UTIL_SetOrigin(pev, pev->origin);

	// pointsize until it lands on the ground.
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	SetTouch(&CBasePlayerItem::DefaultTouch);
	SetThink(&CBasePlayerItem::FallThink);

	pev->nextthink = gpGlobals->time + 0.1f;
}

// FallThink - Items that have just spawned run this think
// to catch them when they hit the ground. Once we're sure
// that the object is grounded, we change its solid type
// to trigger and set it in a large box that helps the
// player get it.
void CBasePlayerItem::FallThink()
{
	pev->nextthink = gpGlobals->time + 0.1f;

	if (pev->flags & FL_ONGROUND)
	{
		// clatter if we have an owner (i.e., dropped by someone)
		// don't clatter if the gun is waiting to respawn (if it's waiting, it is invisible!)
		if (!FNullEnt(pev->owner))
		{
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "items/weapondrop1.wav", VOL_NORM, ATTN_NORM, 0, RANDOM_LONG(0, 29) + 95);
		}

		// lie flat
		pev->angles.x = 0.0f;
		pev->angles.z = 0.0f;

		Materialize();
	}
}

// Materialize - make a CBasePlayerItem visible and tangible
void CBasePlayerItem::Materialize()
{
	if (pev->effects & EF_NODRAW)
	{
		// changing from invisible state to visible.
		if (g_pGameRules->IsMultiplayer())
		{
			EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "items/suitchargeok1.wav", VOL_NORM, ATTN_NORM, 0, 150);
		}

		pev->effects &= ~EF_NODRAW;
		pev->effects |= EF_MUZZLEFLASH;
	}

	pev->solid = SOLID_TRIGGER;

	// link into world.
	UTIL_SetOrigin(pev, pev->origin);
	SetTouch(&CBasePlayerItem::DefaultTouch);

	if (g_pGameRules->IsMultiplayer())
	{
		if (!CanDrop())
		{
			SetTouch(nullptr);
		}

		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time + 1.0f;
	}
	else
	{
		SetThink(nullptr);
	}
}

// whats going on here is that if the player drops this weapon, they shouldn't take it back themselves
// for a little while. But if they throw it at someone else, the other player should get it immediately.
void CBasePlayerItem::DefaultTouch(CBaseEntity *pOther)
{
	// if it's not a player, ignore
	if (!pOther->IsPlayer())
	{
		return;
	}

	CBasePlayer *pPlayer = static_cast<CBasePlayer *>(pOther);

	// can I have this?
	if (!CSGameRules()->CanHavePlayerItem(pPlayer, m_iId, false))
	{
		return;
	}

	if (pOther->AddPlayerItem(this))
	{
		AttachToPlayer(pPlayer);

		EMIT_SOUND(ENT(pPlayer->pev), CHAN_ITEM, "items/gunpickup2.wav", VOL_NORM, ATTN_NORM);
	}

	SUB_UseTargets(pOther, USE_TOGGLE, 0);
}

void CBasePlayerWeapon::SetPlayerShieldAnim()
{
	if (!m_pPlayer->HasShield())
		return;

	if (m_iWeaponState & WPNSTATE_SHIELD_DRAWN)
	{
		Q_strcpy(m_pPlayer->m_szAnimExtention, "shield");
	}
	else
	{
		Q_strcpy(m_pPlayer->m_szAnimExtention, "shieldgun");
	}
}

void CBasePlayerWeapon::ResetPlayerShieldAnim()
{
	if (m_pPlayer->HasShield())
	{
		if (m_iWeaponState & WPNSTATE_SHIELD_DRAWN)
		{
			Q_strcpy(m_pPlayer->m_szAnimExtention, "shieldgun");
		}
	}
}

void CBasePlayerWeapon::EjectBrassLate()
{
	int soundType;
	Vector vecUp, vecRight, vecShellVelocity;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	vecUp = RANDOM_FLOAT(100, 150) * gpGlobals->v_up;
	vecRight = RANDOM_FLOAT(50, 70) * gpGlobals->v_right;

	vecShellVelocity = (m_pPlayer->pev->velocity + vecRight + vecUp) + gpGlobals->v_forward * 25;
	soundType = (m_iId == WEAPON_STRIKER || m_iId == WEAPON_KSG12) ? 2 : 1;

	EjectBrass(pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -9 + gpGlobals->v_forward * 16, gpGlobals->v_right * -9,
		vecShellVelocity, pev->angles.y, m_iShellId, soundType, m_pPlayer->entindex());
}

bool CBasePlayerWeapon::ShieldSecondaryFire(int iUpAnim, int iDownAnim)
{
	if (!m_pPlayer->HasShield())
		return false;

	if (m_iWeaponState & WPNSTATE_SHIELD_DRAWN)
	{
		m_iWeaponState &= ~WPNSTATE_SHIELD_DRAWN;
		SendWeaponAnim(iDownAnim, UseDecrement() != FALSE);
		Q_strcpy(m_pPlayer->m_szAnimExtention, "shieldgun");
		m_fMaxSpeed = 250.0f;
		m_pPlayer->m_bShieldDrawn = false;
	}
	else
	{
		m_iWeaponState |= WPNSTATE_SHIELD_DRAWN;
		SendWeaponAnim(iUpAnim, UseDecrement() != FALSE);
		Q_strcpy(m_pPlayer->m_szAnimExtention, "shielded");
		m_fMaxSpeed = 180.0f;
		m_pPlayer->m_bShieldDrawn = true;
	}

	m_pPlayer->UpdateShieldCrosshair((m_iWeaponState & WPNSTATE_SHIELD_DRAWN) != WPNSTATE_SHIELD_DRAWN);
	m_pPlayer->ResetMaxSpeed();

	m_flNextSecondaryAttack = 0.4f;
	m_flNextPrimaryAttack = 0.4f;
	m_flTimeWeaponIdle = 0.6f;

	return true;
}

void CBasePlayerWeapon::KickBack(float up_base, float lateral_base, float up_modifier, float lateral_modifier, float up_max, float lateral_max, int direction_change)
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
		flKickUp = m_iShotsFired * up_modifier + up_base;
		flKickLateral = m_iShotsFired * lateral_modifier + lateral_base;
	}

	m_pPlayer->pev->punchangle.x -= flKickUp;

	if (m_pPlayer->pev->punchangle.x < -up_max)
	{
		m_pPlayer->pev->punchangle.x = -up_max;
	}

	if (m_iDirection == 1)
	{
		m_pPlayer->pev->punchangle.y += flKickLateral;

		if (m_pPlayer->pev->punchangle.y > lateral_max)
			m_pPlayer->pev->punchangle.y = lateral_max;
	}
	else
	{
		m_pPlayer->pev->punchangle.y -= flKickLateral;

		if (m_pPlayer->pev->punchangle.y < -lateral_max)
			m_pPlayer->pev->punchangle.y = -lateral_max;
	}

	if (!RANDOM_LONG(0, direction_change))
	{
		m_iDirection = !m_iDirection;
	}
}

void CBasePlayerWeapon::FireRemaining(int &shotsFired, float &shootTime, BOOL bIsGlock)
{
	float nexttime = 0.1f;
	if (--m_iClip < 0)
	{
		m_iClip = 0;
		shotsFired = 3;
		shootTime = 0;
		return;
	}

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir;

	int flag;
#ifdef CLIENT_WEAPONS
	flag = FEV_NOTHOST;
#else
	flag = 0;
#endif

	if (bIsGlock)
	{
		vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, 0.05, 8192, 1, BULLET_PLAYER_9MM, 18, 0.9, m_pPlayer->pev, true, m_pPlayer->random_seed);
		--m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType];

		PLAYBACK_EVENT_FULL(flag, m_pPlayer->edict(), m_usFireGlock18, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
			int(m_pPlayer->pev->punchangle.x * 10000), int(m_pPlayer->pev->punchangle.y * 10000), m_iClip == 0, FALSE);
	}
	else
	{

		vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, m_fBurstSpread, 8192, 2, BULLET_PLAYER_556MM, 30, 0.96, m_pPlayer->pev, false, m_pPlayer->random_seed);
		--m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType];

		PLAYBACK_EVENT_FULL(flag, m_pPlayer->edict(), m_usFireFamas, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
			int(m_pPlayer->pev->punchangle.x * 10000000), int(m_pPlayer->pev->punchangle.y * 10000000), FALSE, FALSE);
	}

	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	if (++shotsFired != 3)
	{
		shootTime = gpGlobals->time + nexttime;
	}
	else
		shootTime = 0;
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

bool CBasePlayerWeapon::HasSecondaryAttack()
{
	if (m_pPlayer && m_pPlayer->HasShield())
	{
		return true;
	}

	switch (m_iId)
	{
	case WEAPON_AK47:
	case WEAPON_STRIKER:
	case WEAPON_PM9:
	case WEAPON_P99:
	case WEAPON_FIVESEVEN:
	case WEAPON_MP5N:
	case WEAPON_UMP45:
	case WEAPON_MK46:
	case WEAPON_KSG12:
	case WEAPON_MP7A1:
	case WEAPON_DEAGLE:
	case WEAPON_ANACONDA:
	case WEAPON_P90:
	case WEAPON_C4:
	case WEAPON_CM901:
		return false;

	default:
		break;
	}

	return true;
}

void CBasePlayerWeapon::HandleInfiniteAmmo()
{
	int nInfiniteAmmo = 0;

	if (!nInfiniteAmmo)
		nInfiniteAmmo = static_cast<int>(infiniteAmmo.value);

	if (nInfiniteAmmo == 1 && !IsGrenadeWeapon(m_iId))	// assuming WPNMODE_INFINITE_CLIP == 1
	{
		m_iClip = iinfo()->m_iMaxClip;
	}
	else if (nInfiniteAmmo == 2 || (IsGrenadeWeapon(m_iId) && infiniteGrenades.value == 1.0f))	// assuming WPNMODE_INFINITE_BPAMMO == 2
	{
		if (m_iPrimaryAmmoType > 0)
		{
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = GetAmmoInfo(m_iPrimaryAmmoType)->m_iMax;
		}
	}
}

void CBasePlayerWeapon::ItemPostFrame()
{
	int usableButtons = m_pPlayer->pev->button;

	if (!HasSecondaryAttack())
	{
		usableButtons &= ~IN_ATTACK2;
	}

	if (m_flGlock18Shoot != 0)
	{
		FireRemaining(m_iGlock18ShotsFired, m_flGlock18Shoot, TRUE);
	}
	else if (gpGlobals->time > m_flFamasShoot && m_flFamasShoot != 0)
	{
		FireRemaining(m_iFamasShotsFired, m_flFamasShoot, FALSE);
	}

	// Return zoom level back to previous zoom level before we fired a shot.
	// This is used only for the AWP and Scout
	if (m_flNextPrimaryAttack <= UTIL_WeaponTimeBase())
	{
		if (m_pPlayer->m_bResumeZoom)
		{
			m_pPlayer->m_iFOV = m_pPlayer->m_iLastZoom;
			m_pPlayer->pev->fov = m_pPlayer->m_iFOV;

			if (m_pPlayer->m_iFOV == m_pPlayer->m_iLastZoom)
			{
				// return the fade level in zoom.
				m_pPlayer->m_bResumeZoom = false;
			}
		}
	}

	if (m_pPlayer->m_flEjectBrass != 0 && m_pPlayer->m_flEjectBrass <= gpGlobals->time)
	{
		m_pPlayer->m_flEjectBrass = 0;
		EjectBrassLate();
	}

	if (!(m_pPlayer->pev->button & IN_ATTACK))
	{
		m_flLastFireTime = 0;
	}

	if (m_pPlayer->HasShield())
	{
		if (m_fInReload && (m_pPlayer->pev->button & IN_ATTACK2))
		{
			SecondaryAttack();
			m_pPlayer->pev->button &= ~IN_ATTACK2;
			m_fInReload = FALSE;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase();
		}
	}

	if (m_fInReload && m_pPlayer->m_flNextAttack <= UTIL_WeaponTimeBase())
	{
		// complete the reload.
		int j = Q_min(iinfo()->m_iMaxClip - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);

		// Add them to the clip
		m_iClip += j;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= j;

		m_fInReload = FALSE;
	}

	if ((usableButtons & IN_ATTACK2) && CanAttack(m_flNextSecondaryAttack, UTIL_WeaponTimeBase(), UseDecrement()))
	{
		SecondaryAttack();
		m_pPlayer->pev->button &= ~IN_ATTACK2;
	}
	else if ((m_pPlayer->pev->button & IN_ATTACK) && CanAttack(m_flNextPrimaryAttack, UTIL_WeaponTimeBase(), UseDecrement()))
	{
		if ((m_iClip <= 0 && m_iPrimaryAmmoType > 0) || (iinfo()->m_iMaxClip == WEAPON_NOCLIP && !m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]))
		{
			m_fFireOnEmpty = TRUE;
		}

		// Can't shoot during the freeze period
		// Always allow firing in single player
		if ((m_pPlayer->m_bCanShoot && g_pGameRules->IsMultiplayer() && !g_pGameRules->IsFreezePeriod()) || !g_pGameRules->IsMultiplayer())
		{
			PrimaryAttack();
			m_flNextPrimaryAttack *= m_pPlayer->WeaponFireIntervalModifier(this);	// passive skill applied.
		}
	}
	else if ((m_pPlayer->pev->button & IN_RELOAD) && iinfo()->m_iMaxClip != WEAPON_NOCLIP && !m_fInReload && m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		if (m_flFamasShoot == 0 && m_flGlock18Shoot == 0)
		{
			if (!(m_iWeaponState & WPNSTATE_SHIELD_DRAWN))
			{
				// reload when reload is pressed, or if no buttons are down and weapon is empty.
				Reload();
			}
		}
	}
	else if (!(usableButtons & (IN_ATTACK | IN_ATTACK2)))
	{
		// no fire buttons down

		// The following code prevents the player from tapping the firebutton repeatedly
		// to simulate full auto and retaining the single shot accuracy of single fire
		if (m_bDelayFire)
		{
			m_bDelayFire = false;

			if (m_iShotsFired > 15)
			{
				m_iShotsFired = 15;
			}

			m_flDecreaseShotsFired = gpGlobals->time + 0.4f;
		}

		m_fFireOnEmpty = FALSE;

		// if it's a pistol then set the shots fired to 0 after the player releases a button
		if (IsSecondaryWeapon(m_iId))
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

		if (!IsUseable() && m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
		{
#if 0
			// weapon isn't useable, switch.
			if (!(iFlags() & ITEM_FLAG_NOAUTOSWITCHEMPTY) && g_pGameRules->GetNextBestWeapon(m_pPlayer, this))
			{
				m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.3f;
				return;
			}
#endif
		}
		else
		{
			if (!(m_iWeaponState & WPNSTATE_SHIELD_DRAWN))
			{
				// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
				if (!m_iClip && !(iinfo()->m_bitsFlags & ITEM_FLAG_NOAUTORELOAD) && m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
				{
					if (m_flFamasShoot == 0 && m_flGlock18Shoot == 0)
					{
						Reload();
						return;
					}
				}
			}
		}

		HandleInfiniteAmmo();

		WeaponIdle();
		return;
	}

	HandleInfiniteAmmo();

	// catch all
	if (ShouldWeaponIdle())
	{
		WeaponIdle();
	}
}

void CBasePlayerItem::DestroyItem()
{
	if (m_pPlayer)
	{
		// if attached to a player, remove.
		if (m_pPlayer->RemovePlayerItem(this))
		{
			m_pPlayer->pev->weapons &= ~(1 << m_iId);

			// No more weapon
			if ((m_pPlayer->pev->weapons & ~(1 << WEAPON_SUIT)) == 0)
			{
				m_pPlayer->m_iHideHUD |= HIDEHUD_WEAPONS;
			}
		}
	}

	Kill();
}

int CBasePlayerItem::AddToPlayer(CBasePlayer *pPlayer)
{
	m_pPlayer = pPlayer;

	MESSAGE_BEGIN(MSG_ONE, gmsgWeapPickup, nullptr, pPlayer->pev);
		WRITE_BYTE(m_iId);
	MESSAGE_END();

	return TRUE;
}

int CBasePlayerItem::GetItemInfo(ItemInfo* p)
{
	if (m_iId < 1 || m_iId >= LAST_WEAPON)
		return FALSE;

	Q_memcpy(p, &m_rgItemInfo[m_iId], sizeof(ItemInfo));
	return TRUE;
}

void CBasePlayerItem::Drop()
{
	SetTouch(nullptr);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CBasePlayerItem::Kill()
{
	SetTouch(nullptr);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CBasePlayerItem::Holster(int skiplocal)
{
	m_pPlayer->pev->viewmodel = 0;
	m_pPlayer->pev->weaponmodel = 0;
}

void CBasePlayerItem::AttachToPlayer(CBasePlayer *pPlayer)
{
	pev->movetype = MOVETYPE_FOLLOW;
	pev->solid = SOLID_NOT;
	pev->aiment = pPlayer->edict();
	pev->effects = EF_NODRAW;

	// server won't send down to clients if modelindex == 0
	pev->modelindex = 0;
	pev->model = 0;
	pev->owner = pPlayer->edict();

	// Remove think - prevents futher attempts to materialize
	pev->nextthink = 0;
	SetThink(nullptr);

	SetTouch(nullptr);
}

// CALLED THROUGH the newly-touched weapon's instance. The existing player weapon is pOriginal
int CBasePlayerWeapon::AddDuplicate(CBasePlayerItem *pOriginal)
{
	if (m_iPrimaryAmmoType <= 0)	// make sure we have a ammotype to give ammo.
	{
		m_iPrimaryAmmoType = iinfo()->m_iAmmoType;
		m_iSecondaryAmmoType = AMMO_NONE;
	}

	if (m_iDefaultAmmo)
	{
		return ExtractAmmo((CBasePlayerWeapon *)pOriginal);
	}
	else
	{
		// a dead player dropped this.
		return ExtractClipAmmo((CBasePlayerWeapon *)pOriginal);
	}
}

int CBasePlayerWeapon::AddToPlayer(CBasePlayer *pPlayer)
{
	m_pPlayer = pPlayer;
	pPlayer->pev->weapons |= (1 << m_iId);

	if (m_iPrimaryAmmoType <= 0)	// make sure we have a ammotype to give ammo.
	{
		m_iPrimaryAmmoType = iinfo()->m_iAmmoType;
		m_iSecondaryAmmoType = AMMO_NONE;
	}

	return CBasePlayerItem::AddToPlayer(pPlayer);
}

int CBasePlayerWeapon::UpdateClientData(CBasePlayer *pPlayer)
{
	bool bSend = false;
	int state = 0;

	if (pPlayer->m_pActiveItem == this)
	{
		if (pPlayer->m_fOnTarget)
			state = WEAPON_IS_ONTARGET;
		else
			state = 1;
	}

	if (!pPlayer->m_fWeapon)
		bSend = true;

	if (this == pPlayer->m_pActiveItem || this == pPlayer->m_pClientActiveItem)
	{
		if (pPlayer->m_pActiveItem != pPlayer->m_pClientActiveItem)
			bSend = true;
	}

	if (m_iClip != m_iClientClip || state != m_iClientWeaponState || pPlayer->m_iFOV != pPlayer->m_iClientFOV)
		bSend = true;

	if (bSend)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgCurWeapon, nullptr, pPlayer->pev);
			WRITE_BYTE(state);
			WRITE_BYTE(m_iId);
			WRITE_BYTE(m_iClip);
		MESSAGE_END();

		m_iClientClip = m_iClip;
		m_iClientWeaponState = state;
		pPlayer->m_fWeapon = TRUE;
	}

	if (m_pNext)
	{
		m_pNext->UpdateClientData(pPlayer);
	}

	return 1;
}

void CBasePlayerWeapon::SendWeaponAnim(int iAnim, int skiplocal)
{
	m_pPlayer->pev->weaponanim = iAnim;

#ifdef CLIENT_WEAPONS
	if (skiplocal && ENGINE_CANSKIP(m_pPlayer->edict()))
		return;
#endif

	MESSAGE_BEGIN(MSG_ONE, SVC_WEAPONANIM, nullptr, m_pPlayer->pev);
		WRITE_BYTE(iAnim);		// sequence number
		WRITE_BYTE(pev->body);	// weaponmodel bodygroup.
	MESSAGE_END();
}

bool CBasePlayerWeapon::AddPrimaryAmmo(int iCount)
{
	if (m_iPrimaryAmmoType <= 0 || m_iPrimaryAmmoType >= AMMO_MAXTYPE)	// wrongful ammo type!
		return false;

	bool bGotAmmo = false;

	if (iinfo()->m_iMaxClip < 1)
	{
		m_iClip = WEAPON_NOCLIP;
		bGotAmmo = m_pPlayer->GiveAmmo(iCount, (AmmoIdType)m_iPrimaryAmmoType);
	}
	else if (m_iClip <= 0)
	{
		int i;
		i = Q_min(m_iClip + iCount, iinfo()->m_iMaxClip);
		m_iClip += i;

		bGotAmmo = m_pPlayer->GiveAmmo(iCount - i, (AmmoIdType)m_iPrimaryAmmoType);
	}
	else
	{
		bGotAmmo = m_pPlayer->GiveAmmo(iCount, (AmmoIdType)m_iPrimaryAmmoType);
	}

	if (bGotAmmo)
	{
		if (m_pPlayer->HasPlayerItem((WeaponIdType)m_iId))
		{
			// play the "got ammo" sound only if we gave some ammo to a player that already had this gun.
			// if the player is just getting this gun for the first time, DefaultTouch will play the "picked up gun" sound for us.
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", VOL_NORM, ATTN_NORM);
		}
	}

	return bGotAmmo;
}

// IsUseable - this function determines whether or not a
// weapon is useable by the player in its current state.
// (does it have ammo loaded? do I have any ammo for the
// weapon?, etc)
BOOL CBasePlayerWeapon::IsUseable()
{
	if (m_iClip <= 0)
	{
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 && m_iPrimaryAmmoType > 0)
		{
			// clip is empty (or nonexistant) and the player has no more ammo of this type.
			return FALSE;
		}
	}

	return TRUE;
}

BOOL EXT_FUNC CBasePlayerWeapon::CanDeploy()
{
	return TRUE;
}

BOOL EXT_FUNC CBasePlayerWeapon::DefaultDeploy(char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt, int skiplocal)
{
	if (!CanDeploy())
		return FALSE;

	m_pPlayer->pev->viewmodel = MAKE_STRING(szViewModel);
	m_pPlayer->pev->weaponmodel = MAKE_STRING(szWeaponModel);

	model_name = m_pPlayer->pev->viewmodel;
	Q_strcpy(m_pPlayer->m_szAnimExtention, szAnimExt);
	SendWeaponAnim(iAnim, skiplocal);

	m_pPlayer->m_flNextAttack = 0.75f;
	m_flTimeWeaponIdle = 1.5f;
	m_flLastFireTime = 0.0f;
	m_flDecreaseShotsFired = gpGlobals->time;

	m_pPlayer->m_iFOV = DEFAULT_FOV;
	m_pPlayer->pev->fov = DEFAULT_FOV;
	m_pPlayer->m_iLastZoom = DEFAULT_FOV;
	m_pPlayer->m_bResumeZoom = false;

	return TRUE;
}

void CBasePlayerWeapon::ReloadSound()
{
	CBasePlayer *pPlayer = nullptr;
	while ((pPlayer = UTIL_FindEntityByClassname(pPlayer, "player")))
	{
		if (pPlayer->IsDormant())
			break;

		if (pPlayer == m_pPlayer)
			continue;

		float distance = (m_pPlayer->pev->origin - pPlayer->pev->origin).Length();
		if (distance <= MAX_DIST_RELOAD_SOUND)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgReloadSound, nullptr, pPlayer->pev);
				WRITE_BYTE(int((1.0f - (distance / MAX_DIST_RELOAD_SOUND)) * 255.0f));
			if (!Q_strcmp(STRING(pev->classname), "weapon_m3") || !Q_strcmp(STRING(pev->classname), "weapon_xm1014"))
				WRITE_BYTE(0);
			else
				WRITE_BYTE(1);
			MESSAGE_END();
		}
	}
}

int EXT_FUNC CBasePlayerWeapon::DefaultReload(int iClipSize, int iAnim, float fDelay)
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return FALSE;

	int j = Q_min(iClipSize - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);
	if (!j)
	{
		return FALSE;
	}

	m_pPlayer->m_flNextAttack = fDelay;

	ReloadSound();
	SendWeaponAnim(iAnim, UseDecrement() ? 1 : 0);

	m_fInReload = TRUE;
	m_flTimeWeaponIdle = fDelay + 0.5f;

	return TRUE;
}

bool EXT_FUNC CBasePlayerWeapon::DefaultShotgunReload(int iAnim, int iStartAnim, float fDelay, float fStartDelay, const char *pszReloadSound1, const char *pszReloadSound2)
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip >= iinfo()->m_iMaxClip)
		return false;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return false;

	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
		SendWeaponAnim(iStartAnim, UseDecrement() != FALSE);

		m_fInSpecialReload = 1;
		m_flNextSecondaryAttack = m_flTimeWeaponIdle = m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + fStartDelay;
		m_flNextPrimaryAttack = GetNextAttackDelay(fStartDelay);
	}
	else if (m_fInSpecialReload == 1)
	{
		if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return false;

		// was waiting for gun to move to side
		m_fInSpecialReload = 2;

		const char *pszReloadSound = nullptr;
		if (pszReloadSound1 && pszReloadSound2) pszReloadSound = RANDOM_LONG(0, 1) ? pszReloadSound1 : pszReloadSound2;
		else if (pszReloadSound1)               pszReloadSound = pszReloadSound1;
		else if (pszReloadSound2)               pszReloadSound = pszReloadSound2;

		if (pszReloadSound && pszReloadSound[0] != '\0')
		{
			EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_ITEM, pszReloadSound, VOL_NORM, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 31));
		}

		SendWeaponAnim(iAnim, UseDecrement());

		m_flTimeWeaponIdle = m_flNextReload = UTIL_WeaponTimeBase() + fDelay;
	}
	else if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
	{
		m_iClip++;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
		m_fInSpecialReload = 1;
	}

	return true;
}

BOOL CBasePlayerWeapon::PlayEmptySound()
{
	if (m_iPlayEmptySound)
	{
		switch (m_iId)
		{
		case WEAPON_USP:
		case WEAPON_GLOCK18:
		case WEAPON_ANACONDA:
		case WEAPON_DEAGLE:
		case WEAPON_P99:
		case WEAPON_FIVESEVEN:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dryfire_pistol.wav", 0.8, ATTN_NORM);
			break;
		default:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dryfire_rifle.wav", 0.8, ATTN_NORM);
			break;
		}
	}

	return FALSE;
}

void CBasePlayerWeapon::ResetEmptySound()
{
	m_iPlayEmptySound = 1;
}

void CBasePlayerWeapon::Holster(int skiplocal)
{
	// cancel any reload in progress.
	m_fInReload = FALSE;
	m_pPlayer->pev->viewmodel = 0;
	m_pPlayer->pev->weaponmodel = 0;
}

// called by the new item with the existing item as parameter
//
// if we call ExtractAmmo(), it's because the player is picking up this type of weapon for
// the first time. If it is spawned by the world, m_iDefaultAmmo will have a default ammo amount in it.
// if  this is a weapon dropped by a dying player, has 0 m_iDefaultAmmo, which means only the ammo in
// the weapon clip comes along.
int CBasePlayerWeapon::ExtractAmmo(CBasePlayerWeapon* pWeapon)
{
	// blindly call with m_iDefaultAmmo. It's either going to be a value or zero. If it is zero,
	// we only get the ammo in the weapon's clip, which is what we want.

	int res = pWeapon->AddPrimaryAmmo(m_iDefaultAmmo);
	m_iDefaultAmmo = 0;

	return res;
}

// called by the new item's class with the existing item as parameter
int CBasePlayerWeapon::ExtractClipAmmo(CBasePlayerWeapon *pWeapon)
{
	int iAmmo;
	if (m_iClip == WEAPON_NOCLIP)
	{
		// guns with no clips always come empty if they are second-hand
		iAmmo = 0;
	}
	else
	{
		iAmmo = m_iClip;
	}

	return pWeapon->m_pPlayer->GiveAmmo(iAmmo, (AmmoIdType)pWeapon->m_iPrimaryAmmoType);
}

// RetireWeapon - no more ammo for this gun, put it away.
void CBasePlayerWeapon::RetireWeapon()
{
	// first, no viewmodel at all.
	m_pPlayer->pev->viewmodel = iStringNull;
	m_pPlayer->pev->weaponmodel = iStringNull;

	g_pGameRules->GetNextBestWeapon(m_pPlayer, this);
}

// GetNextAttackDelay - An accurate way of calcualting the next attack time.
float CBasePlayerWeapon::GetNextAttackDelay(float delay)
{
	float flNextAttack = UTIL_WeaponTimeBase() + delay;

	// save the last fire time
	m_flLastFireTime = gpGlobals->time;

	// we need to remember what the m_flNextPrimaryAttack time is set to for each shot,
	// store it as m_flPrevPrimaryAttack.
	m_flPrevPrimaryAttack = flNextAttack - UTIL_WeaponTimeBase();

	return flNextAttack;
}

// true - keep the amount of bpammo
// false - let take away bpammo
void CBasePlayerWeapon::InstantReload(bool bCanRefillBPAmmo)
{
	// if you already reload
	//if (m_fInReload)
	//	return;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		return;

	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = 0;

	// complete the reload.
	int j = Q_min(iinfo()->m_iMaxClip - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);
	if (j == 0)
		return;

	// Add them to the clip
	m_iClip += j;

	if (!bCanRefillBPAmmo)
	{
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= j;
	}
}

TYPEDESCRIPTION CWeaponBox::m_SaveData[] =
{
	DEFINE_ARRAY(CWeaponBox, m_rgAmmo, FIELD_INTEGER, MAX_AMMO_SLOTS),
	DEFINE_ARRAY(CWeaponBox, m_rgpPlayerItems, FIELD_CLASSPTR, MAX_ITEM_TYPES),
};

LINK_ENTITY_TO_CLASS(weaponbox, CWeaponBox)
IMPLEMENT_SAVERESTORE(CWeaponBox, CBaseEntity)

void CWeaponBox::Precache()
{
	PRECACHE_MODEL("models/w_weaponbox.mdl");
	PRECACHE_SOUND(SFX_REFUND_GUNS);
	PRECACHE_SOUND("items/9mmclip1.wav");
}

void CWeaponBox::KeyValue(KeyValueData *pkvd)
{
	if (GiveAmmo(Q_atoi(pkvd->szValue), (AmmoIdType)GetAmmoInfo(pkvd->szKeyName)->m_iId))
		pkvd->fHandled = TRUE;
}

void CWeaponBox::SetModel(const char *pszModelName)
{
	SET_MODEL(ENT(pev), pszModelName);
}

void CWeaponBox::Spawn()
{
	Precache();

	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;

	UTIL_SetSize(pev, g_vecZero, g_vecZero);
	SET_MODEL(ENT(pev), "models/w_weaponbox.mdl");
}

// The think function that removes the box from the world.
void CWeaponBox::Kill()
{
	// destroy the weapons
	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		CBasePlayerItem *pWeapon = m_rgpPlayerItems[i];
		while (pWeapon)
		{
			pWeapon->SetThink(&CBaseEntity::SUB_Remove);
			pWeapon->pev->nextthink = gpGlobals->time + 0.1f;
			pWeapon = pWeapon->m_pNext;
		}
	}

	// remove the box
	UTIL_Remove(this);
}

// Try to add my contents to the toucher if the toucher is a player.
void CWeaponBox::Touch(CBaseEntity *pOther)
{
	if (!(pev->flags & FL_ONGROUND))
	{
		return;
	}

	if (!pOther->IsPlayer())
	{
		// only players may touch a weaponbox.
		return;
	}

	if (!pOther->IsAlive())
	{
		// no dead guys.
		return;
	}

	CBasePlayer *pPlayer = static_cast<CBasePlayer *>(pOther);

	if (pPlayer->m_bShieldDrawn)
		return;

	bool bRemove = true;
	bool bEmitSound = false;

	// go through my weapons and try to give the usable ones to the player.
	// it's important the the player be given ammo first, so the weapons code doesn't refuse
	// to deploy a better weapon that the player may pick up because he has no ammo for it.
	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		if (!m_rgpPlayerItems[i])
			continue;

		CBasePlayerItem *pItem = m_rgpPlayerItems[i];

		// have at least one weapon in this slot
		while (pItem)
		{
			if (!CSGameRules()->CanHavePlayerItem(pPlayer, pItem->m_iId, false)
				|| (pPlayer->IsBot() && TheCSBots() && !TheCSBots()->IsWeaponUseable(pItem)))
			{
				return;
			}

			if (i >= PRIMARY_WEAPON_SLOT && i <= PISTOL_SLOT && pPlayer->m_rgpPlayerItems[i])
			{
				// ...
			}
			else if (i == GRENADE_SLOT)
			{
				CBasePlayerWeapon *pGrenade = static_cast<CBasePlayerWeapon *>(m_rgpPlayerItems[i]);
				if (pGrenade && pGrenade->IsWeapon())
				{
					int playerGrenades = pPlayer->m_rgAmmo[pGrenade->m_iPrimaryAmmoType];
					auto info = GetAmmoInfo(pGrenade->m_iPrimaryAmmoType);

					if (info && playerGrenades < info->m_iMax)
					{
						auto pNext = m_rgpPlayerItems[i]->m_pNext;
						if (pPlayer->AddPlayerItem(pItem))
						{
							pItem->AttachToPlayer(pPlayer);
							bEmitSound = true;
						}

						// unlink this weapon from the box
						m_rgpPlayerItems[i] = pItem = pNext;
						continue;
					}
				}
			}
			else if (pPlayer->HasShield() && i == PRIMARY_WEAPON_SLOT)
			{
				// ...
			}
			else
			{
				auto pNext = m_rgpPlayerItems[i]->m_pNext;
				if (pPlayer->AddPlayerItem(pItem))
				{
					pItem->AttachToPlayer(pPlayer);
					bEmitSound = true;
				}

				// unlink this weapon from the box
				m_rgpPlayerItems[i] = pItem = pNext;
				continue;
			}

			bRemove = false;
			pItem = m_rgpPlayerItems[i]->m_pNext;
		}
	}

	if (bRemove)
	{
		// dole out ammo
		for (int n = 0; n < MAX_AMMO_SLOTS; n++)
		{
			// there's some ammo of this type.
			pPlayer->GiveAmmo(m_rgAmmo[n], (AmmoIdType)n);

			// now empty the ammo from the weaponbox since we just gave it to the player
			m_rgAmmo[n] = 0;
		}
	}

	if (bEmitSound)
	{
		EMIT_SOUND(ENT(pPlayer->pev), CHAN_ITEM, "items/gunpickup2.wav", VOL_NORM, ATTN_NORM);
	}

	if (bRemove)
	{
		SetTouch(nullptr);
		UTIL_Remove(this);
	}
}

void CWeaponBox::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	// theoretically, pActivator and pCaller are the same.
	if (FNullEnt(pActivator) || FNullEnt(pCaller) || !pActivator->IsPlayer() || !pCaller->IsPlayer())
		return;

	CBasePlayer* pPlayer = CBasePlayer::Instance(pActivator->pev);

	if (pPlayer->m_iRoleType != Role_Commander)	// only commander may sold weapon.
		return;

	if (m_bHadBeenSold)	// this box had been sold!
		return;

	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		CBasePlayerItem* pWeapon = m_rgpPlayerItems[i];

		while (pWeapon)
		{
			auto pWpnInfo = GetWeaponInfo(pWeapon->m_iId);

			if (pWpnInfo && !pWeapon->m_bHadBeenSold)
			{
				pPlayer->AddAccount(pWpnInfo->m_iCost / 2, RT_SOLD_ITEM);
				UTIL_PrintChatColor(pPlayer, BLUECHAT, "/gSeizing weapon /t%s/g have you rewarded with /t%d/g$", WeaponIDToAlias(pWeapon->m_iId), pWpnInfo->m_iCost / 2);
				EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, SFX_REFUND_GUNS, VOL_NORM, ATTN_NORM);

				pWeapon->m_bHadBeenSold = true;
			}

			pWeapon = pWeapon->m_pNext;
		}
	}

	pev->nextthink = gpGlobals->time + 0.01f;
	m_bHadBeenSold = true;
}

// Add this weapon to the box
bool CWeaponBox::PackWeapon(CBasePlayerItem *pWeapon)
{
	// is one of these weapons already packed in this box?
	if (HasWeapon(pWeapon))
	{
		// box can only hold one of each weapon type
		return false;
	}

	if (pWeapon->m_pPlayer)
	{
		if (pWeapon->m_pPlayer->m_pActiveItem == pWeapon)
		{
			pWeapon->Holster();
		}

		if (!pWeapon->m_pPlayer->RemovePlayerItem(pWeapon))
		{
			// failed to unhook the weapon from the player!
			return false;
		}
	}

	int iWeaponSlot = pWeapon->iinfo()->m_iSlot;
	if (m_rgpPlayerItems[iWeaponSlot])
	{
		// there's already one weapon in this slot, so link this into the slot's column
		pWeapon->m_pNext = m_rgpPlayerItems[iWeaponSlot];
		m_rgpPlayerItems[iWeaponSlot] = pWeapon;
	}
	else
	{
		// first weapon we have for this slot
		m_rgpPlayerItems[iWeaponSlot] = pWeapon;
		pWeapon->m_pNext = nullptr;
	}

	// never respawn
	pWeapon->pev->spawnflags |= SF_NORESPAWN;
	pWeapon->pev->movetype = MOVETYPE_NONE;
	pWeapon->pev->solid = SOLID_NOT;
	pWeapon->pev->effects = EF_NODRAW;
	pWeapon->pev->modelindex = 0;
	pWeapon->pev->model = 0;
	pWeapon->pev->owner = ENT(pev);
	pWeapon->SetThink(nullptr);
	pWeapon->SetTouch(nullptr);
	pWeapon->m_pPlayer = nullptr;

	return true;
}

bool CWeaponBox::GiveAmmo(int iCount, AmmoIdType iId)
{
	if (iId <= 0 || iId >= AMMO_MAXTYPE)
		return false;

	m_rgAmmo[iId] += iCount;
	return true;
}

// Is a weapon of this type already packed in this box?
bool CWeaponBox::HasWeapon(CBasePlayerItem *pCheckItem)
{
	CBasePlayerItem *pItem = m_rgpPlayerItems[pCheckItem->iinfo()->m_iSlot];

	while (pItem)
	{
		if (FClassnameIs(pItem->pev, pCheckItem->pev->classname)) {
			return true;
		}

		pItem = pItem->m_pNext;
	}

	return false;
}

// Is there anything in this box?
bool CWeaponBox::IsEmpty()
{
	int i;
	for (i = 0; i < MAX_ITEM_TYPES; i++)
	{
		if (m_rgpPlayerItems[i])
		{
			return false;
		}
	}

	for (i = 0; i < MAX_AMMO_SLOTS; i++)
	{
		if (m_rgAmmo[i] > 0)
		{
			// still have a bit of this type of ammo
			return false;
		}
	}

	return true;
}

void CWeaponBox::SetObjectCollisionBox()
{
	pev->absmin = pev->origin + Vector(-16, -16, 0);
	pev->absmax = pev->origin + Vector(16, 16, 16);
}
