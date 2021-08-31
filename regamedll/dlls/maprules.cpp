#include "precompiled.h"

TYPEDESCRIPTION CRuleEntity::m_SaveData[] =
{
	DEFINE_FIELD(CRuleEntity, m_iszMaster, FIELD_STRING),
};

IMPLEMENT_SAVERESTORE(CRuleEntity, CBaseEntity)

void CRuleEntity::Spawn()
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = EF_NODRAW;
}

void CRuleEntity::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "master"))
	{
		SetMaster(ALLOC_STRING(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseEntity::KeyValue(pkvd);
	}
}

bool CRuleEntity::CanFireForActivator(CBaseEntity *pActivator)
{
	if (!FStringNull(m_iszMaster))
	{
		return UTIL_IsMasterTriggered(m_iszMaster, pActivator);
	}

	return true;
}

void CRulePointEntity::Spawn()
{
	CRuleEntity::Spawn();
	pev->frame = 0;
	pev->model = 0;
}

void CRuleBrushEntity::Spawn()
{
	SET_MODEL(edict(), STRING(pev->model));
	CRuleEntity::Spawn();
}

LINK_ENTITY_TO_CLASS(game_score, CGameScore)

void CGameScore::Spawn()
{
	CRulePointEntity::Spawn();
}

void CGameScore::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "points"))
	{
		SetPoints(Q_atoi(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	else
	{
		CRulePointEntity::KeyValue(pkvd);
	}
}

void CGameScore::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!pActivator || !CanFireForActivator(pActivator))
		return;

	// Only players can use this
	if (pActivator->IsPlayer())
	{
		if (AwardToTeam())
		{
			pActivator->AddPointsToTeam(Points(), AllowNegativeScore());
		}
		else
		{
			pActivator->AddPoints(Points(), AllowNegativeScore());
		}
	}
}

LINK_ENTITY_TO_CLASS(game_end, CGameEnd)

void CGameEnd::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!CanFireForActivator(pActivator))
		return;

	g_pGameRules->EndMultiplayerGame();
}

// Save parms as a block.  Will break save/restore if the structure changes, but this entity didn't ship with Half-Life, so
// it can't impact saved Half-Life games.
TYPEDESCRIPTION CGameText::m_SaveData[] =
{
	DEFINE_ARRAY(CGameText, m_textParms, FIELD_CHARACTER, sizeof(hudtextparms_t)),
};

LINK_ENTITY_TO_CLASS(game_text, CGameText)
IMPLEMENT_SAVERESTORE(CGameText, CRulePointEntity)

void CGameText::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "channel"))
	{
		m_textParms.channel = Q_atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "x"))
	{
		m_textParms.x = Q_atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "y"))
	{
		m_textParms.y = Q_atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "effect"))
	{
		m_textParms.effect = Q_atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "color"))
	{
		int color[4];
		UTIL_StringToIntArray(color, ARRAYSIZE(color), pkvd->szValue);

		m_textParms.r1 = color[0];
		m_textParms.g1 = color[1];
		m_textParms.b1 = color[2];
		m_textParms.a1 = color[3];

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "color2"))
	{
		int color[4];
		UTIL_StringToIntArray(color, ARRAYSIZE(color), pkvd->szValue);

		m_textParms.r2 = color[0];
		m_textParms.g2 = color[1];
		m_textParms.b2 = color[2];
		m_textParms.a2 = color[3];

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "fadein"))
	{
		m_textParms.fadeinTime = Q_atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "fadeout"))
	{
		m_textParms.fadeoutTime = Q_atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "holdtime"))
	{
		m_textParms.holdTime = Q_atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "fxtime"))
	{
		m_textParms.fxTime = Q_atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CRulePointEntity::KeyValue(pkvd);
	}
}

void CGameText::Spawn()
{
	// Don't allow entity triggering itself
	if (FStrEq(pev->target, pev->targetname))
	{
		ALERT(at_warning, "%s \"%s\" the target applies to itself.\n", STRING(pev->classname), STRING(pev->targetname));
		pev->target = iStringNull;
	}
}

