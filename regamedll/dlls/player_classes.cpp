/*

Created date: 02/26/2020
Segmentation Date: Sep 20 2020

*/

#include "precompiled.h"

const char* g_rgszRoleNames[ROLE_COUNT] =
{
	"Noobie",

	"Commander",
	"S.W.A.T.",
	"Breacher",
	"Sharpshooter",
	"Medic",

	"Godfather",
	"Lead Enforcer",
	"Mad Scientist",
	"Hitman",
	"Arsonist"
};

const int g_rgRoleWeaponsAccessibility[ROLE_COUNT][LAST_WEAPON] =
{
	//					      0                                  5                                  10                                 15                                 20                                 25                                 30
	//					      NONE   G18C   USP    ANACO  DEAGLE FN57   P99    KSG12  STR    MP7A1  PM9    MP5    UMP45  P90    CM901  QBZ95  AK47   M4A1   SCARL  ACR    M200   M14    AWP    SVD    MK46   C4
	/*Role_UNASSIGNED*/		{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F },

	/*Role_Commander = 1*/	{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_P, WPN_A, WPN_P, WPN_P, WPN_F },
	/*Role_SWAT*/			{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F },
	/*Role_Blaster*/		{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_D, WPN_D, WPN_A, WPN_A, WPN_A, WPN_A, WPN_D, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F },
	/*Role_Sharpshooter*/	{ WPN_F, WPN_A, WPN_A, WPN_D, WPN_D, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_P, WPN_P, WPN_P, WPN_P, WPN_P, WPN_P, WPN_D, WPN_A, WPN_D, WPN_A, WPN_F, WPN_F },
	/*Role_Medic = 5*/		{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_F, WPN_F, WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F },

	/*Role_Godfather = 6*/	{ WPN_F, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_P, WPN_A, WPN_P, WPN_P, WPN_F },
	/*Role_LeadEnforcer*/	{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A, WPN_F },
	/*Role_MadScientist*/	{ WPN_F, WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F },
	/*Role_Assassin*/		{ WPN_F, WPN_A, WPN_D, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_D, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_D, WPN_P, WPN_F, WPN_F, WPN_F, WPN_F },
	/*Role_Arsonist = 10*/	{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_D, WPN_D, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_P, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F },
};

const int g_rgRoleEquipmentsAccessibility[ROLE_COUNT][EQP_COUNT] =
{
	//					      0                                  5                                  10                                 15                                 20                                 25                                 30
	//					      NONE   VEST   V_H    HE     FB     SG     CRYO   INCEN  HEAL_G TOX    NVG
	/*Role_UNASSIGNED*/		{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },

	/*Role_Commander = 1*/	{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },
	/*Role_SWAT*/			{ WPN_F, WPN_A, WPN_A, WPN_F, WPN_D, WPN_D, WPN_F, WPN_F, WPN_F, WPN_F, WPN_D },
	/*Role_Blaster*/		{ WPN_F, WPN_A, WPN_A, WPN_D, WPN_D, WPN_D, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },
	/*Role_Sharpshooter*/	{ WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_A },
	/*Role_Medic = 5*/		{ WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A, WPN_F, WPN_A },

	/*Role_Godfather = 6*/	{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },
	/*Role_LeadEnforcer*/	{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },
	/*Role_MadScientist*/	{ WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A, WPN_A },
	/*Role_Assassin*/		{ WPN_F, WPN_A, WPN_A, WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },
	/*Role_Arsonist = 10*/	{ WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_A, WPN_F, WPN_A, WPN_F, WPN_F, WPN_A },
};

const SkillInfo g_rgSkillInfo[SKILL_COUNT] =
{
	{ SkillIndex_ERROR,	SkillType_UNASSIGNED,	"NULL skill!!",	-1.0f,	-1.0f	},

	// Role_Commander
	{ SkillIndex_RadarScan,			SkillType_Auxiliary,		"Battlefield Analysis",		20.0f,	60.0f	},
	{ SkillIndex_FireRate,			SkillType_WeaponEnhance,	"Haste",					20.0f,	60.0f	},
	{ SkillIndex_ReduceDamage,		SkillType_Defense,			"[N/A] in table.",			20.0f,	60.0f	},

	// Role_SWAT
	{ SkillIndex_Bulletproof,		SkillType_Defense,			"Bulletproof",				15.0f,	40.0f	},
	{ SkillIndex_ArmorRegen,		SkillType_Auxiliary,		"[Passive] Liquid Armor",	-1.0f,	-1.0f	},

	// Role_Breacher
	{ SkillIndex_ExplosiveBullets,	SkillType_WeaponEnhance,	"Explosive Bullets",		10.0f,	30.0f	},
	{ SkillIndex_InfiniteGrenade,	SkillType_Attack,			"Bombard",					10.0f,	30.0f	},

	// Role_Sharpshooter
	{ SkillIndex_EnfoceHeadshot,	SkillType_WeaponEnhance,	"Bullseye",					5.0f,	30.0f	},
	{ SkillIndex_HighlightSight,	SkillType_Attack,			"Glitter Dust",				5.0f,	30.0f	},

	// Role_Medic
	{ SkillIndex_HealingShot,		SkillType_Auxiliary,		"Healing Dart",				5.0f,	20.0f	},

	// Role_Godfather
	{ SkillIndex_Gavelkind,			SkillType_Auxiliary,		"Baptism",					20.0f,	60.0f	},

	// Role_LeadEnforcer
	{ SkillIndex_DmgIncByHP,		SkillType_WeaponEnhance,	"[Passive] Death Wish" ,	-1.0f,	-1.0f	},
	{ SkillIndex_ResistDeath,		SkillType_Attack,			"Swan Song",				6.0f,	40.0f	},

	// Role_MadScientist
	{ SkillIndex_TaserGun,			SkillType_WeaponEnhance,	"Electromagnetic Bullets",	14.5f,	70.0f	},
	{ SkillIndex_Retribution,		SkillType_Defense,			"[Passive] Electromagnetic Armour",	-1.0f,	-1.0f	},

	// Role_Assassin
	{ SkillIndex_Invisible,			SkillType_Defense,			"Chameleon Cloak",			10.0f,	65.0f	},
	{ SkillIndex_CriticalHit,		SkillType_WeaponEnhance,	"[Passive] Backlash",		-1.0f,	-1.0f	},
	{ SkillIndex_RadarScan2,		SkillType_Auxiliary,		"Inside Job",				10.0f,	65.0f	},

	// Role_Arsonist
	{ SkillIndex_IncendiaryAmmo,	SkillType_WeaponEnhance,	"Dragon's Breath",			15.0f,	45.0f	},
};
