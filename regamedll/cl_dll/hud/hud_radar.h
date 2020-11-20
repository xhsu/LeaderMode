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

class CHudRadar : public CBaseHudElement
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float fTime);

	void DrawRadarDot(int x, int y, float z_diff, int iBaseDotSize, int flags, int r, int g, int b, int a);
	void DrawRadar(float flTime);
	void DrawPlayerLocation(void);
	int GetRadarSize(void);
	Vector Translate(const Vector& vecOrigin, float flRange, float flRadarRadius);	// translate a point onto radar map. the returning z coord is actually z_diff rather than normal meaning.

public:
	inline void DrawRadarDot(const Vector& vec, int iBaseDotSize, int flags, int r, int g, int b, int a) { DrawRadarDot(vec.x, vec.y, vec.z, iBaseDotSize, flags, r, g, b, a); }
	inline void DrawRadarDot(const Vector& vec, int iBaseDotSize, int flags, const PackedColorVec& colour) { DrawRadarDot(vec.x, vec.y, vec.z, iBaseDotSize, flags, colour.r, colour.g, colour.b, colour.a); }

public:
	std::array<bool, MAX_POINTS + 1> m_bTrackArray;
	unsigned int m_iPlayerLastPointedAt;
	bool m_bDrawRadar;
	int m_HUD_radar;
	int m_HUD_radaropaque;
	std::array<radar_point_s, MAX_POINTS> m_rgCustomPoints;
	std::array<GLuint, ROLE_COUNT> m_rgiRadarIcons;

private:
	wrect_t* m_hrad;
	wrect_t* m_hradopaque;
	hSprite m_hRadar;
	hSprite m_hRadaropaque;
};
