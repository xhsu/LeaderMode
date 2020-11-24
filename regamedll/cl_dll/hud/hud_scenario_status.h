/*

Created Date: Mar 12 2020
Reincarnation Date: Nov 24 2020

*/

#pragma once

extern std::array<int, 4U> g_rgiManpower;

class CHudScenarioStatus : public CBaseHudElement
{
public:
	int Init(void);
	int VidInit(void);
	void Reset(void);
	int Draw(float fTime);

public:
	void MsgFunc_Scenario(int iSize, void* pbuf);

public:
	hSprite m_hSprite;
	wrect_t m_rect;
	int m_alpha;
	float m_nextFlash;
	float m_flashInterval;
	int m_flashAlpha;
	float m_flTimeSpeakerIconHide{ 0.0f };
	GLuint m_iIdSpeaker{ 0U };
	GLuint m_iIdManpower{ 0U };
	float m_flManpowerTextureRatio{ 1.0f };
};
