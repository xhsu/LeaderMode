/*

Created date: 03/03/2020

*/

#include "precompiled.h"


/////////////////
// OVERHEALING //
/////////////////

LINK_ENTITY_TO_CLASS(healing_smoke_centre, CHealingSmokeCenter);

const char* CHealingSmokeCenter::HEALING_SFX[10] =
{
	"leadermode/choosecountry1.wav",
	"leadermode/choosecountry2.wav",
	"leadermode/choosecountry3.wav",
	"leadermode/choosecountry4.wav",
	"leadermode/choosecountry5.wav",
	"leadermode/choosecountry6.wav",
	"leadermode/choosecountry7.wav",
	"leadermode/choosecountry8.wav",
	"leadermode/choosecountry9.wav",
	"leadermode/choosecountry10.wav",
};

const char* CHealingSmokeCenter::HEALING_FULL_SFX = "leadermode/Building_Completed.wav";
const float CHealingSmokeCenter::HEALING_INTERVAL = 2.0f;
const float CHealingSmokeCenter::HEALING_AMOUNT = 5.0f;

CHealingSmokeCenter* CHealingSmokeCenter::Create(Vector vecOrigin, CBasePlayer* pDoctor, float flRadius, float flTimeRemoved)
{
	CHealingSmokeCenter* pEntity = GetClassPtr((CHealingSmokeCenter*)nullptr);
	SET_ORIGIN(pEntity->edict(), vecOrigin);
	pEntity->pev->nextthink = gpGlobals->time + 0.01f;
	pEntity->m_pPlayer = pDoctor;
	pEntity->m_flTimeRemoved = gpGlobals->time + flTimeRemoved;
	pEntity->m_flRadius = flRadius;

	MESSAGE_BEGIN(MSG_ALL, SVC_TEMPENTITY);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(vecOrigin[0]);
	WRITE_COORD(vecOrigin[1]);
	WRITE_COORD(vecOrigin[2]);
	WRITE_BYTE(int(flRadius / 10.0f));		// range: 275 ?
	WRITE_BYTE(CSkillHealingShot::HEALING_COLOR.x);
	WRITE_BYTE(CSkillHealingShot::HEALING_COLOR.y);
	WRITE_BYTE(CSkillHealingShot::HEALING_COLOR.z);
	WRITE_BYTE(int(flTimeRemoved * 10.0f));	// time: original CS smokegrenade lasts 25 sec.
	WRITE_BYTE(0);
	MESSAGE_END();

	return pEntity;
}

void CHealingSmokeCenter::Precache()
{
	PRECACHE_SOUND(HEALING_FULL_SFX);
}

void CHealingSmokeCenter::Think()
{
	pev->nextthink = gpGlobals->time + 0.01f;

	if (m_flTimeRemoved < gpGlobals->time)
	{
		pev->flags |= FL_KILLME;
		return;
	}

	if (m_flNextHealingScan > gpGlobals->time)
		return;

	m_flNextHealingScan = gpGlobals->time + HEALING_INTERVAL;

	CBasePlayer* pPlayer = nullptr;
	while ((pPlayer = UTIL_FindEntityInSphere(pPlayer, pev->origin, m_flRadius)))
	{
		if (FNullEnt(pPlayer) || !pPlayer->IsPlayer())
			continue;

		/*
		pev(pPlayer, pev_origin, vecVictimOrigin);
		if (!UTIL_PointVisible(vecOrigin, vecVictimOrigin, IGNORE_MONSTERS))
			continue;
		*/

		float flOHMaximum = pPlayer->pev->max_health * gOverHealingMgr::OVERHEALING_MAX_RATIO;
		if (pPlayer->pev->health >= flOHMaximum)
		{
			if (pPlayer->m_flOHNextThink > 0.0f)	// player has been overhealing.
				pPlayer->m_flOHNextThink = gpGlobals->time + HEALING_INTERVAL + gOverHealingMgr::OVERHEALING_DECAY_INTERVAL;	// elongate the decay interval. this prevents player OH decay in the smoke.

			continue;
		}

		bool bShouldPlaySpecialSFX = false;	// play the SFX on:
		if ((pPlayer->pev->health < pPlayer->pev->max_health && (pPlayer->pev->health + HEALING_AMOUNT) >= pPlayer->pev->max_health)	// this healing would cross the 100.0 point!
			|| (pPlayer->pev->health < flOHMaximum && (pPlayer->pev->health + HEALING_AMOUNT) >= flOHMaximum)) // or this healing would reach 200.0 the maximum of OH!
		{
			bShouldPlaySpecialSFX = true;
		}

		if (bShouldPlaySpecialSFX)
			EMIT_SOUND(pPlayer->edict(), CHAN_AUTO, HEALING_FULL_SFX, VOL_NORM, ATTN_NORM);
		else
			UTIL_PlayEarSound(pPlayer, RANDOM_ARRAY(HEALING_SFX));

		if (pPlayer->TakeHealth(HEALING_AMOUNT, HEALING_REMOVE_DOT))
			m_pPlayer->AddAccount(HEALING_AMOUNT, RT_HELPED_TEAMMATE);	// healing for money.

		if (pPlayer->m_flOHNextThink > 0.0f)	// player has been overhealing.
			pPlayer->m_flOHNextThink = gpGlobals->time + HEALING_INTERVAL + gOverHealingMgr::OVERHEALING_DECAY_INTERVAL;	// elongate the decay interval. this prevents player OH decay in the smoke.

		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pPlayer->pev->origin);
		WRITE_BYTE(TE_SPRITE);
		WRITE_COORD(pPlayer->pev->origin.x);
		WRITE_COORD(pPlayer->pev->origin.y);
		WRITE_COORD(pPlayer->pev->origin.z);
		WRITE_SHORT(CSkillHealingShot::m_idHealingSpr);
		WRITE_BYTE(10);
		WRITE_BYTE(255);
		MESSAGE_END();
	}
}

