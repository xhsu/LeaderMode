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

#ifndef TIER1_STRTOOLS_H
#define TIER1_STRTOOLS_H

#include "tier0/platform.h"

#ifdef _WIN32
#pragma once
#elif _LINUX
#include <ctype.h>
#include <wchar.h>
#endif

#include <string.h>
#include <stdlib.h>


#if !defined(_WIN32)
inline char *_strupr(char *start)
{
      char *str = start;
      while (str && *str)
      {
              *str = (char)toupper(*str);
              str++;
      }

      return start;
}

inline char *_strlwr(char *start)
{
      char *str = start;
      while (str && *str)
      {
              *str = (char)tolower(*str);
              str++;
      }

      return start;
}
#endif

#if defined(ASMLIB_H) && defined(HAVE_OPT_STRTOOLS)
	#define Q_memset A_memset
	#define Q_memcpy A_memcpy
	#define Q_memcmp A_memcmp
	#define Q_memmove A_memmove
	#define Q_strlen A_strlen
	#define Q_strcpy A_strcpy
	#define Q_strncpy strncpy
	#define Q_strcat A_strcat
	#define Q_strncat strncat
	#define Q_strcmp A_strcmp
	#define Q_strncmp strncmp
	#define Q_strdup _strdup
	#define Q_stricmp A_stricmp
	#define Q_strnicmp _strnicmp
	#define Q_strstr A_strstr
	#define Q_strchr strchr
	#define Q_strrchr strrchr
	#define Q_strtok strtok
	#define Q_strlwr A_strtolower
	#define Q_strupr A_strtoupper
	#define Q_sprintf sprintf
	#define Q_snprintf _snprintf
	#define Q_vsnprintf _vsnprintf
	#define Q_vsnwprintf _vsnwprintf
	#define Q_atoi atoi
	#define Q_atof atof
	#define Q_sqrt M_sqrt
	#define Q_min M_min
	#define Q_max M_max
	#define Q_clamp M_clamp
	#define Q_abs abs
	#define Q_fabs fabs
	#define Q_tan tan
	#define Q_atan atan
	#define Q_atan2 atan2
	#define Q_acos acos
	#define Q_cos cos
	#define Q_sin sin
	#define Q_pow pow
	#define Q_fmod fmod
#else
	#define Q_memset memset
	#define Q_memcpy memcpy
	#define Q_memcmp memcmp
	#define Q_memmove memmove
	#define Q_strlen strlen
	#define Q_strcpy strcpy_s
	#define Q_strncpy strncpy
	#define Q_strcat strcat_s
	#define Q_strncat strncat
	#define Q_strcmp strcmp
	#define Q_strncmp strncmp
	#define Q_strdup _strdup
	#define Q_stricmp _stricmp
	#define Q_strnicmp _strnicmp
	#define Q_strstr strstr
	#define Q_strchr strchr
	#define Q_strrchr strrchr
	#define Q_strtok strtok
	#define Q_strlwr _strlwr_s
	#define Q_strupr _strupr_s
	#define Q_strdup _strdup
	#define Q_sprintf sprintf_s
	#define Q_snprintf _snprintf
	#define Q_wcsncpy wcsncpy
	#define Q_vsnprintf _vsnprintf
	#define Q_vsnwprintf _vsnwprintf
	#define Q_atoi atoi
	#define Q_atof atof
	#define Q_sqrt sqrt
	#define Q_min min
	#define Q_max max
	#define Q_clamp clamp
	#define Q_abs abs
	#define Q_fabs fabs
	#define Q_tan tan
	#define Q_atan atan
	#define Q_atan2 atan2
	#define Q_acos acos
	#define Q_cos cos
	#define Q_sin sin
	#define Q_pow pow
	#define Q_fmod fmod
#endif // #if defined(ASMLIB_H) && defined(HAVE_OPT_STRTOOLS)

