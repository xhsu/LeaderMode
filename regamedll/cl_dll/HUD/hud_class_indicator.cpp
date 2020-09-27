/*

Created Date: Sep 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

SkillIndex CHudClassIndicator::GetPrimarySkill(void)
{
	switch (g_iRoleType)
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

int CHudClassIndicator::Init(void)
{
	Reset();
	m_bitsFlags = HUD_ACTIVE;
	m_fFade = 0;

	gHUD::AddHudElem(this);
	return TRUE;
}

int CHudClassIndicator::VidInit(void)
{
	m_fFade = 0;

	m_iClassesIcon[Role_UNASSIGNED]	= LoadDDS("texture/HUD/ClassesIcon/Doraemon.dds");

	// CT
	m_iClassesIcon[Role_Breacher]		= LoadDDS("texture/HUD/ClassesIcon/CT/Breacher.dds");
	m_iClassesIcon[Role_Commander]		= LoadDDS("texture/HUD/ClassesIcon/CT/Commander.dds");
	m_iClassesIcon[Role_Medic]			= LoadDDS("texture/HUD/ClassesIcon/CT/Medic.dds");
	m_iClassesIcon[Role_Sharpshooter]	= LoadDDS("texture/HUD/ClassesIcon/CT/Sharpshooter.dds");
	m_iClassesIcon[Role_SWAT]			= LoadDDS("texture/HUD/ClassesIcon/CT/SWAT.dds");

	// T
	m_iClassesIcon[Role_Arsonist]		= LoadDDS("texture/HUD/ClassesIcon/T/Arsonist.dds");
	m_iClassesIcon[Role_Assassin]		= LoadDDS("texture/HUD/ClassesIcon/T/Assassin.dds");
	m_iClassesIcon[Role_Godfather]		= LoadDDS("texture/HUD/ClassesIcon/T/Godfather.dds");
	m_iClassesIcon[Role_LeadEnforcer]	= LoadDDS("texture/HUD/ClassesIcon/T/LeadEnforcer.dds");
	m_iClassesIcon[Role_MadScientist]	= LoadDDS("texture/HUD/ClassesIcon/T/MadScientist.dds");

	return TRUE;
}

BOOL CHudClassIndicator::Draw(float flTime)
{
	// careful for the array bound!
	if (g_iRoleType < Role_UNASSIGNED || g_iRoleType >= ROLE_COUNT)
		return FALSE;

	int a = 255;
	if (m_fFade)
	{
		if (m_fFade > FADE_TIME)
			m_fFade = FADE_TIME;

		m_fFade -= (gHUD::m_flTimeDelta * 20);

		if (m_fFade <= 0)
		{
			a = 100;
			m_fFade = 0;
		}

		a = MIN_ALPHA + (m_fFade / FADE_TIME) * 128;

	}
	else
		a = MIN_ALPHA;

	// class icon
	gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
	gEngfuncs.pTriAPI->Brightness(1.0);

	// in order to make transparent fx on dds texture...
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(VEC_YELLOWISH.r, VEC_YELLOWISH.g, VEC_YELLOWISH.b, a / 255.0);

	gEngfuncs.pTriAPI->CullFace(TRI_NONE);

	glBindTexture(GL_TEXTURE_2D, m_iClassesIcon[g_iRoleType]);
	DrawUtils::Draw2DQuad(ScreenWidth - 128, ScreenHeight / 2, ScreenWidth, ScreenHeight / 2 + 128);

	// if you don't have a avaliable skill, i.e. a Role_UNASSIGNED, the progress bar is unnecessary.
	SkillIndex iSkillIndex = GetPrimarySkill();
	if (iSkillIndex == SkillIndex_ERROR)
		return TRUE;

	// progress bar.
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// a shinny colour make the player notice.
	Vector vecColour = VEC_YELLOWISH;
	if (m_flCurrentTime >= m_flTotalTime)
		vecColour = VEC_YELLOWISH + (VEC_SPRINGGREENISH - VEC_YELLOWISH) * (sin(gHUD::m_flUCDTime * 2.0f) + 1.0f) / 2.0f;	// make this sine wave bouncing between 0 to 1.

	glColor4f(vecColour.r, vecColour.g, vecColour.b, 1.0);

	DrawUtils::Draw2DQuadProgressBar2(ScreenWidth - 128, ScreenHeight / 2, 128, 128, 3, m_flCurrentTime / m_flTotalTime);

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	return TRUE;
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
		return;
	}

	if (m_flCurrentTime < 0.0f)
		m_flCurrentTime = 0;

	if (m_flCurrentTime > m_flTotalTime)
		m_flCurrentTime = m_flTotalTime;
}

void CHudClassIndicator::Reset(void)
{
	SetSkillTimer(1, FREEZED, 0);	// make the bar disappear.
}

void CHudClassIndicator::LightUp(void)
{
	m_fFade = FADE_TIME;
}

void CHudClassIndicator::SetSkillTimer(float flTotalTime, MODE iMode, float flCurrentTime)
{
	m_flTotalTime = flTotalTime;
	m_iMode = iMode;
	m_flCurrentTime = flCurrentTime;
}
