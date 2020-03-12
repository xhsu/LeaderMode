/*

Created Date: Mar 11 2020

*/

#include "cl_base.h"

#define PAIN_NAME "sprites/%d_pain.spr"
#define DAMAGE_NAME "sprites/%d_dmg.spr"

int giDmgHeight, giDmgWidth;

int giDmgFlags[NUM_DMG_TYPES] =
{
	DMG_POISON,
	DMG_ACID,
	DMG_FREEZE | DMG_SLOWFREEZE,
	DMG_DROWN,
	DMG_BURN | DMG_SLOWBURN,
	DMG_NERVEGAS,
	DMG_RADIATION,
	DMG_SHOCK,
};

int CHudHealth::Init(void)
{
	m_iHealth = 100;
	m_fFade = 0;
	m_bitsFlags = 0;
	m_bitsDamage = 0;
	m_fAttackFront = m_fAttackRear = m_fAttackRight = m_fAttackLeft = 0;
	giDmgHeight = 0;
	giDmgWidth = 0;

	Q_memset(m_dmg, 0, sizeof(DAMAGE_IMAGE) * NUM_DMG_TYPES);

	gHUD::AddHudElem(this);
	return 1;
}

int CHudHealth::VidInit(void)
{
	m_hSprite = 0;

	m_HUD_dmg_bio = gHUD::GetSpriteIndex("dmg_bio") + 1;
	m_HUD_cross = gHUD::GetSpriteIndex("cross");

	giDmgHeight = gHUD::GetSpriteRect(m_HUD_dmg_bio).right - gHUD::GetSpriteRect(m_HUD_dmg_bio).left;
	giDmgWidth = gHUD::GetSpriteRect(m_HUD_dmg_bio).bottom - gHUD::GetSpriteRect(m_HUD_dmg_bio).top;

	giDmgHeight = gHUD::GetSpriteRect(m_HUD_dmg_bio).right - gHUD::GetSpriteRect(m_HUD_dmg_bio).left;
	giDmgWidth = gHUD::GetSpriteRect(m_HUD_dmg_bio).bottom - gHUD::GetSpriteRect(m_HUD_dmg_bio).top;

	return 1;
}

int CHudHealth::Draw(float flTime)
{
	int r, g, b;
	int a = 0, x, y;
	int HealthWidth;

	if ((gHUD::m_bitsHideHUDDisplay & HIDEHUD_HEALTH) || g_iUser1)
		return 1;

	if (!m_hSprite)
		m_hSprite = LoadSprite(PAIN_NAME);

	if (m_fFade)
	{
		m_fFade -= (gHUD::m_flTimeDelta * 20);

		if (m_fFade <= 0)
		{
			a = MIN_ALPHA;
			m_fFade = 0;
		}

		a = MIN_ALPHA + (m_fFade / FADE_TIME) * 128;
	}
	else
		a = MIN_ALPHA;

	if (m_iHealth <= 15)
		a = 255;

	GetPainColor(r, g, b);
	ScaleColors(r, g, b, a);

	if (gHUD::m_iWeaponBits & (1 << (WEAPON_SUIT)))
	{
		HealthWidth = gHUD::GetSpriteRect(gHUD::m_HUD_number_0).right - gHUD::GetSpriteRect(gHUD::m_HUD_number_0).left;

		int CrossWidth = gHUD::GetSpriteRect(m_HUD_cross).right - gHUD::GetSpriteRect(m_HUD_cross).left;

		y = ScreenHeight - gHUD::m_iFontHeight - gHUD::m_iFontHeight / 2;
		x = CrossWidth / 2;

		gEngfuncs.pfnSPR_Set(gHUD::GetSprite(m_HUD_cross), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &gHUD::GetSpriteRect(m_HUD_cross));

		x = CrossWidth + HealthWidth / 2;
		x = gHUD::DrawHudNumber(x, y, DHN_3DIGITS | DHN_DRAWZERO, m_iHealth, r, g, b);
		x += HealthWidth / 2;
	}

	DrawDamage(flTime);
	return DrawPain(flTime);
}

void CHudHealth::Reset(void)
{
	m_fAttackFront = m_fAttackRear = m_fAttackRight = m_fAttackLeft = 0;
	m_bitsDamage = 0;

	for (int i = 0; i < NUM_DMG_TYPES; i++)
		m_dmg[i].fExpire = 0;
}

