#include "precompiled.h"

int g_iSpriteGroundexp1 = 0;
int g_iSpriteZerogxplode2 = 0;

TYPEDESCRIPTION CGrenade::m_SaveData[] =
{
	DEFINE_FIELD(CGrenade, m_iType, FIELD_INTEGER),
	DEFINE_FIELD(CGrenade, m_SGSmoke, FIELD_INTEGER),
	DEFINE_FIELD(CGrenade, m_bLightSmoke, FIELD_BOOLEAN),
	DEFINE_FIELD(CGrenade, m_usEvent, FIELD_INTEGER),
};

unsigned short CGrenade::m_rgusEvents[EQP_COUNT];

const float CGrenade::m_rgflFuseTime[EQP_COUNT] =
{
	0,
	0,
	0,

	1.5f,	// EQP_HEGRENADE,
	1.5f,	// EQP_FLASHBANG,
	1.5f,	// EQP_SMOKEGRENADE,
	9999.0f,	// EQP_CRYOGRENADE, explode on touch.
	9999.0f,	// EQP_INCENDIARY_GR, explode on touch.
	1.5f,	// EQP_HEALING_GR,
	1.5f,	// EQP_GAS_GR,
	9999.0f,	// EQP_C4, explode on execute.

	0,
};

LINK_ENTITY_TO_CLASS(grenade, CGrenade)

void CGrenade::Explode(Vector vecSrc, Vector vecAim)
{
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + Vector(0, 0, -32), ignore_monsters, ENT(pev), &tr);
	Explode(&tr, DMG_BLAST);
}

// UNDONE: temporary scorching for PreAlpha - find a less sleazy permenant solution.
void CGrenade::Explode(TraceResult *pTrace, int bitsDamageType)
{
	pev->model = iStringNull; // invisible
	pev->solid = SOLID_NOT;   // intangible
	pev->takedamage = DAMAGE_NO;

	// Pull out of the wall a bit
	if (pTrace->flFraction != 1.0f)
	{
		pev->origin = pTrace->vecEndPos + (pTrace->vecPlaneNormal * (pev->dmg - 24.0f) * 0.6f);
	}

	int iContents = UTIL_PointContents(pev->origin);
	entvars_t *pevOwner = VARS(pev->owner);

	if (TheBots)
	{
		TheBots->OnEvent(EVENT_FLASHBANG_GRENADE_EXPLODED, CBaseEntity::Instance(pev->owner), (CBaseEntity *)&pev->origin);
	}

	// can't traceline attack owner if this is set
	pev->owner = nullptr;

	RadiusFlash(pev->origin, pev, pevOwner, 4, CLASS_NONE, bitsDamageType);

	UTIL_DecalTrace(pTrace, RANDOM_LONG(0, 1) ? DECAL_SCORCH1 : DECAL_SCORCH2);

	switch (RANDOM_LONG(0, 1))
	{
	case 0: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/flashbang-2.wav", 0.55, ATTN_NORM); break;
	default: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/flashbang-1.wav", 0.55, ATTN_NORM); break;
	}

	pev->effects |= EF_NODRAW;
	SetThink(&CGrenade::Smoke);
	pev->velocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.3f;

	// VFX of the flash light.
	MESSAGE_BEGIN(MSG_ALL, SVC_TEMPENTITY, pev->origin);
	WRITE_BYTE(TE_DLIGHT); // TE_DLIGHT
	WRITE_COORD(pev->origin.x); // x
	WRITE_COORD(pev->origin.y); // y
	WRITE_COORD(pev->origin.z); // z
	WRITE_BYTE(50);	// radius
	WRITE_BYTE(255);	// r
	WRITE_BYTE(255);	// g
	WRITE_BYTE(255);	// b
	WRITE_BYTE(8);	// life
	WRITE_BYTE(60);	// decay rate
	MESSAGE_END();

	// draw sparks
	if (iContents != CONTENTS_WATER)
	{
		int sparkCount = RANDOM_LONG(0, 3);
		for (int i = 0; i < sparkCount; i++)
		{
			Create("spark_shower", pev->origin, pTrace->vecPlaneNormal, nullptr);
		}
	}
}

