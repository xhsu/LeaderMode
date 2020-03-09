/*

Created Date: 08 Mar 2020

*/

#pragma once

#define DECLARE_EVENT(x) void EV_##x( event_args_s *args)
#define HOOK_EVENT(x, y) gEngfuncs.pfnHookEvent( "events/" #x ".sc", EV_##y )

void Events_Init(void);
