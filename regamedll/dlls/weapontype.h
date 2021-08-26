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

#define ENGINE_WEAPON_LIMIT	32

#define SLOT_NO				0
#define PRIMARY_WEAPON_SLOT	1
#define PISTOL_SLOT			2
#define KNIFE_SLOT			3
#define GRENADE_SLOT		4
#define EQUIPMENT_SLOT		5

#define ITEM_FLAG_NONE				0
#define ITEM_FLAG_SELECTONEMPTY     1
#define ITEM_FLAG_NOAUTORELOAD      2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY 4
#define ITEM_FLAG_LIMITINWORLD      8
#define ITEM_FLAG_EXHAUSTIBLE       16 // A player can totally exhaust their ammo supply and lose this weapon

// the maximum amount of ammo each weapon's clip can hold
#define WEAPON_NOCLIP               -1

#define LOUD_GUN_VOLUME             1000
#define NORMAL_GUN_VOLUME           600
#define QUIET_GUN_VOLUME            200

#define BRIGHT_GUN_FLASH            512
#define NORMAL_GUN_FLASH            256
#define DIM_GUN_FLASH               128

#define BIG_EXPLOSION_VOLUME        2048
#define NORMAL_EXPLOSION_VOLUME     1024
#define SMALL_EXPLOSION_VOLUME      512

#define WEAPON_ACTIVITY_VOLUME      64


#define WPN_P	-1	// penalty: price * 2
#define WPN_F	0	// forbidden: price * 99999
#define WPN_A	1	// allowed: price * 1
#define WPN_D	2	// discounted: price * 0.5

enum WeaponIdType : BYTE
{
	WEAPON_NONE,

	// PISTOLS
	WEAPON_GLOCK18,
	WEAPON_USP,
	WEAPON_ANACONDA,
	WEAPON_DEAGLE,
	WEAPON_FIVESEVEN,
	WEAPON_M45A1,

	// SHOTGUNS
	WEAPON_KSG12,
	WEAPON_M1014,
	WEAPON_AA12,

	// SMGS
	WEAPON_MP7A1,
	WEAPON_MAC10,
	WEAPON_MP5N,
	WEAPON_UMP45,
	WEAPON_P90,
	WEAPON_VECTOR,

	// ASSAULT RIFLES
	WEAPON_AK47,
	WEAPON_M4A1,
	WEAPON_SCARH,
	WEAPON_XM8,

	// SNIPER RIFLES,
	WEAPON_SRS,
	WEAPON_SVD,
	WEAPON_AWP,
	WEAPON_PSG1,

	// MACHINE GUNS
	WEAPON_MK46,
	WEAPON_RPD,

	// Utils
	LAST_WEAPON,
	FIRST_PISTOL = WEAPON_GLOCK18,
	LAST_PISTOL = WEAPON_M45A1,
	FIRST_SHOTGUN = WEAPON_KSG12,
	LAST_SHOTGUN = WEAPON_AA12,
	FIRST_SMG = WEAPON_MP7A1,
	LAST_SMG = WEAPON_VECTOR,
	FIRST_AR = WEAPON_AK47,
	LAST_AR = WEAPON_XM8,
	FIRST_SR = WEAPON_SRS,
	LAST_SR = WEAPON_PSG1,
	FIRST_LMG = WEAPON_MK46,
	LAST_LMG = WEAPON_RPD,

	WEAPON_SHIELDGUN = 99,
};

constexpr auto g_bitsLastWeaponInCategory = (1 << LAST_PISTOL) | (1 << LAST_SHOTGUN) | (1 << LAST_SMG) | (1 << LAST_AR) | (1 << LAST_SR) | (1 << LAST_LMG);

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

enum WeaponState : uint32
{
	// Manager markers.
	WPNSTATE_DEAD				= BIT(0),	// mark for remove.

	// Melee
	WPNSTATE_MELEE				= BIT(1),	// mark for melee fight

	// Grenade
	WPNSTATE_QUICK_THROWING		= BIT(2),	// mark for using a throwable
	WPNSTATE_QT_RELEASE			= BIT(3),	// mark for the release of the throwable
	WPNSTATE_QT_SHOULD_SPAWN	= BIT(4),	// mark for spawning the selected GR.
	WPNSTATE_QT_EXIT			= BIT(5),	// mark for the end of QT seq.

