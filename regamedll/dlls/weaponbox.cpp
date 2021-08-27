/*

Created Date: Mar 13 2020

*/

#include "precompiled.h"

LINK_ENTITY_TO_CLASS(weaponbox, CWeaponBox)

void CWeaponBox::Precache()
{
	PRECACHE_MODEL("models/w_weaponbox.mdl");
	PRECACHE_SOUND(SFX_REFUND_GUNS);
	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("debris/metal6.wav");	// SFX when it gets hit.
}

void CWeaponBox::KeyValue(KeyValueData* pkvd)
{
	if (GiveAmmo(Q_atoi(pkvd->szValue), (AmmoIdType)GetAmmoInfo(pkvd->szKeyName)->m_iId))
		pkvd->fHandled = TRUE;
}

void CWeaponBox::SetModel(const char* pszModelName)	// the initial velocity would be set here.
{
	float flSpeed = pev->velocity.Length();
	pev->avelocity.x = flSpeed;
	pev->avelocity.y = RANDOM_FLOAT(-flSpeed, flSpeed);

	CBasePlayer* pPlayer = CBasePlayer::Instance(pev->owner);
	if (pPlayer->IsAlive())
	{
		UTIL_MakeVectors(pPlayer->pev->v_angle);

		Vector vecSrc = pPlayer->GetGunPosition();
		Vector vecEnd = vecSrc + gpGlobals->v_forward * 16.0f;

		TraceResult tr;
		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, dont_ignore_glass, pPlayer->edict(), &tr);

		SET_ORIGIN(edict(), tr.vecEndPos);
		pev->velocity = gpGlobals->v_forward * THROWING_FORCE;
	}
	else
	{
		UTIL_MakeVectors(pPlayer->pev->v_angle);

		Vector vecSrc = pPlayer->GetGunPosition();
		Vector vecEnd = vecSrc + gpGlobals->v_forward * 28.0f;

		TraceResult tr;
		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, dont_ignore_glass, pPlayer->edict(), &tr);

		SET_ORIGIN(edict(), tr.vecEndPos);
		pev->velocity = g_vecZero;
	}

	SET_MODEL(ENT(pev), pszModelName);
}

void CWeaponBox::Spawn()
{
	Precache();

	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_TRIGGER;
	pev->takedamage = DAMAGE_YES;
	pev->fuser1 = gpGlobals->time;

	UTIL_SetSize(pev, Vector(-8.0, -5.0, -1.0), Vector(8.0, 5.0, 1.0));
	SET_MODEL(ENT(pev), "models/w_weaponbox.mdl");
}

// The think function that removes the box from the world.
void CWeaponBox::Kill()
{
	// destroy the weapons
	// Aug 10 2021, LUNA: Weapons are now stay on client side.

	// remove the box
	UTIL_Remove(this);
}

// Try to add my contents to the toucher if the toucher is a player.
void CWeaponBox::Touch(CBaseEntity* pOther)
{
	if (pev->fuser1 + 1.0f >= gpGlobals->time && pOther->edict() == pev->owner)
		return;

	if (!pOther->IsPlayer())
	{
		if (pOther->pev->solid < SOLID_BBOX)
			return;

		if (!Q_strcmp(pOther->pev->classname, "func_breakable"))
			DispatchUse(pOther->edict(), edict());

		if (pev->velocity.Length() > 1000.0f)
		{
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_SPARKS);
			WRITE_COORD(pev->origin[0] + RANDOM_FLOAT(-10, 10));
			WRITE_COORD(pev->origin[1] + RANDOM_FLOAT(-10, 10));
			WRITE_COORD(pev->origin[2] + RANDOM_FLOAT(-10, 10));
			MESSAGE_END();
		}

		pev->velocity *= 0.4f;
		pev->avelocity.x = 0;
		pev->avelocity.y = RANDOM_FLOAT(-pev->velocity.Length(), pev->velocity.Length());

		UTIL_LieFlat(this);

		if (m_flNextPhysSFX < gpGlobals->time)
		{
			EMIT_SOUND(edict(), CHAN_WEAPON, "items/weapondrop1.wav", 0.25f, ATTN_STATIC);
			m_flNextPhysSFX = gpGlobals->time + 0.2f;
		}

		// only players may pickup a weaponbox.
		return;
	}

	if (!pOther->IsAlive())
	{
		// no dead guys.
		return;
	}

	CBasePlayer* pPlayer = dynamic_cast<CBasePlayer*>(pOther);

	if (pPlayer->m_bShieldDrawn)
		return;

	bool bRemove = true;

	// it's important the the player be given ammo first, so the weapons code doesn't refuse
	// to deploy a better weapon that the player may pick up because he has no ammo for it.

	if (!IWeapon::Give(m_StoredWeapon.what, pPlayer, m_StoredWeapon.clip, m_StoredWeapon.flags))
		bRemove = false;

	if (bRemove)
	{
		// dole out ammo once we confirm that this player can hold this weapon.
		pPlayer->GiveAmmo(m_StoredAmmo.count, m_StoredAmmo.what);

		m_StoredAmmo.count = 0;
		m_StoredAmmo.what = AMMO_NONE;
	}

	if (bRemove)
	{
		SetTouch(nullptr);
		UTIL_Remove(this);
	}

	// player kick a weaponbox.
	if (!bRemove)
	{
		pev->velocity = (pev->origin - pOther->pev->origin).Normalize() * pOther->pev->velocity.Length() * 3.0f;
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

	auto pInfo = GetWeaponInfo(m_StoredWeapon.what);

	pPlayer->AddAccount(pInfo->m_iCost / 2, RT_SOLD_ITEM);
	UTIL_SayText(pPlayer, "#LeaderMod_Commander_Sk_SeizeWpn", WeaponIDToAlias(m_StoredWeapon.what), std::to_string(pInfo->m_iCost / 2).c_str());
	EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, SFX_REFUND_GUNS, VOL_NORM, ATTN_NORM);

	Q_memset(&m_StoredWeapon, NULL, sizeof(m_StoredWeapon));

	pev->nextthink = gpGlobals->time + 0.01f;	// De facto removal.
	m_bHadBeenSold = true;
}

