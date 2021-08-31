/*

Created Date: Aug 29 2021

Modern Warfare Dev Team
Programmer - Luna the Reborn

*/

#ifndef _GOLDSRC_MSG_HELPER_HPP_
#define _GOLDSRC_MSG_HELPER_HPP_
#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <utility>

template<size_t N>
struct StringLiteral
{
	// Constructor
	constexpr StringLiteral(const char(&str)[N]) { std::copy_n(str, N, value); }

	// Operators
	constexpr operator const char* () const { return &value[0]; }	// automatically convert to char* if necessary.
	constexpr operator char* () { return &value[0]; }
	constexpr decltype(auto) operator[] (size_t index) { assert(index < N); return value[index]; }

	static constexpr size_t length = N;
	char value[N];
};

// DO NOT USE
// This is used only for determine whether a class is a message.
struct __Dummy_GoldSrc_Msg_Struct {};

template<StringLiteral _name, typename... ArgTys>
struct Message : public __Dummy_GoldSrc_Msg_Struct
{
	// Constrains
	static_assert(sizeof(_name.value) <= 11U, "Name of message must less than 11 characters.");

	// Helper class.
	using MsgArgs = std::tuple<ArgTys...>;
	using TypeOfThis = Message<_name, ArgTys...>;

	// Constants
	static constexpr auto NAME = _name;	// Convertible to char* at anytime.
	static constexpr auto COUNT = sizeof...(ArgTys);
	static constexpr std::array<size_t, COUNT> SIZE_OF_EACH_TY = { sizeof(ArgTys)... };
	static constexpr bool HAS_STRING = (std::is_same_v<std::decay_t<ArgTys>, const char*> || ...);	// Once a string is placed, there will be no chance for a constant length message.
	static constexpr bool HAS_VECTOR = (std::is_same_v<std::decay_t<ArgTys>, Vector> || ...);	// The vector uses WRITE_COORD, hence it has total size of 3*2=6 instead of 3*4=12.
	static constexpr auto SIZE = std::accumulate(SIZE_OF_EACH_TY.cbegin(), SIZE_OF_EACH_TY.cend(), 0U);
	static constexpr auto IDX_SEQ = std::make_index_sequence<COUNT>{};

	// Members
	static inline int m_iMessageIndex = 0;

	// Methods
	static void Register(void)
	{
		if (m_iMessageIndex)
			return;

		if constexpr (HAS_STRING || HAS_VECTOR)
			m_iMessageIndex = REG_USER_MSG(NAME, -1);	// Any length.	(Written bytes unchecked by engine)
		else
			m_iMessageIndex = REG_USER_MSG(NAME, SIZE);	// No message arg case is included.
	}

	template<int iDest>
	static void Cast(const Vector& vecOrigin, const entvars_t* pClient, const ArgTys&... args)
	{
		auto fnSend = [](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, int>)
				WRITE_LONG(arg);
			else if constexpr (std::is_same_v<T, float>)
				WRITE_LONG(std::bit_cast<int>(arg));
			else if constexpr (std::is_same_v<T, const char*>)
				WRITE_STRING(arg);
			else if constexpr (std::is_same_v<T, Vector>)
			{
				WRITE_COORD(arg.x);
				WRITE_COORD(arg.y);
				WRITE_COORD(arg.z);
			}
			else if constexpr (std::is_same_v<T, short>)
				WRITE_SHORT(arg);
			else if constexpr (std::is_same_v<T, unsigned char>)
				WRITE_BYTE(arg);
			else if constexpr (std::is_same_v<T, char>)	// signed char
				WRITE_CHAR(arg);
			else if constexpr (std::is_same_v<T, bool>)	// signed char
				WRITE_BYTE(arg);
			else
				static_assert(always_false_v<T>, "non-exhaustive visitor!");
		};
		
