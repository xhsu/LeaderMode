#include "precompiled.h"


char s_shared_token[1500];
char s_shared_quote = '\"';

NOXREF wchar_t *SharedWVarArgs(const wchar_t *format, ...)
{
	va_list argptr;
	const int BufLen = 1024;
	const int NumBuffers = 4;
	static wchar_t string[NumBuffers][BufLen];
	static int curstring = 0;

	curstring = (curstring + 1) % NumBuffers;

	va_start(argptr, format);
	Q_vsnwprintf(string[curstring], BufLen, format, argptr);
	va_end(argptr);

	return string[curstring];
}

char *SharedVarArgs(const char *format, ...)
{
	va_list argptr;
	const int BufLen = 1024;
	const int NumBuffers = 4;

	static char string[NumBuffers][BufLen];
	static int curstring = 0;

	curstring = (curstring + 1) % NumBuffers;

	va_start(argptr, format);
	Q_vsnprintf(string[curstring], BufLen, format, argptr);
	va_end(argptr);

	return string[curstring];
}

char *BufPrintf(char *buf, int &len, const char *fmt, ...)
{
	va_list argptr;
	if (len > 0)
	{
		va_start(argptr, fmt);
		Q_vsnprintf(buf, len, fmt, argptr);
		va_end(argptr);

		len -= Q_strlen(buf);
		return buf + Q_strlen(buf);
	}

	return nullptr;
}

wchar_t *BufWPrintf(wchar_t *buf, int &len, const wchar_t *fmt, ...)
{
	if (len <= 0)
		return nullptr;

	va_list argptr;

	va_start(argptr, fmt);
	Q_vsnwprintf(buf, len, fmt, argptr);
	va_end(argptr);

	len -= wcslen(buf);
	return buf + wcslen(buf);
}

NOXREF const wchar_t *NumAsWString(int val)
{
	const int BufLen = 16;
	const int NumBuffers = 4;
	static wchar_t string[NumBuffers][BufLen];
	static int curstring = 0;

	curstring = (curstring + 1) % NumBuffers;

	int len = BufLen;
	BufWPrintf(string[curstring], len, L"%d", val);
	return string[curstring];
}

const char *NumAsString(int val)
{
	const int BufLen = 16;
	const int NumBuffers = 4;

	static char string[NumBuffers][BufLen];
	static int curstring = 0;

	int len = 16;

	curstring = (curstring + 1) % NumBuffers;
	BufPrintf(string[curstring], len, "%d", val);

	return string[curstring];
}

// Returns the token parsed by SharedParse()
char *SharedGetToken()
{
	return s_shared_token;
}

// Returns the token parsed by SharedParse()
NOXREF void SharedSetQuoteChar(char c)
{
	s_shared_quote = c;
}

// Parse a token out of a string
const char *SharedParse(const char *data)
{
	int c;
	int len;

	len = 0;
	s_shared_token[0] = '\0';

	if (!data)
		return nullptr;

// skip whitespace
skipwhite:
	while ((c = *data) <= ' ')
	{
		if (c == 0)
		{
			// end of file;
			return nullptr;
		}

		data++;
	}

	// skip // comments till the next line
	if (c == '/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;

		// start over new line
		goto skipwhite;
	}

	// handle quoted strings specially: copy till the end or another quote
	if (c == s_shared_quote)
	{
		// skip starting quote
		data++;

		while (true)
		{
			// get char and advance
			c = *data++;
			if (c == s_shared_quote || !c)
			{
				s_shared_token[len] = '\0';
				return data;
			}

			s_shared_token[len++] = c;
		}
	}

	// parse single characters
	if (c == '{' || c == '}'|| c == ')'|| c == '(' || c == '\'' || c == ',')
	{
		s_shared_token[len++] = c;
		s_shared_token[len] = '\0';
		return data + 1;
	}

	// parse a regular word
	do
	{
		s_shared_token[len] = c;
		data++;
		len++;
		c = *data;

		if (c == '{' || c == '}'|| c == ')'|| c == '(' || c == '\'' || c == ',')
			break;

	}
	while (c > 32);

	s_shared_token[len] = '\0';
	return data;
}

// Returns true if additional data is waiting to be processed on this line
bool SharedTokenWaiting(const char *buffer)
{
	const char *p;

	p = buffer;
	while (*p && *p != '\n')
	{
		if (!isspace(*p) || isalnum(*p))
			return true;

		p++;
	}

	return false;
}

