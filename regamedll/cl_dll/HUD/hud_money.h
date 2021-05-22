/*

Created Date: Mar 11 2020
Remastered Date: May 15 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#pragma once


struct CHudAccountBalance	// depends on: CHudStatusIcons.
{
	typedef enum : byte { STATIC, MERGING, FADEOUT } PHASE;

	// Event functions.
	static void	Initialize(void);
	//static void	Shutdown(void);
	//static void	ConnectToServer(void);
	static void	Draw(float flTime, bool bIntermission);
	static void	Think(void);	// Use gHUD::m_flUCDTime
	//static void	OnNewRound(void);
	//static void	ServerAsksReset(void);

	// Message functions.
	static void MsgFunc_Money(int iNewMoney);
	static void MsgFunc_BlinkAcct(int iValue);

	// Custom functions.
	static void Reset(void);
	static float GetBottom(void);

	// Game data.
	static inline int m_iAccount = -1, m_iAccountDisplayed = -1, m_iAccountDelta = 0;
	static inline float m_flBlinkTime = 0;

	// Drawing data.
	static constexpr auto COLOR_REGULAR = Vector(1, 1, 1), COLOR_PLUS = VEC_GREENISH, COLOR_MINUS = VEC_REDISH, COLOR_DELTA_BLINK = VEC_REDISH - COLOR_REGULAR;
	static constexpr auto FONT_SIZE = 36;
	static constexpr auto MARGIN = 8;
	static constexpr auto MARGIN_NUMBER_SIGN = 72, MARGIN_DELTA_TO_TOTAL = 4;
	static constexpr auto BLINK_PERIOD = 0.4;
	static constexpr auto TEXT_PLUS = L"+", TEXT_MINUS = L"-", TEXT_DOLLAR_SIGN = L"$";
	static inline Vector COLOR_OF_DELTA = Vector(1, 1, 1);
	static inline Vector2D ANCHOR = Vector2D(CHudStatusIcons::ANCHOR.x, CHudStatusIcons::ANCHOR.y + CHudStatusIcons::SIZE.height + CHudStatusIcons::MARGIN_TEXTS + CHudStatusIcons::FONT_SIZE + MARGIN), ANCHOR_DELTA_SIGN = Vector2D();
	static inline int NUMBER_MAX_WIDTH = 0;
	static inline int m_hFont = 0;
	static inline int m_iNumTextWidth = 0, m_iDeltaTextWidth = 0;
	static inline std::wstring m_wcsNumber, m_wcsDelta;
	static inline const wchar_t* m_pwcsSign = nullptr;
	static inline float m_flAlphaOfDelta = 0, m_flAlpha = 255;	// byte
	static inline PHASE m_iPhase = FADEOUT;
	static inline float m_flTimeDeltaStatic = 0;
};