// Add this weapon to the box
bool CWeaponBox::PackWeapon(IWeapon* pWeapon)
{
	// is one of these weapons already packed in this box?
	if (m_StoredWeapon.what != WEAPON_NONE)
	{
		// box can only hold one of each weapon type
		return false;
	}

	auto pPlayer = static_cast<CBot*>(pWeapon->GetOwner());
	assert(pPlayer->IsBot());

	if (pPlayer)
	{
		if (pPlayer->m_pActiveItem == pWeapon)
		{
			pWeapon->Holstered();	// it's dropping weapon. just data-ly holster it.
		}

		if (!pPlayer->RemovePlayerItem(pWeapon->Id()))
		{
			// failed to unhook the weapon from the player!
			return false;
		}
	}

	m_StoredWeapon.what = pWeapon->Id();
	m_StoredWeapon.clip = pWeapon->Clip();
	m_StoredWeapon.flags = pWeapon->Flags();

	if (pPlayer)
	{
		m_StoredAmmo.what = pWeapon->AmmoInfo()->m_iId;
		m_StoredAmmo.count = pPlayer->m_rgAmmo[pWeapon->AmmoInfo()->m_iId];
	}

	pWeapon->Attach(this);
	return true;
}

bool CWeaponBox::GiveAmmo(int iCount, AmmoIdType iId)
{
	if (iId <= 0 || iId >= AMMO_MAXTYPE)
		return false;

	m_StoredAmmo.count += iCount;
	m_StoredAmmo.what = iId;

	return true;
}

// Is there anything in this box?
bool CWeaponBox::IsEmpty()
{
	return m_StoredAmmo.count > 0 || m_StoredWeapon.what > WEAPON_NONE;
}

void CWeaponBox::SetObjectCollisionBox()
{
	pev->absmin = pev->origin + Vector(-16, -16, 0);
	pev->absmax = pev->origin + Vector(16, 16, 16);
}

void CWeaponBox::TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, ptr->vecEndPos);
	WRITE_BYTE(TE_SPARKS);
	WRITE_COORD(ptr->vecEndPos[0]);
	WRITE_COORD(ptr->vecEndPos[1]);
	WRITE_COORD(ptr->vecEndPos[2]);
	MESSAGE_END();

	pev->velocity = vecDir.Normalize() * flDamage * 20.0f;
	EMIT_SOUND(edict(), CHAN_ITEM, "debris/metal6.wav", 0.5f, ATTN_STATIC);
}

BOOL CWeaponBox::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	if (bitsDamageType & DMG_EXPLOSION)
	{
		pev->velocity = (pev->origin - pevInflictor->origin).Normalize() * flDamage * 20.0f;
	}

	pev->avelocity.x = 0;
	pev->avelocity.y = RANDOM_FLOAT(-pev->velocity.Length(), pev->velocity.Length());

	return FALSE;	// sorry, damage cannot destory weaponbox.
}
