/*

Created Date: Mar 11 2020

*/

#include "precompiled.h"


int CHudDeathNotice::Init(void)
{
	gHUD::AddHudElem(this);

	Reset();
	return 1;
}

void CHudDeathNotice::InitHUDData(void)
{
	m_lstQueue.clear();
}

void CHudDeathNotice::Reset(void)
{
	m_lstQueue.clear();
}

int CHudDeathNotice::VidInit(void)
{
	m_HUD_d_skull = gHUD::GetSpriteIndex("d_skull");
	m_headSprite = gHUD::GetSpriteIndex("d_headshot");
	m_headWidth = gHUD::GetSpriteRect(m_headSprite)->right - gHUD::GetSpriteRect(m_headSprite)->left;

	return 1;
}

int CHudDeathNotice::Draw(float flTime)
{
	int x, y = DEATHNOTICE_Y_BASE_OFS;
	int TextHeight, TextWidth;

	// shift a little bit for flashlight icon.
	if (!(gHUD::m_bitsHideHUDDisplay & HIDEHUD_FLASHLIGHT))
		y += (gHUD::m_Flash.m_prc1.bottom - gHUD::m_Flash.m_prc1.top);

	for (auto& item : m_lstQueue)
	{
		// calculate starting point.
		x = ScreenWidth - DEATHNOTICE_X_BASE_OFS;

		// victim
		gEngfuncs.pfnDrawConsoleStringLen(item.m_szVictim, &TextWidth, &TextHeight);
		x -= TextWidth;
		gEngfuncs.pfnDrawSetTextColor(item.m_rgflVictimColour[0], item.m_rgflVictimColour[1], item.m_rgflVictimColour[2]);
		gEngfuncs.pfnDrawConsoleString(x, y, item.m_szVictim);
		x -= DEATHNOTICE_INTERSPACE;	// double space between icon and text.

		// headshot
		if (item.m_bHeadshot)
		{
			x -= m_headWidth;
			x -= DEATHNOTICE_INTERSPACE;

			gEngfuncs.pfnSPR_Set(gHUD::GetSprite(m_headSprite), 255, 255, 255);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, gHUD::GetSpriteRect(m_headSprite));
		}

		// weapon
		x -= (item.m_rcWeaponSpriteInfo.right - item.m_rcWeaponSpriteInfo.left);
		x -= DEATHNOTICE_INTERSPACE;
		gEngfuncs.pfnSPR_Set(item.m_hWeaponSprite, 255, 255, 255);	// weapon
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &item.m_rcWeaponSpriteInfo);

		// killer
		if (!item.m_bGhostKill)
		{
			gEngfuncs.pfnDrawConsoleStringLen(item.m_szKiller, &TextWidth, &TextHeight);
			x -= TextWidth;
			x -= DEATHNOTICE_INTERSPACE * 2;	// double space between icon and text.

			gEngfuncs.pfnDrawSetTextColor(item.m_rgflKillerColour[0], item.m_rgflKillerColour[1], item.m_rgflKillerColour[2]);
			gEngfuncs.pfnDrawConsoleString(x, y, item.m_szKiller);
		}

		// move Y coord after each draw.
		y += (item.m_rcWeaponSpriteInfo.bottom - item.m_rcWeaponSpriteInfo.top) + DEATHNOTICE_INTERSPACE;
	}

	return TRUE;
}

void CHudDeathNotice::Think(void)
{
	std::list<DeathNoticeItem>::iterator i = m_lstQueue.begin();

	while (i != m_lstQueue.end())
	{
		if (i->m_flTimeToRemove <= gEngfuncs.GetClientTime())
		{
			m_lstQueue.erase(i++);  // alternatively, i = m_lstQueue.erase(i);
		}
		else
		{
			++i;
		}
	}

	if (m_lstQueue.empty())
		m_bitsFlags &= ~HUD_ACTIVE;
}

void CHudDeathNotice::MsgFunc_DeathMsg(int iKillerIndex, int iVictimIndex, bool bHeadshot, const char* szWeaponName)
{
	// is this a invalid msg?
	if (iVictimIndex < 1 || iVictimIndex > gEngfuncs.GetMaxClients())
		return;

	DeathNoticeItem item;

	// no killer?
	item.m_bGhostKill = (iKillerIndex < 1 || iKillerIndex > gEngfuncs.GetMaxClients());

	// the rest straight-forward data.
	Q_strlcpy(item.m_szKiller, g_PlayerInfoList[iKillerIndex].name ? g_PlayerInfoList[iKillerIndex].name : "");
	Q_strlcpy(item.m_szVictim, g_PlayerInfoList[iVictimIndex].name ? g_PlayerInfoList[iVictimIndex].name : "");
	item.m_bHeadshot = bHeadshot;

	// colour.
	item.m_rgflKillerColour = GetClientColor(iKillerIndex);
	item.m_rgflVictimColour = GetClientColor(iVictimIndex);

	// weapon.
	char szWeapon[192];
	Q_strlcpy(szWeapon, "d_");
	Q_strlcat(szWeapon, szWeaponName);

	int iIndex = gHUD::GetSpriteIndex(szWeapon);
	if (iIndex == -1)	// no found
		iIndex = m_HUD_d_skull;

	item.m_rcWeaponSpriteInfo = *gHUD::GetSpriteRect(iIndex);
	item.m_hWeaponSprite = gHUD::GetSprite(iIndex);

	// remove time.
	item.m_flTimeToRemove = gEngfuncs.GetClientTime() + DEATHNOTICE_DISPLAY_TIME;

	// place it into the queue.
	m_lstQueue.emplace_back(item);
	m_bitsFlags |= HUD_ACTIVE;
}
