#include "precompiled.h"

const AmmoInfo g_rgAmmoInfo[AMMO_MAXTYPE] =
{
	{AMMO_NONE, "", 0, 0, 0, 0},

	// Sniper & Hunter
	{AMMO_338Magnum,	"338Magnum",	20,		10,		125,	45,		8000.0f},
	{AMMO_408CheyTac,	"408CheyTac",	14,		7,		95,		45,		8000.0f},

	// Assault
	{AMMO_762Nato,		"762Nato",		60,		30,		80,		39,		5000.0f},
	{AMMO_556NatoBox,	"556NatoBox",	200,	100,	250,	35,		4000.0f},
	{AMMO_556Nato,		"556Nato",		60,		30,		60,		35,		4000.0f},
	{AMMO_58mmCN,		"58mmCN",		60,		30,		60,		35,		4000.0f},

	// Buckshot
	{AMMO_Buckshot,		"buckshot",		14,		7,		65,		0,		0.0f},

	// PDWs
	{AMMO_57mm,			"57mm",			100,	50,		50,		30,		2000.0f},
	{AMMO_46PDW,		"46pdw",		80,		40,		30,		30,		2000.0f},

	// High retarding force
	{AMMO_45acp,		"45acp",		50,		24,		50,		15,		500.0f},
	{AMMO_50AE,			"50AE",			14,		7,		40,		30,		1000.0f},
	{AMMO_44Magnum,		"44Magnum",		18,		6,		35,		25,		800.0f},
	{AMMO_9mm,			"9mm",			60,		30,		20,		21,		800.0f},

	// Equipments
	{AMMO_Flashbang,	"Flashbang",	2,		1,		FLASHBANG_PRICE,	0,		0.0f},
	{AMMO_HEGrenade,	"HEGrenade",	1,		1,		HEGRENADE_PRICE,	0,		0.0f},
	{AMMO_SmokeGrenade,	"SmokeGrenade",	1,		1,		SMOKEGRENADE_PRICE,	0,		0.0f},
	{AMMO_C4,			"C4",			1,		1,		0,		0,		0.0f},
};

const AmmoInfo* GetAmmoInfo(const char* ammoName)
{
	if (ammoName)
	{
		for (int i = 0; i < MAX_WEAPONS; i++)
		{
			if (!Q_stricmp(ammoName, g_rgAmmoInfo[i].m_pszName))
				return &g_rgAmmoInfo[i];
		}
	}

	return nullptr;
}

const AmmoInfo* GetAmmoInfo(int iId)
{
	return (iId > 0 && iId < AMMO_MAXTYPE) ? &g_rgAmmoInfo[iId] : nullptr;
}

AmmoIdType GetAmmoIdOfEquipment(EquipmentIdType iId)
{
	switch (iId)
	{
	case EQP_HEGRENADE:
		return AMMO_HEGrenade;

	case EQP_FLASHBANG:
		return AMMO_Flashbang;

	case EQP_SMOKEGRENADE:
		return AMMO_SmokeGrenade;

	default:
		return AMMO_NONE;
	}
}

/*
============
DescribeBulletTypeParameters

Sets iPenetrationPower and flPenetrationDistance.
If iBulletType is unknown, calls assert() and sets these two vars to 0
============
*/
bool DescribeBulletTypeParameters(AmmoIdType iBulletType, int& iPenetrationPower, float& flPenetrationDistance)
{
	if (iBulletType <= AMMO_NONE || iBulletType >= AMMO_MAXTYPE)
		return false;

	iPenetrationPower = g_rgAmmoInfo[iBulletType].m_iPenetrationPower;
	flPenetrationDistance = g_rgAmmoInfo[iBulletType].m_flPenetrationDistance;
	return true;
}
