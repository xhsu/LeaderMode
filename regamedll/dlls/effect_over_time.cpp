/*

Created date: 03/03/2020

*/

#include "precompiled.h"


/////////////////
// OVERHEALING //
/////////////////

LINK_ENTITY_TO_CLASS(healing_smoke_centre, CHealingSmokeCenter)

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

		if ((pPlayer->m_iRoleType == Role_Assassin || pPlayer->m_iRoleType == Role_LeadEnforcer) && pPlayer->IsUsingPrimarySkill())
			pPlayer->DischargePrimarySkill(m_pPlayer);

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

	if (pPlayer->m_flElectrifyTimeUp > 0.0f)	// stop the electricity then we can freeze.
		gElectrifiedDOTMgr::Free(pPlayer);

	if (pPlayer->m_flBurningTimeUp > 0.0f)	// quench the flames on him.
		gBurningDOTMgr::Free(pPlayer);

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

/////////////////
// ELECTRIFIED //
/////////////////

const float gElectrifiedDOTMgr::WALK_SPEED_LIMIT = 125.0f;

void gElectrifiedDOTMgr::Set(CBasePlayer* pPlayer, float flTime, Vector vecShotPlace)
{
	if (pPlayer->m_flFrozenNextThink > 0.0)	// you can't electrify some frozen guy.
		return;

	if (pPlayer->m_flElectrifyTimeUp <= 0.0)
	{
		pPlayer->m_flElectrifyStarts = gpGlobals->time;	// this value is not reset even if re-electrified.
		pPlayer->m_bElectrifySFXPlayed = false;
	}

	pPlayer->m_flElectrifyTimeUp = gpGlobals->time + flTime;
	pPlayer->m_vecElectrifyBulletOFS = vecShotPlace - pPlayer->pev->origin;
}

void gElectrifiedDOTMgr::Think(CBasePlayer* pPlayer)
{
	if (pPlayer->m_flElectrifyTimeUp <= 0.0)	// not electrified.
		return;

	if (pPlayer->m_flElectrifyTimeUp < gpGlobals->time)	// de-electrify
	{
		pPlayer->ResetMaxSpeed();
		pPlayer->m_flElectrifyTimeUp = -1.0f;

		STOP_SOUND(pPlayer->edict(), CHAN_STATIC, CSkillTaserGun::ELECTRIFY_SFX);
		UTIL_ScreenFade(pPlayer, Vector(RANDOM_LONG(0, 255), RANDOM_LONG(0, 255), RANDOM_LONG(0, 255)), 0.3f, 0.2f, RANDOM_LONG(60, 100), FFADE_IN);

		return;
	}

	if (!pPlayer->m_bElectrifySFXPlayed)
	{
		pPlayer->m_bElectrifySFXPlayed = true;
		EMIT_SOUND(pPlayer->edict(), CHAN_STATIC, CSkillTaserGun::ELECTRIFY_SFX, VOL_NORM, ATTN_STATIC);
	}

	if (pPlayer->m_flElectrifyIcon < gpGlobals->time)
	{
		MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, gmsgDamage, g_vecZero, pPlayer->pev);
		WRITE_BYTE(0); // damage save
		WRITE_BYTE(0); // damage take
		WRITE_LONG(DMG_SHOCK); // damage type
		WRITE_COORD(0); // x
		WRITE_COORD(0); // y
		WRITE_COORD(0); // z
		MESSAGE_END();

		pPlayer->m_flElectrifyIcon = gpGlobals->time + 3.75f;
	}

	if (pPlayer->m_flElectrifyNextVFX < gpGlobals->time)
	{
		VFX(pPlayer);
		pPlayer->m_flElectrifyNextVFX = gpGlobals->time + RANDOM_FLOAT(0.1f, 0.5f);
	}

	if (pPlayer->m_flElectrifyNextScreenFade < gpGlobals->time)
	{
		UTIL_NvgScreen(pPlayer, RANDOM_LONG(0, 255), RANDOM_LONG(0, 255), RANDOM_LONG(0, 255), RANDOM_LONG(64, 128));
		pPlayer->m_flElectrifyNextScreenFade = gpGlobals->time + RANDOM_FLOAT(1.5, 3.0);
	}

	if (pPlayer->m_flElectrifyNextPunch < gpGlobals->time)
	{
		pPlayer->pev->punchangle = Vector(RANDOM_LONG(-20, 20), RANDOM_LONG(-20, 20), RANDOM_LONG(-20, 20));

		MESSAGE_BEGIN(MSG_ONE, gmsgShake, g_vecZero, pPlayer->edict());
		WRITE_SHORT(FixedUnsigned16(32.0f, 1 << 12));	// atu
		WRITE_SHORT(FixedUnsigned16(0.5f, 1 << 12));	// freq
		WRITE_SHORT(FixedUnsigned16(5.0f, 1 << 12));	// dur
		MESSAGE_END();

		pPlayer->m_flElectrifyNextPunch = gpGlobals->time + RANDOM_FLOAT(0.6f, 1.2f);
	}

	if (gpGlobals->time - pPlayer->m_flElectrifyStarts > 6.0f)	// drop primary weapon.
	{
		if (pPlayer->m_rgpPlayerItems[PRIMARY_WEAPON_SLOT])
		{
			pPlayer->DropPlayerItem(pPlayer->m_rgpPlayerItems[PRIMARY_WEAPON_SLOT]->m_iId);
		}
	}

	// random shoot
	if (!RANDOM_LONG(0, 5))
		pPlayer->pev->button |= IN_ATTACK;

	// random don't shoot.
	if (!RANDOM_LONG(0, 2))
		pPlayer->pev->button &= ~IN_ATTACK;

	SET_CLIENT_MAXSPEED(pPlayer->edict(), WALK_SPEED_LIMIT);
}

