/*

Created Date: Sep 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

class CHudClassIndicator : public CBaseHudElement
{
public:
	typedef enum { DECREASE = -1, FREEZED = 0, INCREASE = 1} MODE;

	static constexpr int INDICATOR_SIZE = 128;

public:
	static	SkillIndex	GetPrimarySkill(void);

public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float flTime);
	virtual void Think(void);
	virtual void Reset(void);

public:
	void	LightUp(void);
	void	SetSkillTimer(float flTotalTime, MODE iMode, float flCurrentTime);
	void	DrawClassName(float flTime);

public:
	std::array<GLuint, ROLE_COUNT> m_iClassesIcon;
	float m_fFade;
	float m_flTotalTime;
	MODE m_iMode;
	float m_flCurrentTime;
	int m_iAlpha{ 255 };
	int m_hClassFont{ 0 };
};
