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
const char* CBaseSkill::CRETICAL_SHOT_SFX = "leadermode/siege_attack.wav";
int CBaseSkill::m_idBulletTrace = 0;

void CBaseSkill::Precache()
{
	PRECACHE_SOUND(CSkillBulletproof::ACTIVATION_SFX);
	PRECACHE_SOUND(CSkillArmorRegen::GIFT_SFX);
	PRECACHE_SOUND(CSkillInfiniteGrenade::ACTIVATION_SFX);
	PRECACHE_SOUND(CSkillEnfoceHeadshot::ACTIVATION_SFX);
	PRECACHE_SOUND(CSkillHealingShot::HEALINGSHOT_SFX);
	PRECACHE_SOUND(CSkillGavelkind::ACTIVATION_SFX);
	PRECACHE_SOUND(CRETICAL_SHOT_SFX);

	CSkillExplosiveBullets::m_rgidSmokeSprite[0] = PRECACHE_MODEL("sprites/black_smoke1.spr");
	CSkillExplosiveBullets::m_rgidSmokeSprite[1] = PRECACHE_MODEL("sprites/black_smoke2.spr");
	CSkillExplosiveBullets::m_rgidSmokeSprite[2] = PRECACHE_MODEL("sprites/black_smoke3.spr");
	CSkillExplosiveBullets::m_rgidSmokeSprite[3] = PRECACHE_MODEL("sprites/black_smoke4.spr");

	CSkillHealingShot::m_idHealingSpr = PRECACHE_MODEL("sprites/leadermode/heal.spr");

	m_idBulletTrace = PRECACHE_MODEL("sprites/leadermode/FireSmoke.spr");
}

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
		WRITE_BYTE(m_pPlayer->entindex());	// hostage index
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
			WRITE_BYTE(m_pPlayer->entindex());	// hostage index
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

		m_flNextRadarUpdate = gpGlobals->time + 1.5f;

		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			CBasePlayer* pTeammate = UTIL_PlayerByIndex(i);

			if (!pTeammate || pTeammate->IsBot())
				continue;

			if (m_pPlayer->m_iTeam != pTeammate->m_iTeam)
				continue;

			MESSAGE_BEGIN(MSG_ONE, gmsgHostagePos, m_pTracing->pev->origin, pTeammate->edict());
			WRITE_BYTE(0);	// flags
			WRITE_BYTE(m_pPlayer->entindex());	// hostage index
			WRITE_COORD(m_pTracing->pev->origin.x);
			WRITE_COORD(m_pTracing->pev->origin.y);
			WRITE_COORD(m_pTracing->pev->origin.z);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ONE, gmsgHostageK, m_pTracing->pev->origin, pTeammate->edict());
			WRITE_BYTE(m_pPlayer->entindex());	// hostage index
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

		MESSAGE_BEGIN(MSG_ONE, gmsgHostageK, g_vecZero, pTeammate->edict());
		WRITE_BYTE(m_pPlayer->entindex());	// hostage index
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
// Role_Arsonist: [Passive] Nonflammable
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
	if (m_pPlayer->m_iRoleType == Role_Arsonist)	// passive skill doesn't require a termination.
		return false;

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

//
// Role_SWAT: Bulletproof
//

const float CSkillBulletproof::DURATION = 15.0f;
const float CSkillBulletproof::COOLDOWN = 40.0f;
const char* CSkillBulletproof::ACTIVATION_SFX = "leadermode/merge_army_fleet.wav";
const char* CSkillBulletproof::CLOSURE_SFX = "leadermode/illegal_move.wav";
const float CSkillBulletproof::GIFT_RADIUS = 250.0f;

bool CSkillBulletproof::Execute()
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

	// give grenades
	m_pPlayer->GiveNamedItem(GetWeaponInfo(WEAPON_FLASHBANG)->m_pszClassName);
	m_pPlayer->GiveNamedItem(GetWeaponInfo(WEAPON_SMOKEGRENADE)->m_pszClassName);

	// give vest + helmet
	m_pPlayer->GiveNamedItem("item_assaultsuit");

	// reload all weapons
	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		CBasePlayerWeapon* pWeapon = (CBasePlayerWeapon*)m_pPlayer->m_rgpPlayerItems[i];

		while (pWeapon)
		{
			if (pWeapon->m_iId != WEAPON_KNIFE)
				pWeapon->AddPrimaryAmmo(pWeapon->iinfo()->m_iMaxClip + pWeapon->ainfo()->m_iMax);

			pWeapon = (CBasePlayerWeapon*)pWeapon->m_pNext;
		}
	}

	CBasePlayer* pPlayer = nullptr;
	while ((pPlayer = UTIL_FindEntityInSphere(pPlayer, m_pPlayer->pev->origin, GIFT_RADIUS)))
	{
		if (!pPlayer->IsPlayer())
			continue;

		if (pPlayer == m_pPlayer)	// not again...
			continue;

		for (int i = 0; i < MAX_ITEM_TYPES; i++)
		{
			CBasePlayerWeapon* pWeapon = (CBasePlayerWeapon*)pPlayer->m_rgpPlayerItems[i];

			while (pWeapon)
			{
				if (pWeapon->iinfo()->m_iMaxClip > 0)	// weapon use a clip.
					pWeapon->AddPrimaryAmmo(pWeapon->iinfo()->m_iMaxClip);	// give your teammate a clip.

				pWeapon = (CBasePlayerWeapon*)pWeapon->m_pNext;
			}
		}
	}

	m_pPlayer->m_pActiveItem->Deploy();	// just some feeling stuff.
	UTIL_ScreenFade(m_pPlayer, Vector(179, 217, 255), 0.5f, GetDuration(), 40, FFADE_IN);
	EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, ACTIVATION_SFX, VOL_NORM, ATTN_NONE);

	m_bUsingSkill = true;
	m_bAllowSkill = false;
	m_flTimeLastUsed = gpGlobals->time;
	return true;
}

