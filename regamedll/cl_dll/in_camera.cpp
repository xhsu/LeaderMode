/*

Created Date: 06 Mar 2020

*/

#include "precompiled.h"

//-------------------------------------------------- Constants

#define CAM_DIST_DELTA 1.0
#define CAM_ANGLE_DELTA 2.5
#define CAM_ANGLE_SPEED 2.5
#define CAM_MIN_DIST 30.0
#define CAM_ANGLE_MOVE .5
#define MAX_ANGLE_DIFF 10.0
#define PITCH_MAX 90.0
#define PITCH_MIN 0
#define YAW_MAX  135.0
#define YAW_MIN	 -135.0

enum ECAM_Command
{
	CAM_COMMAND_NONE = 0,
	CAM_COMMAND_TOTHIRDPERSON = 1,
	CAM_COMMAND_TOFIRSTPERSON = 2
};

//-------------------------------------------------- Global Variables

cvar_t* cam_command;
cvar_t* cam_snapto;
cvar_t* cam_idealyaw;
cvar_t* cam_idealpitch;
cvar_t* cam_idealdist;
cvar_t* cam_contain;

cvar_t* c_maxpitch;
cvar_t* c_minpitch;
cvar_t* c_maxyaw;
cvar_t* c_minyaw;
cvar_t* c_maxdistance;
cvar_t* c_mindistance;

// pitch, yaw, dist
Vector cam_ofs;

// In third person
int cam_thirdperson;
int cam_mousemove; //true if we are moving the cam with the mouse, False if not
int iMouseInUse = 0;
int cam_distancemove;
extern int mouse_x, mouse_y;  //used to determine what the current x and y values are
int cam_old_mouse_x, cam_old_mouse_y; //holds the last ticks mouse movement
POINT		cam_mouse;

//-------------------------------------------------- Local Variables

static kbutton_t cam_pitchup, cam_pitchdown, cam_yawleft, cam_yawright;
static kbutton_t cam_in, cam_out;

//-------------------------------------------------- Local Functions

float MoveToward(float cur, float goal, float maxspeed)
{
	if (cur != goal)
	{
		if (Q_fabs(cur - goal) > 180.0f)
		{
			if (cur < goal)
				cur += 360.0;
			else
				cur -= 360.0;
		}

		if (cur < goal)
		{
			if (cur < goal - 1.0f)
				cur += (goal - cur) / 4.0f;
			else
				cur = goal;
		}
		else
		{
			if (cur > goal + 1.0)
				cur -= (cur - goal) / 4.0f;
			else
				cur = goal;
		}
	}


	// bring cur back into range
	if (cur < 0.0f)
		cur += 360.0f;
	else if (cur >= 360.0f)
		cur -= 360.0f;

	return cur;
}

//-------------------------------------------------- Global Functions

void CAM_ToThirdPerson(void)
{
#if !defined( _DEBUG )
	if (gEngfuncs.GetMaxClients() > 1)
	{
		// no thirdperson in multiplayer.
		return;
	}
#endif
	Vector viewangles;
	gEngfuncs.GetViewAngles(viewangles);

	if (!cam_thirdperson)
	{
		cam_thirdperson = 1;

		cam_ofs[YAW] = viewangles[YAW];
		cam_ofs[PITCH] = viewangles[PITCH];
		cam_ofs[2] = CAM_MIN_DIST;
	}

	gEngfuncs.Cvar_SetValue("cam_command", 0);
}

void CAM_ToFirstPerson(void)
{
	cam_thirdperson = 0;

	gEngfuncs.Cvar_SetValue("cam_command", 0);
}

