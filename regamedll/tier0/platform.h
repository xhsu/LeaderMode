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

#pragma once

#include "osconfig.h"

#include <malloc.h> // need this for _alloca
#include <string.h> // need this for memset

#include "archtypes.h"

#define ALIGN16
#define FORCEINLINE __forceinline
#define RESTRICT __restrict
#define DECL_ALIGN(x) __declspec(align(x))
#define FASTCALL __fastcall

#define PLATFORM_INTERFACE

#define abstract_class class

// Defines MAX_PATH
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

// Used to step into the debugger
#define DebuggerBreak() __asm { int 3 }

// C functions for external declarations that call the appropriate C++ methods
#ifndef EXPORT
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT /**/
#endif
#endif

#ifdef _WIN32
// Used for dll exporting and importing
#define DLL_EXPORT extern "C" __declspec(dllexport)
#define DLL_IMPORT extern "C" __declspec(dllimport)

// Can't use extern "C" when DLL exporting a class
#define DLL_CLASS_EXPORT __declspec(dllexport)
#define DLL_CLASS_IMPORT __declspec(dllimport)

// Can't use extern "C" when DLL exporting a global
#define DLL_GLOBAL_EXPORT extern __declspec(dllexport)
#define DLL_GLOBAL_IMPORT extern __declspec(dllimport)
#elif defined __linux__

// Used for dll exporting and importing
#define DLL_EXPORT extern "C"
#define DLL_IMPORT extern "C"

// Can't use extern "C" when DLL exporting a class
#define DLL_CLASS_EXPORT
#define DLL_CLASS_IMPORT

// Can't use extern "C" when DLL exporting a global
#define DLL_GLOBAL_EXPORT extern
#define DLL_GLOBAL_IMPORT extern

#else
#error "Unsupported Platform."
#endif

#ifdef _WIN32
	// Remove warnings from warning level 4.
	#pragma warning(disable:4514) // warning C4514: 'acosl' : unreferenced inline function has been removed
	#pragma warning(disable:4100) // warning C4100: 'hwnd' : unreferenced formal parameter
	#pragma warning(disable:4127) // warning C4127: conditional expression is constant
	#pragma warning(disable:4512) // warning C4512: 'InFileRIFF' : assignment operator could not be generated
	#pragma warning(disable:4611) // warning C4611: interaction between '_setjmp' and C++ object destruction is non-portable
	#pragma warning(disable:4706) // warning C4706: assignment within conditional expression
	#pragma warning(disable:4710) // warning C4710: function 'x' not inlined
	#pragma warning(disable:4702) // warning C4702: unreachable code
	#pragma warning(disable:4505) // unreferenced local function has been removed
	#pragma warning(disable:4239) // nonstandard extension used : 'argument' ( conversion from class Vector to class Vector& )
	#pragma warning(disable:4097) // typedef-name 'BaseClass' used as synonym for class-name 'CFlexCycler::CBaseFlex'
	#pragma warning(disable:4324) // Padding was added at the end of a structure
	#pragma warning(disable:4244) // type conversion warning.
	#pragma warning(disable:4305) // truncation from 'const double ' to 'float '
	#pragma warning(disable:4786) // Disable warnings about long symbol names

	#if _MSC_VER >= 1300
		#pragma warning(disable:4511) // Disable warnings about private copy constructors
	#endif
#endif

// Methods to invoke the constructor, copy constructor, and destructor
template <class T>
inline void Construct(T *pMemory)
{
	new(pMemory) T;
}

template <class T>
inline void CopyConstruct(T *pMemory, T const &src)
{
	new(pMemory) T(src);
}

template <class T>
inline void Destruct(T *pMemory)
{
	pMemory->~T();

#ifdef _DEBUG
	memset(pMemory, 0xDD, sizeof(T));
#endif
}

inline bool IsPC(void) { return true; }
inline bool IsX360(void) { return false; }
inline bool IsXbox(void) { return false; }


#if defined(_M_IX86)
#define __i386__	1
#endif

//-------------------------------------
// Fast swaps
//-------------------------------------

#if defined( _X360 )

#define WordSwap  WordSwap360Intr
#define DWordSwap DWordSwap360Intr

template <typename T>
inline T WordSwap360Intr(T w)
{
	T output;
	__storeshortbytereverse(w, 0, &output);
	return output;
}

template <typename T>
inline T DWordSwap360Intr(T dw)
{
	T output;
	__storewordbytereverse(dw, 0, &output);
	return output;
}

#elif defined( _MSC_VER )

#define WordSwap  WordSwapAsm
#define DWordSwap DWordSwapAsm

#pragma warning(push)
#pragma warning (disable:4035) // no return value

template <typename T>
inline T WordSwapAsm(T w)
{
	__asm
	{
		mov ax, w
		xchg al, ah
	}
}

template <typename T>
inline T DWordSwapAsm(T dw)
{
	__asm
	{
		mov eax, dw
		bswap eax
	}
}

#pragma warning(pop)

#else

#define WordSwap  WordSwapC
#define DWordSwap DWordSwapC

#endif

