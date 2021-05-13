/*

Created Date: Mar 11 2020
Reincarnation Date: Nov 21 2020

*/

#pragma once

#define RADAR_DOT_NORMAL		0
#define RADAR_DOT_BOMB			BIT(0)
#define RADAR_DOT_HOSTAGE		BIT(1)
#define RADAR_DOT_BOMBCARRIER	BIT(2)
#define RADAR_DOT_BOMB_PLANTED	BIT(3)

struct radar_point_s
{
	bool m_bPhase;
	float m_flTimeSwitchPhase;
	PackedColorVec m_color;
	Vector	m_vecCoord;
	float	m_flFlashInterval;// the on-off phases change interval.
	short	m_iFlashCounts;		// how many flashes in total?
	unsigned m_bitsFlags;
	int		m_iDotSize;
};

struct CHudRadar
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
	static void DrawRadarDot(int x, int y, float z_diff, int iBaseDotSize, int flags, int r, int g, int b, int a);
	static inline void DrawRadarDot(const Vector2D& vec, float z_diff, int iBaseDotSize, int flags, int r, int g, int b, int a) { DrawRadarDot(vec.x, vec.y, z_diff, iBaseDotSize, flags, r, g, b, a); }
	static inline void DrawRadarDot(const Vector2D& vec, float z_diff, int iBaseDotSize, int flags, const PackedColorVec& colour) { DrawRadarDot(vec.x, vec.y, z_diff, iBaseDotSize, flags, colour.r, colour.g, colour.b, colour.a); }
	static inline void DrawRadarDot(const Vector& vec, int iBaseDotSize, int flags, int r, int g, int b, int a) { DrawRadarDot(vec.x, vec.y, vec.z, iBaseDotSize, flags, r, g, b, a); }
	static inline void DrawRadarDot(const Vector& vec, int iBaseDotSize, int flags, const PackedColorVec& colour) { DrawRadarDot(vec.x, vec.y, vec.z, iBaseDotSize, flags, colour.r, colour.g, colour.b, colour.a); }
	static void DrawRadar(const Vector2D& vecAnchorLT = ANCHOR, const Vector2D& vecSize = SIZE);
	static void DrawPlayerLocation(void);

	// Game data.
	static constexpr decltype(auto)	DIAMETER = 2048;	// radar viewable range.
	static inline std::unordered_map<short, radar_point_s> m_rgCustomPoints;
	static inline Matrix3x3 m_mxRadarTransform = Matrix3x3::Identity();

	// Drawing data.
	static constexpr decltype(auto)	SIZE = Vector2D(320, 240), MARGIN = Vector2D(18);
	static constexpr decltype(auto)	ICON_SIZE = 16;
	static constexpr decltype(auto) BORDER_THICKNESS = 2;
	static constexpr decltype(auto)	HUD_SIZE = 240;	// deprecated.
	static inline Vector2D	ANCHOR = Vector2D();
	static inline std::array<GLuint, ROLE_COUNT> RADAR_ICONS;

	inline static Vector2D GetBottom(void) { return Vector2D(ANCHOR.x, ANCHOR.y + SIZE.y); }	// deprecated.
};