void CSkillBulletproof::Think()
{
	// A. it's time up!
	if (m_bUsingSkill && m_flTimeLastUsed + GetDuration() < gpGlobals->time)
	{
		UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
		UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is over!", GetName());

		m_bUsingSkill = false;
		m_flTimeCooldownOver = gpGlobals->time + GetCooldown();
	}

	// B. it's active!
	if (m_bUsingSkill)
	{
		if (m_pPlayer->pev->armorvalue <= 0.0f)	// if you running out of all armour during skill is activated, you would be force to terminate your skill and receive no time refund.
		{
			Terminate();
			m_flTimeCooldownOver = gpGlobals->time + GetCooldown();	// override.
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

bool CSkillBulletproof::Terminate()
{
	UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
	UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is terminated in advanced!", GetName());
	UTIL_ScreenFade(m_pPlayer, Vector(179, 217, 255), 0.5f, 0.1f, 40, FFADE_IN);

	m_bUsingSkill = false;
	m_flTimeCooldownOver = gpGlobals->time + GetCooldown() * Q_clamp((gpGlobals->time - m_flTimeLastUsed) / GetDuration(), 0.0f, 1.0f);	// return the unused time.

	return true;
}

void CSkillBulletproof::OnTraceDamagePre(float& flDamage, TraceResult& tr)
{
	if (m_pPlayer->pev->armortype == ARMOR_NONE || m_pPlayer->pev->armorvalue <= 0.0f)	// no armor, no bulletproof
		return;

	if (!m_bUsingSkill)
		return;

	m_pPlayer->pev->armorvalue = Q_max(m_pPlayer->pev->armorvalue - flDamage * 0.9f, 0.0f);
	flDamage = 0;	// if a single shot damage is more than 100HP + 200AP, this skill will make sure that you won't 1-shot die.
}

//
// Role_SWAT: Liquid Armor
//

const float CSkillArmorRegen::GIFT_RADIUS = 250.0f;
const float CSkillArmorRegen::GIFT_INTERVAL = 5.0f;
const float CSkillArmorRegen::GIFT_AMOUNT = 10.0f;
const char* CSkillArmorRegen::GIFT_SFX = "leadermode/select_army.wav";
const char* CSkillArmorRegen::SELF_REGEN_SFX = "leadermode/society_rank_up_01.wav";
const float CSkillArmorRegen::SELF_REGEN_AFTER_DMG = 5.0f;
const float CSkillArmorRegen::SELF_REGEN_INTERVAL = 0.5f;
const float CSkillArmorRegen::SELF_REGEN_AMOUNT = 1.0f;

void CSkillArmorRegen::Think()
{
	if (!m_pPlayer->IsAlive())
		return;

	// Part I: for others.
	if (m_flNextArmourOffer <= gpGlobals->time)
	{
		CBasePlayer* pPlayer = nullptr;
		while ((pPlayer = UTIL_FindEntityInSphere(pPlayer, m_pPlayer->pev->origin, GIFT_RADIUS)))
		{
			if (!pPlayer->IsPlayer())
				continue;

			if (pPlayer == m_pPlayer)	// not myself.
				continue;

			if (pPlayer->pev->armorvalue < CSGameRules()->PlayerMaxArmour(pPlayer))
			{
				pPlayer->pev->armorvalue = Q_min(pPlayer->pev->armorvalue + GIFT_AMOUNT, CSGameRules()->PlayerMaxArmour(pPlayer));

				if (pPlayer->pev->armortype < ARMOR_KEVLAR)	// remember, this is a float value.
					pPlayer->pev->armortype = ARMOR_KEVLAR;

				EMIT_SOUND(pPlayer->edict(), CHAN_AUTO, GIFT_SFX, 0.75f, ATTN_NORM);
			}
			else if (pPlayer->pev->armortype < ARMOR_VESTHELM)	// no one will reach here unless their armour is full.
			{
				pPlayer->pev->armortype = ARMOR_VESTHELM;
				EMIT_SOUND(pPlayer->edict(), CHAN_AUTO, GIFT_SFX, 0.75f, ATTN_NORM);

				MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, pPlayer->pev->origin, pPlayer->pev);
				WRITE_STRING("item_assaultsuit");
				MESSAGE_END();

				MESSAGE_BEGIN(MSG_ONE, gmsgArmorType, pPlayer->pev->origin, pPlayer->pev);
				WRITE_BYTE(1);
				MESSAGE_END();
			}
		}

		m_flNextArmourOffer = gpGlobals->time + GIFT_INTERVAL;
	}

	// Part II: for myself.
	if (m_flNextSelfArmourRegen <= gpGlobals->time)
	{
		if (m_pPlayer->pev->armorvalue <= 0.0f)
			m_pPlayer->pev->armortype = ARMOR_KEVLAR;

		if (m_pPlayer->pev->armorvalue > 100.0f && m_pPlayer->pev->armortype < ARMOR_VESTHELM)
		{
			m_pPlayer->pev->armortype = ARMOR_VESTHELM;	// upgrade armor after 100.

			MESSAGE_BEGIN(MSG_ONE, gmsgItemPickup, m_pPlayer->pev->origin, m_pPlayer->pev);
			WRITE_STRING("item_assaultsuit");
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ONE, gmsgArmorType, m_pPlayer->pev->origin, m_pPlayer->pev);
			WRITE_BYTE(1);
			MESSAGE_END();
		}

		if (m_bShouldSelfArmourRegenPlaySFX)
		{
			m_bShouldSelfArmourRegenPlaySFX = false;
			UTIL_PlayEarSound(m_pPlayer, SELF_REGEN_SFX);
		}

		m_pPlayer->pev->armorvalue = Q_min(m_pPlayer->pev->armorvalue + SELF_REGEN_AMOUNT, CSGameRules()->PlayerMaxArmour(m_pPlayer));

		m_flNextSelfArmourRegen = gpGlobals->time + SELF_REGEN_INTERVAL;
	}
}

void CSkillArmorRegen::OnPlayerDamagedPre(float& flDamage)
{
	// stop armour regen.
	m_bShouldSelfArmourRegenPlaySFX = true;
	m_flNextSelfArmourRegen = gpGlobals->time + SELF_REGEN_AFTER_DMG;

	// remove the sticky feeling if you have this skill.
	m_pPlayer->m_flVelocityModifier = 1.1f;
}

//
// Role_Breacher: Explosive Bullets
//

const float CSkillExplosiveBullets::DURATION = 10.0f;
const float CSkillExplosiveBullets::COOLDOWN = 30.0f;
const int CSkillExplosiveBullets::EXPLO_LIGHT_RAD = 18 / 2;
const float CSkillExplosiveBullets::EXPLO_RADIUS = 40.0f;
const float CSkillExplosiveBullets::EXPLO_DAMAGE = 8.0f;
const float CSkillExplosiveBullets::RETAINED_DMG_PER_BULLET = 1.0f;
const float CSkillExplosiveBullets::RETAINED_RAD_PER_BULLET = 10.0f;
int CSkillExplosiveBullets::m_rgidSmokeSprite[4];

bool CSkillExplosiveBullets::Execute()
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
	UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/g%s/y is active now. All of your weapons would fire /texplosive bullets/y now.", GetName());

	m_bUsingSkill = true;
	m_bAllowSkill = false;
	m_flTimeLastUsed = gpGlobals->time;

	return true;
}

