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

	m_hClassFont = gFontFuncs::CreateFont();
	gFontFuncs::AddGlyphSetToFont(m_hClassFont, "Trajan Pro", FONT_TALL, FW_BOLD, 1, 0, FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE, 0x0, 0x2E7F);
	gFontFuncs::AddGlyphSetToFont(m_hClassFont, "I.MingCP", FONT_TALL, FW_BOLD, 1, 0, FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE, 0x2E80, 0xFFFF);

	gHUD::AddHudElem(this);
	return TRUE;
}

int CHudClassIndicator::VidInit(void)
{
	m_fFade = 0;

	m_iClassesIcon[Role_UNASSIGNED]	= LoadDDS("sprites/ClassesIcon/Doraemon.dds");

	// CT
	m_iClassesIcon[Role_Breacher]		= LoadDDS("sprites/ClassesIcon/CT/Breacher.dds");
	m_iClassesIcon[Role_Commander]		= LoadDDS("sprites/ClassesIcon/CT/Commander.dds");
	m_iClassesIcon[Role_Medic]			= LoadDDS("sprites/ClassesIcon/CT/Medic.dds");
	m_iClassesIcon[Role_Sharpshooter]	= LoadDDS("sprites/ClassesIcon/CT/Sharpshooter.dds");
	m_iClassesIcon[Role_SWAT]			= LoadDDS("sprites/ClassesIcon/CT/SWAT.dds");

	// T
	m_iClassesIcon[Role_Arsonist]		= LoadDDS("sprites/ClassesIcon/T/Arsonist.dds");
	m_iClassesIcon[Role_Assassin]		= LoadDDS("sprites/ClassesIcon/T/Assassin.dds");
	m_iClassesIcon[Role_Godfather]		= LoadDDS("sprites/ClassesIcon/T/Godfather.dds");
	m_iClassesIcon[Role_LeadEnforcer]	= LoadDDS("sprites/ClassesIcon/T/LeadEnforcer.dds");
	m_iClassesIcon[Role_MadScientist]	= LoadDDS("sprites/ClassesIcon/T/MadScientist.dds");

	return TRUE;
}

static char szKeyText[64];

