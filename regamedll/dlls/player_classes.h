/*

Created date: 02/26/2020

*/


#pragma once

#define THE_GODFATHER CSGameRules()->m_rgpLeaders[TERRORIST]
#define THE_COMMANDER CSGameRules()->m_rgpLeaders[CT]

class CBasePlayer;

enum RoleTypes
{
	Role_UNASSIGNED = 0,

	Role_Commander = 1,
	Role_SWAT,
	Role_Breacher,
	Role_Sharpshooter,
	Role_Medic,

	Role_Godfather = 6,
	Role_Berserker,
	Role_MadScientist,
	Role_Assassin,
	Role_Arsonist,

	ROLE_COUNT
};

enum SkillType
{
	Skill_UNASSIGNED = 0,

	Skill_WeaponEnhance,
	Skill_Attack,
	Skill_Defense,
	Skill_Intel,

	SKILLTYPE_COUNT
};

extern const char* g_rgszRoleNames[ROLE_COUNT];
extern const int g_rgRoleWeaponsAccessibility[ROLE_COUNT][LAST_WEAPON];

class CBaseSkill
{
public:
	// avoid the complex memset();
	void* operator new(size_t size)
	{
		return calloc(1, size);
	}
	void operator delete(void* ptr)
	{
		free(ptr);
	}
	CBaseSkill() {}
	virtual ~CBaseSkill() {}

	template <typename T>
	static T* Grand(CBasePlayer* pPlayer)
	{
		T* p = new T;

		if (pPlayer->m_rgpSkills[p->Classify()])
		{
			delete p;
			return nullptr;
		}

		p->m_bUsingSkill = false;
		p->m_bAllowSkill = true;
		p->m_flTimeCooldownOver = gpGlobals->time;
		p->m_flTimeLastUsed = -1.0;
		p->m_pPlayer = pPlayer;

		pPlayer->m_rgpSkills[p->Classify()] = p;

		return p;
	}

public:
	static const char* RADAR_BEEP_SFX;
	static const char* RADAR_TARGET_DEAD_SFX;
	static const char* COOLDOWN_COMPLETE_SFX;

public:
	bool m_bUsingSkill;
	bool m_bAllowSkill;
	float m_flTimeCooldownOver;
	float m_flTimeLastUsed;
	CBasePlayer* m_pPlayer;

public:	// skill action
	virtual bool Execute()		{ return false; }
	virtual void Think()		{ }
	virtual bool Terminate()	{ return false; }

	// skill info
	virtual const char* GetName() const		{ return "NULL skill!!"; }
	virtual SkillType Classify() const		{ return Skill_UNASSIGNED; }
	virtual float GetDuration() const		{ return 1.0f; }
	virtual float GetCooldown() const		{ return 1.0f; }

	// skill hud
	virtual float GetHudPercentage() const;

	// passive skill: weapons
	virtual float WeaponFireIntervalModifier(CBasePlayerWeapon* pWeapon) { return 1.0f; }
	virtual float PlayerDamageSufferedModifier(int bitsDamageTypes) { return 1.0f; }
};

// Role_Commander: Battlefield Analysis
class CSkillRadarScan : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const float UPDATE_INTERVAL;
	static const char* ACTIVATION_SFX;
	static const char* CLOSURE_SFX;

public:
	EntityHandle<CBasePlayer> m_pTracing;
	float m_flNextRadarUpdate;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const	{ return "Battlefield Analysis"; }
	SkillType Classify() const	{ return Skill_Intel; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }
};

// Role_Commander: Haste
class CSkillFireRate : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const float FIREINTERVAL_MODIFIER;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const { return "Haste"; }
	SkillType Classify() const { return Skill_WeaponEnhance; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }

	float WeaponFireIntervalModifier(CBasePlayerWeapon* pWeapon) { return m_bUsingSkill ? FIREINTERVAL_MODIFIER : 1.0f; }
};

//
// Role_Commander: Stainless Steel
// Role_Godfather: Rockstand
//
class CSkillReduceDamage : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const float DAMAGE_MODIFIER;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const;
	SkillType Classify() const { return Skill_Defense; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }

	float PlayerDamageSufferedModifier(int bitsDamageTypes);
};
