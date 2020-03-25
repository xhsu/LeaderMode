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

class CHudSniperScope : public CBaseHudElement
{
public:
	enum Phase
	{
		FADE_IN,	// fade in the black screen.
		FADE_STAY,
		FADE_OUT,	// fade out to the game world.
	};

public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float flTime);
	virtual void Reset(void);

	int FindBestRatio(void);
	void SetFade(float flRate);
	void SetFadeFromBlack(float flRate, float flStayTime = 0.1f);

private:
	GLuint m_iUsingScope;
	GLuint m_iScopes[4];
	Phase m_iPhase;
	float m_flAlpha;
	float m_flFadeSpeed;
	float m_flTimeToFadeOut;
};
