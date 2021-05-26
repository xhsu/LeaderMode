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
	typedef struct image_s
	{
		GLuint m_iId{ 0U };
		int m_iWidth{ 0 }, m_iHeight{ 0 };
		float m_flW2HRatio{ 0.0f };

		inline constexpr explicit operator GLuint() const { return m_iId; }
		inline constexpr operator Vector2D() const { return Vector2D(m_iWidth, m_iHeight); }
		inline constexpr explicit operator bool() const { return m_iId != 0U; }

		inline void Load(const char* fileName);
		inline constexpr float CalculateHeightByDefinedWidth(float flWidth) const { return m_flW2HRatio != 0.0f ? flWidth / m_flW2HRatio : 0.0f; }
		inline constexpr float CalculateWidthByDefinedHeight(float flHeight) const { return flHeight * m_flW2HRatio; }

	} image_t;

	class LMImageButton : public Button
	{
		DECLARE_CLASS_SIMPLE(LMImageButton, Button);

	public:
		LMImageButton(Panel* parent, const char* panelName, const char* text, Panel* pActionSignalTarget = nullptr, const char* pCmd = nullptr);
		LMImageButton(Panel* parent, const char* panelName, const wchar_t* text, Panel* pActionSignalTarget = nullptr, const char* pCmd = nullptr);

		inline void SetUpImage(const char* fileName) { _upImage.Load(fileName); }
		inline void SetFocusImage(const char* fileName) { _focusImage.Load(fileName); }
		inline void SetDownImage(const char* fileName) { _downImage.Load(fileName); }
		inline void SetDisableImage(const char* fileName) { _disableImage.Load(fileName); }
		inline void SetUpImage(const image_t* image) { Q_memcpy(&_upImage, image, sizeof(image_t)); }
		inline void SetFocusImage(const image_t* image) { Q_memcpy(&_focusImage, image, sizeof(image_t)); }
		inline void SetDownImage(const image_t* image) { Q_memcpy(&_downImage, image, sizeof(image_t)); }
		inline void SetDisableImage(const image_t* image) { Q_memcpy(&_disableImage, image, sizeof(image_t)); }
		bool AddGlyphSetToFont(const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange);
		void SetCommand(const char* command, ...);
		bool IsPendingSelected(void) { return IsEnabled() && (IsDepressed() || IsArmed()); }
		virtual int GetImageWidth(void) { return GetWide(); }

	public:	// Overrides
		void PaintBackground(void) final { /* Draw no background*/ }
		void Paint(void) override;
		void PaintBorder(void) final { /* Draw no border*/ }
		void ApplySettings(KeyValues* inResourceData) final;
		void ApplySchemeSettings(IScheme* pScheme) final;
		void InvalidateLayout(bool layoutNow = false, bool reloadScheme = false) override;

	public:
		static constexpr auto MARGIN_TEXT = 2;

	protected:
		image_t _upImage;
		image_t _focusImage;
		image_t _downImage;
		image_t _disableImage;
		std::wstring _string{ L"\0" };
		int _font{ 0 };
		float m_flSparedBlankHeight{ 0.0f };
	};
}
