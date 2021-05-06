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

bool gInitHUD = true;

TYPEDESCRIPTION CBasePlayer::m_playerSaveData[] =
{
	DEFINE_FIELD(CBasePlayer, m_flFlashLightTime, FIELD_TIME),
	DEFINE_FIELD(CBasePlayer, m_iFlashBattery, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_afButtonLast, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_afButtonPressed, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_afButtonReleased, FIELD_INTEGER),
	DEFINE_ARRAY(CBasePlayer, m_rgItems, FIELD_INTEGER, MAX_ITEMS),
	DEFINE_FIELD(CBasePlayer, m_afPhysicsFlags, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_flTimeStepSound, FIELD_TIME),
	DEFINE_FIELD(CBasePlayer, m_flSwimTime, FIELD_TIME),
	DEFINE_FIELD(CBasePlayer, m_flDuckTime, FIELD_TIME),
	DEFINE_FIELD(CBasePlayer, m_flSuitUpdate, FIELD_TIME),
	DEFINE_ARRAY(CBasePlayer, m_rgSuitPlayList, FIELD_INTEGER, MAX_SUIT_PLAYLIST),
	DEFINE_FIELD(CBasePlayer, m_iSuitPlayNext, FIELD_INTEGER),
	DEFINE_ARRAY(CBasePlayer, m_rgiSuitNoRepeat, FIELD_INTEGER, MAX_SUIT_NOREPEAT),
	DEFINE_ARRAY(CBasePlayer, m_rgflSuitNoRepeatTime, FIELD_TIME, MAX_SUIT_NOREPEAT),
	DEFINE_FIELD(CBasePlayer, m_lastDamageAmount, FIELD_INTEGER),
	DEFINE_ARRAY(CBasePlayer, m_rgpPlayerItems, FIELD_CLASSPTR, MAX_ITEM_TYPES),
	DEFINE_FIELD(CBasePlayer, m_pActiveItem, FIELD_CLASSPTR),
	DEFINE_FIELD(CBasePlayer, m_pLastItem, FIELD_CLASSPTR),
	DEFINE_ARRAY(CBasePlayer, m_rgAmmo, FIELD_INTEGER, MAX_AMMO_SLOTS),
	DEFINE_FIELD(CBasePlayer, m_idrowndmg, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_idrownrestored, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_iTrain, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_bitsHUDDamage, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_flFallVelocity, FIELD_FLOAT),
	DEFINE_FIELD(CBasePlayer, m_iTargetVolume, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_iWeaponVolume, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_iExtraSoundTypes, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_iWeaponFlash, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_fLongJump, FIELD_BOOLEAN),
	DEFINE_FIELD(CBasePlayer, m_fInitHUD, FIELD_BOOLEAN),
	DEFINE_FIELD(CBasePlayer, m_tbdPrev, FIELD_TIME),
	DEFINE_FIELD(CBasePlayer, m_pTank, FIELD_EHANDLE),
	DEFINE_FIELD(CBasePlayer, m_iHideHUD, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_flDisplayHistory, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayer, m_iJoiningState, FIELD_INTEGER),
};

const char *CDeadHEV::m_szPoses[] =
{
	"deadback",
	"deadsitting",
	"deadstomach",
	"deadtable"
};

entvars_t *g_pevLastInflictor;

LINK_ENTITY_TO_CLASS(player, CBasePlayer)

CBasePlayer::~CBasePlayer()
{
	m_hintMessageQueue.Reset();
}

void CBasePlayer::SendItemStatus()
{
	int itemStatus = 0;
	if (m_bHasNightVision)
		itemStatus |= ITEM_STATUS_NIGHTVISION;
	if (m_rgbHasEquipment[EQP_DETONATOR])
		itemStatus |= ITEM_STATUS_DETONATOR;

	MESSAGE_BEGIN(MSG_ONE, gmsgItemStatus, nullptr, pev);
		WRITE_BYTE(itemStatus);
	MESSAGE_END();
}

bool EXT_FUNC CBasePlayer::SetClientUserInfoName(char *infobuffer, char *szNewName)
{
	int nClientIndex = entindex();

	if (IsProxy())
	{
		SET_CLIENT_KEY_VALUE(nClientIndex, infobuffer, "name", szNewName);
		return true;
	}

	if (pev->deadflag != DEAD_NO)
	{
		m_bHasChangedName = true;
		Q_snprintf(m_szNewName, sizeof(m_szNewName), "%s", szNewName);
		ClientPrint(pev, HUD_PRINTTALK, "#Name_change_at_respawn");
		return false;
	}

	// Set the name
	SET_CLIENT_KEY_VALUE(nClientIndex, infobuffer, "name", szNewName);

	MESSAGE_BEGIN(MSG_BROADCAST, gmsgSayText);
		WRITE_BYTE(nClientIndex);
		WRITE_STRING("#Cstrike_Name_Change");
		WRITE_STRING(STRING(pev->netname));
		WRITE_STRING(szNewName);
	MESSAGE_END();

	UTIL_LogPrintf("\"%s<%i><%s><%s>\" changed name to \"%s\"\n", STRING(pev->netname), GETPLAYERUSERID(edict()), GETPLAYERAUTHID(edict()), GetTeam(m_iTeam), szNewName);
	return true;
}

void CBasePlayer::SetClientUserInfoModel(char *infobuffer, char *szNewModel)
{
	if (szNewModel == nullptr)
		return;

	if (Q_strcmp(GET_KEY_VALUE(infobuffer, "model"), szNewModel) != 0)
	{
		SET_CLIENT_KEY_VALUE(entindex(), infobuffer, "model", szNewModel);
	}
}

void CBasePlayer::SetPlayerModel()
{
	char *infobuffer = GET_INFO_BUFFER(edict());
	char *model;

	if (m_iTeam == CT)
	{
		switch (m_iModelName)
		{
		case MODEL_URBAN:
			model = "urban";
			break;
		case MODEL_GSG9:
			model = "gsg9";
			break;
		case MODEL_GIGN:
			model = "gign";
			break;
		case MODEL_SAS:
			model = "sas";
			break;
		case MODEL_VIP:
			model = "vip";
			break;
		case MODEL_SPETSNAZ:
			if (AreRunningCZero())
			{
				model = "spetsnaz";
				break;
			}
		default:
		{
			if (IsBot())
			{
				model = (char *)TheBotProfiles->GetCustomSkinModelname(m_iModelName);
				if (!model)
					model = "urban";
			}
			else
				model = "urban";

			break;
		}
		}
	}
	else if (m_iTeam == TERRORIST)
	{
		switch (m_iModelName)
		{
		case MODEL_TERROR:
			model = "terror";
			break;
		case MODEL_LEET:
			model = "leet";
			break;
		case MODEL_ARCTIC:
			model = "arctic";
			break;
		case MODEL_GUERILLA:
			model = "guerilla";
			break;
		case MODEL_MILITIA:
			if (AreRunningCZero())
			{
				model = "militia";
				break;
			}
		default:
		{
			if (IsBot())
			{
				model = (char *)TheBotProfiles->GetCustomSkinModelname(m_iModelName);
				if (!model)
					model = "terror";
			}
			else
				model = "terror";

			break;
		}
		}
	}
	else
		model = "urban";

	SetClientUserInfoModel(infobuffer, model);
}

CBasePlayer *CBasePlayer::GetNextRadioRecipient(CBasePlayer *pStartPlayer)
{
	CBaseEntity *pEntity = static_cast<CBaseEntity *>(pStartPlayer);
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
	{
		if (FNullEnt(pEntity->edict()))
			break;

		bool bSend = false;
		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);

		if (pEntity->IsPlayer())
		{
			if (pEntity->IsDormant())
				continue;

			if (pPlayer && pPlayer->m_iTeam == m_iTeam)
				bSend = true;
		}
		else if (pPlayer)
		{
			int iSpecMode = GetObserverMode();
			if (iSpecMode != OBS_CHASE_LOCKED && iSpecMode != OBS_CHASE_FREE && iSpecMode != OBS_IN_EYE)
				continue;

			if (!FNullEnt(m_hObserverTarget))
				continue;

			CBasePlayer *pTarget = CBasePlayer::Instance(pPlayer->m_hObserverTarget->pev);
			if (pTarget && pTarget->m_iTeam == m_iTeam)
			{
				bSend = true;
			}
		}

		if (bSend)
		{
			return pPlayer;
		}
	}

	return nullptr;
}

void EXT_FUNC CBasePlayer::Radio(const char *msg_id, const char *msg_verbose, short pitch, bool showIcon)
{
	// Spectators don't say radio messages.
	if (!IsPlayer())
		return;

	// Neither do dead guys.
	if (pev->deadflag != DEAD_NO && !IsBot())
		return;

	CBaseEntity *pEntity = nullptr;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
	{
		if (FNullEnt(pEntity->edict()))
			break;

		bool bSend = false;
		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);

		if (!pPlayer)
			continue;

		// ignorerad command
		if (pPlayer->m_bIgnoreRadio)
			continue;

		// are we a regular player? (not spectator)
		if (pPlayer->IsPlayer())
		{
			if (pPlayer->IsDormant())
				continue;

			// is this player on our team? (even dead players hear our radio calls)
			if (pPlayer->m_iTeam == m_iTeam)
				bSend = true;
		}
		// this means we're a spectator
		else
		{
			// do this when spectator mode is in
			int iSpecMode = pPlayer->GetObserverMode();
			if (iSpecMode != OBS_CHASE_LOCKED && iSpecMode != OBS_CHASE_FREE && iSpecMode != OBS_IN_EYE)
				continue;

			if (FNullEnt(pPlayer->m_hObserverTarget))
				continue;

			if (pPlayer->m_hObserverTarget && pPlayer->m_hObserverTarget->m_iTeam == m_iTeam)
			{
				bSend = true;
			}
		}

		if (bSend)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgSendAudio, nullptr, pEntity->pev);
				WRITE_BYTE(ENTINDEX(edict()));
				WRITE_STRING(msg_id);
				WRITE_SHORT(pitch);
			MESSAGE_END();

			// radio message icon
			if (msg_verbose)
			{
				// search the place name where is located the player
				const char *placeName = nullptr;
				if (AreRunningCZero() && TheBotPhrases)
				{
					Place playerPlace = TheNavAreaGrid.GetPlace(&pev->origin);
					const BotPhraseList *placeList = TheBotPhrases->GetPlaceList();
					for (auto phrase : *placeList)
					{
						if (phrase->GetID() == playerPlace)
						{
							placeName = phrase->GetName();
							break;
						}
					}
				}
				if (placeName)
					ClientPrint(pEntity->pev, HUD_PRINTRADIO, NumAsString(entindex()), "#Game_radio_location", STRING(pev->netname), placeName, msg_verbose);
				else
					ClientPrint(pEntity->pev, HUD_PRINTRADIO, NumAsString(entindex()), "#Game_radio", STRING(pev->netname), msg_verbose);
			}

			// icon over the head for teammates
			if (showIcon && show_radioicon.value)
			{
					// put an icon over this guys head to show that he used the radio
					MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, nullptr, pEntity->pev);
						WRITE_BYTE(TE_PLAYERATTACHMENT);
						WRITE_BYTE(ENTINDEX(edict()));		// byte	(entity index of player)
						WRITE_COORD(35);					// coord (vertical offset) ( attachment origin.z = player origin.z + vertical offset)
						WRITE_SHORT(g_sModelIndexRadio);	// short (model index) of tempent
						WRITE_SHORT(15);					// short (life * 10 ) e.g. 40 = 4 seconds
					MESSAGE_END();
			}
		}
	}
}

void CBasePlayer::SmartRadio()
{
	;
}

void CBasePlayer::Pain(int iLastHitGroup, bool bHasArmour)
{
	int temp = RANDOM_LONG(0, 2);

	if (iLastHitGroup == HITGROUP_HEAD)
	{
		if (pev->armortype == ARMOR_VESTHELM)
		{
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/bhit_helmet-1.wav", VOL_NORM, ATTN_NORM);
			return;
		}

		switch (temp)
		{
		case 0:  EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/headshot1.wav", VOL_NORM, ATTN_NORM); break;
		case 1:  EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/headshot2.wav", VOL_NORM, ATTN_NORM); break;
		default: EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/headshot3.wav", VOL_NORM, ATTN_NORM); break;
		}
	}
	else
	{
		if (iLastHitGroup != HITGROUP_LEFTLEG && iLastHitGroup != HITGROUP_RIGHTLEG)
		{
			if (bHasArmour)
			{
				EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/bhit_kevlar-1.wav", VOL_NORM, ATTN_NORM);
				return;
			}
		}

		switch (temp)
		{
		case 0:  EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/bhit_flesh-1.wav", VOL_NORM, ATTN_NORM); break;
		case 1:  EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/bhit_flesh-2.wav", VOL_NORM, ATTN_NORM); break;
		default: EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/bhit_flesh-3.wav", VOL_NORM, ATTN_NORM); break;
		}
	}
}

NOXREF Vector VecVelocityForDamage(float flDamage)
{
	Vector vec(RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(-100, 100), RANDOM_FLOAT(200, 300));

	if (flDamage > -50.0f)
		vec = vec * 0.7f;
	else if (flDamage > -200.0f)
		vec = vec * 2.0f;
	else
		vec = vec * 10.0f;

	return vec;
}

int TrainSpeed(int iSpeed, int iMax)
{
	float fMax;
	float fSpeed;
	int iRet = 0;

	fMax = float(iMax);
	fSpeed = iSpeed / fMax;

	if (iSpeed < 0)
		iRet = TRAIN_BACK;
	else if (iSpeed == 0)
		iRet = TRAIN_NEUTRAL;
	else if (fSpeed < 0.33f)
		iRet = TRAIN_SLOW;
	else if (fSpeed < 0.66f)
		iRet = TRAIN_MEDIUM;
	else
		iRet = TRAIN_FAST;

	return iRet;
}

void CBasePlayer::DeathSound()
{
	// temporarily using pain sounds for death sounds
	switch (RANDOM_LONG(1, 4))
	{
	case 1: EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/die1.wav", VOL_NORM, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/die2.wav", VOL_NORM, ATTN_NORM); break;
	case 3: EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/die3.wav", VOL_NORM, ATTN_NORM); break;
	case 4: EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/death6.wav", VOL_NORM, ATTN_NORM); break;
	}
}

// override takehealth
// bitsDamageType indicates type of damage healed.
BOOL EXT_FUNC CBasePlayer::TakeHealth(float flHealth, int bitsDamageType)
{
	if (pev->takedamage == DAMAGE_NO)
		return FALSE;

	// clear out any damage types we healed.
	m_bitsDamageType &= ~(bitsDamageType & ~DMG_TIMEBASED);

	if (bitsDamageType & HEALING_REMOVE_DOT)
	{
		if (m_flFrozenNextThink > 0.0f)
			gFrozenDOTMgr::Free(this);

		if (m_flElectrifyTimeUp > 0.0f)
			gElectrifiedDOTMgr::Free(this);

		if (m_flPoisonedTimeUp > 0.0f)
			gPoisonDOTMgr::Free(this);

		if (m_flBurningTimeUp > 0.0f)
			gBurningDOTMgr::Free(this);
	}

	if (bitsDamageType & HEALING_NO_OH)
	{
		if (pev->health >= pev->max_health)
			return FALSE;

		pev->health = Q_min(pev->health + flHealth, pev->max_health);
		return TRUE;	// never move to the later part if overhealing is not allowed.
	}

	if (m_iRoleType == Role_Commander || m_iRoleType == Role_Godfather)
	{
		// save current HP only if the last overhealing was depleted.
		if (m_flOHNextThink <= 0.0f)
			m_flOHOriginalHealth = pev->health;

		m_flOHNextThink = gpGlobals->time + gOverHealingMgr::OVERHEALING_DECAY_INTERVAL;

		// finally, we heal. (temporary)
		pev->health = Q_min(pev->health + flHealth, pev->max_health * gOverHealingMgr::OVERHEALING_MAX_RATIO);
	}
	else
	{
		pev->health = Q_min(pev->health + flHealth, pev->max_health * gOverHealingMgr::OVERHEALING_MAX_RATIO);

		if (pev->health > pev->max_health)
		{
			m_flOHOriginalHealth = pev->max_health;
			m_flOHNextThink = gpGlobals->time + gOverHealingMgr::OVERHEALING_DECAY_INTERVAL;
		}
	}

	return (pev->health - flHealth < pev->max_health * gOverHealingMgr::OVERHEALING_MAX_RATIO);	// if the healing is capped, consider it is a failure.
}

Vector CBasePlayer::GetGunPosition()
{
	return pev->origin + pev->view_ofs;
}

bool CBasePlayer::IsHittingShield(Vector &vecDirection, TraceResult *ptr)
{
	if (!HasShield())
		return false;

	if (ptr->iHitgroup == HITGROUP_SHIELD)
		return true;

	if (m_bShieldDrawn)
		UTIL_MakeVectors(pev->angles);

	return false;
}

void CBasePlayer::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	bool bShouldBleed = true;
	bool bShouldSpark = false;
	bool bHitShield = IsHittingShield(vecDir, ptr);

	CBasePlayer *pAttacker = CBasePlayer::Instance(pevAttacker);

	if (pAttacker && pAttacker->IsPlayer())
	{
		if (!CSGameRules()->FPlayerCanTakeDamage(this, pAttacker))
			bShouldBleed = false;
	}

	if (pev->takedamage == DAMAGE_NO)
		return;

	m_LastHitGroup = ptr->iHitgroup;

	if (bHitShield)
	{
		flDamage = 0;
		bShouldBleed = false;

		if (RANDOM_LONG(0, 1))
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/ric_metal-1.wav", VOL_NORM, ATTN_NORM);
		else
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/ric_metal-2.wav", VOL_NORM, ATTN_NORM);

		UTIL_Sparks(ptr->vecEndPos);

		pev->punchangle.x = flDamage * RANDOM_FLOAT(-0.15, 0.15);
		pev->punchangle.z = flDamage * RANDOM_FLOAT(-0.15, 0.15);

		if (pev->punchangle.x < 4)
			pev->punchangle.x = -4;

		if (pev->punchangle.z < -5)
			pev->punchangle.z = -5;

		else if (pev->punchangle.z > 5)
			pev->punchangle.z = 5;
	}
	else
	{
		switch (ptr->iHitgroup)
		{
		case HITGROUP_GENERIC:
			break;

		case HITGROUP_HEAD:
		{
			if (pev->armortype == ARMOR_VESTHELM)
			{
				bShouldBleed = false;
				bShouldSpark = true;
				flDamage *= 2.0f;
			}
			else
				flDamage *= 6.0f;

			if (bShouldBleed)
			{
				pev->punchangle.x = flDamage * -0.5;

				if (pev->punchangle.x < -12)
					pev->punchangle.x = -12;

				pev->punchangle.z = flDamage * RANDOM_FLOAT(-1, 1);

				if (pev->punchangle.z < -9)
					pev->punchangle.z = -9;

				else if (pev->punchangle.z > 9)
					pev->punchangle.z = 9;
			}
			break;
		}
		case HITGROUP_CHEST:
		{
			flDamage *= 1;

			if (pev->armortype != ARMOR_NONE)
				bShouldBleed = false;

			else if (bShouldBleed)
			{
				pev->punchangle.x = flDamage * -0.1;

				if (pev->punchangle.x < -4)
					pev->punchangle.x = -4;
			}
			break;
		}
		case HITGROUP_STOMACH:
		{
			flDamage *= 1.25;

			if (pev->armortype != ARMOR_NONE)
				bShouldBleed = false;

			else if (bShouldBleed)
			{
				pev->punchangle.x = flDamage * -0.1;

				if (pev->punchangle.x < -4)
					pev->punchangle.x = -4;
			}
			break;
		}
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
		{
			if (pev->armortype != ARMOR_NONE)
				bShouldBleed = false;

			break;
		}
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
		{
			flDamage *= 0.75;
			break;
		}
		}
	}

	if (bShouldBleed)
	{
		BloodSplat(ptr->vecEndPos, vecDir, ptr->iHitgroup, flDamage * 5);
		SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage);			// a little surface blood.
		TraceBleed(flDamage, vecDir, ptr, bitsDamageType);
	}
	// they hit a helmet
	else if (ptr->iHitgroup == HITGROUP_HEAD && bShouldSpark)
	{
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, ptr->vecEndPos);
			WRITE_BYTE(TE_STREAK_SPLASH);
			WRITE_COORD(ptr->vecEndPos.x);
			WRITE_COORD(ptr->vecEndPos.y);
			WRITE_COORD(ptr->vecEndPos.z);
			WRITE_COORD(ptr->vecPlaneNormal.x);
			WRITE_COORD(ptr->vecPlaneNormal.y);
			WRITE_COORD(ptr->vecPlaneNormal.z);
			WRITE_BYTE(5); // color
			WRITE_SHORT(22); // count
			WRITE_SHORT(25); // base speed
			WRITE_SHORT(65); // ramdon velocity
		MESSAGE_END();
	}

	OnTraceDamagePre(flDamage, *ptr);	// hook it right before AddMultiDamage();

	AddMultiDamage(pevAttacker, this, flDamage, bitsDamageType);
}

const char *GetWeaponName(entvars_t *pevInflictor, entvars_t *pKiller)
{
	// by default, the player is killed by the world
	const char *killer_weapon_name = "world";

	// Is the killer a client?
	if (pKiller->flags & FL_CLIENT)
	{
		if (pevInflictor)
		{
			if (pevInflictor == pKiller)
			{
				// If the inflictor is the killer, then it must be their current weapon doing the damage
				CBasePlayer *pAttacker = CBasePlayer::Instance(pKiller);
				if (pAttacker && pAttacker->IsPlayer())
				{
					if (pAttacker->m_pActiveItem)
						killer_weapon_name = pAttacker->m_pActiveItem->m_pItemInfo->m_pszInternalName;
				}
			}
			else
			{
				// it's just that easy
				killer_weapon_name = STRING(pevInflictor->classname);
			}
		}
	}
	else if (pevInflictor)
	{
		killer_weapon_name = STRING(pevInflictor->classname);
	}

	// strip the monster_* or weapon_* from the inflictor's classname
	const char cut_weapon[] = "weapon_";
	const char cut_monster[] = "monster_";
	const char cut_func[] = "func_";

	if (!Q_strncmp(killer_weapon_name, cut_weapon, sizeof(cut_weapon) - 1))
		killer_weapon_name += sizeof(cut_weapon) - 1;

	else if (!Q_strncmp(killer_weapon_name, cut_monster, sizeof(cut_monster) - 1))
		killer_weapon_name += sizeof(cut_monster) - 1;

	else if (!Q_strncmp(killer_weapon_name, cut_func, sizeof(cut_func) - 1))
		killer_weapon_name += sizeof(cut_func) - 1;

	return killer_weapon_name;
}

void LogAttack(CBasePlayer *pAttacker, CBasePlayer *pVictim, int teamAttack, int healthHit, int armorHit, int newHealth, int newArmor, const char *killer_weapon_name)
{
	int detail = logdetail.value;

	if (!detail)
		return;

	if (!pAttacker || !pVictim)
		return;

	if ((teamAttack && (detail & LOG_TEAMMATEATTACK)) || (!teamAttack && (detail & LOG_ENEMYATTACK)))
	{
		UTIL_LogPrintf("\"%s<%i><%s><%s>\" attacked \"%s<%i><%s><%s>\" with \"%s\" (damage \"%d\") (damage_armor \"%d\") (health \"%d\") (armor \"%d\")\n",
			STRING(pAttacker->pev->netname), GETPLAYERUSERID(pAttacker->edict()), GETPLAYERAUTHID(pAttacker->edict()), GetTeam(pAttacker->m_iTeam),
			STRING(pVictim->pev->netname), GETPLAYERUSERID(pVictim->edict()), GETPLAYERAUTHID(pVictim->edict()),
			GetTeam(pVictim->m_iTeam), killer_weapon_name, healthHit, armorHit, newHealth, newArmor);
	}
}

