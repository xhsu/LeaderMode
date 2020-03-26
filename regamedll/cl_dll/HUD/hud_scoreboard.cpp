/*

Created Date: Mar 26 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#include "precompiled.h"

int CHudScoreboard::Init(void)
{
	m_bitsFlags = HUD_ACTIVE|HUD_INTERMISSION;	// this is what intermission for, right?

	gHUD::AddHudElem(this);
	return 1;
}

int CHudScoreboard::VidInit(void)
{
	m_Baseboard.SetAlpha(192);

	m_Baseboard.m_vecCoord.x = (ScreenWidth - 1280) / 2;
	m_Baseboard.m_vecCoord.y = (ScreenHeight - 720) / 2;

	m_Baseboard.m_flWidth = 1280;
	m_Baseboard.m_flHeight = 720;

	return m_Baseboard.VidInit();
}

int CHudScoreboard::Draw(float flTime)
{
	m_Baseboard.Draw(flTime);
	return 1;
}

void CHudScoreboard::Think(void)
{
	/*if (CL_ButtonBits() & IN_SCORE)
		m_bitsFlags |= HUD_ACTIVE;
	else
		m_bitsFlags &= ~HUD_ACTIVE;*/

	m_Baseboard.Think();
}
