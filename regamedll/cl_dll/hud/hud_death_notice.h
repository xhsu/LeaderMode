/*

Created Date: Mar 11 2020

*/

#pragma once

struct DeathNoticeItem
{
	char	m_szVictim[64];
	char	m_szKiller[64];
	bool	m_bHeadshot;
	bool	m_bGhostKill;
	hSprite	m_hWeaponSprite;
	wrect_t m_pWeaponSpriteInfo;
	float	m_flTimeToRemove;
	float*	m_rgflVictimColour;
	float*	m_rgflKillerColour;
};

#define MAX_DEATHNOTICES		8
#define MAX_DRAWDEATHNOTICES	4

#define DEATHNOTICE_DISPLAY_TIME	6
#define DEATHNOTICE_X_BASE_OFS		24
#define DEATHNOTICE_Y_BASE_OFS		24	// below flashlight hud.
#define DEATHNOTICE_INTERSPACE		5

// TODO, UNDONE : this entire class needs to reconstruct due to both CSBTE and CSMoE are sabortaging this class via implanting tons of CSOL code.
class CHudDeathNotice : public CBaseHudElement
{
public:
	int Init(void);
	void InitHUDData(void);
	void Reset(void);
	int VidInit(void);
	int Draw(float flTime);
	void Think(void);

public:
	void MsgFunc_DeathMsg(int iKillerIndex, int iVictimIndex, bool bHeadshot, const char *szWeaponName);

private:
	int m_HUD_d_skull;
	int m_headSprite;
	int m_headWidth;
	int m_iFontHeight;
	std::list<DeathNoticeItem> m_lstQueue;	// we should not use std::quene, because we need 4 items per time.
};
