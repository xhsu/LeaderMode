/*

Created Date: Sep 20 2020

Modern Warfare Dev Team
 - Luna the Reborn

*/

#include "precompiled.h"

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
	m_flSkillTimer = 1;

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

int CHudClassIndicator::Draw(float flTime)
{
	// careful for the array bound!
	if (g_iRoleType < Role_UNASSIGNED || g_iRoleType >= ROLE_COUNT)
		return FALSE;

	int r = 255, g = 255, b = 255, a = 255;
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

	UnpackRGB(r, g, b, RGB_YELLOWISH);

	// class icon
	gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
	gEngfuncs.pTriAPI->Brightness(1.0);

	// in order to make transparent fx on dds texture...
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(r / 255.0, g / 255.0, b / 255.0, a / 255.0);

	gEngfuncs.pTriAPI->CullFace(TRI_NONE);

	glBindTexture(GL_TEXTURE_2D, m_iClassesIcon[g_iRoleType]);
	DrawUtils::Draw2DQuad(0, 0, 128, 128);

	// progress bar.
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	UnpackRGB(r, g, b, RGB_YELLOWISH);	// we can't use scaled colour on our bar!
	glColor4f(r / 255.0, g / 255.0, b / 255.0, a / 255.0);

	DrawUtils::Draw2DQuadProgressBar(ScreenWidth / 2, ScreenHeight / 2, 128, 128, 8, m_bCooldingDown ? (m_flSkillTimer / g_rgSkillInfo) : ());

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	return TRUE;
}

void CHudClassIndicator::Think(void)
{
	m_flSkillTimer -= gHUD::m_flUCDTimeDelta;

	if (m_flSkillTimer < 0.0f)
		m_flSkillTimer = 0;
}

void CHudClassIndicator::Reset(void)
{
	m_flSkillTimer = 1;
}

void CHudClassIndicator::LightUp(void)
{
	m_fFade = FADE_TIME;
}

void CHudClassIndicator::SetSkillTimer(bool bCoolingDown, float flTimer)
{
	m_bCooldingDown = bCoolingDown;
	m_flSkillTimer = flTimer;
}
