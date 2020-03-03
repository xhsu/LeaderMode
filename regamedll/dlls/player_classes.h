/*

Created date: 02/26/2020

*/


#pragma once

#define THE_GODFATHER CSGameRules()->m_rgpCharacters[Role_Godfather]
#define THE_COMMANDER CSGameRules()->m_rgpCharacters[Role_Commander]

class CBasePlayer;

enum RoleTypes
{
	Role_UNASSIGNED = 0,

	Role_Commander = 1,
	// WeaponEnhance:	CSkillFireRate
	// Attack:			-
	// Defense:			CSkillReduceDamage
	// Auxiliary:		CSkillRadarScan

	Role_SWAT,
	// WeaponEnhance:	-
	// Attack:			-
	// Defense:			CSkillBulletproof
	// Auxiliary:		CSkillArmorRegen

	Role_Breacher,
	// WeaponEnhance:	CSkillExplosiveBullets
	// Attack:			CSkillInfiniteGrenade
	// Defense:			-
	// Auxiliary:		-

	Role_Sharpshooter,
	// WeaponEnhance:	CSkillEnfoceHeadshot
	// Attack:			CSkillHighlightSight
	// Defense:			-
	// Auxiliary:		-

	Role_Medic,
	// WeaponEnhance:	-
	// Attack:			-
	// Defense:			-
	// Auxiliary:		CSkillHealingShot

	Role_Godfather = 6,
	// WeaponEnhance:	
	// Attack:			
	// Defense:			CSkillReduceDamage
	// Auxiliary:		

	Role_Berserker,
	// WeaponEnhance:	
	// Attack:			
	// Defense:			
	// Auxiliary:		

	Role_MadScientist,
	// WeaponEnhance:	
	// Attack:			
	// Defense:			
	// Auxiliary:		

	Role_Assassin,
	// WeaponEnhance:	
	// Attack:			
	// Defense:			
	// Auxiliary:		

	Role_Arsonist,
	// WeaponEnhance:	
	// Attack:			
	// Defense:			CSkillReduceDamage
	// Auxiliary:		

	ROLE_COUNT
};

enum SkillType
{
	Skill_UNASSIGNED = 0,

	Skill_WeaponEnhance,
	Skill_Attack,
	Skill_Defense,
	Skill_Auxiliary,

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
	static const char* CRETICAL_SHOT_SFX;

	static int m_idBulletTrace;

	static void	Precache();

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
	virtual void OnGrenadeThrew(WeaponIdType iId, CGrenade* pGrenade) { }

	// passive skill: damage
	virtual float PlayerDamageSufferedModifier(int bitsDamageTypes) { return 1.0f; }
	virtual float PlayerDamageDealtModifier(int bitsDamageTypes) { return 1.0f; }
	virtual void OnPlayerDamagedPre(float& flDamage) { }
	virtual void OnTraceDamagePre(float& flDamage, TraceResult& tr) { }
	virtual void OnFireBullets3PreDamage(float& flDamage, TraceResult& tr) { }
	virtual void OnFireBuckshotsPreTraceAttack(float& flDamage, TraceResult& tr) { }
	virtual void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr) { }

	// passive skill: death
	virtual void OnPlayerDeath(CBasePlayer* pKiller) { Terminate(); }
	virtual void OnPlayerKills(CBasePlayer* pVictim) { }

	// passive skill: sight
	virtual bool OnBlind() { return true; }	// return false to block the blind.
	virtual void OnAddToFullPack(entity_state_s* pState, edict_t* pEnt, BOOL FIsPlayer) { }
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
	SkillType Classify() const	{ return Skill_Auxiliary; }
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

// Role_Commander: Stainless Steel
// Role_Godfather: Rockstand
// Role_Arsonist: [Passive] Nonflammable
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

