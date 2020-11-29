/*

Created Date: Mar 12 2020
Reincarnation Date: Nov 24 2020

*/

#pragma once

enum TacticalSchemes
{
	Scheme_UNASSIGNED = 0U,	// disputation
	Doctrine_SuperiorFirepower,
	Doctrine_MassAssault,
	Doctrine_GrandBattleplan,
	Doctrine_MobileWarfare,

	SCHEMES_COUNT
};

extern std::array<unsigned, 4U> g_rgiManpower;
extern std::array<TacticalSchemes, SCHEMES_COUNT> g_rgiTeamSchemes;
extern std::array<std::wstring, ROLE_COUNT> g_rgwcsRoleNames;
extern std::array<std::wstring, SCHEMES_COUNT> g_rgwcsSchemeNames;

class CHudScenarioStatus : public CBaseHudElement
{
public:
	static constexpr int GAP_PLAYERNAME_MANPOWER = 10;
	static constexpr int GAP_PLAYERNAME_ICON = 4;
	static constexpr int GAP_ROWS = 2;
	static constexpr int GAP_MANPOWER_INTERICON = 2;
	static constexpr int GAP_SCHEMEICON_TEXT = 4;

	static constexpr int MANPOWER_ICON_SIZE = 32;
	static constexpr int SCHEME_ICON_SIZE = 60;

	static constexpr std::array<double, 10U> MANPOWER_ALPHAS = {1, 1, 1, 1, 1, 0.84, 0.68, 0.52, 0.36, 0.2};

public:
	int Init(void);
	int VidInit(void);
	void Reset(void);
	int Draw(float fTime);

public:
	std::array<float, MAX_PLAYERS> m_rgflTimeSpeakerIconHide;
	GLuint m_iIdSpeaker{ 0U };
	GLuint m_iIdManpower{ 0U };
	float m_flManpowerTextureRatio{ 1.0f };
	std::array<std::wstring, 4U> m_rgwcsManpowerTexts;
	std::array<GLuint, SCHEMES_COUNT> m_rgiIdSchemeTexture{ 0U, 0U, 0U, 0U, 0U };
	int m_hNameFont{ 0 };
};
