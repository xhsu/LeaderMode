/*
* This file suppose included after all entity class is declared.
* Aug 21 2021.
*/

#ifndef _SERVER_UTILS_HPP_
#define _SERVER_UTILS_HPP_
#pragma once

#include "progdefs.h"

#define FSND_USING_3D	(1<<0)
#define FSND_USING_SSTR	(1<<1)
#define FSND_HAS_PITCH	(1<<2)
#define FSND_HAS_VOLUME	(1<<3)

enum EFSNDArg
{
	FSND_NO_HOST = 0,
	FSND_2D_HOST,
	FSND_3D_GLOBAL,	// Set the host argument to nullptr.
};

template <EFSNDArg iSendType, typename strTy>
inline void UTIL_Play3DSound(CBasePlayer* pHost, const Vector& vecSrc, float flRange, strTy sample, float flVol = 1.0f, int iPitch = 100)
{
	CBaseEntity* pEntity = nullptr;
	CBasePlayer* pPlayer = nullptr;
	byte bitsFlags = FSND_USING_3D;

	if constexpr (std::is_convertible_v<strTy, SharedString> || std::is_same_v<strTy, SharedString>)
		bitsFlags |= FSND_USING_SSTR;

	if (flVol >= 1.0f)
		bitsFlags |= FSND_HAS_VOLUME;

	if (iPitch != 100)
		bitsFlags |= FSND_HAS_PITCH;

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRange)))
	{
		if (!pEntity->IsPlayer())
			continue;

		pPlayer = dynamic_cast<CBasePlayer*>(pEntity);

		if (pPlayer != nullptr && (pPlayer != pHost || iSendType == FSND_3D_GLOBAL))
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgSound, vecSrc, pPlayer->pev);
			WRITE_BYTE(bitsFlags);
			WRITE_COORD(vecSrc.x);
			WRITE_COORD(vecSrc.y);
			WRITE_COORD(vecSrc.z);
			WRITE_COORD(flRange);

			if constexpr (std::is_convertible_v<strTy, SharedString> || std::is_same_v<strTy, SharedString>)
				WRITE_BYTE(sample);
			else
				WRITE_STRING(sample);

			if (flVol >= 1.0f)
				WRITE_BYTE(std::clamp<int>(std::roundf(flVol * 100.0f), 0, 255));
			if (iPitch != 100)
				WRITE_BYTE(std::clamp(iPitch, 0, 255));

			MESSAGE_END();
		}
		else
		{
			if constexpr (iSendType == FSND_2D_HOST)
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgSound, vecSrc, pPlayer->pev);
				WRITE_BYTE(bitsFlags & ~FSND_USING_3D);	// not using 3D.

				if constexpr (std::is_convertible_v<strTy, SharedString> || std::is_same_v<strTy, SharedString>)
					WRITE_BYTE(sample);
				else
					WRITE_STRING(sample);

				if (flVol >= 1.0f)
					WRITE_BYTE(std::clamp<int>(std::roundf(flVol * 100.0f), 0, 255));
				if (iPitch != 100)
					WRITE_BYTE(std::clamp(iPitch, 0, 255));

				MESSAGE_END();
			}
		}
	}
}

template<size_t N>
struct StringLiteral
{
	constexpr StringLiteral(const char(&str)[N]) { std::copy_n(str, N, value); }

	char value[N];
};

template<StringLiteral _name, typename... ArgTys>
struct Message
{
	// Constrains
	static_assert(sizeof(_name.value) <= 11U, "Name of message must less than 11 characters.");

	// Helper class.
	struct MsgArg
	{
		MsgArg(int e) { INT = e; m_type = Int; }
		MsgArg(float e) { FLOAT = e; m_type = Flt; }
		MsgArg(const char* e) { STRING = e; m_type = Str; }
		MsgArg(const Vector& e) { VEC = e; m_type = Vec; }
		MsgArg(short e) { SRT = e; m_type = Srt; }
		MsgArg(BYTE e) { BYT = e; m_type = Byt; }

