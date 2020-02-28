/*

Created date: 02/26/2020

*/

#include "precompiled.h"

const char* g_rgszRoleNames[ROLE_COUNT] =
{
	"Noobie",

	"Commander",
	"S.W.A.T.",
	"Breacher",
	"Sharpshooter",
	"Medic",

	"Godfather",
	"Lead Enforcer",
	"Mad Scientist",
	"Hitman",
	"Arsonist"
};

const char* CBaseSkill::RADAR_BEEP_SFX = "leadermode/nes_8bit_alien3_radar_beep1.wav";
const char* CBaseSkill::RADAR_TARGET_DEAD_SFX = "leadermode/sfx_event_duel_win_01.wav";
const char* CBaseSkill::COOLDOWN_COMPLETE_SFX = "leadermode/pope_accepts_crusade_arrived.wav";

float CBaseSkill::GetHudPercentage() const
{
	if (m_bUsingSkill)	// give a skill time left percentage.
	{
		return Q_clamp(1.0f - ((gpGlobals->time - m_flTimeLastUsed) / GetDuration()), 0.0f, 1.0f);
	}
	else if (!m_bUsingSkill && !m_bAllowSkill)	// give a cooldown percentage
	{
		return Q_clamp(1.0f - (m_flTimeCooldownOver - gpGlobals->time) / GetCooldown(), 0.0f, 1.0f);
	}

	return 1.0f;	// ready to use.
}

//
// Role_Commander: Battlefield Analysis
//

const float CSkillRadarScan::DURATION = 20.0f;
const float CSkillRadarScan::COOLDOWN = 60.0f;
const char* CSkillRadarScan::ACTIVATION_SFX = "leadermode/peace_summary_message_01.wav";
const char* CSkillRadarScan::CLOSURE_SFX = "leadermode/assign_leader_02.wav";

bool CSkillRadarScan::Execute()
{
	if (!m_pPlayer || !m_pPlayer->IsAlive() || !CSGameRules()->CanSkillBeUsed())	// skill is not allowed in freezing phase.
		return false;

	if (m_bUsingSkill)
	{
		UTIL_PrintChatColor(m_pPlayer, GREYCHAT, "/t%s is currently activated!", GetName());
		return false;
	}

	if (!m_bAllowSkill)
	{
		UTIL_PrintChatColor(m_pPlayer, GREYCHAT, "/t%s is currently cooling down!", GetName());
		return false;
	}

	if (!THE_GODFATHER.IsValid() || !THE_GODFATHER->IsAlive())	// then pick a random guy.
	{
		CBasePlayer* pEnemy = nullptr;

		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			pEnemy = UTIL_PlayerByIndex(i);

			if (!pEnemy || !pEnemy->IsAlive() || pEnemy->IsDormant())
				continue;

			if (m_pPlayer->m_iTeam == pEnemy->m_iTeam)
				continue;

			m_pTracing = pEnemy;
			break;
		}
	}
	else
		m_pTracing = THE_GODFATHER;

	// nobody to trace.
	if (!m_pTracing)
	{
		UTIL_PrintChatColor(m_pPlayer, GREYCHAT, "/tWe have no one to trace currently!");
		return false;
	}

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pTeammate = UTIL_PlayerByIndex(i);

		if (!pTeammate || !pTeammate->IsAlive() || pTeammate->IsBot())
			continue;

		if (m_pPlayer->m_iTeam != pTeammate->m_iTeam)
			continue;

		MESSAGE_BEGIN(MSG_ONE, gmsgHostagePos, m_pTracing->pev->origin, pTeammate->edict());
		WRITE_BYTE(1);	// flags
		WRITE_BYTE(1);	// hostage index
		WRITE_COORD(m_pTracing->pev->origin.x);
		WRITE_COORD(m_pTracing->pev->origin.y);
		WRITE_COORD(m_pTracing->pev->origin.z);
		MESSAGE_END();

		UTIL_PlayEarSound(pTeammate, ACTIVATION_SFX);
	}

	UTIL_PrintChatColor(nullptr, REDCHAT, "/gthe /y%s %s/g starts an UAV scanning, the location of /t%s %s/g is exposed!", g_rgszRoleNames[m_pPlayer->m_iRoleType], STRING(m_pPlayer->pev->netname), g_rgszRoleNames[m_pTracing->m_iRoleType], STRING(m_pTracing->pev->netname));

	m_bUsingSkill = true;
	m_bAllowSkill = false;
	m_flTimeLastUsed = gpGlobals->time;
	m_flNextRadarUpdate = gpGlobals->time + 1.0;

	return true;
}

