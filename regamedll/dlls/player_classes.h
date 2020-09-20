/*

Created date: 02/26/2020
Segmentation Date: Sep 20 2020

*/


#pragma once


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
	SkillType_UNASSIGNED = 0,

	SkillType_WeaponEnhance,
	SkillType_Attack,
	SkillType_Defense,
	SkillType_Auxiliary,

	SKILLTYPE_COUNT
};

enum SkillIndex
{
	SkillIndex_ERROR = 0,

	// Role_Commander
	SkillIndex_RadarScan,
	SkillIndex_FireRate,
	SkillIndex_ReduceDamage,

	// Role_SWAT
	SkillIndex_Bulletproof,
	SkillIndex_ArmorRegen,

	// Role_Breacher
	SkillIndex_ExplosiveBullets,
	SkillIndex_InfiniteGrenade,

	// Role_Sharpshooter
	SkillIndex_EnfoceHeadshot,
	SkillIndex_HighlightSight,

	// Role_Medic
	SkillIndex_HealingShot,

	// Role_Godfather
	SkillIndex_Gavelkind,

	// Role_LeadEnforcer
	SkillIndex_DmgIncByHP,
	SkillIndex_ResistDeath,

	// Role_MadScientist
	SkillIndex_TaserGun,
	SkillIndex_Retribution,

	// Role_Assassin
	SkillIndex_Invisible,
	SkillIndex_CriticalHit,
	SkillIndex_RadarScan2,

	// Role_Arsonist
	SkillIndex_IncendiaryAmmo,

	SKILL_COUNT
};

struct SkillInfo
{
	SkillIndex	m_iSkillIndex;
	SkillType	m_iSkillType;
	const char* m_pszName;
	float		m_flDuration;
	float		m_flCooldown;
};

extern const char* g_rgszRoleNames[ROLE_COUNT];
extern const int g_rgRoleWeaponsAccessibility[ROLE_COUNT][LAST_WEAPON];
extern const int g_rgRoleEquipmentsAccessibility[ROLE_COUNT][EQP_COUNT];
extern const SkillInfo g_rgSkillInfo[SKILL_COUNT];