unsigned int seed_table[256] =
{
	28985U, 27138U, 26457U, 9451U, 17764U, 10909U, 28790U, 8716U, 6361U, 4853U, 17798U, 21977U, 19643U, 20662U, 10834U, 20103,
	27067U, 28634U, 18623U, 25849U, 8576U, 26234U, 23887U, 18228U, 32587U, 4836U, 3306U, 1811U, 3035U, 24559U, 18399U, 315,
	26766U, 907U, 24102U, 12370U, 9674U, 2972U, 10472U, 16492U, 22683U, 11529U, 27968U, 30406U, 13213U, 2319U, 23620U, 16823,
	10013U, 23772U, 21567U, 1251U, 19579U, 20313U, 18241U, 30130U, 8402U, 20807U, 27354U, 7169U, 21211U, 17293U, 5410U, 19223,
	10255U, 22480U, 27388U, 9946U, 15628U, 24389U, 17308U, 2370U, 9530U, 31683U, 25927U, 23567U, 11694U, 26397U, 32602U, 15031,
	18255U, 17582U, 1422U, 28835U, 23607U, 12597U, 20602U, 10138U, 5212U, 1252U, 10074U, 23166U, 19823U, 31667U, 5902U, 24630,
	18948U, 14330U, 14950U, 8939U, 23540U, 21311U, 22428U, 22391U, 3583U, 29004U, 30498U, 18714U, 4278U, 2437U, 22430U, 3439,
	28313U, 23161U, 25396U, 13471U, 19324U, 15287U, 2563U, 18901U, 13103U, 16867U, 9714U, 14322U, 15197U, 26889U, 19372U, 26241,
	31925U, 14640U, 11497U, 8941U, 10056U, 6451U, 28656U, 10737U, 13874U, 17356U, 8281U, 25937U, 1661U, 4850U, 7448U, 12744,
	21826U, 5477U, 10167U, 16705U, 26897U, 8839U, 30947U, 27978U, 27283U, 24685U, 32298U, 3525U, 12398U, 28726U, 9475U, 10208,
	617U, 13467U, 22287U, 2376U, 6097U, 26312U, 2974U, 9114U, 21787U, 28010U, 4725U, 15387U, 3274U, 10762U, 31695U, 17320,
	18324U, 12441U, 16801U, 27376U, 22464U, 7500U, 5666U, 18144U, 15314U, 31914U, 31627U, 6495U, 5226U, 31203U, 2331U, 4668,
	12650U, 18275U, 351U, 7268U, 31319U, 30119U, 7600U, 2905U, 13826U, 11343U, 13053U, 15583U, 30055U, 31093U, 5067U, 761,
	9685U, 11070U, 21369U, 27155U, 3663U, 26542U, 20169U, 12161U, 15411U, 30401U, 7580U, 31784U, 8985U, 29367U, 20989U, 14203,
	29694U, 21167U, 10337U, 1706U, 28578U, 887U, 3373U, 19477U, 14382U, 675U, 7033U, 15111U, 26138U, 12252U, 30996U, 21409,
	25678U, 18555U, 13256U, 23316U, 22407U, 16727U, 991U, 9236U, 5373U, 29402U, 6117U, 15241U, 27715U, 19291U, 19888U, 19847U
};

unsigned int glSeed = 0;

float UTIL_WeaponTimeBase()
{
	return 0.0f;	// always return 0.0 at client.
}

unsigned int U_Random()
{
	glSeed *= 69069;
	glSeed += seed_table[glSeed & 0xFF] + 1;
	return (glSeed & 0xFFFFFFF);
}

void U_Srand(unsigned int seed)
{
	glSeed = seed_table[seed & 0xFF];
}

int UTIL_SharedRandomLong(unsigned int seed, int low, int high)
{
	unsigned int range = high - low + 1;
	U_Srand((unsigned int)(high + low + seed));
	if (range != 1)
	{
		int rnum = U_Random();
		int offset = rnum % range;
		return (low + offset);
	}

	return low;
}

float UTIL_SharedRandomFloat(unsigned int seed, float low, float high)
{
	unsigned int range = high - low;
	U_Srand((unsigned int)seed + *(unsigned int*)&low + *(unsigned int*)&high);

	U_Random();
	U_Random();

	if (range)
	{
		int tensixrand = U_Random() & 0xFFFFu;
		float offset = float(tensixrand) / 0x10000u;
		return (low + offset * range);
	}

	return low;
}

int CalcBody(BodyEnumInfo_t* info, int count)
{
	int		body = 0;
	int		base;
	bool	valid;

	if (count <= 0)
		return 0;

	do
	{
		valid = true;

		for (int i = 0; i < count; i++)
		{
			if (i)
				base *= info[i - 1].nummodels;
			else
				base = 1;

			if (body / base % info[i].nummodels != info[i].body)
			{
				valid = false;
				break;
			}
		}

		if (valid)
			return body;

		body++;
	} while (body <= 2147483647);	// originally: 255

	return 0;
}

template<size_t N>
int CalcBody(BodyEnumInfo_t (&info)[N])
{
	int		body = 0;
	int		base;
	bool	valid;

	if (N <= 0)
		return 0;

	do
	{
		valid = true;

		for (int i = 0; i < N; i++)
		{
			if (i)
				base *= info[i - 1].nummodels;
			else
				base = 1;

			if (body / base % info[i].nummodels != info[i].body)
			{
				valid = false;
				break;
			}
		}

		if (valid)
			return body;

		body++;
	}
	while (body <= 2147483647);	// originally: 255

	return 0;
}

void NORETURN Sys_Error(const char* fmt, ...)
{
	static char output[1024];
	va_list ptr;

	va_start(ptr, fmt);
	vsnprintf(output, charsmax(output), fmt, ptr);
	va_end(ptr);

#ifndef _DEBUG
	// In official release, there is no way to tell users tracing anything.
	// Just give a hint window.
	FILE* hFile = fopen("leadermode_error.txt", "w");
	if (hFile)
	{
		fprintf(hFile, "%s\n", output);
		fclose(hFile);
	}

	HWND hwnd = GetActiveWindow();
	MessageBox(hwnd, output, "Fatal Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
#else
	// However, in debug mode, you can trace the error via access violation.
	//TerminateProcess(GetCurrentProcess(), 1);
	* (int*)(nullptr) = 0;
#endif

	_exit(-1);
}
