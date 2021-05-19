#ifndef BASEPANEL_H
#define BASEPANEL_H

#include <../vgui_controls/Panel.h>

typedef struct tex_s tex_t;

class CBasePanel : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CBasePanel, vgui::Panel);

public:
	CBasePanel();

	void RunFrame(void);

protected:
	virtual void PaintBackground(void);
};

#endif
