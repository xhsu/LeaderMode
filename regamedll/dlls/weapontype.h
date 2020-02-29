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
	WEAPON_ANACONDA,	// WEAPON_P228
	WEAPON_UNUSED,		// WEAPON_GLOCK
	WEAPON_M200,		// WEAPON_SCOUT
	WEAPON_HEGRENADE,
	WEAPON_STRIKER,		// WEAPON_XM1014
	WEAPON_C4,
	WEAPON_PM9,			// WEAPON_MAC10
	WEAPON_ACR,			// WEAPON_AUG
	WEAPON_SMOKEGRENADE,
	WEAPON_P99,			// WEAPON_ELITE
	WEAPON_FIVESEVEN,
	WEAPON_UMP45,
	WEAPON_M14EBR,		// WEAPON_SG550
	WEAPON_CM901,		// WEAPON_GALIL
	WEAPON_QBZ95,		// WEAPON_FAMAS
	WEAPON_USP,
	WEAPON_GLOCK18,
	WEAPON_AWP,
	WEAPON_MP5N,
	WEAPON_MK46,		// WEAPON_M249
	WEAPON_KSG12,		// WEAPON_M3
	WEAPON_M4A1,
	WEAPON_MP7A1,		// WEAPON_TMP
	WEAPON_SVD,			// WEAPON_G3SG1
	WEAPON_FLASHBANG,
	WEAPON_DEAGLE,
	WEAPON_SCARL,		// WEAPON_SG552
	WEAPON_AK47,
	WEAPON_KNIFE,
	WEAPON_P90,

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
	WPNSTATE_GLOCK18_BURST_MODE = BIT(1),
	WPNSTATE_ELITE_LEFT         = BIT(3),
	WPNSTATE_FAMAS_BURST_MODE   = BIT(4),
	WPNSTATE_SHIELD_DRAWN       = BIT(5),
};

enum WeightWeapon
{
	P228_WEIGHT         = 5,
	GLOCK18_WEIGHT      = 5,
	SCOUT_WEIGHT        = 30,
	HEGRENADE_WEIGHT    = 2,
	XM1014_WEIGHT       = 20,
	C4_WEIGHT           = 3,
	MAC10_WEIGHT        = 25,
	AUG_WEIGHT          = 25,
	SMOKEGRENADE_WEIGHT = 1,
	ELITE_WEIGHT        = 5,
	FIVESEVEN_WEIGHT    = 5,
	UMP45_WEIGHT        = 25,
	SG550_WEIGHT        = 20,
	GALIL_WEIGHT        = 25,
	FAMAS_WEIGHT        = 75,
	USP_WEIGHT          = 5,
	AWP_WEIGHT          = 30,
	MP5NAVY_WEIGHT      = 25,
	M249_WEIGHT         = 25,
	M3_WEIGHT           = 20,
	M4A1_WEIGHT         = 25,
	TMP_WEIGHT          = 25,
	G3SG1_WEIGHT        = 20,
	FLASHBANG_WEIGHT    = 1,
	DEAGLE_WEIGHT       = 7,
	SG552_WEIGHT        = 25,
	AK47_WEIGHT         = 25,
	P90_WEIGHT          = 26,
	KNIFE_WEIGHT        = 0,
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

enum shieldgun_e
{
	SHIELDGUN_IDLE,
	SHIELDGUN_SHOOT1,
	SHIELDGUN_SHOOT2,
	SHIELDGUN_SHOOT_EMPTY,
	SHIELDGUN_RELOAD,
	SHIELDGUN_DRAW,
	SHIELDGUN_DRAWN_IDLE,
	SHIELDGUN_UP,
	SHIELDGUN_DOWN,
};

// custom
enum shieldgren_e
{
	SHIELDREN_IDLE = 4,
	SHIELDREN_UP,
	SHIELDREN_DOWN
};

WeaponIdType AliasToWeaponID(const char* alias);
const char* WeaponIDToAlias(int id);	// Given a weapon ID, return its alias
const char* AliasToWeaponClassname(const char* alias);
const char* WeaponIDToWeaponClassname(int id);
WeaponClassType WeaponIDToWeaponClass(WeaponIdType id);
bool IsPrimaryWeapon(int id);	// Return true if given weapon ID is a primary weapon
bool IsSecondaryWeapon(int id);	// Return true if given weapon ID is a secondary weapon
bool IsGrenadeWeapon(int id);	// Return true if given weapon ID is a grenade
const ItemInfo* GetWeaponInfo(int weaponID);
const ItemInfo* GetWeaponInfo(const char* weaponName);

inline WeaponClassType AliasToWeaponClass(const char* alias) { return WeaponIDToWeaponClass(AliasToWeaponID(alias)); }
