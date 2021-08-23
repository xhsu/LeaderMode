/*

Created Date: Mar 13 2020

*/

#pragma once

/*
LUNA:	The concept here is quite simple:
		CWeaponBox is a extension and projection of CBaseWeapon for the GoldSrc engine due to the detachment of CBasePlayerItem from CBaseEntity.
		Thus, ONLY a CBaseWeapon function can call and/or create CWeaponBox entity.
*/

class CWeaponBox : public CBaseEntity
{
	static constexpr float THROWING_FORCE = 350;

public:
	virtual void Spawn();
	virtual void Precache();
	virtual void KeyValue(KeyValueData* pkvd);
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

	bool PackWeapon(IWeapon* pWeapon);

public:
	bool m_bHadBeenSold : 1 { false };
	float m_flNextPhysSFX{ 0.0f };

	struct
	{
		WeaponIdType what{ WEAPON_NONE };
		uint32 flags{ 0U };
		short clip{ 0 };
	}
	m_StoredWeapon;

	struct
	{
		AmmoIdType what{ AMMO_NONE };
		short count{ 0 };
	}
	m_StoredAmmo;
};
