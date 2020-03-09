/*

Created Date: 06 Mar 2020

*/

#pragma once

#define MOUSE_BUTTON_COUNT 5	// LUNA: mine have 6 in total.

extern cvar_t* sensitivity;

void IN_Init(void);
void IN_Move(float frametime, usercmd_t* cmd);

void IN_ActivateMouse2(void);
void IN_DeactivateMouse2(void);
void IN_MouseEvent2(int mstate);
void IN_Accumulate2(void);
void IN_ClearStates2(void);
