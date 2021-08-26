/*

Created Date: Mar 13 2020

*/

#pragma once

extern int g_iSpriteGroundexp1;
extern int g_iSpriteZerogxplode2;

// spawn flags
#define SF_DETONATE                 BIT(0) // Grenades flagged with this will be triggered when the owner calls detonateSatchelCharges

// combat.cpp
float GetAmountOfPlayerVisible(Vector vecSrc, CBaseEntity* pEntity);

class CGrenade : public CBaseMonster
{
public:
	static constexpr float CRYOGR_DAMAGE = 20.0f;
	static constexpr float CRYOGR_RADIUS = 240.0f;
	static constexpr float CRYOGR_EFTIME = 4.0f;

	static constexpr float C4_INIT_SPEED = 290.0f;
	static constexpr float C4_EXPLO_RADIUS = 300.0f;
	static constexpr float C4_EXPLO_DAMAGE = 500.0f;

public:
	virtual void Spawn();
	virtual void Precache();
	virtual int Save(CSave& save);
	virtual int Restore(CRestore& restore);
	virtual int ObjectCaps() { return FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE; }
	virtual void Killed(entvars_t* pevAttacker, int iGib);
	virtual int BloodColor() { return DONT_BLEED; }
	virtual void BounceSound();
	virtual void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType = USE_OFF, float value = 0.0f);	// this is not the original diarm C4.

public:
	static CGrenade* Flashbang(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity);
	static CGrenade* HEGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity, int iTeam);
	static CGrenade* SmokeGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity);

	// skill grenade
	static CGrenade* Cryogrenade(CBasePlayer* pPlayer);
	static CGrenade* HealingGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity);
	static CGrenade* NerveGasGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity);
	static CGrenade* IncendiaryGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity);
	static CGrenade* C4(CBasePlayer* pPlayer);

public:
	void Explode(Vector vecSrc, Vector vecAim);
	void Explode(TraceResult* pTrace, int bitsDamageType);
	void Explode3(TraceResult* pTrace, int bitsDamageType);

	void EXPORT Smoke();
	void EXPORT Smoke2();
	void EXPORT Smoke3_A();
	void EXPORT Smoke3_B();
	void EXPORT Smoke3_C();
	void EXPORT SG_Smoke();

	void EXPORT Detonate();
	void EXPORT SG_Detonate();
	void EXPORT HE_Detonate();
	void EXPORT C4_Detonate();
	static void EXPORT C4_Detonate(CBasePlayer* pPlayer);	// detonate all C4 belongs to a player.

	void EXPORT TumbleThink();
	void EXPORT SG_TumbleThink();
	void EXPORT IncendiaryThink();
	void EXPORT C4PickUpThink();

	void EXPORT BounceTouch(CBaseEntity* pOther);
	void EXPORT SlideTouch(CBaseEntity* pOther);
	void EXPORT ExplodeTouch(CBaseEntity* pOther);
	void EXPORT CryoTouch(CBaseEntity* pOther);
	void EXPORT IncendiaryTouch(CBaseEntity* pOther);
	void EXPORT C4Touch(CBaseEntity* pOther);

public:
	static TYPEDESCRIPTION m_SaveData[];
	static unsigned short m_rgusEvents[];
	static const float m_rgflFuseTime[];

	enum
	{
		ERROR_GR = 0,
		HE = 1,
		FLASHBANG,
		SMOKE,
		CRYO,
		HEALING,
		NERVE_GAS,
		INCENDIARY,
		RCC4,
	}
	m_iType{ ERROR_GR };

	int m_iTeam;
	int m_iCurWave;
	int m_SGSmoke;
	int m_angle;
	unsigned short m_usEvent{ 0 };
	bool m_bLightSmoke;
	bool m_bDetonated;
	Vector m_vSmokeDetonate;
	int m_iBounceCount;
	BOOL m_fRegisteredSound;
	Vector m_vecAttachedSurfaceNorm{ g_vecZero };
	bool m_bC4BeingPickingUp{ false };
};
