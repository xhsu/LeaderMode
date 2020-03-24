/*

Created Date: Mar 13 2020

*/

#pragma once

class CGrenade : public CBaseMonster
{
public:
	static const float CRYOGR_DAMAGE;
	static const float CRYOGR_RADIUS;
	static const float CRYOGR_EFTIME;

public:
	virtual void Spawn();
	virtual void Precache();
	virtual int Save(CSave& save);
	virtual int Restore(CRestore& restore);
	virtual int ObjectCaps() { return 0; }
	virtual void Killed(entvars_t* pevAttacker, int iGib);
	virtual int BloodColor() { return DONT_BLEED; }
	virtual void BounceSound();

public:
	static CGrenade* Flashbang(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity);
	static CGrenade* HEGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity, int iTeam);
	static CGrenade* SmokeGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity);

	// skill grenade
	static CGrenade* Cryogrenade(CBasePlayer* pPlayer);
	static CGrenade* HealingGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity);
	static CGrenade* NerveGasGrenade(entvars_t* pevOwner, Vector vecStart, Vector vecVelocity);
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

	void EXPORT CryoTouch(CBaseEntity* pOther);
	void EXPORT IncendiaryTouch(CBaseEntity* pOther);
	void EXPORT IncendiaryThink();

public:
	static TYPEDESCRIPTION m_SaveData[];
	static unsigned short m_rgusEvents[];
	static const float m_rgflFuseTime[];

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
