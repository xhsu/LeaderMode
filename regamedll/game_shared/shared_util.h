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

#include <type_traits>

#ifndef _WIN32
#include <string.h>
#include <wchar.h>
#endif

#define charsmax(x)		(_countof(x) - 1U)
#define wcharsmax(x)	(_countof(x) - 1U)

wchar_t *SharedWVarArgs(const wchar_t *format, ...);
char *SharedVarArgs(const char *format, ...);
char *BufPrintf(char *buf, int &len, const char *fmt, ...);
wchar_t *BufWPrintf(wchar_t *buf, int &len, const wchar_t *fmt, ...);
const wchar_t *NumAsWString(int val);
const char *NumAsString(int val);
char *SharedGetToken();
void SharedSetQuoteChar(char c);
const char *SharedParse(const char *data);
bool SharedTokenWaiting(const char *buffer);

// Simple utility function to allocate memory and duplicate a string
inline char *CloneString(const char *str)
{
	if (!str)
	{
		char *cloneStr = new char[1];
		cloneStr[0] = '\0';
		return cloneStr;
	}

	char *cloneStr = new char [Q_strlen(str) + 1];
	strcpy(cloneStr, str);	// LUNA: strcpy_s is confirmed not used here.
	return cloneStr;
}

// Simple utility function to allocate memory and duplicate a wide string
inline wchar_t *CloneWString(const wchar_t *str)
{
	if (!str)
	{
		wchar_t *cloneStr = new wchar_t[1];
		cloneStr[0] = L'\0';
		return cloneStr;
	}

	wchar_t *cloneStr = new wchar_t [wcslen(str) + 1];
	wcscpy(cloneStr, str);
	return cloneStr;
}

float UTIL_WeaponTimeBase();
unsigned int U_Random();
void U_Srand(unsigned int seed);
int UTIL_SharedRandomLong(unsigned int seed, int low, int high);
float UTIL_SharedRandomFloat(unsigned int seed, float low, float high);

struct BodyEnumInfo_t
{
	int body;
	int nummodels;
};

int CalcBody(BodyEnumInfo_t* info, int count);
template<size_t N> int CalcBody(BodyEnumInfo_t (&info)[N]);
void NORETURN Sys_Error(const char* fmt, ...);
void Sys_GetRegKeyValueUnderRoot(const char* pszSubKey, const char* pszElement, char* pszReturnString, int nReturnLength, const char* pszDefaultValue);
void Sys_SetRegKeyValueUnderRoot(const char* pszSubKey, const char* pszElement, const char* pszValue);
void Sys_SplitPath(const char* path, char* drive, char* dir, char* fname, char* ext);

#define CREATE_MEMBER_DETECTOR(X)							\
	template <typename T, typename = void>					\
	struct DETECT_##X : std::false_type {};					\
															\
	template <typename T>									\
	struct DETECT_##X<T, std::void_t<decltype(T::X)>>		\
		: std::true_type {}
