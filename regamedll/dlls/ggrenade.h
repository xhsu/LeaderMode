/*

Created Date: Mar 13 2020

*/

#pragma once

class CGrenade : public CBaseMonster
{
public:
	static const float FROST_GR_DAMAGE;
	static const float FROST_GR_RADIUS;
	static const float FROST_GR_EFTIME;

public:
	virtual void Spawn();
	virtual int Save(CSave& save);
	virtual int Restore(CRestore& restore);
	virtual int ObjectCaps() { return 0; }
	virtual void Killed(entvars_t* pevAttacker, int iGib);
	virtual int BloodColor() { return DONT_BLEED; }
	virtual void BounceSound();

public:

	static CGrenade* ShootTimed(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity, float time);
	static CGrenade* ShootTimed2(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity, float time, int iTeam, unsigned short usEvent);
	static CGrenade* ShootSmokeGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity, float time, unsigned short usEvent);

	// skill grenade
	static CGrenade* FrostGrenade(CBasePlayer* pPlayer);
	static CGrenade* HealingGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity, float time, unsigned short usEvent);
	static CGrenade* NerveGasGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity, float time, unsigned short usEvent);
	static CGrenade* IncendiaryGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity);

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
	void EXPORT BounceTouch(CBaseEntity* pOther);
	void EXPORT SlideTouch(CBaseEntity* pOther);
	void EXPORT ExplodeTouch(CBaseEntity* pOther);
	void EXPORT DangerSoundThink();
	void EXPORT PreDetonate();
	void EXPORT Detonate();
	void EXPORT SG_Detonate();
	void EXPORT Detonate3();
	void EXPORT DetonateUse(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	void EXPORT TumbleThink();
	void EXPORT SG_TumbleThink();

	void EXPORT FrostTouch(CBaseEntity* pOther);
	void EXPORT IncendiaryTouch(CBaseEntity* pOther);
	void EXPORT IncendiaryThink();

public:
	static TYPEDESCRIPTION m_SaveData[];

	bool m_bJustBlew;
	int m_iTeam;
	int m_iCurWave;
	int m_SGSmoke;
	int m_angle;
	unsigned short m_usEvent;
	bool m_bLightSmoke;
	bool m_bDetonated;
	Vector m_vSmokeDetonate;
	int m_iBounceCount;
	BOOL m_fRegisteredSound;
	bool m_bHealing;
	bool m_bPoisoned;
};
