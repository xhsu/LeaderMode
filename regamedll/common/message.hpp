#ifndef _GOLDSRC_MSG_HELPER_HPP_
#define _GOLDSRC_MSG_HELPER_HPP_
#pragma once

template<size_t N>
struct StringLiteral
{
	constexpr StringLiteral(const char(&str)[N]) { std::copy_n(str, N, value); }
	constexpr operator const char* () const { return &value[0]; }

	char value[N];
};

template<StringLiteral _name, typename... ArgTys>
void MsgReceived(ArgTys&... args)
{
	std::cout << _name.value << ':' << std::endl;
	((std::cout << args << std::endl), ...);
}

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

//	using MsgArg2 = std::variant<ArgTys...>;
	using CallbackArgs = std::tuple<ArgTys...>;

	// Constants
	static constexpr auto NAME = _name.value;
//	static constexpr auto SIZE = (sizeof(ArgTys) + ...);
	static constexpr auto COUNT = sizeof...(ArgTys);
	static constexpr auto IDX_SEQ = std::make_index_sequence<COUNT>{};

	static void Register(void)
	{
//		cout << "size of the message \"" << NAME << "\" is " << SIZE << '.' << endl;
	}

	static void Cast(int iDest, const Vector& vecOrigin, void* pClient, const ArgTys&... args)
	{
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

		cout << "Cast message to " << iDest << " at " << endl << vecOrigin << " for " << pClient << endl;

		std::array<MsgArg, COUNT> BUFFER = { args... };

		for (auto& arg : BUFFER)
		{
//			std::visit(f, arg);
			switch (arg.m_type)
			{
			case MsgArg::Int:
				cout << "int: " << arg.INT << endl;
				break;

			case MsgArg::Flt:
				cout << "float: " << arg.FLOAT << endl;
				break;

			case MsgArg::Str:
				cout << "string: " << arg.STRING << endl;
				break;

			case MsgArg::Vec:
				cout << "vector: " << endl << arg.VEC;
				break;

			case MsgArg::Srt:
				cout << "short: " << arg.SRT << endl;
				break;

			case MsgArg::Byt:
				cout << "byte: " << arg.BYT << endl;
				break;

			default:
				break;
			}
		}
	}

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

		CallbackArgs tplArgs;
		auto fnFill = [&]<size_t... I>(std::index_sequence<I...>)
		{
			(fnRead(std::get<I>(tplArgs)), ...);
		};

		auto fnApply = [&tplArgs]<size_t... I>(std::index_sequence<I...>)	// Fuck the std::apply. It totally does not work.
		{
			MsgReceived<_name, ArgTys...>(std::get<I>(tplArgs)...);
		};

		fnFill(IDX_SEQ);
		fnApply(IDX_SEQ);
	}
};

using gmsgGiveWpn = Message<"GiveWpn", unsigned char, Vector, const char*, short>;
template<> extern void MsgReceived<StringLiteral("GiveWpn")>(unsigned char&, Vector&, const char*&, short&);

using gmsgRmAllWpn = Message<"RmAllWpn">;
template<> extern void MsgReceived<StringLiteral("RmAllWpn")>();

































#endif // _GOLDSRC_MSG_HELPER_HPP_