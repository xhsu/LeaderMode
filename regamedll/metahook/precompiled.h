/*

Created Date: Mar 22 2020

precompiling header.

Modern Warfare Dev Team
 - Luna the Reborn

*/

#pragma once

#include "../public/basetypes.h"
#include "../engine/maintypes.h"
#include "../engine/archtypes.h"
#include "../public/strtools.h"
#include "../dlls/vector.h"

// interface
#include "interface.h"

// core of metahook (version 0.4)
#include "metahook.h"

// OpenGL
#include <gl/GL.h>
#include <gl/GLU.h>

// dxt
#define FILE_SYSTEM	g_pInterface->FileSystem
#include "../cl_dll/Graph/dxt.h"

// for studiohdr_t and STUDIO_NF_FULLBRIGHT
#include "../engine/studio.h"

// general engine hooks and function prototype definitions
#include "engine.h"

// client extended funcs
#include "client.h"

// specific functions
#include "larger_textures.h"
#include "fullbright_textures.h"