// Take some damage.
// RETURN: TRUE took damage, FALSE otherwise
// NOTE: each call to TakeDamage with bitsDamageType set to a time-based damage
// type will cause the damage time countdown to be reset.  Thus the ongoing effects of poison, radiation
// etc are implemented with subsequent calls to TakeDamage using DMG_GENERIC.
BOOL CBasePlayer::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	BOOL bTookDamage;
	float flRatio = ARMOR_RATIO;
	float flBonus = ARMOR_BONUS;
	int iGunType = 0;
	float flShieldRatio = 0;
	BOOL bTeamAttack = FALSE;
	int armorHit = 0;
	CBasePlayer *pAttack = nullptr;

	if (bitsDamageType & (DMG_EXPLOSION | DMG_BLAST | DMG_FALL))
		m_LastHitGroup = HITGROUP_GENERIC;

	else if (m_LastHitGroup == HITGROUP_SHIELD && (bitsDamageType & DMG_BULLET))
		return FALSE;

	if (HasShield())
		flShieldRatio = 0.2;

	// pre damage modifier. for skills
	flDamage *= PlayerDamageSufferedModifier(bitsDamageType);

	if (!FNullEnt(pevAttacker) && CBaseEntity::Instance(pevAttacker)->IsPlayer())
	{
		pAttack = CBasePlayer::Instance(pevAttacker);
		flDamage *= pAttack->PlayerDamageDealtModifier(bitsDamageType);
	}

	if (bitsDamageType & (DMG_EXPLOSION | DMG_BLAST))
	{
		if (!IsAlive())
			return FALSE;

		if (bitsDamageType & DMG_EXPLOSION)
		{
			CBaseEntity *temp = GetClassPtr((CBaseEntity *)pevInflictor);
			if (FClassnameIs(temp->pev, "grenade"))
			{
				CGrenade *pGrenade = GetClassPtr((CGrenade *)pevInflictor);

				pAttack = CBasePlayer::Instance(pevAttacker);

				if (!CSGameRules()->IsFreeForAll() && pGrenade->m_iTeam == m_iTeam)
				{
					if (friendlyfire.value)
					{
						bTeamAttack = TRUE;
						flDamage *= clamp(((pAttack == this) ?
							ff_damage_reduction_grenade_self.value :
							ff_damage_reduction_grenade.value), 0.0f, 1.0f);
					}
					else if (pAttack == this)
					{
						flDamage *= clamp(ff_damage_reduction_grenade_self.value, 0.0f, 1.0f);
					}
					else
					{
						// if cvar friendlyfire is disabled
						// and if the victim is teammate then ignore this damage
						return FALSE;
					}
				}
			}
		}

		if (!FNullEnt(ENT(pevInflictor)))
			m_vBlastVector = pev->origin - pevInflictor->origin;

		if (pev->armorvalue != 0.0f && IsArmored(m_LastHitGroup))
		{
			real_t flNew = flRatio * flDamage;
			real_t flArmor = (flDamage - flNew) * flBonus;

			// Does this use more armor than we have?
			if (flArmor > pev->armorvalue)
			{
				flNew = flDamage - pev->armorvalue;
				armorHit = flArmor;
				pev->armorvalue = 0;
			}
			else
			{
				int oldValue = pev->armorvalue;

				if (flArmor < 0.0)
					flArmor = 1.0;

				pev->armorvalue -= flArmor;
				armorHit = oldValue - pev->armorvalue;
			}

			flDamage = flNew;

			if (pev->armorvalue <= 0.0)
				pev->armortype = ARMOR_NONE;

			Pain(m_LastHitGroup, true);
		}
		else
		{
			Pain(m_LastHitGroup, false);
		}

		m_lastDamageAmount = flDamage;

		if (pev->health > flDamage)
		{
			SetAnimation(PLAYER_FLINCH);
			Pain(m_LastHitGroup, false);
		}
		else
		{
			if (bitsDamageType & DMG_EXPLOSION)
				m_bKilledByGrenade = true;
		}

		OnPlayerDamagedPre(pevInflictor, pevAttacker, flDamage, bitsDamageType);	// skills should hook on here.

		if (!FNullEnt(pAttack))
			pAttack->OnHurtingAnotherPlayer(this, pevInflictor, flDamage, bitsDamageType);

		LogAttack(pAttack, this, bTeamAttack, int(flDamage), armorHit, pev->health - flDamage, pev->armorvalue, GetWeaponName(pevInflictor, pevAttacker));
		bTookDamage = CBaseMonster::TakeDamage(pevInflictor, pevAttacker, int(flDamage), bitsDamageType);

		if (bTookDamage)
		{
			if (TheBots)
			{
				TheBots->OnEvent(EVENT_PLAYER_TOOK_DAMAGE, this, pAttack);
			}
		}

		// reset damage time countdown for each type of time based damage player just sustained
		for (int i = 0; i < ITBD_END; i++)
		{
			if (bitsDamageType & (DMG_PARALYZE << i))
				m_rgbTimeBasedDamage[i] = 0;
		}

		// damage money.
		if (!FNullEnt(pAttack))
			pAttack->AddAccount(CSGameRules()->IDamageMoney(this, pAttack, flDamage), RT_HURTING_ENEMY);

		// tell director about it
		MESSAGE_BEGIN(MSG_SPEC, SVC_DIRECTOR);
			WRITE_BYTE(9);								// command length in bytes
			WRITE_BYTE(DRC_CMD_EVENT);					// take damage event
			WRITE_SHORT(ENTINDEX(edict()));				// index number of primary entity
			WRITE_SHORT(ENTINDEX(ENT(pevInflictor)));	// index number of secondary entity
			WRITE_LONG(5);								// eventflags (priority and flags)
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_SPEC, gmsgHLTV);
			WRITE_BYTE(ENTINDEX(edict()));
			WRITE_BYTE(int(Q_max(pev->health, 0.0f)) | DRC_FLAG_FACEPLAYER);
		MESSAGE_END();

		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);

			if (!pPlayer || pPlayer->m_hObserverTarget != this)
				continue;

			MESSAGE_BEGIN(MSG_ONE, gmsgSpecHealth, nullptr, pPlayer->edict());
				WRITE_BYTE(int(Q_max(pev->health, 0.0f)));
			MESSAGE_END();
		}

		return bTookDamage;
	}

	CBaseEntity *pAttacker = CBaseEntity::Instance(pevAttacker);

	if (!g_pGameRules->FPlayerCanTakeDamage(this, pAttacker) && !FClassnameIs(pevInflictor, "grenade"))
	{
		// Refuse the damage
		return FALSE;
	}

	if ((bitsDamageType & DMG_BLAST) && g_pGameRules->IsMultiplayer())
	{
		// blasts damage armor more.
		flBonus *= 2;
	}

	// Already dead
	if (!IsAlive())
		return FALSE;

	pAttacker = GetClassPtr((CBaseEntity *)pevAttacker);

	if (pAttacker->IsPlayer())
	{
		pAttack = GetClassPtr((CBasePlayer *)pevAttacker);

		// warn about team attacks
		if (!CSGameRules()->IsFreeForAll() && pAttack->m_iTeam == m_iTeam)
		{
			if (pAttack != this)
			{
				if (!(pAttack->m_flDisplayHistory & DHF_FRIEND_INJURED))
				{
					pAttack->m_flDisplayHistory |= DHF_FRIEND_INJURED;
					pAttack->HintMessage("#Hint_try_not_to_injure_teammates");
				}

				bTeamAttack = TRUE;
				if (gpGlobals->time > pAttack->m_flLastAttackedTeammate + 0.6f)
				{
					CBaseEntity *pEntity = nullptr;
					while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
					{
						if (FNullEnt(pEntity->edict()))
							break;

						CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);

						if (pPlayer->m_iTeam == m_iTeam)
						{
							ClientPrint(pPlayer->pev, HUD_PRINTTALK, "#Game_teammate_attack", STRING(pAttack->pev->netname));
						}
					}

					pAttack->m_flLastAttackedTeammate = gpGlobals->time;
				}
			}

			// bullets hurt teammates less
			flDamage *= Q_clamp(((bitsDamageType & DMG_BULLET) ?
				ff_damage_reduction_bullets.value :
				ff_damage_reduction_other.value), 0.0f, 1.0f);
		}

		if (pAttack->m_pActiveItem)
		{
			iGunType = pAttack->m_pActiveItem->m_iId;
			flRatio += flShieldRatio;

			switch (iGunType)
			{
			case WEAPON_XM8:
			case WEAPON_M4A1:		flRatio *= 1.4;  break;
			case WEAPON_AWP:		flRatio *= 1.95; break;
			case WEAPON_SVD:		flRatio *= 1.65; break;
			case WEAPON_PSG1:		flRatio *= 1.45; break;
			case WEAPON_MK46:		flRatio *= 1.5;  break;
			case WEAPON_M45A1:		flRatio *= 1.05; break;
			case WEAPON_DEAGLE:		flRatio *= 1.5;  break;
			case WEAPON_GLOCK18:	flRatio *= 1.05; break;
			case WEAPON_FIVESEVEN:
			case WEAPON_P90:
				flRatio *= 1.5;
				break;
			case WEAPON_VECTOR:
			case WEAPON_MAC10:
				flRatio *= 0.95;
				break;
			case WEAPON_ANACONDA:
				flRatio *= 1.25;
				break;
			case WEAPON_SRS:
				flRatio *= 1.7;
				break;
			case WEAPON_SCARH:
				flRatio *= 1.4;
				break;
			case WEAPON_RPD:
			case WEAPON_AK47:
				flRatio *= 1.55;
				break;
			}
		}

		if (!ShouldDoLargeFlinch(m_LastHitGroup, iGunType))
		{
			m_flVelocityModifier = 0.5f;

			if (m_LastHitGroup == HITGROUP_HEAD)
				m_bHighDamage = (flDamage > 60);
			else
				m_bHighDamage = (flDamage > 20);

			SetAnimation(PLAYER_FLINCH);
		}
		else
		{
			if (pev->velocity.Length() < 300)
			{
				Vector attack_velocity = (pev->origin - pAttack->pev->origin).Normalize() * 170;
				pev->velocity = pev->velocity + attack_velocity;

				m_flVelocityModifier = 0.65f;
			}

			SetAnimation(PLAYER_LARGE_FLINCH);
		}

		// burning does not slowing so much.
		if (bitsDamageType & (DMG_BURN | DMG_SLOWBURN))
			m_flVelocityModifier = 0.9f;
	}

	// keep track of amount of damage last sustained
	m_lastDamageAmount = flDamage;

	// Armor
	// armor doesn't protect against fall or drown damage!
	if (pev->armorvalue != 0.0f && !(bitsDamageType & (DMG_DROWN | DMG_FALL)) && IsArmored(m_LastHitGroup))
	{
		real_t flNew = flRatio * flDamage;
		real_t flArmor = (flDamage - flNew) * flBonus;

		// Does this use more armor than we have?
		if (flArmor > pev->armorvalue)
		{
			armorHit = flArmor;
			flArmor = pev->armorvalue;
			flArmor *= (1 / flBonus);
			flNew = flDamage - flArmor;
			pev->armorvalue = 0;
		}
		else
		{
			int oldValue = pev->armorvalue;

			if (flArmor < 0.0)
				flArmor = 1.0;

			pev->armorvalue -= flArmor;
			armorHit = oldValue - pev->armorvalue;
		}

		flDamage = flNew;

		if (pev->armorvalue <= 0.0f)
			pev->armortype = ARMOR_NONE;

		Pain(m_LastHitGroup, true);
	}
	else
	{
		Pain(m_LastHitGroup, false);
	}

	OnPlayerDamagedPre(pevInflictor, pevAttacker, flDamage, bitsDamageType);	// skills should hook on here.

	if (!FNullEnt(pAttack))
		pAttack->OnHurtingAnotherPlayer(this, pevInflictor, flDamage, bitsDamageType);

	LogAttack(pAttack, this, bTeamAttack, flDamage, armorHit, pev->health - flDamage, pev->armorvalue, GetWeaponName(pevInflictor, pevAttacker));

	// this cast to INT is critical!!! If a player ends up with 0.5 health, the engine will get that
	// as an int (zero) and think the player is dead! (this will incite a clientside screentilt, etc)
	bTookDamage = CBaseMonster::TakeDamage(pevInflictor, pevAttacker, int(flDamage), bitsDamageType);

	if (bTookDamage)
	{
		if (TheBots)
		{
			TheBots->OnEvent(EVENT_PLAYER_TOOK_DAMAGE, this, pAttack);
		}
	}

	{
		// reset damage time countdown for each type of time based damage player just sustained
		for (int i = 0; i < ITBD_END; i++)
		{
			if (bitsDamageType & (DMG_PARALYZE << i))
				m_rgbTimeBasedDamage[i] = 0;
		}
	}

	// damage money.
	if (!FNullEnt(pAttack))
		pAttack->AddAccount(CSGameRules()->IDamageMoney(this, pAttack, flDamage), RT_HURTING_ENEMY);

	// tell director about it
	MESSAGE_BEGIN(MSG_SPEC, SVC_DIRECTOR);
		WRITE_BYTE(9);								// command length in bytes
		WRITE_BYTE(DRC_CMD_EVENT);					// take damage event
		WRITE_SHORT(ENTINDEX(edict()));				// index number of primary entity
		WRITE_SHORT(ENTINDEX(ENT(pevInflictor)));	// index number of secondary entity
		WRITE_LONG(5);								// eventflags (priority and flags)
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_SPEC, gmsgHLTV);
		WRITE_BYTE(ENTINDEX(edict()));
		WRITE_BYTE(int(Q_max(pev->health, 0.0f)) | DRC_FLAG_FACEPLAYER);
	MESSAGE_END();

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);

		if (!pPlayer)
			continue;

		if (pPlayer->m_hObserverTarget == this)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgSpecHealth, nullptr, pPlayer->edict());
				WRITE_BYTE(int(Q_max(pev->health, 0.0f)));
			MESSAGE_END();
		}
	}

	if ((bitsDamageType & DMG_DROWN) && pev->waterlevel == 0)
		bitsDamageType &= ~DMG_DROWN;

	// Save this so we can report it to the client
	m_bitsHUDDamage = -1;

	// make sure the damage bits get resent
	m_bitsDamageType |= bitsDamageType;

	return bTookDamage;
}

// PackDeadPlayerItems - call this when a player dies to
// pack up the appropriate weapons and ammo items, and to
// destroy anything that shouldn't be packed.
void CBasePlayer::PackDeadPlayerItems()
{
	// get the game rules
	bool bPackGun = (CSGameRules()->DeadPlayerWeapons(this) != GR_PLR_DROP_GUN_NO);
	bool bPackAmmo = (CSGameRules()->DeadPlayerAmmo(this) != GR_PLR_DROP_AMMO_NO);

	if (bPackGun)
	{
		bool bShieldDropped = false;
		if (HasShield())
		{
			DropShield();
			bShieldDropped = true;
		}

		CWeaponBox* pWeaponBox = nullptr;
		AmmoIdType iAmmoId = AMMO_NONE;

		for (int i = 0; i < MAX_ITEM_TYPES; i++)
		{
			// LUNA: in LeaderMod, we pack all weapons.

			if (!m_rgpPlayerItems[i] || !m_rgpPlayerItems[i]->CanDrop())
				continue;

			// save this value. we don't have any access to it after we pack it into CWeaponBox.
			iAmmoId = m_rgpPlayerItems[i]->m_iPrimaryAmmoType;

			if (!m_rgpPlayerItems[i]->Drop((void**)&pWeaponBox))
				continue;

			// drop randomly around player.
			pWeaponBox->pev->origin += Vector(RANDOM_FLOAT(-10, 10), RANDOM_FLOAT(-10, 10), 0);

			// pack some ammo into this weaponbox.
			if (bPackAmmo && iAmmoId != AMMO_NONE)
			{
				pWeaponBox->GiveAmmo(m_rgAmmo[iAmmoId], iAmmoId);
				m_rgAmmo[iAmmoId] = 0;
			}

		}
	}

	RemoveAllItems(TRUE);
}

void CBasePlayer::RemoveAllItems(BOOL removeSuit)
{
	bool bKillProgBar = false;
	int i;

	if (m_pTank)
	{
		m_pTank->Use(this, this, USE_OFF, 0);
		m_pTank = nullptr;
	}

	RemoveShield();

	if (bKillProgBar)
		SetProgressBarTime(0);

	if (m_pActiveItem)
	{
		ResetAutoaim();

		m_pActiveItem->Holstered();	// it's weapon removal anyway.
		m_pActiveItem = nullptr;
	}

	m_pLastItem = nullptr;

	for (i = 0; i < MAX_ITEM_TYPES; i++)
	{
		m_pActiveItem = m_rgpPlayerItems[i];

		if (m_pActiveItem)
		{
			m_pActiveItem->Drop();
		}

		m_rgpPlayerItems[i] = nullptr;
	}

	m_pActiveItem = nullptr;
	m_bHasPrimary = false;

	// ReGameDLL Fixes: Version 5.16.0.465
	// if (m_iFOV != DEFAULT_FOV)
	{
		pev->fov = m_iLastZoom = DEFAULT_FOV;
		m_bResumeZoom = false;
	}

	pev->viewmodel = 0;
	pev->weaponmodel = 0;

	if (removeSuit)
		pev->weapons = 0;
	else
		pev->weapons &= ~WEAPON_ALLWEAPONS;

	for (i = 0; i < MAX_AMMO_SLOTS; i++)
		m_rgAmmo[i] = 0;

	UpdateClientData();

	m_iHideHUD |= HIDEHUD_WEAPONS;

	m_bHasNightVision = false;
	SendItemStatus();
	ResetMaxSpeed();	// ReGameDLL Fixes: Version 5.16.0.465
}

void CBasePlayer::SetProgressBarTime(int time)
{
	if (time)
	{
		m_progressStart = gpGlobals->time;
		m_progressEnd = time + gpGlobals->time;
	}
	else
	{
		m_progressStart = 0;
		m_progressEnd = 0;
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgBarTime, nullptr, pev);
		WRITE_SHORT(time);
	MESSAGE_END();

	int playerIndex = entindex();
	CBaseEntity *pEntity = nullptr;

	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
	{
		if (FNullEnt(pEntity->edict()))
			break;

		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);

		if (pPlayer->GetObserverMode() == OBS_IN_EYE && pPlayer->pev->iuser2 == playerIndex)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgBarTime, nullptr, pPlayer->pev);
				WRITE_SHORT(time);
			MESSAGE_END();
		}
	}
}

void CBasePlayer::SetProgressBarTime2(int time, float timeElapsed)
{
	if (time)
	{
		m_progressStart = gpGlobals->time - timeElapsed;
		m_progressEnd = time + gpGlobals->time - timeElapsed;
	}
	else
	{
		timeElapsed = 0;
		m_progressStart = 0;
		m_progressEnd = 0;
	}

	short iTimeElapsed = (timeElapsed * 100.0 / (m_progressEnd - m_progressStart));

	MESSAGE_BEGIN(MSG_ONE, gmsgBarTime2, nullptr, pev);
		WRITE_SHORT(time);
		WRITE_SHORT(iTimeElapsed);
	MESSAGE_END();

	int playerIndex = entindex();
	CBaseEntity *pEntity = nullptr;

	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")))
	{
		if (FNullEnt(pEntity->edict()))
			break;

		CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);

		if (pPlayer->GetObserverMode() == OBS_IN_EYE && pPlayer->pev->iuser2 == playerIndex)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgBarTime2, nullptr, pPlayer->pev);
				WRITE_SHORT(time);
				WRITE_SHORT(iTimeElapsed);
			MESSAGE_END();
		}
	}
}

void BuyZoneIcon_Set(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgStatusIcon, nullptr, pPlayer->pev);
		WRITE_BYTE(STATUSICON_SHOW);
		WRITE_STRING("buyzone");
		WRITE_BYTE(0);
		WRITE_BYTE(160);
		WRITE_BYTE(0);
	MESSAGE_END();
}

void BuyZoneIcon_Clear(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgStatusIcon, nullptr, pPlayer->pev);
		WRITE_BYTE(STATUSICON_HIDE);
		WRITE_STRING("buyzone");
	MESSAGE_END();

	if (pPlayer->m_iMenu >= Menu_Buy3)
	{
		CLIENT_COMMAND(ENT(pPlayer->pev), "slot10\n");
	}
}

void CBasePlayer::SendFOV(int fov)
{
	pev->fov = real_t(fov);
	m_iClientFOV = fov;

	MESSAGE_BEGIN(MSG_ONE, gmsgSetFOV, nullptr, pev);
		WRITE_BYTE(fov);
	MESSAGE_END();
}

void EXT_FUNC CBasePlayer::Killed(entvars_t *pevAttacker, int iGib)
{
	m_canSwitchObserverModes = false;

	if (m_LastHitGroup == HITGROUP_HEAD)
		m_bHeadshotKilled = true;

	CBaseEntity *pAttackerEntity = CBaseEntity::Instance(pevAttacker);

	if (TheBots)
	{
		TheBots->OnEvent(EVENT_PLAYER_DIED, this, pAttackerEntity);
	}

	// being killed like smashing a ice block is never good..
	if (m_flFrozenNextThink > gpGlobals->time)
	{
		gFrozenDOTMgr::Free(this);
		iGib = GIB_ALWAYS;
	}

	if (m_flElectrifyTimeUp > 0.0f)
		gElectrifiedDOTMgr::Free(this);

	if (m_flPoisonedTimeUp > 0.0f)
		gPoisonDOTMgr::Free(this);

	if (m_flBurningTimeUp > 0.0f)
		gBurningDOTMgr::Free(this);

	CSGameRules()->PlayerKilled(this, pevAttacker, g_pevLastInflictor);

	MESSAGE_BEGIN(MSG_ONE, gmsgNVGToggle, nullptr, pev);
		WRITE_BYTE(0);
	MESSAGE_END();

	m_bNightVisionOn = false;

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pObserver = UTIL_PlayerByIndex(i);

		if (!pObserver)
			continue;

		if (pObserver->IsObservingPlayer(this))
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgNVGToggle, nullptr, pObserver->pev);
				WRITE_BYTE(0);
			MESSAGE_END();

			pObserver->m_bNightVisionOn = false;
		}
	}

	if (m_pTank)
	{
		m_pTank->Use(this, this, USE_OFF, 0);
		m_pTank = nullptr;
	}

	SetAnimation(PLAYER_DIE);

	WPN_UNDONE	// CLIENT should single server to drop a on-death grenade.

	pev->modelindex = m_modelIndexPlayer;
	pev->deadflag = DEAD_DYING;
	pev->movetype = MOVETYPE_TOSS;
	pev->takedamage = DAMAGE_NO;

	pev->gamestate = HITGROUP_SHIELD_DISABLED;
	m_bShieldDrawn = false;

	pev->flags &= ~FL_ONGROUND;

	// FlashlightTurnOff()
	pev->effects &= ~EF_DIMLIGHT;

	switch ((int)fadetoblack.value)
	{
	default:
	{
		pev->iuser1 = OBS_CHASE_FREE;
		pev->iuser2 = ENTINDEX(edict());
		pev->iuser3 = ENTINDEX(ENT(pevAttacker));

		m_hObserverTarget = UTIL_PlayerByIndexSafe(pev->iuser3);

		break;
	}
	case 1:
	{
		UTIL_ScreenFade(this, Vector(0, 0, 0), 3, 3, 255, (FFADE_OUT | FFADE_STAYOUT));
		break;
	}
	case 2:
	{
		pev->iuser1 = OBS_CHASE_FREE;
		pev->iuser2 = ENTINDEX(edict());
		pev->iuser3 = ENTINDEX(ENT(pevAttacker));

		m_hObserverTarget = UTIL_PlayerByIndexSafe(pev->iuser3);

		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			CBasePlayer* pObserver = UTIL_PlayerByIndex(i);

			if (pObserver == this || (pObserver && pObserver->IsObservingPlayer(this)))
			{
				UTIL_ScreenFade(pObserver, Vector(0, 0, 0), 1, 4, 255, (FFADE_OUT));
			}
		}

		break;
	}
	}

	SetScoreboardAttributes();

	if (m_iThrowDirection)
	{
		switch (m_iThrowDirection)
		{
		case THROW_FORWARD:
		{
			UTIL_MakeVectors(pev->angles);
			pev->velocity = gpGlobals->v_forward * RANDOM_FLOAT(100, 200);
			pev->velocity.z = RANDOM_FLOAT(50, 100);
			break;
		}
		case THROW_BACKWARD:
		{
			UTIL_MakeVectors(pev->angles);
			pev->velocity = gpGlobals->v_forward * RANDOM_FLOAT(-100, -200);
			pev->velocity.z = RANDOM_FLOAT(50, 100);
			break;
		}
		case THROW_HITVEL:
		{
			if (FClassnameIs(pevAttacker, "player"))
			{
				UTIL_MakeVectors(pevAttacker->angles);

				pev->velocity = gpGlobals->v_forward * RANDOM_FLOAT(200, 300);
				pev->velocity.z = RANDOM_FLOAT(200, 300);
			}
			break;
		}
		case THROW_GRENADE:
		{
			pev->velocity = m_vBlastVector * (1 / m_vBlastVector.Length()) * (500 - m_vBlastVector.Length());
			pev->velocity.z = (350 - m_vBlastVector.Length()) * 1.5;
			break;
		}
		case THROW_HITVEL_MINUS_AIRVEL:
		{
			if (FClassnameIs(pevAttacker, "player"))
			{
				UTIL_MakeVectors(pevAttacker->angles);
				pev->velocity = gpGlobals->v_forward * RANDOM_FLOAT(200, 300);
			}
			break;
		}
		default:
			break;
		}

		pev->angles.y = UTIL_VecToAngles(-pev->velocity).y;
		pev->v_angle.y = pev->angles.y;

		m_iThrowDirection = THROW_NONE;
	}

	SetSuitUpdate(nullptr, SUIT_SENTENCE, SUIT_REPEAT_OK);
	m_iClientHealth = 0;

	MESSAGE_BEGIN(MSG_ALL, gmsgHealth);
		WRITE_BYTE(entindex());
		WRITE_SHORT(m_iClientHealth);
	MESSAGE_END();

	SendFOV(0);

	CSGameRules()->CheckWinConditions();
	m_bNotKilled = false;

	BuyZoneIcon_Clear(this);

	SetThink(&CBasePlayer::PlayerDeathThink);
	pev->nextthink = gpGlobals->time + 0.1f;
	pev->solid = SOLID_NOT;

	if (m_bPunishedForTK)
	{
		m_bPunishedForTK = false;
		HintMessage("#Hint_cannot_play_because_tk", TRUE, TRUE);
	}

	if ((pev->health < -9000 && iGib != GIB_NEVER) || iGib == GIB_ALWAYS)
	{
		GibMonster();
		pev->effects |= EF_NODRAW;

		return;
	}

	DeathSound();

	pev->angles.x = 0;
	pev->angles.z = 0;

	if (!(m_flDisplayHistory & DHF_SPEC_DUCK))
	{
		HintMessage("#Spec_Duck", TRUE, TRUE);
		m_flDisplayHistory |= DHF_SPEC_DUCK;
	}

	// normally, skills should terminating here.
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnPlayerDeath(pAttackerEntity->IsPlayer() ? CBasePlayer::Instance(pevAttacker) : nullptr);
	}
}

