#include "precompiled.h"

LINK_ENTITY_TO_CLASS(world_items, CWorldItem)

void CWorldItem::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "type"))
	{
		m_iType = Q_atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseEntity::KeyValue(pkvd);
	}
}

void CWorldItem::Spawn()
{
	CBaseEntity *pEntity = nullptr;

	switch (m_iType)
	{
	case 41: // ITEM_HEALTHKIT
		break;
	case 42: // ITEM_ANTIDOTE
		pEntity = CBaseEntity::Create("item_antidote", pev->origin, pev->angles);
		break;
	case 43: // ITEM_SECURITY
		pEntity = CBaseEntity::Create("item_security", pev->origin, pev->angles);
		break;
	case 44: // ITEM_BATTERY
		pEntity = CBaseEntity::Create("item_battery", pev->origin, pev->angles);
		break;
	case 45: // ITEM_SUIT
		pEntity = CBaseEntity::Create("item_suit", pev->origin, pev->angles);
		break;
	}

	if (pEntity)
	{
		pEntity->pev->target = pev->target;
		pEntity->pev->targetname = pev->targetname;
		pEntity->pev->spawnflags = pev->spawnflags;
	}

	REMOVE_ENTITY(edict());
}

void CItem::Spawn()
{
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;

	UTIL_SetOrigin(pev, pev->origin);
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));

	SetTouch(&CItem::ItemTouch);

	if (!DROP_TO_FLOOR(ENT(pev)))
	{
		UTIL_Remove(this);
		return;
	}
}

void CItem::ItemTouch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer() || pOther->pev->deadflag != DEAD_NO)
		return;

	CBasePlayer *pPlayer = static_cast<CBasePlayer *>(pOther);

	if (!g_pGameRules->CanHaveItem(pPlayer, this))
		return;

	if (MyTouch(pPlayer))
	{
		SUB_UseTargets(pOther, USE_TOGGLE, 0);
		SetTouch(nullptr);
		g_pGameRules->PlayerGotItem(pPlayer, this);

		if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_YES)
			Respawn();
		else
			UTIL_Remove(this);
	}
}

CBaseEntity *CItem::Respawn()
{
	SetTouch(nullptr);

	pev->effects |= EF_NODRAW;

	// blip to whereever you should respawn.
	UTIL_SetOrigin(pev, g_pGameRules->VecItemRespawnSpot(this));

	SetThink(&CItem::Materialize);
	pev->nextthink = g_pGameRules->FlItemRespawnTime(this);

	return this;
}

void CItem::Materialize()
{
	if (pev->effects & EF_NODRAW)
	{
		// changing from invisible state to visible.
		EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "items/suitchargeok1.wav", VOL_NORM, ATTN_NORM, 0, 150);

		pev->effects &= ~EF_NODRAW;
		pev->effects |= EF_MUZZLEFLASH;
	}

	SetTouch(&CItem::ItemTouch);
}

void CItemSuit::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_kevlar.mdl");
	CItem::Spawn();
}

void CItemSuit::Precache()
{
	PRECACHE_MODEL("models/w_kevlar.mdl");
	PRECACHE_SOUND("items/tr_kevlar.wav");
}

BOOL CItemSuit::MyTouch(CBasePlayer *pPlayer)
{
	if (pPlayer->pev->weapons & (1 << WEAPON_SUIT))
		return FALSE;

	EMIT_SOUND(pPlayer->edict(), CHAN_VOICE, "items/tr_kevlar.wav", VOL_NORM, ATTN_NORM);

	pPlayer->pev->weapons |= (1 << WEAPON_SUIT);
	pPlayer->m_iHideHUD &= ~(HIDEHUD_HEALTH | HIDEHUD_MONEY);

	return TRUE;
}

LINK_ENTITY_TO_CLASS(item_suit, CItemSuit)

const float CItemBattery::batteryCapacity = 15.0f;

void CItemBattery::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_battery.mdl");
	CItem::Spawn();
}

