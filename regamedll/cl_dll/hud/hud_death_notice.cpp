/*

Created Date: Mar 11 2020

*/

#include "cl_base.h"

DeathNoticeItem rgDeathNoticeList[MAX_DEATHNOTICES + 1];

int CHudDeathNotice::Init(void)
{
	gHUD::AddHudElem(this);

	return 1;
}

void CHudDeathNotice::InitHUDData(void)
{
	Q_memset(rgDeathNoticeList, 0, sizeof(rgDeathNoticeList));
}

void CHudDeathNotice::Reset(void)
{
	m_showIcon = false;
	m_showKill = false;
	m_iconIndex = 0;
	m_killEffectTime = 0;
	m_killIconTime = 0;
	m_lastKillTime = 0;
}

int CHudDeathNotice::VidInit(void)
{
	m_HUD_d_skull = gHUD::GetSpriteIndex("d_skull");
	m_headSprite = gHUD::GetSpriteIndex("d_headshot");
	m_headWidth = gHUD::GetSpriteRect(m_headSprite).right - gHUD::GetSpriteRect(m_headSprite).left;

	return 1;
}

int CHudDeathNotice::Draw(float flTime)
{
	return FALSE;
}