void EXT_FUNC CBasePlayer::SetAnimation(PLAYER_ANIM playerAnim)
{
	int animDesired;
	float speed;
	char szAnim[64];
	int hopSeq;
	int leapSeq;

	if (!pev->modelindex)
		return;

	if ((playerAnim == PLAYER_FLINCH || playerAnim == PLAYER_LARGE_FLINCH) && HasShield())
		return;

	if (playerAnim != PLAYER_FLINCH && playerAnim != PLAYER_LARGE_FLINCH && m_flFlinchTime > gpGlobals->time && pev->health > 0.0f)
		return;

	speed = pev->velocity.Length2D();

	if (pev->flags & FL_FROZEN)
	{
		speed = 0;
		playerAnim = PLAYER_IDLE;
	}

	hopSeq = LookupActivity(ACT_HOP);
	leapSeq = LookupActivity(ACT_LEAP);

	switch (playerAnim)
	{
		case PLAYER_JUMP:
		{
			if (m_Activity == ACT_SWIM || m_Activity == ACT_DIESIMPLE || m_Activity == ACT_HOVER)
				m_IdealActivity = m_Activity;
			else
			{
				m_IdealActivity = ACT_HOP;
				if (TheBots)
				{
					TheBots->OnEvent(EVENT_PLAYER_JUMPED, this);
				}
			}
			break;
		}
		case PLAYER_SUPERJUMP:
		{
			if (m_Activity == ACT_SWIM || m_Activity == ACT_DIESIMPLE || m_Activity == ACT_HOVER)
				m_IdealActivity = m_Activity;
			else
				m_IdealActivity = ACT_LEAP;
			break;
		}
		case PLAYER_DIE:
		{
			m_IdealActivity = ACT_DIESIMPLE;
			break;
		}
		case PLAYER_ATTACK1:
		{
			if (m_Activity == ACT_SWIM || m_Activity == ACT_DIESIMPLE || m_Activity == ACT_HOVER)
				m_IdealActivity = m_Activity;
			else
			{
				m_IdealActivity = ACT_RANGE_ATTACK1;
				if (TheBots)
				{
					TheBots->OnEvent(EVENT_WEAPON_FIRED, this);
				}
			}
			break;
		}
		case PLAYER_ATTACK2:
		{
			if (m_Activity == ACT_SWIM || m_Activity == ACT_DIESIMPLE || m_Activity == ACT_HOVER)
				m_IdealActivity = m_Activity;
			else
			{
				m_IdealActivity = ACT_RANGE_ATTACK2;
				if (TheBots)
				{
					TheBots->OnEvent(EVENT_WEAPON_FIRED, this);
				}
			}
			break;
		}
		case PLAYER_RELOAD:
		{
			if (m_Activity == ACT_SWIM || m_Activity == ACT_DIESIMPLE || m_Activity == ACT_HOVER)
				m_IdealActivity = m_Activity;
			else
			{
				m_IdealActivity = ACT_RELOAD;
				if (TheBots)
				{
					TheBots->OnEvent(EVENT_WEAPON_RELOADED, this);
				}
			}
			break;
		}
		case PLAYER_IDLE:
		case PLAYER_WALK:
		{
			if (pev->flags & FL_ONGROUND || (m_Activity != ACT_HOP && m_Activity != ACT_LEAP))
			{
				if (pev->waterlevel <= 1)
					m_IdealActivity = ACT_WALK;

				else if (speed == 0.0f)
					m_IdealActivity = ACT_HOVER;

				else
					m_IdealActivity = ACT_SWIM;
			}
			else
				m_IdealActivity = m_Activity;
			break;
		}
		case PLAYER_HOLDBOMB:
			m_IdealActivity = ACT_HOLDBOMB;
			break;
		case PLAYER_FLINCH:
			m_IdealActivity = ACT_FLINCH;
			break;
		case PLAYER_LARGE_FLINCH:
			m_IdealActivity = ACT_LARGE_FLINCH;
			break;
		default:
			break;
	}
	switch (m_IdealActivity)
	{
		case ACT_HOP:
		case ACT_LEAP:
		{
			if (m_Activity == m_IdealActivity)
				return;

			switch (m_Activity)
			{
			case ACT_RANGE_ATTACK1:	Q_strcpy(szAnim, "ref_shoot_"); break;
			case ACT_RANGE_ATTACK2:	Q_strcpy(szAnim, "ref_shoot2_"); break;
			case ACT_RELOAD:	Q_strcpy(szAnim, "ref_reload_"); break;
			default:		Q_strcpy(szAnim, "ref_aim_"); break;
			}

			Q_strcat(szAnim, m_szAnimExtention);
			animDesired = LookupSequence(szAnim);
			if (animDesired == -1)
				animDesired = 0;

			if (pev->sequence != animDesired || !m_fSequenceLoops)
				pev->frame = 0;

			if (!m_fSequenceLoops)
				pev->effects |= EF_NOINTERP;

			if (m_IdealActivity == ACT_LEAP)
				pev->gaitsequence = LookupActivity(ACT_LEAP);
			else
				pev->gaitsequence = LookupActivity(ACT_HOP);

			m_Activity = m_IdealActivity;
			break;
		}
		case ACT_RANGE_ATTACK1:
		{
			m_flLastFired = gpGlobals->time;

			if (pev->flags & FL_DUCKING)
				Q_strcpy(szAnim, "crouch_shoot_");
			else
				Q_strcpy(szAnim, "ref_shoot_");

			Q_strcat(szAnim, m_szAnimExtention);
			animDesired = LookupSequence(szAnim);
			if (animDesired == -1)
				animDesired = 0;

			pev->sequence = animDesired;
			pev->frame = 0;

			ResetSequenceInfo();
			m_Activity = m_IdealActivity;
			break;
		}
		case ACT_RANGE_ATTACK2:
		{
			m_flLastFired = gpGlobals->time;

			if (pev->flags & FL_DUCKING)
				Q_strcpy(szAnim, "crouch_shoot2_");
			else
				Q_strcpy(szAnim, "ref_shoot2_");

			Q_strcat(szAnim, m_szAnimExtention);
			animDesired = LookupSequence(szAnim);
			if (animDesired == -1)
				animDesired = 0;

			pev->sequence = animDesired;
			pev->frame = 0;

			ResetSequenceInfo();
			m_Activity = m_IdealActivity;
			break;
		}
		case ACT_RELOAD:
		{
			if (pev->flags & FL_DUCKING)
				Q_strcpy(szAnim, "crouch_reload_");
			else
				Q_strcpy(szAnim, "ref_reload_");

			Q_strcat(szAnim, m_szAnimExtention);
			animDesired = LookupSequence(szAnim);
			if (animDesired == -1)
				animDesired = 0;

			if (pev->sequence != animDesired || !m_fSequenceLoops)
				pev->frame = 0;

			if (!m_fSequenceLoops)
				pev->effects |= EF_NOINTERP;

			m_Activity = m_IdealActivity;
			break;
		}
		case ACT_HOLDBOMB:
		{
			if (pev->flags & FL_DUCKING)
				Q_strcpy(szAnim, "crouch_aim_");
			else
				Q_strcpy(szAnim, "ref_aim_");

			Q_strcat(szAnim, m_szAnimExtention);
			animDesired = LookupSequence(szAnim);
			if (animDesired == -1)
				animDesired = 0;

			m_Activity = m_IdealActivity;
			break;
		}
		case ACT_WALK:
		{
			if ((m_Activity != ACT_RANGE_ATTACK1 || m_fSequenceFinished)
				&& (m_Activity != ACT_RANGE_ATTACK2 || m_fSequenceFinished)
				&& (m_Activity != ACT_FLINCH || m_fSequenceFinished)
				&& (m_Activity != ACT_LARGE_FLINCH || m_fSequenceFinished)
				&& (m_Activity != ACT_RELOAD || m_fSequenceFinished))
			{
				if (speed <= 135.0f || m_flLastFired + 4.0 >= gpGlobals->time)
				{
					if (pev->flags & FL_DUCKING)
						Q_strcpy(szAnim, "crouch_aim_");
					else
						Q_strcpy(szAnim, "ref_aim_");

					Q_strcat(szAnim, m_szAnimExtention);
					animDesired = LookupSequence(szAnim);
					if (animDesired == -1)
						animDesired = 0;

					m_Activity = ACT_WALK;
				}
				else
				{
					Q_strcpy(szAnim, "run_");
					Q_strcat(szAnim, m_szAnimExtention);
					animDesired = LookupSequence(szAnim);
					if (animDesired == -1)
					{
						if (pev->flags & FL_DUCKING)
							Q_strcpy(szAnim, "crouch_aim_");
						else
							Q_strcpy(szAnim, "ref_aim_");

						Q_strcat(szAnim, m_szAnimExtention);
						animDesired = LookupSequence(szAnim);
						if (animDesired == -1)
							animDesired = 0;

						m_Activity = ACT_RUN;
						pev->gaitsequence = LookupActivity(ACT_RUN);
					}
					else
					{
						m_Activity = ACT_RUN;
						pev->gaitsequence = animDesired;
					}
					if (m_Activity == ACT_RUN)
					{
						// TODO: maybe away used variable 'speed'?
						//if (speed > 150.0f)
						if (pev->velocity.Length2D() > 150.0f)
						{
							if (TheBots)
							{
								TheBots->OnEvent(EVENT_PLAYER_FOOTSTEP, this);
							}
						}
					}
				}
			}
			else
				animDesired = pev->sequence;

			if (speed > 135.0f)
				pev->gaitsequence = LookupActivity(ACT_RUN);
			else
				pev->gaitsequence = LookupActivity(ACT_WALK);
			break;
		}
		case ACT_FLINCH:
		case ACT_LARGE_FLINCH:
		{
			m_Activity = m_IdealActivity;

			switch (m_LastHitGroup)
			{
				case HITGROUP_GENERIC:
				{
					if (RANDOM_LONG(0, 1))
						animDesired = LookupSequence("head_flinch");
					else
						animDesired = LookupSequence("gut_flinch");
					break;
				}
				case HITGROUP_HEAD:
				case HITGROUP_CHEST:
					animDesired = LookupSequence("head_flinch");
					break;
				case HITGROUP_SHIELD:
					animDesired = 0;
					break;
				default:
					animDesired = LookupSequence("gut_flinch");
					break;
			}

			if (animDesired == -1)
				animDesired = 0;

			break;
		}
		case ACT_DIESIMPLE:
		{
			if (m_Activity == m_IdealActivity)
				return;

			m_Activity = m_IdealActivity;
			m_flDeathThrowTime = 0;
			m_iThrowDirection = THROW_NONE;

			switch (m_LastHitGroup)
			{
				case HITGROUP_GENERIC:
				{
					switch (RANDOM_LONG(0, 8))
					{
					case 0:
						animDesired = LookupActivity(ACT_DIE_HEADSHOT);
						m_iThrowDirection = THROW_BACKWARD;
						break;
					case 1:
						animDesired = LookupActivity(ACT_DIE_GUTSHOT);
						break;
					case 2:
						animDesired = LookupActivity(ACT_DIE_BACKSHOT);
						m_iThrowDirection = THROW_HITVEL;
						break;
					case 4:
						animDesired = LookupActivity(ACT_DIEBACKWARD);
						m_iThrowDirection = THROW_HITVEL;
						break;
					case 5:
						animDesired = LookupActivity(ACT_DIEFORWARD);
						m_iThrowDirection = THROW_FORWARD;
						break;
					case 6:
						animDesired = LookupActivity(ACT_DIE_CHESTSHOT);
						break;
					case 7:
						animDesired = LookupActivity(ACT_DIE_GUTSHOT);
						break;
					case 8:
						animDesired = LookupActivity(ACT_DIE_HEADSHOT);
						break;
					default:
						animDesired = LookupActivity(ACT_DIESIMPLE);
						break;
					}
					break;
				}
				case HITGROUP_HEAD:
				{
					int random = RANDOM_LONG(0, 8);
					m_bHeadshotKilled = true;

					if (m_bHighDamage)
						random++;

					switch (random)
					{
					case 1:
					case 2:
						m_iThrowDirection = THROW_BACKWARD;
						break;
					case 3:
					case 4:
					case 5:
					case 6:
						m_iThrowDirection = THROW_HITVEL;
						break;
					default:
						m_iThrowDirection = THROW_NONE;
						break;
					}

					animDesired = LookupActivity(ACT_DIE_HEADSHOT);
					break;
				}
				case HITGROUP_CHEST:
					animDesired = LookupActivity(ACT_DIE_CHESTSHOT);
					break;
				case HITGROUP_STOMACH:
					animDesired = LookupActivity(ACT_DIE_GUTSHOT);
					break;
				case HITGROUP_LEFTARM:
					animDesired = LookupSequence("left");
					break;
				case HITGROUP_RIGHTARM:
				{
					m_iThrowDirection = RANDOM_LONG(0, 1) ? THROW_HITVEL : THROW_HITVEL_MINUS_AIRVEL;
					animDesired = LookupSequence("right");
					break;
				}
				default:
					animDesired = LookupActivity(ACT_DIESIMPLE);
					break;
			}

			if (pev->flags & FL_DUCKING)
			{
				animDesired = LookupSequence("crouch_die");
				m_iThrowDirection = THROW_BACKWARD;
			}
			else if (m_bKilledByGrenade)
			{
				UTIL_MakeVectors(pev->angles);

				if (DotProduct(gpGlobals->v_forward, m_vBlastVector) > 0.0f)
					animDesired = LookupSequence("left");
				else
				{
					if (RANDOM_LONG(0, 1))
						animDesired = LookupSequence("crouch_die");
					else
						animDesired = LookupActivity(ACT_DIE_HEADSHOT);
				}

				m_iThrowDirection = THROW_GRENADE;
			}

			if (animDesired == -1)
				animDesired = 0;

			if (pev->sequence != animDesired)
			{
				pev->gaitsequence = 0;
				pev->sequence = animDesired;
				pev->frame = 0.0f;
				ResetSequenceInfo();
			}
			return;
		}
		default:
		{
			if (m_Activity == m_IdealActivity)
				return;

			m_Activity = m_IdealActivity;
			animDesired = LookupActivity(m_IdealActivity);

			if (pev->sequence != animDesired)
			{
				pev->gaitsequence = 0;
				pev->sequence = animDesired;
				pev->frame = 0;

				ResetSequenceInfo();
			}
			return;
		}
	}

	if (pev->gaitsequence != hopSeq && pev->gaitsequence != leapSeq)
	{
		if (pev->flags & FL_DUCKING)
		{
			if (speed != 0.0f)
				pev->gaitsequence = LookupActivity(ACT_CROUCH);
			else
				pev->gaitsequence = LookupActivity(ACT_CROUCHIDLE);
		}
		else
		{
			if (speed > 135.0f)
			{
				if (m_flLastFired + 4.0f < gpGlobals->time)
				{
					if (m_Activity != ACT_FLINCH && m_Activity != ACT_LARGE_FLINCH)
					{
						Q_strcpy(szAnim, "run_");
						Q_strcat(szAnim, m_szAnimExtention);

						animDesired = LookupSequence(szAnim);
						if (animDesired == -1)
						{
							if (pev->flags & FL_DUCKING)
								Q_strcpy(szAnim, "crouch_aim_");
							else
								Q_strcpy(szAnim, "ref_aim_");

							Q_strcat(szAnim, m_szAnimExtention);
							animDesired = LookupSequence(szAnim);
						}
						else
							pev->gaitsequence = animDesired;

						m_Activity = ACT_RUN;
					}
				}
				pev->gaitsequence = LookupActivity(ACT_RUN);
			}
			else
			{
				if (speed > 0.0f)
					pev->gaitsequence = LookupActivity(ACT_WALK);
				else
					pev->gaitsequence = LookupActivity(ACT_IDLE);
			}
		}
	}
	if (pev->sequence != animDesired)
	{
		pev->sequence = animDesired;
		pev->frame = 0;

		ResetSequenceInfo();
	}
}

void CBasePlayer::WaterMove()
{
	int air;

	if (pev->movetype == MOVETYPE_NOCLIP || pev->movetype == MOVETYPE_NONE)
		return;

	if (!IsAlive())
		return;

	// waterlevel 0 - not in water
	// waterlevel 1 - feet in water
	// waterlevel 2 - waist in water
	// waterlevel 3 - head in water
	if (pev->waterlevel != 3)
	{
		// not underwater

		// play 'up for air' sound
		if (pev->air_finished < gpGlobals->time)
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_wade1.wav", VOL_NORM, ATTN_NORM);

		else if (pev->air_finished < gpGlobals->time + 9)
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_wade2.wav", VOL_NORM, ATTN_NORM);

		pev->air_finished = gpGlobals->time + AIRTIME;
		pev->dmg = 2;

		// if we took drowning damage, give it back slowly
		if (m_idrowndmg > m_idrownrestored)
		{
			// set drowning damage bit.  hack - dmg_drownrecover actually
			// makes the time based damage code 'give back' health over time.
			// make sure counter is cleared so we start count correctly.

			// NOTE: this actually causes the count to continue restarting
			// until all drowning damage is healed.
			m_rgbTimeBasedDamage[ITBD_DROWN_RECOVER] = 0;
			m_bitsDamageType |= DMG_DROWNRECOVER;
			m_bitsDamageType &= ~DMG_DROWN;
		}
	}
	else
	{
		// fully under water
		// stop restoring damage while underwater
		m_rgbTimeBasedDamage[ITBD_DROWN_RECOVER] = 0;
		m_bitsDamageType &= ~DMG_DROWNRECOVER;

		// drown!
		if (gpGlobals->time > pev->air_finished)
		{
			if (gpGlobals->time > pev->pain_finished)
			{
				// take drowning damage
				pev->dmg += 1;

				if (pev->dmg > 5)
					pev->dmg = 5;

				TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), pev->dmg, DMG_DROWN);

				// NOTE: If we died after the damage of above and was followed respawn,
				// so we should get out of code.
				if (!(m_bitsDamageType & DMG_DROWN))
					return;

				pev->pain_finished = gpGlobals->time + 1;

				// track drowning damage, give it back when
				// player finally takes a breath
				m_idrowndmg += pev->dmg;
			}
		}
		else
			m_bitsDamageType &= ~(DMG_DROWNRECOVER | DMG_DROWN);
	}

	if (!pev->waterlevel)
	{
		if (pev->flags & FL_INWATER)
			pev->flags &= ~FL_INWATER;

		return;
	}

	// make bubbles
	air = int(pev->air_finished - gpGlobals->time);

	if (!RANDOM_LONG(0, 0x1f) && RANDOM_LONG(0, AIRTIME - 1) >= air)
	{
		switch (RANDOM_LONG(0, 3))
		{
		case 0: EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim1.wav", 0.8, ATTN_NORM); break;
		case 1: EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim2.wav", 0.8, ATTN_NORM); break;
		case 2: EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim3.wav", 0.8, ATTN_NORM); break;
		case 3: EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim4.wav", 0.8, ATTN_NORM); break;
		}
	}

	if (pev->watertype == CONTENT_LAVA)		// do damage
	{
		if (pev->dmgtime < gpGlobals->time)
			TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), pev->waterlevel * 10, DMG_BURN);
	}
	else if (pev->watertype == CONTENT_SLIME)	// do damage
	{
		pev->dmgtime = gpGlobals->time + 1;
		TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), pev->waterlevel * 4, DMG_ACID);
	}
	if (!(pev->flags & FL_INWATER))
	{
		pev->flags |= FL_INWATER;
		pev->dmgtime = 0;
	}
}

BOOL CBasePlayer::IsOnLadder()
{
	return pev->movetype == MOVETYPE_FLY;
}

void CBasePlayer::GiveShield()
{
	m_bOwnsShield = true;
	pev->gamestate = HITGROUP_SHIELD_ENABLED;
}

void CBasePlayer::RemoveShield()
{
	if (HasShield())
	{
		m_bOwnsShield = false;
		pev->gamestate = HITGROUP_SHIELD_DISABLED;
	}
}

bool CBasePlayer::HasShield()
{
	return m_bOwnsShield;
}

CGrenade *CBasePlayer::ThrowGrenade(EquipmentIdType iId)
{
	CGrenade* pGrenade = nullptr;

	// radio everyone.
	Radio("%!MRAD_FIREINHOLE", "#Fire_in_the_hole");

	Vector angThrow = pev->v_angle + pev->punchangle;

	if (angThrow.pitch < 0)
		angThrow.pitch = -10 + angThrow.pitch * ((90 - 10) / 90.0f);
	else
		angThrow.pitch = -10 + angThrow.pitch * ((90 + 10) / 90.0f);

	float flVel = (90.0f - angThrow.pitch) * 6.0f;

	// cap the speed.
	if (flVel > 750.0f)
		flVel = 750.0f;

	UTIL_MakeVectors(angThrow);

	Vector vecSrc = pev->origin + pev->view_ofs + gpGlobals->v_forward * 16.0f;
	Vector vecThrow = gpGlobals->v_forward * flVel + pev->velocity;

	// player "shoot" animation
	SetAnimation(PLAYER_ATTACK1);

	// translate EQP_ to grenade function call.
	switch (iId)
	{
	case EQP_HEGRENADE:	pGrenade = CGrenade::HEGrenade(pev, vecSrc, vecThrow, m_iTeam); break;
	case EQP_FLASHBANG:    pGrenade = CGrenade::Flashbang(pev, vecSrc, vecThrow); break;
	case EQP_SMOKEGRENADE: pGrenade = CGrenade::SmokeGrenade(pev, vecSrc, vecThrow); break;
	case EQP_CRYOGRENADE: pGrenade = CGrenade::Cryogrenade(this); break;
	case EQP_INCENDIARY_GR: pGrenade = CGrenade::IncendiaryGrenade(pev, vecSrc, vecThrow); break;
	case EQP_HEALING_GR: pGrenade = CGrenade::HealingGrenade(pev, vecSrc, vecThrow); break;
	case EQP_GAS_GR: pGrenade = CGrenade::NerveGasGrenade(pev, vecSrc, vecThrow); break;
	}

	OnGrenadeThrew(iId, pGrenade);

	return pGrenade;
}

void EXT_FUNC CBasePlayer::AddAccount(int amount, RewardType type, bool bTrackChange)
{
	bool bSendMoney = true;
	switch (type)
	{
	case RT_INTO_GAME:
	case RT_PLAYER_JOIN:
		bSendMoney = false;
	case RT_PLAYER_RESET:
	case RT_PLAYER_SPEC_JOIN:
		m_iAccount = 0;
		break;
	}

	m_iAccount += amount;

	if (bSendMoney)
	{
		m_iAccount = clamp<int>(m_iAccount, 0, maxmoney.value);

		// Send money update to HUD
		MESSAGE_BEGIN(MSG_ALL, gmsgMoney);
			WRITE_BYTE(entindex());
			WRITE_SHORT(m_iAccount);
		MESSAGE_END();
	}
}

void CBasePlayer::ResetMenu()
{
	m_iMenu = Menu_OFF;

	MESSAGE_BEGIN(MSG_ONE, gmsgShowMenu, nullptr, pev);
		WRITE_SHORT(0);
		WRITE_CHAR(0);
		WRITE_BYTE(0);
		WRITE_STRING("");
	MESSAGE_END();
}

void CBasePlayer::SyncRoundTimer()
{
	float tmRemaining = 0;
	BOOL bFreezePeriod = g_pGameRules->IsFreezePeriod();

	if (g_pGameRules->IsMultiplayer())
	{
		tmRemaining = CSGameRules()->GetRoundRemainingTimeReal();

		// hide timer HUD because it is useless.
		if (tmRemaining <= 0.0f && CSGameRules()->m_iRoundTime <= 0) {
			m_iHideHUD |= HIDEHUD_TIMER;
			return;
		}

		if (m_iHideHUD & HIDEHUD_TIMER)
		{
			m_iHideHUD &= ~HIDEHUD_TIMER;
			MESSAGE_BEGIN(MSG_ONE, gmsgShowTimer, nullptr, pev);
			MESSAGE_END();
		}
	}

	if (tmRemaining < 0)
		tmRemaining = 0;

	MESSAGE_BEGIN(MSG_ONE, gmsgRoundTime, nullptr, pev);
		WRITE_SHORT(int(tmRemaining));
	MESSAGE_END();
}

void CBasePlayer::RemoveLevelText()
{
	ResetMenu();
}

void ShowMenu2(CBasePlayer *pPlayer, int bitsValidSlots, int nDisplayTime, int fNeedMore, char *pszText)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgShowMenu, nullptr, pPlayer->pev);
		WRITE_SHORT(bitsValidSlots);
		WRITE_CHAR(nDisplayTime);
		WRITE_BYTE(fNeedMore);
		WRITE_STRING(pszText);
	MESSAGE_END();
}

void CBasePlayer::MenuPrint(const char *msg)
{
	const char *msg_portion = msg;
	char sbuf[MAX_BUFFER_MENU_BRIEFING + 1];

	while (Q_strlen(msg_portion) >= MAX_BUFFER_MENU_BRIEFING)
	{
		Q_strncpy(sbuf, msg_portion, MAX_BUFFER_MENU_BRIEFING);
		sbuf[MAX_BUFFER_MENU_BRIEFING] = '\0';
		msg_portion += MAX_BUFFER_MENU_BRIEFING;

		MESSAGE_BEGIN(MSG_ONE, gmsgShowMenu, nullptr, pev);
			WRITE_SHORT(0xFFFF);
			WRITE_CHAR(-1);
			WRITE_BYTE(1);	// multipart
			WRITE_STRING(sbuf);
		MESSAGE_END();
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgShowMenu, nullptr, pev);
		WRITE_SHORT(0xFFFF);
		WRITE_CHAR(-1);
		WRITE_BYTE(0);	// multipart
		WRITE_STRING(msg_portion);
	MESSAGE_END();
}

void CBasePlayer::JoiningThink()
{
	switch (m_iJoiningState)
	{
		case JOINED:
		{
			return;
		}
		case SHOWLTEXT:
		{
			ResetMenu();
			m_iJoiningState = SHOWTEAMSELECT;

			MESSAGE_BEGIN(MSG_ONE, gmsgStatusIcon, nullptr, pev);
				WRITE_BYTE(STATUSICON_HIDE);
				WRITE_STRING("defuser");
			MESSAGE_END();

			m_fLastMovement = gpGlobals->time;
			m_bMissionBriefing = false;

			SendItemStatus();
			break;
		}
		case READINGLTEXT:
		{
			if (m_afButtonPressed & (IN_ATTACK | IN_ATTACK2 | IN_JUMP))
			{
				m_afButtonPressed &= ~(IN_ATTACK | IN_ATTACK2 | IN_JUMP);

				RemoveLevelText();
				m_iJoiningState = SHOWTEAMSELECT;
			}
			break;
		}
		case GETINTOGAME:
		{
			if (GetIntoGame()) {
				return;
			}

			break;
		}
	}

	if (m_pIntroCamera && gpGlobals->time >= m_fIntroCamTime)
	{
		// find the next another camera
		m_pIntroCamera = UTIL_FindEntityByClassname(m_pIntroCamera, "trigger_camera");

		// could not find, go back to the start
		if (!m_pIntroCamera)
		{
			m_pIntroCamera = UTIL_FindEntityByClassname(nullptr, "trigger_camera");
		}

		CBaseEntity *Target = UTIL_FindEntityByTargetname(nullptr, STRING(m_pIntroCamera->pev->target));
		if (Target)
		{
			Vector vecAngles = UTIL_VecToAngles((Target->pev->origin - m_pIntroCamera->pev->origin).Normalize());

			vecAngles.x = -vecAngles.x;
			UTIL_SetOrigin(pev, m_pIntroCamera->pev->origin);

			pev->angles = vecAngles;
			pev->v_angle = pev->angles;

			pev->velocity = g_vecZero;
			pev->punchangle = g_vecZero;

			pev->fixangle = 1;
			pev->view_ofs = g_vecZero;
			m_fIntroCamTime = gpGlobals->time + 6;
		}
		else
			m_pIntroCamera = nullptr;
	}
}

void EXT_FUNC CBasePlayer::Disappear()
{
	if (m_pTank)
	{
		m_pTank->Use(this, this, USE_OFF, 0);
		m_pTank = nullptr;
	}

	m_fSequenceFinished = TRUE;
	pev->modelindex = m_modelIndexPlayer;
	pev->view_ofs = Vector(0, 0, -8);
	pev->deadflag = DEAD_DYING;
	pev->solid = SOLID_NOT;
	pev->flags &= ~FL_ONGROUND;

	// FlashlightTurnOff()
	pev->effects &= ~EF_DIMLIGHT;

	SetSuitUpdate(nullptr, SUIT_SENTENCE, SUIT_REPEAT_OK);

	m_iClientHealth = 0;
	MESSAGE_BEGIN(MSG_ALL, gmsgHealth);
		WRITE_BYTE(entindex());
		WRITE_SHORT(m_iClientHealth);
	MESSAGE_END();

	SendFOV(0);

	CSGameRules()->CheckWinConditions();
	m_bNotKilled = false;

	BuyZoneIcon_Clear(this);

	SetThink(&CBasePlayer::PlayerDeathThink);
	pev->nextthink = gpGlobals->time + 0.1f;

	pev->angles.x = 0;
	pev->angles.z = 0;

	m_flRespawnPending = 0.0f;
	m_flSpawnProtectionEndTime = 0.0f;
	m_vecOldvAngle = g_vecZero;
}

void CBasePlayer::PlayerDeathThink()
{
	if (m_iJoiningState != JOINED)
		return;

	// If the anim is done playing, go to the next state (waiting for a keypress to
	// either respawn the guy or put him into observer mode).
	if (pev->flags & FL_ONGROUND)
	{
		float flForward = pev->velocity.Length() - 20;
		if (flForward <= 0)
			pev->velocity = g_vecZero;
		else
			pev->velocity = flForward * pev->velocity.Normalize();
	}

	if (HasWeapons())
	{
		// we drop the guns here because weapons that have an area effect and can kill their user
		// will sometimes crash coming back from CBasePlayer::Killed() if they kill their owner because the
		// player class sometimes is freed. It's safer to manipulate the weapons once we know
		// we aren't calling into any of their code anymore through the player pointer.
		PackDeadPlayerItems();
	}

	// Clear inclination came from client view
	pev->angles.x = 0;

	if (pev->modelindex && !m_fSequenceFinished && pev->deadflag == DEAD_DYING)
	{
		StudioFrameAdvance();
		return;
	}

	// once we're done animating our death and we're on the ground, we want to set movetype to None so our dead body won't do collisions and stuff anymore
	// this prevents a bug where the dead body would go to a player's head if he walked over it while the dead player was clicking their button to respawn
	if (pev->movetype != MOVETYPE_NONE && (pev->flags & FL_ONGROUND))
		pev->movetype = MOVETYPE_NONE;

	if (pev->deadflag == DEAD_DYING)
	{
		// Used for a timer.
		m_fDeadTime = gpGlobals->time;
		pev->deadflag = DEAD_DEAD;
	}

	StopAnimation();
	pev->effects |= EF_NOINTERP;

	// do not make a corpse if the player goes to respawn.
	// LUNA: never make a body if the body is not there.
	if (pev->deadflag != DEAD_RESPAWNABLE && !(pev->effects & EF_NODRAW))
	{
		// if the player has been dead for one second longer than allowed by forcerespawn,
		// forcerespawn isn't on. Send the player off to an intermission camera until they choose to respawn.
		if (g_pGameRules->IsMultiplayer() && gpGlobals->time > m_fDeadTime + 3 && !(m_afPhysicsFlags & PFLAG_OBSERVER))
		{
			// Send message to everybody to spawn a corpse.
			SpawnClientSideCorpse();

			// go to dead camera.
			StartDeathCam();
		}
	}

	// wait for all buttons released
	if (pev->deadflag == DEAD_DEAD && m_iTeam != UNASSIGNED && m_iTeam != SPECTATOR)
	{
		if (pev->button)
			return;

		pev->nextthink = gpGlobals->time + 0.1f;
		return;
	}
}

void EXT_FUNC CBasePlayer::RoundRespawn()
{
	m_canSwitchObserverModes = true;

	// teamkill punishment..
	if (m_bJustKilledTeammate && tkpunish.value)
	{
		m_bPunishedForTK = true;
		m_bJustKilledTeammate = false;
	}

	if (m_iMenu != Menu_ChooseAppearance)
	{
		respawn(pev);

		pev->button = 0;
		pev->nextthink = -1;
	}

	m_lastLocation[0] = '\0';

	if (m_bPunishedForTK && pev->health > 0)
	{
		ClientKill(ENT(pev));
	}
}

// StartDeathCam - find an intermission spot and send the
// player off into observer mode
void EXT_FUNC CBasePlayer::StartDeathCam()
{
	m_canSwitchObserverModes = true;

	if (pev->view_ofs == g_vecZero)
	{
		// don't accept subsequent attempts to StartDeathCam()
		return;
	}

	StartObserver(pev->origin, pev->angles);

	if (TheBots)
	{
		TheBots->OnEvent(EVENT_DEATH_CAMERA_START, this);
	}
}

void EXT_FUNC CBasePlayer::StartObserver(Vector& vecPosition, Vector& vecViewAngle)
{
	// clear any clientside entities attached to this player
	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_KILLPLAYERATTACHMENTS);
	WRITE_BYTE(entindex());
	MESSAGE_END();

	// Holster weapon immediately, to allow it to cleanup
	if (m_pActiveItem)
		m_pActiveItem->Holstered();

	if (m_pTank)
	{
		m_pTank->Use(this, this, USE_OFF, 0);
		m_pTank = nullptr;
	}

	// clear out the suit message cache so we don't keep chattering
	SetSuitUpdate(nullptr, SUIT_SENTENCE, SUIT_REPEAT_OK);

	// Tell Ammo Hud that the player is dead
	// deleted by LUNA: gmsgCurWeapon

	// reset FOV
	SendFOV(0);

	// Setup flags
	m_iHideHUD = (HIDEHUD_WEAPONS | HIDEHUD_HEALTH);
	m_afPhysicsFlags |= PFLAG_OBSERVER;
	pev->effects = EF_NODRAW;

	// set position and viewangle
	pev->view_ofs = g_vecZero;
	pev->angles = pev->v_angle = vecViewAngle;
	pev->fixangle = 1;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->movetype = MOVETYPE_NONE;

	// Move them to the new position
	UTIL_SetOrigin(pev, vecPosition);

	m_afPhysicsFlags &= ~PFLAG_DUCKING;
	pev->flags &= ~FL_DUCKING;
	pev->health = 1;

	m_iObserverWeapon = 0;

	// Find a player to watch
	m_flNextObserverInput = 0;

	pev->iuser1 = OBS_NONE;

	Observer_SetMode(m_iObserverLastMode);

	ResetMaxSpeed();

	// Tell all clients this player is now a spectator
	MESSAGE_BEGIN(MSG_ALL, gmsgSpectator);
	WRITE_BYTE(entindex());
	WRITE_BYTE(1);
	MESSAGE_END();
}

