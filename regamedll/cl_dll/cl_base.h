/*

Created Date: 05 Mar 2020

*/

#pragma once

#include "../public/basetypes.h"
#include "../engine/maintypes.h"
#include "../engine/archtypes.h"
#include "../tier1/strtools.h"
#include "../dlls/vector.h"

// std stuff. some commonly used vectors.
#include <list>
#include <vector>
#include <array>
#include <string>
#include <random>
#include <iterator>
#include <unordered_map>
#include <chrono>

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
#include "../engine/keydefs.h"

// for mstudioanim_t
#include "../engine/studio.h"

// for msg reading.
#include "../common/parsemsg.h"

// LUNA: why do I even need this?
#include "../common/hltv.h"

// some stuff I don't even know.
#include "../game_shared/shared_util.h"
#include "../game_shared/shared_string.h"

// for ISurface
#include "../public/interface.h"

// for local_state_s
#include "../common/entity_state.h"

// For engine sound structs
#include "../engine/sfx.h"

// For gEngfuncs.Con_NXPrintf() param 1.
#include "../common/con_nprint.h"

// Particleman.dll
#include "../public/particleman.h"

// sync the weapon data between cl and sv.
#include "../dlls/ammo.h"
#include "../dlls/weapontype.h"
#include "../dlls/player_classes.h"

// VGUI2
#include "vgui_controls/pch_vgui_controls.h"

// file system. for the file override and usage of other mods' res.
#include <Interface/IFileSystem.h>

// OpenGL
#include <gl/GL.h>
#include <gl/GLU.h>

// Client Viewport
#include "Viewport.h"

// functions loaded(or extended) from metahook module.
#include "../metahook/engine.h"
#include "../metahook/sound.h"
#include "../metahook/client.h"

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
#include "interpolation.h"	// UTIL from CSBTE.
#include "corpse.h"
#include "viewport_interface.h"
#include "weapons.h"
#include "tri.h"
#include "sound.h"
#include "rain.h"

// graphic stuff
#include "Graph/dxt.h"
#include "Graph/bmp.h"
#include "Graph/tga.h"

// font stuff
#include "Fonts/DrawFonts.h"
#include "Fonts/encode.h"

// UI
#include "UI/ui_panel.h"	// grandfather of every other UI elements.
#include "UI/ui_text.h"
#include "UI/ui_button.h"

// HUD
#include "hud/hud.h"

// bottom-left
#include "hud/hud_radar.h"	// motherboard of left-bottom region.
#include "HUD/hud_class_indicator.h"
#include "hud/hud_battery.h"
#include "hud/hud_health.h"

// top.
#include "HUD/hud_match_status.h"

// top-right
#include "HUD/hud_status_icons.h"
#include "HUD/hud_money.h"
#include "HUD/hud_death_notice.h"

// bottom-right
#include "HUD/hud_equipments.h"
#include "HUD/hud_ammo.h"

#include "hud/hud_spectator.h"
#include "hud/hud_geiger.h"
#include "hud/hud_train.h"
#include "hud/hud_message.h"
#include "hud/hud_status_bar.h"
#include "hud/hud_say_text.h"
#include "hud/hud_menu.h"
#include "hud/hud_nightvision.h"
#include "hud/hud_text_message.h"
#include "hud/hud_round_timer.h"
#include "hud/hud_head_name.h"
#include "hud/hud_progress_bar.h"
#include "hud/hud_vgui2print.h"
#include "hud/hud_sniperscope.h"
#include "hud/hud_crosshair.h"
#include "hud/hud_weaponlist.h"
#include "HUD/hud_scoreboard.h"

// UI Instance
#include "UI/menu_buy.h"
