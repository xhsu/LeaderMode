/*

Created Date: 05 Mar 2020

*/

#include "cl_base.h"

cl_enginefunc_t gEngfuncs;

void CL_DLLEXPORT CAM_Think(void)
{
}

void CL_DLLEXPORT CL_CameraOffset(float* ofs)
{
}

void CL_DLLEXPORT CL_CreateMove (float frametime, struct usercmd_s* cmd, int active)
{
}

BOOL CL_DLLEXPORT CL_IsThirdPerson(void)
{
	return FALSE;
}

void* CL_DLLEXPORT ClientFactory(void)
{
	return nullptr;
}

void CL_DLLEXPORT Demo_ReadBuffer(int size, unsigned char* buffer)
{
}

BOOL CL_DLLEXPORT HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname)
{
	return TRUE;
}

void CL_DLLEXPORT HUD_ChatInputPosition(int* x, int* y)
{
}

BOOL CL_DLLEXPORT HUD_ConnectionlessPacket(const struct netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size)
{
	return FALSE;
}

void CL_DLLEXPORT HUD_CreateEntities(void)
{
}

void CL_DLLEXPORT HUD_DirectorMessage(int iSize, void* pbuf)
{
}

void CL_DLLEXPORT HUD_DrawNormalTriangles(void)
{
}

void CL_DLLEXPORT HUD_DrawTransparentTriangles(void)
{
}

void CL_DLLEXPORT HUD_Frame(double time)
{
}

BOOL CL_DLLEXPORT HUD_GetHullBounds(int hullnumber, float* mins, float* maxs)
{
	BOOL FReturn = FALSE;

	switch (hullnumber)
	{
	case 0:				// Normal player
		mins = Vector(-16, -16, -36);
		maxs = Vector(16, 16, 36);
		FReturn = TRUE;
		break;

	case 1:				// Crouched player
		mins = Vector(-16, -16, -18);
		maxs = Vector(16, 16, 18);
		FReturn = TRUE;
		break;

	case 2:				// Point based hull
		mins = Vector(0, 0, 0);
		maxs = Vector(0, 0, 0);
		FReturn = TRUE;
		break;
	}

	return FReturn;
}

int CL_DLLEXPORT HUD_GetPlayerTeam(int iParam)	// useless & unknown.
{
	return 0;
}

BOOL CL_DLLEXPORT HUD_GetStudioModelInterface(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio)
{
	return FALSE;
}

cl_entity_t CL_DLLEXPORT* HUD_GetUserEntity(int index)
{
	return nullptr;
}

void CL_DLLEXPORT HUD_Init(void)
{
}

BOOL CL_DLLEXPORT HUD_Key_Event(int down, int keynum, const char* pszCurrentBinding)
{
	return TRUE;
}

void CL_DLLEXPORT HUD_PlayerMove(struct playermove_s* ppmove, int server)
{
}

void CL_DLLEXPORT HUD_PlayerMoveInit(struct playermove_s* ppmove)
{
}

char CL_DLLEXPORT HUD_PlayerMoveTexture(char* name)
{
	return '\0';
}

void CL_DLLEXPORT HUD_PostRunCmd(struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
{
}

void CL_DLLEXPORT HUD_ProcessPlayerState(struct entity_state_s* dst, const struct entity_state_s* src)
{
}

BOOL CL_DLLEXPORT HUD_Redraw(float time, int intermission)
{
	return TRUE;
}

void CL_DLLEXPORT HUD_Reset(void)
{
}

void CL_DLLEXPORT HUD_Shutdown(void)
{
}

void CL_DLLEXPORT HUD_StudioEvent(const struct mstudioevent_s* event, const struct cl_entity_s* entity)
{
}

void CL_DLLEXPORT HUD_TempEntUpdate(double frametime, double client_time, double cl_gravity, struct tempent_s** ppTempEntFree, struct tempent_s** ppTempEntActive, int (*Callback_AddVisibleEntity)(struct cl_entity_s* pEntity), void (*Callback_TempEntPlaySound)(struct tempent_s* pTemp, float damp))
{
}

void CL_DLLEXPORT HUD_TxferLocalOverrides(struct entity_state_s* state, const struct clientdata_s* client)
{
}

void CL_DLLEXPORT HUD_TxferPredictionData(struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd)
{
}

BOOL CL_DLLEXPORT HUD_UpdateClientData(struct client_data_t* pcldata, float flTime)
{
	return FALSE;
}

BOOL CL_DLLEXPORT HUD_VidInit(void)
{
	return TRUE;
}

void CL_DLLEXPORT HUD_VoiceStatus(int entindex, BOOL bTalking)
{
}

void CL_DLLEXPORT IN_Accumulate(void)
{
}

void CL_DLLEXPORT IN_ActivateMouse(void)
{
}

void CL_DLLEXPORT IN_ClearStates(void)
{
}

void CL_DLLEXPORT IN_DeactivateMouse(void)
{
}

void CL_DLLEXPORT IN_MouseEvent(int mstate)
{
}

BOOL CL_DLLEXPORT Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion)
{
	gEngfuncs = *pEnginefuncs;

	if (iVersion != CLDLL_INTERFACE_VERSION)
		return FALSE;

	Q_memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	// LUNA: UNDONE, crsky told me to do it later.
	//EV_HookEvents();
	//CL_LoadParticleMan();

	// get tracker interface, if any
	return TRUE;
}

struct kbutton_s CL_DLLEXPORT* KB_Find(const char* name)
{
	return nullptr;
}

void CL_DLLEXPORT V_CalcRefdef(struct ref_params_s* pparams)
{
}
