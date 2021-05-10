/*

Created Date: Mar 11 2020

*/

#pragma once

class CHudHeadName : public CBaseHudElement
{
public:
	static constexpr unsigned INTERSPACE = 2;

	static GLuint m_iIdSpeaker;
	static int m_hHeadFont;

public:
	int Draw(float flTime);
	int Init(void);
	int VidInit(void);
	void Reset(void);

public:
	void BuildUnicodeList(void);
	bool IsValidEntity(cl_entity_s* pEntity);

public:
	void MsgFunc_HeadName(int& iDrawType);

public:
	wchar_t m_rgwcsPlayerNames[MAX_CLIENTS][32];
	float m_flTimeUpdatePlayerNames{ 0.0 };
	short m_iDrawType{ HEADNAME_TEAMMATE };
	std::array<float, MAX_CLIENTS> m_rgflTimeSpeakerIconHide;
};
