/*

Created Date: Mar 11 2020
Rebirth Date: May 18 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#pragma once

struct CHudAmmo
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

	// Drawing data.
	static constexpr auto COLOR_CLIP = Vector(0x968837);
	static constexpr auto MARGIN_CLIP_BPAMMO = 4;
	static constexpr auto MARGIN_WITH_EQP = 4;
	static constexpr auto MARGIN_INNERBLOCK = 2;
	static constexpr auto BORDER_THICKNESS = 2;
	static constexpr auto SIZE = Vector2D(144, (CHudEquipments::SIZE.y - MARGIN_CLIP_BPAMMO) / 2);
	static constexpr auto INNERBLOCK_HEIGHT = SIZE.y - MARGIN_INNERBLOCK * 2 - BORDER_THICKNESS * 2;
	static constexpr auto INNERBLOCK_MAX_WIDTH = SIZE.x - MARGIN_INNERBLOCK * 2 - BORDER_THICKNESS * 2;
	static constexpr auto FONT_SIZE = INNERBLOCK_HEIGHT;
	static inline Vector2D ANCHOR = CHudEquipments::ANCHOR - Vector2D(MARGIN_WITH_EQP + SIZE.width, 0), ANCHOR_INNERBLOCK = ANCHOR + Vector2D(MARGIN_INNERBLOCK + BORDER_THICKNESS);
	static inline Vector2D SIZE_INNERBLOCK = Vector2D(INNERBLOCK_MAX_WIDTH, INNERBLOCK_HEIGHT);
	static inline Vector2D ANCHOR_AMMO = ANCHOR + Vector2D(0, MARGIN_CLIP_BPAMMO);
	static inline Vector2D ANCHOR_CLIP_TEXT = ANCHOR, ANCHOR_AMMO_TEXT = ANCHOR_AMMO;
	static inline int m_hFont = 0;
	static inline float m_flAlpha = 255;	// byte
	static inline std::wstring m_wcsClip = L"\0", m_wcsAmmo = L"\0";
};