void CSkillRadarScan::Think()
{
	// A. it's time up!
	if (m_bUsingSkill && m_flTimeLastUsed + GetDuration() < gpGlobals->time)
	{
		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			CBasePlayer* pTeammate = UTIL_PlayerByIndex(i);

			if (!pTeammate || !pTeammate->IsAlive() || pTeammate->IsBot())
				continue;

			if (m_pPlayer->m_iTeam != pTeammate->m_iTeam)
				continue;

			MESSAGE_BEGIN(MSG_ONE, gmsgHostageK, m_pTracing->pev->origin, pTeammate->edict());
			WRITE_BYTE(1);	// hostage index
			MESSAGE_END();

			UTIL_PlayEarSound(pTeammate, CLOSURE_SFX);
		}

		UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is over!", GetName());

		m_bUsingSkill = false;
		m_flTimeCooldownOver = gpGlobals->time + GetCooldown();
	}

	// B. it's active!
	else if (m_bUsingSkill && m_flNextRadarUpdate <= gpGlobals->time)
	{
		if (!m_pPlayer->IsAlive())
			return;

		// if the target we are tracing just disappear, then terminate skill.
		if (!m_pTracing.IsValid())
		{
			Terminate();
			return;
		}

		if (!m_pTracing->IsAlive())
		{
			if (m_pTracing->entindex() != THE_GODFATHER->entindex())	// we have to specify the != operator.
			{
				for (int i = 1; i <= gpGlobals->maxClients; i++)
				{
					CBasePlayer* pTeammate = UTIL_PlayerByIndex(i);

					if (!pTeammate || pTeammate->IsBot())
						continue;

					if (m_pPlayer->m_iTeam != pTeammate->m_iTeam)
						continue;

					UTIL_PlayEarSound(pTeammate, RADAR_TARGET_DEAD_SFX);
					UTIL_PrintChatColor(pTeammate, REDCHAT, "/gThe target /t%s %s/g traced by /y%s %s/g was eliminated!", g_rgszRoleNames[m_pTracing->m_iRoleType], STRING(m_pTracing->pev->netname), g_rgszRoleNames[m_pPlayer->m_iRoleType], STRING(m_pPlayer->pev->netname));
				}
			}

			Terminate();
			return;
		}

		m_flNextRadarUpdate = gpGlobals->time + 1.0f;

		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			CBasePlayer* pTeammate = UTIL_PlayerByIndex(i);

			if (!pTeammate || pTeammate->IsBot())
				continue;

			if (m_pPlayer->m_iTeam != pTeammate->m_iTeam)
				continue;

			MESSAGE_BEGIN(MSG_ONE, gmsgHostagePos, m_pTracing->pev->origin, pTeammate->edict());
			WRITE_BYTE(0);	// flags
			WRITE_BYTE(1);	// hostage index
			WRITE_COORD(m_pTracing->pev->origin.x);
			WRITE_COORD(m_pTracing->pev->origin.y);
			WRITE_COORD(m_pTracing->pev->origin.z);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ONE, gmsgHostageK, m_pTracing->pev->origin, pTeammate->edict());
			WRITE_BYTE(1);	// hostage index
			MESSAGE_END();

			UTIL_PlayEarSound(pTeammate, RADAR_BEEP_SFX);
		}
	}

	// C. the CD is over!
	else if (!m_bUsingSkill && !m_bAllowSkill && m_flTimeCooldownOver <= gpGlobals->time)
	{
		m_bAllowSkill = true;

		UTIL_PrintChatColor(m_pPlayer, GREENCHAT, "/g%s is ready again!", GetName());
		UTIL_PlayEarSound(m_pPlayer, COOLDOWN_COMPLETE_SFX);
	}
}

bool CSkillRadarScan::Terminate()
{
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer* pTeammate = UTIL_PlayerByIndex(i);

		if (!pTeammate || !pTeammate->IsAlive() || pTeammate->IsBot())
			continue;

		if (m_pPlayer->m_iTeam != pTeammate->m_iTeam)
			continue;

		MESSAGE_BEGIN(MSG_ONE, gmsgHostageK, m_pTracing->pev->origin, pTeammate->edict());
		WRITE_BYTE(1);	// hostage index
		MESSAGE_END();

		UTIL_PlayEarSound(pTeammate, CLOSURE_SFX);
	}

	UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is terminated in advanced!", GetName());

	m_bUsingSkill = false;
	m_flTimeCooldownOver = gpGlobals->time + GetCooldown() * Q_clamp((gpGlobals->time - m_flTimeLastUsed) / GetDuration(), 0.0f, 1.0f);	// return the unused time.

	return true;
}

//
// Role_Commander: Haste
//

const float CSkillFireRate::DURATION = 20.0f;
const float CSkillFireRate::COOLDOWN = 60.0f;
const float CSkillFireRate::FIREINTERVAL_MODIFIER = 0.5f;

bool CSkillFireRate::Execute()
{
	if (!m_pPlayer || !m_pPlayer->IsAlive() || !CSGameRules()->CanSkillBeUsed())	// skill is not allowed in freezing phase.
		return false;

	if (m_bUsingSkill)
	{
		UTIL_PrintChatColor(m_pPlayer, GREYCHAT, "/t%s is currently activated!", GetName());
		return false;
	}

	if (!m_bAllowSkill)
	{
		UTIL_PrintChatColor(m_pPlayer, GREYCHAT, "/t%s is currently cooling down!", GetName());
		return false;
	}

	//UTIL_PlayEarSound(m_pPlayer, ACTIVATION_SFX);
	UTIL_PrintChatColor(m_pPlayer, BLUECHAT, "/g%s/y is active now. Your /tweapon fire interval/y is /thalved/y.", GetName());

	m_bUsingSkill = true;
	m_bAllowSkill = false;
	m_flTimeLastUsed = gpGlobals->time;

	return true;
}

