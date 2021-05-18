/*

Created Date: Mar 23 2020
Remastered Date: May 17 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Model		- Matoilet
	Artist		- HL&CL

*/

#pragma once

typedef void (*FUNC_CustomDrawFunction)(Vector2D vecOrigin, EquipmentIdType iEqpId, BYTE iSlotPos);

struct CHudEquipments
{
	using BarSlotVecs = std::array<Vector2D, 5>;
	using BarSlotDynV = std::vector<Vector2D>;
	typedef enum : BYTE { MOVING_OUT, MOVING_IN, } STAGE;

	// Event functions.
	static void	Initialize(void);
	//static void	Shutdown(void);
	//static void	ConnectToServer(void);
	static void	Draw(float flTime, bool bIntermission);
	static void	Think(void);	// Use gHUD::m_flUCDTime
	//static void	OnNewRound(void);
	//static void	ServerAsksReset(void);

	// Message functions.
	static void OnPrev(void);
	static void OnNext(void);
	static void WakeUp(void);
	static void MsgFunc_Flashlight(bool bOn, int iBattery);
	static void MsgFunc_FlashBat(int iBattery);

	// Custom functions.
	static void Reset(void);
	static void DrawCount(Vector2D vecOrigin, EquipmentIdType iEqpId, BYTE iSlotPos);
	static void Flashlight(Vector2D vecOrigin, EquipmentIdType iEqpId, BYTE iSlotPos);

	// Game data.
	static inline float m_flFLBatteryPercentage = 1;
	static inline int m_iFLBattery = 100;
	static inline bool m_bFLOn = false;

	// Drawing data.
	static constexpr auto COLOR_FLASHLIGHT = 0xFFC600, ALPHA_FL_CHARGE = 48;
	static constexpr auto FL_BORDER_THICKNESS = 2;
	static constexpr std::array<float, 5> ALPHA_FOR_EACH_SLOT = { 64, 128, 255, 128, 64 };
	static constexpr auto FONT_SIZE = 20;
	static constexpr auto SIZE = Vector2D(96, 96);
	static constexpr BarSlotVecs SIZE_FOR_EACH_SLOT = { SIZE / 4, SIZE / 2, SIZE, SIZE / 2, SIZE / 4 };
	static constexpr auto MARGIN = Vector2D(18);
	static constexpr auto DRIFT_SPEED = 5.0, TIME_MOVING_OUT = 5.0;
	static constexpr auto MARGIN_BETWEEN_ITEMS = 8, MARGIN_NUMBER_IN_ICON = 2;
	static inline Vector2D ANCHOR = Vector2D(ScreenWidth, ScreenHeight) - MARGIN, ANCHOR_MOVING_OUT = ANCHOR - SIZE * 2;
	static inline BarSlotVecs ANCHOR_FOR_EACH_SLOT;
	static inline std::array<GLuint, EQP_COUNT> EQUIPMENT_ICONS;
	static inline std::array<FUNC_CustomDrawFunction, EQP_COUNT> CUSTOM_DRAW_FUNCS;
	static inline int m_hFont = 0;
	static inline float m_flAlpha = 255;	// byte
	static inline STAGE m_iStage = MOVING_IN;
	static inline BarSlotDynV m_rgvSlotCurOrigin, m_rgvSlotCurSize;
	static inline std::vector<float> m_rgflSlotCurAlpha;
	static inline float m_flNextStageIncrease = 0;
};
