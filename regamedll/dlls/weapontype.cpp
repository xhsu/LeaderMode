#include "precompiled.h"

const ItemInfo g_rgItemInfo[LAST_WEAPON] =
{
	{WEAPON_NONE, "weapon_null", "AIR", AMMO_NONE, WEAPON_NOCLIP, SLOT_NO, ITEM_FLAG_NONE, -1, WEAPONCLASS_NONE, 0},

	// PISTOLS
	{WEAPON_GLOCK18,		"weapon_glock18",		"Glock 18C",					AMMO_9mm,			17,	PISTOL_SLOT,			ITEM_FLAG_NONE,			PISTOL_BASIC_WEIGHT + 1,		WEAPONCLASS_PISTOL,			400},
	{WEAPON_USP,			"weapon_usp",			"H&K USP.45",					AMMO_45acp,			12,	PISTOL_SLOT,			ITEM_FLAG_NONE,			PISTOL_BASIC_WEIGHT + 2,		WEAPONCLASS_PISTOL,			500},
	{WEAPON_ANACONDA,		"weapon_anaconda",		"Colt Anaconda",				AMMO_44Magnum,		6,	PISTOL_SLOT,			ITEM_FLAG_NONE,			PISTOL_BASIC_WEIGHT + 3,		WEAPONCLASS_PISTOL,			600},
	{WEAPON_DEAGLE,			"weapon_deagle",		"IMI Desert Eagle",				AMMO_50AE,			7,	PISTOL_SLOT,			ITEM_FLAG_NONE,			PISTOL_BASIC_WEIGHT + 4,		WEAPONCLASS_PISTOL,			650},
	{WEAPON_FIVESEVEN,		"weapon_fiveseven",		"FiveseveN",					AMMO_57mm,			20,	PISTOL_SLOT,			ITEM_FLAG_NONE,			PISTOL_BASIC_WEIGHT + 5,		WEAPONCLASS_PISTOL,			750},
	{WEAPON_P99,			"weapon_p99",			"Walther P99",					AMMO_9mm,			30,	PISTOL_SLOT,			ITEM_FLAG_NONE,			PISTOL_BASIC_WEIGHT + 6,		WEAPONCLASS_PISTOL,			800},

	// SHOTGUNS
	{WEAPON_KSG12,			"weapon_ksg12",			"Kel-Tec KSG-12",				AMMO_Buckshot,		14,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			SHOTGUN_BASIC_WEIGHT + 1,		WEAPONCLASS_SHOTGUN,		1700},
	{WEAPON_STRIKER,		"weapon_striker",		"Armsel Striker",				AMMO_Buckshot,		7,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			SHOTGUN_BASIC_WEIGHT + 2,		WEAPONCLASS_SHOTGUN,		3000},
	// AA12/USAS12, M1014

	// SMGS
	{WEAPON_MP7A1,			"weapon_mp7a1",			"H&K MP7A1",					AMMO_46PDW,			40,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			SMG_BASIC_WEIGHT + 1,			WEAPONCLASS_SUBMACHINEGUN,	1250},
	{WEAPON_PM9,			"weapon_pm9",			"Minebea PM-9",					AMMO_9mm,			25,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			SMG_BASIC_WEIGHT + 2,			WEAPONCLASS_SUBMACHINEGUN,	1400},
	{WEAPON_MP5N,			"weapon_mp5navy",		"H&K MP5",						AMMO_9mm,			30,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			SMG_BASIC_WEIGHT + 3,			WEAPONCLASS_SUBMACHINEGUN,	1500},
	{WEAPON_UMP45,			"weapon_ump45",			"H&K UMP45",					AMMO_45acp,			25,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			SMG_BASIC_WEIGHT + 4,			WEAPONCLASS_SUBMACHINEGUN,	1700},
	{WEAPON_P90,			"weapon_p90",			"FN P90",						AMMO_57mm,			50,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			SMG_BASIC_WEIGHT + 5,			WEAPONCLASS_SUBMACHINEGUN,	2350},
	// Kriss Vector, MAC10

	// ASSAULT RIFLES
	{WEAPON_CM901,			"weapon_cm901",			"Colt CM901",					AMMO_556Nato,		30,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			ASSAULT_RIFLE_BASIC_WEIGHT + 1,	WEAPONCLASS_RIFLE,			2000},
	{WEAPON_QBZ95,			"weapon_qbz95",			"Norinco QBZ95",				AMMO_58mmCN,		30, PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			ASSAULT_RIFLE_BASIC_WEIGHT + 2,	WEAPONCLASS_RIFLE,			2250},
	{WEAPON_AK47,			"weapon_ak47",			"Kalashnikov AK-47",			AMMO_762Nato,		30,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			ASSAULT_RIFLE_BASIC_WEIGHT + 3,	WEAPONCLASS_RIFLE,			2500},
	{WEAPON_M4A1,			"weapon_m4a1",			"Colt M4A1",					AMMO_556Nato,		30,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			ASSAULT_RIFLE_BASIC_WEIGHT + 4,	WEAPONCLASS_RIFLE,			3100},
	{WEAPON_SCARL,			"weapon_scarl",			"FN SCAR-L",					AMMO_556Nato,		30,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			ASSAULT_RIFLE_BASIC_WEIGHT + 5,	WEAPONCLASS_RIFLE,			3500},
	{WEAPON_ACR,			"weapon_acr",			"Remington ACR",				AMMO_556Nato,		30,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			ASSAULT_RIFLE_BASIC_WEIGHT + 6,	WEAPONCLASS_RIFLE,			3500},
	// XM8

	// SNIPER RIFLES
	{WEAPON_M200,			"weapon_m200",			"CheyTac Intervention M200",	AMMO_408CheyTac,	7,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			SNIPER_RIFLE_BASIC_WEIGHT + 1,	WEAPONCLASS_SNIPERRIFLE,	2750},
	{WEAPON_M14EBR,			"weapon_m14ebr",		"Mk.14 EBR",					AMMO_762Nato,		20,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			SNIPER_RIFLE_BASIC_WEIGHT + 2,	WEAPONCLASS_SNIPERRIFLE,	4200},
	{WEAPON_AWP,			"weapon_awp",			"A.I. Arctic Warfare",			AMMO_338Magnum,		10, PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			SNIPER_RIFLE_BASIC_WEIGHT + 3,	WEAPONCLASS_SNIPERRIFLE,	4750},
	{WEAPON_SVD,			"weapon_svd",			"Dragunov SVD",					AMMO_762Nato,		10,	PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			SNIPER_RIFLE_BASIC_WEIGHT + 4,	WEAPONCLASS_SNIPERRIFLE,	5000},
	// PSG-1

	// MACHINE GUNS
	{WEAPON_MK46,			"weapon_mk46",			"Mk.46 Mod.0",					AMMO_556NatoBox,	100,PRIMARY_WEAPON_SLOT,	ITEM_FLAG_NONE,			MACHINE_GUN_BASIC_WEIGHT + 1,	WEAPONCLASS_MACHINEGUN,		5750},
	// RPD

	// THROWABLES
	{WEAPON_HEGRENADE,		"weapon_hegrenade",		"HE Grenade",					AMMO_HEGrenade,		-1,	GRENADE_SLOT,			ITEM_FLAG_EXHAUSTIBLE,	THROWABLE_BASIC_WEIGHT + 1,		WEAPONCLASS_GRENADE,		HEGRENADE_PRICE},
	{WEAPON_FLASHBANG,		"weapon_flashbang",		"Flashbang",					AMMO_Flashbang,		-1,	GRENADE_SLOT,			ITEM_FLAG_EXHAUSTIBLE,	THROWABLE_BASIC_WEIGHT + 2,		WEAPONCLASS_GRENADE,		FLASHBANG_PRICE},
	{WEAPON_SMOKEGRENADE,	"weapon_smokegrenade",	"Smoke Grenade",				AMMO_SmokeGrenade,	-1,	GRENADE_SLOT,			ITEM_FLAG_EXHAUSTIBLE,	THROWABLE_BASIC_WEIGHT + 3,		WEAPONCLASS_GRENADE,		SMOKEGRENADE_PRICE},

	// DISUSED
	{WEAPON_UNUSED, "", "", AMMO_NONE, WEAPON_NOCLIP, SLOT_NO, ITEM_FLAG_NONE, -1, WEAPONCLASS_NONE, 0},
	{WEAPON_C4, "weapon_c4", "C4", AMMO_NONE, WEAPON_NOCLIP, SLOT_NO, ITEM_FLAG_NONE, -1, WEAPONCLASS_NONE, 0},	// C4
	{WEAPON_KNIFE,			"weapon_knife",			"Seal Knife",					AMMO_NONE,			-1,	KNIFE_SLOT,				ITEM_FLAG_NONE,			MELEE_BASIC_WEIGHT,				WEAPONCLASS_KNIFE,			0},

	//{LAST_WEAPON, nullptr, AMMO_NONE, WEAPON_NOCLIP, SLOT_NO, ITEM_FLAG_NONE, -1, 0},	// vest, should never use it.
};