//-------------------------------------
// The typically used methods.
//-------------------------------------

#if defined(__i386__)
#define LITTLE_ENDIAN 1
#endif

#if defined( _SGI_SOURCE ) || defined( _X360 )
#define	BIG_ENDIAN 1
#endif

// If a swapped float passes through the fpu, the bytes may get changed.
// Prevent this by swapping floats as DWORDs.
#define SafeSwapFloat( pOut, pIn )	(*((uint*)pOut) = DWordSwap( *((uint*)pIn) ))

#if defined(LITTLE_ENDIAN)

#define BigShort( val )				WordSwap( val )
#define BigWord( val )				WordSwap( val )
#define BigLong( val )				DWordSwap( val )
#define BigDWord( val )				DWordSwap( val )
#define LittleShort( val )			( val )
#define LittleWord( val )			( val )
#define LittleLong( val )			( val )
#define LittleDWord( val )			( val )
#define SwapShort( val )			BigShort( val )
#define SwapWord( val )				BigWord( val )
#define SwapLong( val )				BigLong( val )
#define SwapDWord( val )			BigDWord( val )

// Pass floats by pointer for swapping to avoid truncation in the fpu
#define BigFloat( pOut, pIn )		SafeSwapFloat( pOut, pIn )
#define LittleFloat( pOut, pIn )	( *pOut = *pIn )
#define SwapFloat( pOut, pIn )		BigFloat( pOut, pIn )

#elif defined(BIG_ENDIAN)

#define BigShort( val )				( val )
#define BigWord( val )				( val )
#define BigLong( val )				( val )
#define BigDWord( val )				( val )
#define LittleShort( val )			WordSwap( val )
#define LittleWord( val )			WordSwap( val )
#define LittleLong( val )			DWordSwap( val )
#define LittleDWord( val )			DWordSwap( val )
#define SwapShort( val )			LittleShort( val )
#define SwapWord( val )				LittleWord( val )
#define SwapLong( val )				LittleLong( val )
#define SwapDWord( val )			LittleDWord( val )

// Pass floats by pointer for swapping to avoid truncation in the fpu
#define BigFloat( pOut, pIn )		( *pOut = *pIn )
#define LittleFloat( pOut, pIn )	SafeSwapFloat( pOut, pIn )
#define SwapFloat( pOut, pIn )		LittleFloat( pOut, pIn )

#else

// @Note (toml 05-02-02): this technique expects the compiler to
// optimize the expression and eliminate the other path. On any new
// platform/compiler this should be tested.
inline short BigShort(short val) { int test = 1; return (*(char*)&test == 1) ? WordSwap(val) : val; }
inline uint16 BigWord(uint16 val) { int test = 1; return (*(char*)&test == 1) ? WordSwap(val) : val; }
inline long BigLong(long val) { int test = 1; return (*(char*)&test == 1) ? DWordSwap(val) : val; }
inline uint32 BigDWord(uint32 val) { int test = 1; return (*(char*)&test == 1) ? DWordSwap(val) : val; }
inline short LittleShort(short val) { int test = 1; return (*(char*)&test == 1) ? val : WordSwap(val); }
inline uint16 LittleWord(uint16 val) { int test = 1; return (*(char*)&test == 1) ? val : WordSwap(val); }
inline long LittleLong(long val) { int test = 1; return (*(char*)&test == 1) ? val : DWordSwap(val); }
inline uint32 LittleDWord(uint32 val) { int test = 1; return (*(char*)&test == 1) ? val : DWordSwap(val); }
inline short SwapShort(short val) { return WordSwap(val); }
inline uint16 SwapWord(uint16 val) { return WordSwap(val); }
inline long SwapLong(long val) { return DWordSwap(val); }
inline uint32 SwapDWord(uint32 val) { return DWordSwap(val); }

// Pass floats by pointer for swapping to avoid truncation in the fpu
inline void BigFloat(float* pOut, const float* pIn) { int test = 1; (*(char*)&test == 1) ? SafeSwapFloat(pOut, pIn) : (*pOut = *pIn); }
inline void LittleFloat(float* pOut, const float* pIn) { int test = 1; (*(char*)&test == 1) ? (*pOut = *pIn) : SafeSwapFloat(pOut, pIn); }
inline void SwapFloat(float* pOut, const float* pIn) { SafeSwapFloat(pOut, pIn); }

#endif

#if _X360
inline unsigned long LoadLittleDWord(unsigned long* base, unsigned int dwordIndex)
{
	return __loadwordbytereverse(dwordIndex << 2, base);
}

inline void StoreLittleDWord(unsigned long* base, unsigned int dwordIndex, unsigned long dword)
{
	__storewordbytereverse(dword, dwordIndex << 2, base);
}
#else
inline unsigned long LoadLittleDWord(unsigned long* base, unsigned int dwordIndex)
{
	return LittleDWord(base[dwordIndex]);
}

inline void StoreLittleDWord(unsigned long* base, unsigned int dwordIndex, unsigned long dword)
{
	base[dwordIndex] = LittleDWord(dword);
}
#endif