void CItemBattery::Precache()
{
	PRECACHE_MODEL("models/w_battery.mdl");
	PRECACHE_SOUND("items/gunpickup2.wav");
}

BOOL CItemBattery::MyTouch(CBasePlayer *pPlayer)
{
	if (pPlayer->HasRestrictItem(ITEM_BATTERY, ITEM_TYPE_TOUCHED))
		return FALSE;

	if (pPlayer->pev->armorvalue < CSGameRules()->PlayerMaxArmour(pPlayer) && (pPlayer->pev->weapons & (1 << WEAPON_SUIT)))
	{
		auto armorValue = batteryCapacity;

		if (pev->armorvalue != 0.0f)
		{
			armorValue = pev->armorvalue;
		}

		if (pPlayer->pev->armortype == ARMOR_NONE)
			pPlayer->pev->armortype = ARMOR_KEVLAR;

		pPlayer->pev->armorvalue += armorValue;
		pPlayer->pev->armorvalue = Q_min(pPlayer->pev->armorvalue, CSGameRules()->PlayerMaxArmour(pPlayer));

		EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "items/gunpickup2.wav", VOL_NORM, ATTN_NORM);

		MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, nullptr, pPlayer->pev);
			WRITE_STRING(STRING(pev->classname));
		MESSAGE_END();

		// Suit reports new power level
		// For some reason this wasn't working in release build -- round it.
		int pct = int(float(pPlayer->pev->armorvalue * 100.0f) * (1.0f / CSGameRules()->PlayerMaxArmour(pPlayer)) + 0.5f);
		pct = (pct / 5);

		if (pct > 0)
			pct--;

		char szcharge[64];
		Q_sprintf(szcharge, "!HEV_%1dP", pct);
		pPlayer->SetSuitUpdate(szcharge, SUIT_SENTENCE, SUIT_NEXT_IN_30SEC);

		return TRUE;
	}

	return FALSE;
}

LINK_ENTITY_TO_CLASS(item_battery, CItemBattery)

void CItemAntidote::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_antidote.mdl");
	CItem::Spawn();
}

void CItemAntidote::Precache()
{
	PRECACHE_MODEL("models/w_antidote.mdl");
}

BOOL CItemAntidote::MyTouch(CBasePlayer *pPlayer)
{
	if (pPlayer->HasRestrictItem(ITEM_ANTIDOTE, ITEM_TYPE_TOUCHED))
		return FALSE;

	pPlayer->SetSuitUpdate("!HEV_DET4", SUIT_SENTENCE, SUIT_NEXT_IN_1MIN);
	pPlayer->m_rgItems[ITEM_ID_ANTIDOTE] += 1;

	return TRUE;
}

LINK_ENTITY_TO_CLASS(item_antidote, CItemAntidote)

void CItemSecurity::Spawn()
{
	Precache();

	if (pev->model.IsNullOrEmpty())
	{
		// default model
		SET_MODEL(ENT(pev), "models/w_security.mdl");
	}
	else
	{
		// custom model
		SET_MODEL(ENT(pev), pev->model);
	}

	CItem::Spawn();
}

void CItemSecurity::Precache()
{
	if (pev->model.IsNullOrEmpty())
	{
		// default model
		PRECACHE_MODEL("models/w_security.mdl");
		return;
	}

	// custom model
	PRECACHE_MODEL(pev->model);
}

BOOL CItemSecurity::MyTouch(CBasePlayer *pPlayer)
{
	pPlayer->m_rgItems[ITEM_ID_SECURITY] += 1;
	return TRUE;
}

LINK_ENTITY_TO_CLASS(item_security, CItemSecurity)

void CItemLongJump::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_longjump.mdl");
	CItem::Spawn();
}

void CItemLongJump::Precache()
{
	PRECACHE_MODEL("models/w_longjump.mdl");
}

