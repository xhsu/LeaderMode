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

#define DEFINE_ITEMINFO(id, classname)\
	{ 0, 0, "", 0, "", 0, classname, 0, id, 0, 0 }

enum ItemRestType
{
	ITEM_TYPE_BUYING,  // When a player is buying items
	ITEM_TYPE_TOUCHED, // When the player touches with a weaponbox or armoury_entity
	ITEM_TYPE_EQUIPPED // When an entity game_player_equip gives item to player or default items on player spawn
};

// constant items
#define ITEM_ID_ANTIDOTE 2
#define ITEM_ID_SECURITY 3

enum ItemID
{
	ITEM_NONE = -1,
	ITEM_KEVLAR,
	ITEM_ASSAULT,
	ITEM_LONGJUMP,
	ITEM_SODACAN,
	ITEM_HEALTHKIT,
	ITEM_ANTIDOTE,
	ITEM_BATTERY
};

class CItem: public CBaseEntity
{
public:
	virtual void Spawn();
	virtual CBaseEntity *Respawn();
	virtual BOOL MyTouch(CBasePlayer *pPlayer) { return FALSE; }

public:
	void EXPORT ItemTouch(CBaseEntity *pOther);
	void EXPORT Materialize();
};

class CWorldItem: public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void KeyValue(KeyValueData *pkvd);

public:
	int m_iType;
};

class CItemSuit: public CItem
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL MyTouch(CBasePlayer *pPlayer);
};

class CItemBattery: public CItem
{
public:
	static const float batteryCapacity;

public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL MyTouch(CBasePlayer *pPlayer);
};

class CItemAntidote: public CItem
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL MyTouch(CBasePlayer *pPlayer);
};

class CItemSecurity: public CItem
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL MyTouch(CBasePlayer *pPlayer);
};

class CItemLongJump: public CItem
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL MyTouch(CBasePlayer *pPlayer);
};

class CItemKevlar: public CItem
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL MyTouch(CBasePlayer *pPlayer);
};

class CItemAssaultSuit: public CItem
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual BOOL MyTouch(CBasePlayer *pPlayer);
};