// size - sizeof(buffer)
inline char *Q_strlcpy(char *dest, const char *src, size_t size) {
	Q_strncpy(dest, src, size - 1);
	dest[size - 1] = '\0';
	return dest;
}

// a safe variant of strcpy that truncates the result to fit in the destination buffer
template <size_t size>
char *Q_strlcpy(char (&dest)[size], const char *src) {
	return Q_strlcpy(dest, src, size);
}

inline wchar_t* Q_wcslcpy(wchar_t* dest, const wchar_t* src, size_t count)
{
	Q_wcsncpy(dest, src, count - 1U);
	dest[count - 1] = L'\0';
	return dest;
}

template <size_t count>
wchar_t* Q_wcslcpy(wchar_t(&dest)[count], const wchar_t* src)
{
	return Q_wcslcpy(dest, src, count);
}

// a safe variant of sprintf which formatting strings.
template <size_t size, typename ... Args>
decltype(auto) Q_slprintf(char(&dest)[size], const char* format, Args ... args)
{
	auto _return = _snprintf(dest, size - 1U, format, args ...);
	dest[size - 1U] = '\0';
	return _return;
}

template <size_t size, typename ... Args>
decltype(auto) Q_wslprintf(wchar_t(&dest)[size], const wchar_t* format, Args ... args)
{
	auto _return = _snwprintf(dest, size - 1U, format, args ...);
	dest[size - 1U] = L'\0';
	return _return;
}

// safely concatenate two strings.
// a variant of strcat that truncates the result to fit in the destination buffer
template <size_t size>
size_t Q_strlcat(char (&dest)[size], const char *src)
{
	size_t srclen; // Length of source string
	size_t dstlen; // Length of destination string

	// Figure out how much room is left
	dstlen = Q_strlen(dest);
	size_t length = size - dstlen + 1;

	if (!length) {
		// No room, return immediately
		return dstlen;
	}

	// Figure out how much room is needed
	srclen = Q_strlen(src);

	// Copy the appropriate amount
	if (srclen > length) {
		srclen = length;
	}

	Q_memcpy(dest + dstlen, src, srclen);
	dest[dstlen + srclen] = '\0';

	return dstlen + srclen;
}

// strcpy that works correctly with overlapping src and dst buffers
inline char *Q_strcpy_s(char *dst, char *src)
{
	int len = Q_strlen(src);
	Q_memmove(dst, src, len + 1);
	return dst;
}

//===== Copyright ?1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

template< class T, class I > class CUtlMemory;
template< class T, class A > class CUtlVector;


//-----------------------------------------------------------------------------
// Portable versions of standard string functions
//-----------------------------------------------------------------------------

inline void		V_memset(void* dest, int fill, int count) { memset(dest, fill, count); }
inline void		V_memcpy(void* dest, const void* src, int count) { memcpy(dest, src, count); }
inline void		V_memmove(void* dest, const void* src, int count) { memmove(dest, src, count); }
inline int		V_memcmp(const void* m1, const void* m2, int count) { return memcmp(m1, m2, count); }
inline int		V_strlen(const char* str) { return (int)strlen(str); }
inline void		V_strcpy(char* dest, const char* src) { strcpy(dest, src); }
inline int		V_wcslen(const wchar_t* pwch) { return (int)wcslen(pwch); }
inline char*	V_strrchr(const char* s, char c) { return (char*)strrchr(s, c); }
inline int		V_strcmp(const char* s1, const char* s2) { return strcmp(s1, s2); }
inline int		V_wcscmp(const wchar_t* s1, const wchar_t* s2) { return wcscmp(s1, s2); }
inline int		V_stricmp(const char* s1, const char* s2) { return Q_stricmp(s1, s2); }
inline char*	V_strstr(const char* s1, const char* search) { return (char*)strstr(s1, search); }
inline char*	V_strupr(char* start) { return _strupr(start); }
inline char*	V_strlower(char* start) { return _strlwr(start); }


