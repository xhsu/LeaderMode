/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#pragma once

constexpr int MAX_TEXTURES           = 1024; // max number of textures loaded
constexpr int MAX_TEXTURENAME_LENGHT = 17;   // only load first n chars of name

// Texture types
constexpr char CHAR_TEX_CONCRETE = 'C'; // Cinder block
constexpr char CHAR_TEX_METAL    = 'M'; // Metal
constexpr char CHAR_TEX_DIRT     = 'D';
constexpr char CHAR_TEX_VENT     = 'V';
constexpr char CHAR_TEX_GRATE    = 'G';
constexpr char CHAR_TEX_TILE     = 'T'; // Ceiling tile
constexpr char CHAR_TEX_SLOSH    = 'S';
constexpr char CHAR_TEX_WOOD     = 'W';
constexpr char CHAR_TEX_COMPUTER = 'P';
constexpr char CHAR_TEX_GRASS    = 'X';
constexpr char CHAR_TEX_GLASS    = 'Y';
constexpr char CHAR_TEX_FLESH    = 'F';
constexpr char CHAR_TEX_SNOW     = 'N';
