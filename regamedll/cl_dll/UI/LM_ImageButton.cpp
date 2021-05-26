/*

Created Date: May 24 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#include "precompiled.h"

using namespace vgui;

inline void vgui::image_s::Load(const char* fileName)
{
	if (Q_stristr(fileName, ".dds"))
		m_iId = LoadDDS(fileName, &m_iWidth, &m_iHeight);
	else if (Q_stristr(fileName, ".tga"))
		m_iId = LoadTGA(fileName, &m_iWidth, &m_iHeight);
	else if (Q_stristr(fileName, ".bmp"))
		m_iId = LoadBMP(fileName, &m_iWidth, &m_iHeight);

	if (m_iHeight)
		m_flW2HRatio = float(m_iWidth) / float(m_iHeight);
}

vgui::LMImageButton::LMImageButton(Panel* parent, const char* panelName, const char* text, Panel* pActionSignalTarget, const char* pCmd) : Button(parent, panelName, text, pActionSignalTarget, pCmd)
{
	static wchar_t string[128];
	GetText(string, sizeof(string));
	_string = string;
}

vgui::LMImageButton::LMImageButton(Panel* parent, const char* panelName, const wchar_t* text, Panel* pActionSignalTarget, const char* pCmd) : Button(parent, panelName, text, pActionSignalTarget, pCmd)
{
	_string = text;
}

bool vgui::LMImageButton::AddGlyphSetToFont(const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange)
{
	if (!_font)
		_font = gFontFuncs::CreateFont();

	return gFontFuncs::AddGlyphSetToFont(_font, windowsFontName, tall, weight, blur, scanlines, flags, lowRange, highRange);
}

void LMImageButton::SetCommand(const char *command, ...)
{
	va_list arg;
	static char string[256];

	va_start(arg, command);
	Q_vsnprintf(string, charsmax(string), command, arg);
	va_end(arg);

	BaseClass::SetCommand(string);
}

void vgui::LMImageButton::Paint(void)
{
	if (!ShouldPaint())
		return;

	// Setup render
	DrawUtils::glRegularTexDrawingInit(0xFFFFFF, GetAlpha());

	// Draw button image
	auto iImageToDraw = &_upImage;	// Default status.
	if (IsEnabled())
	{
		if (IsDepressed() && _downImage)	// Button pressed
		{
			iImageToDraw = &_downImage;
		}
		else if (IsArmed() && _focusImage)	// Button actived (Hovered?)
		{
			iImageToDraw = &_focusImage;
		}

		// else: default: _upImage;
	}
	else
	{
		if (_disableImage)
		{
			iImageToDraw = &_disableImage;
		}

		// If disable image is not set, we draw the _upImage.
	}

	// Get size for draw.
	// However, this is a picture with able beneath. Have to spare some room.
	int iWidth = GetImageWidth();
	int iHeight = round(iImageToDraw->CalculateHeightByDefinedWidth(iWidth));

	DrawUtils::glSetTexture(iImageToDraw->m_iId);
	DrawUtils::Draw2DQuad(0, 0, iWidth, iHeight);	// When drawing, the origin of canvas shifted to the boundary of current control.

	DrawUtils::glRegularPureColorDrawingInit(0xFFFFFF, GetAlpha());
	DrawUtils::Draw2DQuadNoTex(0, iHeight, iWidth, GetTall());	// The bottom-right conor of the box.
	DrawUtils::glRegularPureColorDrawingExit();

	if (!_string.empty())
	{
		gFontFuncs::DrawSetTextFont(_font ? _font : gHUD::m_hTrajanProFont);
		gFontFuncs::DrawSetTextColor(0x0, GetAlpha());
		gFontFuncs::DrawSetTextPos(MARGIN_TEXT, iHeight);	// Just below the image.
		gFontFuncs::DrawPrintText(_string.c_str());
	}
}

void vgui::LMImageButton::ApplySettings(KeyValues* inResourceData)
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

void vgui::LMImageButton::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// Unset button border
	SetDefaultBorder(nullptr);
	SetDepressedBorder(nullptr);
	SetKeyFocusBorder(nullptr);
}

void vgui::LMImageButton::InvalidateLayout(bool layoutNow, bool reloadScheme)
{
	BaseClass::InvalidateLayout(layoutNow, reloadScheme);

	m_flSparedBlankHeight = GetTall() - round(GetImageWidth() / _upImage.m_flW2HRatio);	// Use the default size as the 
}
