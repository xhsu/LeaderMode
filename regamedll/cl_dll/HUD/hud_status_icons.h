/*

Created Date: Mar 12 2020
Remastered Date: May 14 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#pragma once

extern std::array<GLuint, SCHEMES_COUNT> g_rgiIdSchemeIcons;

typedef struct status_icon_s
{
	GLuint m_iIdTexture{ 0 };
	unsigned short m_bitsFlags{ 0U };

	// Icon
	Vector m_vecColour{ Vector(1, 1, 1) };
	Vector m_vecOscillationColorDelta{ VEC_REDISH - Vector(1, 1, 1) };
	float m_flColorOscillationPeriod{ M_PI };
	float m_flAlpha{ 255 };	// byte
	float m_flOscillationAlphaDelta{ 0 };	// byte
	float m_flAlphaOscillationPeriod{ M_PI };

	// Text
	std::wstring m_wcsText{ L"\0" };
	int m_iTextLength{ 0 };
	Vector m_vecTextColour{ Vector(1, 1, 1) };
	float m_flTextAlpha{ 1 };	// GLfloat, [0-1]

} status_icon_t;

struct CHudStatusIcons
{
	// Event functions.
	static void	Initialize(void);
	//static void	Shutdown(void);
	//static void	ConnectToServer(void);
	static void	Draw(float flTime, bool bIntermission);
	static void	Think(void);	// Use gHUD::m_flUCDTime. Only used for team schemes.
	//static void	OnNewRound(void);
	//static void	ServerAsksReset(void);

	// Message functions.
	static void MsgFunc_StatusIcon(int iSize, void* pbuf);
	static void MsgFunc_Scheme(TacticalSchemes iScheme);

	// Custom functions.
	static void Reset(void);

	// Game data.
	static inline std::unordered_map<std::string, status_icon_t> m_mapShowing;
	static inline std::unordered_map<std::string, GLuint> m_mapTextureLib;

	// Drawing data.
	static constexpr auto MARGIN = Vector2D(16);
	static constexpr auto MARGIN_ICONS = 8;
	static constexpr auto MARGIN_TEXTS = 4;
	static constexpr auto SIZE = Vector2D(64);
	static constexpr auto FONT_SIZE = 16;
	static inline Vector2D ANCHOR = Vector2D();
	static inline int m_hFont = 0;
};
