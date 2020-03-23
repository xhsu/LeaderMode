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

#include "precompiled.h"

bool HasDefaultPistol(CCSBot *me)
{
	auto *pSecondary = me->m_rgpPlayerItems[PISTOL_SLOT];

	if (!pSecondary)
		return false;

	if (pSecondary->m_iId == WEAPON_USP)	// LUNA: now this is the default starter weapon.
		return true;

	return false;
}

// Buy weapons, armor, etc.
void BuyState::OnEnter(CCSBot *me)
{
	m_retries = 0;
	m_prefRetries = 0;
	m_prefIndex = 0;

	m_doneBuying = false;
	m_isInitialDelay = true;

	// this will force us to stop holding live grenade
	me->EquipBestWeapon();

	m_buyDefuseKit = false;
	m_buyShield = false;

	if (me->m_iTeam == CT)
	{
		// determine if we want a tactical shield
		if (!me->m_bHasPrimary && TheCSBots()->AllowTacticalShield())
		{
			if (me->m_iAccount > 2500)
			{
				if (me->m_iAccount < 4000)
					m_buyShield = (RANDOM_FLOAT(0, 100.0f) < 33.3f) ? true : false;
				else
					m_buyShield = (RANDOM_FLOAT(0, 100.0f) < 10.0f) ? true : false;
			}
		}
	}

	if (TheCSBots()->AllowGrenades())
	{
		m_buyGrenade = (RANDOM_FLOAT(0.0f, 100.0f) < 33.3f) ? true : false;
	}
	else
	{
		m_buyGrenade = false;
	}

	m_buyPistol = false;

	if (TheCSBots()->AllowPistols())
	{
		auto *pSecondary = me->m_rgpPlayerItems[PISTOL_SLOT];

		// check if we have a pistol
		if (pSecondary)
		{
			// if we have our default pistol, think about buying a different one
			if (HasDefaultPistol(me))
			{
				// if everything other than pistols is disallowed, buy a pistol
				if (!TheCSBots()->AllowShotguns()
					&& !TheCSBots()->AllowSubMachineGuns()
					&& !TheCSBots()->AllowRifles()
					&& !TheCSBots()->AllowMachineGuns()
					&& !TheCSBots()->AllowTacticalShield()
					&& !TheCSBots()->AllowSnipers())
				{
					m_buyPistol = (RANDOM_FLOAT(0, 100) < 75.0f);
				}
				else if (me->m_iAccount < 1000)
				{
					// if we're low on cash, buy a pistol
					m_buyPistol = (RANDOM_FLOAT(0, 100) < 75.0f);
				}
				else
				{
					m_buyPistol = (RANDOM_FLOAT(0, 100) < 33.3f);
				}
			}
		}
		else
		{
			// we dont have a pistol - buy one
			m_buyPistol = true;
		}
	}
}

enum WeaponType
{
	PISTOL,
	SHOTGUN,
	SUB_MACHINE_GUN,
	RIFLE,
	MACHINE_GUN,
	SNIPER_RIFLE,
	GRENADE,
	NUM_WEAPON_TYPES,
};

struct BuyInfo
{
	WeaponType type;
	bool preferred;		// more challenging bots prefer these weapons
	WeaponIdType iId;	// use iId to buy this equipment
};

BuyInfo primaryWeapons[] =
{
	{ SHOTGUN,          false, WEAPON_KSG12		},
	{ SHOTGUN,          false, WEAPON_STRIKER	},

	{ SUB_MACHINE_GUN,  false, WEAPON_MP7A1		},
	{ SUB_MACHINE_GUN,  false, WEAPON_PM9		},
	{ SUB_MACHINE_GUN,  false, WEAPON_MP5N		},
	{ SUB_MACHINE_GUN,  false, WEAPON_UMP45		},
	{ SUB_MACHINE_GUN,  false, WEAPON_P90		},

	{ RIFLE,            true,  WEAPON_CM901		},
	{ RIFLE,            true,  WEAPON_QBZ95		},
	{ RIFLE,            true,  WEAPON_AK47		},
	{ RIFLE,            true,  WEAPON_M4A1		},
	{ RIFLE,            false, WEAPON_ACR		},
	{ RIFLE,            false, WEAPON_SCARL		},

	{ SNIPER_RIFLE,     false, WEAPON_M200		},
	{ SNIPER_RIFLE,     true,  WEAPON_M14EBR	},
	{ SNIPER_RIFLE,     true,  WEAPON_AWP		},
	{ SNIPER_RIFLE,     true,  WEAPON_SVD		},

	{ MACHINE_GUN,      false, WEAPON_MK46		},
};

BuyInfo secondaryWeapon[] =
{
	{ PISTOL, false,	WEAPON_GLOCK18	},
	{ PISTOL, false,	WEAPON_USP		},
	{ PISTOL, true,		WEAPON_ANACONDA	},
	{ PISTOL, true,		WEAPON_DEAGLE	},
	{ PISTOL, true,		WEAPON_FIVESEVEN},
	{ PISTOL, true,		WEAPON_P99		},
};