		if constexpr (iDest == MSG_ONE || iDest == MSG_ONE_UNRELIABLE || iDest == MSG_INIT)
			MESSAGE_BEGIN(iDest, m_iMessageIndex, nullptr, pClient);
		else if constexpr (iDest == MSG_ALL || iDest == MSG_BROADCAST || iDest == MSG_SPEC)
			MESSAGE_BEGIN(iDest, m_iMessageIndex);
		else if constexpr (iDest == MSG_PAS || iDest == MSG_PAS_R || iDest == MSG_PVS || iDest == MSG_PVS_R)
			MESSAGE_BEGIN(iDest, m_iMessageIndex, vecOrigin);
		else
			static_assert(always_false_v<TypeOfThis>, "Invalid message casting method!");

		MsgArgs tplArgs = std::make_tuple(args...);

		// No panic, this is a instant-called lambda function.
		// De facto static_for.
		[&]<size_t... I>(std::index_sequence<I...>)
		{
			(fnSend(std::get<I>(tplArgs)), ...);
		}
		(IDX_SEQ);

		MESSAGE_END();
	}

	static inline void Send(const entvars_t* pClient, const ArgTys&... args) { return Cast<MSG_ONE>(nullptr, pClient, args...); }
	template<int _dest> static inline void Broadcast(const ArgTys&... args) { return Cast<_dest>(Vector::Zero(), nullptr, args...); }
	template<int _dest> static inline void Region(const Vector& vecOrigin, const ArgTys&... args) { return Cast<_dest>(vecOrigin, nullptr, args...); }

	static void Parse(void)
	{
		auto fnRead = [](auto& val)
		{
			using T = std::decay_t<decltype(val)>;

			if constexpr (std::is_same_v<T, int>)
				val = std::numeric_limits<int>::max();
			else if constexpr (std::is_same_v<T, float>)
				val = std::numeric_limits<float>::max();
			else if constexpr (std::is_same_v<T, const char*>)
			{
				constexpr auto str = "3.1415926535";	// strdup.
				val = str;
			}
			else if constexpr (std::is_same_v<T, Vector>)
				val = Vector(3, 4, 5);
			else if constexpr (std::is_same_v<T, short>)
				val = std::numeric_limits<short>::max();
			else if constexpr (std::is_same_v<T, BYTE>)
				val = std::numeric_limits<BYTE>::max();
			else
				static_assert(always_false_v<T>, "non-exhaustive visitor!");
		};

		MsgArgs tplArgs;
		auto fnFill = [&]<size_t... I>(std::index_sequence<I...>)
		{
			(fnRead(std::get<I>(tplArgs)), ...);
		};

		auto fnApply = [&tplArgs]<size_t... I>(std::index_sequence<I...>)	// Fuck the std::apply. It totally does not work.
		{
			MsgReceived<TypeOfThis, ArgTys...>(std::get<I>(tplArgs)...);
		};

		fnFill(IDX_SEQ);
		fnApply(IDX_SEQ);
	}
};

// Is this class a GoldSrc msg class?
template<typename T>
concept IsMessage = std::is_base_of_v<__Dummy_GoldSrc_Msg_Struct, T>;

// Automatically register message to engine.
template<typename... MsgTys>
requires(IsMessage<MsgTys> && ...)
inline void RegisterMessage()
{
	(MsgTys::Register(), ...);
}

// [CLIENT] Default fallback function.
template<typename MsgClass, typename... ArgTys>
requires(IsMessage<MsgClass>)
void MsgReceived(ArgTys&... args)
{
#ifdef _DEBUG
	static const std::string error_msg = std::string("Message: ") + MsgClass::NAME + std::string(" not handled by user!");
	assert(error_msg.c_str() && false);
#endif
}

//using gmsgGiveWpn = Message<"GiveWpn", unsigned char, Vector, const char*, short>;
//template<> extern void MsgReceived<gmsgGiveWpn>(unsigned char&, Vector&, const char*&, short&);

//using gmsgRmAllWpn = Message<"RmAllWpn">;
//template<> extern void MsgReceived<StringLiteral("RmAllWpn")>();

































#endif // _GOLDSRC_MSG_HELPER_HPP_