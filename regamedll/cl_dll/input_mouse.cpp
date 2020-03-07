/*

Created Date: 06 Mar 2020

*/

#include "cl_base.h"
#include "../external/SDL2/SDL_mouse.h"

globalvars_t* gpGlobals;	// MOVEME: hl_weapons.cpp

int	g_iVisibleMouse = 0;

int			mouse_buttons;
int			mouse_oldbuttonstate;
POINT		current_pos;
POINT		old_mouse_pos;
int			old_mouse_x, old_mouse_y, mx_accum, my_accum;
float		mouse_x, mouse_y;
long		s_mouseDeltaX = 0, s_mouseDeltaY = 0;

static int		restore_spi;
static int		originalmouseparms[3], newmouseparms[3] = { 0, 0, 1 };
static int		mouseactive = 0;
int				mouseinitialized;
static int		mouseparmsvalid;
static int		mouseshowtoggle = 1;
static double	s_flRawInputUpdateTime = 0.0;
static bool		m_bRawInput = false;
static bool		m_bMouseThread = false;

// mouse variables
cvar_t* m_filter;
cvar_t* sensitivity;

// Custom mouse acceleration (0 disable, 1 to enable, 2 enable with separate yaw/pitch rescale)
static cvar_t* m_customaccel;
//Formula: mousesensitivity = ( rawmousedelta^m_customaccel_exponent ) * m_customaccel_scale + sensitivity
// If mode is 2, then x and y sensitivity are scaled by m_pitch and m_yaw respectively.
// Custom mouse acceleration value.
static cvar_t* m_customaccel_scale;
//Max mouse move scale factor, 0 for no limit
static cvar_t* m_customaccel_max;
//Mouse move is raised to this power before being scaled by scale factor
static cvar_t* m_customaccel_exponent;
// if threaded mouse is enabled then the time to sleep between polls
static cvar_t* m_mousethread_sleep;

/*
===========
Pitch Drift
===========
*/

struct pitchdrift_s
{
	float		pitchvel;
	int			nodrift;
	float		driftmove;
	double		laststop;
} pd;

void V_StartPitchDrift(void)
{
	if (pd.laststop == gEngfuncs.GetClientTime())
	{
		return;		// something else is keeping it from drifting
	}

	if (pd.nodrift || !pd.pitchvel)
	{
		pd.pitchvel = v_centerspeed->value;
		pd.nodrift = 0;
		pd.driftmove = 0;
	}
}

void V_StopPitchDrift (void)
{
	pd.laststop = gEngfuncs.GetClientTime();
	pd.nodrift = 1;
	pd.pitchvel = 0;
}

/*
===========
ThreadInterlockedExchange
===========
*/
long ThreadInterlockedExchange(long* pDest, long value)
{
	return InterlockedExchange(pDest, value);
}

//-----------------------------------------------------------------------------
// Purpose: Allows modulation of mouse scaling/senstivity value and application
//  of custom algorithms.
// Input  : *x - 
//			*y - 
//-----------------------------------------------------------------------------
void IN_ScaleMouse(float* x, float* y)
{
	float mx = *x;
	float my = *y;

	// This is the default sensitivity
	// UNDONE
	float mouse_senstivity = /*(gHUD.GetSensitivity() != 0) ? gHUD.GetSensitivity() : */sensitivity->value;

	// Using special accleration values
	if (m_customaccel->value != 0)
	{
		float raw_mouse_movement_distance = sqrt(mx * mx + my * my);
		float acceleration_scale = m_customaccel_scale->value;
		float accelerated_sensitivity_max = m_customaccel_max->value;
		float accelerated_sensitivity_exponent = m_customaccel_exponent->value;
		float accelerated_sensitivity = ((float)pow(raw_mouse_movement_distance, accelerated_sensitivity_exponent) * acceleration_scale + mouse_senstivity);

		if (accelerated_sensitivity_max > 0.0001f &&
			accelerated_sensitivity > accelerated_sensitivity_max)
		{
			accelerated_sensitivity = accelerated_sensitivity_max;
		}

		*x *= accelerated_sensitivity;
		*y *= accelerated_sensitivity;

		// Further re-scale by yaw and pitch magnitude if user requests alternate mode 2
		// This means that they will need to up their value for m_customaccel_scale greatly (>40x) since m_pitch/yaw default
		//  to 0.022
		if (m_customaccel->value == 2)
		{
			*x *= m_yaw->value;
			*y *= m_pitch->value;
		}
	}
	else
	{
		// Just apply the default
		*x *= mouse_senstivity;
		*y *= mouse_senstivity;
	}
}