void gElectrifiedDOTMgr::Free(CBasePlayer* pPlayer)
{
	pPlayer->m_flElectrifyTimeUp = Q_min(pPlayer->m_flElectrifyTimeUp, 1.0f);	// if it was 0, never set it back to 1.
}

void gElectrifiedDOTMgr::VFX(CBasePlayer* pPlayer)
{
	Vector vecOrigin = pPlayer->pev->origin + pPlayer->m_vecElectrifyBulletOFS;

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecOrigin);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(vecOrigin[0]);
	WRITE_COORD(vecOrigin[1]);
	WRITE_COORD(vecOrigin[2]);
	WRITE_BYTE(20);		//range
	WRITE_BYTE(160);
	WRITE_BYTE(250);
	WRITE_BYTE(250);
	WRITE_BYTE(1);		//time
	WRITE_BYTE(0);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecOrigin);
	WRITE_BYTE(TE_SPARKS);
	WRITE_COORD(vecOrigin[0]);
	WRITE_COORD(vecOrigin[1]);
	WRITE_COORD(vecOrigin[2]);
	MESSAGE_END();

	vecOrigin[0] += RANDOM_FLOAT(-32.0, 32.0);
	vecOrigin[1] += RANDOM_FLOAT(-32.0, 32.0);
	vecOrigin[2] += RANDOM_FLOAT(-36.0, 36.0);

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecOrigin);
	WRITE_BYTE(TE_SPARKS);
	WRITE_COORD(vecOrigin[0]);
	WRITE_COORD(vecOrigin[1]);
	WRITE_COORD(vecOrigin[2]);
	MESSAGE_END();

	UTIL_BeamEntPoint(pPlayer->entindex(), vecOrigin, MODEL_INDEX("sprites/lgtning.spr"), 0, 100, 1, 31, 125, 160, 250, 250, 255, RANDOM_LONG(20, 30));
}

//////////////
// POISONED //
//////////////

LINK_ENTITY_TO_CLASS(poisoned_smoke_centre, CPoisonedSmokeCentre)

const char* CPoisonedSmokeCentre::COUGH_SFX[6] =
{
	"leadermode/cough1.wav",
	"leadermode/cough2.wav",
	"leadermode/cough3.wav",
	"leadermode/cough4.wav",
	"leadermode/cough5.wav",
	"leadermode/cough6.wav",
};
const char* CPoisonedSmokeCentre::BREATHE_SFX = "leadermode/breathe1.wav";
const float CPoisonedSmokeCentre::DURATION = 5.0f;
const Vector CPoisonedSmokeCentre::POISON_COLOUR = Vector(128, 255, 128);

