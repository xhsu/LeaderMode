/*

Created Date: Mar 27 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#include "precompiled.h"

/////////////////
// CBaseButton //
/////////////////

bool CBaseButton::KeyEvent(bool bDown, int iKeyIndex, const char* pszCurrentBinding)
{
	if (IsMouseInside() && iKeyIndex == K_MOUSE1)
	{
		if (bDown && !m_bClickedOn)
		{
			OnPress();
			m_bClickedOn = true;
		}
		else if (!bDown && m_bClickedOn)
		{
			OnRelease();
			m_bClickedOn = false;
		}

		return false;
	}

	return CBasePanel::KeyEvent(bDown, iKeyIndex, pszCurrentBinding);	// true to allow engine procees.
}

void CBaseButton::Think(void)
{
	bool bCurInSide = IsMouseInside();

	if (bCurInSide && !m_bInSide)
	{
		OnPlace();
		m_bInSide = true;
	}
	else if (!bCurInSide && m_bInSide)
	{
		OnMoveAway();
		m_bInSide = false;
	}

	return CBasePanel::Think();
}

void CBaseButton::OnRelease(void)
{
	if (Q_strlen(m_szCommand))
		gEngfuncs.pfnServerCmd(m_szCommand);
}

bool CBaseButton::IsMouseInside(void)
{
	auto x = GetX();
	auto y = GetY();

	int iMouseX = 0, iMouseY = 0;
	gEngfuncs.GetMousePosition(&iMouseX, &iMouseY);

	float ofs_x = iMouseX - x;
	float ofs_y = iMouseY - y;

	// mouse in the range of this button.
	return (ofs_x >= 0.0f && ofs_x <= m_flWidth && ofs_y >= 0.0f && ofs_y <= m_flHeight);
}

/////////////////////
// CBaseTextButton //
/////////////////////

bool CBaseTextButton::Initialize(void)
{
	m_Background.m_bitsFlags |= HUD_ACTIVE;
	m_Text.m_bitsFlags |= HUD_ACTIVE;

	AddChild(&m_Background);
	AddChild(&m_Text);

	return CBaseButton::Initialize();
}

bool CBaseTextButton::Draw(float flTime)
{
	// can't just call the CBasePanel's version. it would draw a huge while background instead a borderline.
	if (!m_lstChildren.empty())
	{
		for (auto pChild : m_lstChildren)
		{
			if (pChild->m_bitsFlags & HUD_ACTIVE)
				pChild->Draw(flTime);
		}
	}

	// we only draw a border here.
	// do it post.
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(m_flScaledColour.r, m_flScaledColour.g, m_flScaledColour.b, m_flScaledColour.a);

	DrawUtils::Draw2DHollowQuad(GetX(), GetY(), m_flWidth, m_flHeight);

	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	return true;
}

void CBaseTextButton::SetSize(float flWidth, float flHeight)
{
	m_Background.m_flWidth = m_flWidth = flWidth;
	m_Background.m_flHeight = m_flHeight = flHeight;

	int iTextWidth = 0, iTextHeight = 0;
	gFontFuncs.GetTextSize(m_Text.m_hFont, m_Text.m_wszWords, &iTextWidth, &iTextHeight);

	// re-center the text.
	m_Text.m_vecCoord.x = (m_flWidth - float(iTextWidth)) / 2.0f;
	m_Text.m_vecCoord.y = (m_flHeight - float(iTextHeight)) / 2.0f;
}

//////////////////////
// CBaseTextButton2 //
//////////////////////

bool CBaseTextButton2::Draw(float flTime)
{
	if (m_bInSide)
	{
		SetBackgroundOn();
	}
	else
	{
		SetNoBackground();
	}

	return CBaseTextButton::Draw(flTime);
}

void CBaseTextButton2::SetSize(float flWidth, float flHeight)
{
	m_flWidth = flWidth;
	m_flHeight = flHeight;

	int iTextWidth = 0, iTextHeight = 0;
	gFontFuncs.GetTextSize(m_Text.m_hFont, m_Text.m_wszWords, &iTextWidth, &iTextHeight);

	// re-center the text.
	m_Text.m_vecCoord.x = (m_flWidth - float(iTextWidth)) / 2.0f;
	m_Text.m_vecCoord.y = (m_flHeight - float(iTextHeight)) / 2.0f;
}

void CBaseTextButton2::SetGap(float flGap)
{
	m_Background.m_vecCoord = Vector2D(flGap, flGap + 1);
	m_Background.m_flWidth = m_flWidth - flGap * 2.0f - 1;
	m_Background.m_flHeight = m_flHeight - flGap * 2.0f - 1;
}
