/*

Created Date: Mar 11 2020

*/

#pragma once

struct DeathNoticeItem
{
	char szKiller[MAX_PLAYER_NAME_LENGTH * 2];
	char szVictim[MAX_PLAYER_NAME_LENGTH * 2];
	int iId;
	int iHeadShotId;
	int iSuicide;
	int iTeamKill;
	int iNonPlayerKill;
	float flDisplayTime;
	float* KillerColor;
	float* VictimColor;
	//vgui::IImage** DrawBg; // UNDONE: this is what BTE use to simulate CSOL.
};

#define MAX_DEATHNOTICES		8
#define MAX_DRAWDEATHNOTICES	4

#define DEATHNOTICE_DISPLAY_TIME	6
#define KILLICON_DISPLAY_TIME		1

// TODO, UNDONE : this entire class needs to reconstruct due to both CSBTE and CSMoE are sabortaging this class via implanting tons of CSOL code.
class CHudDeathNotice : public CBaseHUDElement
{
public:
	int Init(void);
	void InitHUDData(void);
	void Reset(void);
	int VidInit(void);
	int Draw(float flTime);
	int GetDeathNoticeY(void);

public:
	int MsgFunc_DeathMsg(const char* pszName, int iSize, void* pbuf);

private:
	int m_HUD_d_skull;
	float m_lastKillTime;
	int m_headSprite, m_headWidth;
	bool m_showIcon, m_showKill;
	int m_iconIndex;
	float m_killEffectTime, m_killIconTime;
	int m_iFontHeight;
};
