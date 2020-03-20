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
	m_iScopes[0] = LoadDDS("texture/Scope/Def_Scope_1440_1080.dds");
	m_iScopes[1] = LoadDDS("texture/Scope/Def_Scope_1620_1080.dds");
	m_iScopes[2] = LoadDDS("texture/Scope/Def_Scope_1728_1080.dds");
	m_iScopes[3] = LoadDDS("texture/Scope/Def_Scope_1920_1080.dds");

	m_iUsingScope = m_iScopes[FindBestRatio()];

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

	glBindTexture(GL_TEXTURE_2D, m_iUsingScope);
	DrawUtils::Draw2DQuad(0, 0, ScreenWidth, ScreenHeight);

	return TRUE;
}

int CHudSniperScope::FindBestRatio(void)
{
	float flRatio = float(ScreenWidth) / float(ScreenHeight);
	float flBestDelta = 9999.0f;
	float flDelta = 9999.0f;
	int iCandidate = 3;	// default: 16:9

	for (int i = 0; i < 4; i++)
	{
		flDelta = Q_abs(flRatio - SCOPE_RATIOS[i]);

		if (flDelta < flBestDelta)
		{
			flBestDelta = flDelta;
			iCandidate = i;
		}
	}

	return iCandidate;
}
