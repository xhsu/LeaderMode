#ifndef ISURFACEV26_H
#define ISURFACEV26_H

#include <interface.h>

#include "VGUI.h"
#include "IHTML.h"

namespace vgui
{

class ISurfaceV26 : public IBaseInterface
{
public:
	virtual void Shutdown(void) = 0;
	virtual void RunFrame(void) = 0;
	virtual VPANEL GetEmbeddedPanel(void) = 0;
	virtual void SetEmbeddedPanel(VPANEL pPanel) = 0;
	virtual void PushMakeCurrent(VPANEL panel, bool useInsets) = 0;
	virtual void PopMakeCurrent(VPANEL panel) = 0;
	virtual void DrawSetColor(int r, int g, int b, int a) = 0;
	virtual void DrawSetColor(Color col) = 0;
	virtual void DrawFilledRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawOutlinedRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawLine(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawPolyLine(int *px, int *py, int numPoints) = 0;
	virtual void DrawSetTextFont(HFont font) = 0;
	virtual void DrawSetTextColor(int r, int g, int b, int a) = 0;
	virtual void DrawSetTextColor(Color col) = 0;
	virtual void DrawSetTextPos(int x, int y) = 0;
	virtual void DrawGetTextPos(int &x, int &y) = 0;
	virtual void DrawPrintText(const wchar_t *text, int textLen) = 0;
	virtual void DrawUnicodeChar(wchar_t wch) = 0;
	virtual void DrawUnicodeCharAdd(wchar_t wch) = 0;
	virtual void DrawFlushText(void) = 0;
	virtual IHTML *CreateHTMLWindow(IHTMLEvents *events, VPANEL context) = 0;
	virtual void PaintHTMLWindow(IHTML *htmlwin) = 0;
	virtual void DeleteHTMLWindow(IHTML *htmlwin) = 0;
	virtual void DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload) = 0;
	virtual void DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload) = 0;
	virtual void DrawSetTexture(int id) = 0;
	virtual void DrawGetTextureSize(int id, int &wide, int &tall) = 0;
	virtual void DrawTexturedRect(int x0, int y0, int x1, int y1) = 0;
	virtual bool IsTextureIDValid(int id) = 0;
	virtual int CreateNewTextureID(bool procedural = false) = 0;
	virtual void GetScreenSize(int &wide, int &tall) = 0;
	virtual void SetAsTopMost(VPANEL panel, bool state) = 0;
	virtual void BringToFront(VPANEL panel) = 0;
	virtual void SetForegroundWindow(VPANEL panel) = 0;
	virtual void SetPanelVisible(VPANEL panel, bool state) = 0;
	virtual void SetMinimized(VPANEL panel, bool state) = 0;
	virtual bool IsMinimized(VPANEL panel) = 0;
	virtual void FlashWindow(VPANEL panel, bool state) = 0;
	virtual void SetTitle(VPANEL panel, const wchar_t *title) = 0;
	virtual void SetAsToolBar(VPANEL panel, bool state) = 0;
	virtual void CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true) = 0;
	virtual void SwapBuffers(VPANEL panel) = 0;
	virtual void Invalidate(VPANEL panel) = 0;
	virtual void SetCursor(HCursor cursor) = 0;
	virtual bool IsCursorVisible(void) = 0;
	virtual void ApplyChanges(void) = 0;
	virtual bool IsWithin(int x, int y) = 0;
	virtual bool HasFocus(void) = 0;
	virtual bool SupportsFeature(int feature) = 0;
	virtual void RestrictPaintToSinglePanel(VPANEL panel) = 0;
	virtual void SetModalPanel(VPANEL panel) = 0;
	virtual VPANEL GetModalPanel(void) = 0;
	virtual void UnlockCursor(void) = 0;
	virtual void LockCursor(void) = 0;
	virtual void SetTranslateExtendedKeys(bool state) = 0;
	virtual VPANEL GetTopmostPopup(void) = 0;
	virtual void SetTopLevelFocus(VPANEL panel) = 0;
	virtual HFont CreateFont(void) = 0;
	virtual bool AddGlyphSetToFont(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange) = 0;
	virtual bool AddCustomFontFile(const char *fontFileName) = 0;
	virtual int GetFontTall(HFont font) = 0;
	virtual void GetCharABCwide(HFont font, int ch, int &a, int &b, int &c) = 0;
	virtual int GetCharacterWidth(HFont font, int ch) = 0;
	virtual void GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall) = 0;
	virtual VPANEL GetNotifyPanel(void) = 0;
	virtual void SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text) = 0;
	virtual void PlaySound(const char *fileName) = 0;
	virtual int GetPopupCount(void) = 0;
	virtual VPANEL GetPopup(int index) = 0;
	virtual bool ShouldPaintChildPanel(VPANEL childPanel) = 0;
	virtual bool RecreateContext(VPANEL panel) = 0;
	virtual void AddPanel(VPANEL panel) = 0;
	virtual void ReleasePanel(VPANEL panel) = 0;
	virtual void MovePopupToFront(VPANEL panel) = 0;
	virtual void MovePopupToBack(VPANEL panel) = 0;
	virtual void SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false) = 0;
	virtual void PaintTraverse(VPANEL panel) = 0;
	virtual void EnableMouseCapture(VPANEL panel, bool state) = 0;
	virtual void GetWorkspaceBounds(int &x, int &y, int &wide, int &tall) = 0;
	virtual void GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall) = 0;
	virtual void GetProportionalBase(int &width, int &height) = 0;
	virtual void CalculateMouseVisible(void) = 0;
	virtual bool NeedKBInput(void) = 0;
	virtual bool HasCursorPosFunctions(void) = 0;
	virtual void SurfaceGetCursorPos(int &x, int &y) = 0;
	virtual void SurfaceSetCursorPos(int x, int y) = 0;
	virtual void DrawTexturedPolygon(int *p, int n) = 0;
	virtual int GetFontAscent(HFont font, wchar_t wch) = 0;
	virtual void SetAllowHTMLJavaScript(bool state) = 0;
};

}

#define VGUI_SURFACEV26_INTERFACE_VERSION "VGUI_Surface026"

#endif