int			V_strncmp(const char* s1, const char* s2, int count);
int			V_strcasecmp(const char* s1, const char* s2);
int			V_strncasecmp(const char* s1, const char* s2, int n);
int			V_strnicmp(const char* s1, const char* s2, int n);
int			V_atoi(const char* str);
float		V_atof(const char* str);
char*		V_stristr(char* pStr, const char* pSearch);
const char*	V_stristr(const char* pStr, const char* pSearch);
const char*	V_strnistr(const char* pStr, const char* pSearch, int n);
const char*	V_strnchr(const char* pStr, char c, int n);

// returns string immediately following prefix, (ie str+strlen(prefix)) or NULL if prefix not found
const char* StringAfterPrefix(const char* str, const char* prefix);
const char* StringAfterPrefixCaseSensitive(const char* str, const char* prefix);
inline bool	StringHasPrefix(const char* str, const char* prefix) { return StringAfterPrefix(str, prefix) != NULL; }
inline bool	StringHasPrefixCaseSensitive(const char* str, const char* prefix) { return StringAfterPrefixCaseSensitive(str, prefix) != NULL; }


// Normalizes a float string in place.  
// (removes leading zeros, trailing zeros after the decimal point, and the decimal point itself where possible)
void			V_normalizeFloatString(char* pFloat);



// These are versions of functions that guarantee NULL termination.
//
// maxLen is the maximum number of bytes in the destination string.
// pDest[maxLen-1] is always NULL terminated if pSrc's length is >= maxLen.
//
// This means the last parameter can usually be a sizeof() of a string.
void V_strncpy(char* pDest, const char* pSrc, int maxLen);
int V_snprintf(char* pDest, int destLen, const char* pFormat, ...);
void V_wcsncpy(wchar_t* pDest, wchar_t const* pSrc, int maxLenInBytes);

#define COPY_ALL_CHARACTERS -1
char* V_strncat(char*, const char*, size_t destBufferSize, int max_chars_to_copy = COPY_ALL_CHARACTERS);
char* V_strnlwr(char*, size_t);


// UNDONE: Find a non-compiler-specific way to do this
#include <stdarg.h>

#ifdef _WIN32
#define CORRECT_PATH_SEPARATOR '\\'
#define INCORRECT_PATH_SEPARATOR '/'
#elif _LINUX
#define CORRECT_PATH_SEPARATOR '/'
#define INCORRECT_PATH_SEPARATOR '\\'
#endif

int V_vsnprintf(char* pDest, int maxLen, const char* pFormat, va_list params);

// Prints out a pretified memory counter string value ( e.g., 7,233.27 Mb, 1,298.003 Kb, 127 bytes )
char* V_pretifymem(float value, int digitsafterdecimal = 2, bool usebinaryonek = false);

// Prints out a pretified integer with comma separators (eg, 7,233,270,000)
char* V_pretifynum(int64 value);

// conversion functions wchar_t <-> char, returning the number of characters converted
int V_UTF8ToUnicode(const char* pUTF8, wchar_t* pwchDest, int cubDestSizeInBytes);
int V_UnicodeToUTF8(const wchar_t* pUnicode, char* pUTF8, int cubDestSizeInBytes);

// Functions for converting hexidecimal character strings back into binary data etc.
//
// e.g., 
// int output;
// V_hextobinary( "ffffffff", 8, &output, sizeof( output ) );
// would make output == 0xfffffff or -1
// Similarly,
// char buffer[ 9 ];
// V_binarytohex( &output, sizeof( output ), buffer, sizeof( buffer ) );
// would put "ffffffff" into buffer (note null terminator!!!)
void V_hextobinary(char const* in, int numchars, BYTE* out, int maxoutputbytes);
void V_binarytohex(const BYTE* in, int inputbytes, char* out, int outsize);

