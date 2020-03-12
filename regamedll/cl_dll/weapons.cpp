/*

Created Date: Mar 12 2020

*/

#include "cl_base.h"

int g_runfuncs = 0;

// Name says it!
// Override stupid Xash(or even GoldSrc?) bug with overwriting
// already predicted values, like maxspeed or punchangle vector
#define _CS16CLIENT_TAKE_PREDICTED_INFO_FOR_WEAPON_PREDICTION

#ifdef _CS16CLIENT_TAKE_PREDICTED_INFO_FOR_WEAPON_PREDICTION
#define STATE to
#else
#define STATE from
#endif


/*
=====================
HUD_PostRunCmd

Client calls this during prediction, after it has moved the player and updated any info changed into to->
time is the current client clock based on prediction
cmd is the command that caused the movement, etc
runfuncs is 1 if this is the first time we've predicted this command.  If so, sounds and effects should play, otherwise, they should
be ignored
=====================
*/
void HUD_PostRunCmd2(local_state_t* from, local_state_t* to, usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
{
	g_runfuncs = runfuncs;

	//HUD_WeaponsPostThink(from, to, cmd, time, random_seed);
	to->client.fov = g_lastFOV;

	if (runfuncs)
	{
		CounterStrike_SetSequence(to->playerstate.sequence, to->playerstate.gaitsequence);
		CounterStrike_SetOrientation(to->playerstate.origin, cmd->viewangles);
	}
}
