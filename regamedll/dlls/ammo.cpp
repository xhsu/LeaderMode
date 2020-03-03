#include "precompiled.h"

const AmmoInfo CBasePlayerItem::m_rgAmmoInfo[MAX_AMMO_SLOTS] =
{
	{AMMO_NONE, "", 0, 0, 0, 0},
	{AMMO_338Magnum,	"338Magnum",	30,		10,		125,	BULLET_PLAYER_338MAG},
	{AMMO_408CheyTac,	"408CheyTac",	28,		7,		95,		BULLET_PLAYER_338MAG},
	{AMMO_762Nato,		"762Nato",		180,	30,		80,		BULLET_PLAYER_762MM},
	{AMMO_556NatoBox,	"556NatoBox",	200,	30,		60,		BULLET_PLAYER_556MM},
	{AMMO_556Nato,		"556Nato",		180,	30,		60,		BULLET_PLAYER_556MM},
	{AMMO_Buckshot,		"buckshot",		70,		14,		115,	BULLET_PLAYER_BUCKSHOT},
	{AMMO_45acp,		"45acp",		200,	12,		25,		BULLET_PLAYER_45ACP},
	{AMMO_57mm,			"57mm",			200,	50,		50,		BULLET_PLAYER_57MM},
	{AMMO_50AE,			"50AE",			70,		7,		40,		BULLET_PLAYER_50AE},
	{AMMO_357SIG,		"357SIG",		60,		6,		25,		BULLET_PLAYER_357SIG},
	{AMMO_9mm,			"9mm",			240,	30,		20,		BULLET_PLAYER_9MM},
	{AMMO_46PDW,		"46pdw",		160,	40,		30,		BULLET_PLAYER_57MM},
	{AMMO_Flashbang,	"Flashbang",	2,		1,		200,	BULLET_NONE},
	{AMMO_HEGrenade,	"HEGrenade",	1,		1,		300,	BULLET_NONE},
	{AMMO_SmokeGrenade,	"SmokeGrenade",	1,		1,		300,	BULLET_NONE},
	{AMMO_C4,			"C4",			1,		1,		0,		BULLET_NONE},
};

const AmmoInfo* GetAmmoInfo(const char* ammoName)
{
	if (ammoName)
	{
		for (int i = 0; i < MAX_WEAPONS; i++)
		{
			if (!Q_stricmp(ammoName, CBasePlayerItem::m_rgAmmoInfo[i].m_pszName))
				return &CBasePlayerItem::m_rgAmmoInfo[i];
		}
	}

	return nullptr;
}

const AmmoInfo* GetAmmoInfo(int iId)
{
	return (iId > 0 && iId < AMMO_MAXTYPE) ? &CBasePlayerItem::m_rgAmmoInfo[iId] : nullptr;
}