bool CanSeeUseable(CBasePlayer *me, CBaseEntity *pEntity)
{
	TraceResult result;
	Vector eye = me->pev->origin + me->pev->view_ofs;

	if (FClassnameIs(pEntity->pev, "hostage_entity"))
	{
		Vector chest = pEntity->pev->origin + Vector(0, 0, HalfHumanHeight);
		Vector head  = pEntity->pev->origin + Vector(0, 0, HumanHeight * 0.9);
		Vector knees = pEntity->pev->origin + Vector(0, 0, StepHeight);

		UTIL_TraceLine(eye, chest, ignore_monsters, ignore_glass, me->edict(), &result);
		if (result.flFraction < 1.0f)
		{
			UTIL_TraceLine(eye, head, ignore_monsters, ignore_glass, pEntity->edict(), &result);
			if (result.flFraction < 1.0f)
			{
				UTIL_TraceLine(eye, knees, ignore_monsters, ignore_glass, pEntity->edict(), &result);
				if (result.flFraction < 1.0f)
				{
					return false;
				}
			}
		}
	}

	return true;
}

void CBasePlayer::PlayerUse()
{
	// Was use pressed or released?
	if (!((pev->button | m_afButtonPressed | m_afButtonReleased) & IN_USE))
		return;

	// Hit Use on a train?
	if (m_afButtonPressed & IN_USE)
	{
		if (m_pTank)
		{
			// Stop controlling the tank
			// TODO: Send HUD Update
			m_pTank->Use(this, this, USE_OFF, 0);
			m_pTank = nullptr;
			return;
		}

		if (m_afPhysicsFlags & PFLAG_ONTRAIN)
		{
			m_iTrain = (TRAIN_NEW | TRAIN_OFF);
			m_afPhysicsFlags &= ~PFLAG_ONTRAIN;

			CBaseEntity *pTrain = Instance(pev->groundentity);
			if (pTrain && pTrain->Classify() == CLASS_VEHICLE)
			{
				((CFuncVehicle *)pTrain)->m_pDriver = nullptr;
			}
			return;
		}
		else
		{
			// Start controlling the train!
			CBaseEntity *pTrain = Instance(pev->groundentity);

			if (pTrain && !(pev->button & IN_JUMP) && (pev->flags & FL_ONGROUND) && (pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE) && pTrain->OnControls(pev))
			{
				m_afPhysicsFlags |= PFLAG_ONTRAIN;

				m_iTrain = TrainSpeed(pTrain->pev->speed, pTrain->pev->impulse);
				m_iTrain |= TRAIN_NEW;

				if (pTrain->Classify() == CLASS_VEHICLE)
				{
					EMIT_SOUND(ENT(pev), CHAN_ITEM, "plats/vehicle_ignition.wav", 0.8, ATTN_NORM);
					((CFuncVehicle *)pTrain)->m_pDriver = this;
				}
				else
					EMIT_SOUND(ENT(pev), CHAN_ITEM, "plats/train_use1.wav", 0.8, ATTN_NORM);

				return;
			}
		}
	}

	CBaseEntity *pObject = nullptr;
	CBaseEntity *pClosest = nullptr;
	Vector vecLOS;
	float flMaxDot = VIEW_FIELD_NARROW;
	float flDot;

	// so we know which way we are facing
	UTIL_MakeVectors(pev->v_angle);

	if (!pClosest)
	{
		while ((pObject = UTIL_FindEntityInSphere(pObject, pev->origin, MAX_PLAYER_USE_RADIUS)))
		{
			if (pObject->ObjectCaps() & (FCAP_IMPULSE_USE | FCAP_CONTINUOUS_USE | FCAP_ONOFF_USE))
			{
				// TODO: PERFORMANCE- should this check be done on a per case basis AFTER we've determined that
				// this object is actually usable? This dot is being done for every object within PLAYER_SEARCH_RADIUS
				// when player hits the use key. How many objects can be in that area, anyway? (sjb)
				vecLOS = (VecBModelOrigin(pObject->pev) - (pev->origin + pev->view_ofs));
				vecLOS.NormalizeInPlace();

				flDot = DotProduct(vecLOS, gpGlobals->v_forward);

				// only if the item is in front of the user
				if (flDot > flMaxDot)
				{
					flMaxDot = flDot;
					pClosest = pObject;
				}
			}
		}
	}

	pObject = pClosest;

	// Found an object
	if (pObject)
	{
		if (CanSeeUseable(this, pObject))
		{
			// TODO: traceline here to prevent +USEing buttons through walls
			int caps = pObject->ObjectCaps();

			if (m_afButtonPressed & IN_USE)
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "common/wpn_select.wav", 0.4, ATTN_NORM);

			if (((pev->button & IN_USE) && (caps & FCAP_CONTINUOUS_USE))
				|| ((m_afButtonPressed & IN_USE) && (caps & (FCAP_IMPULSE_USE | FCAP_ONOFF_USE))))
			{
				if (caps & FCAP_CONTINUOUS_USE)
					m_afPhysicsFlags |= PFLAG_USING;

				pObject->Use(this, this, USE_SET, 1);
			}
			// UNDONE: Send different USE codes for ON/OFF.  Cache last ONOFF_USE object to send 'off' if you turn away
			// BUGBUG This is an "off" use
			else if ((m_afButtonReleased & IN_USE) && (pObject->ObjectCaps() & FCAP_ONOFF_USE))
			{
				pObject->Use(this, this, USE_SET, 0);
			}
		}
	}
	else if (m_afButtonPressed & IN_USE)
	{
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "common/wpn_denyselect.wav", 0.4, ATTN_NORM);
	}
}

void CBasePlayer::HostageUsed()
{
	if (m_flDisplayHistory & DHF_HOSTAGE_USED)
		return;

	if (m_iTeam == TERRORIST)
		HintMessage("#Hint_use_hostage_to_stop_him");

	else if (m_iTeam == CT)
		HintMessage("#Hint_lead_hostage_to_rescue_point");

	m_flDisplayHistory |= DHF_HOSTAGE_USED;
}

void EXT_FUNC CBasePlayer::Jump()
{
	if (pev->flags & FL_WATERJUMP)
		return;

	if (pev->waterlevel >= 2)
	{
		return;
	}

	// jump velocity is sqrt( height * gravity * 2)
	// If this isn't the first frame pressing the jump button, break out.
	if (!(m_afButtonPressed & IN_JUMP))
	{
		// don't pogo stick
		return;
	}

	if (!(pev->flags & FL_ONGROUND) || !pev->groundentity)
		return;

	// many features in this function use v_forward, so makevectors now.
	UTIL_MakeVectors(pev->angles);
	SetAnimation(PLAYER_JUMP);

	if ((pev->flags & FL_DUCKING) || (m_afPhysicsFlags & PFLAG_DUCKING))
	{
		if (m_fLongJump && (pev->button & IN_DUCK) && (gpGlobals->time - m_flDuckTime < 1.0f) && pev->velocity.Length() > 50)
		{
			SetAnimation(PLAYER_SUPERJUMP);
		}
	}

	// If you're standing on a conveyor, add it's velocity to yours (for momentum)
	entvars_t *pevGround = VARS(pev->groundentity);
	if (pevGround)
	{
		if (pevGround->flags & FL_CONVEYOR)
		{
			pev->velocity = pev->velocity + pev->basevelocity;
		}

		if (FClassnameIs(pevGround, "func_tracktrain")
			|| FClassnameIs(pevGround, "func_train")
			|| FClassnameIs(pevGround, "func_vehicle"))
		{
			pev->velocity = pevGround->velocity + pev->velocity;
		}
	}
}

// This is a glorious hack to find free space when you've crouched into some solid space
// Our crouching collisions do not work correctly for some reason and this is easier
// than fixing the problem :(
NOXREF void FixPlayerCrouchStuck(edict_t *pPlayer)
{
	TraceResult trace;

	// Move up as many as 18 pixels if the player is stuck.
	for (int i = 0; i < 18; i++)
	{
		UTIL_TraceHull(pPlayer->v.origin, pPlayer->v.origin, dont_ignore_monsters, head_hull, pPlayer, &trace);

		if (trace.fStartSolid)
			pPlayer->v.origin.z++;
		else
			break;
	}
}

void EXT_FUNC CBasePlayer::Duck()
{
	if (pev->button & IN_DUCK)
		SetAnimation(PLAYER_WALK);
}

int EXT_FUNC CBasePlayer::ObjectCaps()
{
	return (CBaseMonster::ObjectCaps() & ~FCAP_ACROSS_TRANSITION);
}

// ID's player as such.
int EXT_FUNC CBasePlayer::Classify()
{
	return CLASS_PLAYER;
}

void EXT_FUNC CBasePlayer::AddPoints(int score, BOOL bAllowNegativeScore)
{
	// Positive score always adds
	if (score < 0 && !bAllowNegativeScore)
	{
		// Can't go more negative
		if (pev->frags < 0)
			return;

		if (-score > pev->frags)
		{
			// Sum will be 0
			score = -pev->frags;
		}
	}

	pev->frags += score;

	MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
		WRITE_BYTE(ENTINDEX(edict()));
		WRITE_SHORT(int(pev->frags));
		WRITE_SHORT(m_iDeaths);
		WRITE_SHORT(0);
		WRITE_SHORT(m_iTeam);
	MESSAGE_END();
}

void EXT_FUNC CBasePlayer::AddPointsToTeam(int score, BOOL bAllowNegativeScore)
{
	int index = entindex();
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);

		if (pPlayer && i != index)
		{
			if (g_pGameRules->PlayerRelationship(this, pPlayer) == GR_TEAMMATE)
			{
				pPlayer->AddPoints(score, bAllowNegativeScore);
			}
		}
	}
}

void EXT_FUNC CBasePlayer::PreThink()
{
	// Merge shooting recoil into bot's v_angle
	if (IsBot() && m_vecVAngleShift.LengthSquared())
	{
		pev->v_angle += m_vecVAngleShift;
		pev->fixangle = TRUE;
		m_vecVAngleShift = g_vecZero;
	}

	// These buttons have changed this frame
	int buttonsChanged = (m_afButtonLast ^ pev->button);

	// this means the player has pressed or released a key
	if (buttonsChanged)
	{
		m_fLastMovement = gpGlobals->time;
	}

	// Debounced button codes for pressed/released
	// UNDONE: Do we need auto-repeat?
	m_afButtonPressed  = (buttonsChanged & pev->button);		// The changed ones still down are "pressed"
	m_afButtonReleased = (buttonsChanged & (~pev->button));		// The ones not down are "released"

	// Hint messages should be updated even if the game is over
	m_hintMessageQueue.Update(this);
	g_pGameRules->PlayerThink(this);

	if (g_pGameRules->IsGameOver())
	{
		// intermission or finale
		return;
	}

	if (m_iJoiningState != JOINED)
		JoiningThink();

	// Mission Briefing text, remove it when the player hits an important button
	if (m_bMissionBriefing)
	{
		if (m_afButtonPressed & (IN_ATTACK | IN_ATTACK2))
		{
			m_afButtonPressed &= ~(IN_ATTACK | IN_ATTACK2);
			RemoveLevelText();
			m_bMissionBriefing = false;
		}
	}

	// is this still used?
	UTIL_MakeVectors(pev->v_angle);

	ItemPreFrame();
	WaterMove();

	if (pev->flags & FL_ONGROUND)
	{
		// Slow down the player based on the velocity modifier
		if (m_flVelocityModifier < 1.0f)
		{
			real_t modvel = m_flVelocityModifier + 0.01;

			m_flVelocityModifier = modvel;
			pev->velocity = pev->velocity * modvel;
		}

		if (m_flVelocityModifier > 1.0f)
			m_flVelocityModifier = 1;
	}

	if (IsAlive() && (m_flIdleCheckTime <= (double)gpGlobals->time || m_flIdleCheckTime == 0.0f))
	{
		// check every 5 seconds
		m_flIdleCheckTime = gpGlobals->time + 5.0;

		if (CheckActivityInGame())
		{
			m_fLastMovement = gpGlobals->time;
		}

		real_t flLastMove = gpGlobals->time - m_fLastMovement;

		//check if this player has been inactive for 2 rounds straight
		if (!IsBot() && flLastMove > CSGameRules()->m_fMaxIdlePeriod)
		{
			DropIdlePlayer("Player idle");

			m_fLastMovement = gpGlobals->time;
		}
	}

	if (g_pGameRules && g_pGameRules->FAllowFlashlight())
		m_iHideHUD &= ~HIDEHUD_FLASHLIGHT;
	else
		m_iHideHUD |= HIDEHUD_FLASHLIGHT;

	// JOHN: checks if new client data (for HUD and view control) needs to be sent to the client
	UpdateClientData();
	QueryClientCvar();

	CheckTimeBasedDamage();
	CheckSuitUpdate();

	// So the correct flags get sent to client asap.
	if (m_afPhysicsFlags & PFLAG_ONTRAIN)
		pev->flags |= FL_ONTRAIN;
	else
		pev->flags &= ~FL_ONTRAIN;

	PlayerRespawnThink();

	// Observer Button Handling
	if (GetObserverMode() != OBS_NONE && (m_afPhysicsFlags & PFLAG_OBSERVER))
	{
		Observer_Think();
		return;
	}

	if (pev->deadflag >= DEAD_DYING && pev->deadflag != DEAD_RESPAWNABLE)
	{
		PlayerDeathThink();
		return;
	}

	// new code to determine if a player is on a train or not
	CBaseEntity *pGroundEntity = Instance(pev->groundentity);
	if (pGroundEntity && pGroundEntity->Classify() == CLASS_VEHICLE)
	{
		pev->iuser4 = 1;
	}
	else
	{
		pev->iuser4 = 0;
	}

	// Train speed control
	if (m_afPhysicsFlags & PFLAG_ONTRAIN)
	{
		CBaseEntity *pTrain = Instance(pev->groundentity);
		float vel;

		if (!pTrain)
		{
			TraceResult trainTrace;
			// Maybe this is on the other side of a level transition
			UTIL_TraceLine(pev->origin, pev->origin + Vector(0, 0, -38), ignore_monsters, ENT(pev), &trainTrace);

			// HACKHACK - Just look for the func_tracktrain classname
			if (trainTrace.flFraction != 1.0f && trainTrace.pHit)
				pTrain = Instance(trainTrace.pHit);

			if (!pTrain || !(pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE) || !pTrain->OnControls(pev))
			{
				m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
				m_iTrain = (TRAIN_NEW | TRAIN_OFF);
				((CFuncVehicle *)pTrain)->m_pDriver = nullptr;
				return;
			}
		}
		else if (!(pev->flags & FL_ONGROUND) || (pTrain->pev->spawnflags & SF_TRACKTRAIN_NOCONTROL))
		{
			// Turn off the train if you jump, strafe, or the train controls go dead
			m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
			m_iTrain = (TRAIN_NEW | TRAIN_OFF);
			((CFuncVehicle *)pTrain)->m_pDriver = nullptr;
			return;
		}

		pev->velocity = g_vecZero;
		vel = 0;

		if (pTrain->Classify() == CLASS_VEHICLE)
		{
			if (pev->button & IN_FORWARD)
			{
				vel = 1;
				pTrain->Use(this, this, USE_SET, vel);
			}

			if (pev->button & IN_BACK)
			{
				vel = -1;
				pTrain->Use(this, this, USE_SET, vel);
			}

			if (pev->button & IN_MOVELEFT)
			{
				vel = 20;
				pTrain->Use(this, this, USE_SET, vel);
			}
			if (pev->button & IN_MOVERIGHT)
			{
				vel = 30;
				pTrain->Use(this, this, USE_SET, vel);
			}
		}
		else
		{
			if (m_afButtonPressed & IN_FORWARD)
			{
				vel = 1;
				pTrain->Use(this, this, USE_SET, vel);
			}
			else if (m_afButtonPressed & IN_BACK)
			{
				vel = -1;
				pTrain->Use(this, this, USE_SET, vel);
			}
		}

		if (vel)
		{
			m_iTrain = TrainSpeed(pTrain->pev->speed, pTrain->pev->impulse);
			m_iTrain |= (TRAIN_ACTIVE | TRAIN_NEW);
		}
	}
	else if (m_iTrain & TRAIN_ACTIVE)
	{
		// turn off train
		m_iTrain = TRAIN_NEW;
	}

	if (pev->button & IN_JUMP)
	{
		// If on a ladder, jump off the ladder
		// else Jump
		Jump();
	}

	// If trying to duck, already ducked, or in the process of ducking
	if ((pev->button & IN_DUCK) || (pev->flags & FL_DUCKING) || (m_afPhysicsFlags & PFLAG_DUCKING))
	{
		Duck();
	}

	if (!(pev->flags & FL_ONGROUND))
	{
		m_flFallVelocity = -pev->velocity.z;
	}

	// TODO: (HACKHACK) Can't be hit by traceline when not animating?
	//StudioFrameAdvance();

	// Clear out ladder pointer
	m_hEnemy = nullptr;

	if (m_afPhysicsFlags & PFLAG_ONBARNACLE)
	{
		pev->velocity = g_vecZero;
	}

	if (!(m_flDisplayHistory & DHF_ROUND_STARTED))
	{
		HintMessage("#Hint_press_buy_to_purchase", FALSE);
		m_flDisplayHistory |= DHF_ROUND_STARTED;
	}

	UpdateLocation();

	gFrozenDOTMgr::Think(this);
	gElectrifiedDOTMgr::Think(this);
	gPoisonDOTMgr::Think(this);
	gBurningDOTMgr::Think(this);
}

// If player is taking time based damage, continue doing damage to player -
// this simulates the effect of being poisoned, gassed, dosed with radiation etc -
// anything that continues to do damage even after the initial contact stops.
// Update all time based damage counters, and shut off any that are done.

// The m_bitsDamageType bit MUST be set if any damage is to be taken.
// This routine will detect the initial on value of the m_bitsDamageType
// and init the appropriate counter.  Only processes damage every second.
void CBasePlayer::CheckTimeBasedDamage()
{
	int i;
	byte bDuration = 0;

	if (!(m_bitsDamageType & DMG_TIMEBASED))
		return;

	// only check for time based damage approx. every 2 seconds
	if (Q_abs(gpGlobals->time - m_tbdPrev) < 2.0f)
		return;

	m_tbdPrev = gpGlobals->time;

	for (i = 0; i < ITBD_END; i++)
	{
		// make sure bit is set for damage type
		if (m_bitsDamageType & (DMG_PARALYZE << i))
		{
			switch (i)
			{
			case ITBD_PARALLYZE:
				// UNDONE - flag movement as half-speed
				bDuration = PARALYZE_DURATION;
				break;
			case ITBD_NERVE_GAS:
				bDuration = NERVEGAS_DURATION;
				break;
			case ITBD_POISON:
			{
				TakeDamage(pev, pev, POISON_DAMAGE, DMG_GENERIC);
				bDuration = POISON_DURATION;
				break;
			}
			case ITBD_DROWN_RECOVER:
			{
				// NOTE: this hack is actually used to RESTORE health
				// after the player has been drowning and finally takes a breath
				if (m_idrowndmg > m_idrownrestored)
				{
					int idif = Q_min(m_idrowndmg - m_idrownrestored, 10);

					TakeHealth(idif, DMG_GENERIC);
					m_idrownrestored += idif;
				}
				// get up to 5*10 = 50 points back
				bDuration = 4;
				break;
			}
			case ITBD_RADIATION:
				bDuration = RADIATION_DURATION;
				break;
			case ITBD_ACID:
				bDuration = ACID_DURATION;
				break;
			case ITBD_SLOW_BURN:
				bDuration = SLOWBURN_DURATION;
				break;
			case ITBD_SLOW_FREEZE:
				bDuration = SLOWFREEZE_DURATION;
				break;
			default:
				bDuration = 0;
				break;
			}

			if (m_rgbTimeBasedDamage[i])
			{
				// use up an antitoxin on poison or nervegas after a few seconds of damage
				if ((i == ITBD_NERVE_GAS && m_rgbTimeBasedDamage[i] < NERVEGAS_DURATION) || (i == ITBD_POISON && m_rgbTimeBasedDamage[i] < POISON_DURATION))
				{
					if (m_rgItems[ITEM_ID_ANTIDOTE])
					{
						m_rgbTimeBasedDamage[i] = 0;
						m_rgItems[ITEM_ID_ANTIDOTE]--;
						SetSuitUpdate("!HEV_HEAL4", SUIT_SENTENCE, SUIT_REPEAT_OK);
					}
				}

				// decrement damage duration, detect when done.
				if (!m_rgbTimeBasedDamage[i] || --m_rgbTimeBasedDamage[i] == 0)
				{
					m_rgbTimeBasedDamage[i] = 0;
					// if we're done, clear damage bits
					m_bitsDamageType &= ~(DMG_PARALYZE << i);
				}
			}
			else
				// first time taking this damage type - init damage duration
				m_rgbTimeBasedDamage[i] = bDuration;
		}
	}
}

void CBasePlayer::UpdateGeigerCounter()
{
	byte range;

	// delay per update ie: don't flood net with these msgs
	if (gpGlobals->time < m_flgeigerDelay)
		return;

	m_flgeigerDelay = gpGlobals->time + 0.25;

	// send range to radition source to client
	range = byte(m_flgeigerRange / 4.0);//* 0.25);		// TODO: ACHECK!

	if (range != m_igeigerRangePrev)
	{
		m_igeigerRangePrev = range;

		MESSAGE_BEGIN(MSG_ONE, gmsgGeigerRange, nullptr, pev);
			WRITE_BYTE(range);
		MESSAGE_END();
	}

	// reset counter and semaphore
	if (!RANDOM_LONG(0, 3))
	{
		m_flgeigerRange = 1000.0;
	}
}

void CBasePlayer::CheckSuitUpdate()
{
	int i;
	int isentence = 0;
	int isearch = m_iSuitPlayNext;

	// Ignore suit updates if no suit
	if (!(pev->weapons & (1 << WEAPON_SUIT)))
		return;

	// if in range of radiation source, ping geiger counter
	UpdateGeigerCounter();

	if (g_pGameRules->IsMultiplayer())
	{
		// don't bother updating HEV voice in multiplayer.
		return;
	}

	if (gpGlobals->time >= m_flSuitUpdate && m_flSuitUpdate > 0)
	{
		// play a sentence off of the end of the queue
		for (i = 0; i < MAX_SUIT_NOREPEAT; i++)
		{
			if ((isentence = m_rgSuitPlayList[isearch]))
				break;

			if (++isearch == MAX_SUIT_NOREPEAT)
				isearch = 0;
		}

		if (isentence)
		{
			m_rgSuitPlayList[isearch] = 0;

			if (isentence > 0)
			{
				// play sentence number
				char sentence[MAX_SENTENCE_NAME + 1];
				Q_strcpy(sentence, "!");
				Q_strcat(sentence, gszallsentencenames[isentence]);
				EMIT_SOUND_SUIT(ENT(pev), sentence);
			}
			else
			{
				// play sentence group
				EMIT_GROUPID_SUIT(ENT(pev), -isentence);
			}

			m_flSuitUpdate = gpGlobals->time + SUIT_UPDATE_TIME;
		}
		else
			// queue is empty, don't check
			m_flSuitUpdate = 0;
	}
}

// add sentence to suit playlist queue. if group is true, then
// name is a sentence group (HEV_AA), otherwise name is a specific
// sentence name ie: !HEV_AA0.  If iNoRepeat is specified in
// seconds, then we won't repeat playback of this word or sentence
// for at least that number of seconds.
void CBasePlayer::SetSuitUpdate(char *name, bool group, int iNoRepeatTime)
{
	;
}

void CBasePlayer::CheckPowerups()
{
	if (pev->health <= 0.0f)
		return;

	// don't use eyes
	pev->modelindex = m_modelIndexPlayer;
}

void CBasePlayer::SetNewPlayerModel(const char *modelName)
{
	SET_MODEL(edict(), modelName);
	m_modelIndexPlayer = pev->modelindex;

	ResetSequenceInfo();
}

void EXT_FUNC CBasePlayer::PostThink()
{
	// intermission or finale
	if (g_pGameRules->IsGameOver())
		goto pt_end;

	if (!IsAlive())
		goto pt_end;

	// Handle Tank controlling
	if (m_pTank)
	{
		// if they've moved too far from the gun,  or selected a weapon, unuse the gun
		if (m_pTank->OnControls(pev) && !pev->weaponmodel)
		{
			// try fire the gun
			m_pTank->Use(this, this, USE_SET, 2);
		}
		else
		{
			// they've moved off the platform
			m_pTank->Use(this, this, USE_OFF, 0);
			m_pTank = nullptr;
		}
	}

	// do weapon stuff
	ItemPostFrame();

	// check to see if player landed hard enough to make a sound
	// falling farther than half of the maximum safe distance, but not as far a max safe distance will
	// play a bootscrape sound, and no damage will be inflicted. Fallling a distance shorter than half
	// of maximum safe distance will make no sound. Falling farther than max safe distance will play a
	// fallpain sound, and damage will be inflicted based on how far the player fell
	if ((pev->flags & FL_ONGROUND) && pev->health > 0.0f && m_flFallVelocity >= PLAYER_FALL_PUNCH_THRESHHOLD)
	{
		if (pev->watertype != CONTENT_WATER)
		{
			// after this point, we start doing damage
			if (m_flFallVelocity > MAX_PLAYER_SAFE_FALL_SPEED)
			{
				float flFallDamage = g_pGameRules->FlPlayerFallDamage(this);

				// splat
				if (flFallDamage > pev->health)
				{
					// NOTE: play on item channel because we play footstep landing on body channel
					EMIT_SOUND(ENT(pev), CHAN_ITEM, "common/bodysplat.wav", VOL_NORM, ATTN_NORM);
				}

				if (flFallDamage >= 1.0f)
				{
					m_LastHitGroup = HITGROUP_GENERIC;
					TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), flFallDamage, DMG_FALL);
					pev->punchangle.x = 0;
					if (TheBots)
					{
						TheBots->OnEvent(EVENT_PLAYER_LANDED_FROM_HEIGHT, this);
					}
				}
			}
		}

		if (IsAlive())
		{
			SetAnimation(PLAYER_WALK);
		}
	}

	if (pev->flags & FL_ONGROUND)
	{
		m_flFallVelocity = 0;
	}

	// select the proper animation for the player character
	if (IsAlive())
	{
		if (pev->velocity.x || pev->velocity.y)
		{
			if (((pev->velocity.x || pev->velocity.y) && (pev->flags & FL_ONGROUND)) || pev->waterlevel > 1)
				SetAnimation(PLAYER_WALK);
		}
		else if (pev->gaitsequence != ACT_FLY)
			SetAnimation(PLAYER_IDLE);
	}

	StudioFrameAdvance();
	CheckPowerups();

	if (m_flTimeStepSound)
	{
		pev->flTimeStepSound = int(m_flTimeStepSound);
	}

	// no Overhealing think for the dead, of course.
	gOverHealingMgr::Think(this);

pt_end:
#ifdef CLIENT_WEAPONS
	// Decay timers on weapons
	// go through all of the weapons and make a list of the ones to pack
	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		if (m_rgpPlayerItems[i])
		{
			m_rgpPlayerItems[i]->m_flNextPrimaryAttack = Q_max(m_rgpPlayerItems[i]->m_flNextPrimaryAttack - gpGlobals->frametime, -1.0f);
			m_rgpPlayerItems[i]->m_flNextSecondaryAttack = Q_max(m_rgpPlayerItems[i]->m_flNextSecondaryAttack - gpGlobals->frametime, -0.001f);

			if (m_rgpPlayerItems[i]->m_flTimeWeaponIdle != 1000.0f)
			{
				m_rgpPlayerItems[i]->m_flTimeWeaponIdle = Q_max(m_rgpPlayerItems[i]->m_flTimeWeaponIdle - gpGlobals->frametime, -0.001f);
			}

			// used by original CBasePlayerItem.
			//pPlayerItem = pPlayerItem->m_pNext;
		}
	}