void CSkillExplosiveBullets::Think()
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

bool CSkillExplosiveBullets::Terminate()
{
	//UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
	UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is terminated in advanced!", GetName());

	m_bUsingSkill = false;
	m_flTimeCooldownOver = gpGlobals->time + GetCooldown() * Q_clamp((gpGlobals->time - m_flTimeLastUsed) / GetDuration(), 0.0f, 1.0f);	// return the unused time.

	return true;
}

void CSkillExplosiveBullets::OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr)
{
	// active condition:
	// a. use KSG12 or STRIKER
	// b. active your skill.

	if (!((1 << m_pPlayer->m_pActiveItem->m_iId) & ((1 << WEAPON_KSG12) | (1 << WEAPON_STRIKER))) && !m_bUsingSkill)
	{
		// or you will make sure that your victim receive a hidden explosive bullet inside them.

		if (FNullEnt(tr.pHit))
			return;

		if (!CBaseEntity::Instance(tr.pHit)->IsPlayer())
			return;

		CBasePlayer* pPlayer = CBasePlayer::Instance(tr.pHit);
		m_rgiRetainedBullets[pPlayer->entindex()]++;

		return;
	}

	if (POINT_CONTENTS(tr.vecEndPos) == CONTENTS_SKY)	// never explode on skybox
		return;

	EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, CRETICAL_SHOT_SFX, VOL_NORM, ATTN_NORM);

	Vector vecOrigin = tr.vecPlaneNormal * 27.0f + tr.vecEndPos;

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecOrigin);
	WRITE_BYTE(TE_SMOKE);
	WRITE_COORD(vecOrigin[0]);
	WRITE_COORD(vecOrigin[1]);
	WRITE_COORD(vecOrigin[2] - 23.0f);
	WRITE_SHORT(RANDOM_ARRAY(m_rgidSmokeSprite));
	WRITE_BYTE(RANDOM_LONG(10, 15));
	WRITE_BYTE(RANDOM_LONG(100, 110));
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecOrigin);
	WRITE_BYTE(TE_DLIGHT);
	WRITE_COORD(tr.vecEndPos[0]);
	WRITE_COORD(tr.vecEndPos[1]);
	WRITE_COORD(tr.vecEndPos[2]);
	WRITE_BYTE(EXPLO_LIGHT_RAD);	// radius
	WRITE_BYTE(255);	// R
	WRITE_BYTE(150);	// G
	WRITE_BYTE(15);		// B
	WRITE_BYTE(8);		// life
	WRITE_BYTE(60);		// decay
	MESSAGE_END();

	CBaseEntity* pEntity = nullptr;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, tr.vecEndPos, EXPLO_RADIUS)))
	{
		if (FNullEnt(pEntity))
			continue;

		if (pEntity->pev->takedamage == DAMAGE_NO)
			continue;

		float flDamage = Q_clamp(EXPLO_DAMAGE * float(1.0f - ((pEntity->Center() - tr.vecEndPos).Length() - 21.0f) / EXPLO_RADIUS), 0.0f, EXPLO_DAMAGE);
		if (flDamage <= 0.0f)
			continue;

		pEntity->TakeDamage(m_pPlayer->m_pActiveItem->pev, m_pPlayer->pev, flDamage, DMG_EXPLOSION);

		if (pEntity->IsPlayer())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgShake, g_vecZero, pEntity->edict());
			WRITE_SHORT(FixedUnsigned16(32.0f, 1 << 12));	// atu
			WRITE_SHORT(FixedUnsigned16(0.5f, 1 << 12));	// freq
			WRITE_SHORT(FixedUnsigned16(5.0f, 1 << 12));	// dur
			MESSAGE_END();
		}
	}
}

