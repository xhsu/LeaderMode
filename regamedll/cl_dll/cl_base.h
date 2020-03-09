/*

Created Date: 05 Mar 2020

*/

#pragma once

#include "../public/basetypes.h"
#include "../engine/maintypes.h"
#include "../engine/archtypes.h"
#include "../public/strtools.h"
#include "../dlls/vector.h"

// const, OH, why you are const!
#include "../common/const.h"

// offers cl_enginefunc_t
#include "../engine/APIProxy.h"

// offers efx_api_s
#include "../common/r_efx.h"

// offers event_api_s
#include "../common/event_api.h"

// offers triangleapi_s
#include "../common/triangleapi.h"

// offers demo_api_s
#include "../common/demo_api.h"

// offers cl_entity_s
#include "../common/cl_entity.h"
#include "../common/entity_types.h"

// PM, of course.
#include "../pm_shared/pm_shared.h"
#include "../pm_shared/pm_defs.h"
#include "../pm_shared/pm_math.h"
#include "../pm_shared/pm_materials.h"
#include "../pm_shared/pm_debug.h"
#include "../pm_shared/pm_movevars.h"

// for POINT_CONTENTS return values
#include "../engine/bspfile.h"

// offers weapon_data_s
#include "../common/weaponinfo.h"

// offers mstudioevent_s
#include "../common/studio_event.h"

// offers pmtrace_s
#include "../common/pmtrace.h"

// for some const from ReGameDLL-CS
#include "../dlls/cdll_dll.h"

// for models & hulls
#include "../common/com_model.h"

// for ref_params_s
#include "../common/ref_params.h"

// for cvar_t
#include "../common/cvardef.h"

//for engine_studio_api_t
#include "../common/r_studioint.h"

// for keybinds
#include "../public/keydefs.h"

// for mstudioanim_t
#include "../engine/studio.h"

// for msg reading.
#include "../common/parsemsg.h"

// custom header starts from here.
#include "cdll_int.h"
#include "entity.h"
#include "cl_util.h"
#include "view.h"
#include "input.h"
#include "input_mouse.h"
#include "in_camera.h"
#include "StudioModelRenderer.h"
#include "message.h"
#include "events.h"

// hud
#include "hud/hud.h"
