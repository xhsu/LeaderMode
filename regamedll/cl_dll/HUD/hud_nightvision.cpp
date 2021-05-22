/*

Created Date: Mar 11 2020

*/

#include "precompiled.h"

void CHudNightVision::Reset(void)
{
	m_fOn = FALSE;
	m_iAlpha = 110;
}

void NVGAdjustUp(void)
{
	gHUD::m_NightVision.m_iAlpha += 10;

	if (gHUD::m_NightVision.m_iAlpha > 220)
		gHUD::m_NightVision.m_iAlpha = 220;
}

void NVGAdjustDown(void)
{
	gHUD::m_NightVision.m_iAlpha -= 10;

	if (gHUD::m_NightVision.m_iAlpha < 30)
		gHUD::m_NightVision.m_iAlpha = 30;
}

int CHudNightVision::Init(void)
{
	Reset();

	gEngfuncs.pfnAddCommand("+nvgadjust", NVGAdjustUp);
	gEngfuncs.pfnAddCommand("-nvgadjust", NVGAdjustDown);

	m_bitsFlags |= HUD_ACTIVE;

	gHUD::AddHudElem(this);
	return 1;
}

int CHudNightVision::Draw(float flTime)
{
	if (m_fOn && !gEngfuncs.IsSpectateOnly())
	{
		gEngfuncs.pfnFillRGBA(0, 0, ScreenWidth, ScreenHeight, 50, 225, 50, m_iAlpha);

		dlight_t* te = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
		te->origin = gHUD::m_vecOrigin;
		te->radius = gEngfuncs.pfnRandomLong(0, 50) + 750;
		te->color.r = 1;
		te->color.g = 20;
		te->color.b = 1;
		te->die = gHUD::m_flTime + 0.1f;
	}

	return 1;
}

void CHudNightVision::MsgFunc_NVGToggle(bool bOn)
{
	m_fOn = bOn;
}