void CSkillExplosiveBullets::OnPlayerDeath(CBasePlayer* pKiller)
{
	CBaseSkill::OnPlayerDeath(pKiller);

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer* pVictim = UTIL_PlayerByIndex(i);

		if (FNullEnt(pVictim) || !pVictim->IsAlive())
			continue;

		if (m_rgiRetainedBullets[i] <= 0)
			continue;

		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pVictim->pev->origin);
		WRITE_BYTE(TE_EXPLOSION);	// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD(pVictim->pev->origin.x);		// Send to PAS because of the sound
		WRITE_COORD(pVictim->pev->origin.y);
		WRITE_COORD(pVictim->pev->origin.z + 20.0f);
		WRITE_SHORT(g_sModelIndexFireball3);
		WRITE_BYTE(25);			// scale * 10
		WRITE_BYTE(30);		// framerate
		WRITE_BYTE(TE_EXPLFLAG_NONE);	// flags
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pVictim->pev->origin);
		WRITE_BYTE(TE_EXPLOSION);	// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD(pVictim->pev->origin.x + RANDOM_FLOAT(-64, 64));	// Send to PAS because of the sound
		WRITE_COORD(pVictim->pev->origin.y + RANDOM_FLOAT(-64, 64));
		WRITE_COORD(pVictim->pev->origin.z + RANDOM_FLOAT(30, 35));
		WRITE_SHORT(g_sModelIndexFireball2);
		WRITE_BYTE(30);			// scale * 10
		WRITE_BYTE(30);		// framerate
		WRITE_BYTE(TE_EXPLFLAG_NONE);	// flags
		MESSAGE_END();

		float flDamage = float(m_rgiRetainedBullets[i]) * RETAINED_DMG_PER_BULLET;
		float flRadius = float(m_rgiRetainedBullets[i]) * RETAINED_RAD_PER_BULLET;

		RadiusDamage(pVictim->pev->origin + Vector(0.1f, 0.1f, 0.1f), m_pPlayer->pev, m_pPlayer->pev, flDamage, flRadius, CLASS_NONE, DMG_EXPLOSION);

		TraceResult tr;
		UTIL_TraceLine(pVictim->pev->origin, pVictim->pev->origin + Vector(0, 0, -9999), ignore_monsters, ignore_glass, pVictim->edict(), &tr);
		UTIL_DecalTrace(&tr, RANDOM_LONG(0, 1) ? DECAL_SCORCH1 : DECAL_SCORCH2);

		if (UTIL_PointContents(pVictim->pev->origin) == CONTENTS_WATER)
		{
			UTIL_Bubbles(pVictim->pev->origin - Vector(64, 64, 64), pVictim->pev->origin + Vector(64, 64, 64), 100);
		}
		else
		{
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pVictim->pev->origin);
			WRITE_BYTE(TE_SMOKE);
			WRITE_COORD(pVictim->pev->origin.x);
			WRITE_COORD(pVictim->pev->origin.y);
			WRITE_COORD(pVictim->pev->origin.z - 5.0f);
			WRITE_SHORT(g_sModelIndexSmoke);
			WRITE_BYTE(35 + RANDOM_FLOAT(0, 10)); // scale * 10
			WRITE_BYTE(5); // framerate
			MESSAGE_END();
		}

		int sparkCount = RANDOM_LONG(1, 3);
		for (int i = 0; i < sparkCount; i++)
			CBaseEntity::Create("spark_shower", pVictim->pev->origin, tr.vecPlaneNormal, nullptr);
	}
}