#ifdef CHECKING_NEXT_PRIM_ATTACK_SYNC
	if (m_pActiveItem && m_pActiveItem->m_flNextPrimaryAttack > 0.0f)
		SERVER_PRINT(SharedVarArgs("[Server] m_flNextPrimaryAttack: %f\n", m_pActiveItem->m_flNextPrimaryAttack));
#endif

	m_flNextAttack -= gpGlobals->frametime;

	if (m_flNextAttack < -0.001)
		m_flNextAttack = -0.001;
#endif // CLIENT_WEAPONS

	// Track button info so we can detect 'pressed' and 'released' buttons next frame
	m_afButtonLast = pev->button;
	m_iGaitsequence = pev->gaitsequence;

	StudioProcessGait();

	// Skill Think() functions are called from here.
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->Think();
	}
}

// checks if the spot is clear of players
BOOL IsSpawnPointValid(CBaseEntity *pPlayer, CBaseEntity *pSpot)
{
	if (!pSpot->IsTriggered(pPlayer))
		return FALSE;

	if (!kill_filled_spawn.value)
		return TRUE;

	CBaseEntity *pEntity = nullptr;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pSpot->pev->origin, MAX_PLAYER_USE_RADIUS)))
	{
		// if ent is a client, don't spawn on 'em
		if (pEntity->IsPlayer() && pEntity != pPlayer)
			return FALSE;
	}

	return TRUE;
}

bool CBasePlayer::SelectSpawnSpot(const char *pEntClassName, CBaseEntity *&pSpot)
{
	edict_t *pPlayer = edict();

	// Find the next spawn spot.
	pSpot = UTIL_FindEntityByClassname(pSpot, pEntClassName);

	// skip over the null point
	if (FNullEnt(pSpot))
	{
		pSpot = UTIL_FindEntityByClassname(pSpot, pEntClassName);
	}

	CBaseEntity *pFirstSpot = pSpot;

	do
	{
		if (pSpot)
		{
			// check if pSpot is valid
			if (IsSpawnPointValid(this, pSpot))
			{
				if (pSpot->pev->origin == Vector(0, 0, 0))
				{
					pSpot = UTIL_FindEntityByClassname(pSpot, pEntClassName);
					continue;
				}

				// if so, go to pSpot
				return true;
			}
		}

		// increment pSpot
		pSpot = UTIL_FindEntityByClassname(pSpot, pEntClassName);
	}
	// loop if we're not back to the start
	while (pSpot != pFirstSpot);

	// we haven't found a place to spawn yet,  so kill any guy at the first spawn point and spawn there
	if (!FNullEnt(pSpot))
	{
		if (kill_filled_spawn.value != 0.0)
		{
			CBaseEntity *pEntity = nullptr;
			while ((pEntity = UTIL_FindEntityInSphere(pEntity, pSpot->pev->origin, MAX_PLAYER_USE_RADIUS)))
			{
				// if ent is a client, kill em (unless they are ourselves)
				if (pEntity->IsPlayer() && pEntity->edict() != pPlayer)
				{
					pEntity->TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), 200, DMG_GENERIC);
				}
			}
		}

		// if so, go to pSpot
		return true;
	}

	return false;
}

CBaseEntity *g_pLastSpawn;
CBaseEntity *g_pLastCTSpawn;
CBaseEntity *g_pLastTerroristSpawn;

edict_t *CBasePlayer::EntSelectSpawnPoint()
{
	CBaseEntity *pSpot;

	// choose a info_player_deathmatch point
	if (g_pGameRules->IsCoOp())
	{
		pSpot = UTIL_FindEntityByClassname(g_pLastSpawn, "info_player_coop");

		if (!FNullEnt(pSpot))
			goto ReturnSpot;

		pSpot = UTIL_FindEntityByClassname(g_pLastSpawn, "info_player_start");

		if (!FNullEnt(pSpot))
			goto ReturnSpot;
	}
	// the counter-terrorist spawns at "info_player_start"
	else if (g_pGameRules->IsDeathmatch() && m_iTeam == CT)
	{
		pSpot = g_pLastCTSpawn;

		if (SelectSpawnSpot("info_player_start", pSpot))
		{
			goto ReturnSpot;
		}
	}
	// The terrorist spawn points
	else if (g_pGameRules->IsDeathmatch() && m_iTeam == TERRORIST)
	{
		pSpot = g_pLastTerroristSpawn;

		if (SelectSpawnSpot("info_player_deathmatch", pSpot))
		{
			goto ReturnSpot;
		}
	}

	// If startspot is set, (re)spawn there.
	if (FStringNull(gpGlobals->startspot) || !Q_strlen(STRING(gpGlobals->startspot)))
	{
		pSpot = UTIL_FindEntityByClassname(nullptr, "info_player_deathmatch");

		if (!FNullEnt(pSpot))
			goto ReturnSpot;
	}
	else
	{
		pSpot = UTIL_FindEntityByTargetname(nullptr, STRING(gpGlobals->startspot));

		if (!FNullEnt(pSpot))
			goto ReturnSpot;
	}

ReturnSpot:
	if (FNullEnt(pSpot))
	{
		ALERT(at_error, "PutClientInServer: no info_player_start on level\n");
		return INDEXENT(0);
	}

	if (m_iTeam == TERRORIST)
		g_pLastTerroristSpawn = pSpot;
	else
		g_pLastCTSpawn = pSpot;

	return pSpot->edict();
}

void CBasePlayer::SetScoreAttrib(CBasePlayer *dest)
{
	int state = 0;
	if (pev->deadflag != DEAD_NO)
		state |= SCORE_STATUS_DEAD;

	// TODO: Remove these fixes when they are implemented on the client side
	if (state & (SCORE_STATUS_GODFATHER | SCORE_STATUS_DEFKIT) && GetForceCamera(dest) != CAMERA_MODE_SPEC_ANYONE)
	{
		if (CSGameRules()->PlayerRelationship(this, dest) != GR_TEAMMATE)
			state &= ~(SCORE_STATUS_GODFATHER | SCORE_STATUS_DEFKIT);
	}

	if (gmsgScoreAttrib)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgScoreAttrib, nullptr, dest->pev);
			WRITE_BYTE(entindex());
			WRITE_BYTE(state);
		MESSAGE_END();
	}
}

void EXT_FUNC CBasePlayer::Spawn()
{
	int i;

	// Do not allow to do spawn, if player chooses a team or appearance.
	if (m_bJustConnected && m_iJoiningState == PICKINGTEAM)
	{
		return;
	}

	m_iGaitsequence = 0;

	m_flGaitframe = 0;
	m_flGaityaw = 0;
	m_flGaitMovement = 0;
	m_prevgaitorigin = Vector(0, 0, 0);
	m_progressStart = 0;
	m_progressEnd = 0;

	MAKE_STRING_CLASS("player", pev);

	pev->health = 100;

	if (!m_bNotKilled)
	{
		pev->armorvalue = 0;
		pev->armortype = ARMOR_NONE;
	}

	pev->maxspeed = 1000;
	pev->takedamage = DAMAGE_AIM;
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_WALK;
	pev->max_health = pev->health;

	pev->flags &= FL_PROXY;
	pev->flags |= FL_CLIENT;
	pev->air_finished = gpGlobals->time + AIRTIME;
	pev->dmg = 2;
	pev->effects = 0;
	pev->deadflag = DEAD_NO;
	pev->dmg_take = 0;
	pev->dmg_save = 0;

	pev->watertype = CONTENTS_EMPTY;
	pev->waterlevel = 0;
	pev->basevelocity = g_vecZero;	// pushed by trigger_push

	m_bitsHUDDamage = -1;
	m_bitsDamageType = 0;
	m_afPhysicsFlags = 0;
	m_fLongJump = FALSE;
	m_iClientFOV = 0;
	m_pentCurBombTarget = nullptr;

	if (m_bOwnsShield)
		pev->gamestate = HITGROUP_SHIELD_ENABLED;
	else
		pev->gamestate = HITGROUP_SHIELD_DISABLED;

	ResetStamina();
	pev->friction = 1;
	pev->gravity = 1;

	SET_PHYSICS_KEY_VALUE(edict(), "slj", "0");
	SET_PHYSICS_KEY_VALUE(edict(), "hl", "1");
	m_hintMessageQueue.Reset();

	m_flVelocityModifier = 1;
	m_iLastZoom = DEFAULT_FOV;
	m_flLastTalk = 0;
	m_flIdleCheckTime = 0;
	m_flRadioTime = 0;
	m_iRadioMessages = int(radio_maxinround.value);
	m_bKilledByGrenade = false;
	m_flDisplayHistory &= ~DHM_ROUND_CLEAR;
	m_tmHandleSignals = 0;
	m_fCamSwitch = 0;
	m_iChaseTarget = 1;
	m_tmNextRadarUpdate = gpGlobals->time;

	m_vLastOrigin = Vector(0, 0, 0);
	m_iCurrentKickVote = 0;
	m_flNextVoteTime = 0;
	m_bJustKilledTeammate = false;

	SET_VIEW(ENT(pev), ENT(pev));

	m_hObserverTarget = nullptr;
	pev->iuser1 =
		pev->iuser2 =
		pev->iuser3 = 0;

	m_flLastFired = -15;
	m_bHeadshotKilled = false;
	m_bReceivesNoMoneyNextRound = false;
	m_bShieldDrawn = false;

	m_blindUntilTime = 0;
	m_blindStartTime = 0;
	m_blindHoldTime = 0;
	m_blindFadeTime = 0;
	m_blindAlpha = 0;

	m_canSwitchObserverModes = true;
	m_lastLocation[0] = '\0';

	m_bitsDamageType &= ~(DMG_DROWN | DMG_DROWNRECOVER);
	m_rgbTimeBasedDamage[ITBD_DROWN_RECOVER] = 0;
	m_idrowndmg = 0;
	m_idrownrestored = 0;

	MESSAGE_BEGIN(MSG_ONE, SVC_ROOMTYPE, nullptr, pev);
		WRITE_SHORT(int(CVAR_GET_FLOAT("room_type")));
	MESSAGE_END();

	if (g_pGameRules->IsFreezePeriod())
		m_bCanShoot = false;
	else
		m_bCanShoot = true;

	m_iNumSpawns++;
	InitStatusBar();

	for (i = 0; i < MAX_RECENT_PATH; i++)
		m_vRecentPath[i] = Vector(0, 0, 0);

	pev->fov = DEFAULT_FOV;
	m_flNextDecalTime = 0;
	m_flTimeStepSound = 0;
	m_iStepLeft = 0;
	m_flFieldOfView = 0.5;
	m_bloodColor = BLOOD_COLOR_RED;
	m_flNextAttack = 0;
	m_flgeigerDelay = gpGlobals->time + 2;

	m_iFlashBattery = 99;
	m_flFlashLightTime = 1;

	ReloadWeapons();

	pev->body = 0;

	if (m_bMissionBriefing)
	{
		RemoveLevelText();
		m_bMissionBriefing = false;
	}

	m_flFallVelocity = 0;

	CSGameRules()->GetPlayerSpawnSpot(this);

	if (!pev->modelindex)
	{
		// get rid of the dependency on m_modelIndexPlayer.
		SET_MODEL(ENT(pev), "models/player.mdl");
		m_modelIndexPlayer = pev->modelindex;
	}

	pev->sequence = LookupActivity(ACT_IDLE);

	if (pev->flags & FL_DUCKING)
		UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);

	// Override what CBasePlayer set for the view offset.
	pev->view_ofs = VEC_VIEW;
	Precache();

	m_HackedGunPos = Vector(0, 32, 0);

	m_iHideHUD &= ~(HIDEHUD_WEAPONS | HIDEHUD_HEALTH | HIDEHUD_TIMER | HIDEHUD_MONEY | HIDEHUD_CROSSHAIR);
	m_fNoPlayerSound = FALSE;
	m_pLastItem = nullptr;
	m_bClientWeaponUpToDate = FALSE;
	m_pClientActiveItem = nullptr;
	m_iClientBattery = -1;

	m_iClientHideHUD = -1;

	if (!m_bNotKilled)
	{

		for (i = 0; i < MAX_AMMO_SLOTS; i++)
			m_rgAmmo[i] = 0;

		m_bHasPrimary = false;
		m_bHasNightVision = false;

		m_iHideHUD |= HIDEHUD_WEAPONS;

		SendItemStatus();
	}
	else
	{
		for (i = 0; i < MAX_AMMO_SLOTS; i++)
			m_rgAmmoLast[i] = -1;
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgNVGToggle, nullptr, pev);
		WRITE_BYTE(0);
	MESSAGE_END();

	m_bNightVisionOn = false;

	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pObserver = UTIL_PlayerByIndex(i);

		if (pObserver && pObserver->IsObservingPlayer(this))
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgNVGToggle, nullptr, pObserver->pev);
				WRITE_BYTE(0);
			MESSAGE_END();

			pObserver->m_bNightVisionOn = false;
		}
	}

	m_lastx = m_lasty = 0;

	g_pGameRules->PlayerSpawn(this);

	m_bNotKilled = true;

	m_flRespawnPending = 0.0f;
	m_flSpawnProtectionEndTime = 0.0f;
	m_vecOldvAngle = g_vecZero;

	// Get rid of the progress bar...
	SetProgressBarTime(0);
	ResetMaxSpeed();

	UTIL_SetOrigin(pev, pev->origin);

	SetScoreboardAttributes();

	MESSAGE_BEGIN(MSG_ALL, gmsgTeamInfo);
		WRITE_BYTE(entindex());
		WRITE_BYTE(m_iTeam);
	MESSAGE_END();

	UpdateLocation(true);

	MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
		WRITE_BYTE(ENTINDEX(edict()));
		WRITE_SHORT(int(pev->frags));
		WRITE_SHORT(m_iDeaths);
		WRITE_SHORT(0);
		WRITE_SHORT(m_iTeam);
	MESSAGE_END();

	if (m_bHasChangedName)
	{
		char *infobuffer = GET_INFO_BUFFER(edict());

		if (!FStrEq(m_szNewName, GET_KEY_VALUE(infobuffer, "name")))
		{
			SET_CLIENT_KEY_VALUE(entindex(), infobuffer, "name", m_szNewName);
		}

		m_bHasChangedName = false;
		m_szNewName[0] = '\0';
	}

	UTIL_ScreenFade(this, Vector(0, 0, 0), 0.001);
	SyncRoundTimer();

	if (TheBots)
	{
		TheBots->OnEvent(EVENT_PLAYER_SPAWNED, this);
	}

	m_allowAutoFollowTime = false;

	for (i = 0; i < COMMANDS_TO_TRACK; i++)
		m_flLastCommandTime[i] = -1;

	m_flOHNextThink = 0.0f;
	m_flOHOriginalHealth = pev->max_health;
	m_flFrozenNextThink = 0.0f;
	gElectrifiedDOTMgr::Free(this);
	gPoisonDOTMgr::Free(this);
	gBurningDOTMgr::Free(this);

	m_flNextSkillTimerUpdate = 0.0f;
	m_flNextClientCvarQuery = 0.0f;

	m_rgbHasEquipment.fill(false);	// clear it.

	// everything that comes after this, this spawn of the player a the game.
	if (m_bJustConnected)
		return;

	FireTargets("game_playerspawn", this, this, USE_TOGGLE, 0);
}

void EXT_FUNC CBasePlayer::Precache()
{
	// SOUNDS / MODELS ARE PRECACHED in ClientPrecache() (game specific)
	// because they need to precache before any clients have connected

	// init geiger counter vars during spawn and each time
	// we cross a level transition

	m_flgeigerRange = 1000;
	m_igeigerRangePrev = 1000;
	m_bitsDamageType = 0;
	m_bitsHUDDamage = -1;
	m_iClientBattery = -1;
	m_iTrain = TRAIN_NEW;

	// Make sure any necessary user messages have been registered
	LinkUserMessages();

	// won't update for 1/2 a second
	m_iUpdateTime = 5;

	if (gInitHUD)
		m_fInitHUD = true;
}

int CBasePlayer::Save(CSave &save)
{
	if (!CBaseMonster::Save(save))
		return 0;

	return save.WriteFields("PLAYER", this, m_playerSaveData, ARRAYSIZE(m_playerSaveData));
}

void CBasePlayer::SetScoreboardAttributes(CBasePlayer *destination)
{
	if (destination)
	{
		SetScoreAttrib(destination);
		return;
	}

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);

		if (pPlayer && !FNullEnt(pPlayer->edict()))
			SetScoreboardAttributes(pPlayer);
	}
}

int CBasePlayer::Restore(CRestore &restore)
{
	if (!CBaseMonster::Restore(restore))
		return 0;

	int status = restore.ReadFields("PLAYER", this, m_playerSaveData, ARRAYSIZE(m_playerSaveData));
	SAVERESTOREDATA *pSaveData = (SAVERESTOREDATA *)gpGlobals->pSaveData;

	// landmark isn't present.
	if (!pSaveData->fUseLandmark)
	{
		ALERT(at_console, "No Landmark:%s\n", pSaveData->szLandmarkName);

		// default to normal spawn
		edict_t *pentSpawnSpot = EntSelectSpawnPoint();

		pev->origin = pentSpawnSpot->v.origin + Vector(0, 0, 1);
		pev->angles = pentSpawnSpot->v.angles;
	}

	// Clear out roll
	pev->v_angle.z = 0;
	pev->angles = pev->v_angle;

	// turn this way immediately
	pev->fixangle = 1;

	// Copied from spawn() for now
	m_bloodColor = BLOOD_COLOR_RED;
	m_modelIndexPlayer = pev->modelindex;

	if (pev->flags & FL_DUCKING)
		UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);

	m_flDisplayHistory &= ~DHM_CONNECT_CLEAR;
	SetScoreboardAttributes();

	return status;
}

void CBasePlayer::Reset()
{
	pev->frags = 0;
	m_iDeaths = 0;

	MESSAGE_BEGIN(MSG_ALL, gmsgMoney);
		WRITE_BYTE(entindex());
		WRITE_SHORT(m_iAccount);
	MESSAGE_END();

	m_bNotKilled = false;

	// RemoveShield() included
	RemoveAllItems(TRUE);

	CheckStartMoney();
	AddAccount(startmoney.value, RT_PLAYER_RESET);

	MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
		WRITE_BYTE(ENTINDEX(edict()));
		WRITE_SHORT(0);
		WRITE_SHORT(0);
		WRITE_SHORT(0);
		WRITE_SHORT(m_iTeam);
	MESSAGE_END();

	m_flRespawnPending = 0.0f;
	m_flSpawnProtectionEndTime = 0.0f;
	m_vecOldvAngle = g_vecZero;
	m_lstRebuy.clear();
	m_iVotedTS = Scheme_UNASSIGNED;
}

void CBasePlayer::SelectItem(const char *pstr)
{
	if (!pstr)
	{
		return;
	}

	if (m_pActiveItem && !m_pActiveItem->CanHolster())
		return;

	auto pWeapon = GetItemById(WeaponClassnameToID(pstr));
	if (!pWeapon || pWeapon == m_pActiveItem)
		return;

	StartSwitchingWeapon(pWeapon);
}

void CBasePlayer::SelectLastItem()
{
	// this action can cancel grenade throw.
	if (m_pActiveItem && m_pActiveItem->m_bitsFlags & WPNSTATE_QUICK_THROWING)
	{
		m_pActiveItem->m_bitsFlags |= WPNSTATE_QT_EXIT;
		m_flNextAttack = 0;
		return;
	}

	if (m_pActiveItem && !m_pActiveItem->CanHolster())
		return;

	if (!m_pLastItem || m_pLastItem == m_pActiveItem)
	{
		for (int i = PRIMARY_WEAPON_SLOT; i < MAX_ITEM_TYPES; i++)
		{
			if (m_rgpPlayerItems[i] && m_rgpPlayerItems[i] != m_pActiveItem)
			{
				m_pLastItem = m_rgpPlayerItems[i];
				break;
			}
		}
	}

	if (!m_pLastItem || m_pLastItem == m_pActiveItem)
		return;

	auto temp = m_pActiveItem;	// save this, and we can set it to last weapon later on.
	StartSwitchingWeapon(m_pLastItem);
	m_pLastItem = temp;
}

// HasWeapons - do I have any weapons at all?
bool CBasePlayer::HasWeapons()
{
	for (auto item : m_rgpPlayerItems)
	{
		if (item)
			return true;
	}

	return false;
}

const char *CBasePlayer::TeamID()
{
	// Not fully connected yet
	if (!pev)
		return "";

	// return their team name
	return m_szTeamName;
}

void CBasePlayer::Touch(CBaseEntity* pOther)
{
	CBaseMonster::Touch(pOther);

	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnTouched(pOther);
	}
}

void CSprayCan::Spawn(entvars_t *pevOwner)
{
	pev->origin = pevOwner->origin + pevOwner->view_ofs;

	pev->angles = pevOwner->v_angle;
	pev->owner = ENT(pevOwner);
	pev->frame = 0;

	pev->nextthink = gpGlobals->time + 0.1f;
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/sprayer.wav", VOL_NORM, ATTN_NORM);
}

void CSprayCan::Think()
{
	CBasePlayer *pPlayer = GET_PRIVATE<CBasePlayer>(pev->owner);

	int nFrames = -1;
	if (pPlayer)
	{
		nFrames = pPlayer->GetCustomDecalFrames();
	}

	TraceResult tr;
	int playernum = ENTINDEX(pev->owner);

	UTIL_MakeVectors(pev->angles);
	UTIL_TraceLine(pev->origin, pev->origin + gpGlobals->v_forward * 128, ignore_monsters, pev->owner, &tr);

	// No customization present.
	if (nFrames == -1)
	{
		UTIL_DecalTrace(&tr, DECAL_LAMBDA6);
		UTIL_Remove(this);
	}
	else
	{
		UTIL_PlayerDecalTrace(&tr, playernum, pev->frame, TRUE);

		// Just painted last custom frame.
		if (pev->frame++ >= (nFrames - 1))
			UTIL_Remove(this);
	}

	pev->nextthink = gpGlobals->time + 0.1f;
}

