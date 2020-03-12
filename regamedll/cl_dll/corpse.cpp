/*

Created Date: Mar 12 2020

*/

#include "cl_base.h"

void RemoveBody(TEMPENTITY* te, float frametime, float current_time)
{
	if (current_time >= te->entity.curstate.fuser2)
		te->entity.origin[2] -= frametime * 5;
}

void HitBody(TEMPENTITY* ent, pmtrace_s* ptr)
{
	if (ptr->plane.normal[2] > 0)
		ent->flags |= FTENT_IGNOREGRAVITY;
}

void CreateCorpse(Vector vOrigin, Vector vAngles, const char* pModel, float flAnimTime, int iSequence, int iBody)
{
	int iModelIndex;
	TEMPENTITY* pBody;

	iModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex(pModel);
	pBody = gEngfuncs.pEfxAPI->R_TempModel(vOrigin, Vector(0, 0, 0), vAngles, 100.0, iModelIndex, 0);

	if (pBody)
	{
		pBody->flags |= FTENT_COLLIDEWORLD | FTENT_FADEOUT | FTENT_SPRANIMATE | FTENT_PERSIST | FTENT_CLIENTCUSTOM | 0x100000;
		pBody->frameMax = 255;
		pBody->entity.curstate.framerate = 1.0;
		pBody->entity.curstate.animtime = flAnimTime;
		pBody->entity.curstate.frame = 0;
		pBody->entity.curstate.sequence = iSequence;
		pBody->entity.curstate.body = iBody;
		pBody->entity.baseline.renderamt = 255;
		pBody->entity.curstate.renderamt = 255;

		pBody->entity.curstate.fuser1 = gHUD::m_flTime + 1;
		pBody->entity.curstate.fuser2 = gHUD::m_flTime + cl_corpsestay->value;

		pBody->hitcallback = HitBody;
		pBody->bounceFactor = 0;

		if (cl_corpsefade->value)
		{
			pBody->die = gEngfuncs.GetClientTime() + cl_corpsestay->value;
		}
		else
		{
			pBody->callback = RemoveBody;
			pBody->die = gEngfuncs.GetClientTime() + cl_corpsestay->value + 9.0;
		}

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (!g_DeadPlayerModels[i])
			{
				g_DeadPlayerModels[i] = pBody;
				break;
			}
		}
	}
}
