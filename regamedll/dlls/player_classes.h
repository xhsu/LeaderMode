/*

Created date: 02/26/2020

*/


#pragma once

#include <list>

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
	// WeaponEnhance:	-
	// Attack:			-
	// Defense:			CSkillReduceDamage
	// Auxiliary:		CSkillGavelkind

	Role_LeadEnforcer,
	// WeaponEnhance:	CSkillDmgIncByHP
	// Attack:			CSkillResistDeath
	// Defense:			-
	// Auxiliary:		-

	Role_MadScientist,
	// WeaponEnhance:	CSkillTaserGun
	// Attack:			-
	// Defense:			CSkillRetribution
	// Auxiliary:		-

	Role_Assassin,
	// WeaponEnhance:	CSkillCriticalHit
	// Attack:			-
	// Defense:			CSkillInvisible
	// Auxiliary:		CSkillRadarScan2

	Role_Arsonist,
	// WeaponEnhance:	CSkillIncendiaryAmmo
	// Attack:			-
	// Defense:			CSkillReduceDamage
	// Auxiliary:		-

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
extern const int g_rgRoleEquipmentsAccessibility[ROLE_COUNT][EQP_COUNT];

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
	static const char* CRITICAL_SHOT_SFX;

	static int m_idBulletTrace;

	static void	Precache();

public:
	bool m_bUsingSkill;
	bool m_bAllowSkill;
	float m_flTimeCooldownOver;
	float m_flTimeLastUsed;
	CBasePlayer* m_pPlayer;

public:	// skill action
	virtual bool Execute()						{ return false; }
	virtual void Think()						{ }
	virtual bool Terminate()					{ return false; }
	virtual void Discharge(CBasePlayer* pCause) { Terminate(); }

	// skill info
	virtual const char* GetName() const		{ return "NULL skill!!"; }
	virtual SkillType Classify() const		{ return Skill_UNASSIGNED; }
	virtual float GetDuration() const		{ return 1.0f; }
	virtual float GetCooldown() const		{ return 1.0f; }

	// skill hud
	virtual float GetHudPercentage() const;

	// passive skill: weapons
	virtual float WeaponFireIntervalModifier(CBaseWeapon* pWeapon) { return 1.0f; }
	virtual void OnGrenadeThrew(WeaponIdType iId, CGrenade* pGrenade) { }

	// passive skill: damage
	virtual float PlayerDamageSufferedModifier(int bitsDamageTypes) { return 1.0f; }
	virtual float PlayerDamageDealtModifier(int bitsDamageTypes) { return 1.0f; }
	virtual void OnPlayerDamagedPre(entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int& bitsDamageTypes) { }
	virtual void OnTraceDamagePre(float& flDamage, TraceResult& tr) { }
	virtual void OnFireBullets3PreDamage(float& flDamage, TraceResult& tr) { }
	virtual void OnFireBuckshotsPreTraceAttack(float& flDamage, TraceResult& tr) { }
	virtual void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr) { }
	virtual void OnHurtingAnotherPlayer(CBasePlayer* pVictim, entvars_t* pevInflictor, float& flDamage, int& bitsDamageTypes) { }

	// passive skill: death
	virtual void OnPlayerDeath(CBasePlayer* pKiller) { if (m_bUsingSkill) Terminate(); }
	virtual void OnPlayerKills(CBasePlayer* pVictim) { }

	// passive skill: visual
	virtual bool OnBlind() { return true; }	// return false to block the blind.
	virtual void OnAddToFullPack(entity_state_s* pState, edict_t* pEnt, BOOL FIsPlayer) { }
	virtual void OnBeingAddToFullPack(entity_state_s* pState, CBasePlayer* pHost) { }

	// passive skill: misc
	virtual void OnResetPlayerMaxspeed(float& flSpeed) { }
	virtual void OnTouched(CBaseEntity* pOther) { }
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
	virtual bool Execute();
	virtual void Think();
	virtual bool Terminate();

	virtual const char* GetName() const	{ return "Battlefield Analysis"; }
	virtual SkillType Classify() const	{ return Skill_Auxiliary; }
	virtual float GetDuration() const { return DURATION; }
	virtual float GetCooldown() const { return COOLDOWN; }
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

	float WeaponFireIntervalModifier(CBaseWeapon* pWeapon) { return m_bUsingSkill ? FIREINTERVAL_MODIFIER : 1.0f; }
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

	void OnPlayerDamagedPre(entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int& bitsDamageTypes);
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

// Role_Godfather: Baptism
class CSkillGavelkind : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const char* ACTIVATION_SFX;
	static const char* CLOSURE_SFX;
	static const char* PASSIVE_SFX;
	static const float RADIUS;
	static const float PASSIVE_HEALING_INTERVAL;
	static const float PASSIVE_HEALING_RADIUS;
	static const float PASSIVE_HEALING_AMOUNT;

public:
	typedef class godchild_c
	{
	public:
		godchild_c(CBasePlayer* pPlayer, float flOriginalHealth)
		{
			m_pGodchild = pPlayer;
			m_flOriginalHealth = flOriginalHealth;
		}

		EntityHandle<CBasePlayer> m_pGodchild;
		float m_flOriginalHealth;
	} godchild_t;