	// Standard behaviours: Draw, Reload, Holster and Dash.
	WPNSTATE_DRAW_FIRST			= BIT(6),	// play draw_first?
	WPNSTATE_RELOAD_EMPTY		= BIT(7),	// play reload_empty?
	WPNSTATE_HOLSTERING			= BIT(8),	// disable all other behaviours?
	WPNSTATE_DASHING			= BIT(9),	// get a speed boost?
	WPNSTATE_NO_LHAND			= BIT(10),	// left hand is currently doing something...
	WPNSTATE_AUTO_LAND_UP		= BIT(11),	// left hand will automaticlly backup when the m_flNextAttack is up.
	WPNSTATE_PAUSED				= BIT(12),	// weapon are foced paused.
	WPNSTATE_AUTO_RESUME		= BIT(13),	// Automatically resume when some time variable reaches zero. (UNDONE, WPN_UNDONE)
	WPNSTATE_VISUAL_FREEZED		= BIT(14),	// A special case of WPNSTATE_PAUSED. In this case, the animation will freezed as long as data.

	// Weapon-specific flags.
	WPNSTATE_XM8_CHANGING		= BIT(15),

	// Sets
	WPNSTATE_SPECIAL_STATE		= WPNSTATE_XM8_CHANGING,
	WPNSTATE_BUSY				= WPNSTATE_MELEE | WPNSTATE_QUICK_THROWING | WPNSTATE_HOLSTERING | WPNSTATE_DASHING | WPNSTATE_SPECIAL_STATE | WPNSTATE_NO_LHAND | WPNSTATE_PAUSED,	// unable to do other 'busy' things.

	// Disused
	WPNSTATE_SHIELD_DRAWN		= BIT(31)	// disused, but reserved.
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

enum WeaponClassType : BYTE
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
enum AmmoIdType : BYTE;

// one of the core data structure.
struct WeaponInfo
{
	WeaponIdType	m_iId;
	const char*		m_pszInternalName;
	const char*		m_pszExternalName;
	AmmoIdType		m_iAmmoType;
	int				m_iMaxClip : 8;
	int				m_iSlot : 4;
	int				m_bitsFlags;
	int				m_iWeight : 8;
	WeaponClassType	m_iClassType;
	int				m_iCost : 16;
};

extern const WeaponInfo g_rgWpnInfo[LAST_WEAPON];

// equipments
enum EquipmentIdType : BYTE
{
	EQP_NONE = 0,

	// armour
	EQP_KEVLAR,
	EQP_ASSAULT_SUIT,

	// grenades
	EQP_HEGRENADE,
	EQP_FLASHBANG,
	EQP_SMOKEGRENADE,
	EQP_CRYOGRENADE,
	EQP_INCENDIARY_GR,
	EQP_HEALING_GR,
	EQP_GAS_GR,
	EQP_C4,

	// misc
	EQP_DETONATOR,
	EQP_NVG,
	EQP_FLASHLIGHT,

	// Utils
	EQP_COUNT,
	FIRST_ARMOUR = EQP_KEVLAR,
	LAST_ARMOUR = EQP_ASSAULT_SUIT,
	FIRST_GR = EQP_HEGRENADE,
	LAST_GR = EQP_C4,
	FIRST_MISC = EQP_DETONATOR,
	LAST_MISC = EQP_FLASHLIGHT,
};

#define ARMOR_NONE		0.0f	// No armor
#define ARMOR_KEVLAR	1.0f	// Body vest only
#define ARMOR_VESTHELM	2.0f	// Vest and helmet

constexpr float MAX_NORMAL_BATTERY = 100.0f;
constexpr float MAX_DIST_RELOAD_SOUND = 512.0f;

struct EquipmentInfo
{
	EquipmentIdType	m_iId;
	const char*		m_pszInternalName;
	const char*		m_pszExternalName;
	int				m_iCost;
};

extern const EquipmentInfo g_rgEquipmentInfo[EQP_COUNT];

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
const WeaponInfo* GetWeaponInfo(int weaponID);
const WeaponInfo* GetWeaponInfo(const char* weaponName);

inline WeaponClassType AliasToWeaponClass(const char* alias) { return WeaponIDToWeaponClass(AliasToWeaponID(alias)); }