void CGrenade::Explode3(TraceResult *pTrace, int bitsDamageType)
{
	pev->model = iStringNull; // invisible
	pev->solid = SOLID_NOT;   // intangible
	pev->takedamage = DAMAGE_NO;

	if (pTrace->flFraction != 1.0f)
	{
		pev->origin = pTrace->vecEndPos + (pTrace->vecPlaneNormal * (pev->dmg - 24.0f) * 0.6f);
	}

	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_EXPLOSION);	// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD(pev->origin.x);		// Send to PAS because of the sound
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z + 20.0f);
		WRITE_SHORT(g_sModelIndexFireball3);
		WRITE_BYTE(25);			// scale * 10
		WRITE_BYTE(30);		// framerate
		WRITE_BYTE(TE_EXPLFLAG_NONE);	// flags
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
		WRITE_BYTE(TE_EXPLOSION);	// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD(pev->origin.x + RANDOM_FLOAT(-64, 64));	// Send to PAS because of the sound
		WRITE_COORD(pev->origin.y + RANDOM_FLOAT(-64, 64));
		WRITE_COORD(pev->origin.z + RANDOM_FLOAT(30, 35));
		WRITE_SHORT(g_sModelIndexFireball2);
		WRITE_BYTE(30);			// scale * 10
		WRITE_BYTE(30);		// framerate
		WRITE_BYTE(TE_EXPLFLAG_NONE);	// flags
	MESSAGE_END();

	entvars_t *pevOwner = VARS(pev->owner);

	if (TheBots)
	{
		TheBots->OnEvent(EVENT_HE_GRENADE_EXPLODED, CBaseEntity::Instance(pev->owner));
	}

	pev->owner = nullptr;
	RadiusDamage(pev, pevOwner, pev->dmg, CLASS_NONE, bitsDamageType);

	UTIL_DecalTrace(pTrace, RANDOM_LONG(0, 1) ? DECAL_SCORCH1 : DECAL_SCORCH2);

	switch (RANDOM_LONG(0, 2))
	{
	case 0: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris1.wav", 0.55, ATTN_NORM); break;
	case 1: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris2.wav", 0.55, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris3.wav", 0.55, ATTN_NORM); break;
	default: break;
	}

	pev->effects |= EF_NODRAW;
	SetThink(&CGrenade::Smoke3_C);
	pev->velocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.55f;

	int sparkCount = RANDOM_LONG(0, 3);
	for (int i = 0; i < sparkCount; i++)
	{
		Create("spark_shower", pev->origin, pTrace->vecPlaneNormal, nullptr);
	}

	// additional VFX...
	PLAYBACK_EVENT_FULL(0, nullptr, m_usEvent, 0, pev->origin, (float*)&g_vecZero, 0, 0, 0, 0, FALSE, FALSE);
}

void CGrenade::Smoke3_C()
{
	if (UTIL_PointContents(pev->origin) == CONTENTS_WATER)
	{
		UTIL_Bubbles(pev->origin - Vector(64, 64, 64), pev->origin + Vector(64, 64, 64), 100);
	}
	else
	{
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z - 5.0f);
			WRITE_SHORT(g_sModelIndexSmoke);
			WRITE_BYTE(35 + RANDOM_FLOAT(0, 10)); // scale * 10
			WRITE_BYTE(5); // framerate
		MESSAGE_END();
	}

	UTIL_Remove(this);
}

void CGrenade::Smoke3_B()
{
	if (UTIL_PointContents(pev->origin) == CONTENTS_WATER)
	{
		UTIL_Bubbles(pev->origin - Vector(64, 64, 64), pev->origin + Vector(64, 64, 64), 100);
	}
	else
	{
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(pev->origin.x + RANDOM_FLOAT(-128, 128));
			WRITE_COORD(pev->origin.y + RANDOM_FLOAT(-128, 128));
			WRITE_COORD(pev->origin.z + RANDOM_FLOAT(-10, 10));
			WRITE_SHORT(g_sModelIndexSmoke);
			WRITE_BYTE(15 + RANDOM_FLOAT(0, 10)); // scale * 10
			WRITE_BYTE(10); // framerate
		MESSAGE_END();
	}

	pev->nextthink = gpGlobals->time + 0.15f;
	SetThink(&CGrenade::Smoke3_A);
}

void CGrenade::Smoke3_A()
{
	if (UTIL_PointContents(pev->origin) == CONTENTS_WATER)
	{
		UTIL_Bubbles(pev->origin - Vector(64, 64, 64), pev->origin + Vector(64, 64, 64), 100);
	}
	else
	{
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(pev->origin.x + RANDOM_FLOAT(-128, 128));
			WRITE_COORD(pev->origin.y + RANDOM_FLOAT(-128, 128));
			WRITE_COORD(pev->origin.z + RANDOM_FLOAT(-10, 10));
			WRITE_SHORT(g_sModelIndexSmoke);
			WRITE_BYTE(15 + RANDOM_FLOAT(0, 10)); // scale * 10
			WRITE_BYTE(12); // framerate
		MESSAGE_END();
	}
}