const float gOverHealingMgr::OVERHEALING_DECAY_INTERVAL = 1.0f;
const float gOverHealingMgr::OVERHEALING_DECAY_AMOUNT = 1.0f;
const char* gOverHealingMgr::OVERHEALING_DECAY_SFX = "leadermode/click_01.wav";
const float gOverHealingMgr::OVERHEALING_MAX_RATIO = 2.0f;

void gOverHealingMgr::Think(CBasePlayer* pPlayer)
{
	if (pPlayer->m_flOHNextThink <= 0.0f)
		return;

	if (pPlayer->m_flOHNextThink > gpGlobals->time)
		return;

	// GLITCH: what if someone hurting so bad, but overhealing again within less than 1 sec?
	if (int(pPlayer->pev->health) <= int(pPlayer->m_flOHOriginalHealth))	// use int to compare would be more accurate
	{
		pPlayer->m_flOHNextThink = 0;
		pPlayer->m_flOHOriginalHealth = pPlayer->pev->max_health;
		return;
	}

	pPlayer->m_flOHNextThink = gpGlobals->time + OVERHEALING_DECAY_INTERVAL;
	pPlayer->pev->health -= OVERHEALING_DECAY_AMOUNT;

	UTIL_PlayEarSound(pPlayer, OVERHEALING_DECAY_SFX);
}

////////////
// FROZEN //
////////////

const char* gFrozenDOTMgr::ICEGRE_NOVA_SFX = "weapons/frostnova.wav";
const char* gFrozenDOTMgr::ICEGRE_FLESH_SFX = "weapons/impalehit.wav";
const char* gFrozenDOTMgr::ICEGRE_BREAKOUT_SFX = "weapons/impalelaunch1.wav";
const int gFrozenDOTMgr::ICE_BLOOD_COLOUR = 45;

void gFrozenDOTMgr::Set(CBasePlayer* pPlayer, int iDamage, entvars_t* pevInflictor, entvars_t* pevAttacker, float flFreezeTime)
{
	if (pPlayer->m_flFrozenNextThink <= 0)	// not forzen before.
	{
		pPlayer->m_flFrozenFrame = pPlayer->pev->frame;
		pPlayer->m_vecFrozenAngles = pPlayer->pev->angles;
		pPlayer->m_vecFrozenVAngle = pPlayer->pev->v_angle;

		CIceCube::Create(pPlayer);
	}

	if (iDamage > 0)
		pPlayer->TakeDamage(pevInflictor, pevAttacker, iDamage, DMG_FREEZE);

	UTIL_NvgScreen(pPlayer, 0, 50, 200, 100);
	EMIT_SOUND(pPlayer->edict(), CHAN_AUTO, ICEGRE_FLESH_SFX, VOL_NORM, ATTN_NORM);

	pPlayer->m_flFrozenNextThink = gpGlobals->time + flFreezeTime;
	pPlayer->pev->flags |= FL_FROZEN;
	pPlayer->m_flNextAttack = gpGlobals->time + flFreezeTime;
	pPlayer->m_bloodColor = ICE_BLOOD_COLOUR;
}

void gFrozenDOTMgr::Think(CBasePlayer* pPlayer)
{
	if (pPlayer->m_flFrozenNextThink <= 0.0f)
		return;

	if (pPlayer->m_flFrozenNextThink <= gpGlobals->time)
	{
		Free(pPlayer);
		return;
	}

	pPlayer->pev->angles = pPlayer->m_vecFrozenAngles;
	pPlayer->pev->frame = pPlayer->m_flFrozenFrame;
	pPlayer->pev->v_angle = pPlayer->m_vecFrozenVAngle;
	pPlayer->pev->fixangle = 1;
	pPlayer->pev->framerate = 0;
	pPlayer->pev->velocity.x = 0;
	pPlayer->pev->velocity.y = 0;
	pPlayer->pev->velocity.z -= 386.09f * gpGlobals->frametime;	// gravity acceleration
}

void gFrozenDOTMgr::Free(CBasePlayer* pPlayer)
{
	pPlayer->m_bloodColor = BLOOD_COLOR_RED;
	pPlayer->m_flNextAttack = 0;
	pPlayer->pev->flags &= ~FL_FROZEN;
	pPlayer->pev->framerate = 1.0f;
	pPlayer->pev->fixangle = 0;

	if (pPlayer->IsAlive())
		UTIL_ScreenFade(pPlayer, Vector(0, 50, 200), 0.9f, 0.1f, 100, FFADE_IN);

	EMIT_SOUND(pPlayer->edict(), CHAN_AUTO, ICEGRE_BREAKOUT_SFX, VOL_NORM, ATTN_NONE);

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pPlayer->pev->origin);
	WRITE_BYTE(TE_BREAKMODEL);
	WRITE_COORD(pPlayer->pev->origin[0]);
	WRITE_COORD(pPlayer->pev->origin[1]);
	WRITE_COORD(pPlayer->pev->origin[2]);
	WRITE_COORD(1.5f);
	WRITE_COORD(1.5f);
	WRITE_COORD(1.5f);
	WRITE_COORD(0);	// velocity.x
	WRITE_COORD(0);	// velocity.y
	WRITE_COORD(50.0f);	// velocity.z
	WRITE_BYTE(20);
	WRITE_SHORT(MODEL_INDEX("models/glassgibs.mdl"));
	WRITE_BYTE(12);
	WRITE_BYTE(25);
	WRITE_BYTE(0x01);
	MESSAGE_END();

	pPlayer->m_flFrozenNextThink = 0.0f;
}
