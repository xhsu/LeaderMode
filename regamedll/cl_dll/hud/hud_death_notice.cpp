/*

Created Date: Mar 11 2020
Remastered Date: May 12 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn

*/

#include "precompiled.h"


void CHudDeathNotice::Initialize(void)
{
	gHUD::m_lstElements.push_back({
		Initialize,
		nullptr,
		ConnectToServer,
		Draw,
		Think,
		Reset,	// Reset on new round.
		Reset,
		});

#ifdef _DEBUG
	gEngfuncs.pfnAddCommand("cl_debug_deathmsg", &CmdFunc_DeathMsg);
	gEngfuncs.pfnAddCommand("cl_debug_ghostkill", &CmdFunc_GhostKill);
#endif // _DEBUG
}

void CHudDeathNotice::ConnectToServer(void)
{
	HUDINDEX_SKULL = gHUD::GetSpriteIndex("d_skull");
	HUDINDEX_HEADSHOT = gHUD::GetSpriteIndex("d_headshot");
	HUDWIDTH_HEADSHOT = gHUD::GetSpriteRect(HUDINDEX_HEADSHOT)->right - gHUD::GetSpriteRect(HUDINDEX_HEADSHOT)->left;

	Reset();
}

void CHudDeathNotice::Draw(float flTime, bool bIntermission)
{
	if (bIntermission || m_lstQueue.empty())
		return;

	if (gHUD::m_bitsHideHUDDisplay & HIDEHUD_ALL)
		return;

	int x, y = MARGIN.height;
	int TextHeight, TextWidth;

	for (auto& item : m_lstQueue)
	{
		// calculate at every starting point.
		x = ScreenWidth - MARGIN.width;

		// victim
		gEngfuncs.pfnDrawConsoleStringLen(item.m_szVictim, &TextWidth, &TextHeight);
		x -= TextWidth;
		gEngfuncs.pfnDrawSetTextColor(item.m_vecVictimColour.r, item.m_vecVictimColour.g, item.m_vecVictimColour.b);
		gEngfuncs.pfnDrawConsoleString(x, y, item.m_szVictim);
		x -= ELEM_MARGIN;	// double space between icon and text.

		// headshot
		if (item.m_bHeadshot)
		{
			x -= HUDWIDTH_HEADSHOT;
			x -= ELEM_MARGIN;

			gEngfuncs.pfnSPR_Set(gHUD::GetSprite(HUDINDEX_HEADSHOT), 255, 255, 255);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, gHUD::GetSpriteRect(HUDINDEX_HEADSHOT));
		}

		// weapon
		x -= (item.m_rcWeaponSpriteInfo.right - item.m_rcWeaponSpriteInfo.left);
		x -= ELEM_MARGIN;
		gEngfuncs.pfnSPR_Set(item.m_hWeaponSprite, 255, 255, 255);	// weapon
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &item.m_rcWeaponSpriteInfo);

		// killer
		if (!item.m_bGhostKill)
		{
			gEngfuncs.pfnDrawConsoleStringLen(item.m_szKiller, &TextWidth, &TextHeight);
			x -= TextWidth;
			x -= ELEM_MARGIN * 2;	// double space between icon and text.

			gEngfuncs.pfnDrawSetTextColor(item.m_vecKillerColour.r, item.m_vecKillerColour.g, item.m_vecKillerColour.b);
			gEngfuncs.pfnDrawConsoleString(x, y, item.m_szKiller);
		}

		// move Y coord after each draw.
		y += (item.m_rcWeaponSpriteInfo.bottom - item.m_rcWeaponSpriteInfo.top) + LINE_MARGIN;
	}
}

void CHudDeathNotice::Reset(void)
{
	m_lstQueue.clear();

	ANCHOR.x = ScreenWidth - MARGIN.width;
	ANCHOR.y = MARGIN.height;
}

void CHudDeathNotice::Think(void)
{
	auto i = m_lstQueue.begin();

	while (i != m_lstQueue.end())
	{
		if (i->m_flTimeToRemove <= gHUD::m_flUCDTime)
		{
			m_lstQueue.erase(i++);  // alternatively, i = m_lstQueue.erase(i);
		}
		else
		{
			++i;
		}
	}
}