void CGrenade::Smoke2()
{
	if (UTIL_PointContents(pev->origin) == CONTENTS_WATER)
	{
		UTIL_Bubbles(pev->origin - Vector(64, 64, 64), pev->origin + Vector(64, 64, 64), 100);
	}
	else
	{
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(g_sModelIndexSmoke);
			WRITE_BYTE(150); // scale * 10
			WRITE_BYTE(8); // framerate
		MESSAGE_END();
	}

	UTIL_Remove(this);
}

void CGrenade::Smoke()
{
	if (UTIL_PointContents(pev->origin) == CONTENTS_WATER)
	{
		UTIL_Bubbles(pev->origin - Vector(64, 64, 64), pev->origin + Vector(64, 64, 64), 100);
	}
	else
	{
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_SHORT(g_sModelIndexSmoke);
			WRITE_BYTE(25); // scale * 10
			WRITE_BYTE(6);  // framerate
		MESSAGE_END();
	}

	UTIL_Remove(this);
}

void CGrenade::SG_Smoke()
{
	int iMaxSmokePuffs = 100;

	if (UTIL_PointContents(pev->origin) == CONTENTS_WATER)
	{
		UTIL_Bubbles(pev->origin - Vector(64, 64, 64), pev->origin + Vector(64, 64, 64), 100);
	}
	else
	{
		Vector origin, angle;
		UTIL_MakeVectors(pev->angles);

		origin = gpGlobals->v_forward * RANDOM_FLOAT(3, 8);

		auto flSmokeInterval = RANDOM_FLOAT(1.5f, 3.5f) * iMaxSmokePuffs;

		auto R_angle = m_angle / (180.00433335 / M_PI);

		auto x_old = Q_cos(real_t(R_angle));
		auto y_old = Q_sin(real_t(R_angle));

		angle.x = origin.x * x_old - origin.y * y_old;
		angle.y = origin.x * y_old + origin.y * x_old;

		m_angle = (m_angle + 30) % 360;

		PLAYBACK_EVENT_FULL(0, nullptr, m_usEvent, 0, pev->origin, m_vSmokeDetonate, angle.x, angle.y, flSmokeInterval, 4, m_bLightSmoke, 6);
	}

	if (m_SGSmoke <= 20)
	{
		pev->nextthink = gpGlobals->time + 1.0f;
		SetThink(&CGrenade::SG_Smoke);
		m_SGSmoke++;
	}
	else
	{
		pev->effects |= EF_NODRAW;

		if (TheBots)
		{
			TheBots->RemoveGrenade(this);
		}
		UTIL_Remove(this);
	}
}

void CGrenade::Killed(entvars_t *pevAttacker, int iGib)
{
	Detonate();
}

void CGrenade::Detonate()
{
	TraceResult tr;
	Vector vecSpot; // trace starts here!

	vecSpot = pev->origin + Vector(0, 0, 8);
	UTIL_TraceLine(vecSpot, vecSpot + Vector(0, 0, -40), ignore_monsters, ENT(pev), &tr);
	Explode(&tr, DMG_BLAST);
}

void CGrenade::SG_Detonate()
{
	TraceResult tr;
	Vector vecSpot;
	edict_t *pentFind = nullptr;

	vecSpot = pev->origin + Vector(0, 0, 8);

	UTIL_TraceLine(vecSpot, vecSpot + Vector(0, 0, -40), ignore_monsters, ENT(pev), &tr);

	if (TheBots)
	{
		TheBots->OnEvent(EVENT_SMOKE_GRENADE_EXPLODED, CBaseEntity::Instance(pev->owner));
		TheBots->AddGrenade(EQP_SMOKEGRENADE, this);
	}

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/sg_explode.wav", VOL_NORM, ATTN_NORM);

	while ((pentFind = FIND_ENTITY_BY_CLASSNAME(pentFind, "grenade")))
	{
		if (FNullEnt(pentFind))
			break;

		CBaseEntity *pEnt = CBaseEntity::Instance(pentFind);
		if (pEnt)
		{
			float fDistance = (pEnt->pev->origin - pev->origin).Length();
			if (fDistance != 0.0f && fDistance <= 250.0f)
			{
				if (gpGlobals->time > pEnt->pev->dmgtime)
				{
					m_bLightSmoke = true;
				}
			}
		}
	}

	m_bDetonated = true;
	PLAYBACK_EVENT_FULL(0, nullptr, m_usEvent, 0, pev->origin, (float *)&g_vecZero, 0, 0, 0, 1, m_bLightSmoke, FALSE);
	m_vSmokeDetonate = pev->origin;

	pev->velocity.x = RANDOM_FLOAT(-175, 175);
	pev->velocity.y = RANDOM_FLOAT(-175, 175);
	pev->velocity.z = RANDOM_FLOAT(250, 350);

	pev->nextthink = gpGlobals->time + 0.1f;
	SetThink(&CGrenade::SG_Smoke);

	switch (m_iType)
	{
	case HEALING:
		CHealingSmokeCenter::Create(vecSpot, CBasePlayer::Instance(pev->owner));
		break;

	case NERVE_GAS:
		CPoisonedSmokeCentre::Create(vecSpot, CBasePlayer::Instance(pev->owner));
		break;

	default:
		break;
	}
}

