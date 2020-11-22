/*

Created Date: Mar 11 2020

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

typedef struct
{
	GLuint  textureId; // OV图文理ID
	GLuint  width;     // OV图宽度
	GLuint  height;    // OV图高度
	GLfloat zoom;      // 用于计算OV区域大小
	GLfloat originX;   // OV区域中心点X坐标
	GLfloat originY;   // OV区域中心点Y坐标
	bool    rotated;   // OV区域是否需要旋转
} overview_t;

class CHudRadar : public CBaseHudElement
{
public:
	int Init(void);
	int VidInit(void);
	void Reset(void);
	int Draw(float fTime);

	void DrawRadarDot(int x, int y, float z_diff, int iBaseDotSize, int flags, int r, int g, int b, int a);
	void DrawRadar(float flTime);
	void DrawPlayerLocation(void);
	Vector Translate(const Vector& vecOrigin, float flScanRange, float flRadarHudRadius);	// translate a point onto radar map. the returning z coord is actually z_diff rather than normal meaning.
	Vector2D Translate(const Vector& vecOrigin);
	void Translate(Vector2D& vecOrigin);

public:
	inline void DrawRadarDot(const Vector& vec, int iBaseDotSize, int flags, int r, int g, int b, int a) { DrawRadarDot(vec.x, vec.y, vec.z, iBaseDotSize, flags, r, g, b, a); }
	inline void DrawRadarDot(const Vector& vec, int iBaseDotSize, int flags, const PackedColorVec& colour) { DrawRadarDot(vec.x, vec.y, vec.z, iBaseDotSize, flags, colour.r, colour.g, colour.b, colour.a); }

public:
	std::array<bool, MAX_POINTS + 1> m_bTrackArray;
	unsigned int m_iPlayerLastPointedAt;
	bool m_bDrawRadar{ true };
	int m_HUD_radar;
	int m_HUD_radaropaque;
	std::array<radar_point_s, MAX_POINTS> m_rgCustomPoints;
	std::array<GLuint, ROLE_COUNT> m_rgiRadarIcons;
	GLuint m_iIdArrow{ 0U };
	overview_t m_OVData;

	struct
	{
		GLuint		m_iId		{ 0U };
		int			m_iHeight	{ 0 };
		int			m_iWidth	{ 0 };
		Vector2D	m_vecScale	{ 0.0f, 0.0f };
	}
	m_Overview;

private:
	const wrect_t* m_hrad;
	const wrect_t* m_hradopaque;
	hSprite m_hRadar;
	hSprite m_hRadaropaque;
};
