/*

Created Date: May 24 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#include "precompiled.h"

using namespace vgui;

vgui::CTeamButton::CTeamButton(Panel* parent, const char* panelName, const wchar_t* text, Panel* pActionSignalTarget, const char* pCmd) : Button(parent, panelName, text, pActionSignalTarget, pCmd)
{
	_string = text;
}

void vgui::CTeamButton::SetUpImage(const char* fileName)
{
	if (Q_stristr(fileName, ".dds"))
		_upImage = LoadDDS(fileName);
	else if (Q_stristr(fileName, ".tga"))
		_upImage = LoadTGA(fileName);
	else if (Q_stristr(fileName, ".bmp"))
		_upImage = LoadBMP(fileName);
}

void vgui::CTeamButton::SetFocusImage(const char* fileName)
{
	if (Q_stristr(fileName, ".dds"))
		_focusImage = LoadDDS(fileName);
	else if (Q_stristr(fileName, ".tga"))
		_focusImage = LoadTGA(fileName);
	else if (Q_stristr(fileName, ".bmp"))
		_focusImage = LoadBMP(fileName);
}

void vgui::CTeamButton::SetDownImage(const char* fileName)
{
	if (Q_stristr(fileName, ".dds"))
		_downImage = LoadDDS(fileName);
	else if (Q_stristr(fileName, ".tga"))
		_downImage = LoadTGA(fileName);
	else if (Q_stristr(fileName, ".bmp"))
		_downImage = LoadBMP(fileName);
}

void vgui::CTeamButton::SetDisableImage(const char* fileName)
{
	if (Q_stristr(fileName, ".dds"))
		_disableImage = LoadDDS(fileName);
	else if (Q_stristr(fileName, ".tga"))
		_disableImage = LoadTGA(fileName);
	else if (Q_stristr(fileName, ".bmp"))
		_disableImage = LoadBMP(fileName);
}

void vgui::CTeamButton::Paint(void)
{
	if (!ShouldPaint())
		return;

	// Get size for draw.
	// However, this is a picture with able beneath. Have to spare some room.
	int iWidth = GetWide();
	int iHeight = GetTall();
	int iSide = Q_min(iWidth, iHeight);

	// Setup render
	DrawUtils::glRegularTexDrawingInit(0xFFFFFF, 0xFF);

	// Draw button image
	auto iImageToDraw = _upImage;	// Default status.
	if (IsEnabled())
	{
		if (IsDepressed() && _downImage)	// Button pressed
		{
			iImageToDraw = _downImage;
		}
		else if (IsArmed() && _focusImage)	// Button actived (Hovered?)
		{
			iImageToDraw = _focusImage;
		}

		// else: default: _upImage;
	}
	else
	{
		if (_disableImage)
		{
			iImageToDraw = _disableImage;
		}

		// If disable image is not set, we draw the _upImage.
	}

	DrawUtils::glSetTexture(iImageToDraw);
	DrawUtils::Draw2DQuad(0, 0, iSide, iSide);	// When drawing, the origin of canvas shifted to the boundary of current control.

	// Get the empty region of this button.
	int iEmptyRegionHeight = Q_max(iHeight, iWidth) - iSide;
	DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, 0xFF);
	DrawUtils::Draw2DQuadNoTex(0, iSide, iWidth, iHeight);
	DrawUtils::glRegularPureColorDrawingExit();

	gFontFuncs::DrawSetTextFont(gHUD::m_hTrajanProFont);
	gFontFuncs::DrawSetTextColor(0x0, 0xFF);
	gFontFuncs::DrawSetTextPos(0, iSide);
	gFontFuncs::DrawPrintText(_string.c_str());
}

void vgui::CTeamButton::ApplySettings(KeyValues* inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	const char* fileName;

	fileName = inResourceData->GetString("upImage");

	if (fileName && *fileName)
	{
		SetUpImage(fileName);
	}

	fileName = inResourceData->GetString("focusImage");

	if (fileName && *fileName)
	{
		SetFocusImage(fileName);
	}

	fileName = inResourceData->GetString("downImage");

	if (fileName && *fileName)
	{
		SetDownImage(fileName);
	}

	fileName = inResourceData->GetString("disableImage");

	if (fileName && *fileName)
	{
		SetDisableImage(fileName);
	}
}

void vgui::CTeamButton::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// Unset button border
	SetDefaultBorder(nullptr);
	SetDepressedBorder(nullptr);
	SetKeyFocusBorder(nullptr);
}