void CSkillExplosiveBullets::OnPlayerKills(CBasePlayer* pVictim)
{
	m_rgiRetainedBullets[pVictim->entindex()] = 0;	// re-zero the bullets count when they dead.
}

//
// Role_Breacher: Bombard
//

const float CSkillInfiniteGrenade::DURATION = 10.0f;
const float CSkillInfiniteGrenade::COOLDOWN = 3.0f;
const char* CSkillInfiniteGrenade::ACTIVATION_SFX = "leadermode/sabotage_event_01.wav";
const char* CSkillInfiniteGrenade::CLOSURE_SFX = "leadermode/drum_02.wav";
const float CSkillInfiniteGrenade::SELF_EXPLO_DMG_MUL = 0.25f;
const float CSkillInfiniteGrenade::DEALT_EXPLO_DMG_MUL = 1.5f;
const float CSkillInfiniteGrenade::GIFT_RADIUS = 250.0f;

bool CSkillInfiniteGrenade::Execute()
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

	// give grenades
	m_pPlayer->GiveNamedItem(GetWeaponInfo(WEAPON_HEGRENADE)->m_pszClassName);
	m_pPlayer->GiveNamedItem(GetWeaponInfo(WEAPON_FLASHBANG)->m_pszClassName);
	m_pPlayer->GiveNamedItem(GetWeaponInfo(WEAPON_SMOKEGRENADE)->m_pszClassName);

	// fill up grenades
	CBasePlayerWeapon* pWeapon = (CBasePlayerWeapon*)m_pPlayer->m_rgpPlayerItems[GRENADE_SLOT];
	while (pWeapon)
	{
		if (pWeapon->ainfo()->m_iId > AMMO_NONE)
			pWeapon->AddPrimaryAmmo(pWeapon->ainfo()->m_iMax);

		pWeapon = (CBasePlayerWeapon*)pWeapon->m_pNext;
	}

	CBasePlayer* pPlayer = nullptr;
	while ((pPlayer = UTIL_FindEntityInSphere(pPlayer, m_pPlayer->pev->origin, GIFT_RADIUS)))
	{
		if (!pPlayer->IsPlayer())
			continue;

		if (pPlayer == m_pPlayer)	// not again...
			continue;

		if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][WEAPON_HEGRENADE] != WPN_F)
			pPlayer->GiveNamedItem(GetWeaponInfo(WEAPON_HEGRENADE)->m_pszClassName);

		if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][WEAPON_FLASHBANG] != WPN_F)
			pPlayer->GiveNamedItem(GetWeaponInfo(WEAPON_FLASHBANG)->m_pszClassName);

		if (g_rgRoleWeaponsAccessibility[pPlayer->m_iRoleType][WEAPON_SMOKEGRENADE] != WPN_F)
			pPlayer->GiveNamedItem(GetWeaponInfo(WEAPON_SMOKEGRENADE)->m_pszClassName);

		pWeapon = (CBasePlayerWeapon*)pPlayer->m_rgpPlayerItems[GRENADE_SLOT];
		while (pWeapon)
		{
			if (pWeapon->ainfo()->m_iId > AMMO_NONE)
				pWeapon->AddPrimaryAmmo(pWeapon->ainfo()->m_iMax);

			pWeapon = (CBasePlayerWeapon*)pWeapon->m_pNext;
		}
	}

	EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, ACTIVATION_SFX, VOL_NORM, ATTN_NONE);

	m_bUsingSkill = true;
	m_bAllowSkill = false;
	m_flTimeLastUsed = gpGlobals->time;
	return true;
}