public:
	std::list<godchild_t> m_lstGodchildren;
	float m_flSavedDeltaHP;
	float m_flNextPassiveHealingThink;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const { return "Baptism"; }
	SkillType Classify() const { return Skill_Auxiliary; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }
};

// Role_LeadEnforcer: Death Wish
class CSkillDmgIncByHP : public CBaseSkill
{
public:
	const char* GetName() const { return "[Passive] Death Wish"; }
	SkillType Classify() const { return Skill_WeaponEnhance; }

	float PlayerDamageDealtModifier(int bitsDamageTypes);
};

// Role_LeadEnforcer: Swan Song
class CSkillResistDeath : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const char* ACTIVATION_SFX;
	static const float DASH_SPEED;
	static const float INJURE_SPEED;

public:
	EntityHandle<CBaseEntity> m_pLastAttacker;
	EntityHandle<CBaseEntity> m_pLastInflictor;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const { return "Swan Song"; }
	SkillType Classify() const { return Skill_Attack; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }

	void OnPlayerDamagedPre(entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int& bitsDamageTypes);
	bool OnBlind() { return !m_bUsingSkill; }	// return false to block the blind.
	void OnResetPlayerMaxspeed(float& flSpeed);
};

// Role_MadScientist: Electromagnetic Bullets
class CSkillTaserGun : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const char* ACTIVATION_SFX;
	static const char* STATIC_ELEC_SFX;
	static const char* ELECTROBULLETS_SFX;
	static const char* ELECTRIFY_SFX;
	static const float DRAG_SPEED;
	static const float ELEC_LASTING;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const { return "Electromagnetic Bullets"; }
	SkillType Classify() const { return Skill_WeaponEnhance; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }

	float WeaponFireIntervalModifier(CBaseWeapon* pWeapon);
	void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr);
};

// Role_MadScientist: Electromagnetic Armour
class CSkillRetribution : public CBaseSkill
{
public:
	static const float RETRIBUTION_RATIO;

public:
	const char* GetName() const { return "[Passive] Electromagnetic Armour"; }
	SkillType Classify() const { return Skill_Defense; }

	void OnPlayerDamagedPre(entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int& bitsDamageTypes);
};

// Role_Assassin: Chameleon Cloak
class CSkillInvisible : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const char* ACTIVATION_SFX;
	static const char* DISCOVERED_SFX;
	static const float SNEAKING_SPEED;
	static const float SNEAKING_GRAVITY;
	static const int HIDEHUD;
	static const Vector SCREEN_COLOUR;

public:
	bool Execute();
	void Think();
	bool Terminate();
	void Discharge(CBasePlayer* pCause);

	const char* GetName() const { return "Chameleon Cloak"; }
	SkillType Classify() const { return Skill_Defense; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }

	void OnPlayerDamagedPre(entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int& bitsDamageTypes);
	void OnBeingAddToFullPack(entity_state_s* pState, CBasePlayer* pHost);
	void OnResetPlayerMaxspeed(float& flSpeed) { if (m_bUsingSkill) flSpeed = SNEAKING_SPEED; }
	void OnTouched(CBaseEntity* pOther);
};

// Role_Assassin: Backlash
class CSkillCriticalHit : public CBaseSkill
{
public:
	static const float NORMAL_CHANCE;
	static const float BACKSTAB_CHANCE;
	static const int ALLOWED_WEAPONS;

public:
	const char* GetName() const { return "[Passive] Backlash"; }
	SkillType Classify() const { return Skill_WeaponEnhance; }

	void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr);
};

// Role_Assassin: Inside Job
class CSkillRadarScan2 : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const float UPDATE_DISTANCE_INTERVAL;
	static const float FORCE_TO_REMOVE_TIME;

public:
	EntityHandle<CBasePlayer> m_pTracing;
	Vector m_vecLastPosition;
	float m_flForcedToRemove;

public:
	virtual bool Execute();
	virtual void Think();
	virtual bool Terminate();

	virtual const char* GetName() const { return "Inside Job"; }
	virtual SkillType Classify() const { return Skill_Auxiliary; }
	virtual float GetDuration() const { return DURATION; }
	virtual float GetCooldown() const { return COOLDOWN; }
};

// Role_Arsonist: Dragon's Breath
class CSkillIncendiaryAmmo : public CBaseSkill
{
public:
	static const float DURATION;
	static const float COOLDOWN;
	static const char* ACTIVATION_SFX;
	static const char* CLOSURE_SFX;
	static const float IGNITE_DURATION;
	static const float DRAGING_MODIFIER;
	static int m_idSmokeTail;
	static int m_idSpark;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const { return "Dragon's Breath"; }
	SkillType Classify() const { return Skill_WeaponEnhance; }
	float GetDuration() const { return DURATION; }
	float GetCooldown() const { return COOLDOWN; }

	void OnHurtingAnotherPlayer(CBasePlayer* pVictim, entvars_t* pevInflictor, float& flDamage, int& bitsDamageTypes);
	void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr);
};
