/*

Created Date: May 09 2021

Inherite from
	HUD_Battery	(Mar 11 2020)
	HUD_Health	(Mar 11 2020)

Modern Warfare Dev Team
	Programmer		- Luna the Reborn
	Art & Design	- HL&CL

*/

#pragma once

class CHudVitality : public CBaseHudElement
{
public:
	struct HealthLossDB
	{
		HealthLossDB(float flDamage) : m_flAlpha(255.0f), m_flAmount(flDamage) {}

		float m_flAlpha{ 255.0f };
		float m_flAmount{ 1.0f };
	};

public:
	virtual int Init(void) final;
	//virtual int VidInit(void) final;
	virtual int Draw(float fTime) final;
	//virtual void Reset(void) final;
	//virtual void Think(void) final;

	void MsgFunc_Role(RoleTypes iRole);

public:
	float	m_flMaxHealth	{ 100.0f };
	float	m_flHealth		{ 100.0f };
	float	m_flHPMaxPercent{ 0.5f };
	float	m_flAPMaxPercent{ 0.5f };
	std::list<HealthLossDB> m_lstHealthLoss;
};