void CGameText::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!CanFireForActivator(pActivator))
		return;

	if (MessageToAll())
	{
		UTIL_HudMessageAll(m_textParms, MessageGet());
		ALERT(at_aiconsole, "HUD-MSG to all: \"%s\"\n", MessageGet());
	}
	else
	{
		if (FNullEnt(pActivator))
		{
			ALERT(at_console, "Game_text \"%s\" got no activator for activator-only message.\n", STRING(pev->targetname));
		}
		else if (pActivator->IsNetClient())
		{
			UTIL_HudMessage(pActivator, m_textParms, MessageGet());
			ALERT(at_aiconsole, "HUD-MSG to \"%s\": \"%s\"\n", STRING(pActivator->pev->netname), MessageGet());
		}
	}

	SUB_UseTargets(pActivator, USE_TOGGLE, 0);
}

LINK_ENTITY_TO_CLASS(game_team_master, CGameTeamMaster)

void CGameTeamMaster::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "teamindex"))
	{
		m_teamIndex = Q_atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "triggerstate"))
	{
		int type = Q_atoi(pkvd->szValue);
		switch (type)
		{
		case 0:
			m_triggerType = USE_OFF;
			break;
		case 2:
			m_triggerType = USE_TOGGLE;
			break;
		default:
			m_triggerType = USE_ON;
			break;
		}

		pkvd->fHandled = TRUE;
	}
	else
	{
		CRulePointEntity::KeyValue(pkvd);
	}
}

void CGameTeamMaster::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!pActivator || !CanFireForActivator(pActivator))
		return;

	if (useType == USE_SET)
	{
		if (value < 0)
		{
			m_teamIndex = -1;
		}
		else
		{
			if (pActivator->IsPlayer())
				m_teamIndex = ((CBasePlayer *)pActivator)->m_iTeam;
			else
				m_teamIndex = -1;
		}

		return;
	}

	if (TeamMatch(pActivator))
	{
		SUB_UseTargets(pActivator, m_triggerType, value);

		if (RemoveOnFire())
		{
			UTIL_Remove(this);
		}
	}
}

bool CGameTeamMaster::IsTriggered(CBaseEntity *pActivator)
{
	return TeamMatch(pActivator);
}

const char *CGameTeamMaster::TeamID()
{
	// Currently set to "no team"
	if (m_teamIndex < 0)
	{
		return "";
	}

	// UNDONE: Fill this in with the team from the "teamlist"
	return GetTeamName(m_teamIndex);
}

bool CGameTeamMaster::TeamMatch(CBaseEntity *pActivator)
{
	if (m_teamIndex < 0 && AnyTeam())
		return true;

	if (!pActivator)
		return false;

	CBasePlayer *pPlayer = static_cast<CBasePlayer *>(pActivator);
	if (!pPlayer->IsPlayer())
		return false;

	return pPlayer->m_iTeam == m_teamIndex;
}

LINK_ENTITY_TO_CLASS(game_team_set, CGameTeamSet)

void CGameTeamSet::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!CanFireForActivator(pActivator))
		return;

	if (ShouldClearTeam())
	{
		// clear the team of our target
		SUB_UseTargets(pActivator, USE_SET, -1);
	}
	else
	{
		// set the team of our target to our activator's team
		SUB_UseTargets(pActivator, USE_SET, 0);
	}

	if (RemoveOnFire())
	{
		UTIL_Remove(this);
	}
}

TYPEDESCRIPTION CGamePlayerZone::m_SaveData[] =
{
	DEFINE_FIELD(CGamePlayerZone, m_iszInTarget, FIELD_STRING),
	DEFINE_FIELD(CGamePlayerZone, m_iszOutTarget, FIELD_STRING),
	DEFINE_FIELD(CGamePlayerZone, m_iszInCount, FIELD_STRING),
	DEFINE_FIELD(CGamePlayerZone, m_iszOutCount, FIELD_STRING),
};

LINK_ENTITY_TO_CLASS(game_zone_player, CGamePlayerZone)
IMPLEMENT_SAVERESTORE(CGamePlayerZone, CRuleBrushEntity)

