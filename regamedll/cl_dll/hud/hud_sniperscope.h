/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

constexpr float SCOPE_RATIOS[4] =
{
	1440.0f / 1080.0f,
	1620.0f / 1080.0f,
	1728.0f / 1080.0f,
	1920.0f / 1080.0f,
};

class CHudSniperScope : public CBaseHUDElement
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float flTime);

	int FindBestRatio(void);

private:
	GLuint m_iUsingScope;
	GLuint m_iScopes[4];
	Vector2D m_vecCentre;
	int m_left;
	int m_right;
};
