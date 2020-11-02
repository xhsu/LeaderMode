/*

Created Date: 06 Mar 2020

*/

#pragma once

typedef struct kbutton_s
{
	int		down[2];		// key nums holding it down
	int		state;			// low bit is down state
} kbutton_t;

struct kblist_t
{
	kblist_t* next;
	kbutton_t* pkey;
	char name[32];
};

extern kbutton_t	in_mlook;
extern kbutton_t	in_score;

extern cvar_t* m_pitch;
extern cvar_t* m_yaw;
extern cvar_t* m_forward;
extern cvar_t* m_side;
extern cvar_t* cl_pitchup;
extern cvar_t* cl_pitchdown;
extern cvar_t* cl_upspeed;
extern cvar_t* cl_forwardspeed;
extern cvar_t* cl_backspeed;
extern cvar_t* cl_sidespeed;
extern cvar_t* cl_walkingspeedmodifier;
extern cvar_t* cl_yawspeed;
extern cvar_t* cl_pitchspeed;
extern cvar_t* cl_anglespeedkey;
extern cvar_t* cl_vsmoothing;

void InitInput(void);
void KB_Init(void);
int KB_ConvertString(char* in, char** ppout);

bool CL_IsDead(void);	// why is it here??
float CL_KeyState(kbutton_t* key);
int CL_ButtonBits(bool bResetState = false);
void CL_ResetButtonBits(int bits);

void KeyDown(kbutton_t* b);
void KeyUp(kbutton_t* b);

kbutton_s* KB_Find2(const char* name);
BOOL HUD_Key_Event2(int down, int keynum, const char* pszCurrentBinding);
void CL_CreateMove2(float frametime, usercmd_s* cmd, int active);
