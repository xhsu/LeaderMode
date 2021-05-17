/*

Created Date: Mar 23 2020
Remastered Date: May 12 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Model		- Matoilet
	Artist		- HL&CL

*/

#pragma once

class CHudGrenade : public CBaseHudElement
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(float flTime);
	virtual void Think(void) {}
	virtual void Reset(void) {}
	virtual void InitHUDData(void) {}
	virtual void Shutdown(void) {}

public:
	hSprite m_rghGrenadeIcons[EQP_COUNT];
	wrect_t m_rgrcGrenadeIcons[EQP_COUNT];
};

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

	// Custom functions.
	static void Reset(void);
	static void DrawCount(Vector2D vecOrigin, EquipmentIdType iEqpId, BYTE iSlotPos);

	// Game data.

	// Drawing data.
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