		enum { Int, Flt, Str, Vec, Srt, Byt } m_type;

		union
		{
			int			INT;
			float		FLOAT;
			const char* STRING;
			Vector		VEC;
			short		SRT;
			BYTE		BYT;
		};
	};

	//using MsgArg2 = std::variant<ArgTys...>;

	// Constants
	static constexpr auto NAME = _name.value;
//	static constexpr auto SIZE = (sizeof(ArgTys) + ...);	// #WPN_POLISH_CODE This does not work on message without any args.
	static constexpr auto COUNT = sizeof...(ArgTys);

	// Members
	static inline int m_iMessageIndex = 0;

	static void Register(void)
	{
		if (m_iMessageIndex)
			return;

		if constexpr (COUNT > 0)
			m_iMessageIndex = REG_USER_MSG(NAME, -1/*SIZE*/);	// Luna: We can't tell whether the arguments contains a string.
		else
			m_iMessageIndex = REG_USER_MSG(NAME, 0);
	}

	static void Send(int iDest, const entvars_t* pClient, const ArgTys&... args)
	{
		assert(iDest == MSG_ONE || iDest == MSG_ONE_UNRELIABLE || iDest == MSG_INIT);

		/*auto f = [](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<T, int>)
				std::cout << "int with value " << arg << '\n';
			else if constexpr (std::is_same_v<T, float>)
				std::cout << "float with value " << arg << '\n';
			else if constexpr (std::is_same_v<T, const char*>)
				std::cout << "std::string with value " << std::quoted(arg) << '\n';
			else if constexpr (std::is_same_v<T, Vector>)
				cout << "vector: " << endl << arg.m_data.VEC;
			else if constexpr (std::is_same_v<T, short>)
				std::cout << "short with value " << arg << '\n';
			else if constexpr (std::is_same_v<T, unsigned char>)
				std::cout << "byte with value " << arg << '\n';
			else
				static_assert(always_false_v<T>, "non-exhaustive visitor!");
		};*/

		MESSAGE_BEGIN(iDest, m_iMessageIndex, nullptr, pClient);

		if constexpr (COUNT > 0)
		{
			std::array<MsgArg, COUNT> BUFFER = { args... };

			for (auto& arg : BUFFER)
			{
				//std::visit(f, arg);
				switch (arg.m_type)
				{
				case MsgArg::Int:
					WRITE_LONG(arg.INT);
					break;

				case MsgArg::Flt:
					WRITE_LONG(std::bit_cast<int>(arg.FLOAT));
					break;

				case MsgArg::Str:
					WRITE_STRING(arg.STRING);
					break;

				case MsgArg::Vec:
					WRITE_COORD(arg.VEC.x);
					WRITE_COORD(arg.VEC.y);
					WRITE_COORD(arg.VEC.z);
					break;

				case MsgArg::Srt:
					WRITE_SHORT(arg.SRT);
					break;

				case MsgArg::Byt:
					WRITE_BYTE(arg.BYT);
					break;

				default:
					break;
				}
			}
		}

		MESSAGE_END();
	}

	static void Broadcast(int iDest, const ArgTys&... args)
	{
		assert(iDest == MSG_ALL || iDest == MSG_BROADCAST || iDest == MSG_SPEC);

		MESSAGE_BEGIN(iDest, m_iMessageIndex);

		if constexpr (COUNT > 0)
		{
			std::array<MsgArg, COUNT> BUFFER = { args... };

			for (auto& arg : BUFFER)
			{
				//std::visit(f, arg);
				switch (arg.m_type)
				{
				case MsgArg::Int:
					WRITE_LONG(arg.INT);
					break;

				case MsgArg::Flt:
					WRITE_LONG(std::bit_cast<int>(arg.FLOAT));
					break;

				case MsgArg::Str:
					WRITE_STRING(arg.STRING);
					break;

				case MsgArg::Vec:
					WRITE_COORD(arg.VEC.x);
					WRITE_COORD(arg.VEC.y);
					WRITE_COORD(arg.VEC.z);
					break;

				case MsgArg::Srt:
					WRITE_SHORT(arg.SRT);
					break;

				case MsgArg::Byt:
					WRITE_BYTE(arg.BYT);
					break;

				default:
					break;
				}
			}
		}

		MESSAGE_END();
	}

