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

extern float g_flSpread;

class CHudCrosshair : public CBaseHudElement
{
public:
	float m_flCrosshairDistance		{ 0.1f };
	float m_flCurChDistance			{ 0.1f };
	float m_flCrosshairBarLength	{ 15.0f };
	double m_flAlphaMul				{ 1.0 };

public:
	virtual int Init(void);
	virtual int Draw(float flTime);
	virtual void Reset(void);
};
