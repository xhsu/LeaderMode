/*

Created Date: Mar 13 2020

*/

#include "precompiled.h"

TYPEDESCRIPTION CWeaponBox::m_SaveData[] =
{
	DEFINE_ARRAY(CWeaponBox, m_rgAmmo, FIELD_INTEGER, MAX_AMMO_SLOTS),
	DEFINE_ARRAY(CWeaponBox, m_rgpPlayerItems, FIELD_CLASSPTR, MAX_ITEM_TYPES),
};

const float CWeaponBox::THROWING_FORCE = 350.0f;

LINK_ENTITY_TO_CLASS(weaponbox, CWeaponBox)
IMPLEMENT_SAVERESTORE(CWeaponBox, CBaseEntity)

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
	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		if (m_rgpPlayerItems[i])
			m_rgpPlayerItems[i]->Kill();
	}

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

	CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pOther);

	if (pPlayer->m_bShieldDrawn)
		return;

	bool bRemove = true;

	// go through my weapons and try to give the usable ones to the player.
	// it's important the the player be given ammo first, so the weapons code doesn't refuse
	// to deploy a better weapon that the player may pick up because he has no ammo for it.
	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		if (!m_rgpPlayerItems[i])
			continue;

		if (pPlayer->AddPlayerItem(m_rgpPlayerItems[i]))
		{
			m_rgpPlayerItems[i] = nullptr;	// unlink this weapon from our CWeaponBox.
		}
	}

	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		// still, at least a weapon remains! don't del me!
		if (m_rgpPlayerItems[i])
		{
			bRemove = false;
			break;
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

	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		auto pWeapon = m_rgpPlayerItems[i];

		if (pWeapon && !pWeapon->IsDead())
		{
			pPlayer->AddAccount(pWeapon->m_pItemInfo->m_iCost / 2, RT_SOLD_ITEM);
			UTIL_PrintChatColor(pPlayer, BLUECHAT, "/gSeizing weapon /t%s/g have you rewarded with /t%d/g$", WeaponIDToAlias(pWeapon->m_iId), pWeapon->m_pItemInfo->m_iCost / 2);
			EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, SFX_REFUND_GUNS, VOL_NORM, ATTN_NORM);

			pWeapon->Kill();
		}
	}

	pev->nextthink = gpGlobals->time + 0.01f;
	m_bHadBeenSold = true;
}

// Add this weapon to the box
bool CWeaponBox::PackWeapon(CBaseWeapon* pWeapon)
{
	// is one of these weapons already packed in this box?
	if (HasWeapon(pWeapon->m_iId))
	{
		// box can only hold one of each weapon type
		return false;
	}

	if (m_rgpPlayerItems[pWeapon->m_pItemInfo->m_iSlot] && m_rgpPlayerItems[pWeapon->m_pItemInfo->m_iSlot] != pWeapon)
	{
		// the slot it wants to occupy has beed taken.
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

	m_rgpPlayerItems[pWeapon->m_pItemInfo->m_iSlot] = pWeapon;
	pWeapon->m_pWeaponBox = this;

	return true;
}

bool CWeaponBox::GiveAmmo(int iCount, AmmoIdType iId)
{
	if (iId <= 0 || iId >= AMMO_MAXTYPE)
		return false;

	m_rgAmmo[iId] += iCount;	// there is no ammunition carry limit for CWeaponBox.
	return true;
}

// Is a weapon of this type already packed in this box?
bool CWeaponBox::HasWeapon(WeaponIdType iId)
{
	for (auto pWeapon : CBaseWeapon::m_lstWeapons)
	{
		if (pWeapon->m_pWeaponBox != this)
			continue;

		if (pWeapon->m_iId != iId)
			continue;

		return true;
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

const char* CWeaponBox::GetCSModelName(WeaponIdType iId)
{
	const char* modelName = nullptr;
	switch (iId)
	{
	case WEAPON_ANACONDA:		modelName = "models/w_p228.mdl"; break;
	case WEAPON_M200:			modelName = "models/w_scout.mdl"; break;
	case WEAPON_HEGRENADE:		modelName = "models/w_hegrenade.mdl"; break;
	case WEAPON_STRIKER:		modelName = "models/w_xm1014.mdl"; break;
	case WEAPON_PM9:			modelName = "models/w_mac10.mdl"; break;
	case WEAPON_ACR:			modelName = "models/w_aug.mdl"; break;
	case WEAPON_SMOKEGRENADE:	modelName = "models/w_smokegrenade.mdl"; break;
	case WEAPON_P99:			modelName = "models/w_elite.mdl"; break;
	case WEAPON_FIVESEVEN:		modelName = "models/w_fiveseven.mdl"; break;
	case WEAPON_UMP45:			modelName = "models/w_ump45.mdl"; break;
	case WEAPON_M14EBR:			modelName = "models/w_sg550.mdl"; break;
	case WEAPON_CM901:			modelName = "models/weapons/w_cm901.mdl"; break;
	case WEAPON_QBZ95:			modelName = "models/w_famas.mdl"; break;
	case WEAPON_USP:			modelName = "models/weapons/w_usp.mdl"; break;
	case WEAPON_GLOCK18:		modelName = "models/w_glock18.mdl"; break;
	case WEAPON_AWP:			modelName = "models/weapons/w_awp.mdl"; break;
	case WEAPON_MP5N:			modelName = "models/w_mp5.mdl"; break;
	case WEAPON_MK46:			modelName = "models/w_m249.mdl"; break;
	case WEAPON_KSG12:			modelName = "models/weapons/w_ksg12.mdl"; break;
	case WEAPON_M4A1:			modelName = "models/w_m4a1.mdl"; break;
	case WEAPON_MP7A1:			modelName = "models/w_tmp.mdl"; break;
	case WEAPON_SVD:			modelName = "models/w_g3sg1.mdl"; break;
	case WEAPON_FLASHBANG:		modelName = "models/w_flashbang.mdl"; break;
	case WEAPON_DEAGLE:			modelName = "models/w_deagle.mdl"; break;
	case WEAPON_SCARL:			modelName = "models/w_sg552.mdl"; break;
	case WEAPON_AK47:			modelName = "models/w_ak47.mdl"; break;
	case WEAPON_KNIFE:			modelName = "models/w_knife.mdl"; break;
	case WEAPON_P90:			modelName = "models/w_p90.mdl"; break;
	case WEAPON_SHIELDGUN:		modelName = "models/w_shield.mdl"; break;
	default:
		ALERT(at_console, "CBasePlayer::PackDeadPlayerItems(): Unhandled item- not creating weaponbox\n");
	}

	return modelName;
}
