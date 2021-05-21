#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <vgui_controls/Panel.h>

namespace vgui
{

class CViewPort : public Panel
{
	DECLARE_CLASS_SIMPLE(CViewPort, Panel);

public:
	CViewPort(void);
	virtual ~CViewPort(void);

protected:
	virtual void PaintBackground(void);
};

}

extern vgui::CViewPort *g_pViewport;

#endif