void CBloodSplat::Spawn(entvars_t *pevOwner)
{
	pev->origin = pevOwner->origin + Vector(0, 0, 32);
	pev->angles = pevOwner->v_angle;
	pev->owner = ENT(pevOwner);

	SetThink(&CBloodSplat::Spray);
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CBloodSplat::Spray()
{
	TraceResult tr;
	if (g_Language != LANGUAGE_GERMAN)
	{
		UTIL_MakeVectors(pev->angles);
		UTIL_TraceLine(pev->origin, pev->origin + gpGlobals->v_forward * 128, ignore_monsters, pev->owner, &tr);
		UTIL_BloodDecalTrace(&tr, BLOOD_COLOR_RED);
	}

	SetThink(&CBloodSplat::SUB_Remove);
	pev->nextthink = gpGlobals->time + 0.1f;
}

// external function for 3rd-party
CBaseEntity *CBasePlayer::GiveNamedItem(const char *pszName)
{
	string_t istr = ALLOC_STRING(pszName);
	edict_t *pent = CREATE_NAMED_ENTITY(istr);

	if (FNullEnt(pent))
	{
		ALERT(at_console, "NULL Ent in GiveNamedItemEx classname `%s`!\n", pszName);
		return nullptr;
	}

	pent->v.origin = pev->origin;
	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn(pent);
	DispatchTouch(pent, ENT(pev));

	CBaseEntity *pEntity = GET_PRIVATE<CBaseEntity>(pent);

	// not allow the item to fall to the ground.
	if (FNullEnt(pent->v.owner) || pent->v.owner != edict())
	{
		pent->v.flags |= FL_KILLME;
		UTIL_Remove(pEntity);
		return nullptr;
	}

	return pEntity;
}

CBaseEntity *FindEntityForward(CBaseEntity *pEntity)
{
	TraceResult tr;
	Vector vecStart(pEntity->pev->origin + pEntity->pev->view_ofs);

	UTIL_MakeVectors(pEntity->pev->v_angle);
	UTIL_TraceLine(vecStart, vecStart + gpGlobals->v_forward * 8192, dont_ignore_monsters, pEntity->edict(), &tr);

	if (tr.flFraction != 1.0f && !FNullEnt(tr.pHit))
	{
		CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
		return pHit;
	}

	return nullptr;
}

BOOL CBasePlayer::FlashlightIsOn()
{
	return pev->effects & EF_DIMLIGHT;
}

void CBasePlayer::FlashlightTurnOn()
{
	if (!g_pGameRules->FAllowFlashlight())
		return;

	if (pev->weapons & (1 << WEAPON_SUIT))
	{
		EMIT_SOUND(ENT(pev), CHAN_ITEM, SOUND_FLASHLIGHT_ON, VOL_NORM, ATTN_NORM);

		pev->effects |= EF_DIMLIGHT;

		MESSAGE_BEGIN(MSG_ONE, gmsgFlashlight, nullptr, pev);
			WRITE_BYTE(1);
			WRITE_BYTE(m_iFlashBattery);
		MESSAGE_END();

		m_flFlashLightTime = gpGlobals->time + FLASH_DRAIN_TIME;
	}
}

void CBasePlayer::FlashlightTurnOff()
{
	EMIT_SOUND(ENT(pev), CHAN_ITEM, SOUND_FLASHLIGHT_OFF, VOL_NORM, ATTN_NORM);

	pev->effects &= ~EF_DIMLIGHT;
	MESSAGE_BEGIN(MSG_ONE, gmsgFlashlight, nullptr, pev);
		WRITE_BYTE(0);
		WRITE_BYTE(m_iFlashBattery);
	MESSAGE_END();

	m_flFlashLightTime = gpGlobals->time + FLASH_CHARGE_TIME;
}

// When recording a demo, we need to have the server tell us the entire client state so that the client side .dll can behave correctly.
// Reset stuff so that the state is transmitted.
void CBasePlayer::ForceClientDllUpdate()
{
	// fix for https://github.com/ValveSoftware/halflife/issues/1567
	m_iClientHideHUD = -1;
	m_iClientHealth = -1;
	m_iClientBattery = -1;

	m_bClientWeaponUpToDate = true;		// Force weapon send
	m_fInitHUD = true;		// Force HUD gmsgResetHUD message
	m_iTrain |= TRAIN_NEW;	// Force new train message.

	// Now force all the necessary messages to be sent.
	UpdateClientData();
	HandleSignals();
	QueryClientCvar();
}

void EXT_FUNC CBasePlayer::ImpulseCommands()
{
	TraceResult tr;

	// Handle use events
	PlayerUse();

	int iImpulse = pev->impulse;

	switch (iImpulse)
	{
		case 99:
		{
			int iOn;

			if (!gmsgLogo)
			{
				iOn = 1;
				gmsgLogo = REG_USER_MSG("Logo", 1);
			}
			else
				iOn = 0;

			assert(gmsgLogo > 0);

			MESSAGE_BEGIN(MSG_ONE, gmsgLogo, nullptr, pev);
				WRITE_BYTE(iOn);
			MESSAGE_END();

			if (!iOn)
				gmsgLogo = 0;

			break;
		}
		case 100:
		{
			// temporary flashlight for level designers
			if (FlashlightIsOn())
				FlashlightTurnOff();
			else
				FlashlightTurnOn();

			break;
		}
		case 201:
		{
			// paint decal
			if (gpGlobals->time < m_flNextDecalTime)
			{
				// too early!
				break;
			}

			UTIL_MakeVectors(pev->v_angle);
			UTIL_TraceLine(pev->origin + pev->view_ofs, pev->origin + pev->view_ofs + gpGlobals->v_forward * 128, ignore_monsters, edict(), &tr);

			if (tr.flFraction != 1.0f)
			{
				// line hit something, so paint a decal
				m_flNextDecalTime = gpGlobals->time + CVAR_GET_FLOAT("decalfrequency");
				CSprayCan *pCan = GetClassPtr((CSprayCan *)nullptr);
				pCan->Spawn(pev);
			}
			break;
		}
		default:
			// check all of the cheat impulse commands now
			CheatImpulseCommands(iImpulse);
	}

	pev->impulse = 0;
}

void CBasePlayer::CheatImpulseCommands(int iImpulse)
{
	if (!CVAR_GET_FLOAT("sv_cheats"))
		return;

	CBaseEntity *pEntity;
	TraceResult tr;

	switch (iImpulse)
	{
		case 101:
			AddAccount(int(maxmoney.value));
			ALERT(at_console, "Crediting %s with $%i\n", STRING(pev->netname), int(maxmoney.value));
			break;

		case 102:
			CGib::SpawnRandomGibs(pev, 1, 1);
			break;

		case 104:
			// Dump all of the global state varaibles (and global entity names)
			gGlobalState.DumpGlobals();
			break;

		case 106:
		{
			// Give me the classname and targetname of this entity.
			pEntity = FindEntityForward(this);

			if (pEntity)
			{
				ALERT(at_console, "Classname: %s", STRING(pEntity->pev->classname));

				if (!FStringNull(pEntity->pev->targetname))
					ALERT(at_console, " - Targetname: %s\n", STRING(pEntity->pev->targetname));
				else
					ALERT(at_console, " - TargetName: No Targetname\n");

				ALERT(at_console, "Model: %s\n", STRING(pEntity->pev->model));

				if (pEntity->pev->globalname)
					ALERT(at_console, "Globalname: %s\n", STRING(pEntity->pev->globalname));
			}
			break;
		}
		case 107:
		{
			TraceResult tr;
			edict_t *pWorld = INDEXENT(0);

			Vector start = pev->origin + pev->view_ofs;
			Vector end = start + gpGlobals->v_forward * 1024;
			UTIL_TraceLine(start, end, ignore_monsters, edict(), &tr);

			if (tr.pHit)
				pWorld = tr.pHit;

			const char *pszTextureName = TRACE_TEXTURE(pWorld, start, end);

			if (pszTextureName)
				ALERT(at_console, "Texture: %s\n", pszTextureName);

			break;
		}
		case 202:
		{
			// Random blood splatter
			UTIL_MakeVectors(pev->v_angle);
			UTIL_TraceLine(pev->origin + pev->view_ofs, pev->origin + pev->view_ofs + gpGlobals->v_forward * 128, ignore_monsters, edict(), &tr);

			if (tr.flFraction != 1.0f)
			{
				// line hit something, so paint a decal
				CBloodSplat *pBlood = GetClassPtr((CBloodSplat *)nullptr);
				pBlood->Spawn(pev);
			}
			break;
		}
		case 203:
		{
			// remove creature.
			pEntity = FindEntityForward(this);

			if (pEntity && pEntity->pev->takedamage != DAMAGE_NO)
			{
				pEntity->SetThink(&CBaseEntity::SUB_Remove);
			}
			break;
		}
		case 204:
		{
			TraceResult tr;
			Vector dir(0, 0, 1);

			UTIL_BloodDrips(pev->origin, dir, BLOOD_COLOR_RED, 2000);

			for (int r = 1; r < 4; r++)
			{
				float bloodRange = r * 50.0f;
				for (int i = 0; i < 50; i++)
				{
					dir.x = RANDOM_FLOAT(-1, 1);
					dir.y = RANDOM_FLOAT(-1, 1);
					dir.z = RANDOM_FLOAT(-1, 1);

					if (dir.x || dir.y || dir.z)
						dir.NormalizeInPlace();
					else
						dir.z = -1.0f;

					UTIL_TraceLine(EyePosition(), EyePosition() + dir * bloodRange, ignore_monsters, pev->pContainingEntity, &tr);

					if (tr.flFraction < 1.0f)
						UTIL_BloodDecalTrace(&tr, BLOOD_COLOR_RED);
				}
			}
			break;
		}
		case 255:
		{
			// Give weapons
			for (int wpnid = WEAPON_NONE + 1; wpnid < LAST_WEAPON; wpnid++)
			{
				// If by some case the weapon got invalid
				const auto pInfo = GetWeaponInfo(wpnid);
				if (pInfo) {
					GiveNamedItem(pInfo->m_pszInternalName);
					GiveAmmo(255, (AmmoIdType)pInfo->m_iAmmoType);
				}
			}

			GiveNamedItem("item_longjump");
			GiveNamedItem("item_thighpack");
			GiveNamedItem("item_kevlar");
			break;
		}
	}
}

void OLD_CheckBuyZone(CBasePlayer *pPlayer)
{
	const char *pszSpawnClass = nullptr;

	if (!CSGameRules()->CanPlayerBuy(pPlayer))
	{
		return;
	}

	if (pPlayer->m_iTeam == TERRORIST)
	{
		pszSpawnClass = "info_player_deathmatch";
	}
	else if (pPlayer->m_iTeam == CT)
	{
		pszSpawnClass = "info_player_start";
	}

	if (pszSpawnClass)
	{
		CBaseEntity *pSpot = nullptr;
		while ((pSpot = UTIL_FindEntityByClassname(pSpot, pszSpawnClass)))
		{
			if ((pSpot->pev->origin - pPlayer->pev->origin) < 200)
			{
				pPlayer->m_signals.Signal(SIGNAL_BUY);
				break;
			}
		}
	}
}

void CBasePlayer::HandleSignals()
{
	if (!CSGameRules()->m_bMapHasBuyZone)
		OLD_CheckBuyZone(this);

	// Doctrine_MobileWarfare allows buying everywhere.
	if (CSGameRules()->m_rgTeamTacticalScheme[m_iTeam] == Doctrine_MobileWarfare)
		m_signals.Signal(SIGNAL_BUY);

	int state = m_signals.GetSignal();
	int changed = m_signals.GetState() ^ state;

	// Push signals from this frame to stock.
	m_signals.Update();

	if (changed & SIGNAL_BUY)
	{
		if (state & SIGNAL_BUY)
			BuyZoneIcon_Set(this);
		else
			BuyZoneIcon_Clear(this);
	}
}

// Add a weapon to the player (Item == Weapon == Selectable Object)
BOOL EXT_FUNC CBasePlayer::AddPlayerItem(CBaseWeapon *pItem)
{
	if (!pItem)
		return FALSE;

	if (pItem->AddToPlayer(this))
	{
		CSGameRules()->PlayerGotWeapon(this, pItem);

		if (pItem->m_pItemInfo->m_iSlot == PRIMARY_WEAPON_SLOT)
			m_bHasPrimary = true;

		// get it into player's inventory.
		m_rgpPlayerItems[pItem->m_pItemInfo->m_iSlot] = pItem;
		pev->weapons |= (1 << pItem->m_iId);	// TODO: abolish this?

		// FX
		EMIT_SOUND(edict(), CHAN_WEAPON, "items/gunpickup2.wav", VOL_NORM, ATTN_NORM);
		MESSAGE_BEGIN(MSG_ONE, gmsgWeapPickup, nullptr, pev);
		WRITE_BYTE(pItem->m_iId);
		MESSAGE_END();

		// Slot info for HUD.
		MESSAGE_BEGIN(MSG_ONE, gmsgSetSlot, nullptr, pev);
		WRITE_BYTE(pItem->m_iId);
		WRITE_BYTE(pItem->m_pItemInfo->m_iSlot);
		MESSAGE_END();

		if (HasShield())
			pev->gamestate = HITGROUP_SHIELD_ENABLED;

		// should we switch to this item?
		if (CSGameRules()->FShouldSwitchWeapon(this, pItem))
		{
			if (!m_bShieldDrawn)
			{
				StartSwitchingWeapon(pItem);
			}
		}

		m_iHideHUD &= ~HIDEHUD_WEAPONS;
		return TRUE;
	}

	return FALSE;
}

BOOL EXT_FUNC CBasePlayer::RemovePlayerItem(CBaseWeapon *pItem)	// this should be called from CBaseWeapon::Kill() !!!
{
	if (m_pActiveItem == pItem)
	{
		ResetAutoaim();
		ResetMaxSpeed();

		// ReGameDLL Fixes: Version 5.16.0.465
		pev->fov = m_iLastZoom = DEFAULT_FOV;
		m_bResumeZoom = false;
		m_pActiveItem = nullptr;

		pev->viewmodel = 0;
		pev->weaponmodel = 0;
	}

	// if item being removed is the last weapon, we should clear the record.
	// because the SelectLastItem() won't check the ownership of last item.
	if (m_pLastItem == pItem)
		m_pLastItem = nullptr;

	// remove from client display.
	pev->weapons &= ~(1 << pItem->m_iId);
	MESSAGE_BEGIN(MSG_ONE, gmsgSetSlot, nullptr, pev);
	WRITE_BYTE(0);
	WRITE_BYTE(pItem->m_pItemInfo->m_iSlot);
	MESSAGE_END();

	// remove from server inventory.
	if (m_rgpPlayerItems[pItem->m_pItemInfo->m_iSlot] == pItem)
	{
		m_rgpPlayerItems[pItem->m_pItemInfo->m_iSlot] = nullptr;	// however, even if you don't call from CBaseWeapon::Kill(), WeaponsThink() would kill the weapon due to this nullptr assignment.
		return TRUE;
	}

	return FALSE;
}

// Returns the unique ID for the ammo, or -1 if error
bool CBasePlayer::GiveAmmo(int iAmount, AmmoIdType iId)
{
	if (iAmount <= 0)
		return false;	// never add air to your inventory.

	if (pev->flags & FL_SPECTATOR)
		return false;

	if (iId <= AMMO_NONE || iId >= AMMO_MAXTYPE)
	{
		// no ammo.
		return false;
	}

	if (!CSGameRules()->CanHaveAmmo(this, iId))	// game rules say I can't have any more of this ammo type.
		return false;

	int iAdd = Q_min(iAmount, GetAmmoInfo(iId)->m_iMax - m_rgAmmo[iId]);
	if (iAdd < 1)
		return false;

	m_rgAmmo[iId] += iAdd;

	// make sure the ammo messages have been linked first
	if (gmsgAmmoPickup)
	{
		// Send the message that ammo has been picked up
		MESSAGE_BEGIN(MSG_ONE, gmsgAmmoPickup, nullptr, pev);
			WRITE_BYTE(iId); // ammo ID
			WRITE_BYTE(iAdd); // amount
		MESSAGE_END();
	}

	return true;
}

// Called every frame by the player PreThink
void CBasePlayer::ItemPreFrame()
{
	if (!m_pActiveItem)
		return;

	m_pActiveItem->Think();	// think would be call nomatter what.
}

// Called every frame by the player PostThink
void CBasePlayer::ItemPostFrame()
{
	static int fInSelect = FALSE;

	// check if the player is using a tank
	if (m_pTank)
		return;

	if (m_pActiveItem)
	{
		if (HasShield() && IsReloading())
		{
			if (pev->button & IN_ATTACK2)
				m_flNextAttack = 0;
		}
	}

#ifdef CLIENT_WEAPONS
	if (m_flNextAttack > 0)
#else
	if (gpGlobals->time < m_flNextAttack)
#endif
		return;

	ImpulseCommands();

	if (m_pActiveItem)
		m_pActiveItem->PostFrame();
}

void CBasePlayer::SendAmmoUpdate()
{
	for (int i = 0; i < MAX_AMMO_SLOTS; i++)
	{
		if (m_rgAmmo[i] != m_rgAmmoLast[i])
		{
			m_rgAmmoLast[i] = m_rgAmmo[i];

			assert(m_rgAmmo[i] >= 0);
			assert(m_rgAmmo[i] <= 255);

			// send "Ammo" update message
			MESSAGE_BEGIN(MSG_ONE, gmsgAmmoX, nullptr, pev);
				WRITE_BYTE(i);
				WRITE_BYTE(Q_clamp(m_rgAmmo[i], 0, 255)); // clamp the value to one byte
			MESSAGE_END();
		}
	}
}

void CBasePlayer::SendWeatherInfo()
{
	auto SendReceiveW = [&](BYTE byte)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgReceiveW, nullptr, pev);
				WRITE_BYTE(byte);
		MESSAGE_END();
	};

	/* Rain */
	if (UTIL_FindEntityByClassname(nullptr, "env_rain"))
		return SendReceiveW(1);

	if (UTIL_FindEntityByClassname(nullptr, "func_rain"))
		return SendReceiveW(1);

	/* Snow */
	if (UTIL_FindEntityByClassname(nullptr, "env_snow"))
		return SendReceiveW(2);

	if (UTIL_FindEntityByClassname(nullptr, "func_snow"))
		return SendReceiveW(2);
}

// resends any changed player HUD info to the client.
// Called every frame by PlayerPreThink
// Also called at start of demo recording and playback by
// ForceClientDllUpdate to ensure the demo gets messages
// reflecting all of the HUD state info.
void EXT_FUNC CBasePlayer::UpdateClientData()
{
	if (m_fInitHUD)
	{
		m_fInitHUD = false;
		gInitHUD = false;
		m_signals.Reset();

		MESSAGE_BEGIN(MSG_ONE, gmsgResetHUD, nullptr, pev);
		MESSAGE_END();

		if (!m_fGameHUDInitialized)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgInitHUD, nullptr, pev);
			MESSAGE_END();

			CBaseEntity *pEntity = UTIL_FindEntityByClassname(nullptr, "env_fog");
			if (pEntity)
			{
				CClientFog *pFog = static_cast<CClientFog *>(pEntity);

				int r = clamp(int(pFog->pev->rendercolor[0]), 0, 255);
				int g = clamp(int(pFog->pev->rendercolor[1]), 0, 255);
				int b = clamp(int(pFog->pev->rendercolor[2]), 0, 255);

				union
				{
					float f;
					char b[4];

				} density;

				density.f = pFog->m_fDensity;

				MESSAGE_BEGIN(MSG_ONE, gmsgFog, nullptr, pev);
					WRITE_BYTE(r);
					WRITE_BYTE(g);
					WRITE_BYTE(b);
					WRITE_BYTE(density.b[0]);
					WRITE_BYTE(density.b[1]);
					WRITE_BYTE(density.b[2]);
					WRITE_BYTE(density.b[3]);
				MESSAGE_END();
			}

			g_pGameRules->InitHUD(this);
			m_fGameHUDInitialized = true;

			if (g_pGameRules->IsMultiplayer())
			{
				FireTargets("game_playerjoin", this, this, USE_TOGGLE, 0);
			}

			m_iObserverLastMode = OBS_ROAMING;
			SetObserverAutoDirector(false);
		}

		MESSAGE_BEGIN(MSG_ONE, gmsgMoney, nullptr, pev);
			WRITE_BYTE(entindex());
			WRITE_SHORT(m_iAccount);
		MESSAGE_END();

		SyncRoundTimer();
		SendWeatherInfo();

		if (g_pGameRules->IsMultiplayer())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgTeamScore, nullptr, pev);
				WRITE_BYTE(CT);
				WRITE_SHORT(CSGameRules()->m_iNumCTWins);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ONE, gmsgTeamScore, nullptr, pev);
				WRITE_BYTE(TERRORIST);
				WRITE_SHORT(CSGameRules()->m_iNumTerroristWins);
			MESSAGE_END();
		}

		// send "flashlight" update message
		if (FlashlightIsOn())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgFlashlight, nullptr, pev);
				WRITE_BYTE(1);
				WRITE_BYTE(m_iFlashBattery);
			MESSAGE_END();
		}
	}

	if (m_iHideHUD != m_iClientHideHUD)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgHideWeapon, nullptr, pev);
			WRITE_BYTE(m_iHideHUD);
		MESSAGE_END();

		if (m_iHideHUD && !(m_iHideHUD & HIDEHUD_OBSERVER_CROSSHAIR))
		{
			if (m_iClientHideHUD < 0)
				m_iClientHideHUD = 0;

			int hudChanged = m_iClientHideHUD ^ m_iHideHUD;
			if (hudChanged & (HIDEHUD_FLASHLIGHT | HIDEHUD_HEALTH | HIDEHUD_TIMER | HIDEHUD_MONEY))
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgCrosshair, nullptr, pev);
					WRITE_BYTE(0);
				MESSAGE_END();
			}
		}

		m_iClientHideHUD = m_iHideHUD;
	}

	if (int(pev->fov) != m_iClientFOV)
	{
		// cache FOV change at end of function, so weapon updates can see that FOV has changed

		MESSAGE_BEGIN(MSG_ONE, gmsgSetFOV, nullptr, pev);
			WRITE_BYTE(int(pev->fov));
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_SPEC, gmsgHLTV);
			WRITE_BYTE(ENTINDEX(edict()));
			WRITE_BYTE(int(pev->fov));
		MESSAGE_END();
	}

	// HACKHACK -- send the message to display the game title
	if (gDisplayTitle)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgShowGameTitle, nullptr, pev);
			WRITE_BYTE(0);
		MESSAGE_END();

		gDisplayTitle = FALSE;
	}

	if (int(pev->health) != m_iClientHealth)
	{
		int iHealth = int(pev->health);
		if (pev->health > 0.0f && pev->health <= 1.0f)	// prevent display bug.
			iHealth = 1;

		// send "health" update message
		MESSAGE_BEGIN(MSG_ALL, gmsgHealth);
			WRITE_BYTE(entindex());
			WRITE_SHORT(iHealth);
		MESSAGE_END();

		m_iClientHealth = int(pev->health);
	}

	if (int(pev->armorvalue) != m_iClientBattery)
	{
		m_iClientBattery = int(pev->armorvalue);

		assert(gmsgBattery > 0);

		// send "armor" update message
		MESSAGE_BEGIN(MSG_ONE, gmsgBattery, nullptr, pev);
			WRITE_SHORT(int(pev->armorvalue));
		MESSAGE_END();
	}

	if (pev->dmg_take != 0.0f || pev->dmg_save != 0.0f || m_bitsHUDDamage != m_bitsDamageType)
	{
		// Comes from inside me if not set
		Vector damageOrigin = pev->origin;

		// send "damage" message
		// causes screen to flash, and pain compass to show direction of damage
		edict_t *other = pev->dmg_inflictor;

		if (other)
		{
			CBaseEntity *pEntity = CBaseEntity::Instance(other);

			if (pEntity)
			{
				damageOrigin = pEntity->Center();
			}
		}

		// only send down damage type that have hud art
		int visibleDamageBits = m_bitsDamageType & DMG_SHOWNHUD;

		MESSAGE_BEGIN(MSG_ONE, gmsgDamage, nullptr, pev);
			WRITE_BYTE(int(pev->dmg_save));
			WRITE_BYTE(int(pev->dmg_take));
			WRITE_LONG(visibleDamageBits);
			WRITE_COORD(damageOrigin.x);
			WRITE_COORD(damageOrigin.y);
			WRITE_COORD(damageOrigin.z);
		MESSAGE_END();

		pev->dmg_take = 0;
		pev->dmg_save = 0;
		m_bitsHUDDamage = m_bitsDamageType;

		// Clear off non-time-based damage indicators
		m_bitsDamageType &= DMG_TIMEBASED;
	}

	// Update Flashlight
	if (m_flFlashLightTime && m_flFlashLightTime <= gpGlobals->time)
	{
		if (FlashlightIsOn())
		{
			if (m_iFlashBattery)
			{
				m_flFlashLightTime = gpGlobals->time + FLASH_DRAIN_TIME;

				if (--m_iFlashBattery <= 0)
				{
					FlashlightTurnOff();
				}
			}
		}
		else
		{
			if (m_iFlashBattery < 100)
			{
				m_flFlashLightTime = gpGlobals->time + FLASH_CHARGE_TIME;
				m_iFlashBattery++;
			}
			else
				m_flFlashLightTime = 0;
		}

		MESSAGE_BEGIN(MSG_ONE, gmsgFlashBattery, nullptr, pev);
			WRITE_BYTE(m_iFlashBattery);
		MESSAGE_END();
	}

	if (m_iTrain & TRAIN_NEW)
	{
		assert(gmsgTrain > 0);

		// send "train hud" update message
		MESSAGE_BEGIN(MSG_ONE, gmsgTrain, nullptr, pev);
			WRITE_BYTE(m_iTrain & 0xF);
		MESSAGE_END();

		m_iTrain &= ~TRAIN_NEW;
	}

	SendAmmoUpdate();

	// Update all the items
	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		if (m_rgpPlayerItems[i])
		{
			// each item updates it's successors
			m_rgpPlayerItems[i]->UpdateClientData();
		}
	}

	// Cache and client weapon change
	m_pClientActiveItem = m_pActiveItem;
	m_iClientFOV = int(pev->fov);

	// Update Status Bar
	if (m_flNextSBarUpdateTime < gpGlobals->time)
	{
		UpdateStatusBar();
		m_flNextSBarUpdateTime = gpGlobals->time + 0.2f;
	}

	if (!(m_flDisplayHistory & DHF_AMMO_EXHAUSTED))
	{
		if (m_pActiveItem && !(m_pActiveItem->m_pItemInfo->m_bitsFlags & ITEM_FLAG_EXHAUSTIBLE))
		{
			if (m_rgAmmo[m_pActiveItem->m_iPrimaryAmmoType] < 1 && m_pActiveItem->m_iClip == 0)
			{
				m_flDisplayHistory |= DHF_AMMO_EXHAUSTED;
				HintMessage("#Hint_out_of_ammo");
			}
		}
	}

	if (gpGlobals->time > m_tmHandleSignals)
	{
		m_tmHandleSignals = gpGlobals->time + 0.5f;
		HandleSignals();
	}

	if (pev->deadflag == DEAD_NO && gpGlobals->time > m_tmNextRadarUpdate)
	{
		Vector vecOrigin = pev->origin;
		m_tmNextRadarUpdate = gpGlobals->time + 1.0f;

		if (CSGameRules()->IsFreeForAll())
			vecOrigin = g_vecZero;

		const float flToleranceDist = 64.0f;
		if ((pev->origin - m_vLastOrigin).Length() >= flToleranceDist)
		{
			for (int i = 1; i <= gpGlobals->maxClients; i++)
			{
				CBaseEntity *pEntity = UTIL_PlayerByIndex(i);

				if (!pEntity || i == entindex())
					continue;

				CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pEntity->pev);

				if (pPlayer->pev->flags == FL_DORMANT)
					continue;

				if (pPlayer->pev->deadflag != DEAD_NO)
					continue;

				if (pPlayer->m_iTeam == m_iTeam)
				{
					MESSAGE_BEGIN(MSG_ONE, gmsgRadar, nullptr, pPlayer->pev);
						WRITE_BYTE(entindex());
						WRITE_COORD(vecOrigin.x);
						WRITE_COORD(vecOrigin.y);
						WRITE_COORD(vecOrigin.z);
					MESSAGE_END();
				}
			}
		}

		m_vLastOrigin = pev->origin;
	}

	if (m_iClientKnownUsingGrenadeId != m_iUsingGrenadeId)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgEqpSelect, nullptr, pev);
		WRITE_BYTE(m_iUsingGrenadeId);
		MESSAGE_END();

		m_iClientKnownUsingGrenadeId = m_iUsingGrenadeId;
	}

	if (m_flNextSkillTimerUpdate <= gpGlobals->time)
	{
		m_flNextSkillTimerUpdate = gpGlobals->time + 1.0f;

		CBaseSkill* pSkill = nullptr;

		switch (m_iRoleType)
		{
		case Role_Arsonist:		// CSkillIncendiaryAmmo
		case Role_MadScientist:	// CSkillTaserGun
		case Role_Sharpshooter:	// CSkillEnfoceHeadshot
			pSkill = m_rgpSkills[SkillType_WeaponEnhance];
			break;

		case Role_Assassin:	// CSkillInvisible
		case Role_SWAT:		// CSkillBulletproof
			pSkill = m_rgpSkills[SkillType_Defense];
			break;

		case Role_Breacher:		// CSkillInfiniteGrenade
		case Role_LeadEnforcer:	// CSkillResistDeath
			pSkill = m_rgpSkills[SkillType_Attack];
			break;

		case Role_Commander:	// CSkillRadarScan
		case Role_Godfather:	// CSkillGavelkind
		case Role_Medic:		// CSkillHealingShot
			pSkill = m_rgpSkills[SkillType_Auxiliary];
			break;

		default:
			break;
		}

		int iTotalTime = 1;
		int iMode = 0;
		int iTimerSent = 0;

		if (pSkill)
		{
			iTotalTime = pSkill->IsCoolingDown() ? pSkill->GetCooldown() : pSkill->GetDuration();
			iTimerSent = pSkill->GetCountingTime() * 10000.0f;

			if (pSkill->IsCoolingDown())
				iMode = 1;	// increase.
			else if (pSkill->m_bUsingSkill)
				iMode = -1;	// decrease.
		}

		MESSAGE_BEGIN(MSG_ONE, gmsgSkillTimer, nullptr, pev);
		WRITE_BYTE(iTotalTime);
		WRITE_BYTE(iMode + 1);	// the BYTE type contains only 0~255.
		WRITE_LONG(iTimerSent);
		MESSAGE_END();
	}

	for (byte i = 0; i < EQP_COUNT; i++)
	{
		if (m_rgbHasEquipment[i] != m_rgbClientHasEquipment[i])
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgEquipment, nullptr, pev);
			WRITE_BYTE(i);
			WRITE_BYTE(m_rgbHasEquipment[i]);
			MESSAGE_END();

			m_rgbClientHasEquipment[i] = m_rgbHasEquipment[i];
		}
	}
}

bool CBasePlayer::ShouldToShowAccount(CBasePlayer *pReceiver) const
{
	int iShowAccount = static_cast<int>(scoreboard_showmoney.value);

	if (iShowAccount == 0)
		return false; // don't send any update for this field to any clients

	// show only Terrorist or CT 'Money' field to all clients
	if (m_iTeam == iShowAccount)
		return true;

	switch (iShowAccount)
	{
	// show field to teammates
	case 3: return !CSGameRules()->IsFreeForAll() && pReceiver->m_iTeam == m_iTeam;

	// show field to all clients
	case 4: return true;

	// show field to teammates and spectators
	case 5: return ((!CSGameRules()->IsFreeForAll() && pReceiver->m_iTeam == m_iTeam) || pReceiver->m_iTeam == SPECTATOR);
	default:
		break;
	}

	return false;
}

bool CBasePlayer::ShouldToShowHealthInfo(CBasePlayer *pReceiver) const
{
	int iShowHealth = static_cast<int>(scoreboard_showhealth.value);

	if (iShowHealth == 0)
		return false; // don't send any update for this field to any clients

	// show only Terrorist or CT 'HP' fields to all clients
	if (m_iTeam == iShowHealth)
		return true;

	switch (iShowHealth)
	{
	// show field to teammates
	case 3: return !CSGameRules()->IsFreeForAll() && pReceiver->m_iTeam == m_iTeam;

	// show field to all clients
	case 4: return true;

	// show field to teammates and spectators
	case 5: return ((!CSGameRules()->IsFreeForAll() && pReceiver->m_iTeam == m_iTeam) || pReceiver->m_iTeam == SPECTATOR);
	default:
		break;
	}

	return false;
}

BOOL CBasePlayer::FBecomeProne()
{
	m_afPhysicsFlags |= PFLAG_ONBARNACLE;
	return TRUE;
}

// return player light level plus virtual muzzle flash
int CBasePlayer::Illumination()
{
	int iIllum = CBaseEntity::Illumination();

	iIllum += m_iWeaponFlash;

	if (iIllum > 255)
		return 255;

	return iIllum;
}

void CBasePlayer::EnableControl(BOOL fControl)
{
	if (!fControl)
		pev->flags |= FL_FROZEN;
	else
		pev->flags &= ~FL_FROZEN;
}

void EXT_FUNC CBasePlayer::ResetMaxSpeed()
{
	// default speed.
	float speed = 240;

	if (GetObserverMode() != OBS_NONE)
	{
		// Player gets speed bonus in observer mode
		speed = 900;
	}
	else if (g_pGameRules->IsMultiplayer() && g_pGameRules->IsFreezePeriod())
	{
		// Player should not move during the freeze period
		speed = 1;
	}
	else if (m_pActiveItem)
	{
		// Get player speed from selected weapon
		speed = m_pActiveItem->GetMaxSpeed();
	}

	// hook it right before reset it.
	OnResetPlayerMaxspeed(speed);

	// since the addition of dashing mechanism, players are limited in around 35% of original speed.
	// however, BOTs are not. therefore, the speed set for players have to increase.
	// the constant 40 is the threshold of sniper scoping. below that value, client would consider this player is using his full speed no matter what.
	if (!IsBot() && int(pev->fov) > 40)
		speed *= 1.35f;

	SET_CLIENT_MAXSPEED(edict(), speed);
	pev->maxspeed = speed;
}