// Tools for working with filenames
// Extracts the base name of a file (no path, no extension, assumes '/' or '\' as path separator)
void V_FileBase(const char* in, char* out, int maxlen);
// Remove the final characters of ppath if it's '\' or '/'.
void V_StripTrailingSlash(char* ppath);
// Remove any extension from in and return resulting string in out
void V_StripExtension(const char* in, char* out, int outLen);
// Make path end with extension if it doesn't already have an extension
void V_DefaultExtension(char* path, const char* extension, int pathStringLength);
// Strips any current extension from path and ensures that extension is the new extension
void V_SetExtension(char* path, const char* extension, int pathStringLength);
// Removes any filename from path ( strips back to previous / or \ character )
void V_StripFilename(char* path);
// Remove the final directory from the path
bool V_StripLastDir(char* dirName, int maxlen);
// Returns a pointer to the unqualified file name (no path) of a file name
const char* V_UnqualifiedFileName(const char* in);
// Given a path and a filename, composes "path\filename", inserting the (OS correct) separator if necessary
void V_ComposeFileName(const char* path, const char* filename, char* dest, int destSize);

// Copy out the path except for the stuff after the final pathseparator
bool V_ExtractFilePath(const char* path, char* dest, int destSize);
// Copy out the file extension into dest
void V_ExtractFileExtension(const char* path, char* dest, int destSize);

const char* V_GetFileExtension(const char* path);

// This removes "./" and "../" from the pathname. pFilename should be a full pathname.
// Returns false if it tries to ".." past the root directory in the drive (in which case 
// it is an invalid path).
bool V_RemoveDotSlashes(char* pFilename, char separator = CORRECT_PATH_SEPARATOR);

// If pPath is a relative path, this function makes it into an absolute path
// using the current working directory as the base, or pStartingDir if it's non-NULL.
// Returns false if it runs out of room in the string, or if pPath tries to ".." past the root directory.
void V_MakeAbsolutePath(char* pOut, int outLen, const char* pPath, const char* pStartingDir = NULL);

// Creates a relative path given two full paths
// The first is the full path of the file to make a relative path for.
// The second is the full path of the directory to make the first file relative to
// Returns false if they can't be made relative (on separate drives, for example)
bool V_MakeRelativePath(const char* pFullPath, const char* pDirectory, char* pRelativePath, int nBufLen);

// Adds a path separator to the end of the string if there isn't one already. Returns false if it would run out of space.
void V_AppendSlash(char* pStr, int strSize);

// Returns true if the path is an absolute path.
bool V_IsAbsolutePath(const char* pPath);

// Scans pIn and replaces all occurences of pMatch with pReplaceWith.
// Writes the result to pOut.
// Returns true if it completed successfully.
// If it would overflow pOut, it fills as much as it can and returns false.
bool V_StrSubst(const char* pIn, const char* pMatch, const char* pReplaceWith,
	char* pOut, int outLen, bool bCaseSensitive = false);

// Split the specified string on the specified separator.
// Returns a list of strings separated by pSeparator.
// You are responsible for freeing the contents of outStrings (call outStrings.PurgeAndDeleteElements).
void V_SplitString(const char* pString, const char* pSeparator, CUtlVector<char*, CUtlMemory<char*, int> >& outStrings);

// Just like V_SplitString, but it can use multiple possible separators.
void V_SplitString2(const char* pString, const char** pSeparators, int nSeparators, CUtlVector<char*, CUtlMemory<char*, int> >& outStrings);


// This function takes a slice out of pStr and stores it in pOut.
// It follows the Python slice convention:
// Negative numbers wrap around the string (-1 references the last character).
// Large numbers are clamped to the end of the string.
void V_StrSlice(const char* pStr, int firstChar, int lastCharNonInclusive, char* pOut, int outSize);

// Chop off the left nChars of a string.
void V_StrLeft(const char* pStr, int nChars, char* pOut, int outSize);

// Chop off the right nChars of a string.
void V_StrRight(const char* pStr, int nChars, char* pOut, int outSize);

