/*

Created date: 03/03/2020

*/

#pragma once

class CHealingSmokeCenter : public CBaseEntity
{
public:
	static const char* HEALING_SFX[10];
	static const char* HEALING_FULL_SFX;
	static const float HEALING_INTERVAL;
	static const float HEALING_AMOUNT;

public:
	static CHealingSmokeCenter* Create(Vector vecOrigin, CBasePlayer* pDoctor, float flRadius = 275.0f, float flTimeRemoved = 25.5f);

public:
	void Precache();
	void Think();

public:
	CBasePlayer* m_pPlayer;
	float m_flTimeRemoved;
	float m_flNextHealingScan;
	float m_flRadius;
};

namespace gOverHealingMgr
{
	extern const float OVERHEALING_DECAY_INTERVAL;
	extern const float OVERHEALING_DECAY_AMOUNT;
	extern const char* OVERHEALING_DECAY_SFX;
	extern const float OVERHEALING_MAX_RATIO;	// overhealing cannot have pev->health greater than this value * pev->max_health.

	void Think(CBasePlayer* pPlayer);
};

namespace gFrozenDOTMgr
{
	extern const char* ICEGRE_NOVA_SFX;
	extern const char* ICEGRE_FLESH_SFX;
	extern const char* ICEGRE_BREAKOUT_SFX;
	extern const int ICE_BLOOD_COLOUR;

	void Set(CBasePlayer* pPlayer, int iDamage, entvars_t* pevInflictor, entvars_t* pevAttacker, float flFreezeTime);
	void Think(CBasePlayer* pPlayer);
	void Free(CBasePlayer* pPlayer);
};