void CGrenade::HE_Detonate()
{
	TraceResult tr;
	Vector vecSpot;// trace starts here!

	vecSpot = pev->origin + Vector(0, 0, 8);
	UTIL_TraceLine(vecSpot, vecSpot + Vector(0, 0, -40), ignore_monsters, ENT(pev), &tr);
	Explode3(&tr, DMG_EXPLOSION);
}

void CGrenade::C4_Detonate()
{
	CBasePlayer* pPlayer = CBasePlayer::Instance(pev->owner);
	CBaseEntity* pEntity = nullptr;

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, C4_EXPLO_RADIUS)))
	{
		if (pEntity == this)
			continue;

		if (pEntity->pev->takedamage == DAMAGE_NO)
			continue;

		if (FClassnameIs(pEntity->pev, "func_breakable"))
		{
			pEntity->TakeDamage(pev, pPlayer->pev, C4_EXPLO_DAMAGE, DMG_EXPLOSION);
			continue;
		}

		if (!friendlyfire.value && pEntity->IsPlayer())
		{
			CBasePlayer* pVictim = CBasePlayer::Instance(pEntity->pev);

			if (pVictim->m_iTeam == pPlayer->m_iTeam)
				continue;
		}

		float flDistance = (pev->origin - pEntity->pev->origin).Length();
		float flDamage = C4_EXPLO_DAMAGE * Q_pow(50.0f, -flDistance / C4_EXPLO_RADIUS);	// this math model makes sure that at this exact point, the damage goes down to 1/50 i.e. 2% of the original..

		if (pEntity->IsPlayer())
			flDamage *= GetAmountOfPlayerVisible(pev->origin, pEntity);

		if (flDamage <= 1.0f)
			continue;

		pEntity->TakeDamage(pev, pPlayer->pev, flDamage, DMG_EXPLOSION);
	}

	// LUNA: EFX(s) moved to client.
	PLAYBACK_EVENT_FULL(FEV_GLOBAL | FEV_RELIABLE, nullptr, m_usEvent, 0, pev->origin, m_vecAttachedSurfaceNorm, 0.0f, 0.0f, 0, 0, false, false);

	// But only this stays.
	UTIL_ScreenShake(pev->origin, 10, 10, 1.5, C4_EXPLO_RADIUS * 2);

	// LUNA: Why DSHGFHDS always likes to remove entity this way?
	pev->flags |= FL_KILLME;
}

void CGrenade::C4_Detonate(CBasePlayer* pPlayer)
{
	CGrenade* pC4 = nullptr;
	while ((pC4 = UTIL_FindEntityByClassname(pC4, "grenade")))
	{
		if (pC4->m_iType != RCC4)
			continue;

		if (pC4->pev->owner != pPlayer->edict())
			continue;

		pC4->C4_Detonate();
	}
}

void CGrenade::ExplodeTouch(CBaseEntity *pOther)
{
	// LUNA: the function of this function had been changed.
	// this is set when a grenade was thrown by a Breacher.

	pev->dmgtime = gpGlobals->time;
	pev->nextthink = gpGlobals->time;

	if (!Q_strcmp(STRING(pev->model), "models/w_smokegrenade.mdl"))
		pev->flags |= FL_ONGROUND;	// SG is a little bit different.
}

void CGrenade::BounceTouch(CBaseEntity *pOther)
{
	// don't hit the guy that launched this grenade
	if (pOther->edict() == pev->owner)
		return;

	if (FClassnameIs(pOther->pev, "func_breakable") && pOther->pev->rendermode != kRenderNormal)
	{
		pev->velocity = pev->velocity * -2.0f;
		return;
	}

	Vector vecTestVelocity;

	// this is my heuristic for modulating the grenade velocity because grenades dropped purely vertical
	// or thrown very far tend to slow down too quickly for me to always catch just by testing velocity.
	// trimming the Z velocity a bit seems to help quite a bit.
	vecTestVelocity = pev->velocity;
	vecTestVelocity.z *= 0.7f;

	if (pev->flags & FL_ONGROUND)
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.8f;
		pev->sequence = RANDOM_LONG(1, 1); // TODO: what?
	}
	else
	{
		if (m_iBounceCount < 5)
		{
			// play bounce sound
			BounceSound();
		}

		if (m_iBounceCount >= 10)
		{
			pev->groundentity = ENT(0);
			pev->flags |= FL_ONGROUND;
			pev->velocity = g_vecZero;
		}

		m_iBounceCount++;
	}

	pev->framerate = pev->velocity.Length() / 200.0f;

	if (pev->framerate > 1)
	{
		pev->framerate = 1.0f;
	}
	else if (pev->framerate < 0.5f)
	{
		pev->framerate = 0.0f;
	}
}