// change "special" characters to have their c-style backslash sequence. like \n, \r, \t, ", etc.
// returns a pointer to a newly allocated string, which you must delete[] when finished with.
char* V_AddBackSlashesToSpecialChars(char const* pSrc);

// Force slashes of either type to be = separator character
void V_FixSlashes(char* pname, char separator = CORRECT_PATH_SEPARATOR);

// This function fixes cases of filenames like materials\\blah.vmt or somepath\otherpath\\ and removes the extra double slash.
void V_FixDoubleSlashes(char* pStr);

// Convert multibyte to wchar + back
// Specify -1 for nInSize for null-terminated string
void V_strtowcs(const char* pString, int nInSize, wchar_t* pWString, int nOutSize);
void V_wcstostr(const wchar_t* pWString, int nInSize, char* pString, int nOutSize);

// buffer-safe strcat
inline void V_strcat(char* dest, const char* src, int cchDest)
{
	V_strncat(dest, src, cchDest, COPY_ALL_CHARACTERS);
}


//-----------------------------------------------------------------------------
// generic unique name helper functions
//-----------------------------------------------------------------------------

// returns startindex if none found, 2 if "prefix" found, and n+1 if "prefixn" found



// NOTE: This is for backward compatability!
// We need to DLL-export the Q methods in vstdlib but not link to them in other projects
#if !defined( VSTDLIB_BACKWARD_COMPAT )

#define Q_wcscmp				V_wcscmp
#define Q_wcslen				V_wcslen
#define	Q_strcasecmp			V_strcasecmp
#define	Q_strncasecmp			V_strncasecmp
#define	Q_stristr				V_stristr
#define	Q_strnistr				V_strnistr
#define	Q_strnchr				V_strnchr
#define Q_normalizeFloatString	V_normalizeFloatString
#define Q_strnlwr				V_strnlwr
#define Q_pretifymem			V_pretifymem
#define Q_pretifynum			V_pretifynum
#define Q_UTF8ToUnicode			V_UTF8ToUnicode
#define Q_UnicodeToUTF8			V_UnicodeToUTF8
#define Q_hextobinary			V_hextobinary
#define Q_binarytohex			V_binarytohex
#define Q_FileBase				V_FileBase
#define Q_StripTrailingSlash	V_StripTrailingSlash
#define Q_StripExtension		V_StripExtension
#define	Q_DefaultExtension		V_DefaultExtension
#define Q_SetExtension			V_SetExtension
#define Q_StripFilename			V_StripFilename
#define Q_StripLastDir			V_StripLastDir
#define Q_UnqualifiedFileName	V_UnqualifiedFileName
#define Q_ComposeFileName		V_ComposeFileName
#define Q_ExtractFilePath		V_ExtractFilePath
#define Q_ExtractFileExtension	V_ExtractFileExtension
#define Q_GetFileExtension		V_GetFileExtension
#define Q_RemoveDotSlashes		V_RemoveDotSlashes
#define Q_MakeAbsolutePath		V_MakeAbsolutePath
#define Q_AppendSlash			V_AppendSlash
#define Q_IsAbsolutePath		V_IsAbsolutePath
#define Q_StrSubst				V_StrSubst
#define Q_SplitString			V_SplitString
#define Q_SplitString2			V_SplitString2
#define Q_StrSlice				V_StrSlice
#define Q_StrLeft				V_StrLeft
#define Q_StrRight				V_StrRight
#define Q_FixSlashes			V_FixSlashes
#define Q_strtowcs				V_strtowcs
#define Q_wcstostr				V_wcstostr
#define Q_GenerateUniqueNameIndex	V_GenerateUniqueNameIndex
#define Q_GenerateUniqueName		V_GenerateUniqueName
#define Q_MakeRelativePath		V_MakeRelativePath

#endif // !defined( VSTDLIB_DLL_EXPORT )


#endif	// TIER1_STRTOOLS_H