CPoisonedSmokeCentre* CPoisonedSmokeCentre::Create(Vector vecOrigin, CBasePlayer* pAttacker, float flRadius, float flTimeRemoved)
{
	CPoisonedSmokeCentre* pEntity = GetClassPtr((CPoisonedSmokeCentre*)nullptr);
	SET_ORIGIN(pEntity->edict(), vecOrigin);
	pEntity->pev->nextthink = gpGlobals->time + 0.01f;
	pEntity->m_pPlayer = pAttacker;
	pEntity->m_flTimeRemoved = gpGlobals->time + flTimeRemoved;
	pEntity->m_flRadius = flRadius;

	MESSAGE_BEGIN(MSG_ALL, SVC_TEMPENTITY);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(vecOrigin[0]);
	WRITE_COORD(vecOrigin[1]);
	WRITE_COORD(vecOrigin[2]);
	WRITE_BYTE(int(flRadius / 10.0f));		// range: 275 ?
	WRITE_BYTE(POISON_COLOUR.x);
	WRITE_BYTE(POISON_COLOUR.y);
	WRITE_BYTE(POISON_COLOUR.z);
	WRITE_BYTE(int(flTimeRemoved * 10.0f));	// time: original CS smokegrenade lasts 25 sec.
	WRITE_BYTE(0);
	MESSAGE_END();

	return pEntity;
}

void CPoisonedSmokeCentre::Precache()
{
	PRECACHE_SOUND(BREATHE_SFX);
	PRECACHE_SOUND_ARRAY(COUGH_SFX);
}

void CPoisonedSmokeCentre::Think()
{
	if (m_flTimeRemoved < gpGlobals->time)
	{
		pev->flags |= FL_KILLME;
		return;
	}

	CBasePlayer* pPlayer = nullptr;
	while ((pPlayer = UTIL_FindEntityInSphere(pPlayer, pev->origin, m_flRadius)))
	{
		if (FNullEnt(pPlayer) || !pPlayer->IsPlayer())
			continue;

		TraceResult tr;
		UTIL_TraceLine(pev->origin, pPlayer->pev->origin, ignore_monsters, ignore_glass, pPlayer->edict(), &tr);
		if (tr.flFraction < 1.0f)	// not visible.
			continue;

		if (pPlayer->m_iRoleType == Role_Assassin && pPlayer->IsUsingPrimarySkill())
			pPlayer->DischargePrimarySkill(m_pPlayer);

		gPoisonDOTMgr::Set(pPlayer, m_pPlayer, DURATION);
	}

	pev->nextthink = gpGlobals->time + 0.1f;
}

extern const float gPoisonDOTMgr::MAX_DAMAGE = 7.0f;
extern const float gPoisonDOTMgr::DAMAGE_INTERVAL = 1.0f;

void gPoisonDOTMgr::Set(CBasePlayer* pPlayer, CBasePlayer* pAttacker, float flTime)
{
	pPlayer->m_pPoisonedBy = pAttacker;
	pPlayer->m_flPoisonedTimeUp = gpGlobals->time + flTime;
}

void gPoisonDOTMgr::Think(CBasePlayer* pPlayer)
{
	if (pPlayer->m_flPoisonedBreathSFXStop > 0.0 && pPlayer->m_flPoisonedBreathSFXStop < gpGlobals->time)
	{
		STOP_SOUND(pPlayer->edict(), CHAN_STATIC, CPoisonedSmokeCentre::BREATHE_SFX);
		pPlayer->m_flPoisonedBreathSFXStop = 0;
		return;
	}

	if (pPlayer->m_flPoisonedTimeUp <= 0.0)	// not poisoned.
		return;

	if (pPlayer->m_flPoisonedTimeUp < gpGlobals->time)	// detoxify
	{
		pPlayer->m_bPoisonedScreenFadeIn = false;
		pPlayer->m_flPoisonedTimeUp = 0.0;
		pPlayer->m_flPoisonedBreathSFXStop = gpGlobals->time + 6.0f;
		EMIT_SOUND(pPlayer->edict(), CHAN_STATIC, CPoisonedSmokeCentre::BREATHE_SFX, VOL_NORM, ATTN_STATIC);
		return;
	}

	if (pPlayer->m_flPoisonedNextFade < gpGlobals->time)
	{
		UTIL_ScreenFade(pPlayer, CPoisonedSmokeCentre::POISON_COLOUR, 0.6f, 0.4f, 150, pPlayer->m_bPoisonedScreenFadeIn ? FFADE_IN : FFADE_OUT);

		pPlayer->m_bPoisonedScreenFadeIn = !pPlayer->m_bPoisonedScreenFadeIn;
		pPlayer->m_flPoisonedNextFade = gpGlobals->time + 1.0f;
	}

	if (pPlayer->m_flPoisonedNextCoughSFX < gpGlobals->time)
	{
		EMIT_SOUND(pPlayer->edict(), CHAN_AUTO, RANDOM_ARRAY(CPoisonedSmokeCentre::COUGH_SFX), VOL_NORM, ATTN_STATIC);
		pPlayer->m_flPoisonedNextCoughSFX = gpGlobals->time + RANDOM_FLOAT(2.5f, 4.0f);
	}

	if (pPlayer->m_flPoisonedNextDamage < gpGlobals->time)
	{
		pPlayer->TakeDamage(pPlayer->m_pPoisonedBy.IsValid() ? pPlayer->m_pPoisonedBy->pev : pPlayer->pev,
							pPlayer->m_pPoisonedBy.IsValid() ? pPlayer->m_pPoisonedBy->pev : pPlayer->pev,
							RANDOM_FLOAT(MAX_DAMAGE / 2.0f, MAX_DAMAGE),
							DMG_NERVEGAS | DMG_NEVERGIB);

		pPlayer->m_flPoisonedNextDamage = gpGlobals->time + DAMAGE_INTERVAL;
	}
}

