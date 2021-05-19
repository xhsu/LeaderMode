#include "precompiled.h"

CHintMessage::CHintMessage(const char *hintString, bool isHint, std::vector<const char *> *args, float duration)
{
	m_hintString = CloneString(hintString);
	m_duration = duration;
	m_isHint = isHint;

	if (args)
	{
		for (unsigned short i = 0; i < args->size(); i++)
			m_args.push_back(CloneString((*args)[i]));
	}
}

CHintMessage::~CHintMessage()
{
	// LunaTheReborn: these are the genuine equivalent of UtlVector::PurgeAndDeleteElements().
	auto iter = m_args.begin();
	while (iter != m_args.end())
	{
		// LunaTheReborn: free memorys allocated by CloneString().
		delete[](*iter);

		iter = m_args.erase(iter);
	}

	m_args.clear();

	if (m_hintString)
	{
		delete[] m_hintString;
		m_hintString = NULL;
	}
}

void CHintMessage::Send(CBaseEntity *client)
{
	UTIL_ShowMessageArgs(m_hintString, client, &m_args, m_isHint);
}

void CHintMessageQueue::Reset()
{
	m_tmMessageEnd = 0;

	// LunaTheReborn: these are the genuine equivalent of UtlVector::PurgeAndDeleteElements().
	auto iter = m_messages.begin();
	while (iter != m_messages.end())
	{
		// LunaTheReborn: free memorys allocated by CloneString().
		delete (*iter);

		iter = m_messages.erase(iter);
	}

	m_messages.clear();
}

void CHintMessageQueue::Update(CBaseEntity *client)
{
	if (gpGlobals->time <= m_tmMessageEnd)
		return;

	if (m_messages.empty())
		return;

	CHintMessage *msg = m_messages[0];
	m_tmMessageEnd = gpGlobals->time + msg->GetDuration();
	msg->Send(client);
	delete msg;
	m_messages.erase(m_messages.begin());	// pop_front
}

bool CHintMessageQueue::AddMessage(const char *message, float duration, bool isHint, std::vector<const char *> *args)
{
	CHintMessage *msg = new CHintMessage(message, isHint, args, duration);
	m_messages.push_back(msg);	// not emplace_back.

	return true;
}
