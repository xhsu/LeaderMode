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
