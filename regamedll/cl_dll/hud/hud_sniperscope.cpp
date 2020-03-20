/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

int CHudSniperScope::Init(void)
{
	Reset();
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
	switch (m_iPhase)
	{
	case FADE_IN:
		m_flAlpha += (255.0f - m_flAlpha) * gHUD::m_flTimeDelta * m_flFadeSpeed;
		break;

	case FADE_STAY:
		m_flAlpha = 255.0f;
		break;

	default:
		m_flAlpha += (0.0f - m_flAlpha) * gHUD::m_flTimeDelta * m_flFadeSpeed;
		break;
	}

	m_flAlpha = Q_clamp(m_flAlpha, 0.0f, 255.0f);

	if (m_flAlpha > 254.0f)
	{
		if (m_iPhase == FADE_IN)
		{
			m_iPhase = FADE_STAY;
			m_flTimeToFadeOut = gHUD::m_flTime + 0.3f;
		}
		else if (m_iPhase == FADE_STAY && m_flTimeToFadeOut <= gHUD::m_flTime)
			m_iPhase = FADE_OUT;
	}

	// the scope fading efx is active nomatter what FOV you have.
	if (m_flAlpha > 1)
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0, 0, 0, m_flAlpha / 255.0);

		glBegin(GL_QUADS);
		glVertex2f(0, 0);
		glVertex2f(0, ScreenHeight);
		glVertex2f(ScreenWidth, ScreenHeight);
		glVertex2f(ScreenWidth, 0);
		glEnd();

		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
	}

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

void CHudSniperScope::Reset(void)
{
	m_flAlpha = 0;
	m_flFadeSpeed = 0;
	m_iPhase = FADE_OUT;
	m_flTimeToFadeOut = 0;
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

void CHudSniperScope::SetFade(float flRate)
{
	m_flFadeSpeed = flRate;
	m_iPhase = FADE_IN;
}

void CHudSniperScope::SetFadeFromBlack(float flRate, float flStayTime)
{
	m_flAlpha = 255.0f;
	m_flFadeSpeed = flRate;
	m_iPhase = FADE_STAY;
	m_flTimeToFadeOut = gHUD::m_flTime + flStayTime;
}