void CSkillInfiniteGrenade::Think()
{
	// A. it's time up!
	if (m_bUsingSkill && m_flTimeLastUsed + GetDuration() < gpGlobals->time)
	{
		UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
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

bool CSkillInfiniteGrenade::Terminate()
{
	UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
	UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is terminated in advanced!", GetName());

	m_bUsingSkill = false;
	m_flTimeCooldownOver = gpGlobals->time + GetCooldown() * Q_clamp((gpGlobals->time - m_flTimeLastUsed) / GetDuration(), 0.0f, 1.0f);	// return the unused time.

	return true;
}

void CSkillInfiniteGrenade::OnGrenadeThrew(WeaponIdType iId, CGrenade* pGrenade)
{
	if (!m_bUsingSkill)
		return;

	m_pPlayer->m_rgAmmo[GetWeaponInfo(iId)->m_iAmmoType]++;

	pGrenade->pev->dmgtime = gpGlobals->time + 9999.0f;
	pGrenade->SetTouch(&CGrenade::ExplodeTouch);
}

//
// Role_Sharpshooter: Bullseye
//

const float CSkillEnfoceHeadshot::DURATION = 5.0f;
const float CSkillEnfoceHeadshot::COOLDOWN = 30.0f;
const char* CSkillEnfoceHeadshot::ACTIVATION_SFX = "leadermode/agent_recruited.wav";
const char* CSkillEnfoceHeadshot::CLOSURE_SFX = "leadermode/attack_out_of_range_01.wav";

bool CSkillEnfoceHeadshot::Execute()
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

	EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, ACTIVATION_SFX, VOL_NORM, ATTN_NONE);

	m_bUsingSkill = true;
	m_bAllowSkill = false;
	m_flTimeLastUsed = gpGlobals->time;

	return true;
}

void CSkillEnfoceHeadshot::Think()
{
	// A. it's time up!
	if (m_bUsingSkill && m_flTimeLastUsed + GetDuration() < gpGlobals->time)
	{
		UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
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

bool CSkillEnfoceHeadshot::Terminate()
{
	//UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
	UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is terminated in advanced!", GetName());

	m_bUsingSkill = false;
	m_flTimeCooldownOver = gpGlobals->time + GetCooldown() * Q_clamp((gpGlobals->time - m_flTimeLastUsed) / GetDuration(), 0.0f, 1.0f);	// return the unused time.

	return true;
}

void CSkillEnfoceHeadshot::OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr)
{
	if (!m_bUsingSkill)
		return;

	WeaponIdType iId = m_pPlayer->m_pActiveItem->m_iId;

	// you have to use one of these weapon to trigger the enforced headshot.
	if (iId != WEAPON_M200 && iId != WEAPON_M14EBR && iId != WEAPON_AWP && iId != WEAPON_SVD && iId != WEAPON_ANACONDA && iId != WEAPON_DEAGLE)
		return;

	if (FNullEnt(tr.pHit))
		return;

	if (!CBaseEntity::Instance(tr.pHit)->IsPlayer())
		return;

	CBasePlayer* pVictim = CBasePlayer::Instance(tr.pHit);

	// step I. enforce headshot.
	tr.iHitgroup = HITGROUP_HEAD;

	// step II. flashbang.
	RadiusFlash(tr.vecEndPos, pVictim->pev, m_pPlayer->pev, 1.0f, CLASS_NONE, DMG_BULLET | DMG_NEVERGIB);
}

//
// Role_Sharpshooter: Glitter Dust
//

const float CSkillHighlightSight::DURATION = 5.0f;
const float CSkillHighlightSight::COOLDOWN = 30.0f;

bool CSkillHighlightSight::Execute()
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

	UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/g%s/y is active now. All your /tenemies /y would be /ghighlight/y in your sight.", GetName());

	m_bUsingSkill = true;
	m_bAllowSkill = false;
	m_flTimeLastUsed = gpGlobals->time;

	return true;
}

void CSkillHighlightSight::Think()
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

bool CSkillHighlightSight::Terminate()
{
	//UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
	UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is terminated in advanced!", GetName());

	m_bUsingSkill = false;
	m_flTimeCooldownOver = gpGlobals->time + GetCooldown() * Q_clamp((gpGlobals->time - m_flTimeLastUsed) / GetDuration(), 0.0f, 1.0f);	// return the unused time.

	return true;
}

void CSkillHighlightSight::OnAddToFullPack(entity_state_s* pState, edict_t* pEnt, BOOL FIsPlayer)
{
	if (!m_bUsingSkill || !FIsPlayer)
		return;

	CBasePlayer* pPlayer = CBasePlayer::Instance(pEnt);

	if (pPlayer->m_iTeam == m_pPlayer->m_iTeam)	// never marks teammates.
		return;

	if (!pPlayer->IsAlive())	// never marks the dead.
		return;

	if (pPlayer != THE_COMMANDER && pPlayer != THE_GODFATHER && pPlayer->m_flFrozenNextThink <= 0.0f)	// never re-render the frozen player.
	{
		pState->rendermode = kRenderTransAdd;
		pState->renderamt = 255;
		pState->renderfx = kRenderFxFadeSlow;
		pState->rendercolor = { 0, 0, 0 };
	}

	pState->effects |= EF_DIMLIGHT;
}

//
// Role_Medic: Healing Dart
//