void CGrenade::SlideTouch(CBaseEntity *pOther)
{
	// don't hit the guy that launched this grenade
	if (pOther->edict() == pev->owner)
		return;

	if (pev->flags & FL_ONGROUND)
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.95f;
	}
	else
	{
		BounceSound();
	}
}

void CGrenade::BounceSound()
{
	if (pev->dmg > 50.0f)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/he_bounce-1.wav", 0.25, ATTN_NORM);
		return;
	}

	switch (RANDOM_LONG(0, 2))
	{
	case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit1.wav", 0.25, ATTN_NORM); break;
	case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit2.wav", 0.25, ATTN_NORM); break;
	case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/grenade_hit3.wav", 0.25, ATTN_NORM); break;
	default: break;
	}
}

void CGrenade::TumbleThink()
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1f;

	if (pev->dmgtime <= gpGlobals->time)
	{
		if (pev->dmg <= 40.0f)
		{
			SetThink(&CGrenade::Detonate);
		}
		else
		{
			SetThink(&CGrenade::HE_Detonate);
		}
	}

	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5f;
		pev->framerate = 0.2f;
	}
}

void CGrenade::SG_TumbleThink()
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	if (pev->flags & FL_ONGROUND)
	{
		pev->velocity = pev->velocity * 0.95f;
	}

	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1f;

	if (pev->dmgtime <= gpGlobals->time)
	{
		if (pev->flags & FL_ONGROUND)
		{
			SetThink(&CGrenade::SG_Detonate);
		}
	}

	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5f;
		pev->framerate = 0.2f;
	}
}

void CGrenade::Spawn()
{
	m_iBounceCount = 0;
	pev->movetype = MOVETYPE_BOUNCE;

	MAKE_STRING_CLASS("grenade", pev);

	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/grenade.mdl");
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	pev->dmg = 30.0f;
	m_fRegisteredSound = FALSE;
}

void CGrenade::Precache()
{
	m_rgusEvents[EQP_HEGRENADE] = PRECACHE_EVENT(1, "events/createexplo.sc");
	m_rgusEvents[EQP_FLASHBANG] = 0;	// Flashbang is not using any event.
	m_rgusEvents[EQP_SMOKEGRENADE] = PRECACHE_EVENT(1, "events/createsmoke.sc");
	m_rgusEvents[EQP_CRYOGRENADE] = PRECACHE_EVENT(1, "events/CryoExplo.sc");
	m_rgusEvents[EQP_INCENDIARY_GR] = PRECACHE_EVENT(1, "events/Molotov.sc");
	m_rgusEvents[EQP_HEALING_GR] = PRECACHE_EVENT(1, "events/createsmoke.sc");	// UNDONE
	m_rgusEvents[EQP_GAS_GR] = PRECACHE_EVENT(1, "events/createsmoke.sc");	// UNDONE
	m_rgusEvents[EQP_C4] = PRECACHE_EVENT(1, "events/C4Explo.sc");

	PRECACHE_SOUND("weapons/hegrenade-1.wav");
	PRECACHE_SOUND("weapons/hegrenade-2.wav");
	PRECACHE_SOUND("weapons/he_bounce-1.wav");

	PRECACHE_SOUND("weapons/flashbang-1.wav");
	PRECACHE_SOUND("weapons/flashbang-2.wav");

	PRECACHE_SOUND("weapons/sg_explode.wav");

	PRECACHE_MODEL("sprites/VFX/fire3.spr");

	g_iSpriteGroundexp1 = PRECACHE_MODEL("sprites/VFX/groundexp1.spr");
	g_iSpriteZerogxplode2 = PRECACHE_MODEL("sprites/VFX/zerogxplode2.spr");
}

