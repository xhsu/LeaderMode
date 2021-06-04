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

enum AmmoIdType : BYTE
{
	AMMO_NONE = 0,

	// Sniper & Hunter
	AMMO_338Magnum,
	AMMO_762SovietR,

	// Assault
	AMMO_762Nato,
	AMMO_556NatoBox,
	AMMO_556Nato,
	AMMO_762Soviet,
	AMMO_762SovietBox,

	// Buckshot
	AMMO_Buckshot,

	// PDWs
	AMMO_57mm,
	AMMO_46PDW,

	// High retarding force
	AMMO_45acp,
	AMMO_50AE,
	AMMO_44Magnum,
	AMMO_9mm,

	// Equipments
	AMMO_HEGrenade,
	AMMO_Flashbang,
	AMMO_SmokeGrenade,
	AMMO_Cryogrenade,
	AMMO_Molotov,
	AMMO_HealingGr,
	AMMO_GasGrenade,
	AMMO_C4,

	// util valus.
	AMMO_MAXTYPE,
	AMMO_THROWABLE_START = AMMO_HEGrenade,
	AMMO_THROWABLE_END = AMMO_GasGrenade,
};

struct AmmoInfo
{
	AmmoIdType m_iId;
	const char* m_pszName;
	int m_iMax : 16;
	int m_iCountPerBox : 16;
	int m_iCostPerBox : 16;
	int m_iPenetrationPower : 16;
	float m_flPenetrationDistance;
};

extern const AmmoInfo g_rgAmmoInfo[AMMO_MAXTYPE];

// only used in func_tank
#define BULLET_MONSTER_9MM		6
#define BULLET_MONSTER_MP5		7
#define BULLET_MONSTER_12MM		8

// only used in Sound.cpp
#define BULLET_PLAYER_CROWBAR	5	// used in knife

// dummy
enum EquipmentIdType : BYTE;

const AmmoInfo* GetAmmoInfo(const char* ammoName);
const AmmoInfo* GetAmmoInfo(int iId);
AmmoIdType GetAmmoIdOfEquipment(EquipmentIdType iId);
bool DescribeBulletTypeParameters(AmmoIdType iBulletType, int& iPenetrationPower, float& flPenetrationDistance);