void CHudDeathNotice::MsgFunc_DeathMsg(int iKillerIndex, int iVictimIndex, bool bHeadshot, const char* szWeaponName)
{
	// is this a invalid msg?
	if (iVictimIndex < 1 || iVictimIndex > gEngfuncs.GetMaxClients())
		return;

	DeathNoticeItem item;

	// no killer?
	item.m_bGhostKill = (iKillerIndex < 1 || iKillerIndex > gEngfuncs.GetMaxClients() || iKillerIndex == iVictimIndex);	// Suicide is now included in ghost kill.

	// the rest straight-forward data.
	Q_strlcpy(item.m_szKiller, g_PlayerInfoList[iKillerIndex].name ? g_PlayerInfoList[iKillerIndex].name : "");
	Q_strlcpy(item.m_szVictim, g_PlayerInfoList[iVictimIndex].name ? g_PlayerInfoList[iVictimIndex].name : "");
	item.m_bHeadshot = bHeadshot;

	// colour.
	item.m_vecKillerColour = gHUD::GetColor(iKillerIndex);
	item.m_vecVictimColour = gHUD::GetColor(iVictimIndex);

	// weapon.
	char szWeapon[192];
	Q_strlcpy(szWeapon, "d_");
	Q_strlcat(szWeapon, szWeaponName);

	int iIndex = gHUD::GetSpriteIndex(szWeapon);
	if (iIndex == -1)	// no found
		iIndex = HUDINDEX_SKULL;	// default killing icon.

	item.m_rcWeaponSpriteInfo = *gHUD::GetSpriteRect(iIndex);
	item.m_hWeaponSprite = gHUD::GetSprite(iIndex);

	// remove time.
	item.m_flTimeToRemove = gHUD::m_flUCDTime + DISPLAY_TIME;

	// place it into the queue.
	m_lstQueue.emplace_back(item);
}

#ifdef _DEBUG
void CHudDeathNotice::CmdFunc_DeathMsg(void)
{
	DeathNoticeItem item;

	// no killer?
	item.m_bGhostKill = false;

	// the rest straight-forward data.
	Q_strlcpy(item.m_szKiller, gEngfuncs.Cmd_Argv(1));
	Q_strlcpy(item.m_szVictim, gEngfuncs.Cmd_Argv(2));
	item.m_bHeadshot = Q_atoi(gEngfuncs.Cmd_Argv(3));

	// colour.
	item.m_vecKillerColour = gHUD::GetColor(gHUD::m_iPlayerNum);
	item.m_vecVictimColour = gHUD::GetColor(gHUD::m_iPlayerNum);

	// weapon.
	int iIndex = gHUD::GetSpriteIndex(gEngfuncs.Cmd_Argv(4));
	if (iIndex == -1)	// no found
		iIndex = HUDINDEX_SKULL;	// default killing icon.

	item.m_rcWeaponSpriteInfo = *gHUD::GetSpriteRect(iIndex);
	item.m_hWeaponSprite = gHUD::GetSprite(iIndex);

	// remove time.
	item.m_flTimeToRemove = gHUD::m_flUCDTime + DISPLAY_TIME;

	// place it into the queue.
	m_lstQueue.emplace_back(item);
}

void CHudDeathNotice::CmdFunc_GhostKill()
{
	DeathNoticeItem item;

	item.m_bGhostKill = true;

	Q_strlcpy(item.m_szKiller, gEngfuncs.Cmd_Argv(1));
	Q_strlcpy(item.m_szVictim, gEngfuncs.Cmd_Argv(1));
	item.m_bHeadshot = Q_atoi(gEngfuncs.Cmd_Argv(2));

	item.m_vecKillerColour = gHUD::GetColor(gHUD::m_iPlayerNum);
	item.m_vecVictimColour = gHUD::GetColor(gHUD::m_iPlayerNum);

	int iIndex = gHUD::GetSpriteIndex(gEngfuncs.Cmd_Argv(3));
	if (iIndex == -1)	// no found
		iIndex = HUDINDEX_SKULL;	// default killing icon.

	item.m_rcWeaponSpriteInfo = *gHUD::GetSpriteRect(iIndex);
	item.m_hWeaponSprite = gHUD::GetSprite(iIndex);

	// remove time.
	item.m_flTimeToRemove = gHUD::m_flUCDTime + DISPLAY_TIME;

	// place it into the queue.
	m_lstQueue.emplace_back(item);
}
#endif // _DEBUG
