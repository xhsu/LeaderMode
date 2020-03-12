/*

Created Date: 06 Mar 2020

precompiled.h
LUNA: this is a dummy file created while I trying to fix a VS2019 bug.
If I want to shared the pm_shared.cpp among CL and SV, I have to leave #include "precompiled.h" line outside of #ifndef _CLIENT_DLL.
Or I have to turn off precompile function from VS2019.
However, I decide to do neither, but create a dummy for CL side.

*/

#pragma once

#include "cl_base.h"