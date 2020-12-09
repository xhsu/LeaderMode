/*

Created date: 02/26/2020
Segmentation Date: Sep 20 2020

*/

#include "precompiled.h"

const char* g_rgszRoleNames[ROLE_COUNT] =
{
	"#LeaderMod_Role_UNASSIGNED",

	"#LeaderMod_Role_Commander",
	"#LeaderMod_Role_SWAT",
	"#LeaderMod_Role_Breacher",
	"#LeaderMod_Role_Sharpshooter",
	"#LeaderMod_Role_Medic",

	"#LeaderMod_Role_Godfather",
	"#LeaderMod_Role_LeadEnforcer",
	"#LeaderMod_Role_MadScientist",
	"#LeaderMod_Role_Assassin",
	"#LeaderMod_Role_Arsonist"
};

const int g_rgRoleWeaponsAccessibility[ROLE_COUNT][LAST_WEAPON] =
{
	//					      0                                  5                                  10                                 15                                 20                                 25                                 30
	//					      NONE   G18C   USP    ANACO  DEAGLE FN57   M45A1  KSG12  M1014  AA12   MP7A1  MAC10  MP5N   UMP45  P90    K.VEC  AK47   M4A1   SCARH  XM8    SRS    SVD    AWP    PSG1   MK46   RPD
	/*Role_UNASSIGNED*/		{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A },

	/*Role_Commander = 1*/	{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_P, WPN_A, WPN_P, WPN_P, WPN_P },
	/*Role_SWAT*/			{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_F, WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F },
	/*Role_Breacher*/		{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_D, WPN_D, WPN_D, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F },
	/*Role_Sharpshooter*/	{ WPN_F, WPN_A, WPN_A, WPN_D, WPN_D, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_P, WPN_P, WPN_P, WPN_P, WPN_D, WPN_A, WPN_D, WPN_A, WPN_F, WPN_F },
	/*Role_Medic = 5*/		{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F },

	/*Role_Godfather = 6*/	{ WPN_F, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_P, WPN_A, WPN_P, WPN_P, WPN_P },
	/*Role_LeadEnforcer*/	{ WPN_F, WPN_D, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_D, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A, WPN_A },
	/*Role_MadScientist*/	{ WPN_F, WPN_F, WPN_A, WPN_A, WPN_F, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F },
	/*Role_Assassin*/		{ WPN_F, WPN_A, WPN_D, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_D, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_D, WPN_F, WPN_F, WPN_P, WPN_F, WPN_F },
	/*Role_Arsonist = 10*/	{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_D, WPN_D, WPN_D, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_P, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F },
};

const int g_rgRoleEquipmentsAccessibility[ROLE_COUNT][EQP_COUNT] =
{
	//					      0                                  5                                  10                                 15                                 20                                 25                                 30
	//					      NONE   VEST   V_H    HE     FB     SG     CRYO   INCEN  HSG    TOX    C4     C4DTR  NVG
	/*Role_UNASSIGNED*/		{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },

	/*Role_Commander = 1*/	{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },
	/*Role_SWAT*/			{ WPN_F, WPN_A, WPN_A, WPN_F, WPN_D, WPN_D, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_D },
	/*Role_Breacher*/		{ WPN_F, WPN_A, WPN_A, WPN_D, WPN_D, WPN_D, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A, WPN_F, WPN_A },
	/*Role_Sharpshooter*/	{ WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },
	/*Role_Medic = 5*/		{ WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A, WPN_F, WPN_F, WPN_F, WPN_A },

	/*Role_Godfather = 6*/	{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },
	/*Role_LeadEnforcer*/	{ WPN_F, WPN_A, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },
	/*Role_MadScientist*/	{ WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A, WPN_F, WPN_F, WPN_A },
	/*Role_Assassin*/		{ WPN_F, WPN_A, WPN_A, WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },
	/*Role_Arsonist = 10*/	{ WPN_F, WPN_A, WPN_A, WPN_F, WPN_F, WPN_A, WPN_F, WPN_A, WPN_F, WPN_F, WPN_F, WPN_F, WPN_A },
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