	static void Region(int iDest, const Vector& vecOrigin, const ArgTys&... args)
	{
		assert(iDest == MSG_PAS || iDest == MSG_PAS_R || iDest == MSG_PVS || iDest == MSG_PVS_R);

		MESSAGE_BEGIN(iDest, m_iMessageIndex, vecOrigin);

		if constexpr (COUNT > 0)
		{
			std::array<MsgArg, COUNT> BUFFER = { args... };

			for (auto& arg : BUFFER)
			{
				//std::visit(f, arg);
				switch (arg.m_type)
				{
				case MsgArg::Int:
					WRITE_LONG(arg.INT);
					break;

				case MsgArg::Flt:
					WRITE_LONG(std::bit_cast<int>(arg.FLOAT));
					break;

				case MsgArg::Str:
					WRITE_STRING(arg.STRING);
					break;

				case MsgArg::Vec:
					WRITE_COORD(arg.VEC.x);
					WRITE_COORD(arg.VEC.y);
					WRITE_COORD(arg.VEC.z);
					break;

				case MsgArg::Srt:
					WRITE_SHORT(arg.SRT);
					break;

				case MsgArg::Byt:
					WRITE_BYTE(arg.BYT);
					break;

				default:
					break;
				}
			}
		}

		MESSAGE_END();
	}

	template<typename... CustomTys>
	static void Custom(int iDest, const Vector& vecOrigin, const entvars_t* pClient, const CustomTys&... args) requires(COUNT > 0)
	{
		std::array<MsgArg, sizeof...(args)> BUFFER = { args... };

		MESSAGE_BEGIN(iDest, m_iMessageIndex, vecOrigin, pClient);

		for (auto& arg : BUFFER)
		{
			//std::visit(f, arg);
			switch (arg.m_type)
			{
			case MsgArg::Int:
				WRITE_LONG(arg.INT);
				break;

			case MsgArg::Flt:
				WRITE_LONG(std::bit_cast<int>(arg.FLOAT));
				break;

			case MsgArg::Str:
				WRITE_STRING(arg.STRING);
				break;

			case MsgArg::Vec:
				WRITE_COORD(arg.VEC.x);
				WRITE_COORD(arg.VEC.y);
				WRITE_COORD(arg.VEC.z);
				break;

			case MsgArg::Srt:
				WRITE_SHORT(arg.SRT);
				break;

			case MsgArg::Byt:
				WRITE_BYTE(arg.BYT);
				break;

			default:
				break;
			}
		}

		MESSAGE_END();
	}
};

template<typename... MsgTys>
inline void RegisterMessage()
{
	(MsgTys::Register(), ...);
}

using gmsgRmWpn = Message<"RmWpn", BYTE/*Which weapon?*/>;	// [0: Current Weapon]; [255: All weapons]; [255-Slot: Weapon in this slot];
using gmsgUseTank = Message<"UseTank", short/* Entindex */, BYTE/* On or off */>;
using gmsgSchemeEv = Message<"SchemeEv">;	// No arg.
using gmsgAmmo = Message<"Ammo", BYTE/* What ammo? */, short/* Count */>;	// [AmmoType == 255: All ammo.]
using gmsgDeployWpn = Message<"DeployWpn", BYTE /* Weapon ID */, BYTE /* Is insta switch? */>;
using gmsgBuy = Message<"Buy", BYTE /* [0-Wpn; 1-Ammo; 2-Eqp] */, BYTE /* Index */, short /* Amount */>; enum : BYTE { BUYTYPE_WPN = 0U, BUYTYPE_AMMO, BUYTYPE_EQP, };


































#endif	// _SERVER_UTILS_HPP_