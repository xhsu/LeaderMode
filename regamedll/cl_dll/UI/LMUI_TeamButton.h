/*

Created Date: May 24 2021

Modern Warfare Dev Team
	Programmer	- Luna the Reborn
	Advisor		- Crsky
	Artist		- HL&CL

*/

#pragma once

#include <string>

namespace vgui
{
	class CTeamButton : public Button
	{
		DECLARE_CLASS_SIMPLE(CTeamButton, Button);

	public:
		CTeamButton(Panel* parent, const char* panelName, const wchar_t* text, Panel* pActionSignalTarget = nullptr, const char* pCmd = nullptr);

		void SetUpImage(const char* fileName);
		void SetFocusImage(const char* fileName);
		void SetDownImage(const char* fileName);
		void SetDisableImage(const char* fileName);

	protected:
		void PaintBackground(void) final { /* Draw no background*/ }
		void Paint(void) final;
		void PaintBorder(void) final { /* Draw no border*/ }
		void ApplySettings(KeyValues* inResourceData) final;
		void ApplySchemeSettings(IScheme* pScheme) final;

	private:
		GLuint _upImage{ 0 };
		GLuint _focusImage{ 0 };
		GLuint _downImage{ 0 };
		GLuint _disableImage{ 0 };
		std::wstring _string{ L"\0" };
	};
}