/*
===========
IN_ResetMouse
===========
*/
void IN_ResetMouse(void)
{
	// no work to do in SDL

	if (!m_bRawInput && mouseactive && gEngfuncs.GetWindowCenterX && gEngfuncs.GetWindowCenterY)
	{

		SetCursorPos (gEngfuncs.GetWindowCenterX(), gEngfuncs.GetWindowCenterY());
		ThreadInterlockedExchange(&old_mouse_pos.x, gEngfuncs.GetWindowCenterX());
		ThreadInterlockedExchange(&old_mouse_pos.y, gEngfuncs.GetWindowCenterY());
	}

	if (gpGlobals && gpGlobals->time - s_flRawInputUpdateTime > 1.0f)
	{
		s_flRawInputUpdateTime = gpGlobals->time;
		m_bRawInput = CVAR_GET_FLOAT("m_rawinput") != 0;
	}
}

/*
===========
IN_MouseMove
===========
*/
void IN_MouseMove (float frametime, usercmd_t* cmd)
{
	int		mx, my;
	Vector viewangles;

	gEngfuncs.GetViewAngles(viewangles);

	if (in_mlook.state & 1)
	{
		V_StopPitchDrift();
	}

	//jjb - this disbles normal mouse control if the user is trying to 
	//      move the camera, or if the mouse cursor is visible or if we're in intermission
	// UNDONE
	if (!iMouseInUse /*&& !gHUD.m_iIntermission*/ && !g_iVisibleMouse)
	{
		int deltaX, deltaY;
		if (!m_bRawInput)
		{
			if (m_bMouseThread)
			{
				ThreadInterlockedExchange(&current_pos.x, s_mouseDeltaX);
				ThreadInterlockedExchange(&current_pos.y, s_mouseDeltaY);
				ThreadInterlockedExchange(&s_mouseDeltaX, 0);
				ThreadInterlockedExchange(&s_mouseDeltaY, 0);
			}
			else
			{
				GetCursorPos (&current_pos);
			}
		}
		else
		{
			SDL_GetRelativeMouseState(&deltaX, &deltaY);
			current_pos.x = deltaX;
			current_pos.y = deltaY;
		}

		if (!m_bRawInput)
		{
			if (m_bMouseThread)
			{
				mx = current_pos.x;
				my = current_pos.y;
			}
			else
			{
				mx = current_pos.x - gEngfuncs.GetWindowCenterX() + mx_accum;
				my = current_pos.y - gEngfuncs.GetWindowCenterY() + my_accum;
			}
		}
		else
		{
			mx = deltaX + mx_accum;
			my = deltaY + my_accum;
		}

		mx_accum = 0;
		my_accum = 0;

		if (m_filter && m_filter->value)
		{
			mouse_x = (mx + old_mouse_x) * 0.5;
			mouse_y = (my + old_mouse_y) * 0.5;
		}
		else
		{
			mouse_x = mx;
			mouse_y = my;
		}

		old_mouse_x = mx;
		old_mouse_y = my;

		// Apply custom mouse scaling/acceleration
		IN_ScaleMouse(&mouse_x, &mouse_y);

		// add mouse X/Y movement to cmd
		if ((in_strafe.state & 1) || (lookstrafe->value && (in_mlook.state & 1)))
			cmd->sidemove += m_side->value * mouse_x;
		else
			viewangles[YAW] -= m_yaw->value * mouse_x;

		if ((in_mlook.state & 1) && !(in_strafe.state & 1))
		{
			viewangles[PITCH] += m_pitch->value * mouse_y;
			if (viewangles[PITCH] > cl_pitchdown->value)
				viewangles[PITCH] = cl_pitchdown->value;
			if (viewangles[PITCH] < -cl_pitchup->value)
				viewangles[PITCH] = -cl_pitchup->value;
		}
		else
		{
			if ((in_strafe.state & 1) && gEngfuncs.IsNoClipping())
			{
				cmd->upmove -= m_forward->value * mouse_y;
			}
			else
			{
				cmd->forwardmove -= m_forward->value * mouse_y;
			}
		}

		// if the mouse has moved, force it to the center, so there's room to move
		if (mx || my)
		{
			IN_ResetMouse();
		}
	}

	gEngfuncs.SetViewAngles((float*)viewangles);

	/*
	//#define TRACE_TEST
	#if defined( TRACE_TEST )
		{
			int mx, my;
			void V_Move( int mx, int my );
			IN_GetMousePos( &mx, &my );
			V_Move( mx, my );
		}
	#endif
	*/
}

