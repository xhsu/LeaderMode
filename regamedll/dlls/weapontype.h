/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#pragma once

#define MAX_WEAPONS			32

#define SLOT_NO				0
#define PRIMARY_WEAPON_SLOT	1
#define PISTOL_SLOT			2
#define KNIFE_SLOT			3
#define GRENADE_SLOT		4
#define EQUIPMENT_SLOT		5

#define WPN_P	-1	// penalty: price * 2
#define WPN_F	0	// forbidden: price * 99999
#define WPN_A	1	// allowed: price * 1
#define WPN_D	2	// discounted: price * 0.5

enum WeaponIdType
{
	WEAPON_NONE,

	// PISTOLS
	WEAPON_GLOCK18,
	WEAPON_USP,
	WEAPON_ANACONDA,
	WEAPON_DEAGLE,
	WEAPON_FIVESEVEN,
	WEAPON_P99,

	// SHOTGUNS
	WEAPON_KSG12,
	WEAPON_STRIKER,

	// SMGS
	WEAPON_MP7A1,
	WEAPON_PM9,
	WEAPON_MP5N,
	WEAPON_UMP45,
	WEAPON_P90,

	// ASSAULT RIFLES
	WEAPON_CM901,
	WEAPON_QBZ95,
	WEAPON_AK47,
	WEAPON_M4A1,
	WEAPON_SCARL,
	WEAPON_ACR,

	// SNIPER RIFLES,
	WEAPON_M200,
	WEAPON_M14EBR,
	WEAPON_AWP,
	WEAPON_SVD,

	// MACHINE GUNS
	WEAPON_MK46,

	// THROWABLES
	WEAPON_HEGRENADE,
	WEAPON_FLASHBANG,
	WEAPON_SMOKEGRENADE,

	//DISUSED
	WEAPON_UNUSED,
	WEAPON_C4,
	WEAPON_KNIFE,

	LAST_WEAPON,
	WEAPON_SHIELDGUN = 99
};

enum ItemCostType
{
	ASSAULTSUIT_PRICE   = 1000,
	FLASHBANG_PRICE     = 200,
	HEGRENADE_PRICE     = 300,
	SMOKEGRENADE_PRICE  = 300,
	KEVLAR_PRICE        = 650,
	HELMET_PRICE        = 350,
	NVG_PRICE           = 1250,
	DEFUSEKIT_PRICE     = 200,
	SHIELDGUN_PRICE		= 2200,
};

enum WeaponState
{
	WPNSTATE_DEAD				= BIT(0),	// mark for remove.
	WPNSTATE_MELEE				= BIT(1),
	WPNSTATE_ELITE_LEFT         = BIT(3),
	WPNSTATE_FAMAS_BURST_MODE   = BIT(4),
	WPNSTATE_SHIELD_DRAWN       = BIT(5),
};

enum WeaponClassWeight
{
	EQUIPMENT_BASIC_WEIGHT = 0,
	MELEE_BASIC_WEIGHT,
	THROWABLE_BASIC_WEIGHT,

	PISTOL_BASIC_WEIGHT,
	SHOTGUN_BASIC_WEIGHT,
	SMG_BASIC_WEIGHT,
	ASSAULT_RIFLE_BASIC_WEIGHT,
	MACHINE_GUN_BASIC_WEIGHT,
	SNIPER_RIFLE_BASIC_WEIGHT,
};

enum WeaponClassType
{
	WEAPONCLASS_NONE,
	WEAPONCLASS_KNIFE,
	WEAPONCLASS_PISTOL,
	WEAPONCLASS_GRENADE,
	WEAPONCLASS_SUBMACHINEGUN,
	WEAPONCLASS_SHOTGUN,
	WEAPONCLASS_MACHINEGUN,
	WEAPONCLASS_RIFLE,
	WEAPONCLASS_SNIPERRIFLE,
	WEAPONCLASS_MAX,
};

// dummy.
enum AmmoIdType;

// one of the core data structure.
struct ItemInfo
{
	WeaponIdType	m_iId;
	const char*		m_pszInternalName;
	const char*		m_pszExternalName;
	AmmoIdType		m_iAmmoType;
	int				m_iMaxClip;
	int				m_iSlot;
	int				m_bitsFlags;
	int				m_iWeight;
	WeaponClassType	m_iClassType;
	int				m_iCost;
};

extern const ItemInfo g_rgItemInfo[LAST_WEAPON];

WeaponIdType AliasToWeaponID(const char* alias);
const char* WeaponIDToAlias(int id);	// Given a weapon ID, return its alias
const char* AliasToWeaponClassname(const char* alias);
const char* WeaponIDToWeaponClassname(WeaponIdType id);
WeaponIdType WeaponClassnameToID(const char* classname);
WeaponClassType WeaponIDToWeaponClass(WeaponIdType id);
bool IsPrimaryWeapon(int id);	// Return true if given weapon ID is a primary weapon
bool IsSecondaryWeapon(int id);	// Return true if given weapon ID is a secondary weapon
bool IsGrenadeWeapon(int id);	// Return true if given weapon ID is a grenade
bool IsSemiautoWeapon(WeaponIdType iId);	// Return true if this weapon is logically coded as a semi-auto weapon.
const ItemInfo* GetWeaponInfo(int weaponID);
const ItemInfo* GetWeaponInfo(const char* weaponName);

inline WeaponClassType AliasToWeaponClass(const char* alias) { return WeaponIDToWeaponClass(AliasToWeaponID(alias)); }