const EquipmentInfo g_rgEquipmentInfo[EQP_COUNT] =
{
	{EQP_NONE,	"",	"",	0},

	// armour
	{EQP_KEVLAR,		"item_kevlar",			"Kevlar",			KEVLAR_PRICE},
	{EQP_ASSAULT_SUIT,	"item_assaultsuit",		"Kevlar & Helmet",	ASSAULTSUIT_PRICE},

	// grenades
	{EQP_HEGRENADE,		"weapon_hegrenade",		"HE Grenade",			HEGRENADE_PRICE},
	{EQP_FLASHBANG,		"weapon_flashbang",		"Flashbang",			FLASHBANG_PRICE},
	{EQP_SMOKEGRENADE,	"weapon_smokegrenade",	"Smoke Grenade",		SMOKEGRENADE_PRICE},
	{EQP_FROST_GR,		"weapon_frostgrenade",	"Cryogrenade",			HEGRENADE_PRICE * 2},
	{EQP_INCENDIARY_GR,	"weapon_incendiary_gr",	"Incendiary Grenade",	HEGRENADE_PRICE * 2},
	{EQP_HEALING_GR,	"weapon_healing_gr",	"Healing Grenade",		SMOKEGRENADE_PRICE * 2},
	{EQP_GAS_GR,		"weapon_gasgrenade",	"Nerve Gas Grenade",	SMOKEGRENADE_PRICE * 2},

	// misc
	{EQP_NVG,			"item_nvg",				"Nightvision",			NVG_PRICE},
};

