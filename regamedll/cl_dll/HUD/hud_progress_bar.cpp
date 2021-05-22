/*

Created Date: Mar 12 2020

*/

#include "precompiled.h"

int CHudProgressBar::Init(void)
{
	gHUD::AddHudElem(this);

	Reset();
	return 1;
}

void CHudProgressBar::Reset(void)
{
	m_bitsFlags |= HUD_ACTIVE;
	m_tmEnd = 0;
	m_tmNewPeriod = 0;

	m_height = 10;
	m_x0 = m_width / 2;
	m_width = (ScreenWidth * 0.5);
	m_x1 = m_width + m_width / 2 - 1;
	m_y0 = (ScreenHeight * 0.5) + 64;
	m_y1 = m_y0 + 9;
}

int CHudProgressBar::Draw(float flTime)
{
	int slot, pos;

	if (m_tmNewPeriod)
	{
		m_tmEnd = m_tmNewPeriod + flTime - m_tmNewElapsed;
		m_tmStart = flTime - m_tmNewElapsed;
		m_tmNewPeriod = 0;
		m_tmNewElapsed = 0;
	}

	if (flTime > m_tmEnd)
		return 1;

	if (m_tmStart > flTime)
		return 1;

	int r, g, b;
	UnpackRGB(r, g, b, RGB_YELLOWISH);

	gEngfuncs.pfnFillRGBA(m_x0, m_y0, 1, m_height, r, g, b, 80);
	gEngfuncs.pfnFillRGBA(m_x1, m_y0, 1, m_height, r, g, b, 80);
	gEngfuncs.pfnFillRGBA(m_x0 + 1, m_y0, m_width - 2, 1, r, g, b, 80);
	gEngfuncs.pfnFillRGBA(m_x0 + 1, m_y1, m_width - 2, 1, r, g, b, 80);

	pos = m_x0 + 2;
	slot = (m_width - 4) * (flTime - m_tmStart) / (m_tmEnd - m_tmStart);

	if (slot >= m_width - 3)
		slot = m_width - 4;

	gEngfuncs.pfnFillRGBA(pos, m_y0 + 2, slot, m_height - 4, r, g, b, 160);
	return 1;
}

void CHudProgressBar::MsgFunc_BarTime(int& val)
{
	if (val > 0)
		m_tmNewPeriod = val;
	else
		m_tmNewPeriod = -1;
}

void CHudProgressBar::MsgFunc_BarTime2(int& total, float& elapsedPercent)
{
	if (total > 0)
	{
		m_tmNewPeriod = total;
		m_tmNewElapsed = total * elapsedPercent;
	}
	else
	{
		m_tmNewPeriod = -1;
		m_tmNewElapsed = 0;
	}
}
