/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

float g_flSpread = 0;
static wrect_t nullrc = { 0, 0, 0, 0 };

int CHudCrosshair::Init(void)
{
	gHUD::AddHudElem(this);

	Reset();

	m_bitsFlags |= HUD_ACTIVE;

	return TRUE;
};

int CHudCrosshair::Draw(float flTime)
{
	if (!g_pCurWeapon || CL_IsDead())
		return TRUE;

	// in steel sight calibrating mode.
	if (cl_gun_ofs[0]->value || cl_gun_ofs[1]->value || cl_gun_ofs[2]->value)
	{
		gEngfuncs.pfnFillRGBA(ScreenWidth / 2 - 1, ScreenHeight / 2 - 1, 2, 2, 255, 255, 255, 255);
		return TRUE;
	}

	g_flSpread = 0.1f;
	if (g_pCurWeapon)
		g_flSpread = g_pCurWeapon->GetSpread();

	m_flCrosshairDistance = round(float(ScreenWidth) * g_flSpread);
	m_flCurChDistance += (m_flCrosshairDistance - m_flCurChDistance) * g_flClientTimeDelta * 5;
	m_flAlphaMul = gHUD::m_iFOV >= DEFAULT_FOV ? 1.0 : 0.0;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// black part. (Background)
	glColor4f(0, 0, 0, 0.5 * m_flAlphaMul);
	DrawUtils::Draw2DQuad(
		float(ScreenWidth) / 2.0f - 3,
		float(ScreenHeight) / 2.0f - m_flCurChDistance / 2.0f - m_flCrosshairBarLength - 2,
		float(ScreenWidth) / 2.0f + 3,
		float(ScreenHeight) / 2.0f - m_flCurChDistance / 2.0);

	DrawUtils::Draw2DQuad(
		float(ScreenWidth) / 2.0f - 3,
		float(ScreenHeight) / 2.0f + m_flCurChDistance / 2.0f,
		float(ScreenWidth) / 2.0f + 3,
		float(ScreenHeight) / 2.0f + m_flCurChDistance / 2.0 + m_flCrosshairBarLength + 2);

	DrawUtils::Draw2DQuad(
		float(ScreenWidth) / 2.0f - m_flCurChDistance / 2.0f - m_flCrosshairBarLength - 2,
		float(ScreenHeight) / 2.0f - 3,
		float(ScreenWidth) / 2.0f - m_flCurChDistance / 2.0f,
		float(ScreenHeight) / 2.0f + 3);

	DrawUtils::Draw2DQuad(
		float(ScreenWidth) / 2.0f + m_flCurChDistance / 2.0f,
		float(ScreenHeight) / 2.0f - 3,
		float(ScreenWidth) / 2.0f + m_flCurChDistance / 2.0f + m_flCrosshairBarLength + 2,
		float(ScreenHeight) / 2.0f + 3);

	// white part. (Front)
	glColor4f(1, 1, 1, 1 * m_flAlphaMul);
	DrawUtils::Draw2DQuad(
		float(ScreenWidth) / 2.0f - 1.0f,
		float(ScreenHeight) / 2.0f - m_flCurChDistance / 2.0f - m_flCrosshairBarLength,
		float(ScreenWidth) / 2.0f + 1,
		float(ScreenHeight) / 2.0f - m_flCurChDistance / 2.0);

	DrawUtils::Draw2DQuad(
		float(ScreenWidth) / 2.0f - 1.0f,
		float(ScreenHeight) / 2.0f + m_flCurChDistance / 2.0f,
		float(ScreenWidth) / 2.0f + 1.0f,
		float(ScreenHeight) / 2.0f + m_flCurChDistance / 2.0 + m_flCrosshairBarLength);

	DrawUtils::Draw2DQuad(
		float(ScreenWidth) / 2.0f - m_flCurChDistance / 2.0f - m_flCrosshairBarLength,
		float(ScreenHeight) / 2.0f - 1,
		float(ScreenWidth) / 2.0f - m_flCurChDistance / 2.0f,
		float(ScreenHeight) / 2.0f + 1);

	DrawUtils::Draw2DQuad(
		float(ScreenWidth) / 2.0f + m_flCurChDistance / 2.0f,
		float(ScreenHeight) / 2.0f - 1,
		float(ScreenWidth) / 2.0f + m_flCurChDistance / 2.0f + m_flCrosshairBarLength,
		float(ScreenHeight) / 2.0f + 1);

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	return TRUE;
}

void CHudCrosshair::Reset(void)
{
	m_bitsFlags |= HUD_ACTIVE;

	gHUD::m_bitsHideHUDDisplay = 0;
}