// Given an alias, return the associated weapon ID
WeaponIdType AliasToWeaponID(const char *alias)
{
	if (alias)
	{
		for (int i = 0; i < LAST_WEAPON; i++)
		{
			if (!Q_stricmp(alias, g_rgItemInfo[i].m_pszExternalName))
				return WeaponIdType(i);
		}
	}

	return WEAPON_NONE;
}

// Given a weapon ID, return its alias
const char* WeaponIDToAlias(int id)
{
	return g_rgItemInfo[id].m_pszExternalName;
}

const char* AliasToWeaponClassname(const char* alias)
{
	if (alias)
	{
		for (int i = 0; i < LAST_WEAPON; i++)
		{
			if (!Q_stricmp(alias, g_rgItemInfo[i].m_pszExternalName))
				return g_rgItemInfo[i].m_pszInternalName;
		}
	}

	return nullptr;
}

const char* WeaponIDToWeaponClassname(WeaponIdType id)
{
	return g_rgItemInfo[id].m_pszInternalName;
}

WeaponIdType WeaponClassnameToID(const char* classname)
{
	if (classname && Q_strlen(classname))
	{
		for (int i = 0; i < LAST_WEAPON; i++)
		{
			if (g_rgItemInfo[i].m_pszInternalName && !Q_stricmp(classname, g_rgItemInfo[i].m_pszInternalName))
				return g_rgItemInfo[i].m_iId;
		}
	}

	return WEAPON_NONE;
}

WeaponClassType WeaponIDToWeaponClass(WeaponIdType id)
{
	return g_rgItemInfo[id].m_iClassType;
}

// Return true if given weapon ID is a primary weapon
bool IsPrimaryWeapon(int id)
{
	return g_rgItemInfo[id].m_iSlot == PRIMARY_WEAPON_SLOT;
}

// Return true if given weapon ID is a secondary weapon
bool IsSecondaryWeapon(int id)
{
	return g_rgItemInfo[id].m_iSlot == PISTOL_SLOT;
}

// Return true if given weapon ID is a grenade
bool IsGrenadeWeapon(int id)
{
	return g_rgItemInfo[id].m_iSlot == GRENADE_SLOT;
}

constexpr int BITS_SEMIAUTO_WEAPON = ((1 << WEAPON_USP) | (1 << WEAPON_ANACONDA) | (1 << WEAPON_DEAGLE) | (1 << WEAPON_FIVESEVEN) | (1 << WEAPON_P99) | (1 << WEAPON_M14EBR) | (1 << WEAPON_SVD));

bool IsSemiautoWeapon(WeaponIdType iId)
{
	return !!((1 << iId) & BITS_SEMIAUTO_WEAPON);
}

const ItemInfo* GetWeaponInfo(int weaponID)
{
	return (weaponID > 0 && weaponID < LAST_WEAPON) ? &g_rgItemInfo[weaponID] : nullptr;
}

const ItemInfo* GetWeaponInfo(const char* weaponName)
{
	if (weaponName)
	{
		for (int i = 0; i < LAST_WEAPON; i++)
		{
			if (g_rgItemInfo[i].m_pszInternalName && !Q_stricmp(weaponName, g_rgItemInfo[i].m_pszInternalName))
				return &g_rgItemInfo[i];
		}
	}

	return nullptr;
}
