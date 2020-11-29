/*

Created Date: 05 Mar 2020

*/

#pragma once

// originally it should be __declspec(dllexport)
// however, it's nothing more than a marker now.
#define CL_DLLEXPORT

// interface def. used in Initialize()
#define CLDLL_INTERFACE_VERSION	7

// core of cores
extern cl_enginefunc_t gEngfuncs;
extern cl_extendedfunc_t gExtFuncs;

// function list.
BOOL CL_DLLEXPORT CL_IsThirdPerson(void);
void CL_DLLEXPORT IN_ActivateMouse(void);
void CL_DLLEXPORT IN_DeactivateMouse(void);
kbutton_s CL_DLLEXPORT* KB_Find(const char* name);
