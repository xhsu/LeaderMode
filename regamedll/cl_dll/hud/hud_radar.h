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

class CHudRadar : public CBaseHUDElement
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float fTime);

	void DrawRadarDot(int x, int y, float z_diff, int iBaseDotSize, int flags, int r, int g, int b, int a);
	void DrawRadar(float flTime);
	void DrawPlayerLocation(void);
	int GetRadarSize(void);

public:
	bool m_bTrackArray[MAX_POINTS + 1];
	int m_iPlayerLastPointedAt;
	bool m_bDrawRadar;
	int m_HUD_radar;
	int m_HUD_radaropaque;

private:
	wrect_t* m_hrad;
	wrect_t* m_hradopaque;
	hSprite m_hRadar;
	hSprite m_hRadaropaque;
};
