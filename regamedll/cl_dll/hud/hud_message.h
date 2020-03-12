/*

Created Date: Mar 11 2020

*/

#pragma once

#define MESSAGE_ARG_LEN		64
#define MAX_MESSAGE_ARGS	4

constexpr int maxHUDMessages = 16;

typedef struct
{
	client_textmessage_t* pMessage;
	unsigned int font;
	wchar_t args[MAX_MESSAGE_ARGS][MESSAGE_ARG_LEN];
	int numArgs;
	int hintMessage;
}
client_message_t;

struct message_parms_t
{
	client_textmessage_t* pMessage;
	float time;
	int x, y;
	int totalWidth, totalHeight;
	int width;
	int lines;
	int lineLength;
	int length;
	int r, g, b;
	int text;
	int fadeBlend;
	float charTime;
	float fadeTime;
};

class CHudMessage : public CBaseHUDElement
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Reset(void);

public:
	void MsgFunc_HudText(const char* pString, BOOL& hintMessage);
	void MsgFunc_HudTextPro(const char* pString, BOOL& hintMessage);
	void MsgFunc_HudTextArgs(int iSize, void* pbuf);
	void MsgFunc_GameTitle(void);

public:
	float FadeBlend(float fadein, float fadeout, float hold, float localTime);
	int XPosition(float x, int width, int lineWidth);
	int YPosition(float y, int height);

public:
	void MessageAdd(client_textmessage_t* newMessage);
	int MessageAdd(const char* pName, float time, int hintMessage, unsigned int font);
	void MessageScanNextChar(unsigned int font);
	void MessageScanStart(void);
	void MessageDrawScan(client_message_t* pClientMessage, float time, unsigned int font);

private:
	client_message_t m_pMessages[maxHUDMessages];
	float m_startTime[maxHUDMessages];
	message_parms_t m_parms;
	float m_gameTitleTime;
	client_textmessage_t* m_pGameTitle;

	int m_HUD_title_life;
	int m_HUD_title_half;
};
