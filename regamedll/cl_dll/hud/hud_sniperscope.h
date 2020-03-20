/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

class CHudSniperScope : public CBaseHUDElement
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float flTime);

private:
	GLuint m_iScopeArc[4];
	Vector2D m_vecCentre;
	int m_left;
	int m_right;
};