void CAM_Think2(void)
{
	Vector origin;
	Vector ext, pnt, camForward, camRight, camUp;
	float dist;
	Vector camAngles;
	float flSensitivity;
	Vector viewangles;

	switch ((int)cam_command->value)
	{
	case CAM_COMMAND_TOTHIRDPERSON:
		CAM_ToThirdPerson();
		break;

	case CAM_COMMAND_TOFIRSTPERSON:
		CAM_ToFirstPerson();
		break;

	case CAM_COMMAND_NONE:
	default:
		break;
	}

	if (!cam_thirdperson)
		return;

	camAngles[PITCH] = cam_idealpitch->value;
	camAngles[YAW] = cam_idealyaw->value;
	dist = cam_idealdist->value;
	//
	//movement of the camera with the mouse
	//
	if (cam_mousemove)
	{
		//get windows cursor position
		GetCursorPos (&cam_mouse);
		//check for X delta values and adjust accordingly
		//eventually adjust YAW based on amount of movement
		//don't do any movement of the cam using YAW/PITCH if we are zooming in/out the camera	
		if (!cam_distancemove)
		{

			//keep the camera within certain limits around the player (ie avoid certain bad viewing angles)  
			if (cam_mouse.x > gEngfuncs.GetWindowCenterX())
			{
				//if ((camAngles[YAW]>=225.0)||(camAngles[YAW]<135.0))
				if (camAngles[YAW] < c_maxyaw->value)
				{
					camAngles[YAW] += (CAM_ANGLE_MOVE) * ((cam_mouse.x - gEngfuncs.GetWindowCenterX()) / 2);
				}
				if (camAngles[YAW] > c_maxyaw->value)
				{
					camAngles[YAW] = c_maxyaw->value;
				}
			}
			else if (cam_mouse.x < gEngfuncs.GetWindowCenterX())
			{
				//if ((camAngles[YAW]<=135.0)||(camAngles[YAW]>225.0))
				if (camAngles[YAW] > c_minyaw->value)
				{
					camAngles[YAW] -= (CAM_ANGLE_MOVE) * ((gEngfuncs.GetWindowCenterX() - cam_mouse.x) / 2.0f);
				}
				if (camAngles[YAW] < c_minyaw->value)
				{
					camAngles[YAW] = c_minyaw->value;
				}
			}

			//check for y delta values and adjust accordingly
			//eventually adjust PITCH based on amount of movement
			//also make sure camera is within bounds
			if (cam_mouse.y > gEngfuncs.GetWindowCenterY())
			{
				if (camAngles[PITCH] < c_maxpitch->value)
				{
					camAngles[PITCH] += (CAM_ANGLE_MOVE) * ((cam_mouse.y - gEngfuncs.GetWindowCenterY()) / 2);
				}
				if (camAngles[PITCH] > c_maxpitch->value)
				{
					camAngles[PITCH] = c_maxpitch->value;
				}
			}
			else if (cam_mouse.y < gEngfuncs.GetWindowCenterY())
			{
				if (camAngles[PITCH] > c_minpitch->value)
				{
					camAngles[PITCH] -= (CAM_ANGLE_MOVE) * ((gEngfuncs.GetWindowCenterY() - cam_mouse.y) / 2);
				}
				if (camAngles[PITCH] < c_minpitch->value)
				{
					camAngles[PITCH] = c_minpitch->value;
				}
			}

			//set old mouse coordinates to current mouse coordinates
			//since we are done with the mouse
			if ((flSensitivity = gHUD::GetSensitivity()) != 0)
			{
				cam_old_mouse_x = cam_mouse.x * flSensitivity;
				cam_old_mouse_y = cam_mouse.y * flSensitivity;
			}
			else
			{
				cam_old_mouse_x = cam_mouse.x;
				cam_old_mouse_y = cam_mouse.y;
			}
			SetCursorPos (gEngfuncs.GetWindowCenterX(), gEngfuncs.GetWindowCenterY());
		}
	}

	//Nathan code here
	if (CL_KeyState(&cam_pitchup))
		camAngles[PITCH] += CAM_ANGLE_DELTA;
	else if (CL_KeyState(&cam_pitchdown))
		camAngles[PITCH] -= CAM_ANGLE_DELTA;

	if (CL_KeyState(&cam_yawleft))
		camAngles[YAW] -= CAM_ANGLE_DELTA;
	else if (CL_KeyState(&cam_yawright))
		camAngles[YAW] += CAM_ANGLE_DELTA;

	if (CL_KeyState(&cam_in))
	{
		dist -= CAM_DIST_DELTA;
		if (dist < CAM_MIN_DIST)
		{
			// If we go back into first person, reset the angle
			camAngles[PITCH] = 0;
			camAngles[YAW] = 0;
			dist = CAM_MIN_DIST;
		}

	}
	else if (CL_KeyState(&cam_out))
		dist += CAM_DIST_DELTA;

	if (cam_distancemove)
	{
		if (cam_mouse.y > gEngfuncs.GetWindowCenterY())
		{
			if (dist < c_maxdistance->value)
			{
				dist += CAM_DIST_DELTA * ((cam_mouse.y - gEngfuncs.GetWindowCenterY()) / 2);
			}
			if (dist > c_maxdistance->value)
			{
				dist = c_maxdistance->value;
			}
		}
		else if (cam_mouse.y < gEngfuncs.GetWindowCenterY())
		{
			if (dist > c_mindistance->value)
			{
				dist -= (CAM_DIST_DELTA) * ((gEngfuncs.GetWindowCenterY() - cam_mouse.y) / 2);
			}
			if (dist < c_mindistance->value)
			{
				dist = c_mindistance->value;
			}
		}
		//set old mouse coordinates to current mouse coordinates
		//since we are done with the mouse
		cam_old_mouse_x = cam_mouse.x * gHUD::GetSensitivity();
		cam_old_mouse_y = cam_mouse.y * gHUD::GetSensitivity();
		SetCursorPos (gEngfuncs.GetWindowCenterX(), gEngfuncs.GetWindowCenterY());
	}

	// update ideal
	cam_idealpitch->value = camAngles[PITCH];
	cam_idealyaw->value = camAngles[YAW];
	cam_idealdist->value = dist;

	// Move towards ideal
	VectorCopy(cam_ofs, camAngles);

	gEngfuncs.GetViewAngles((float*)viewangles);

	if (cam_snapto->value)
	{
		camAngles[YAW] = cam_idealyaw->value + viewangles[YAW];
		camAngles[PITCH] = cam_idealpitch->value + viewangles[PITCH];
		camAngles[2] = cam_idealdist->value;
	}
	else
	{
		if (camAngles[YAW] - viewangles[YAW] != cam_idealyaw->value)
			camAngles[YAW] = MoveToward(camAngles[YAW], cam_idealyaw->value + viewangles[YAW], CAM_ANGLE_SPEED);

		if (camAngles[PITCH] - viewangles[PITCH] != cam_idealpitch->value)
			camAngles[PITCH] = MoveToward(camAngles[PITCH], cam_idealpitch->value + viewangles[PITCH], CAM_ANGLE_SPEED);

		if (fabs(camAngles[2] - cam_idealdist->value) < 2.0)
			camAngles[2] = cam_idealdist->value;
		else
			camAngles[2] += (cam_idealdist->value - camAngles[2]) / 4.0;
	}
	cam_ofs[0] = camAngles[0];
	cam_ofs[1] = camAngles[1];
	cam_ofs[2] = dist;
}

