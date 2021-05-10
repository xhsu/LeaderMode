/*

Created date: 02/26/2020
Segmentation Date: Sep 20 2020

*/


#pragma once


enum RoleTypes : BYTE
{
	Role_UNASSIGNED = 0,

	Role_Commander = 1,
	/**
	* Skill Set:
	[Information centre, lead of assault.]
	[LUNA: will become a shield eventually.]
		WeaponEnhance:	CSkillFireRate
		Attack:			-
		Defense:		CSkillReduceDamage
		Auxiliary:		CSkillRadarScan

	* Weapon Choice:
	[Any.]
		Encouraged:	-
		Allowed:	[The rest part.], HE, FB, SG
		Penalized:	M14, SVD, Lmgs.

	* Weapon Style:
		Regular.
	**/

	Role_SWAT,
	/**
	* Skill Set:
	[Heavy infantry and frontline-defender. Armour, protection & team ammunition supply.]
		WeaponEnhance:	-
		Attack:			-
		Defense:		CSkillBulletproof
		Auxiliary:		CSkillArmorRegen

	* Weapon Choice:
	[Gears used in counter-terrorism operations.]
	[LUNA: his access to Assault Rifles should be reevaluated.]
		Encouraged:	Shotguns, Smgs, FB, SG
		Allowed:	Pistols
		Penalized:	SCARH, XM8

	* Weapon Style:
	[Modifications used in counter-terrorism operations.]
		Tactically modified.
	**/

	Role_Breacher,
	/**
	* Skill Set:
	[AOE damage inflictor.]
		WeaponEnhance:	CSkillExplosiveBullets
		Attack:			CSkillInfiniteGrenade
		Defense:		-
		Auxiliary:		-

	* Weapon Choice:
	[Shotguns inserted with HE rounds.]
	[LUNA: Independent GR launcher and attached GR launcher should be considered.]
	[LUNA: C4 & detonator pending.]
		Encouraged:	Shotguns, P90, HE, FB, SG
		Allowed:	Pistols, Smgs
		Penalized:	-

	* Weapon Style:
	[Grenade launcher, C4, HE rounds, and stuffs like those.]
		Explosive-related modifications.
	**/

	Role_Sharpshooter,
	/**
	* Skill Set:
	[Located, Freezed & Snip down.]
		WeaponEnhance:	CSkillEnfoceHeadshot
		Attack:			CSkillHighlightSight
		Defense:		-
		Auxiliary:		-

	* Weapon Choice:
	[Sniper rifles, high damage & precision weapons.]
		Encouraged:	ANACO, DEAGLE, M200, AWP
		Allowed:	Pistols, M14, SVD, SG, CRYO
		Penalized:	Assault rifles

	* Weapon Style:
	[Mounted with laser and optical scopes.]
		Precision modifications.
	**/

	Role_Medic,
	/**
	* Skill Set:
	[Heal up teammates and elongate assault operations.]
		WeaponEnhance:	-
		Attack:			-
		Defense:		-
		Auxiliary:		CSkillHealingShot

	* Weapon Choice:
	[Simple, self-defense & user-friendly weapons.]
		Encouraged:	Smgs
		Allowed:	Pistols, Shotguns, M4A1, SCARH, HSG
		Penalized:	-

	* Weapon Style:
	[Simple modifications making weapon easy to use.]
		Foregrip, simple optical sight.
	**/

	Role_Godfather = 6,
	/**
	* Skill Set:
	[Supporter, boost his teammates before their assault charging.]
	[LUNA: Dual-pistol pending.]
		WeaponEnhance:	-
		Attack:			-
		Defense:		CSkillReduceDamage
		Auxiliary:		CSkillGavelkind

	* Weapon Choice:
	[Any.]
		Encouraged:	Pistols
		Allowed:	[The rest part.], HE, FB, SG
		Penalized:	M14, SVD, Lmgs.

	* Weapon Style:
	[Dual pistols. Luxury skin.]
		Retexture with luxury skin.
	**/

	Role_LeadEnforcer,
	/**
	* Skill Set:
	[Sacrifice himself to make a breakthrough point for the team.]
		WeaponEnhance:	CSkillDmgIncByHP
		Attack:			CSkillResistDeath
		Defense:		-
		Auxiliary:		-

	* Weapon Choice:
	[Any full-automatic firearms are welcomed.]
		Encouraged:	Shotguns, Smgs, Assault rifles
		Allowed:	Pistols, Lmgs, HE
		Penalized:	-

	* Weapon Style:
	[No scope or any expensive modification.]
		Simple muzzle modification without anything else.
	**/

	Role_MadScientist,
	/**
	* Skill Set:
	[Blocker. Drag & slow the other team down. Occasionally weaken them.]
		WeaponEnhance:	CSkillTaserGun
		Attack:			-
		Defense:		CSkillRetribution
		Auxiliary:		-

	* Weapon Choice:
	[Extremely limited weapon choices. Simple self-defense weapons only.]
		Encouraged:	-
		Allowed:	USP, ANACO, FN57, M45A1, KSG12, UMP45, TOX
		Penalized:	-

	* Weapon Style:
	[Hi-tech retexture. Modifications makes it easier to use.]
	[LUNA: Pending. Overlapping with sharpshooter?]
		Helper scope, laser.
	**/

	Role_Assassin,	// [Hitman]
	/**
	* Skill Set:
	[Information collector, sneaky approach, hit & run.]
		WeaponEnhance:	CSkillCriticalHit
		Attack:			-
		Defense:		CSkillInvisible
		Auxiliary:		CSkillRadarScan2

	* Weapon Choice:
	[Handy & conceal weapons.]
		Encouraged:	USP, MP7A1, M200
		Allowed:	Pistols, Smgs, FB, SG
		Penalized:	M14

	* Weapon Style:
	[Modifications that make assassination easier.]
		Silencer and scope.
	**/

	Role_Arsonist,
	/**
	* Skill Set:
	[Blocker. Trap, damage & weaken enemies.]
		WeaponEnhance:	CSkillIncendiaryAmmo
		Attack:			-
		Defense:		CSkillReduceDamage
		Auxiliary:		-

	* Weapon Choice:
	[Weapons that filled with incendiary ammo.]
	[LUNA: Independent flamethrower?]
		Encouraged:	Shotguns
		Allowed:	Pistols, SG, INCEN
		Penalized:	AK47

	* Weapon Style:
	[Burner skin. Fire-related attachments.]
		Retexture with bondage and burn marks. Flamethrower attachments?
	**/

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
