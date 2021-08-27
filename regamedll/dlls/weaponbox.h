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
public:
	static constexpr float THROWING_FORCE = 350;

public:
	void Spawn() override;
	void Precache() override;
	void KeyValue(KeyValueData* pkvd) override;
	int ObjectCaps() override { return FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE; }
	void SetObjectCollisionBox() override;
	void TraceAttack(entvars_t* pevAttacker, float flDamage, Vector vecDir, TraceResult* ptr, int bitsDamageType) override;
	bool TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	bool GiveAmmo(int iCount, AmmoIdType iId) override;
	void Touch(CBaseEntity* pOther) override;
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType = USE_OFF, float value = 0.0f) override;

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