BOOL CItemLongJump::MyTouch(CBasePlayer *pPlayer)
{
	if (pPlayer->HasRestrictItem(ITEM_LONGJUMP, ITEM_TYPE_TOUCHED))
		return FALSE;

	if (pPlayer->m_fLongJump)
		return FALSE;

	if (pPlayer->pev->weapons & (1 << WEAPON_SUIT))
	{
		// player now has longjump module
		pPlayer->m_fLongJump = TRUE;
		SET_PHYSICS_KEY_VALUE(pPlayer->edict(), "slj", "1");

		MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, nullptr, pPlayer->pev);
			WRITE_STRING(STRING(pev->classname));
		MESSAGE_END();

		// Play the longjump sound UNDONE: Kelly? correct sound?
		EMIT_SOUND_SUIT(pPlayer->edict(), "!HEV_A1");
		return TRUE;
	}

	return FALSE;
}

LINK_ENTITY_TO_CLASS(item_longjump, CItemLongJump)

void CItemKevlar::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_kevlar.mdl");
	CItem::Spawn();
}

void CItemKevlar::Precache()
{
	PRECACHE_MODEL("models/w_kevlar.mdl");
}

BOOL CItemKevlar::MyTouch(CBasePlayer *pPlayer)
{
	if (pPlayer->HasRestrictItem(ITEM_KEVLAR, ITEM_TYPE_TOUCHED))
		return FALSE;

	if (pPlayer->pev->armortype != ARMOR_NONE && pPlayer->pev->armorvalue >= CSGameRules()->PlayerMaxArmour(pPlayer))
		return FALSE;

	if (pPlayer->pev->armortype == ARMOR_NONE)
		pPlayer->pev->armortype = ARMOR_KEVLAR;

	pPlayer->pev->armorvalue = CSGameRules()->PlayerMaxArmour(pPlayer);
	EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "items/ammopickup2.wav", VOL_NORM, ATTN_NORM);

	MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, nullptr, pPlayer->pev);
		WRITE_STRING(STRING(pev->classname));
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ONE, gmsgArmorType, nullptr, pPlayer->pev);
		WRITE_BYTE(pPlayer->pev->armortype == ARMOR_KEVLAR ? 0 : 1); // 0 = ARMOR_KEVLAR, 1 = ARMOR_VESTHELM
	MESSAGE_END();

	if (TheTutor)
	{
		TheTutor->OnEvent(EVENT_PLAYER_BOUGHT_SOMETHING, pPlayer);
	}

	return TRUE;
}

LINK_ENTITY_TO_CLASS(item_kevlar, CItemKevlar)

void CItemAssaultSuit::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_assault.mdl");
	CItem::Spawn();
}

void CItemAssaultSuit::Precache()
{
	PRECACHE_MODEL("models/w_assault.mdl");
}

BOOL CItemAssaultSuit::MyTouch(CBasePlayer *pPlayer)
{
	if (pPlayer->HasRestrictItem(ITEM_ASSAULT, ITEM_TYPE_TOUCHED))
		return FALSE;

	if (pPlayer->pev->armortype == ARMOR_VESTHELM && pPlayer->pev->armorvalue >= CSGameRules()->PlayerMaxArmour(pPlayer))
		return FALSE;

	pPlayer->pev->armortype = ARMOR_VESTHELM;
	pPlayer->pev->armorvalue = CSGameRules()->PlayerMaxArmour(pPlayer);

	EMIT_SOUND(pPlayer->edict(), CHAN_ITEM, "items/ammopickup2.wav", VOL_NORM, ATTN_NORM);

	MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, nullptr, pPlayer->pev);
		WRITE_STRING(STRING(pev->classname));
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ONE, gmsgArmorType, nullptr, pPlayer->pev);
		WRITE_BYTE(1); // 0 = ARMOR_KEVLAR, 1 = ARMOR_VESTHELM
	MESSAGE_END();

	if (TheTutor)
	{
		TheTutor->OnEvent(EVENT_PLAYER_BOUGHT_SOMETHING, pPlayer);
	}

	return TRUE;
}

LINK_ENTITY_TO_CLASS(item_assaultsuit, CItemAssaultSuit)