bool EXT_FUNC CBasePlayer::HintMessageEx(const char *pMessage, float duration, bool bDisplayIfPlayerDead, bool bOverride)
{
	if (!bDisplayIfPlayerDead && !IsAlive())
		return false;

	if (bOverride || m_bShowHints)
		return m_hintMessageQueue.AddMessage(pMessage, duration, true, nullptr);

	return true;
}

bool EXT_FUNC CBasePlayer::HintMessage(const char *pMessage, BOOL bDisplayIfPlayerDead, BOOL bOverride)
{
	return HintMessageEx(pMessage, 6.0f, bDisplayIfPlayerDead == TRUE, bOverride == TRUE);
}

Vector CBasePlayer::GetAutoaimVector(float flDelta)
{
	Vector vecSrc;
	BOOL m_fOldTargeting;
	Vector angles;

	vecSrc = GetGunPosition();
	m_fOldTargeting = m_fOnTarget;
	m_vecAutoAim = Vector(0, 0, 0);
	angles = AutoaimDeflection(vecSrc, 8192, flDelta);

	if (g_pGameRules->AllowAutoTargetCrosshair())
	{
	}
	else
		m_fOnTarget = FALSE;

	if (angles.x > 180.0f)
		angles.x -= 360.0f;
	if (angles.x < -180.0f)
		angles.x += 360.0f;

	if (angles.y > 180.0f)
		angles.y -= 360.0f;
	if (angles.y < -180.0f)
		angles.y += 360.0f;

	if (angles.x > 25.0f)
		angles.x = 25.0f;
	if (angles.x < -25.0f)
		angles.x = -25.0f;

	if (angles.y > 12.0f)
		angles.y = 12.0f;
	if (angles.y < -12.0f)
		angles.y = -12.0f;

	m_vecAutoAim = angles * 0.9f;

	if (g_psv_aim && g_psv_aim->value > 0.0f)
	{
		if (m_vecAutoAim.x != m_lastx || m_vecAutoAim.y != m_lasty)
		{
			SET_CROSSHAIRANGLE(ENT(pev), -m_vecAutoAim.x, m_vecAutoAim.y);

			m_lastx = m_vecAutoAim.x;
			m_lasty = m_vecAutoAim.y;
		}
	}

	UTIL_MakeVectors(pev->v_angle + pev->punchangle + m_vecAutoAim);
	return gpGlobals->v_forward;
}

Vector CBasePlayer::AutoaimDeflection(Vector &vecSrc, float flDist, float flDelta)
{
	m_fOnTarget = FALSE;
	return g_vecZero;
}

void CBasePlayer::ResetAutoaim()
{
	if (m_vecAutoAim.x != 0.0f || m_vecAutoAim.y != 0.0f)
	{
		m_vecAutoAim = Vector(0, 0, 0);
		SET_CROSSHAIRANGLE(ENT(pev), 0, 0);
	}
	m_fOnTarget = FALSE;
}

// UNDONE: Determine real frame limit, 8 is a placeholder.
// Note: -1 means no custom frames present.
void CBasePlayer::SetCustomDecalFrames(int nFrames)
{
	if (nFrames > 0 && nFrames < 8)
		m_nCustomSprayFrames = nFrames;
	else
		m_nCustomSprayFrames = -1;
}

// Returns the # of custom frames this player's custom clan logo contains.
int CBasePlayer::GetCustomDecalFrames()
{
	return m_nCustomSprayFrames;
}

void EXT_FUNC CBasePlayer::Blind(float duration, float holdTime, float fadeTime, int alpha)
{
	m_blindUntilTime = gpGlobals->time + duration;
	m_blindStartTime = gpGlobals->time;

	m_blindHoldTime = holdTime;
	m_blindFadeTime = fadeTime;
	m_blindAlpha = alpha;
}

void CBasePlayer::UpdateOnRemove()
{
	CBaseMonster::UpdateOnRemove();

	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			delete m_rgpSkills[i];
	}
}

void CBasePlayer::InitStatusBar()
{
	m_flStatusBarDisappearDelay = 0.0f;
	m_SbarString0[0] = '\0';
}

void CBasePlayer::UpdateStatusBar()
{
	int newSBarState[SBAR_END];
	char sbuf0[MAX_SBAR_STRING];

	Q_memset(newSBarState, 0, sizeof(newSBarState));
	Q_strlcpy(sbuf0, " ");

	// Find an ID Target
	TraceResult tr;
	UTIL_MakeVectors(pev->v_angle + pev->punchangle);

	Vector vecSrc = EyePosition();
	Vector vecEnd = vecSrc + (gpGlobals->v_forward * ((pev->flags & FL_SPECTATOR) != 0 ? MAX_SPEC_ID_RANGE : MAX_ID_RANGE));

	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, edict(), &tr);

	if (tr.flFraction != 1.0f)
	{
		if (!FNullEnt(tr.pHit))
		{
			CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
			bool isVisiblePlayer = ((TheBots == nullptr || !TheBots->IsLineBlockedBySmoke(&pev->origin, &pEntity->pev->origin)) && pEntity->IsPlayer());

			if (gpGlobals->time >= m_blindUntilTime && isVisiblePlayer)
			{
				CBasePlayer *pTarget = CBasePlayer::Instance(pEntity->pev);
				bool sameTeam = !CSGameRules()->IsFreeForAll() && pTarget->m_iTeam == m_iTeam;

				newSBarState[SBAR_ID_TARGETNAME] = ENTINDEX(pTarget->edict());
				newSBarState[SBAR_ID_TARGETTYPE] = sameTeam ? SBAR_TARGETTYPE_TEAMMATE : SBAR_TARGETTYPE_ENEMY;

				if (m_iRoleType == Role_Sharpshooter
					|| (m_iRoleType == Role_Assassin && IsUsingPrimarySkill())	// only sniper and sneaking assassin have the detail.
					|| sameTeam		// or you are in the same team.
					|| (pTarget->m_iRoleType == Role_Assassin && pTarget->IsUsingPrimarySkill())	// watch out! you are aiming at a sneaking assassin!
					)
				{
					Q_snprintf(sbuf0, sizeof(sbuf0) - 1, "%s: %%p2 | HP: %d%%%%", g_rgszRoleNames[pTarget->m_iRoleType], int(pTarget->pev->health));
				}

				// hint message.
				if (sameTeam || GetObserverMode() != OBS_NONE)
				{
					if (!(m_flDisplayHistory & DHF_FRIEND_SEEN) && !(pev->flags & FL_SPECTATOR))
					{
						m_flDisplayHistory |= DHF_FRIEND_SEEN;
						HintMessage("#Hint_spotted_a_friend");
					}
				}
				else if (GetObserverMode() == OBS_NONE)
				{
					if (!(m_flDisplayHistory & DHF_ENEMY_SEEN))
					{
						m_flDisplayHistory |= DHF_ENEMY_SEEN;
						HintMessage("#Hint_spotted_an_enemy");
					}
				}

				m_flStatusBarDisappearDelay = gpGlobals->time + 2.0f;
			}
		}
	}
	else if (m_flStatusBarDisappearDelay > gpGlobals->time)
	{
		// hold the values for a short amount of time after viewing the object
		newSBarState[SBAR_ID_TARGETTYPE] = m_izSBarState[SBAR_ID_TARGETTYPE];
		newSBarState[SBAR_ID_TARGETNAME] = m_izSBarState[SBAR_ID_TARGETNAME];
		newSBarState[SBAR_ID_TARGETHEALTH] = m_izSBarState[SBAR_ID_TARGETHEALTH];
	}

	bool bForceResend = false;

	if (Q_strcmp(sbuf0, m_SbarString0))
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgStatusText, nullptr, pev);
			WRITE_BYTE(0);
			WRITE_STRING(sbuf0);
		MESSAGE_END();

		Q_strlcpy(m_SbarString0, sbuf0);

		// make sure everything's resent
		bForceResend = true;
	}

	// Check values and send if they don't match
	for (int i = 1; i < SBAR_END; i++)
	{
		if (newSBarState[i] != m_izSBarState[i] || bForceResend)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgStatusValue, nullptr, pev);
				WRITE_BYTE(i);
				WRITE_SHORT(newSBarState[i]);
			MESSAGE_END();

			m_izSBarState[i] = newSBarState[i];
		}
	}
}

// DropPlayerItem - drop the named item, or if no name, the active item.
CBaseEntity *EXT_FUNC CBasePlayer::DropPlayerItem(WeaponIdType iId)
{
	if (iId <= WEAPON_NONE || iId >= LAST_WEAPON)
	{
		// if this string has no length, the client didn't type a name!
		// assume player wants to drop the active item.
		// make the string null to make future operations in this function easier
		iId = WEAPON_NONE;
	}

	if (!iId && HasShield())
	{
		DropShield();
		return nullptr;
	}

	auto pWeapon = iId ? GetItemById(iId) : m_pActiveItem;

	if (pWeapon)
	{
		if (!pWeapon->CanDrop() && IsAlive())
		{
			ClientPrint(pev, HUD_PRINTCENTER, "#Weapon_Cannot_Be_Dropped");
			return false;
		}

		// No more weapon
		if ((pev->weapons & ~(1 << WEAPON_SUIT)) == 0)
			m_iHideHUD |= HIDEHUD_WEAPONS;

		g_pGameRules->GetNextBestWeapon(this, pWeapon);
		UTIL_MakeVectors(pev->angles);

		if (pWeapon->m_pItemInfo->m_iSlot == PRIMARY_WEAPON_SLOT)
			m_bHasPrimary = false;

		// the actual drop.
		CWeaponBox* pWeaponBox = nullptr;
		if (!pWeapon->Drop((void**)&pWeaponBox))
			return nullptr;

		return pWeaponBox;
	}

	return nullptr;
}

// Does the player already have this item?
bool CBasePlayer::HasPlayerItem(WeaponIdType iId)
{
	for (auto pWeapon : CBaseWeapon::m_lstWeapons)
	{
		if (pWeapon->m_pPlayer == this && pWeapon->m_iId == iId && !pWeapon->IsDead())
			return true;
	}

	return false;
}

void CBasePlayer::SwitchTeam()
{
	int oldTeam;
	char *szOldTeam;
	char *szNewTeam;
	const char *szName;
	char *szNewModel = nullptr;

	oldTeam = m_iTeam;

	if (m_iTeam == CT)
	{
		m_iTeam = TERRORIST;

		switch (m_iModelName)
		{
		case MODEL_URBAN:
			m_iModelName = MODEL_LEET;
			szNewModel = "leet";
			break;
		case MODEL_GIGN:
			m_iModelName = MODEL_GUERILLA;
			szNewModel = "guerilla";
			break;
		case MODEL_SAS:
			m_iModelName = MODEL_ARCTIC;
			szNewModel = "arctic";
			break;
		case MODEL_SPETSNAZ:
			if (AreRunningCZero())
			{
				m_iModelName = MODEL_MILITIA;
				szNewModel = "militia";
				break;
			}
		default:
			if (m_iModelName == MODEL_GSG9 || !IsBot() || !TheBotProfiles->GetCustomSkinModelname(m_iModelName))
			{
				m_iModelName = MODEL_TERROR;
				szNewModel = "terror";
			}
			break;
		}
	}
	else if (m_iTeam == TERRORIST)
	{
		m_iTeam = CT;

		switch (m_iModelName)
		{
		case MODEL_TERROR:
			m_iModelName = MODEL_GSG9;
			szNewModel = "gsg9";
			break;

		case MODEL_ARCTIC:
			m_iModelName = MODEL_SAS;
			szNewModel = "sas";
			break;

		case MODEL_GUERILLA:
			m_iModelName = MODEL_GIGN;
			szNewModel = "gign";
			break;

		case MODEL_MILITIA:
			if (AreRunningCZero())
			{
				m_iModelName = MODEL_SPETSNAZ;
				szNewModel = "spetsnaz";
				break;
			}
		default:
			if (m_iModelName == MODEL_LEET || !IsBot() || !TheBotProfiles->GetCustomSkinModelname(m_iModelName))
			{
				m_iModelName = MODEL_URBAN;
				szNewModel = "urban";
			}
			break;
		}
	}

	SetClientUserInfoModel(GET_INFO_BUFFER(edict()), szNewModel);

	MESSAGE_BEGIN(MSG_ALL, gmsgTeamInfo);
		WRITE_BYTE(entindex());
		WRITE_BYTE(m_iTeam);
	MESSAGE_END();

	if (TheBots)
	{
		TheBots->OnEvent(EVENT_PLAYER_CHANGED_TEAM, this);
	}

	UpdateLocation(true);

	if (m_iTeam)
	{
		SetScoreboardAttributes();
	}

	if (pev->netname)
	{
		szName = STRING(pev->netname);

		if (!szName[0])
			szName = "<unconnected>";
	}
	else
		szName = "<unconnected>";

	UTIL_ClientPrintAll(HUD_PRINTNOTIFY, (m_iTeam == TERRORIST) ? "#Game_join_terrorist_auto" : "#Game_join_ct_auto", szName);

	szOldTeam = GetTeam(oldTeam);
	szNewTeam = GetTeam(m_iTeam);

	UTIL_LogPrintf("\"%s<%i><%s><%s>\" joined team \"%s\" (auto)\n", STRING(pev->netname), GETPLAYERUSERID(edict()), GETPLAYERAUTHID(edict()), szOldTeam, szNewTeam);

	CCSBot *pBot = static_cast<CCSBot *>(this);

	if (pBot->IsBot())
	{
		const BotProfile *pProfile = pBot->GetProfile();

		if (pProfile)
		{
			bool bKick = false;

			if (m_iTeam == CT && !pProfile->IsValidForTeam(BOT_TEAM_CT))
				bKick = true;

			else if (m_iTeam == TERRORIST && !pProfile->IsValidForTeam(BOT_TEAM_T))
				bKick = true;

			if (bKick)
			{
				SERVER_COMMAND(UTIL_VarArgs("kick \"%s\"\n", STRING(pev->netname)));
			}
		}
	}
}

void CBasePlayer::UpdateShieldCrosshair(bool draw)
{
	if (draw)
		m_iHideHUD &= ~HIDEHUD_CROSSHAIR;
	else
		m_iHideHUD |= HIDEHUD_CROSSHAIR;
}

LINK_ENTITY_TO_CLASS(monster_hevsuit_dead, CDeadHEV)

void CDeadHEV::Spawn()
{
	PRECACHE_MODEL("models/player.mdl");
	SET_MODEL(ENT(pev), "models/player.mdl");

	pev->effects = 0;
	pev->yaw_speed = 8.0f;

	pev->sequence = 0;
	pev->body = 1;

	m_bloodColor = BLOOD_COLOR_RED;
	pev->sequence = LookupSequence(m_szPoses[m_iPose]);

	if (pev->sequence == -1)
	{
		ALERT(at_console, "Dead hevsuit with bad pose\n");
		pev->sequence = 0;
		pev->effects = EF_BRIGHTFIELD;
	}

	// Corpses have less health
	pev->health = 8;
	MonsterInitDead();
}

void CDeadHEV::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "pose"))
	{
		m_iPose = Q_atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseMonster::KeyValue(pkvd);
	}
}

LINK_ENTITY_TO_CLASS(player_weaponstrip, CStripWeapons)

void CStripWeapons::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CBasePlayer *pPlayer = nullptr;
	if (pActivator && pActivator->IsPlayer())
	{
		pPlayer = (CBasePlayer *)pActivator;
	}
	else if (!g_pGameRules->IsDeathmatch())
	{
		pPlayer = (CBasePlayer *)Instance(INDEXENT(1));
	}

	if (pPlayer)
	{
		pPlayer->RemoveAllItems(FALSE);
	}
}

void CBasePlayer::StudioEstimateGait()
{
	real_t dt;
	Vector est_velocity;

	dt = gpGlobals->frametime;

	if (dt < 0)
		dt = 0;

	else if (dt > 1.0)
		dt = 1;

	if (dt == 0)
	{
		m_flGaitMovement = 0;
		return;
	}

	est_velocity = pev->origin - m_prevgaitorigin;
	m_prevgaitorigin = pev->origin;

	m_flGaitMovement = est_velocity.Length();

	if (dt <= 0 || m_flGaitMovement / dt < 5)
	{
		m_flGaitMovement = 0;

		est_velocity.x = 0;
		est_velocity.y = 0;
	}

	if (!est_velocity.x && !est_velocity.y)
	{
		real_t flYawDiff = pev->angles.y - m_flGaityaw;
		real_t flYaw = Q_fmod(flYawDiff, 360);

		flYawDiff = flYawDiff - int64(flYawDiff / 360) * 360;

		if (flYawDiff > 180)
			flYawDiff -= 360;

		if (flYawDiff < -180)
			flYawDiff += 360;

		if (flYaw < -180)
			flYaw += 360;

		else if (flYaw > 180)
			flYaw -= 360;

		if (flYaw > -5 && flYaw < 5)
			m_flYawModifier = 0.05f;

		if (flYaw < -90 || flYaw > 90)
			m_flYawModifier = 3.5f;

		if (dt < 0.25f)
			flYawDiff *= dt * m_flYawModifier;
		else
			flYawDiff *= dt;

		if (real_t(Q_abs(flYawDiff)) < 0.1f)
			flYawDiff = 0;

		m_flGaityaw += flYawDiff;
		m_flGaityaw -= int64(m_flGaityaw / 360) * 360;
		m_flGaitMovement = 0;
	}
	else
	{
		m_flGaityaw = (Q_atan2(real_t(est_velocity.y), real_t(est_velocity.x)) * 180 / M_PI);

		if (m_flGaityaw > 180)
			m_flGaityaw = 180;

		if (m_flGaityaw < -180)
			m_flGaityaw = -180;
	}
}

void CBasePlayer::StudioPlayerBlend(int *pBlend, float *pPitch)
{
	// calc up/down pointing
	float range = float(int64(*pPitch * 3.0f));

	*pBlend = range;

	if (range <= -45.0f)
	{
		*pBlend = 255;
		*pPitch = 0;
	}
	else if (range >= 45.0f)
	{
		*pBlend = 0;
		*pPitch = 0;
	}
	else
	{
		*pBlend = int64((45.0f - range) * (255.0f / 90.0f));
		*pPitch = 0;
	}
}

void CBasePlayer::CalculatePitchBlend()
{
	int iBlend;
	float temp = pev->angles.x;

	StudioPlayerBlend(&iBlend, &temp);

	pev->blending[1] = iBlend;
	m_flPitch = iBlend;
}

void CBasePlayer::CalculateYawBlend()
{
	float dt;
	float maxyaw = 255.0f;

	real_t flYaw;		// view direction relative to movement
	real_t blend_yaw;

	dt = gpGlobals->frametime;

	if (dt < 0.0f)
		dt = 0;

	else if (dt > 1.0f)
		dt = 1;

	StudioEstimateGait();

	// calc side to side turning
	flYaw = Q_fmod(real_t(pev->angles.y - m_flGaityaw), 360);

	if (flYaw < -180)
		flYaw += 360;

	else if (flYaw > 180)
		flYaw -= 360;

	if (m_flGaitMovement != 0.0)
	{
		if (flYaw > 120)
		{
			m_flGaityaw -= 180;
			m_flGaitMovement = -m_flGaitMovement;
			flYaw -= 180;
		}
		else if (flYaw < -120)
		{
			m_flGaityaw += 180;
			m_flGaitMovement = -m_flGaitMovement;
			flYaw += 180;
		}
	}

	flYaw = (flYaw / 90) * 128 + 127;

	if (flYaw > 255)
		flYaw = 255;

	else if (flYaw < 0)
		flYaw = 0;

	blend_yaw = maxyaw - flYaw;

	pev->blending[0] = int64(blend_yaw);
	m_flYaw = blend_yaw;
}

void CBasePlayer::StudioProcessGait()
{
	mstudioseqdesc_t *pseqdesc;
	real_t dt = gpGlobals->frametime;

	if (dt < 0.0)
		dt = 0;

	else if (dt > 1.0)
		dt = 1;

	CalculateYawBlend();
	CalculatePitchBlend();

	studiohdr_t *pstudiohdr = (studiohdr_t *)GET_MODEL_PTR(edict());

	if (!pstudiohdr)
		return;

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + pev->gaitsequence;

	// calc gait frame
	if (pseqdesc->linearmovement.x > 0.0f)
		m_flGaitframe += (m_flGaitMovement / pseqdesc->linearmovement.x) * pseqdesc->numframes;
	else
		m_flGaitframe += pev->framerate * pseqdesc->fps * dt;

	// do modulo
	m_flGaitframe -= int(m_flGaitframe / pseqdesc->numframes) * pseqdesc->numframes;

	if (m_flGaitframe < 0)
		m_flGaitframe += pseqdesc->numframes;
}

void CBasePlayer::ResetStamina()
{
	pev->fuser1 = 0;
	pev->fuser3 = 0;
	pev->fuser2 = 0;
}

real_t GetPlayerPitch(const edict_t *pEdict)
{
	if (!pEdict)
		return 0.0f;

	entvars_t *pev = VARS(const_cast<edict_t *>(pEdict));
	CBasePlayer *pPlayer = CBasePlayer::Instance(pev);

	if (!pPlayer || !pPlayer->IsPlayer())
		return 0.0f;

	return pPlayer->m_flPitch;
}

real_t GetPlayerYaw(const edict_t *pEdict)
{
	if (!pEdict)
		return 0.0f;

	entvars_t *pev = VARS(const_cast<edict_t *>(pEdict));
	CBasePlayer *pPlayer = CBasePlayer::Instance(pev);

	if (!pPlayer || !pPlayer->IsPlayer())
		return 0.0f;

	return pPlayer->m_flYaw;
}

int GetPlayerGaitsequence(const edict_t *pEdict)
{
	if (!pEdict)
		return 1;

	entvars_t *pev = VARS(const_cast<edict_t *>(pEdict));
	CBasePlayer *pPlayer = CBasePlayer::Instance(pev);

	if (!pPlayer || !pPlayer->IsPlayer())
		return 1;

	return pPlayer->m_iGaitsequence;
}

void CBasePlayer::SpawnClientSideCorpse()
{
	// not allow to spawn, if the player was torn to gib
	if (pev->effects & EF_NODRAW)
		return;

	// do not make a corpse if the player goes to respawn.
	if (pev->deadflag == DEAD_RESPAWNABLE)
		return;

	char *infobuffer = GET_INFO_BUFFER(edict());
	char *pModel = GET_KEY_VALUE(infobuffer, "model");

	MESSAGE_BEGIN(MSG_ALL, gmsgSendCorpse);
		WRITE_STRING(pModel);
		WRITE_LONG(pev->origin.x * 128);
		WRITE_LONG(pev->origin.y * 128);
		WRITE_LONG(pev->origin.z * 128);
		WRITE_COORD(pev->angles.x);
		WRITE_COORD(pev->angles.y);
		WRITE_COORD(pev->angles.z);
		WRITE_LONG((pev->animtime - gpGlobals->time) * 100);
		WRITE_BYTE(pev->sequence);
		WRITE_BYTE(pev->body);
		WRITE_BYTE(m_iTeam);
		WRITE_BYTE(entindex());
	MESSAGE_END();

	m_canSwitchObserverModes = true;
}

BOOL CBasePlayer::IsArmored(int nHitGroup)
{
	BOOL fApplyArmor = FALSE;

	if (pev->armortype == ARMOR_NONE)
		return FALSE;

	switch (nHitGroup)
	{
	case HITGROUP_HEAD:
	{
		fApplyArmor = (pev->armortype == ARMOR_VESTHELM);
		break;
	}
	case HITGROUP_GENERIC:
	case HITGROUP_CHEST:
	case HITGROUP_STOMACH:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
		fApplyArmor = TRUE;
		break;
	}

	return fApplyArmor;
}

BOOL CBasePlayer::ShouldDoLargeFlinch(int nHitGroup, int nGunType)
{
	if (pev->flags & FL_DUCKING)
		return FALSE;

	if (nHitGroup != HITGROUP_LEFTLEG && nHitGroup != HITGROUP_RIGHTLEG)
	{
		switch (nGunType)
		{
		case WEAPON_SRS:
		case WEAPON_XM8:
		case WEAPON_AA12:
		case WEAPON_PSG1:
		case WEAPON_AWP:
		case WEAPON_KSG12:
		case WEAPON_M4A1:
		case WEAPON_SVD:
		case WEAPON_DEAGLE:
		case WEAPON_SCARH:
		case WEAPON_AK47:
			return TRUE;
		}
	}

	return FALSE;
}

void CBasePlayer::SetPrefsFromUserinfo(char *infobuffer)
{
	const char *pszKeyVal;

	pszKeyVal = GET_KEY_VALUE(infobuffer, "_cl_autowepswitch");

	if (Q_strcmp(pszKeyVal, "") != 0)
		m_iAutoWepSwitch = Q_atoi(pszKeyVal);
	else
		m_iAutoWepSwitch = 1;

	pszKeyVal = GET_KEY_VALUE(infobuffer, "_vgui_menus");

	if (Q_strcmp(pszKeyVal, "") != 0)
		m_bVGUIMenus = Q_atoi(pszKeyVal) != 0;
	else
		m_bVGUIMenus = true;

	pszKeyVal = GET_KEY_VALUE(infobuffer, "_ah");

	if (Q_strcmp(pszKeyVal, "") != 0)
		m_bShowHints = Q_atoi(pszKeyVal) != 0;
	else
		m_bShowHints = true;
}

bool CBasePlayer::IsLookingAtPosition(Vector *pos, float angleTolerance)
{
	Vector to = *pos - EyePosition();
	Vector idealAngle = UTIL_VecToAngles(to);

	idealAngle.x = 360.0 - idealAngle.x;

	float deltaYaw = NormalizeAngle(idealAngle.y - pev->v_angle.y);
	float deltaPitch = NormalizeAngle(idealAngle.x - pev->v_angle.x);

	return (Q_abs(deltaYaw) < angleTolerance
		&& Q_abs(deltaPitch) < angleTolerance);
}

void CBasePlayer::ClientCommand(const char *cmd, const char *arg1, const char *arg2, const char *arg3)
{
	BotArgs[0] = cmd;
	BotArgs[1] = arg1;
	BotArgs[2] = arg2;
	BotArgs[3] = arg3;

	UseBotArgs = true;
	::ClientCommand_(ENT(pev));
	UseBotArgs = false;
}

const char *GetBuyStringForWeaponClass(int weaponClass)
{
	switch (weaponClass)
	{
	case WEAPONCLASS_PISTOL:
		return "deagle elites fn57 usp glock p228 shield";
	case WEAPONCLASS_SNIPERRIFLE:
		return "awp sg550 g3sg1 scout";
	case WEAPONCLASS_GRENADE:
		return "hegren";
	case WEAPONCLASS_SHOTGUN:
		return "xm1014 m3";
	case WEAPONCLASS_SUBMACHINEGUN:
		return "p90 ump45 mp5 tmp mac10";
	case WEAPONCLASS_MACHINEGUN:
		return "m249";
	case WEAPONCLASS_RIFLE:
		return "sg552 aug ak47 m4a1 galil famas";
	}

	return nullptr;
}

bool IsPrimaryWeaponClass(int classId)
{
	return (classId >= WEAPONCLASS_SUBMACHINEGUN && classId <= WEAPONCLASS_SNIPERRIFLE);
}

bool IsPrimaryWeaponId(int id)
{
	int classId = WEAPONCLASS_NONE;
	const char *alias = WeaponIDToAlias(id);

	if (alias)
	{
		classId = AliasToWeaponClass(alias);
	}

	return IsPrimaryWeaponClass(classId);
}

bool IsSecondaryWeaponClass(int classId)
{
	return (classId == WEAPONCLASS_PISTOL);
}

bool IsSecondaryWeaponId(int id)
{
	int classId = WEAPONCLASS_NONE;
	const char *alias = WeaponIDToAlias(id);

	if (alias)
	{
		classId = AliasToWeaponClass(alias);
	}

	return IsSecondaryWeaponClass(classId);
}

const char *GetWeaponAliasFromName(const char *weaponName)
{
	const char cut_weapon[] = "weapon_";
	if (!Q_strncmp(weaponName, cut_weapon, sizeof(cut_weapon) - 1))
	{
		weaponName += sizeof(cut_weapon) - 1;
	}

	return weaponName;
}

