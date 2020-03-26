/*

Created Date: 06 Mar 2020

*/

#pragma once

#define BAD_ENT_PTR 0xFFFFFFFF

#define	CAM_MODE_RELAX		1
#define CAM_MODE_FOCUS		2

#define ORIGIN_BACKUP 64
#define ORIGIN_MASK ( ORIGIN_BACKUP - 1 )

struct viewinterp_t
{
	Vector Origins[ORIGIN_BACKUP];
	float OriginTime[ORIGIN_BACKUP];

	Vector Angles[ORIGIN_BACKUP];
	float AngleTime[ORIGIN_BACKUP];

	int CurrentOrigin;
	int CurrentAngle;
};

extern cvar_t* v_centermove;
extern cvar_t* v_centerspeed;
extern cvar_t* cl_gun_ofs[3];

extern double g_flGunBobAmplitudeModifier;	// some weapons model is larger than others.
extern double g_flGunBobOmegaModifier;	// some weapons model is larger than others.

extern Vector g_vecGunCurOfs;
extern Vector g_vecGunOfsGoal;
extern Vector g_vecTranslatedCurGunOfs;
extern float  g_flGunOfsMovingSpeed;

void V_Init(void);

void V_CalcIntermissionRefdef(ref_params_s* pparams);
void V_CalcSpectatorRefdef(ref_params_s* pparams);
void V_CalcNormalRefdef(ref_params_s* pparams);
void V_CalcThirdPersonRefdef(ref_params_t* pparams);

// for other cpps
void V_ResetChaseCam(void);
void V_GetInEyePos(int target, float* origin, float* angles);
void V_GetChasePos(int target, float* cl_angles, float* origin, float* angles);
void V_GetDirectedChasePosition(cl_entity_t* ent1, cl_entity_t* ent2, float* angle, float* origin);
void V_GetInEyePos(int target, float* origin, float* angles);
void V_GetChasePos(int target, float* cl_angles, float* origin, float* angles);

// exporting this to make other cpp make use of it.
extern ref_params_s g_pparams;
