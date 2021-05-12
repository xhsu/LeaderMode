/*

Created Date: Mar 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#define ACCURACY_AIR				(1 << 0) // accuracy depends on FL_ONGROUND
#define ACCURACY_SPEED				(1 << 1)
#define ACCURACY_DUCK				(1 << 2) // more accurate when ducking
#define ACCURACY_MULTIPLY_BY_14		(1 << 3) // accuracy multiply to 1.4

struct CHudCrosshair
{
	// Event functions.
	static void	Initialize(void);
	//static void	Shutdown(void);
	//static void	ConnectToServer(void);
	static void	Draw(float flTime, bool bIntermission);
	static void	Think(void);	// Use gHUD::m_flUCDTime
	//static void	OnNewRound(void);
	//static void	ServerAsksReset(void);

	// Message functions.

	// Custom functions.
	static void Reset(void);

	// Game data.
	static inline float m_flSpread = 0.1f;
	static inline float m_flCrosshairDistance = 0.1f, m_flCurChDistance = 0.1f;

	// Drawing data.
	static constexpr decltype(auto) CALIBRATING_CROSSHAIR_SIZE = 2;
	static constexpr decltype(auto) CROSSHAIR_LENGTH = 15;
	static inline float m_flAlpha = 255;
};
