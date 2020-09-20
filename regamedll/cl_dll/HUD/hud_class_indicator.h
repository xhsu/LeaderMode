/*

Created Date: Sep 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

class CHudClassIndicator : public CBaseHudElement
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float flTime);
	virtual void Think(void);
	virtual void Reset(void);

public:
	void	LightUp(void);
	void	SetSkillTimer(bool bCoolingDown, float flTimer);

public:
	GLuint m_iClassesIcon[ROLE_COUNT];
	float m_fFade;
	float m_flSkillTimer;
	bool m_bCooldingDown;
};