void CHudHealth::MsgFunc_Health(int& iNewHealth)
{
	m_bitsFlags |= HUD_ACTIVE;

	if (iNewHealth != m_iHealth)
	{
		m_fFade = FADE_TIME;
		m_iHealth = iNewHealth;
	}
}

void CHudHealth::MsgFunc_Damage(int& armor, int& damageTaken, int& bitsDamage, Vector& vecFrom)
{
	UpdateTiles(gHUD::m_flTime, bitsDamage);

	if (damageTaken > 0 || armor > 0)
		CalcDamageDirection(vecFrom);
}

void CHudHealth::GetPainColor(int& r, int& g, int& b)
{
	int iHealth = m_iHealth;

	if (iHealth > 25)
		iHealth -= 25;
	else if (iHealth < 0)
		iHealth = 0;

	if (m_iHealth > 25)
	{
		UnpackRGB(r, g, b, RGB_YELLOWISH);
	}
	else
	{
		r = 250;
		g = 0;
		b = 0;
	}
}

int CHudHealth::DrawPain(float flTime)
{
	if (!(m_fAttackFront || m_fAttackRear || m_fAttackLeft || m_fAttackRight))
		return 1;

	int r, g, b;
	int x, y, a, shade;

	a = 255;

	float fFade = gHUD::m_flTimeDelta * 2;

	if (m_fAttackFront > 0.4)
	{
		GetPainColor(r, g, b);
		shade = a * Q_max(m_fAttackFront, 0.5f);
		ScaleColors(r, g, b, shade);
		gEngfuncs.pfnSPR_Set(m_hSprite, r, g, b);

		x = ScreenWidth / 2 - gEngfuncs.pfnSPR_Width(m_hSprite, 0) / 2;
		y = ScreenHeight / 2 - gEngfuncs.pfnSPR_Height(m_hSprite, 0) * 3;
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, NULL);
		m_fAttackFront = Q_max(0.0f, m_fAttackFront - fFade);
	}
	else
		m_fAttackFront = 0;

	if (m_fAttackRight > 0.4)
	{
		GetPainColor(r, g, b);
		shade = a * Q_max(m_fAttackRight, 0.5f);
		ScaleColors(r, g, b, shade);
		gEngfuncs.pfnSPR_Set(m_hSprite, r, g, b);

		x = ScreenWidth / 2 + gEngfuncs.pfnSPR_Width(m_hSprite, 1) * 2;
		y = ScreenHeight / 2 - gEngfuncs.pfnSPR_Height(m_hSprite, 1) / 2;
		gEngfuncs.pfnSPR_DrawAdditive(1, x, y, NULL);
		m_fAttackRight = Q_max(0.0f, m_fAttackRight - fFade);
	}
	else
		m_fAttackRight = 0;

	if (m_fAttackRear > 0.4)
	{
		GetPainColor(r, g, b);
		shade = a * Q_max(m_fAttackRear, 0.5f);
		ScaleColors(r, g, b, shade);
		gEngfuncs.pfnSPR_Set(m_hSprite, r, g, b);

		x = ScreenWidth / 2 - gEngfuncs.pfnSPR_Width(m_hSprite, 2) / 2;
		y = ScreenHeight / 2 + gEngfuncs.pfnSPR_Height(m_hSprite, 2) * 2;
		gEngfuncs.pfnSPR_DrawAdditive(2, x, y, NULL);
		m_fAttackRear = Q_max(0.0f, m_fAttackRear - fFade);
	}
	else
		m_fAttackRear = 0;

	if (m_fAttackLeft > 0.4)
	{
		GetPainColor(r, g, b);
		shade = a * Q_max(m_fAttackLeft, 0.5f);
		ScaleColors(r, g, b, shade);
		gEngfuncs.pfnSPR_Set(m_hSprite, r, g, b);

		x = ScreenWidth / 2 - gEngfuncs.pfnSPR_Width(m_hSprite, 3) * 3;
		y = ScreenHeight / 2 - gEngfuncs.pfnSPR_Height(m_hSprite, 3) / 2;
		gEngfuncs.pfnSPR_DrawAdditive(3, x, y, NULL);

		m_fAttackLeft = Q_max(0.0f, m_fAttackLeft - fFade);
	}
	else
		m_fAttackLeft = 0;

	return 1;
}