// Given a weapon alias, return the kind of weapon it is
inline WeaponType GetWeaponType(WeaponIdType iId)
{
	int i;
	for (i = 0; i < _countof(primaryWeapons); i++)
	{
		if (iId == primaryWeapons[i].iId)
			return primaryWeapons[i].type;
	}

	for (i = 0; i < _countof(secondaryWeapon); i++)
	{
		if (iId == secondaryWeapon[i].iId)
			return secondaryWeapon[i].type;
	}

	return NUM_WEAPON_TYPES;
}

void BuyState::OnUpdate(CCSBot *me)
{
	// wait for a Navigation Mesh
	if (!TheNavAreaList.size())
		return;

	// apparently we cant buy things in the first few seconds, so wait a bit
	if (m_isInitialDelay)
	{
		const float waitToBuyTime = 2.0f; // 0.25f;
		if (gpGlobals->time - me->GetStateTimestamp() < waitToBuyTime)
			return;

		m_isInitialDelay = false;
	}

	// if we're done buying and still in the freeze period, wait
	if (m_doneBuying)
	{
		if (CSGameRules()->IsMultiplayer() && CSGameRules()->IsFreezePeriod())
		{
			// make sure we're locked and loaded
			me->EquipBestWeapon(MUST_EQUIP);
			me->Reload();
			me->ResetStuckMonitor();

			return;
		}

		me->Idle();
		return;
	}

	// is the bot spawned outside of a buy zone?
	if (!(me->m_signals.GetState() & SIGNAL_BUY))
	{
		m_doneBuying = true;
		UTIL_DPrintf("%s bot spawned outside of a buy zone (%d, %d, %d)\n", (me->m_iTeam == CT) ? "CT" : "Terrorist", int(me->pev->origin.x), int(me->pev->origin.y), int(me->pev->origin.z));
		return;
	}

	// try to buy some weapons
	const float buyInterval = 0.2f; // 0.02f
	if (gpGlobals->time - me->GetStateTimestamp() > buyInterval)
	{
		me->m_stateTimestamp = gpGlobals->time;

		bool isPreferredAllDisallowed = true;

		// try to buy our preferred weapons first
		if (m_prefIndex < me->GetProfile()->GetWeaponPreferenceCount())
		{
			// need to retry because sometimes first buy fails??
			const int maxPrefRetries = 2;
			if (m_prefRetries >= maxPrefRetries)
			{
				// try to buy next preferred weapon
				m_prefIndex++;
				m_prefRetries = 0;
				return;
			}

			int weaponPreference = me->GetProfile()->GetWeaponPreference(m_prefIndex);

			// don't buy it again if we still have one from last round
			if (me->m_pActiveItem && me->m_pActiveItem->m_iId == weaponPreference)
			{
				// done with buying preferred weapon
				m_prefIndex = 9999;
				return;
			}

			if (me->HasShield() && weaponPreference == WEAPON_SHIELDGUN)
			{
				// done with buying preferred weapon
				m_prefIndex = 9999;
				return;
			}

			WeaponIdType buyWeaponId = WEAPON_NONE;
			if (weaponPreference == WEAPON_SHIELDGUN)
			{
				if (TheCSBots()->AllowTacticalShield())
					buyWeaponId = WEAPON_SHIELDGUN;
			}
			else
			{
				buyWeaponId = (WeaponIdType)weaponPreference;
				WeaponType type = GetWeaponType(buyWeaponId);

				switch (type)
				{
				case PISTOL:
					if (!TheCSBots()->AllowPistols())
						buyWeaponId = WEAPON_NONE;
					break;
				case SHOTGUN:
					if (!TheCSBots()->AllowShotguns())
						buyWeaponId = WEAPON_NONE;
					break;
				case SUB_MACHINE_GUN:
					if (!TheCSBots()->AllowSubMachineGuns())
						buyWeaponId = WEAPON_NONE;
					break;
				case RIFLE:
					if (!TheCSBots()->AllowRifles())
						buyWeaponId = WEAPON_NONE;
					break;
				case MACHINE_GUN:
					if (!TheCSBots()->AllowMachineGuns())
						buyWeaponId = WEAPON_NONE;
					break;
				case SNIPER_RIFLE:
					if (!TheCSBots()->AllowSnipers())
						buyWeaponId = WEAPON_NONE;
					break;
				}

				if (g_rgRoleWeaponsAccessibility[me->m_iRoleType][buyWeaponId] == WPN_F)
					buyWeaponId = WEAPON_NONE;
			}

			if (buyWeaponId != WEAPON_NONE)
			{
				BuyWeapon(me, buyWeaponId);
				me->PrintIfWatched("Tried to buy preferred weapon %s.\n", buyWeaponId == WEAPON_SHIELDGUN ? "Tactical Shield" : g_rgItemInfo[buyWeaponId].m_pszExternalName);

				isPreferredAllDisallowed = false;
			}

			m_prefRetries++;

			// bail out so we dont waste money on other equipment
			// unless everything we prefer has been disallowed, then buy at random
			if (isPreferredAllDisallowed == false)
				return;
		}

		// if we have no preferred primary weapon (or everything we want is disallowed), buy at random
		if (!me->m_bHasPrimary && (isPreferredAllDisallowed || !me->GetProfile()->HasPrimaryPreference()))
		{
			if (m_buyShield)
			{
				// buy a shield
				me->ClientCommand("shield");
				me->PrintIfWatched("Tried to buy a shield.\n");
			}
			else
			{
				// build list of allowable weapons to buy
				BuyInfo* masterPrimary = primaryWeapons;
				BuyInfo *stockPrimary[_countof(primaryWeapons)];
				int stockPrimaryCount = 0;

				// dont choose sniper rifles as often
				const float sniperRifleChance = 50.0f;
				bool wantSniper = (RANDOM_FLOAT(0, 100) < sniperRifleChance) ? true : false;

				for (int i = 0; i < _countof(primaryWeapons); i++)
				{
					if ((masterPrimary[i].type == SHOTGUN && TheCSBots()->AllowShotguns())
						|| (masterPrimary[i].type == SUB_MACHINE_GUN && TheCSBots()->AllowSubMachineGuns())
						|| (masterPrimary[i].type == RIFLE && TheCSBots()->AllowRifles())
						|| (masterPrimary[i].type == SNIPER_RIFLE && TheCSBots()->AllowSnipers() && wantSniper)
						|| (masterPrimary[i].type == MACHINE_GUN && TheCSBots()->AllowMachineGuns()))
					{
						stockPrimary[stockPrimaryCount++] = &masterPrimary[i];
					}
				}

				if (stockPrimaryCount)
				{
					// buy primary weapon if we don't have one
					int which = 0;

					// on hard difficulty levels, bots try to buy preferred weapons on the first pass
					if (m_retries == 0 && TheCSBots()->GetDifficultyLevel() >= BOT_HARD)
					{
						// count up available preferred weapons
						int prefCount = 0;
						for (which = 0; which < stockPrimaryCount; which++)
						{
							if (stockPrimary[which]->preferred)
								prefCount++;
						}

						if (prefCount)
						{
							int whichPref = RANDOM_LONG(0, prefCount - 1);
							for (which = 0; which < stockPrimaryCount; which++)
							{
								if (stockPrimary[which]->preferred && whichPref-- == 0)
									break;
							}
						}
						else
						{
							// no preferred weapons available, just pick randomly
							which = RANDOM_LONG(0, stockPrimaryCount - 1);
						}
					}
					else
					{
						which = RANDOM_LONG(0, stockPrimaryCount - 1);
					}

					BuyWeapon(me, stockPrimary[which]->iId);
					me->PrintIfWatched("Tried to buy %s.\n", g_rgItemInfo[stockPrimary[which]->iId].m_pszExternalName);
				}
			}
		}

		// If we now have a weapon, or have tried for too long, we're done
		if (me->m_bHasPrimary || m_retries++ > 5)
		{
			// primary ammo
			if (me->m_bHasPrimary)
			{
				BuyAmmo(me, PRIMARY_WEAPON_SLOT, false);
			}

			// buy armor last, to make sure we bought a weapon first
			BuyEquipment(me, EQP_ASSAULT_SUIT);
			BuyEquipment(me, EQP_KEVLAR);

			// pistols - if we have no preferred pistol, buy at random
			if (TheCSBots()->AllowPistols() && !me->GetProfile()->HasPistolPreference())
			{
				if (m_buyPistol)
				{
					int which = RANDOM_LONG(0, _countof(secondaryWeapon) - 1);
					BuyWeapon(me, secondaryWeapon[which].iId);

					// only buy one pistol
					m_buyPistol = false;
				}

				BuyAmmo(me, PISTOL_SLOT, false);
			}

			// buy a grenade if we wish, and we don't already have one
			if (m_buyGrenade && !me->HasGrenade())
			{
				if (UTIL_IsTeamAllBots(me->m_iTeam))
				{
					// only allow Flashbangs if everyone on the team is a bot (dont want to blind our friendly humans)
					float rnd = RANDOM_FLOAT(0, 100);

					if (rnd < 10.0f)
					{
						// smoke grenade
						BuyEquipment(me, EQP_SMOKEGRENADE);
					}
					else if (rnd < 35.0f)
					{
						// flashbang
						BuyEquipment(me, EQP_FLASHBANG);
					}
					else
					{
						// he grenade
						BuyEquipment(me, EQP_HEGRENADE);
					}
				}
				else
				{
					if (RANDOM_FLOAT(0, 100) < 10.0f)
					{
						// smoke grenade
						BuyEquipment(me, EQP_SMOKEGRENADE);
					}
					else
					{
						// he grenade
						BuyEquipment(me, EQP_HEGRENADE);
					}
				}
			}

			if (m_buyDefuseKit)
			{
				// disused.
				//BuyEquipment(me, MENU_SLOT_ITEM_DEFUSEKIT);
			}

			m_doneBuying = true;
		}
	}
}

void BuyState::OnExit(CCSBot *me)
{
	me->ResetStuckMonitor();
	me->EquipBestWeapon();
}