// Role_SWAT: Bulletproof
class CSkillBulletproof : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const char* ACTIVATION_SFX;
	static const char* CLOSURE_SFX;
	static const float GIFT_RADIUS;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const { return "Bulletproof"; }
	SkillType Classify() const { return Skill_Defense; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }

	void OnTraceDamagePre(float& flDamage, TraceResult& tr);
};

// Role_SWAT: Liquid Armor
class CSkillArmorRegen : public CBaseSkill
{
public:
	CSkillArmorRegen() { m_bShouldSelfArmourRegenPlaySFX = true; }

public:
	static const float GIFT_RADIUS;
	static const float GIFT_INTERVAL;
	static const float GIFT_AMOUNT;
	static const char* GIFT_SFX;
	static const char* SELF_REGEN_SFX;
	static const float SELF_REGEN_AFTER_DMG;
	static const float SELF_REGEN_INTERVAL;
	static const float SELF_REGEN_AMOUNT;

public:
	float m_flNextArmourOffer;
	float m_flNextSelfArmourRegen;
	bool m_bShouldSelfArmourRegenPlaySFX;

public:
	void Think();

	const char* GetName() const { return "[Passive] Liquid Armor"; }
	SkillType Classify() const { return Skill_Auxiliary; }

	void OnPlayerDamagedPre(float& flDamage);
};

// Role_Breacher: Explosive Bullets
class CSkillExplosiveBullets : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const int EXPLO_LIGHT_RAD;
	static const float EXPLO_RADIUS;
	static const float EXPLO_DAMAGE;
	static const float RETAINED_DMG_PER_BULLET;
	static const float RETAINED_RAD_PER_BULLET;
	static int m_rgidSmokeSprite[4];

public:
	int m_rgiRetainedBullets[33];

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const { return "Explosive Bullets"; }
	SkillType Classify() const { return Skill_WeaponEnhance; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }

	void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr);
	void OnPlayerDeath(CBasePlayer* pKiller);
	void OnPlayerKills(CBasePlayer* pVictim);
};

// Role_Breacher: Bombard
class CSkillInfiniteGrenade : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const char* ACTIVATION_SFX;
	static const char* CLOSURE_SFX;
	static const float SELF_EXPLO_DMG_MUL;
	static const float DEALT_EXPLO_DMG_MUL;
	static const float GIFT_RADIUS;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const { return "Bombard"; }
	SkillType Classify() const { return Skill_Attack; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }

	float PlayerDamageSufferedModifier(int bitsDamageTypes) { return (m_bUsingSkill && (bitsDamageTypes & (DMG_EXPLOSION | DMG_BLAST))) ? SELF_EXPLO_DMG_MUL : 1.0f; }
	float PlayerDamageDealtModifier(int bitsDamageTypes) { return (bitsDamageTypes & (DMG_EXPLOSION | DMG_BLAST)) ? DEALT_EXPLO_DMG_MUL : 1.0f; }
	void OnGrenadeThrew(WeaponIdType iId, CGrenade* pGrenade);
};

// Role_Sharpshooter: Bullseye
class CSkillEnfoceHeadshot : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const char* ACTIVATION_SFX;
	static const char* CLOSURE_SFX;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const { return "Bullseye"; }
	SkillType Classify() const { return Skill_WeaponEnhance; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }

	void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr);
};

// Role_Sharpshooter: Glitter Dust
class CSkillHighlightSight : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const { return "Glitter Dust"; }
	SkillType Classify() const { return Skill_Attack; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }

	bool OnBlind() { return !m_bUsingSkill; }	// return false to block the blind.
	void OnAddToFullPack(entity_state_s* pState, edict_t* pEnt, BOOL FIsPlayer);
};

// Role_Medic: Healing Dart
class CSkillHealingShot : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const float DMG_HEAL_CONVERTING_RATIO;
	static const char* HEALINGSHOT_SFX;
	static const Vector HEALING_COLOR;
	static int m_idHealingSpr;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const { return "Healing Dart"; }
	SkillType Classify() const { return Skill_Auxiliary; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }

	void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr);
};
