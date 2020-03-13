/*

Created Date: Mar 13 2020

*/

#pragma once

class CWeaponBox : public CBaseEntity
{
	static const float THROWING_FORCE;

public:
	virtual void Spawn();
	virtual void Precache();
	virtual void KeyValue(KeyValueData* pkvd);
	virtual int Save(CSave& save);
	virtual int Restore(CRestore& restore);
	virtual int ObjectCaps() { return FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE; }
	virtual void SetObjectCollisionBox();
	virtual void TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType);
	virtual BOOL TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	virtual bool GiveAmmo(int iCount, AmmoIdType iId);
	virtual void Touch(CBaseEntity* pOther);
	virtual void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType = USE_OFF, float value = 0.0f);

public:
	bool IsEmpty();

	void EXPORT Kill();
	void SetModel(const char* pszModelName);

	bool HasWeapon(WeaponIdType iId);
	bool PackWeapon(CBaseWeapon* pWeapon);

public:
	static TYPEDESCRIPTION m_SaveData[];

	CBaseWeapon* m_rgpPlayerItems[MAX_ITEM_TYPES];
	int m_rgAmmo[MAX_AMMO_SLOTS];
	bool m_bHadBeenSold;
	float m_flNextPhysSFX;
};
