/*

Created Date: Sep 20 2020
Remastered Date: May 12 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Artist		- HL&CL

*/

#include "precompiled.h"

void CHudClassIndicator::Initialize(void)
{
	gHUD::m_lstElements.push_back({
		Initialize,
		nullptr,
		Reset,
		Draw,
		Think,
		nullptr,
		Reset,
		});

	CLASS_PORTRAIT[Role_UNASSIGNED] = LoadDDS("sprites/ClassesIcon/Doraemon.dds");

	// CT
	CLASS_PORTRAIT[Role_Breacher] = LoadDDS("sprites/ClassesIcon/CT/Breacher.dds");
	CLASS_PORTRAIT[Role_Commander] = LoadDDS("sprites/ClassesIcon/CT/Commander.dds");
	CLASS_PORTRAIT[Role_Medic] = LoadDDS("sprites/ClassesIcon/CT/Medic.dds");
	CLASS_PORTRAIT[Role_Sharpshooter] = LoadDDS("sprites/ClassesIcon/CT/Sharpshooter.dds");
	CLASS_PORTRAIT[Role_SWAT] = LoadDDS("sprites/ClassesIcon/CT/SWAT.dds");

	// T
	CLASS_PORTRAIT[Role_Arsonist] = LoadDDS("sprites/ClassesIcon/T/Arsonist.dds");
	CLASS_PORTRAIT[Role_Assassin] = LoadDDS("sprites/ClassesIcon/T/Assassin.dds");
	CLASS_PORTRAIT[Role_Godfather] = LoadDDS("sprites/ClassesIcon/T/Godfather.dds");
	CLASS_PORTRAIT[Role_LeadEnforcer] = LoadDDS("sprites/ClassesIcon/T/LeadEnforcer.dds");
	CLASS_PORTRAIT[Role_MadScientist] = LoadDDS("sprites/ClassesIcon/T/MadScientist.dds");
}

void CHudClassIndicator::Draw(float flTime, bool bIntermission)
{
	if (bIntermission)
		return;

	if (CL_IsDead() || g_iUser1)
		return;

	DrawUtils::glRegularTexDrawingInit(0xFFFFFF, m_flAlpha);	// Pure white.
	DrawUtils::glSetTexture(CLASS_PORTRAIT[g_iRoleType]);
	DrawUtils::Draw2DQuad(PORTRAIT_ANCHOR, PORTRAIT_ANCHOR + PORTRAIT_SIZE);

	if (GetPrimarySkill() == SkillIndex_ERROR)
		return;

	// Outter line
	DrawUtils::glRegularPureColorDrawingInit(m_vecCurColor, 1);	// Outter line always highlighted.
	DrawUtils::Draw2DQuadProgressBar2(PORTRAIT_ANCHOR, PORTRAIT_SIZE, BORDER_THICKNESS, m_flPercentage);
	DrawUtils::glRegularPureColorDrawingExit();
}

void CHudClassIndicator::Think(void)
{
	switch (m_iMode)
	{
	case INCREASE:
		m_flCurrentTime += gHUD::m_flUCDTimeDelta;
		break;

	case DECREASE:
		m_flCurrentTime -= gHUD::m_flUCDTimeDelta;
		break;

	case FREEZED:
	default:
		break;
	}

	m_flCurrentTime = Q_clamp<float>(m_flCurrentTime, 0, m_flTotalTime);
	m_flPercentage = m_flCurrentTime / m_flTotalTime;
	m_flAlpha = Q_clamp<float>(m_flAlpha - gHUD::m_flUCDTimeDelta * 20.0f, MIN_ALPHA, 255);

	if (m_flPercentage >= 1.0f - FLT_EPSILON)
	{
		m_vecCurColor = COLOR_REGULAR + (COLOR_READY - COLOR_REGULAR) * gHUD::GetOscillation(2.0 * M_PI / READY_COLOR_OCSILLATING_PERIOD);
	}
	else if (m_flPercentage <= 0.25f)
	{
		m_vecCurColor = COLOR_REGULAR + (COLOR_WARNING - COLOR_REGULAR) * gHUD::GetOscillation(2.0 * M_PI / DEPLETING_COLOR_OCSILLATING_PERIOD);
	}
	else
	{
		m_vecCurColor += (COLOR_REGULAR - m_vecCurColor) * gHUD::m_flUCDTimeDelta * 5;
	}
}

void CHudClassIndicator::Reset(void)
{
	// Game data.
	m_flTotalTime = 10.0, m_flCurrentTime = 10.0, m_flPercentage = 1;
	m_iMode = FREEZED;

	// Drawing data.
	PORTRAIT_ANCHOR = CHudRadar::ANCHOR + Vector2D(0, CHudRadar::SIZE.height + MARGIN);
	m_flAlpha = 255;
	m_vecCurColor = COLOR_REGULAR;
}

void CHudClassIndicator::MsgFunc_Role(const RoleTypes& iRole)
{
	m_flAlpha = 255;	// Lights up.
}

void CHudClassIndicator::MsgFunc_SkillTimer(const float& flTotalTime, const MODE& iMode, const float& flCurrentTime)
{
	if (iMode != m_iMode)
		m_flAlpha = 255;	// lights up when READY/DEPLETED/USING.

	m_flTotalTime = flTotalTime;
	m_iMode = iMode;
	m_flCurrentTime = flCurrentTime;
}

SkillIndex CHudClassIndicator::GetPrimarySkill(RoleTypes iRole)
{
	switch (iRole)
	{
	case Role_Arsonist:
		return SkillIndex_IncendiaryAmmo;

	case Role_Assassin:
		return SkillIndex_Invisible;

	case Role_Breacher:
		return SkillIndex_InfiniteGrenade;

	case Role_Commander:
		return SkillIndex_RadarScan;

	case Role_Godfather:
		return SkillIndex_Gavelkind;

	case Role_LeadEnforcer:
		return SkillIndex_ResistDeath;

	case Role_MadScientist:
		return SkillIndex_TaserGun;

	case Role_Medic:
		return SkillIndex_HealingShot;

	case Role_Sharpshooter:
		return SkillIndex_EnfoceHeadshot;

	case Role_SWAT:
		return SkillIndex_Bulletproof;

	case Role_UNASSIGNED:
	default:
		return SkillIndex_ERROR;
	}
}
