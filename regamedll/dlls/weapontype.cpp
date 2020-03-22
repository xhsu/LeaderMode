#include "precompiled.h"

const ItemInfo g_rgItemInfo[LAST_WEAPON] =
{
	{WEAPON_NONE, nullptr, AMMO_NONE, WEAPON_NOCLIP, SLOT_NO, -1, ITEM_FLAG_NONE, -1, 0},
	{WEAPON_ANACONDA,		"weapon_anaconda",		AMMO_44Magnum,		6,	PISTOL_SLOT,			1,	ITEM_FLAG_NONE,									P228_WEIGHT,			600},
	{WEAPON_UNUSED, nullptr, AMMO_NONE, WEAPON_NOCLIP, SLOT_NO, -1, ITEM_FLAG_NONE, -1, 0},
	{WEAPON_M200,			"weapon_m200",			AMMO_408CheyTac,	7,	PRIMARY_WEAPON_SLOT,	1,	ITEM_FLAG_NONE,									SCOUT_WEIGHT,			2750},
	{WEAPON_HEGRENADE,		"weapon_hegrenade",		AMMO_HEGrenade,		-1,	GRENADE_SLOT,			1,	ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE,	HEGRENADE_WEIGHT,		HEGRENADE_PRICE},
	{WEAPON_STRIKER,		"weapon_striker",		AMMO_Buckshot,		7,	PRIMARY_WEAPON_SLOT,	2,	ITEM_FLAG_NONE,									XM1014_WEIGHT,			3000},
	{WEAPON_C4, nullptr, AMMO_NONE, WEAPON_NOCLIP, SLOT_NO, -1, ITEM_FLAG_NONE, -1, 0},	// C4
	{WEAPON_PM9,			"weapon_pm9",			AMMO_9mm,			25,	PRIMARY_WEAPON_SLOT,	3,	ITEM_FLAG_NONE,									MAC10_WEIGHT,			1400},
	{WEAPON_ACR,			"weapon_acr",			AMMO_556Nato,		30,	PRIMARY_WEAPON_SLOT,	4,	ITEM_FLAG_NONE,									AUG_WEIGHT,				3500},
	{WEAPON_SMOKEGRENADE,	"weapon_smokegrenade",	AMMO_SmokeGrenade,	-1,	GRENADE_SLOT,			2,	ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE,	SMOKEGRENADE_WEIGHT,	SMOKEGRENADE_PRICE},
	{WEAPON_P99,			"weapon_p99",			AMMO_9mm,			30,	PISTOL_SLOT,			2,	ITEM_FLAG_NONE,									ELITE_WEIGHT,			800},
	{WEAPON_FIVESEVEN,		"weapon_fiveseven",		AMMO_57mm,			20,	PISTOL_SLOT,			3,	ITEM_FLAG_NONE,									FIVESEVEN_WEIGHT,		750},
	{WEAPON_UMP45,			"weapon_ump45",			AMMO_45acp,			25,	PRIMARY_WEAPON_SLOT,	5,	ITEM_FLAG_NONE,									UMP45_WEIGHT,			1700},
	{WEAPON_M14EBR,			"weapon_m14ebr",		AMMO_762Nato,		20,	PRIMARY_WEAPON_SLOT,	6,	ITEM_FLAG_NONE,									SG550_WEIGHT,			4200},
	{WEAPON_CM901,			"weapon_cm901",			AMMO_556Nato,		30,	PRIMARY_WEAPON_SLOT,	7,	ITEM_FLAG_NONE,									GALIL_WEIGHT,			2000},
	{WEAPON_QBZ95,			"weapon_qbz95",			AMMO_58mmCN,		30, PRIMARY_WEAPON_SLOT,	8,	ITEM_FLAG_NONE,									FAMAS_WEIGHT,			2250},
	{WEAPON_USP,			"weapon_usp",			AMMO_45acp,			12,	PISTOL_SLOT,			4,	ITEM_FLAG_NONE,									USP_WEIGHT,				500},
	{WEAPON_GLOCK18,		"weapon_glock18",		AMMO_9mm,			17,	PISTOL_SLOT,			5,	ITEM_FLAG_NONE,									GLOCK18_WEIGHT,			400},
	{WEAPON_AWP,			"weapon_awp",			AMMO_338Magnum,		10, PRIMARY_WEAPON_SLOT,	9,	ITEM_FLAG_NONE,									AWP_WEIGHT,				4750},
	{WEAPON_MP5N,			"weapon_mp5navy",		AMMO_9mm,			30,	PRIMARY_WEAPON_SLOT,	10,	ITEM_FLAG_NONE,									MP5NAVY_WEIGHT,			1500},
	{WEAPON_MK46,			"weapon_mk46",			AMMO_556NatoBox,	100,PRIMARY_WEAPON_SLOT,	11,	ITEM_FLAG_NONE,									M249_WEIGHT,			5750},
	{WEAPON_KSG12,			"weapon_ksg12",			AMMO_Buckshot,		14,	PRIMARY_WEAPON_SLOT,	12,	ITEM_FLAG_NONE,									M3_WEIGHT,				1700},
	{WEAPON_M4A1,			"weapon_m4a1",			AMMO_556Nato,		30,	PRIMARY_WEAPON_SLOT,	13,	ITEM_FLAG_NONE,									M4A1_WEIGHT,			3100},
	{WEAPON_MP7A1,			"weapon_mp7a1",			AMMO_46PDW,			40,	PRIMARY_WEAPON_SLOT,	14,	ITEM_FLAG_NONE,									TMP_WEIGHT,				1250},
	{WEAPON_SVD,			"weapon_svd",			AMMO_762Nato,		10,	PRIMARY_WEAPON_SLOT,	15,	ITEM_FLAG_NONE,									G3SG1_WEIGHT,			5000},
	{WEAPON_FLASHBANG,		"weapon_flashbang",		AMMO_Flashbang,		-1,	GRENADE_SLOT,			3,	ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE,	FLASHBANG_WEIGHT,		FLASHBANG_PRICE},
	{WEAPON_DEAGLE,			"weapon_deagle",		AMMO_50AE,			7,	PISTOL_SLOT,			6,	ITEM_FLAG_NONE,									DEAGLE_WEIGHT,			650},
	{WEAPON_SCARL,			"weapon_scarl",			AMMO_556Nato,		30,	PRIMARY_WEAPON_SLOT,	16,	ITEM_FLAG_NONE,									SG552_WEIGHT,			3500},
	{WEAPON_AK47,			"weapon_ak47",			AMMO_762Nato,		30,	PRIMARY_WEAPON_SLOT,	17,	ITEM_FLAG_NONE,									AK47_WEIGHT,			2500},
	{WEAPON_KNIFE,			"weapon_knife",			AMMO_NONE,			-1,	KNIFE_SLOT,				1,	ITEM_FLAG_NONE,									KNIFE_WEIGHT,			0},
	{WEAPON_P90,			"weapon_p90",			AMMO_57mm,			50,	PRIMARY_WEAPON_SLOT,	18,	ITEM_FLAG_NONE,									P90_WEIGHT,				2350},
	//{LAST_WEAPON, nullptr, AMMO_NONE, WEAPON_NOCLIP, SLOT_NO, -1, ITEM_FLAG_NONE, -1, 0},	// vest, should never use it.
};

