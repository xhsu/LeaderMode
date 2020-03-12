/*

Created Date: Mar 11 2020

*/

#include "cl_base.h"

int CHudTrain::Init(void)
{
	m_iPos = 0;
	m_bitsFlags = 0;

	gHUD::AddHudElem(this);
	return 1;
}

int CHudTrain::VidInit(void)
{
	m_hSprite = 0;

	return 1;
}

int CHudTrain::Draw(float fTime)
{
	if (!m_hSprite)
		m_hSprite = LoadSprite("sprites/%d_train.spr");

	if (m_iPos)
	{
		int r, g, b, x, y;

		UnpackRGB(r, g, b, RGB_YELLOWISH);
		gEngfuncs.pfnSPR_Set(m_hSprite, r, g, b);

		y = ScreenHeight - gEngfuncs.pfnSPR_Height(m_hSprite, 0) - gHUD::m_iFontHeight;
		x = ScreenWidth / 3 + gEngfuncs.pfnSPR_Width(m_hSprite, 0) / 4;

		gEngfuncs.pfnSPR_DrawAdditive(m_iPos - 1, x, y, NULL);
	}

	return 1;
}

void CHudTrain::MsgFunc_Train(int& iStatus)
{
	m_iPos = iStatus;

	if (m_iPos)
		m_bitsFlags |= HUD_ACTIVE;
	else
		m_bitsFlags &= ~HUD_ACTIVE;
}