void gPoisonDOTMgr::Free(CBasePlayer* pPlayer)
{
	pPlayer->m_flPoisonedTimeUp = Q_min(pPlayer->m_flPoisonedTimeUp, 1.0f);	// if it was already 0, never set it back to 1.
}

/////////////
// BURNING //
/////////////

LINK_ENTITY_TO_CLASS(incendiary_grenade_centre, CIncendiaryGrenadeCentre)

const char* CIncendiaryGrenadeCentre::DETONATE = "leadermode/molotov_detonate_3.wav";
const char* CIncendiaryGrenadeCentre::FIRE_SFX_LOOP = "leadermode/fire_loop_1.wav";
const char* CIncendiaryGrenadeCentre::FIRE_SFX_FADEOUT = "leadermode/fire_loop_fadeout_01.wav";
const char* CIncendiaryGrenadeCentre::SCREAMS[5] = { "leadermode/burning_scream_01.wav", "leadermode/burning_scream_02.wav", "leadermode/burning_scream_03.wav", "leadermode/burning_scream_04.wav", "leadermode/burning_scream_05.wav" };
const float CIncendiaryGrenadeCentre::DAMAGE = 15.0f;
const float CIncendiaryGrenadeCentre::INTERVAL = 0.7f;
int CIncendiaryGrenadeCentre::FLAME_SPR = 0;

CIncendiaryGrenadeCentre* CIncendiaryGrenadeCentre::Create(Vector vecOrigin, CBasePlayer* pAttacker, float flRadius, float flTimeRemoved)
{
	CIncendiaryGrenadeCentre* pEntity = GetClassPtr((CIncendiaryGrenadeCentre*)nullptr);
	SET_ORIGIN(pEntity->edict(), vecOrigin);
	pEntity->m_pPlayer = pAttacker;
	pEntity->m_flRadius = flRadius;
	pEntity->m_flTimeRemoved = gpGlobals->time + flTimeRemoved + 10.0f;	// a buffer for SFX.
	pEntity->pev->rendermode = kRenderTransAlpha;
	pEntity->pev->renderamt = 0;
	pEntity->pev->solid = SOLID_NOT;
	pEntity->pev->movetype = MOVETYPE_NONE;
	pEntity->pev->nextthink = gpGlobals->time + INTERVAL;

	EMIT_SOUND(pEntity->edict(), CHAN_AUTO, DETONATE, VOL_NORM, ATTN_NORM);

	CBasePlayer* pPlayer = nullptr;
	while ((pPlayer = UTIL_FindEntityInSphere(pPlayer, pEntity->pev->origin, pEntity->m_flRadius)))
	{
		if (FNullEnt(pPlayer) || !pPlayer->IsPlayer())
			continue;

		if (pPlayer->pev->takedamage == DAMAGE_NO)
			continue;

		TraceResult tr;
		UTIL_TraceLine(pEntity->pev->origin, pPlayer->pev->origin, ignore_monsters, ignore_glass, pPlayer->edict(), &tr);
		if (tr.flFraction < 1.0f)	// not visible.
			continue;

		gBurningDOTMgr::Set(pPlayer, pAttacker, flTimeRemoved / 2.0f);	// it would be an extremely long burning if you are inside.
	}

	return nullptr;
}

void CIncendiaryGrenadeCentre::Precache()
{
	PRECACHE_SOUND(DETONATE);
	PRECACHE_SOUND(FIRE_SFX_LOOP);
	PRECACHE_SOUND(FIRE_SFX_FADEOUT);
	PRECACHE_SOUND_ARRAY(SCREAMS);

	FLAME_SPR = PRECACHE_MODEL("sprites/leadermode/flame.spr");
}

