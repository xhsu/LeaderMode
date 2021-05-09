/*

Created Date: May 09 2021

Inherite from
	HUD_Battery	(Mar 11 2020)
	HUD_Health	(Mar 11 2020)

Modern Warfare Dev Team
	Programmer		- Luna the Reborn
	Art & Design	- HL&CL

*/

#include "precompiled.h"

int CHudVitality::Init(void)
{
	m_bitsFlags |= HUD_ACTIVE | HUD_ENFORCE_THINK;

	gHUD::AddHudElem(this);
	return 1;
}

int CHudVitality::Draw(float fTime)
{
	float x = CHudRadar::GetBottom().x;
	float y = CHudRadar::GetBottom().y;
	constexpr float flTotalLength = CHudRadar::SIZE.x;
	const float flVitalitySum = float(gHUD::m_Health.m_iHealth) + gHUD::m_Battery.m_flArmour;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4ub(0, 0, 0, 64);
	DrawUtils::Draw2DQuadNoTex(x, y, x + flTotalLength, y + 10);

	// Draw the backbone anyway.
	glColor4ub(33, 192, 0, 64);
	DrawUtils::Draw2DQuadNoTex(x, y, x + m_flHPMaxPercent * flTotalLength, y + 10);

	if (gHUD::m_Health.m_iHealth > 0)
	{
		glColor4ub(33, 192, 0, 255);
		DrawUtils::Draw2DQuadNoTex(x, y, x + float(gHUD::m_Health.m_iHealth) / gHUD::m_Health.GetMaxHealth() * m_flHPMaxPercent * flTotalLength, y + 10);
	}

	x += m_flHPMaxPercent * flTotalLength;

	glColor4ub(0, 94, 159, 64);
	DrawUtils::Draw2DQuadNoTex(x, y, x + m_flAPMaxPercent * flTotalLength, y + 10);

	if (gHUD::m_Battery.m_flArmour > 0.0f)
	{
		glColor4ub(0, 94, 159, 255);
		DrawUtils::Draw2DQuadNoTex(x, y, x + gHUD::m_Battery.m_flArmour / gHUD::m_Battery.GetMaxArmour() * m_flAPMaxPercent * flTotalLength, y + 10);
	}

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	return 1;
}

void CHudVitality::MsgFunc_Role(RoleTypes iRole)
{
	switch (iRole)
	{
	case Role_Commander:
	case Role_Godfather:
		m_flHPMaxPercent = 0.75f;
		m_flAPMaxPercent = 0.25f;
		break;

	case Role_SWAT:
		m_flHPMaxPercent = 0.3334f;
		m_flAPMaxPercent = 0.6666;
		break;

	default:
		m_flHPMaxPercent = 0.5f;
		m_flAPMaxPercent = 0.5f;
		break;
	}
}
