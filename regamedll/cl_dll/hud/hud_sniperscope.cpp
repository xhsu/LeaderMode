/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

int CHudSniperScope::Init(void)
{
	m_bitsFlags = HUD_ACTIVE;

	gHUD::AddHudElem(this);
	return TRUE;
}

int CHudSniperScope::VidInit(void)
{
	m_iScopeArc[0] = LoadDDS("sprites/scope_arc_nw.dds");
	m_iScopeArc[1] = LoadDDS("sprites/scope_arc_ne.dds");
	m_iScopeArc[2] = LoadDDS("sprites/scope_arc.dds");
	m_iScopeArc[3] = LoadDDS("sprites/scope_arc_sw.dds");

	m_left = (ScreenWidth - ScreenHeight) / 2;
	m_right = m_left + ScreenHeight;
	m_vecCentre.x = ScreenWidth / 2;
	m_vecCentre.y = ScreenHeight / 2;

	return TRUE;
}

int CHudSniperScope::Draw(float flTime)
{
	if (gHUD::m_iFOV > 40)
		return FALSE;

	gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
	gEngfuncs.pTriAPI->Brightness(1.0);
	gEngfuncs.pTriAPI->Color4ub(0, 0, 0, 255);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);

	glBindTexture(GL_TEXTURE_2D, m_iScopeArc[0]);
	DrawUtils::Draw2DQuad(m_left, 0, m_vecCentre.x, m_vecCentre.y);

	glBindTexture(GL_TEXTURE_2D, m_iScopeArc[1]);
	DrawUtils::Draw2DQuad(m_vecCentre.x, 0, m_right, m_vecCentre.y);

	glBindTexture(GL_TEXTURE_2D, m_iScopeArc[2]);
	DrawUtils::Draw2DQuad(m_vecCentre.x, m_vecCentre.y, m_right, ScreenHeight);

	glBindTexture(GL_TEXTURE_2D, m_iScopeArc[3]);
	DrawUtils::Draw2DQuad(m_left, m_vecCentre.y, m_vecCentre.x, ScreenHeight);

	gEngfuncs.pfnFillRGBABlend(0, 0, m_left, ScreenHeight, 0, 0, 0, 255);
	gEngfuncs.pfnFillRGBABlend(m_right, 0, m_left, ScreenHeight, 0, 0, 0, 255);

	return TRUE;
}
