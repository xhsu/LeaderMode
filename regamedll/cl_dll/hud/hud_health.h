/*

Created Date: Mar 11 2020
Remastered Date: May 13 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#pragma once

#define DMG_IMAGE_LIFE 2

#define DMG_IMAGE_POISON 0
#define DMG_IMAGE_ACID 1
#define DMG_IMAGE_COLD 2
#define DMG_IMAGE_DROWN 3
#define DMG_IMAGE_BURN 4
#define DMG_IMAGE_NERVE 5
#define DMG_IMAGE_RAD 6
#define DMG_IMAGE_SHOCK 7

#define NUM_DMG_TYPES 8

// damage types definition.
#include "../public/regamedll/regamedll_const.h"

typedef struct
{
	float fExpire;
	float fBaseline;
	int x, y;
}
DAMAGE_IMAGE;

#define MAX_POINTS 64

constexpr int HEALTH_BASIC_OFS = 12;
constexpr int HEALTH_BAR_LENGTH = 100;
constexpr int HEALTH_BAR_WIDTH = 10;
constexpr int HEALTH_SHAKE_AMPLITUDE = 3;
constexpr int HEALTH_ICON_BAR_INTERSPACE = 10;

struct CHudHealth	// Depends on: CHudBattery
{
	// Event functions.
	static void	Initialize(void);
	//static void	Shutdown(void);
	static void	ConnectToServer(void);
	static void	Draw(float flTime, bool bIntermission);
	static void	Think(void);	// Use gHUD::m_flUCDTime
	//static void	OnNewRound(void);
	static void	ServerAsksReset(void);

	// Message functions.
	static void MsgFunc_Health(int iNewHealth);
	static void MsgFunc_Damage(int armor, int damageTaken, int bitsDamage, const Vector& vecFrom);
#ifdef _DEBUG
	static void CmdFunc_Health(void);
#endif // _DEBUG

	// Custom functions.
	static float GetMaxHealth(void);

	// Game data.
	static inline short m_iHealth = 100;
	static inline float m_flPercentage = 1;
	static inline std::wstring m_wcsHPText = L"100";

	// Drawing data.
	static constexpr auto COLOR = VEC_REDISH;
	static constexpr auto LOW_HP_ALPHA_OSCL_PERIOD = 0.25;
	static constexpr auto LOW_HP_SHAKING_AMP = 2;
	static constexpr auto MARGIN = Vector2D(CHudBattery::MARGIN_LEFT, CHudBattery::MARGIN_BOTTOM);	// X: between this and class portrait on the left. Y: between this and AP bar.
	static constexpr auto SIZE = Vector2D(CHudRadar::SIZE.x - CHudClassIndicator::PORTRAIT_SIZE.width - MARGIN.x, CHudBattery::BAR_SIZE.y);
	static constexpr auto BORDER_THICKNESS = 2;
	static constexpr auto PROGRESS_BAR_MARGIN = 2;
	static constexpr auto INNERBLOCK_HEIGHT = SIZE.y - BORDER_THICKNESS * 2 - PROGRESS_BAR_MARGIN * 2;	// standalone, constexpr version. for font initialization.
	static constexpr auto FONT_SIZE = INNERBLOCK_HEIGHT;
	static inline auto ANCHOR = CHudBattery::ICON_ANCHOR + Vector2D(0, CHudBattery::ICON_SIZE.height + MARGIN.height);
	static inline auto INNERBLOCK_SIZE = Vector2D(SIZE.width - BORDER_THICKNESS * 2 - PROGRESS_BAR_MARGIN * 2, INNERBLOCK_HEIGHT), INNERBLOCK_ANCHOR = ANCHOR + Vector2D(BORDER_THICKNESS + PROGRESS_BAR_MARGIN);
	static inline auto TEXT_ANCHOR = ANCHOR + Vector2D(0, (SIZE.height - FONT_SIZE) / 2);
	static inline int m_hFont = 0;
	static inline float m_flAlpha = 255;	// byte
	static inline int m_iTextLength = 0;
};