CGrenade *CGrenade::HEGrenade(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, int iTeam)
{
	CGrenade *pGrenade = GetClassPtr((CGrenade *)nullptr);
	pGrenade->Spawn();

	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = pevOwner->angles;
	pGrenade->pev->owner = ENT(pevOwner);

	pGrenade->m_usEvent = m_rgusEvents[EQP_HEGRENADE];

	pGrenade->SetTouch(&CGrenade::BounceTouch);

	pGrenade->pev->dmgtime = gpGlobals->time + m_rgflFuseTime[EQP_HEGRENADE];
	pGrenade->SetThink(&CGrenade::TumbleThink);
	pGrenade->pev->nextthink = gpGlobals->time + 0.1f;

	pGrenade->pev->sequence = RANDOM_LONG(3, 6);
	pGrenade->pev->framerate = 1.0f;

	pGrenade->pev->gravity = 0.55f;
	pGrenade->pev->friction = 0.7f;

	pGrenade->m_iTeam = iTeam;

	SET_MODEL(ENT(pGrenade->pev), "models/w_hegrenade.mdl");
	pGrenade->pev->dmg = 100.0f;

	pGrenade->m_iType = HE;
	return pGrenade;
}

CGrenade *CGrenade::Flashbang(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity)
{
	CGrenade *pGrenade = GetClassPtr((CGrenade *)nullptr);
	pGrenade->Spawn();

	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = pevOwner->angles;
	pGrenade->pev->owner = ENT(pevOwner);

	// Bounce if touched
	pGrenade->SetTouch(&CGrenade::BounceTouch);

	// Take one second off of the desired detonation time and set the think to PreDetonate. PreDetonate
	// will insert a DANGER sound into the world sound list and delay detonation for one second so that
	// the grenade explodes after the exact amount of time specified in the call to Flashbang().

	pGrenade->pev->dmgtime = gpGlobals->time + m_rgflFuseTime[EQP_FLASHBANG];
	pGrenade->SetThink(&CGrenade::TumbleThink);
	pGrenade->pev->nextthink = gpGlobals->time + 0.1f;

	if (m_rgflFuseTime[EQP_FLASHBANG] < 0.1f)	// technically it won't happen...
	{
		pGrenade->pev->nextthink = gpGlobals->time;
		pGrenade->pev->velocity = Vector(0, 0, 0);
	}

	pGrenade->pev->sequence = RANDOM_LONG(3, 6);
	pGrenade->pev->framerate = 1.0f;

	pGrenade->pev->gravity = 0.5f;
	pGrenade->pev->friction = 0.8f;

	SET_MODEL(ENT(pGrenade->pev), "models/w_flashbang.mdl");
	pGrenade->pev->dmg = 35.0f;

	pGrenade->m_iType = FLASHBANG;
	return pGrenade;
}

CGrenade *CGrenade::SmokeGrenade(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity)
{
	CGrenade *pGrenade = GetClassPtr((CGrenade *)nullptr);
	pGrenade->Spawn();

	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = pevOwner->angles;
	pGrenade->pev->owner = ENT(pevOwner);
	pGrenade->m_usEvent = m_rgusEvents[EQP_SMOKEGRENADE];
	pGrenade->m_bLightSmoke = false;
	pGrenade->m_bDetonated = false;
	pGrenade->SetTouch(&CGrenade::BounceTouch);
	pGrenade->pev->dmgtime = gpGlobals->time + m_rgflFuseTime[EQP_SMOKEGRENADE];
	pGrenade->SetThink(&CGrenade::SG_TumbleThink);
	pGrenade->pev->nextthink = gpGlobals->time + 0.1f;

	if (m_rgflFuseTime[EQP_SMOKEGRENADE] < 0.1)	// technically it won't happen...
	{
		pGrenade->pev->nextthink = gpGlobals->time;
		pGrenade->pev->velocity = Vector(0, 0, 0);
	}

	pGrenade->pev->sequence = RANDOM_LONG(3, 6);
	pGrenade->pev->framerate = 1.0f;
	pGrenade->pev->gravity = 0.5f;
	pGrenade->pev->friction = 0.8f;
	pGrenade->m_SGSmoke = 0;

	SET_MODEL(ENT(pGrenade->pev), "models/w_smokegrenade.mdl");
	pGrenade->pev->dmg = 35.0f;

	pGrenade->m_iType = SMOKE;
	return pGrenade;
}