void CSkillFireRate::Think()
{
	// A. it's time up!
	if (m_bUsingSkill && m_flTimeLastUsed + GetDuration() < gpGlobals->time)
	{
		//UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
		UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is over!", GetName());

		m_bUsingSkill = false;
		m_flTimeCooldownOver = gpGlobals->time + GetCooldown();
	}

	// B. it's active!


	// C. the CD is over!
	else if (!m_bUsingSkill && !m_bAllowSkill && m_flTimeCooldownOver <= gpGlobals->time)
	{
		m_bAllowSkill = true;

		UTIL_PrintChatColor(m_pPlayer, GREENCHAT, "/g%s is ready again!", GetName());
		UTIL_PlayEarSound(m_pPlayer, COOLDOWN_COMPLETE_SFX);
	}
}

bool CSkillFireRate::Terminate()
{
	//UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
	UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is terminated in advanced!", GetName());

	m_bUsingSkill = false;
	m_flTimeCooldownOver = gpGlobals->time + GetCooldown() * Q_clamp((gpGlobals->time - m_flTimeLastUsed) / GetDuration(), 0.0f, 1.0f);	// return the unused time.

	return true;
}


//
// Role_Commander: Stainless Steel
// Role_Godfather: Rockstand
//

const float CSkillReduceDamage::DURATION = 20.0f;
const float CSkillReduceDamage::COOLDOWN = 60.0f;
const float CSkillReduceDamage::DAMAGE_MODIFIER = 0.5f;

bool CSkillReduceDamage::Execute()
{
	if (!m_pPlayer || !m_pPlayer->IsAlive() || !CSGameRules()->CanSkillBeUsed())	// skill is not allowed in freezing phase.
		return false;

	if (m_bUsingSkill)
	{
		UTIL_PrintChatColor(m_pPlayer, GREYCHAT, "/t%s is currently activated!", GetName());
		return false;
	}

	if (!m_bAllowSkill)
	{
		UTIL_PrintChatColor(m_pPlayer, GREYCHAT, "/t%s is currently cooling down!", GetName());
		return false;
	}

	if (m_pPlayer->m_iRoleType == Role_Arsonist)	// this is a passive skill to arsonist.
		return false;

	//UTIL_PlayEarSound(m_pPlayer, ACTIVATION_SFX);
	UTIL_PrintChatColor(m_pPlayer, GREYCHAT, "/g%s/y is active now. /tDamage/y dealt to you is /ghalved/y.", GetName());

	m_bUsingSkill = true;
	m_bAllowSkill = false;
	m_flTimeLastUsed = gpGlobals->time;

	return true;
}

void CSkillReduceDamage::Think()
{
	// A. it's time up!
	if (m_bUsingSkill && m_flTimeLastUsed + GetDuration() < gpGlobals->time)
	{
		//UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
		UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is over!", GetName());

		m_bUsingSkill = false;
		m_flTimeCooldownOver = gpGlobals->time + GetCooldown();
	}

	// B. it's active!


	// C. the CD is over!
	else if (!m_bUsingSkill && !m_bAllowSkill && m_flTimeCooldownOver <= gpGlobals->time)
	{
		m_bAllowSkill = true;

		UTIL_PrintChatColor(m_pPlayer, GREENCHAT, "/g%s is ready again!", GetName());
		UTIL_PlayEarSound(m_pPlayer, COOLDOWN_COMPLETE_SFX);
	}
}

bool CSkillReduceDamage::Terminate()
{
	//UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
	UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is terminated in advanced!", GetName());

	m_bUsingSkill = false;
	m_flTimeCooldownOver = gpGlobals->time + GetCooldown() * Q_clamp((gpGlobals->time - m_flTimeLastUsed) / GetDuration(), 0.0f, 1.0f);	// return the unused time.

	return true;
}

const char* CSkillReduceDamage::GetName() const
{
	if (m_pPlayer->m_iRoleType == Role_Arsonist)
		return "[Passive] Nonflammable";
	else if (m_pPlayer->m_iTeam == CT)
		return "Stainless Steel";
	else if (m_pPlayer->m_iTeam == TERRORIST)
		return "Rockstand";

	return "ERROR - CONTACT LUNA";
}

float CSkillReduceDamage::PlayerDamageSufferedModifier(int bitsDamageTypes)
{
	if (m_pPlayer->m_iRoleType == Role_Arsonist && bitsDamageTypes & (DMG_BURN | DMG_SLOWBURN))	// arsonist is immune to fire.
		return 0.0f;

	return m_bUsingSkill ? DAMAGE_MODIFIER : 1.0f;
}