const float CSkillHealingShot::DURATION = 5.0f;
const float CSkillHealingShot::COOLDOWN = 20.0f;
const float CSkillHealingShot::DMG_HEAL_CONVERTING_RATIO = 0.5f;
const char* CSkillHealingShot::HEALINGSHOT_SFX = "leadermode/healsound.wav";
const Vector CSkillHealingShot::HEALING_COLOR(51, 204, 255);
int CSkillHealingShot::m_idHealingSpr = 0;

bool CSkillHealingShot::Execute()
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
	UTIL_ScreenFade(m_pPlayer, HEALING_COLOR, 0.2f, 0.2f, 60, FFADE_IN);
	UTIL_PrintChatColor(m_pPlayer, BLUECHAT, "/g%s/y is active now. All your weapons are loaded with /thealing dart/y.", GetName());

	m_bUsingSkill = true;
	m_bAllowSkill = false;
	m_flTimeLastUsed = gpGlobals->time;

	return true;
}

void CSkillHealingShot::Think()
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

bool CSkillHealingShot::Terminate()
{
	//UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
	UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is terminated in advanced!", GetName());

	m_bUsingSkill = false;
	m_flTimeCooldownOver = gpGlobals->time + GetCooldown() * Q_clamp((gpGlobals->time - m_flTimeLastUsed) / GetDuration(), 0.0f, 1.0f);	// return the unused time.

	return true;
}

void CSkillHealingShot::OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr)
{
	if (!m_bUsingSkill)
		return;

	if (FNullEnt(tr.pHit))
		return;

	if (!CBaseEntity::Instance(tr.pHit)->IsPlayer())
		return;

	CBasePlayer* pPatient = CBasePlayer::Instance(tr.pHit);

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, tr.vecEndPos);
	WRITE_BYTE(TE_SPRITE);
	WRITE_COORD(pPatient->pev->origin.x);
	WRITE_COORD(pPatient->pev->origin.y);
	WRITE_COORD(pPatient->pev->origin.z);
	WRITE_SHORT(m_idHealingSpr);
	WRITE_BYTE(10);
	WRITE_BYTE(255);
	MESSAGE_END();

	UTIL_BeamEntPoint(m_pPlayer->entindex() | 0x1000, tr.vecEndPos, m_idBulletTrace, 1, 10, 1, 6, 0, HEALING_COLOR[0], HEALING_COLOR[1], HEALING_COLOR[2], 128, 10);
	EMIT_SOUND(pPatient->edict(), CHAN_AUTO, HEALINGSHOT_SFX, 0.8f, ATTN_NORM);

	float flLastHealth = pPatient->pev->health;
	pPatient->TakeHealth(float(iDamage) * DMG_HEAL_CONVERTING_RATIO, HEALING_REMOVE_DOT);

	m_pPlayer->AddAccount(pPatient->pev->health - flLastHealth, RT_HELPED_TEAMMATE);
	UTIL_PlayEarSound(m_pPlayer, SFX_TSD_GBD);

	tr.flFraction = 1.0f;	// make sure no further damage dealt.
}

//
// Role_Godfather: Baptism
//

const float CSkillGavelkind::DURATION = 20.0f;
const float CSkillGavelkind::COOLDOWN = 60.0f;
const char* CSkillGavelkind::ACTIVATION_SFX = "leadermode/sfx_event_sainthood_01.wav";
const char* CSkillGavelkind::CLOSURE_SFX = "leadermode/sfx_bloodline_add_bloodline_01.wav";
const char* CSkillGavelkind::PASSIVE_SFX = "leadermode/holy_roman_empire_screen.wav";
const float CSkillGavelkind::RADIUS = 200.0f;
const float CSkillGavelkind::PASSIVE_HEALING_AMOUNT = 10.0f;
const float CSkillGavelkind::PASSIVE_HEALING_INTERVAL = 5.0f;
const float CSkillGavelkind::PASSIVE_HEALING_RADIUS = 250.0f;

bool CSkillGavelkind::Execute()
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

	EMIT_SOUND(m_pPlayer->edict(), CHAN_AUTO, ACTIVATION_SFX, VOL_NORM, ATTN_NORM);

	// empty the list.
	m_lstGodchildren.clear();

	CBasePlayer* pPlayer = nullptr;
	while ((pPlayer = UTIL_FindEntityInSphere(pPlayer, m_pPlayer->pev->origin, RADIUS)))
	{
		if (!pPlayer->IsPlayer())
			continue;

		if (pPlayer == m_pPlayer)	// you can't be your own godchild!
			continue;

		if (pPlayer->m_iTeam != m_pPlayer->m_iTeam)	// you have to fight side by side to be godchild.
			continue;

		if (pPlayer->m_iRoleType == Role_LeadEnforcer && pPlayer->IsUsingPrimarySkill())	// berserker would not allow to be both godchildren and crazy freaking monster.
			pPlayer->TerminatePrimarySkill();

		if (pPlayer->m_iRoleType == Role_Assassin && pPlayer->IsUsingPrimarySkill())	// assassin cannot accepts baptism while sneaking.
			continue;

		m_lstGodchildren.emplace_back(pPlayer, pPlayer->pev->health);
	}

	// remove the OH health.
	if (m_pPlayer->m_flOHNextThink > 0.0f)
	{
		m_pPlayer->pev->health = m_pPlayer->m_flOHOriginalHealth;
		m_pPlayer->m_flOHNextThink = 0;
	}

	// time to 'gavelkind' the HP!
	float flDividedHealth = m_pPlayer->pev->health / float(m_lstGodchildren.size() + 1);	// including godfather himself.
	m_flSavedDeltaHP = m_pPlayer->pev->health - flDividedHealth;	// the point is, the godchildrens may receive their original health back, but not the godfather.

	// assign the health to the godfather first.
	m_pPlayer->pev->health = flDividedHealth;

	// set the health to all godchildren.
	if (!m_lstGodchildren.empty())
	{
		for (auto Godchild : m_lstGodchildren)
		{
			Godchild.m_pGodchild->pev->health += flDividedHealth;	// as for the godchildren, the health is PLUS not just ASSIGN.
		}
	}

	m_bUsingSkill = true;
	m_bAllowSkill = false;
	m_flTimeLastUsed = gpGlobals->time;

	return true;
}

