/*

Created Date: Mar 11 2020
Reincarnation Date: Nov 21 2020

*/

#pragma once

#define MAX_POINTS 64

#define RADAR_DOT_NORMAL		0
#define RADAR_DOT_BOMB			BIT(0)
#define RADAR_DOT_HOSTAGE		BIT(1)
#define RADAR_DOT_BOMBCARRIER	BIT(2)
#define RADAR_DOT_BOMB_PLANTED	BIT(3)

struct radar_point_s
{
	bool m_bGlobalOn;
	bool m_bPhase;
	float m_flTimeSwitchPhase;
	PackedColorVec m_color;
	Vector m_vecCoord;
	float m_flFlashInterval;// the on-off phases change interval.
	int	m_iFlashCounts;		// how many flashes in total?
	int m_bitsFlags;
	int m_iDotSize;
};

class CHudRadar : public CBaseHudElement
{
public:
	static Vector2D	MARGIN;	// This is variable now.

	static constexpr Vector2D	SIZE		= Vector2D(280, 200);
	static constexpr float		DIAMETER	= 2048;
	static constexpr int		ICON_SIZE	= 16;

	static constexpr float	BORDER_GAP = 12;
	static constexpr float	HUD_SIZE = 240;

public:
	int Init(void);
	int VidInit(void);
	int Draw(float fTime);

	void DrawRadarDot(int x, int y, float z_diff, int iBaseDotSize, int flags, int r, int g, int b, int a);
	void DrawRadar(float flTime, const Vector2D& vecMargin = MARGIN, const Vector2D& vecSize = SIZE);
	void DrawPlayerLocation(void);

public:
	inline void DrawRadarDot(const Vector2D& vec, float z_diff, int iBaseDotSize, int flags, int r, int g, int b, int a) { DrawRadarDot(vec.x, vec.y, z_diff, iBaseDotSize, flags, r, g, b, a); }
	inline void DrawRadarDot(const Vector2D& vec, float z_diff, int iBaseDotSize, int flags, const PackedColorVec& colour) { DrawRadarDot(vec.x, vec.y, z_diff, iBaseDotSize, flags, colour.r, colour.g, colour.b, colour.a); }
	inline void DrawRadarDot(const Vector& vec, int iBaseDotSize, int flags, int r, int g, int b, int a) { DrawRadarDot(vec.x, vec.y, vec.z, iBaseDotSize, flags, r, g, b, a); }
	inline void DrawRadarDot(const Vector& vec, int iBaseDotSize, int flags, const PackedColorVec& colour) { DrawRadarDot(vec.x, vec.y, vec.z, iBaseDotSize, flags, colour.r, colour.g, colour.b, colour.a); }
	inline static Vector2D GetBottom(void) { return Vector2D(MARGIN.x, MARGIN.y + SIZE.y); }

public:
	unsigned int							m_iPlayerLastPointedAt	{ 0U };
	bool									m_bDrawRadar			{ true };
	std::array<radar_point_s, MAX_POINTS>	m_rgCustomPoints;
	std::array<GLuint, ROLE_COUNT>			m_rgiRadarIcons;
	GLuint									m_iIdArrow				{ 0U };
	Matrix3x3								m_mxRadarTransform		{ Matrix3x3::Identity() };
};
