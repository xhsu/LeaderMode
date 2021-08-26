/*

Segmentation Date: Sep 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once


#include <list>

#define THE_GODFATHER CSGameRules()->m_rgpCharacters[Role_Godfather]
#define THE_COMMANDER CSGameRules()->m_rgpCharacters[Role_Commander]

class CBasePlayer;


class CBaseSkill
{
public:
	// avoid the complex memset();
	void* operator new(size_t size)	{ return calloc(1, size); }
	void operator delete(void* ptr) { return free(ptr); }
	CBaseSkill() {}
	virtual ~CBaseSkill() {}

	template <typename T>
	static T* Grand(CBasePlayer* pPlayer);

public:
	static constexpr auto RADAR_BEEP_SFX = "leadermode/nes_8bit_alien3_radar_beep1.wav";
	static constexpr auto RADAR_TARGET_DEAD_SFX = "leadermode/sfx_event_duel_win_01.wav";
	static constexpr auto COOLDOWN_COMPLETE_SFX = "leadermode/pope_accepts_crusade_arrived.wav";
	static constexpr auto CRITICAL_SHOT_SFX = "leadermode/siege_attack.wav";

	static inline int m_idBulletTrace = 0;

	static void	Precache();

public:
	bool m_bUsingSkill : 1;
	bool m_bAllowSkill : 1;
	float m_flTimeCooldownOver;
	float m_flTimeLastUsed;
	CBasePlayer* m_pPlayer;

public:	// skill action
	virtual bool Execute() { return false; }
	virtual void Think() { }
	virtual bool Terminate() { return false; }
	virtual void Discharge(CBasePlayer* pCause) { Terminate(); }

	// skill info
	virtual const char* GetName() const { return g_rgSkillInfo[GetIndex()].m_pszName; }
	virtual SkillType Classify() const { return g_rgSkillInfo[GetIndex()].m_iSkillType; }
	virtual float GetDuration() const { return g_rgSkillInfo[GetIndex()].m_flDuration; }
	virtual float GetCooldown() const { return g_rgSkillInfo[GetIndex()].m_flCooldown; }
	virtual SkillIndex GetIndex() const { return SkillIndex_ERROR; }

	// skill hud
	virtual float GetHudPercentage() const;
	virtual float GetCountingTime() const;
	virtual bool  IsCoolingDown() const { return (!m_bUsingSkill && !m_bAllowSkill); }
	virtual bool  IsReady()	const { return (!m_bUsingSkill && m_bAllowSkill); }

	// utils
	virtual void TerminatePeers() const;

	// passive skill: weapons
	virtual float WeaponFireIntervalModifier(IWeapon* pWeapon) { return 1.0f; }
	virtual void OnGrenadeThrew(EquipmentIdType iId, CGrenade* pGrenade) { }

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

	virtual SkillIndex GetIndex() const { return SkillIndex_RadarScan; }
};

// Role_Commander: Haste
class CSkillFireRate : public CBaseSkill
{
public:
	static const float FIREINTERVAL_MODIFIER;

public:
	bool Execute();
	void Think();
	bool Terminate();

	SkillIndex GetIndex() const { return SkillIndex_FireRate; }

	float WeaponFireIntervalModifier(IWeapon* pWeapon) { return m_bUsingSkill ? FIREINTERVAL_MODIFIER : 1.0f; }
};

// Role_Commander: Stainless Steel
// Role_Godfather: Rockstand
// Role_Arsonist: [Passive] Nonflammable
class CSkillReduceDamage : public CBaseSkill
{
public:
	static const float DAMAGE_MODIFIER;

public:
	bool Execute();
	void Think();
	bool Terminate();

	const char* GetName() const;
	SkillIndex GetIndex() const { return SkillIndex_ReduceDamage; }

	float PlayerDamageSufferedModifier(int bitsDamageTypes);
};

// Role_SWAT: Bulletproof
class CSkillBulletproof : public CBaseSkill
{
public:
	static const char* ACTIVATION_SFX;
	static const char* CLOSURE_SFX;
	static const float GIFT_RADIUS;

public:
	bool Execute();
	void Think();
	bool Terminate();

	SkillIndex GetIndex() const { return SkillIndex_Bulletproof; }

	void OnTraceDamagePre(float& flDamage, TraceResult& tr);
};

// Role_SWAT: Liquid Armor
class CSkillArmorRegen : public CBaseSkill
{
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
	bool m_bShouldSelfArmourRegenPlaySFX{ true };

public:
	void Think();

	SkillIndex GetIndex() const { return SkillIndex_ArmorRegen; }

	void OnPlayerDamagedPre(entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int& bitsDamageTypes);
};

// Role_Breacher: Explosive Bullets
class CSkillExplosiveBullets : public CBaseSkill
{
public:
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

	SkillIndex GetIndex() const { return SkillIndex_ExplosiveBullets; }

	void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr);
	void OnPlayerDeath(CBasePlayer* pKiller);
	void OnPlayerKills(CBasePlayer* pVictim);
};

// Role_Breacher: Bombard
class CSkillInfiniteGrenade : public CBaseSkill
{
public:
	static const char* ACTIVATION_SFX;
	static const char* CLOSURE_SFX;
	static const float SELF_EXPLO_DMG_MUL;
	static const float DEALT_EXPLO_DMG_MUL;
	static const float GIFT_RADIUS;

public:
	bool Execute();
	void Think();
	bool Terminate();

	SkillIndex GetIndex() const { return SkillIndex_InfiniteGrenade; }

	float PlayerDamageSufferedModifier(int bitsDamageTypes) { return (m_bUsingSkill && (bitsDamageTypes & (DMG_EXPLOSION | DMG_BLAST))) ? SELF_EXPLO_DMG_MUL : 1.0f; }
	float PlayerDamageDealtModifier(int bitsDamageTypes) { return (bitsDamageTypes & (DMG_EXPLOSION | DMG_BLAST)) ? DEALT_EXPLO_DMG_MUL : 1.0f; }
	void OnGrenadeThrew(EquipmentIdType iId, CGrenade* pGrenade);
};

// Role_Sharpshooter: Bullseye
class CSkillEnfoceHeadshot : public CBaseSkill
{
public:
	static const char* ACTIVATION_SFX;
	static const char* CLOSURE_SFX;

public:
	bool Execute();
	void Think();
	bool Terminate();

	SkillIndex GetIndex() const { return SkillIndex_EnfoceHeadshot; }

	void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr);
};

// Role_Sharpshooter: Glitter Dust
class CSkillHighlightSight : public CBaseSkill
{
public:
	bool Execute();
	void Think();
	bool Terminate();

	SkillIndex GetIndex() const { return SkillIndex_HighlightSight; }

	bool OnBlind() { return !m_bUsingSkill; }	// return false to block the blind.
	void OnAddToFullPack(entity_state_s* pState, edict_t* pEnt, BOOL FIsPlayer);
};

// Role_Medic: Healing Dart
class CSkillHealingShot : public CBaseSkill
{
public:
	static const float DMG_HEAL_CONVERTING_RATIO;
	static const char* HEALINGSHOT_SFX;
	static const Vector HEALING_COLOR;
	static int m_idHealingSpr;

public:
	bool Execute();
	void Think();
	bool Terminate();

	SkillIndex GetIndex() const { return SkillIndex_HealingShot; }

	void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr);
};

// Role_Godfather: Baptism
class CSkillGavelkind : public CBaseSkill
{
public:
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

	SkillIndex GetIndex() const { return SkillIndex_Gavelkind; }
};

// Role_LeadEnforcer: Death Wish
class CSkillDmgIncByHP : public CBaseSkill
{
public:
	SkillIndex GetIndex() const { return SkillIndex_DmgIncByHP; }

	float PlayerDamageDealtModifier(int bitsDamageTypes);
};

// Role_LeadEnforcer: Swan Song
class CSkillResistDeath : public CBaseSkill
{
public:
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

	SkillIndex GetIndex() const { return SkillIndex_ResistDeath; }

	void OnPlayerDamagedPre(entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int& bitsDamageTypes);
	bool OnBlind() { return !m_bUsingSkill; }	// return false to block the blind.
	void OnResetPlayerMaxspeed(float& flSpeed);
};

// Role_MadScientist: Electromagnetic Bullets
class CSkillTaserGun : public CBaseSkill
{
public:
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

	SkillIndex GetIndex() const { return SkillIndex_TaserGun; }

	float WeaponFireIntervalModifier(IWeapon* pWeapon);
	void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr);
};

// Role_MadScientist: Electromagnetic Armour
class CSkillRetribution : public CBaseSkill
{
public:
	static const float RETRIBUTION_RATIO;

public:
	SkillIndex GetIndex() const { return SkillIndex_Retribution; }

	void OnPlayerDamagedPre(entvars_t* pevInflictor, entvars_t* pevAttacker, float& flDamage, int& bitsDamageTypes);
};

// Role_Assassin: Chameleon Cloak
class CSkillInvisible : public CBaseSkill
{
public:
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

	SkillIndex GetIndex() const { return SkillIndex_Invisible; }

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
	SkillIndex GetIndex() const { return SkillIndex_CriticalHit; }

	void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr);
};

// Role_Assassin: Inside Job
class CSkillRadarScan2 : public CBaseSkill
{
public:
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

	virtual SkillIndex GetIndex() const { return SkillIndex_RadarScan2; }
};

// Role_Arsonist: Dragon's Breath
class CSkillIncendiaryAmmo : public CBaseSkill
{
public:
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

	SkillIndex GetIndex() const { return SkillIndex_IncendiaryAmmo; }

	void OnHurtingAnotherPlayer(CBasePlayer* pVictim, entvars_t* pevInflictor, float& flDamage, int& bitsDamageTypes);
	void OnPlayerFiringTraceLine(int& iDamage, TraceResult& tr);
};