CGrenade* CGrenade::Cryogrenade(CBasePlayer* pPlayer)
{
	CGrenade* pGrenade = GetClassPtr((CGrenade*)nullptr);
	pGrenade->Spawn();

	UTIL_MakeVectors(pPlayer->pev->v_angle + pPlayer->pev->punchangle);
	Vector vecSrc = pPlayer->pev->origin + pPlayer->pev->view_ofs + gpGlobals->v_forward * 16.0f;

	UTIL_SetOrigin(pGrenade->pev, vecSrc);
	pGrenade->pev->velocity = gpGlobals->v_forward * 9000.0f;	// LONGBOW grenade from Borderlands.
	pGrenade->pev->angles = pPlayer->pev->angles;
	pGrenade->pev->owner = pPlayer->edict();

	pGrenade->SetTouch(&CGrenade::CryoTouch);
	pGrenade->SetThink(&CGrenade::SUB_DoNothing);

	pGrenade->pev->gravity = 0.55f;
	pGrenade->pev->friction = 0.7f;

	pGrenade->m_iTeam = pPlayer->m_iTeam;

	SET_MODEL(ENT(pGrenade->pev), "models/w_hegrenade.mdl");
	pGrenade->pev->dmg = CRYOGR_DAMAGE;

	// Give it a glow
	pGrenade->pev->renderfx = kRenderFxGlowShell;
	pGrenade->pev->rendercolor = Vector(0, 100, 200);
	pGrenade->pev->rendermode = kRenderNormal;
	pGrenade->pev->renderamt = 16.0f;

	// And a colored trail
	MESSAGE_BEGIN(MSG_ALL, SVC_TEMPENTITY);
	WRITE_BYTE(TE_BEAMFOLLOW); // TE id
	WRITE_SHORT(pGrenade->entindex()); // entity
	WRITE_SHORT(MODEL_INDEX("sprites/lgtning.spr")); // sprite
	WRITE_BYTE(10); // life
	WRITE_BYTE(10); // width
	WRITE_BYTE(0); // r
	WRITE_BYTE(100); // g
	WRITE_BYTE(200); // b
	WRITE_BYTE(200); // brightness
	MESSAGE_END();

	pGrenade->m_usEvent = m_rgusEvents[EQP_CRYOGRENADE];
	pGrenade->m_iType = CRYO;
	return pGrenade;
}

void CGrenade::CryoTouch(CBaseEntity* pOther)
{
	CBaseEntity* pEntity = nullptr;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, CRYOGR_RADIUS)))
	{
		if (FNullEnt(pEntity) || pEntity->pev == pev)
			continue;

		if (pEntity->pev->takedamage == DAMAGE_NO)
			continue;

		if (pEntity->IsPlayer())
		{
			CBasePlayer* pPlayer = CBasePlayer::Instance(pEntity->pev);
			if (pPlayer->IsAlive())
			{
				gFrozenDOTMgr::Set(pPlayer, CRYOGR_DAMAGE, pev, &pev->owner->v, CRYOGR_EFTIME);
			}
		}
		else
		{
			pEntity->TakeDamage(pev, &pev->owner->v, CRYOGR_DAMAGE, DMG_FREEZE);
		}
	}

	EMIT_SOUND(edict(), CHAN_AUTO, gFrozenDOTMgr::ICEGRE_NOVA_SFX, VOL_NORM, ATTN_NORM);
	pev->flags |= FL_KILLME;

	// VFX now move to client.
	PLAYBACK_EVENT_FULL(0, nullptr, m_usEvent, 0, pev->origin, (float*)&g_vecZero, 0, 0, 0, 0, FALSE, FALSE);
}

void CGrenade::IncendiaryTouch(CBaseEntity* pOther)
{
	if (!ENT_IS_ON_FLOOR(edict()))
	{
		DROP_TO_FLOOR(edict());
		return;
	}

	CIncendiaryGrenadeCentre::Create(pev->origin, CBasePlayer::Instance(pev->owner));
	pev->flags |= FL_KILLME;

	// VFX
	// fparam1 == RADIUS
	// fparam2 == DURATION
	PLAYBACK_EVENT_FULL(FEV_GLOBAL | FEV_RELIABLE, nullptr, m_usEvent, 0, pev->origin, (float*)&g_vecZero, CIncendiaryGrenadeCentre::RADIUS, CIncendiaryGrenadeCentre::DURATION, 0, 0, FALSE, FALSE);
}

void CGrenade::C4Touch(CBaseEntity* pOther)
{
	if (pOther->IsPlayer())
		return;

	if (FClassnameIs(pOther->pev, "func_breakable") || FClassnameIs(pOther->pev, "func_ladder") || FClassnameIs(pOther->pev, "func_wall"))
		return;

	if (POINT_CONTENTS(pev->origin) == CONTENTS_SKY)
		return;

	if (pev->movetype == MOVETYPE_NONE)
		return;

	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + pev->velocity, ignore_monsters, edict(), &tr);

	Vector angle;
	VEC_TO_ANGLES(tr.vecPlaneNormal, angle);
	pev->angles = tr.vecPlaneNormal.VectorAngles();
	pev->sequence = 1;
	pev->movetype = MOVETYPE_NONE;

	m_vecAttachedSurfaceNorm = tr.vecPlaneNormal;

	EMIT_SOUND(edict(), CHAN_ITEM, C4_PLACED_SFX, 0.2f, ATTN_NORM);
}

