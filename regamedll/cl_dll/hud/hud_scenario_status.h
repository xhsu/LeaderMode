/*

Created Date: Mar 12 2020

*/

#pragma once

class CHudScenarioStatus : public CBaseHUDElement
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
};