void CGamePlayerZone::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "intarget"))
	{
		m_iszInTarget = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "outtarget"))
	{
		m_iszOutTarget = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "incount"))
	{
		m_iszInCount = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "outcount"))
	{
		m_iszOutCount = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CRuleBrushEntity::KeyValue(pkvd);
	}
}

class CGameZonePlayerVolumeListener: public CPlayerInVolumeAdapter
{
public:
	CGameZonePlayerVolumeListener(CGamePlayerZone *pBrushEntity, CBaseEntity *pActivator, USE_TYPE useType, float value);

	void PlayerDetected(const bool fInVolume, CBasePlayer *pPlayer) override
	{
		string_t targetName = fInVolume ? m_iszInTarget : m_iszOutTarget;
		if (FStringNull(targetName))
			return;

		FireTargets(STRING(targetName), pPlayer, m_hActivator, m_useType, m_value);
	}

private:
	string_t m_iszInTarget;
	string_t m_iszOutTarget;

	EHANDLE m_hActivator;
	USE_TYPE m_useType;
	float m_value;
};

CGameZonePlayerVolumeListener::CGameZonePlayerVolumeListener(CGamePlayerZone *pBrushEntity, CBaseEntity *pActivator, USE_TYPE useType, float value)
{
	m_iszInTarget  = pBrushEntity->m_iszInTarget;
	m_iszOutTarget = pBrushEntity->m_iszOutTarget;

	m_hActivator = pActivator;
	m_useType    = useType;
	m_value      = value;
}

void CGamePlayerZone::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	int playersInCount = 0;
	int playersOutCount = 0;

	if (!CanFireForActivator(pActivator))
		return;

	CGameZonePlayerVolumeListener adapter(this, pActivator, useType, value);
	UTIL_CountPlayersInBrushVolume(HasOnlyAlive(), this, playersInCount, playersOutCount, &adapter);

	if (!FStringNull(m_iszInCount))
	{
		FireTargets(STRING(m_iszInCount), pActivator, this, USE_SET, playersInCount);
	}

	if (!FStringNull(m_iszOutCount))
	{
		FireTargets(STRING(m_iszOutCount), pActivator, this, USE_SET, playersOutCount);
	}
}

LINK_ENTITY_TO_CLASS(game_player_hurt, CGamePlayerHurt)

void CGamePlayerHurt::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!pActivator || !CanFireForActivator(pActivator))
		return;

	if (pActivator->IsPlayer())
	{
		if (pev->dmg < 0)
			pActivator->TakeHealth(-pev->dmg, DMG_GENERIC);
		else
			pActivator->TakeDamage(pev, pev, pev->dmg, DMG_GENERIC);
	}

	SUB_UseTargets(pActivator, useType, value);

	if (RemoveOnFire())
	{
		UTIL_Remove(this);
	}
}

LINK_ENTITY_TO_CLASS(game_counter, CGameCounter)

void CGameCounter::Spawn()
{
	// Save off the initial count
	SetInitialValue(CountValue());
	CRulePointEntity::Spawn();
}

void CGameCounter::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!CanFireForActivator(pActivator))
		return;

	switch (useType)
	{
	case USE_ON:
	case USE_TOGGLE:
		CountUp();
		break;

	case USE_OFF:
		CountDown();
		break;

	case USE_SET:
		SetCountValue(int(value));
		break;
	}

	if (HitLimit())
	{
		SUB_UseTargets(pActivator, USE_TOGGLE, 0);

		if (RemoveOnFire())
		{
			UTIL_Remove(this);
		}

		if (ResetOnFire())
		{
			ResetCount();
		}
	}
}

LINK_ENTITY_TO_CLASS(game_counter_set, CGameCounterSet)

void CGameCounterSet::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!CanFireForActivator(pActivator))
		return;

	SUB_UseTargets(pActivator, USE_SET, pev->frags);

	if (RemoveOnFire())
	{
		UTIL_Remove(this);
	}
}