int CHudHealth::DrawDamage(float flTime)
{
	int r, g, b, a;
	DAMAGE_IMAGE* pdmg;

	if (!m_bitsDamage)
		return 1;

	UnpackRGB(r, g, b, RGB_YELLOWISH);

	a = (int)(Q_fabs(Q_sin(flTime * 2)) * 256.0);

	ScaleColors(r, g, b, a);

	for (int i = 0; i < NUM_DMG_TYPES; i++)
	{
		if (m_bitsDamage & giDmgFlags[i])
		{
			pdmg = &m_dmg[i];
			gEngfuncs.pfnSPR_Set(gHUD::GetSprite(m_HUD_dmg_bio + i), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, pdmg->x, pdmg->y, &gHUD::GetSpriteRect(m_HUD_dmg_bio + i));
		}
	}

	for (int i = 0; i < NUM_DMG_TYPES; i++)
	{
		DAMAGE_IMAGE* pdmg = &m_dmg[i];

		if (m_bitsDamage & giDmgFlags[i])
		{
			pdmg->fExpire = Q_min(flTime + DMG_IMAGE_LIFE, pdmg->fExpire);

			if (pdmg->fExpire <= flTime && a < 40)
			{
				pdmg->fExpire = 0;
				int y = pdmg->y;
				pdmg->x = pdmg->y = 0;

				for (int j = 0; j < NUM_DMG_TYPES; j++)
				{
					pdmg = &m_dmg[j];

					if ((pdmg->y) && (pdmg->y < y))
						pdmg->y += giDmgHeight;
				}

				m_bitsDamage &= ~giDmgFlags[i];
			}
		}
	}

	return 1;
}

void CHudHealth::CalcDamageDirection(Vector vecFrom)
{
	Vector forward, right, up;
	float side, front;

	if (!vecFrom[0] && !vecFrom[1] && !vecFrom[2])
	{
		m_fAttackFront = m_fAttackRear = m_fAttackRight = m_fAttackLeft = 0;
		return;
	}

	VectorSubtract(vecFrom, gHUD::m_vecOrigin, vecFrom);

	float flDistToTarget = vecFrom.Length();

	vecFrom = vecFrom.Normalize();
	AngleVectors(gHUD::m_vecAngles, forward, right, up);

	front = DotProduct(vecFrom, right);
	side = DotProduct(vecFrom, forward);

	if (flDistToTarget <= 50)
	{
		m_fAttackFront = m_fAttackRear = m_fAttackRight = m_fAttackLeft = 1;
	}
	else
	{
		if (side > 0)
		{
			if (side > 0.3)
				m_fAttackFront = Q_max(m_fAttackFront, side);
		}
		else
		{
			float f = Q_fabs(side);

			if (f > 0.3)
				m_fAttackRear = Q_max(m_fAttackRear, f);
		}

		if (front > 0)
		{
			if (front > 0.3)
				m_fAttackRight = Q_max(m_fAttackRight, front);
		}
		else
		{
			float f = Q_fabs(front);

			if (f > 0.3)
				m_fAttackLeft = Q_max(m_fAttackLeft, f);
		}
	}
}

void CHudHealth::UpdateTiles(float flTime, long bitsDamage)
{
	DAMAGE_IMAGE* pdmg;
	long bitsOn = ~m_bitsDamage & bitsDamage;

	for (int i = 0; i < NUM_DMG_TYPES; i++)
	{
		pdmg = &m_dmg[i];

		if (m_bitsDamage & giDmgFlags[i])
		{
			pdmg->fExpire = flTime + DMG_IMAGE_LIFE;

			if (!pdmg->fBaseline)
				pdmg->fBaseline = flTime;
		}

		if (bitsOn & giDmgFlags[i])
		{
			pdmg->x = giDmgWidth / 8;
			pdmg->y = ScreenHeight - giDmgHeight * 2;
			pdmg->fExpire = flTime + DMG_IMAGE_LIFE;

			for (int j = 0; j < NUM_DMG_TYPES; j++)
			{
				if (j == i)
					continue;

				pdmg = &m_dmg[j];

				if (pdmg->y)
					pdmg->y -= giDmgHeight;
			}

			pdmg = &m_dmg[i];
		}
	}

	m_bitsDamage |= bitsDamage;
}
