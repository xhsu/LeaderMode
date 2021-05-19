#ifndef IVGUIDLL_H
#define IVGUIDLL_H

#include <interface.h>

class IVGuiDLL : public IBaseInterface
{
public:
	virtual void Init(CreateInterfaceFn *factoryList, int numFactories) = 0;
};

#define VGUI_IVGUIDLL_INTERFACE_VERSION "IVGuiDLL001"

#endif