void CL_CameraOffset2(float* ofs)
{
	VectorCopy(cam_ofs, ofs);
}

void CAM_PitchUpDown(void) { KeyDown(&cam_pitchup); }
void CAM_PitchUpUp(void) { KeyUp(&cam_pitchup); }
void CAM_PitchDownDown(void) { KeyDown(&cam_pitchdown); }
void CAM_PitchDownUp(void) { KeyUp(&cam_pitchdown); }
void CAM_YawLeftDown(void) { KeyDown(&cam_yawleft); }
void CAM_YawLeftUp(void) { KeyUp(&cam_yawleft); }
void CAM_YawRightDown(void) { KeyDown(&cam_yawright); }
void CAM_YawRightUp(void) { KeyUp(&cam_yawright); }
void CAM_InDown(void) { KeyDown(&cam_in); }
void CAM_InUp(void) { KeyUp(&cam_in); }
void CAM_OutDown(void) { KeyDown(&cam_out); }
void CAM_OutUp(void) { KeyUp(&cam_out); }

void CAM_StartMouseMove(void)
{
	float flSensitivity;

	//only move the cam with mouse if we are in third person.
	if (cam_thirdperson)
	{
		//set appropriate flags and initialize the old mouse position
		//variables for mouse camera movement
		if (!cam_mousemove)
		{
			cam_mousemove = 1;
			iMouseInUse = 1;
			GetCursorPos (&cam_mouse);

			if ((flSensitivity = gHUD::GetSensitivity()) != 0)
			{
				cam_old_mouse_x = cam_mouse.x * flSensitivity;
				cam_old_mouse_y = cam_mouse.y * flSensitivity;
			}
			else
			{
				cam_old_mouse_x = cam_mouse.x;
				cam_old_mouse_y = cam_mouse.y;
			}
		}
	}
	//we are not in 3rd person view..therefore do not allow camera movement
	else
	{
		cam_mousemove = 0;
		iMouseInUse = 0;
	}
}

//the key has been released for camera movement
//tell the engine that mouse camera movement is off
void CAM_EndMouseMove(void)
{
	cam_mousemove = 0;
	iMouseInUse = 0;
}