bool CurrentWeaponSatisfies(CBaseWeapon *pWeapon, int id, int classId)
{
	if (!pWeapon)
		return false;

	const char *weaponName = GetWeaponAliasFromName(pWeapon->m_pItemInfo->m_pszInternalName);

	if (id && AliasToWeaponID(weaponName) == id)
		return true;

	if (classId && AliasToWeaponClass(weaponName) == classId)
		return true;

	return false;
}

void CBasePlayer::ParseAutoBuy()
{
	WeaponIdType iRecommandedPrim = WEAPON_NONE, iRecommandedSed = WEAPON_NONE;

	switch (m_iRoleType)
	{
	case Role_Commander:
		iRecommandedPrim = WEAPON_M4A1;
		break;

	case Role_Godfather:
	case Role_LeadEnforcer:
		iRecommandedPrim = WEAPON_AK47;
		break;

	case Role_SWAT:
	case Role_Medic:
	case Role_Assassin:
		iRecommandedPrim = WEAPON_MP7A1;
		break;

	case Role_Breacher:
	case Role_Arsonist:
		iRecommandedPrim = WEAPON_KSG12;
		break;

	case Role_MadScientist:
		iRecommandedPrim = WEAPON_UMP45;
		break;

	case Role_Sharpshooter:	// we recommand sharpshooter use DEAGLE.
	default:
		iRecommandedPrim = WEAPON_NONE;
		break;
	}

	// analysis best 2nd weapon from prim weapon.
	if (m_rgpPlayerItems[PRIMARY_WEAPON_SLOT])
	{
		switch (m_rgpPlayerItems[PRIMARY_WEAPON_SLOT]->m_pItemInfo->m_iClassType)
		{
		case WEAPONCLASS_SUBMACHINEGUN:
		case WEAPONCLASS_MACHINEGUN:
		case WEAPONCLASS_RIFLE:
			iRecommandedSed = WEAPON_DEAGLE;
			break;

		case WEAPONCLASS_SHOTGUN:
		case WEAPONCLASS_SNIPERRIFLE:
			iRecommandedSed = WEAPON_GLOCK18;
			break;

		default:
			iRecommandedSed = WEAPON_NONE;
			break;
		}
	}
	else
	{
		switch (m_iRoleType)
		{
		case Role_Commander:
		case Role_SWAT:
		case Role_Sharpshooter:
		case Role_Medic:
		case Role_Godfather:
		case Role_LeadEnforcer:
		case Role_MadScientist:
			iRecommandedSed = WEAPON_DEAGLE;
			break;

		case Role_Breacher:
		case Role_Arsonist:
			iRecommandedSed = WEAPON_GLOCK18;
			break;

		case Role_Assassin:
			iRecommandedSed = WEAPON_USP;	// because his skill.
			break;

		default:
			iRecommandedSed = WEAPON_NONE;
			break;
		}
	}

	if (!m_rgpPlayerItems[PRIMARY_WEAPON_SLOT])
		BuyWeapon(this, iRecommandedPrim);

	BuyAmmo(this, PRIMARY_WEAPON_SLOT);

	if (!m_rgpPlayerItems[PISTOL_SLOT])
		BuyWeapon(this, iRecommandedSed);

	BuyAmmo(this, PISTOL_SLOT);

	BuyEquipment(this, EQP_KEVLAR);
	BuyEquipment(this, EQP_ASSAULT_SUIT);
	BuyEquipment(this, CSGameRules()->SelectProperGrenade(this));

	if (m_iAccount > 12000)	// too much money? I can fix that for you!
		BuyEquipment(this, EQP_NVG);
}

void CBasePlayer::ParseRebuy()
{
	if (m_lstRebuy.empty())
	{
		UTIL_SayText(this, "#LeaderMod_SetRebuyFirst");
		return;
	}

	for (auto iId : m_lstRebuy)
	{
		BuyWeapon(this, iId);
	}
}

void CBasePlayer::SaveRebuy()
{
	m_lstRebuy.clear();

	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		if (m_rgpPlayerItems[i])
			m_lstRebuy.emplace_back(m_rgpPlayerItems[i]->m_iId);
	}

	char szText[192];
	Q_snprintf(szText, sizeof(szText) - 1, "$[Cyan]Rebuy items set: ");

	for (auto& iId : m_lstRebuy)
	{
		Q_strlcat(szText, "$[Yellowish][$[Springgreen]");
		Q_strlcat(szText, g_rgWpnInfo[iId].m_pszExternalName);
		Q_strlcat(szText, "$[Yellowish]]$[Cyan] ");
	}

	UTIL_SayText(this, szText);
}

bool CBasePlayer::IsObservingPlayer(CBasePlayer *pPlayer)
{
	if (!pPlayer || pev->flags == FL_DORMANT)
		return false;

	if (FNullEnt(pPlayer))
		return false;

	return (GetObserverMode() == OBS_IN_EYE && pev->iuser2 == pPlayer->entindex()) != 0;
}

void CBasePlayer::UpdateLocation(bool forceUpdate)
{
	if (!forceUpdate && m_flLastUpdateTime >= gpGlobals->time + 2.0f)
		return;

	const char *placeName = "";

	if (pev->deadflag == DEAD_NO && AreRunningCZero())
	{
		// search the place name where is located the player
		Place playerPlace = TheNavAreaGrid.GetPlace(&pev->origin);
		const BotPhraseList *placeList = TheBotPhrases->GetPlaceList();
		for (auto phrase : *placeList)
		{
			if (phrase->GetID() == playerPlace)
			{
				placeName = phrase->GetName();
				break;
			}
		}
	}

	if (!placeName[0] || (m_lastLocation[0] && !Q_strcmp(placeName, &m_lastLocation[1])))
	{
		return;
	}

	m_flLastUpdateTime = gpGlobals->time;
	Q_snprintf(m_lastLocation, sizeof(m_lastLocation), "#%s", placeName);

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);

		if (!pPlayer)
			continue;

		if (pPlayer->m_iTeam == m_iTeam || pPlayer->m_iTeam == SPECTATOR)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgLocation, nullptr, pPlayer->edict());
				WRITE_BYTE(entindex());
				WRITE_STRING(m_lastLocation);
			MESSAGE_END();
		}
		else if (forceUpdate)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgLocation, nullptr, pPlayer->edict());
				WRITE_BYTE(entindex());
				WRITE_STRING("");
			MESSAGE_END();
		}
	}
}

void CBasePlayer::ReloadWeapons(CBaseWeapon *pWeapon, bool bForceReload, bool bForceRefill)
{
	bool bCanAutoReload = (bForceReload || auto_reload_weapons.value != 0.0f);
	bool bCanRefillBPAmmo = (bForceRefill || refill_bpammo_weapons.value != 0.0f);

	if (!bCanAutoReload && !bCanRefillBPAmmo)
		return;

	// if we died in the previous round
	// so that we have nothing to reload
	if (!m_bNotKilled)
		return;

	// to ignore first spawn on ClientPutinServer
	if (m_bJustConnected)
		return;

	for (auto pWeapon : CBaseWeapon::m_lstWeapons)
	{
		if (pWeapon->m_pPlayer != this)
			continue;

		if (bCanAutoReload)
			pWeapon->m_iClip = pWeapon->m_pItemInfo->m_iMaxClip;

		if (bCanRefillBPAmmo)
			m_rgAmmo[pWeapon->m_iPrimaryAmmoType] = pWeapon->m_pAmmoInfo->m_iMax;
	}
}

void CBasePlayer::TeamChangeUpdate()
{
	MESSAGE_BEGIN(MSG_ALL, gmsgTeamInfo);
		WRITE_BYTE(entindex());
		WRITE_BYTE(m_iTeam);
	MESSAGE_END();

	if (m_iTeam != UNASSIGNED)
	{
		SetScoreboardAttributes();
	}
}

bool EXT_FUNC CBasePlayer::HasRestrictItem(ItemID item, ItemRestType type)
{
	return false;
}

void CBasePlayer::DropSecondary()
{
	if (HasShield())
	{
		if (IsProtectedByShield() && m_pActiveItem)
		{
			m_pActiveItem->SecondaryAttack();
		}

		m_bShieldDrawn = false;
	}

	for (auto pWeapon : CBaseWeapon::m_lstWeapons)
	{
		if (pWeapon->m_pPlayer != this)
			continue;

		if (pWeapon->m_pItemInfo->m_iSlot != PISTOL_SLOT)
			continue;

		DropPlayerItem(pWeapon->m_iId);
	}
}

void CBasePlayer::DropPrimary()
{
	if (HasShield())
	{
		DropShield();
		return;
	}

	for (auto pWeapon : CBaseWeapon::m_lstWeapons)
	{
		if (pWeapon->m_pPlayer != this)
			continue;

		if (pWeapon->m_pItemInfo->m_iSlot != PRIMARY_WEAPON_SLOT)
			continue;

		DropPlayerItem(pWeapon->m_iId);
	}
}

CBaseWeapon *CBasePlayer::GetItemById(WeaponIdType weaponID)
{
	for (auto pWeapon : CBaseWeapon::m_lstWeapons)
	{
		if (pWeapon->m_pPlayer != this)
			continue;

		if (pWeapon->m_iId != weaponID)
			continue;

		return pWeapon;
	}

	return nullptr;
}

void CBasePlayer::Disconnect()
{
	SetThink(nullptr);
}

void EXT_FUNC CBasePlayer::OnSpawnEquip(bool addDefault, bool equipGame)
{
	if (equipGame)
	{
		CBaseEntity *pWeaponEntity = nullptr;
		while ((pWeaponEntity = UTIL_FindEntityByClassname(pWeaponEntity, "game_player_equip")))
		{
			pWeaponEntity->Touch(this);
			addDefault = false;
		}
	}

	if (m_bNotKilled)
		addDefault = false;

	if (addDefault)
	{
		CSGameRules()->GiveDefaultItems(this);
	}
}

void CBasePlayer::HideTimer()
{
	m_iHideHUD |= HIDEHUD_WEAPONS;
}

bool EXT_FUNC CBasePlayer::GetIntoGame()
{
	m_bNotKilled = false;
	m_iIgnoreGlobalChat = IGNOREMSG_NONE;

	m_iTeamKills = 0;
	pev->fov = DEFAULT_FOV;

	m_bJustConnected = false;
	m_fLastMovement = gpGlobals->time;

	ResetMaxSpeed();
	m_iJoiningState = JOINED;

	if (CSGameRules()->FPlayerCanRespawn(this))
	{
		RoundRespawn();
		CSGameRules()->CheckWinConditions();
	}
	else
	{
		pev->deadflag = DEAD_RESPAWNABLE;

		MAKE_STRING_CLASS("player", pev);

		pev->flags &= (FL_PROXY | FL_FAKECLIENT);
		pev->flags |= (FL_SPECTATOR | FL_CLIENT);

		edict_t *pentSpawnSpot = g_pGameRules->GetPlayerSpawnSpot(this);
		StartObserver(pev->origin, pentSpawnSpot->v.angles);

		CSGameRules()->CheckWinConditions();

		MESSAGE_BEGIN(MSG_ALL, gmsgTeamInfo);
			WRITE_BYTE(entindex());
			WRITE_BYTE(m_iTeam);
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ALL, gmsgLocation);
			WRITE_BYTE(entindex());
			WRITE_STRING("");
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
			WRITE_BYTE(ENTINDEX(edict()));
			WRITE_SHORT(int(pev->frags));
			WRITE_SHORT(m_iDeaths);
			WRITE_SHORT(0);
			WRITE_SHORT(m_iTeam);
		MESSAGE_END();

		if (!(m_flDisplayHistory & DHF_SPEC_DUCK))
		{
			HintMessage("#Spec_Duck", TRUE, TRUE);
			m_flDisplayHistory |= DHF_SPEC_DUCK;
		}
	}

	return true;
}

void CBasePlayer::PlayerRespawnThink()
{
	if (GetObserverMode() != OBS_NONE && (m_iTeam == UNASSIGNED || m_iTeam == SPECTATOR))
		return;

	// Player cannot respawn while in the Choose Appearance menu
	if (m_iMenu == Menu_ChooseAppearance || m_iJoiningState == SHOWTEAMSELECT)
		return;

	if (pev->deadflag < DEAD_DYING)
		return;

	if (!CSGameRules()->FPlayerCanRespawn(this))
	{
		if (m_flRespawnPending > 0.0f)	// redeployment is forced to stop.
		{
			m_flRespawnPending = -1.0f;
			SetProgressBarTime(0);
		}

		return;
	}

	if (CSGameRules()->FPlayerCanRespawn(this) && m_flRespawnPending <= 0)	// ready to redeploy in battlefield.
	{
		m_flRespawnPending = gpGlobals->time + CSGameRules()->GetPlayerRespawnTime(this);
		SetProgressBarTime(CSGameRules()->GetPlayerRespawnTime(this));
	}

	if (m_flRespawnPending > 0.0f && m_flRespawnPending <= gpGlobals->time)
	{
		respawn(pev);
		pev->button = 0;
		pev->nextthink = -1;
		
		CSGameRules()->m_rgiManpowers[m_iTeam]--;

		if (m_iRoleType == Role_LeadEnforcer)	// berserker has a doubled consumption.
			CSGameRules()->m_rgiManpowers[m_iTeam]--;
	}
}

bool CBasePlayer::CanSwitchTeam(TeamName teamToSwap)
{
	if (m_iTeam != teamToSwap)
		return false;

	// we won't VIP player to switch team
	// TODO: godfather and commander also aren't allowed.

	return true;
}

void EXT_FUNC CBasePlayer::SetSpawnProtection(float flProtectionTime)
{
	if (respawn_immunity_effects.value > 0)
	{
		pev->rendermode = kRenderTransAdd;
		pev->renderamt  = 100.0f;
	}

	m_flSpawnProtectionEndTime = gpGlobals->time + flProtectionTime;
}

void CBasePlayer::RemoveSpawnProtection()
{
	if (respawn_immunity_effects.value > 0)
	{
		if (pev->rendermode == kRenderTransAdd &&
			pev->renderamt == 100.0f)
		{
			pev->renderamt  = 255.0f;
			pev->rendermode = kRenderNormal;
		}
	}

	m_flSpawnProtectionEndTime = 0.0f;
}

void EXT_FUNC CBasePlayer::DropIdlePlayer(const char *reason)
{
	if (!autokick.value)
		return;

	edict_t *pEntity = edict();

	int iUserID = GETPLAYERUSERID(pEntity);

	// Log the kick
	UTIL_LogPrintf("\"%s<%i><%s><%s>\" triggered \"Game_idle_kick\" (auto)\n", STRING(pev->netname), iUserID , GETPLAYERAUTHID(pEntity), GetTeam(m_iTeam));
	UTIL_ClientPrintAll(HUD_PRINTCONSOLE, "#Game_idle_kick", STRING(pev->netname));

	if (iUserID != -1)
	{
		SERVER_COMMAND(UTIL_VarArgs("kick #%d \"%s\"\n", iUserID, reason));
	}
}

bool CBasePlayer::CheckActivityInGame()
{
	const float deltaYaw = (m_vecOldvAngle.y - pev->v_angle.y);
	const float deltaPitch = (m_vecOldvAngle.x - pev->v_angle.x);

	m_vecOldvAngle = pev->v_angle;

	return (Q_fabs(deltaYaw) >= 0.1f && Q_fabs(deltaPitch) >= 0.1f);
}

int CBasePlayer::GetGrenadeInventory(EquipmentIdType iId)
{
	return *GetGrenadeInventoryPointer(iId);
}

int* CBasePlayer::GetGrenadeInventoryPointer(EquipmentIdType iId)
{
	return &m_rgAmmo[GetAmmoIdOfEquipment(iId)];
}

void CBasePlayer::ResetUsingEquipment(void)
{
	// the stock is ok.
	if (GetGrenadeInventory(m_iUsingGrenadeId) > 0)
		return;

	// if this is a non-consumable accessory, it's also fine.
	if (m_rgbHasEquipment[m_iUsingGrenadeId])
		return;

	AmmoIdType iAmmoId = AMMO_NONE;
	EquipmentIdType iCandidate = CSGameRules()->SelectProperGrenade(this);

	// check inventory of recommanded equipment.
	if (GetGrenadeInventory(iCandidate))
	{
		// it's good? take and leave.
		m_iUsingGrenadeId = iCandidate;
		return;
	}

	// start the searching from the first.
	for (int i = EQP_NONE; i < EQP_COUNT; i++)
	{
		iAmmoId = GetAmmoIdOfEquipment((EquipmentIdType)i);

		if (!iAmmoId || m_rgAmmo[iAmmoId] <= 0)
			continue;

		iCandidate = (EquipmentIdType)i;
		break;
	}

	m_iUsingGrenadeId = iCandidate;
}

bool CBasePlayer::StartSwitchingWeapon(CBaseWeapon* pSwitchingTo)
{
	if (!pSwitchingTo)
		return false;

	// TODO
	if ((m_pActiveItem && !m_pActiveItem->CanHolster()) /*|| !pSwitchingTo->CanDeploy()*/)
		return false;

	if (m_pActiveItem)
	{
		m_pActiveItem->HolsterStart();
		m_iWpnSwitchingTo = pSwitchingTo->m_iId;

		return true;
	}
	else
	{
		// no active weapon? which means we can directly deploy this one.
		return SwitchWeapon(pSwitchingTo);
	}
}

bool CBasePlayer::SwitchWeapon(CBaseWeapon* pSwitchingTo)
{
	if (!pSwitchingTo)
		return false;

	// TODO
	if ((m_pActiveItem && !m_pActiveItem->CanHolster()) /*|| !pSwitchingTo->CanDeploy()*/)
		return false;

	ResetAutoaim();

	if (m_pActiveItem)
	{
		m_pActiveItem->Holstered();
	}

	if (HasShield())
	{
		if (m_pActiveItem)
			m_pActiveItem->m_bitsFlags &= ~WPNSTATE_SHIELD_DRAWN;

		m_bShieldDrawn = false;
		UpdateShieldCrosshair(true);
	}

	m_pLastItem = m_pActiveItem;
	m_pActiveItem = pSwitchingTo;

	m_pActiveItem->Deploy();
	m_pActiveItem->UpdateClientData();

	ResetMaxSpeed();

	return true;
}

void CBasePlayer::QueryClientCvar(void)
{
	// never query BOTs. it would cause a CTD.
	if (IsBot() || m_flNextClientCvarQuery > gpGlobals->time)
		return;

	m_flNextClientCvarQuery = gpGlobals->time + clientcvar_query_interval.value;

	// the list of cvar query.
	g_engfuncs.pfnQueryClientCvarValue2(edict(), "cl_holdtoaim", QueryIndex(0));
}

void CBasePlayer::UpdateClientCvar(const char* cvarName, const char* value, int requestID)
{
	if (requestID == QueryIndex(0))
	{
		m_bHoldToAim = !!Q_atoi(value);
	}
}

void CBasePlayer::AssignRole(RoleTypes iNewRole)
{
	// these two needs special handle.
	if (m_iRoleType == Role_Commander && iNewRole != Role_Commander)
		CSGameRules()->AssignCommander(nullptr);
	else if (m_iRoleType == Role_Godfather && iNewRole != Role_Godfather)
		CSGameRules()->AssignGodfather(nullptr);

	// remove all skills from last role.
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
		{
			if (m_rgpSkills[i]->m_bUsingSkill)
				m_rgpSkills[i]->Terminate();	// terminate skill before remove it.

			delete m_rgpSkills[i];
		}

		m_rgpSkills[i] = nullptr;
	}

	switch (iNewRole)
	{
	case Role_Commander:
		CBaseSkill::Grand<CSkillFireRate>(this);
		CBaseSkill::Grand<CSkillRadarScan>(this);
		CBaseSkill::Grand<CSkillReduceDamage>(this);
		break;

	case Role_SWAT:
		CBaseSkill::Grand<CSkillBulletproof>(this);
		CBaseSkill::Grand<CSkillArmorRegen>(this);
		break;

	case Role_Breacher:
		CBaseSkill::Grand<CSkillExplosiveBullets>(this);
		CBaseSkill::Grand<CSkillInfiniteGrenade>(this);
		break;

	case Role_Sharpshooter:
		CBaseSkill::Grand<CSkillEnfoceHeadshot>(this);
		CBaseSkill::Grand<CSkillHighlightSight>(this);
		break;

	case Role_Medic:
		CBaseSkill::Grand<CSkillHealingShot>(this);
		break;

	case Role_Godfather:
		CBaseSkill::Grand<CSkillReduceDamage>(this);
		CBaseSkill::Grand<CSkillGavelkind>(this);
		break;

	case Role_LeadEnforcer:
		CBaseSkill::Grand<CSkillDmgIncByHP>(this);
		CBaseSkill::Grand<CSkillResistDeath>(this);
		break;

	case Role_MadScientist:
		CBaseSkill::Grand<CSkillTaserGun>(this);
		CBaseSkill::Grand<CSkillRetribution>(this);
		break;

	case Role_Assassin:
		CBaseSkill::Grand<CSkillInvisible>(this);
		CBaseSkill::Grand<CSkillCriticalHit>(this);
		CBaseSkill::Grand<CSkillRadarScan2>(this);
		break;

	case Role_Arsonist:
		CBaseSkill::Grand<CSkillReduceDamage>(this);
		CBaseSkill::Grand<CSkillIncendiaryAmmo>(this);
		break;

	case Role_UNASSIGNED:
	default:
		break;
	}

	// send the news to gamerules.
	CSGameRules()->m_rgpCharacters[iNewRole] = this;
	CSGameRules()->m_rgpCharacters[m_iRoleType] = nullptr;

	// self-crowned
	m_iRoleType = iNewRole;

	// is all weapon you have okay now?
	// remember, this must be used after m_iRoleType be renewed.
	CheckItemAccessibility();

	// after the weapons are checked, variegate them.
	for (auto pWeapon : CBaseWeapon::m_lstWeapons)
	{
		if (pWeapon->m_pPlayer != this)
			continue;

		if (pWeapon->IsDead())
			continue;

		pWeapon->SetVariation(m_iRoleType);
	}

	// tell client the fact.
	MESSAGE_BEGIN(MSG_ALL, gmsgRole);
	WRITE_BYTE(entindex());
	WRITE_BYTE(iNewRole);
	MESSAGE_END();

	// enforce the health and armor update.
	m_iClientHealth = -1;
	m_iClientBattery = -1;
}

void CBasePlayer::CheckItemAccessibility()
{
	for (auto pWeapon : CBaseWeapon::m_lstWeapons)
	{
		if (pWeapon->m_pPlayer != this)
			continue;

		if (pWeapon->IsDead())
			continue;

		if (g_rgRoleWeaponsAccessibility[m_iRoleType][pWeapon->m_iId] == WPN_F)
		{
			AddAccount(GetWeaponInfo(pWeapon->m_iId)->m_iCost / 2, RT_SOLD_ITEM);
			UTIL_SayText(this, "#LeaderMod_RefundWpn", pWeapon->m_pItemInfo->m_pszExternalName, std::to_string(GetWeaponInfo(pWeapon->m_iId)->m_iCost / 2).c_str());
			UTIL_PlayEarSound(this, SFX_REFUND_GUNS);

			pWeapon->Kill();	// RemovePlayerItem() is included!
		}
	}
}

CBaseSkill* CBasePlayer::GetPrimarySkill()
{
	CBaseSkill* pSkill = nullptr;

	switch (m_iRoleType)
	{
	case Role_Commander:
		pSkill = m_rgpSkills[SkillType_Auxiliary];
		break;

	case Role_SWAT:
		pSkill = m_rgpSkills[SkillType_Defense];
		break;

	case Role_Breacher:
		pSkill = m_rgpSkills[SkillType_Attack];
		break;

	case Role_Sharpshooter:
		pSkill = m_rgpSkills[SkillType_WeaponEnhance];
		break;

	case Role_Medic:
		pSkill = m_rgpSkills[SkillType_Auxiliary];
		break;

	case Role_Godfather:
		pSkill = m_rgpSkills[SkillType_Auxiliary];
		break;

	case Role_LeadEnforcer:
		pSkill = m_rgpSkills[SkillType_Attack];
		break;

	case Role_MadScientist:
		pSkill = m_rgpSkills[SkillType_WeaponEnhance];
		break;

	case Role_Assassin:
		pSkill = m_rgpSkills[SkillType_Defense];
		break;

	case Role_Arsonist:
		pSkill = m_rgpSkills[SkillType_WeaponEnhance];
		break;

	default:
		break;
	}

	return pSkill;
}

bool CBasePlayer::IsUsingPrimarySkill()
{
	CBaseSkill* pSkill = GetPrimarySkill();

	if (pSkill)
	{
		return pSkill->m_bUsingSkill;
	}

	return false;
}

void CBasePlayer::DischargePrimarySkill(CBasePlayer* pCause)
{
	CBaseSkill* pSkill = GetPrimarySkill();

	if (pSkill)
	{
		pSkill->Discharge(pCause);
	}
}

float CBasePlayer::WeaponFireIntervalModifier(CBaseWeapon* pWeapon)
{
	float flResult = 1.0f;

	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			flResult *= m_rgpSkills[i]->WeaponFireIntervalModifier(pWeapon);	// the active or not is determind in the function itself.
	}

	return flResult;
}

float CBasePlayer::PlayerDamageSufferedModifier(int bitsDamageTypes)
{
	float flResult = 1.0f;

	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			flResult *= m_rgpSkills[i]->PlayerDamageSufferedModifier(bitsDamageTypes);	// the active or not is determind in the function itself.
	}

	return flResult;
}

float CBasePlayer::PlayerDamageDealtModifier(int bitsDamageTypes)
{
	float flResult = 1.0f;

	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			flResult *= m_rgpSkills[i]->PlayerDamageDealtModifier(bitsDamageTypes);	// the active or not is determind in the function itself.
	}

	return flResult;
}

void CBasePlayer::OnPlayerDamagedPre(entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int& bitsDamageTypes)
{
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnPlayerDamagedPre(pevInflictor, pevAttacker, flDamage, bitsDamageTypes);	// the active or not is determind in the function itself.
	}
}

void CBasePlayer::OnTraceDamagePre(float& flDamage, TraceResult& tr)
{
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnTraceDamagePre(flDamage, tr);	// the active or not is determind in the function itself.
	}
}

void CBasePlayer::OnFireBullets3PreDamage(float& flDamage, TraceResult& tr)
{
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnFireBullets3PreDamage(flDamage, tr);	// the active or not is determind in the function itself.
	}
}

void CBasePlayer::OnFireBuckshotsPreTraceAttack(float& flDamage, TraceResult& tr)
{
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnFireBuckshotsPreTraceAttack(flDamage, tr);	// the active or not is determind in the function itself.
	}
}

void CBasePlayer::OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr)
{
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnPlayerFiringTraceLine(iDamage, tr);	// the active or not is determind in the function itself.
	}
}

void CBasePlayer::OnPlayerKills(CBasePlayer* pVictim)
{
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnPlayerKills(pVictim);
	}
}

void CBasePlayer::OnHurtingAnotherPlayer(CBasePlayer* pVictim, entvars_t* pevInflictor, float& flDamage, int& bitsDamageTypes)
{
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnHurtingAnotherPlayer(pVictim, pevInflictor, flDamage, bitsDamageTypes);	// the active or not is determind in the function itself.
	}
}

void CBasePlayer::OnGrenadeThrew(EquipmentIdType iId, CGrenade* pGrenade)
{
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnGrenadeThrew(iId, pGrenade);	// the active or not is determind in the function itself.
	}
}

bool CBasePlayer::OnBlind()
{
	bool rgbResults[SKILLTYPE_COUNT];

	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			rgbResults[i] = m_rgpSkills[i]->OnBlind();
	}

	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (!rgbResults[i])	// if a skill decided not to blind, it would be a veto.
			return false;
	}

	return true;
}

void CBasePlayer::OnAddToFullPack(entity_state_s* pState, edict_t* pEnt, BOOL FIsPlayer)
{
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnAddToFullPack(pState, pEnt, FIsPlayer);
	}
}

void CBasePlayer::OnBeingAddToFullPack(entity_state_s* pState, CBasePlayer* pHost)
{
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnBeingAddToFullPack(pState, pHost);
	}
}

void CBasePlayer::OnResetPlayerMaxspeed(float& flSpeed)
{
	for (int i = SkillType_UNASSIGNED; i < SKILLTYPE_COUNT; i++)
	{
		if (m_rgpSkills[i])
			m_rgpSkills[i]->OnResetPlayerMaxspeed(flSpeed);
	}
}