BOOL CHudClassIndicator::Draw(float flTime)
{
	if (gHUD::m_bPlayerDead)
		return TRUE;

	DrawLeftPortion(flTime);	// At the left bottom conor.

	// careful for the array bound!
	if (g_iRoleType < Role_UNASSIGNED || g_iRoleType >= ROLE_COUNT)
		return FALSE;

	m_iAlpha = 255;
	if (m_fFade)
	{
		if (m_fFade > FADE_TIME)
			m_fFade = FADE_TIME;

		m_fFade -= (gHUD::m_flTimeDelta * 20);

		if (m_fFade <= 0)
		{
			m_iAlpha = 100;
			m_fFade = 0;
		}

		m_iAlpha = MIN_ALPHA + (m_fFade / FADE_TIME) * 128;

	}
	else
		m_iAlpha = MIN_ALPHA;

	DrawRightPortion(flTime);

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

void CHudClassIndicator::DrawLeftPortion(float flTime)
{
	int iTall = 0, iWidth = 0;
	gFontFuncs::GetTextSize(m_hClassFont, g_rgwcsRoleNames[g_iRoleType].c_str(), &iWidth, &iTall);

	int x = 0, y = gHUD::m_Battery.m_flLastDrawingY - iTall / 2;
	int x2 = HEALTH_BASIC_OFS - 2, y2 = y - HEALTH_BASIC_OFS;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(VEC_YELLOWISH.r, VEC_YELLOWISH.g, VEC_YELLOWISH.b, m_iAlpha / 255.0);

	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
	glBegin(GL_POLYGON);
	glVertex2f(x, y - 2);
	glVertex2f(x2, y2 - 2);
	glVertex2f(x2, y2 + 1);
	glVertex2f(x, y + 1);
	glEnd();
	glDisable(GL_POLYGON_SMOOTH);

	glBegin(GL_QUADS);
	glVertex2f(x2 + 2 + iWidth + 4, y2 - 1);
	glVertex2f(HEALTH_BASIC_OFS + gHUD::m_Battery.m_iHeight + HEALTH_ICON_BAR_INTERSPACE + HEALTH_BAR_LENGTH * 2, y2 - 1);
	glVertex2f(HEALTH_BASIC_OFS + gHUD::m_Battery.m_iHeight + HEALTH_ICON_BAR_INTERSPACE + HEALTH_BAR_LENGTH * 2, y2 + 1);
	glVertex2f(x2 + 2 + iWidth + 4, y2 + 1);
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	gFontFuncs::DrawSetTextFont(m_hClassFont);
	gFontFuncs::DrawSetTextPos(x2 + 2, y2 - iTall / 2);
	gFontFuncs::DrawSetTextColor(235, 235, 235, m_iAlpha);	// have to keep the text white.
	gFontFuncs::DrawPrintText(g_rgwcsRoleNames[g_iRoleType].c_str());
}

void CHudClassIndicator::DrawRightPortion(float flTime)
{
	float x = ScreenWidth - INDICATOR_SIZE;
	float y = gHUD::m_WeaponList.m_flLastY - INDICATOR_SIZE - FONT_TALL / 2;

	// get & show the keyname.
	Q_snprintf(szKeyText, _countof(szKeyText) - 1U, "[%s]", gExtFuncs.pfnKey_NameForBinding("executeskill"));
	const wchar_t* pwcsKeybind = ANSIToUnicode(szKeyText);

	// 1. Class icon
	gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);
	gEngfuncs.pTriAPI->Brightness(1.0);

	// in order to make transparent fx on dds texture...
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(VEC_YELLOWISH.r, VEC_YELLOWISH.g, VEC_YELLOWISH.b, m_iAlpha / 255.0);

	gEngfuncs.pTriAPI->CullFace(TRI_NONE);

	glBindTexture(GL_TEXTURE_2D, m_iClassesIcon[g_iRoleType]);
	DrawUtils::Draw2DQuad(x, y, x + INDICATOR_SIZE, y + INDICATOR_SIZE);

	// for CHudMoney. It's already the highest point.
	m_flLastY = y;

	// if you don't have a avaliable skill, i.e. a Role_UNASSIGNED, the progress bar is unnecessary.
	if (GetPrimarySkill() == SkillIndex_ERROR)
		return;

	// 2. Progression bar.
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// a shinny colour make the player notice.
	Vector vecColour = VEC_YELLOWISH;
	if (m_flCurrentTime >= m_flTotalTime)
		vecColour = VEC_YELLOWISH + (VEC_SPRINGGREENISH - VEC_YELLOWISH) * gHUD::GetOscillation();	// make this sine wave bouncing between 0 to 1.

	glColor4f(vecColour.r, vecColour.g, vecColour.b, 1.0);

	DrawUtils::Draw2DQuadProgressBar2(x, y, INDICATOR_SIZE, INDICATOR_SIZE, 3, m_flCurrentTime / m_flTotalTime);

	// 3. HUD art border.
	y += INDICATOR_SIZE * 0.75f;

	int iTall = 0, iWidth = 0;
	gFontFuncs::GetTextSize(m_hClassFont, pwcsKeybind, &iWidth, &iTall);

	glColor4f(VEC_YELLOWISH.r, VEC_YELLOWISH.g, VEC_YELLOWISH.b, m_iAlpha / 255.0);

	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
	glBegin(GL_POLYGON);
	glVertex2f(x, y - 1);
	glVertex2f(x, y + 1);
	glVertex2f(x - 40, y + 32 - 1);
	glVertex2f(x - 40, y + 32 + 1);
	glEnd();
	glDisable(GL_POLYGON_SMOOTH);

	DrawUtils::Draw2DQuad(
		x - 40, y + 32 - 1,
		x - 40 - iWidth, y + 32 + 1
	);

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	// 4. Keybind text
	x = x - 40 - iWidth;
	y = y + 32 - iTall - 1;

	vecColour = Vector(1, 1, 1);
	if (m_flCurrentTime >= m_flTotalTime)	// cooldown is over.
		vecColour += (VEC_SPRINGGREENISH - Vector(1, 1, 1)) * gHUD::GetOscillation();

	gFontFuncs::DrawSetTextFont(m_hClassFont);
	gFontFuncs::DrawSetTextPos(x, y);
	gFontFuncs::DrawSetTextColor(vecColour, m_flCurrentTime >= m_flTotalTime ? 1.0f : (float(m_iAlpha) / 255));	// only high-bright if not in the cooldown.
	gFontFuncs::DrawPrintText(pwcsKeybind);
}