const char* g_rgszWeaponAlias[LAST_WEAPON] =
{
	"",
	"Colt Anaconda",
	"",
	"CheyTac Intervention M200",
	"HE Grenade",
	"Armsel Striker",
	"C4",
	"Minebea PM-9",
	"Remington ACR",
	"Smoke Grenade",
	"Walther P99",
	"FiveseveN",
	"H&K UMP45",
	"Mk.14 EBR",
	"Colt CM901",
	"Norinco QBZ95",
	"H&K USP.45",
	"Glock 18C",
	"A.I. Arctic Warfare",
	"H&K MP5",
	"Mk.46 Mod.0",
	"Kel-Tec KSG-12",
	"Colt M4A1",
	"H&K MP7A1",
	"Dragunov SVD",
	"Flashbang",
	"IMI Desert Eagle",
	"FN SCAR-L",
	"Kalashnikov AK-47",
	"Seal Knife",
	"FN P90",
	// "HL suit"
};

// Given an alias, return the associated weapon ID
WeaponIdType AliasToWeaponID(const char *alias)
{
	if (alias)
	{
		for (int i = 0; i < LAST_WEAPON; i++)
		{
			if (!Q_stricmp(alias, g_rgszWeaponAlias[i]))
				return WeaponIdType(i);
		}
	}

	return WEAPON_NONE;
}

// Given a weapon ID, return its alias
const char* WeaponIDToAlias(int id)
{
	return g_rgszWeaponAlias[id];
}

const char* AliasToWeaponClassname(const char* alias)
{
	if (alias)
	{
		for (int i = 0; i < LAST_WEAPON; i++)
		{
			if (!Q_stricmp(alias, g_rgszWeaponAlias[i]))
				return g_rgItemInfo[i].m_pszClassName;
		}
	}

	return nullptr;
}

const char* WeaponIDToWeaponClassname(WeaponIdType id)
{
	return g_rgItemInfo[id].m_pszClassName;
}

WeaponIdType WeaponClassnameToID(const char* classname)
{
	if (classname)
	{
		for (int i = 0; i < LAST_WEAPON; i++)
		{
			if (g_rgItemInfo[i].m_pszClassName && !Q_stricmp(classname, g_rgItemInfo[i].m_pszClassName))
				return g_rgItemInfo[i].m_iId;
		}
	}

	return WEAPON_NONE;
}

WeaponClassType WeaponIDToWeaponClass(WeaponIdType id)
{
	switch (id)
	{
	case WEAPON_ACR:
	case WEAPON_CM901:
	case WEAPON_M4A1:
	case WEAPON_SCARL:
	case WEAPON_AK47:
	case WEAPON_QBZ95:
		return WEAPONCLASS_RIFLE;

	case WEAPON_GLOCK18:
	case WEAPON_USP:
	case WEAPON_P99:
	case WEAPON_FIVESEVEN:
	case WEAPON_ANACONDA:
	case WEAPON_DEAGLE:
		return WEAPONCLASS_PISTOL;

	case WEAPON_MP5N:
	case WEAPON_PM9:
	case WEAPON_MP7A1:
	case WEAPON_UMP45:
	case WEAPON_P90:
		return WEAPONCLASS_SUBMACHINEGUN;

	case WEAPON_M200:
	case WEAPON_M14EBR:
	case WEAPON_AWP:
	case WEAPON_SVD:
		return WEAPONCLASS_SNIPERRIFLE;

	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
		return WEAPONCLASS_GRENADE;

	case WEAPON_KSG12:
	case WEAPON_STRIKER:
		return WEAPONCLASS_SHOTGUN;

	case WEAPON_MK46:
		return WEAPONCLASS_MACHINEGUN;

	default:
		break;
	}

	return WEAPONCLASS_NONE;
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
			if (g_rgItemInfo[i].m_pszClassName && !Q_stricmp(weaponName, g_rgItemInfo[i].m_pszClassName))
				return &g_rgItemInfo[i];
		}
	}

	return nullptr;
}