/*
===========
IN_Move
===========
*/
void IN_Move(float frametime, usercmd_t* cmd)
{
	if (!iMouseInUse && mouseactive)
	{
		IN_MouseMove(frametime, cmd);
	}

	// LUNA: joystick was removed.
}

/*
===========
IN_ActivateMouse
===========
*/
void IN_ActivateMouse2(void)
{
	if (mouseinitialized)
	{
		if (mouseparmsvalid)
			restore_spi = SystemParametersInfo (SPI_SETMOUSE, 0, newmouseparms, 0);

		mouseactive = 1;
	}
}

/*
===========
IN_DeactivateMouse
===========
*/
void IN_DeactivateMouse2(void)
{
	if (mouseinitialized)
	{
		if (restore_spi)
			SystemParametersInfo (SPI_SETMOUSE, 0, originalmouseparms, 0);

		mouseactive = 0;
	}
}

/*
===========
IN_MouseEvent
===========
*/
void IN_MouseEvent2(int mstate)
{
	int i;

	if (iMouseInUse || g_iVisibleMouse)
		return;

	// perform button actions
	for (i = 0; i < mouse_buttons; i++)
	{
		if ((mstate & (1 << i)) &&
			!(mouse_oldbuttonstate & (1 << i)))
		{
			gEngfuncs.Key_Event(K_MOUSE1 + i, 1);
		}

		if (!(mstate & (1 << i)) &&
			(mouse_oldbuttonstate & (1 << i)))
		{
			gEngfuncs.Key_Event(K_MOUSE1 + i, 0);
		}
	}

	mouse_oldbuttonstate = mstate;
}

/*
===========
IN_Accumulate
===========
*/
void IN_Accumulate2(void)
{
	//only accumulate mouse if we are not moving the camera with the mouse
	if (!iMouseInUse && !g_iVisibleMouse)
	{
		if (mouseactive)
		{
			if (!m_bRawInput)
			{
				if (!m_bMouseThread)
				{
					GetCursorPos (&current_pos);

					mx_accum += current_pos.x - gEngfuncs.GetWindowCenterX();
					my_accum += current_pos.y - gEngfuncs.GetWindowCenterY();
				}
			}
			else
			{
				int deltaX, deltaY;
				SDL_GetRelativeMouseState(&deltaX, &deltaY);
				mx_accum += deltaX;
				my_accum += deltaY;
			}
			// force the mouse to the center, so there's room to move
			IN_ResetMouse();

		}
	}

}

/*
===================
IN_ClearStates
===================
*/
void IN_ClearStates2(void)
{
	if (!mouseactive)
		return;

	mx_accum = 0;
	my_accum = 0;
	mouse_oldbuttonstate = 0;
}