void CSkillGavelkind::Think()
{
	// A. it's time up!
	if (m_bUsingSkill && m_flTimeLastUsed + GetDuration() < gpGlobals->time)
	{
		// the death of godchildren will NOT stop the HP payback. this is the rule. intended.
		if (!m_lstGodchildren.empty())
		{
			for (auto Godchild : m_lstGodchildren)
			{
				if (!Godchild.m_pGodchild.IsValid() || !Godchild.m_pGodchild->IsAlive())	// only death or ... disconnection would stop this.
					continue;

				Godchild.m_pGodchild->pev->health = Godchild.m_flOriginalHealth;
				UTIL_PlayEarSound(Godchild.m_pGodchild, CLOSURE_SFX);
			}
		}

		// clear the list.
		m_lstGodchildren.clear();

		UTIL_PlayEarSound(m_pPlayer, CLOSURE_SFX);
		UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/t%s is over!", GetName());

		// as for the godfather... he would get all his health back.
		m_pPlayer->pev->health += m_flSavedDeltaHP;
		m_flSavedDeltaHP = 0;

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

	// D. the Passive portion of skill!
	if (m_flNextPassiveHealingThink < gpGlobals->time && !m_bUsingSkill)	// passive effect is disabled while using main skill.
	{
		m_flNextPassiveHealingThink = gpGlobals->time + PASSIVE_HEALING_INTERVAL;

		CBasePlayer* pPlayer = nullptr;
		while ((pPlayer = UTIL_FindEntityInSphere(pPlayer, m_pPlayer->pev->origin, RADIUS)))
		{
			if (!pPlayer->IsPlayer())
				continue;

			if (pPlayer == m_pPlayer)	// you can't heal yourself.
				continue;

			if (pPlayer->m_iTeam != m_pPlayer->m_iTeam)	// you have to be the same team.
				continue;

			if (pPlayer->m_iRoleType == Role_Assassin && pPlayer->IsUsingPrimarySkill())	// the heal might expose him.
				continue;

			if (pPlayer->m_iRoleType == Role_LeadEnforcer && pPlayer->IsUsingPrimarySkill())
				pPlayer->TerminatePrimarySkill();

			if (pPlayer->TakeHealth(PASSIVE_HEALING_AMOUNT, HEALING_NO_OH))	// weaker healing. no DOT removal, no overheal.
			{
				// but with a LOT of FXs!
				UTIL_PlayEarSound(pPlayer, PASSIVE_SFX);
				UTIL_ScreenFade(pPlayer, Vector(179, 217, 255), 0.2f, 0.1f, 30, FFADE_IN);
			}
		}
	}
}

bool CSkillGavelkind::Terminate()	// the terminate machism is different in this skill. this would permanently increase their HP limit in the rest of round.
{
	if (!m_lstGodchildren.empty())
	{
		for (auto Godchild : m_lstGodchildren)
		{
			if (!Godchild.m_pGodchild.IsValid() || !Godchild.m_pGodchild->IsAlive())
				continue;

			Godchild.m_pGodchild->pev->max_health = Godchild.m_pGodchild->pev->health;	// increase HP limit to the current value.

			UTIL_PlayEarSound(m_pPlayer, PASSIVE_SFX);
			UTIL_PrintChatColor(m_pPlayer, REDCHAT, "/yThough /t%s %s/y was killed, but a part of him is /gnow a part of you/y.", g_rgszRoleNames[m_pPlayer->m_iRoleType], STRING(m_pPlayer->pev->netname));
		}
	}

	m_bUsingSkill = false;
	m_flTimeCooldownOver = gpGlobals->time + GetCooldown() * Q_clamp((gpGlobals->time - m_flTimeLastUsed) / GetDuration(), 0.0f, 1.0f);	// return the unused time.

	return true;
}