void CGrenade::IncendiaryThink()
{
	if (POINT_CONTENTS(pev->origin) == CONTENT_WATER)	// this cannot be in water, right?
	{
		pev->flags |= FL_KILLME;
		return;
	}

	pev->nextthink = gpGlobals->time + 0.1f;
}

CGrenade* CGrenade::HealingGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity)
{
	CGrenade* pGrenade = SmokeGrenade(pevOwner, vecStart, vecVelocity);
	pGrenade->m_iType = HEALING;
	pGrenade->pev->dmgtime = gpGlobals->time + m_rgflFuseTime[EQP_HEALING_GR];

	return pGrenade;
}

CGrenade* CGrenade::NerveGasGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity)
{
	CGrenade* pGrenade = SmokeGrenade(pevOwner, vecStart, vecVelocity);
	pGrenade->m_iType = NERVE_GAS;
	pGrenade->pev->dmgtime = gpGlobals->time + m_rgflFuseTime[EQP_GAS_GR];

	return pGrenade;
}

CGrenade* CGrenade::IncendiaryGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity)
{
	CGrenade* pGrenade = GetClassPtr((CGrenade*)nullptr);
	pGrenade->Spawn();

	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = pevOwner->angles;
	pGrenade->pev->owner = ENT(pevOwner);

	pGrenade->pev->nextthink = gpGlobals->time + 0.1f;

	pGrenade->pev->sequence = RANDOM_LONG(3, 6);
	pGrenade->pev->framerate = 1.0f;

	pGrenade->pev->gravity = 0.55f;
	pGrenade->pev->friction = 0.7f;

	pGrenade->SetTouch(&CGrenade::IncendiaryTouch);
	pGrenade->SetThink(&CGrenade::IncendiaryThink);

	SET_MODEL(ENT(pGrenade->pev), "models/w_hegrenade.mdl");

	// Give it a glow
	pGrenade->pev->renderfx = kRenderFxGlowShell;
	pGrenade->pev->rendercolor = Vector(255, 117, 26);
	pGrenade->pev->rendermode = kRenderNormal;
	pGrenade->pev->renderamt = 16.0f;

	// And a colored trail
	MESSAGE_BEGIN(MSG_ALL, SVC_TEMPENTITY);
	WRITE_BYTE(TE_BEAMFOLLOW); // TE id
	WRITE_SHORT(pGrenade->entindex()); // entity
	WRITE_SHORT(MODEL_INDEX("sprites/lgtning.spr")); // sprite
	WRITE_BYTE(10); // life
	WRITE_BYTE(10); // width
	WRITE_BYTE(255); // r
	WRITE_BYTE(117); // g
	WRITE_BYTE(26); // b
	WRITE_BYTE(200); // brightness
	MESSAGE_END();

	pGrenade->m_usEvent = m_rgusEvents[EQP_INCENDIARY_GR];
	pGrenade->m_iType = INCENDIARY;
	return pGrenade;
}

CGrenade* CGrenade::C4(CBasePlayer* pPlayer)
{
	CGrenade* pGrenade = GetClassPtr((CGrenade*)nullptr);
	pGrenade->Spawn();

	UTIL_MakeVectors(pPlayer->pev->v_angle + pPlayer->pev->punchangle);
	pGrenade->pev->angles = gpGlobals->v_up.VectorAngles();
	pGrenade->pev->solid = SOLID_TRIGGER;
	pGrenade->pev->movetype = MOVETYPE_TOSS;
	pGrenade->pev->owner = pPlayer->edict();
	SET_MODEL(pGrenade->edict(), C4_WORLD_MODEL);
	SET_ORIGIN(pGrenade->edict(), pPlayer->GetGunPosition() + gpGlobals->v_forward * 16.0f);
	pGrenade->pev->velocity = pPlayer->pev->velocity + gpGlobals->v_forward * C4_INIT_SPEED;
	pGrenade->pev->avelocity = Vector(C4_INIT_SPEED, RANDOM_FLOAT(-C4_INIT_SPEED, C4_INIT_SPEED), 0.0f);

	pGrenade->SetTouch(&CGrenade::C4Touch);
	pGrenade->SetThink(&CGrenade::SUB_DoNothing);

	pGrenade->m_usEvent = m_rgusEvents[EQP_C4];
	pGrenade->m_iType = RCC4;
	return pGrenade;
}

IMPLEMENT_SAVERESTORE(CGrenade, CBaseMonster)
