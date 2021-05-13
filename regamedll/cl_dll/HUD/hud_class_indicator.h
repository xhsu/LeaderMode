/*

Created Date: Sep 20 2020
Remastered Date: May 12 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#pragma once

#ifdef COLOR_HIGHLIGHT
#undef COLOR_HIGHLIGHT
#endif

struct CHudClassIndicator
{
	typedef enum : char { DECREASE = -1, FREEZED = 0, INCREASE = 1} MODE;

	// Event functions.
	static void	Initialize(void);
	//static void	Shutdown(void);
	//static void	ConnectToServer(void);
	static void	Draw(float flTime, bool bIntermission);
	static void	Think(void);	// Use gHUD::m_flUCDTime
	//static void	OnNewRound(void);
	//static void	ServerAsksReset(void);

	// Message functions.
	static void MsgFunc_Role(const RoleTypes& iRole);
	static void MsgFunc_SkillTimer(const float& flTotalTime, const MODE& iMode, const float& flCurrentTime);

	// Custom functions.
	static SkillIndex	GetPrimarySkill(RoleTypes iRole = g_iRoleType);
	static void			Reset(void);

	// Game data.
	static inline float m_flTotalTime = 10.0, m_flCurrentTime = 10.0, m_flPercentage = 1;
	static inline MODE m_iMode = FREEZED;

	// Drawing data.
	static constexpr decltype(auto) COLOR_REGULAR = Vector(1, 1, 1), COLOR_READY = VEC_SPRINGGREENISH, COLOR_WARNING = VEC_REDISH;
	static constexpr decltype(auto) DEPLETING_COLOR_OCSILLATING_PERIOD = 0.5, READY_COLOR_OCSILLATING_PERIOD = 3.5;
	static constexpr decltype(auto) BORDER_THICKNESS = 3;
	static constexpr decltype(auto) PORTRAIT_SIZE = Vector2D(96);
	static constexpr decltype(auto) MARGIN = 8;	// Between this and radar above.
	static inline Vector2D PORTRAIT_ANCHOR = Vector2D();
	static inline std::array<GLuint, ROLE_COUNT> CLASS_PORTRAIT;
	static inline float m_flAlpha = 255;
	static inline Vector m_vecCurColor = COLOR_REGULAR;
};
