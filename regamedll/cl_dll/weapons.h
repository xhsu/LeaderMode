/*

Created Date: Mar 12 2020

*/

#pragma once

#include "../dlls/weapons.h"	// only import this.

extern int g_runfuncs;

// export func
void HUD_PostRunCmd2(local_state_t* from, local_state_t* to, usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed);