//----------------------------------------------------------
//routines to start the process of moving the cam in or out 
//using the mouse
//----------------------------------------------------------
void CAM_StartDistance(void)
{
	//only move the cam with mouse if we are in third person.
	if (cam_thirdperson)
	{
		//set appropriate flags and initialize the old mouse position
		//variables for mouse camera movement
		if (!cam_distancemove)
		{
			cam_distancemove = 1;
			cam_mousemove = 1;
			iMouseInUse = 1;
			GetCursorPos (&cam_mouse);

			cam_old_mouse_x = cam_mouse.x * gHUD::GetSensitivity();
			cam_old_mouse_y = cam_mouse.y * gHUD::GetSensitivity();
		}
	}
	//we are not in 3rd person view..therefore do not allow camera movement
	else
	{
		cam_distancemove = 0;
		cam_mousemove = 0;
		iMouseInUse = 0;
	}
}

//the key has been released for camera movement
//tell the engine that mouse camera movement is off
void CAM_EndDistance(void)
{
	cam_distancemove = 0;
	cam_mousemove = 0;
	iMouseInUse = 0;
}

void CAM_ToggleSnapto(void)
{
	cam_snapto->value = !cam_snapto->value;
}

void CAM_Init(void)
{
	gEngfuncs.pfnAddCommand("+campitchup", CAM_PitchUpDown);
	gEngfuncs.pfnAddCommand("-campitchup", CAM_PitchUpUp);
	gEngfuncs.pfnAddCommand("+campitchdown", CAM_PitchDownDown);
	gEngfuncs.pfnAddCommand("-campitchdown", CAM_PitchDownUp);
	gEngfuncs.pfnAddCommand("+camyawleft", CAM_YawLeftDown);
	gEngfuncs.pfnAddCommand("-camyawleft", CAM_YawLeftUp);
	gEngfuncs.pfnAddCommand("+camyawright", CAM_YawRightDown);
	gEngfuncs.pfnAddCommand("-camyawright", CAM_YawRightUp);
	gEngfuncs.pfnAddCommand("+camin", CAM_InDown);
	gEngfuncs.pfnAddCommand("-camin", CAM_InUp);
	gEngfuncs.pfnAddCommand("+camout", CAM_OutDown);
	gEngfuncs.pfnAddCommand("-camout", CAM_OutUp);
	gEngfuncs.pfnAddCommand("thirdperson", CAM_ToThirdPerson);
	gEngfuncs.pfnAddCommand("firstperson", CAM_ToFirstPerson);
	gEngfuncs.pfnAddCommand("+cammousemove", CAM_StartMouseMove);
	gEngfuncs.pfnAddCommand("-cammousemove", CAM_EndMouseMove);
	gEngfuncs.pfnAddCommand("+camdistance", CAM_StartDistance);
	gEngfuncs.pfnAddCommand("-camdistance", CAM_EndDistance);
	gEngfuncs.pfnAddCommand("snapto", CAM_ToggleSnapto);

	cam_command = gEngfuncs.pfnRegisterVariable ("cam_command", "0", 0);	 // tells camera to go to thirdperson
	cam_snapto = gEngfuncs.pfnRegisterVariable ("cam_snapto", "0", 0);	 // snap to thirdperson view
	cam_idealyaw = gEngfuncs.pfnRegisterVariable ("cam_idealyaw", "90", 0);	 // thirdperson yaw
	cam_idealpitch = gEngfuncs.pfnRegisterVariable ("cam_idealpitch", "0", 0);	 // thirperson pitch
	cam_idealdist = gEngfuncs.pfnRegisterVariable ("cam_idealdist", "64", 0);	 // thirdperson distance
	cam_contain = gEngfuncs.pfnRegisterVariable ("cam_contain", "0", 0);	// contain camera to world

	c_maxpitch = gEngfuncs.pfnRegisterVariable ("c_maxpitch", "90.0", 0);
	c_minpitch = gEngfuncs.pfnRegisterVariable ("c_minpitch", "0.0", 0);
	c_maxyaw = gEngfuncs.pfnRegisterVariable ("c_maxyaw", "135.0", 0);
	c_minyaw = gEngfuncs.pfnRegisterVariable ("c_minyaw", "-135.0", 0);
	c_maxdistance = gEngfuncs.pfnRegisterVariable ("c_maxdistance", "200.0", 0);
	c_mindistance = gEngfuncs.pfnRegisterVariable ("c_mindistance", "30.0", 0);
}