void CIncendiaryGrenadeCentre::Think()
{
	if (m_flTimeRemoved - gpGlobals->time <= 10.0f)
	{
		STOP_SOUND(edict(), CHAN_WEAPON, FIRE_SFX_LOOP);
		EMIT_SOUND(edict(), CHAN_ITEM, FIRE_SFX_FADEOUT, VOL_NORM, ATTN_NORM);

		pev->flags |= FL_KILLME;
		return;
	}

	pev->nextthink = gpGlobals->time + 0.01f;

	if (m_flNextDamageCheck <= gpGlobals->time)
	{
		vec_t flHighterZ = 0;
		Vector vecCentre;
		TraceResult tr;
		CBaseEntity* pEntity = nullptr;

		while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, m_flRadius)))
		{
			if (FNullEnt(pEntity))
				continue;

			if (pEntity->pev->takedamage == DAMAGE_NO)
				continue;

			vecCentre = pEntity->Center();
			flHighterZ = Q_max(vecCentre.z, pev->origin.z);	// test visibility at the same height.

			UTIL_TraceLine(Vector(vecCentre.x, vecCentre.y, flHighterZ), Vector(pev->origin.x, pev->origin.y, flHighterZ), ignore_monsters, ignore_glass, pEntity->edict(), &tr);
			if (tr.flFraction < 1.0f)	// not visible.
				continue;

			if (pEntity->IsPlayer())
			{
				CBasePlayer* pPlayer = CBasePlayer::Instance(pEntity->pev);

				if (pPlayer->m_flFrozenNextThink > 0.0f)	// melt the ice.
					gFrozenDOTMgr::Free(pPlayer);

				if (pPlayer->m_iRoleType == Role_Assassin && pPlayer->IsUsingPrimarySkill())
					pPlayer->DischargePrimarySkill(m_pPlayer);

				gBurningDOTMgr::Scream(pPlayer);	// scream SFX CD is included in function.
			}

			float flDamage = Q_max(float(DAMAGE * ((m_flRadius - (pev->origin - pEntity->pev->origin).Length()) / m_flRadius)), 0.0f);

			if (flDamage <= 1.0)
				continue;

			pEntity->TakeDamage(pev, m_pPlayer.IsValid() ? m_pPlayer->pev : &INDEXENT(0)->v, flDamage, DMG_SLOWBURN);
		}

		m_flNextDamageCheck = gpGlobals->time + INTERVAL;
	}

	if (m_flNextFireLoopSFX <= gpGlobals->time)
	{
		EMIT_SOUND(edict(), CHAN_WEAPON, FIRE_SFX_LOOP, VOL_NORM, ATTN_NORM);
		m_flNextFireLoopSFX = gpGlobals->time + 4.0f;
	}

	if (m_flNextFlameSpr <= gpGlobals->time)
	{
		m_flNextFlameSpr = gpGlobals->time + 0.1f;

#ifndef CLIENT_VFX
		Vector vecOrigin;
		edict_t* pEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

		for (int i = 0; i < int(m_flRadius / 40.0f); i++)
		{
			vecOrigin = pev->origin;
			vecOrigin.x += RANDOM_FLOAT(-m_flRadius / 2.0f, m_flRadius / 2.0f);
			vecOrigin.y += RANDOM_FLOAT(-m_flRadius / 2.0f, m_flRadius / 2.0f);

			if (POINT_CONTENTS(vecOrigin) != CONTENTS_EMPTY)
				vecOrigin[2] += (vecOrigin - pev->origin).Length2D();

			pEntity->v.origin = vecOrigin;
			DROP_TO_FLOOR(pEntity);
			vecOrigin = pEntity->v.origin;

			if (POINT_CONTENTS(vecOrigin) != CONTENTS_EMPTY)
				continue;

			TraceResult tr;
			UTIL_TraceLine(pEntity->v.origin, vecOrigin, ignore_monsters, ignore_glass, pEntity, &tr);
			if (tr.flFraction < 1.0f)	// not visible.
				continue;

			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecOrigin);
			WRITE_BYTE(TE_SPRITE);
			WRITE_COORD(vecOrigin[0]);
			WRITE_COORD(vecOrigin[1]);
			WRITE_COORD(vecOrigin[2] + RANDOM_FLOAT(75.0f, 95.0f));
			WRITE_SHORT(FLAME_SPR);
			WRITE_BYTE(RANDOM_LONG(9, 11));
			WRITE_BYTE(100);
			MESSAGE_END();
		}

		pEntity->v.flags |= FL_KILLME;
