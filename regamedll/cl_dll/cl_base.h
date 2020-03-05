/*

Created Date: 05 Mar 2020

*/

#pragma once

#include "../public/basetypes.h"
#include "../engine/archtypes.h"
#include "../public/strtools.h"
#include "../dlls/vector.h"
typedef int string_t;

// const, OH, why you are const!
#include "../common/const.h"

// offers cl_enginefunc_t
#include "../engine/APIProxy.h"

// offers efx_api_s
#include "../common/r_efx.h"

// offers event_api_s
#include "../common/event_api.h"

// offers cl_entity_s
#include "../common/cl_entity.h"
#include "../common/entity_types.h"

// PM, of course.
#include "../pm_shared/pm_shared.h"
#include "../pm_shared/pm_defs.h"

// offers weapon_data_s
#include "../common/weaponinfo.h"

// offers mstudioevent_s
#include "../common/studio_event.h"

// offers pmtrace_s
#include "../common/pmtrace.h"

// custom header starts from here.
#include "cdll_int.h"
#include "entity.h"
#include "cl_util.h"

