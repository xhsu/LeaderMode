/*

Created Date: Mar 11 2020

*/

#pragma once

#define DMG_IMAGE_LIFE 2

#define DMG_IMAGE_POISON 0
#define DMG_IMAGE_ACID 1
#define DMG_IMAGE_COLD 2
#define DMG_IMAGE_DROWN 3
#define DMG_IMAGE_BURN 4
#define DMG_IMAGE_NERVE 5
#define DMG_IMAGE_RAD 6
#define DMG_IMAGE_SHOCK 7

#define NUM_DMG_TYPES 8

// damage types definition.
#include "../public/regamedll/regamedll_const.h"

typedef struct
{
	float fExpire;
	float fBaseline;
	int x, y;
}
DAMAGE_IMAGE;

#define MAX_POINTS 64

class CHudHealth : public CBaseHUDElement
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float fTime);
	void Reset(void);

	void MsgFunc_Health(int& iNewHealth);
	void MsgFunc_Damage(int& armor, int& damageTaken, int& bitsDamage, Vector& vecFrom);

	void GetPainColor(int& r, int& g, int& b);
	int DrawPain(float fTime);
	int DrawDamage(float fTime);
	void CalcDamageDirection(Vector vecFrom);
	void UpdateTiles(float fTime, long bits);

public:
	int m_iHealth;
	int m_HUD_dmg_bio;
	int m_HUD_cross;
	float m_fFade;
	float m_fAttackFront, m_fAttackRear, m_fAttackLeft, m_fAttackRight;

private:
	hSprite m_hSprite;
	hSprite m_hDamage;
	DAMAGE_IMAGE m_dmg[NUM_DMG_TYPES];
	int m_bitsDamage;
};