#endif
	}
}

const float gBurningDOTMgr::MAX_DAMAGE = 14.0f;
const float gBurningDOTMgr::DAMAGE_INTERVAL = 0.5f;

void gBurningDOTMgr::Set(CBasePlayer* pPlayer, CBasePlayer* pAttacker, float flTime)
{
	pPlayer->m_pIgnitedBy = pAttacker;
	pPlayer->m_flBurningTimeUp = gpGlobals->time + flTime;

	UTIL_NvgScreen(pPlayer, 255, 117, 26, 40);
}

void gBurningDOTMgr::Think(CBasePlayer* pPlayer)
{
	if (pPlayer->m_flBurningTimeUp <= 0.0f)	// not burning.
		return;

	if (pPlayer->m_flFrozenNextThink > 0.0)	// melt
		gFrozenDOTMgr::Free(pPlayer);

	if (pPlayer->m_flBurningTimeUp < gpGlobals->time)	// quench.
	{
		UTIL_ScreenFade(pPlayer, Vector(255, 117, 26), 0.3f, 0.2f, 40, FFADE_IN);
		pPlayer->ResetMaxSpeed();
		pPlayer->m_flBurningTimeUp = -1.0f;
		STOP_SOUND(pPlayer->edict(), CHAN_STATIC, CIncendiaryGrenadeCentre::FIRE_SFX_LOOP);
		EMIT_SOUND(pPlayer->edict(), CHAN_AUTO, CIncendiaryGrenadeCentre::FIRE_SFX_FADEOUT, VOL_NORM, ATTN_STATIC);

		return;
	}

	if (pPlayer->m_flBurningNextDamage < gpGlobals->time)
	{
		pPlayer->TakeDamage(&INDEXENT(0)->v, pPlayer->m_pIgnitedBy.IsValid() ? pPlayer->m_pIgnitedBy->pev : &INDEXENT(0)->v, RANDOM_FLOAT(MAX_DAMAGE / 2.0F, MAX_DAMAGE), DMG_BURN | DMG_NEVERGIB);

		pPlayer->m_flBurningNextDamage = gpGlobals->time + DAMAGE_INTERVAL;
	}

	if (pPlayer->m_flBurningFlameThink < gpGlobals->time)
	{
		Vector vecOrigin = pPlayer->pev->origin + Vector(RANDOM_FLOAT(-16.0f, 16.0f), RANDOM_FLOAT(-16.0f, 16.0f), RANDOM_FLOAT(-36.0f, 36.0f));

		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecOrigin);
		WRITE_BYTE(TE_SPRITE);
		WRITE_COORD(vecOrigin[0]);
		WRITE_COORD(vecOrigin[1]);
		WRITE_COORD(vecOrigin[2]);
		WRITE_SHORT(CIncendiaryGrenadeCentre::FLAME_SPR);
		WRITE_BYTE(RANDOM_LONG(9, 11));
		WRITE_BYTE(100);
		MESSAGE_END();

		pPlayer->m_flBurningFlameThink = gpGlobals->time + 0.1f;
	}

	if (pPlayer->m_flBurningNextScream < gpGlobals->time)
		Scream(pPlayer);

	if (pPlayer->m_flBurningSFX < gpGlobals->time)
	{
		EMIT_SOUND(pPlayer->edict(), CHAN_STATIC, CIncendiaryGrenadeCentre::FIRE_SFX_LOOP, VOL_NORM, ATTN_STATIC);
		pPlayer->m_flBurningSFX = gpGlobals->time + 4.0f;
	}
}

void gBurningDOTMgr::Free(CBasePlayer* pPlayer)
{
	pPlayer->m_flBurningTimeUp = Q_min(pPlayer->m_flBurningTimeUp, 1.0f);	// if it was already 0, never set it back to 1.
}

void gBurningDOTMgr::Scream(CBasePlayer* pPlayer)
{
	if (pPlayer->m_flBurningNextScream >= gpGlobals->time)
		return;

	EMIT_SOUND(pPlayer->edict(), CHAN_AUTO, RANDOM_ARRAY(CIncendiaryGrenadeCentre::SCREAMS), VOL_NORM, ATTN_NORM);
	pPlayer->m_flBurningNextScream = gpGlobals->time + RANDOM_FLOAT(3.0f, 3.5f);
}
