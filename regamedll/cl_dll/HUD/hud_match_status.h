/*

Created Date: Mar 12 2020
Reincarnation Date: Nov 24 2020
Remastered Date: May 14 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#pragma once

enum TacticalSchemes : byte
{
	Scheme_UNASSIGNED = 0U,	// disputation
	Doctrine_SuperiorFirepower,
	Doctrine_MassAssault,
	Doctrine_GrandBattleplan,
	Doctrine_MobileWarfare,

	SCHEMES_COUNT
};

extern std::array<unsigned, 4U> g_rgiManpower;
extern std::array<TacticalSchemes, SCHEMES_COUNT> g_rgiTeamSchemes;
extern std::array<std::wstring, ROLE_COUNT> g_rgwcsRoleNames;
extern std::array<std::wstring, SCHEMES_COUNT> g_rgwcsSchemeNames;

struct CHudMatchStatus
{
	// Event functions.
	static void	Initialize(void);
	//static void	Shutdown(void);
	//static void	ConnectToServer(void);
	static void	Draw(float flTime, bool bIntermission);
	static void	Think(void);	// Use gHUD::m_flUCDTime
	//static void	OnNewRound(void);
	//static void	ServerAsksReset(void);

	// Message functions.

	// Custom functions.
	static void Reset(void);

	// Game data.
	static constexpr auto FACTOR_HP = 1.0;
	static constexpr auto FACTOR_MANPOWER = 120.0;
	static constexpr auto FACTOR_FUND = 0.075;
	static inline std::array<double, 4> m_rgflTeamPower;
	static inline double m_flTerroristPercentage = 0;
	static inline double m_flCTPercentage = 0;

	// Drawing data.
	static constexpr auto BOP_DEFAULT_ALPHA = 192;
	static constexpr auto BOP_HIGHLIGHT_PERIOD = 5.0;
	static constexpr auto BOP_SIZE = Vector2D(256, 24);	// Balance of Power
	static constexpr auto BOP_INNERBLOCK_MARGIN = 2;
	static constexpr auto BOP_BORDER_THICKNESS = 2;
	static inline Vector2D BOP_ANCHOR = Vector2D();
	static inline Vector2D BOP_INNERBLOCK_ANCHOR = BOP_ANCHOR + Vector2D(BOP_INNERBLOCK_MARGIN) + Vector2D(BOP_BORDER_THICKNESS);
	static inline Vector2D BOP_INNERBLOCK_SIZE = BOP_SIZE - Vector2D(BOP_INNERBLOCK_MARGIN) * 2 - Vector2D(BOP_BORDER_THICKNESS) * 2;
};
