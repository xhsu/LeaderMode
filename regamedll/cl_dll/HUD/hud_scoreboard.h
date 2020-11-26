/*

Created Date: Mar 26 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#pragma once

inline bool IsConnected(int playerIndex)
{
	return (g_PlayerInfoList[playerIndex].name && g_PlayerInfoList[playerIndex].name[0] != 0);
}

inline int GetTeamCounts(short teamnumber)
{
	int count = 0;

	for (int i = 1; i <= MAX_PLAYERS; i++)
	{
		gEngfuncs.pfnGetPlayerInfo(i, &g_PlayerInfoList[i]);

		if (!IsConnected(i))
			continue;

		if (g_PlayerExtraInfo[i].m_iTeam == teamnumber)
			count++;
	}

	return count;
}

inline int GetTeamAliveCounts(short teamnumber)
{
	int count = 0;

	for (int i = 1; i <= MAX_PLAYERS; i++)
	{
		gEngfuncs.pfnGetPlayerInfo(i, &g_PlayerInfoList[i]);

		if (!IsConnected(i))
			continue;

		if (g_PlayerExtraInfo[i].m_iTeam == teamnumber && g_PlayerExtraInfo[i].m_iHealth > 0)
			count++;
	}

	return count;
}

extern int g_rgiTeamScore[4];

class CHudScoreboard : public CBaseHudElement
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float flTime);
	virtual void Think(void);
	virtual void Reset(void) { m_Baseboard.OnNewRound(); }
	virtual void InitHUDData(void) { m_Baseboard.InitHUDData(); }
	virtual void Shutdown(void) { m_Baseboard.Shutdown(); }

public:
	enum ScoreboardElementName
	{
		SBE_Name = 0,
		SBE_Class,
		SBE_HP,
		SBE_Money,
		SBE_Kill,
		SBE_Death,
		SBE_KDA,
		SBE_Ping
	};

	// Localise keys
	static const char* m_rgszScoreboardElemKeyName[8];

	// Localise texts
	static const wchar_t* m_pwszTeamName[4];
	static const wchar_t* m_pwszPlayerCalled;
	static const wchar_t* m_pwszDeathCalled;
	static const wchar_t* m_rgpwcScoreboardElementName[8];
	static const wchar_t* m_pwcTeamWinsText;

public:
	CBasePanel		m_Baseboard;
	CBaseText		m_ServerName;
	CBasePanel		m_Line01;
	int				m_hPlayerNameFont;
	float			m_flChunkOffset;
};
