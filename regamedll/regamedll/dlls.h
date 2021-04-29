/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "extdll.h"


#define VFUNC virtual


using VectorRef = Vector;
using FloatRef = float;

#include "archtypes.h"
#include "enginecallback.h"
#include "../common/entity_state.h"
#include "../common/screenfade.h"
#include "utlvector.h"

// SDK
#include "../common/usercmd.h"
#include "../common/weaponinfo.h"
#include "customentity.h"

// Globals vars
#include "globals.h"
#include "saverestore.h"
#include "game_shared/shared_string.h"

// GameInit
#include "game.h"

// PM
#include "pm_shared/pm_defs.h"
#include "pm_shared/pm_materials.h"
#include "pm_shared/pm_movevars.h"
#include "pm_shared/pm_debug.h"
#include "pm_shared/pm_shared.h"
#include "pm_shared/pm_math.h"

#include "decals.h"
#include "../common/hltv.h"
#include "cbase.h"

#include "sound.h"
#include "basemonster.h"
#include "monsters.h"
#include "unisignals.h"
#include "maprules.h"
#include "mortar.h"
#include "effects.h"
#include "lights.h"
#include "healthkit.h"
#include "vehicle.h"
#include "func_tank.h"
#include "func_break.h"
#include "doors.h"
#include "bmodels.h"

#include "debug.h"
#include "observer.h"
#include "client.h"

#include "trains.h"
#include "pathcorner.h"
#include "plats.h"
#include "buttons.h"

// CSBOT and Nav
#include "game_shared/GameEvent.h"		// Game event enum used by career mode, tutor system, and bots
#include "game_shared/bot/bot_util.h"
#include "game_shared/bot/simple_state_machine.h"

#include "game_shared/steam_util.h"
#include "game_shared/counter.h"

#include "game_shared/bot/bot_manager.h"
#include "game_shared/bot/bot_constants.h"
#include "game_shared/bot/bot.h"

#include "game_shared/shared_util.h"
#include "game_shared/bot/bot_profile.h"

#include "game_shared/bot/nav.h"
#include "game_shared/bot/improv.h"
#include "game_shared/bot/nav_node.h"
#include "game_shared/bot/nav_area.h"
#include "game_shared/bot/nav_file.h"
#include "game_shared/bot/nav_path.h"

#include "airtank.h"
#include "h_cycler.h"
#include "h_battery.h"

#include "bot/cs_bot.h"

// Addons
#include "addons/trigger_setorigin.h"
#include "addons/trigger_random.h"
#include "addons/point_command.h"
#include "metahook/portable/metahook.h"

#include "revert_saved.h"
#include "triggers.h"
#include "subs.h"
#include "doors.h"
#include "explode.h"
#include "gamerules.h"
#include "mapinfo.h"
#include "maprules.h"
#include "monsterevent.h"
#include "spectator.h"
#include "talkmonster.h"
#include "animation.h"

#include "../common/com_model.h"
#include "studio.h"
#include "../common/r_studioint.h"

#include "shake.h"
