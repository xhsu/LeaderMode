/*

Created Date: Mar 11 2020
Remastered Date: May 11 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL
*/

#pragma once

struct CHudBattery	// Place after CHudRadar
{
	// Event functions.
	static void	Initialize		(void);
	//static void	Shutdown		(void);
	//static void	ConnectToServer	(void);
	static void	Draw			(float flTime, bool bIntermission);
	static void	Think			(void);
	//static void	OnNewRound		(void);
	//static void	ServerAsksReset	(void);

	// Message functions.
	static void MsgFunc_Battery(int& iNewArmourValue);
	static void MsgFunc_ArmorType(int& iArmourType);

	// Custom functions.
	static float GetMaxArmour(void);
	static void	Reset(void);

	// Game data.
	static inline GLuint m_iIdKevlar = 0U, m_iIdAssaultSuit = 0U;
	static inline unsigned short m_iMaxAP = 100, m_iAP = 0, m_iArmorType = ARMOR_NONE;

	// Drawing data.
	static constexpr decltype(auto) COLOR = Vector(8.0 / 255.0, 131.0 / 255.0, 211.0 / 255.0);
	static constexpr decltype(auto) BORDER_THICKNESS = 2;
	static constexpr decltype(auto) PROGRESS_BAR_GAP_SIZE = 2;
	static constexpr decltype(auto) GAP_SIZE = 12;
	static constexpr decltype(auto) BAR_SIZE = Vector2D(96, 24), ICON_SIZE = Vector2D(24, 24);
	static inline Vector2D BAR_MARGINE = Vector2D(), ICON_MARGINE = Vector2D();
	static inline Vector2D INNERBLOCK_MARGINE = Vector2D(), INNERBLOCK_SIZE = Vector2D();
	static inline float m_flAlpha = 255;
	static inline int m_hFont = 0;
};
