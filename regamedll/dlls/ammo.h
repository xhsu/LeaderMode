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

enum AmmoIdType
{
	AMMO_NONE = 0,
	AMMO_338Magnum = 1,
	AMMO_408CheyTac,
	AMMO_762Nato,
	AMMO_556NatoBox,
	AMMO_556Nato,
	AMMO_Buckshot,
	AMMO_45acp,
	AMMO_57mm,
	AMMO_50AE,
	AMMO_357SIG,
	AMMO_9mm,
	AMMO_46PDW,
	AMMO_Flashbang,
	AMMO_HEGrenade,
	AMMO_SmokeGrenade,
	AMMO_C4,

	AMMO_MAXTYPE
};

struct AmmoInfo
{
	AmmoIdType m_iId;
	const char* m_pszName;
	int m_iMax;
	int m_iCountPerBox;
	int m_iCostPerBox;
	int m_iBulletBehavior;
};

extern const AmmoInfo g_rgAmmoInfo[AMMO_MAXTYPE];

#define BULLET_NONE				0
#define BULLET_PLAYER_9MM		1
#define BULLET_PLAYER_BUCKSHOT	4
#define BULLET_PLAYER_CROWBAR	5	// used in knife
#define BULLET_PLAYER_45ACP		9
#define BULLET_PLAYER_338MAG	10
#define BULLET_PLAYER_762MM		11
#define BULLET_PLAYER_556MM		12
#define BULLET_PLAYER_50AE		13
#define BULLET_PLAYER_57MM		14
#define BULLET_PLAYER_357SIG	15

// only used in func_tank
#define BULLET_MONSTER_9MM		6
#define BULLET_MONSTER_MP5		7
#define BULLET_MONSTER_12MM		8

const AmmoInfo* GetAmmoInfo(const char* ammoName);
const AmmoInfo* GetAmmoInfo(int iId);
