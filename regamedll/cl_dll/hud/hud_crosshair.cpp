/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

void CHudCrosshair::Initialize(void)
{
	gHUD::m_lstElements.push_back({
		Initialize,
		nullptr,
		Reset,
		Draw,
		Think,
		nullptr,
		Reset,
		});
}

void CHudCrosshair::Draw(float flTime, bool bIntermission)
{
	if (!g_pCurWeapon || CL_IsDead() || g_iUser1 || bIntermission)
		return;

	if (gHUD::m_bitsHideHUDDisplay & HIDEHUD_CROSSHAIR)
		return;

	// in steel sight calibrating mode.
	if (cl_gun_ofs[0]->value || cl_gun_ofs[1]->value || cl_gun_ofs[2]->value)
	{
		gEngfuncs.pfnFillRGBA((ScreenWidth - CALIBRATING_CROSSHAIR_SIZE) / 2, (ScreenHeight - CALIBRATING_CROSSHAIR_SIZE) / 2, CALIBRATING_CROSSHAIR_SIZE, CALIBRATING_CROSSHAIR_SIZE, 255, 255, 255, 255);
		return;
	}

	// black part. (Background)
	DrawUtils::glRegularPureColorDrawingInit(0x000000, byte(0.5f * m_flAlpha));
	DrawUtils::Draw2DQuadNoTex(
		float(ScreenWidth) / 2.0f - 3,
		float(ScreenHeight) / 2.0f - m_flCurChDistance / 2.0f - CROSSHAIR_LENGTH - 2,
		float(ScreenWidth) / 2.0f + 3,
		float(ScreenHeight) / 2.0f - m_flCurChDistance / 2.0
	);
	DrawUtils::Draw2DQuadNoTex(
		float(ScreenWidth) / 2.0f - 3,
		float(ScreenHeight) / 2.0f + m_flCurChDistance / 2.0f,
		float(ScreenWidth) / 2.0f + 3,
		float(ScreenHeight) / 2.0f + m_flCurChDistance / 2.0 + CROSSHAIR_LENGTH + 2
	);
	DrawUtils::Draw2DQuadNoTex(
		float(ScreenWidth) / 2.0f - m_flCurChDistance / 2.0f - CROSSHAIR_LENGTH - 2,
		float(ScreenHeight) / 2.0f - 3,
		float(ScreenWidth) / 2.0f - m_flCurChDistance / 2.0f,
		float(ScreenHeight) / 2.0f + 3
	);
	DrawUtils::Draw2DQuadNoTex(
		float(ScreenWidth) / 2.0f + m_flCurChDistance / 2.0f,
		float(ScreenHeight) / 2.0f - 3,
		float(ScreenWidth) / 2.0f + m_flCurChDistance / 2.0f + CROSSHAIR_LENGTH + 2,
		float(ScreenHeight) / 2.0f + 3
	);

	// white part. (Front)
	DrawUtils::glSetColor(0xFFFFFF, byte(m_flAlpha));
	DrawUtils::Draw2DQuadNoTex(
		float(ScreenWidth) / 2.0f - 1.0f,
		float(ScreenHeight) / 2.0f - m_flCurChDistance / 2.0f - CROSSHAIR_LENGTH,
		float(ScreenWidth) / 2.0f + 1,
		float(ScreenHeight) / 2.0f - m_flCurChDistance / 2.0
	);
	DrawUtils::Draw2DQuadNoTex(
		float(ScreenWidth) / 2.0f - 1.0f,
		float(ScreenHeight) / 2.0f + m_flCurChDistance / 2.0f,
		float(ScreenWidth) / 2.0f + 1.0f,
		float(ScreenHeight) / 2.0f + m_flCurChDistance / 2.0 + CROSSHAIR_LENGTH
	);
	DrawUtils::Draw2DQuadNoTex(
		float(ScreenWidth) / 2.0f - m_flCurChDistance / 2.0f - CROSSHAIR_LENGTH,
		float(ScreenHeight) / 2.0f - 1,
		float(ScreenWidth) / 2.0f - m_flCurChDistance / 2.0f,
		float(ScreenHeight) / 2.0f + 1
	);
	DrawUtils::Draw2DQuadNoTex(
		float(ScreenWidth) / 2.0f + m_flCurChDistance / 2.0f,
		float(ScreenHeight) / 2.0f - 1,
		float(ScreenWidth) / 2.0f + m_flCurChDistance / 2.0f + CROSSHAIR_LENGTH,
		float(ScreenHeight) / 2.0f + 1
	);

	DrawUtils::glRegularPureColorDrawingExit();
}

void CHudCrosshair::Think(void)
{
	m_flSpread = g_pCurWeapon ? g_pCurWeapon->GetSpread() : 0.1f;
	m_flCrosshairDistance = round(float(ScreenWidth) * m_flSpread);
	m_flCurChDistance += (m_flCrosshairDistance - m_flCurChDistance) * gHUD::m_flUCDTimeDelta * 5;

	m_flAlpha += ((gHUD::m_iFOV >= DEFAULT_FOV ? 255.0f : 0.0f) - m_flAlpha) * gHUD::m_flUCDTimeDelta * 3;
}

void CHudCrosshair::Reset(void)
{
	m_flSpread = 0.1f;
	m_flCrosshairDistance = 0.1f, m_flCurChDistance = 0.1f;

	m_flAlpha = 255;
}
