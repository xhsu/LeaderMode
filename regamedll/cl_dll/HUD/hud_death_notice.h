/*

Created Date: Mar 11 2020
Remastered Date: May 12 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn

*/

#pragma once

struct DeathNoticeItem
{
	char	m_szVictim[64];
	char	m_szKiller[64];
	bool	m_bHeadshot;
	bool	m_bGhostKill;
	hSprite	m_hWeaponSprite;
	wrect_t m_rcWeaponSpriteInfo;
	float	m_flTimeToRemove;
	Vector	m_vecVictimColour;
	Vector	m_vecKillerColour;
};

struct CHudDeathNotice	// depends on: CHudAccountBalance
{
	// Event functions.
	static void	Initialize(void);
	//static void	Shutdown(void);
	static void	ConnectToServer(void);
	static void	Draw(float flTime, bool bIntermission);
	static void	Think(void);	// Use gHUD::m_flUCDTime
	//static void	OnNewRound(void);
	//static void	ServerAsksReset(void);

	// Message functions.
	static void MsgFunc_DeathMsg(int iKillerIndex, int iVictimIndex, bool bHeadshot, const char* szWeaponName);
#ifdef _DEBUG
	static void CmdFunc_DeathMsg(void);
	static void CmdFunc_GhostKill(void);
#endif // _DEBUG

	// Custom functions.
	static void Reset(void);

	// Game data.
	static inline std::list<DeathNoticeItem> m_lstQueue;	// we should not use std::quene, because we need 4 items per time.

	// Drawing data.
	static constexpr decltype(auto) DISPLAY_TIME = 6;
	static constexpr decltype(auto) MARGIN = Vector2D(24);
	static constexpr decltype(auto) LINE_MARGIN = 12;
	static constexpr decltype(auto) ELEM_MARGIN = 5;
	static inline Vector2D ANCHOR = Vector2D();
	static inline int HUDINDEX_HEADSHOT = -1, HUDINDEX_SKULL = -1;
	static inline int HUDWIDTH_HEADSHOT = 0;
};
