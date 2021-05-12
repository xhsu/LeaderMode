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

struct CHudEquipments
{
	// Event functions.
	static void	Initialize(void);
	//static void	Shutdown(void);
	static void	ConnectToServer(void);
	static void	Draw(float flTime, bool bIntermission);
	static void	Think(void);	// Use gHUD::m_flUCDTime
	static void	OnNewRound(void);
	static void	ServerAsksReset(void);

	// Message functions.

	// Custom functions.

	// Game data.

	// Drawing data.
	static inline Vector2D ANCHOR = Vector2D();
	static inline std::array<GLuint, EQP_COUNT> EQUIPMENT_ICONS;
	static inline int FONT = 0;
};
