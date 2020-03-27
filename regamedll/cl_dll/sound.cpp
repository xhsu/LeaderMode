/*

Remastered Date: Mar 27 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#include "precompiled.h"
#include <map>

#include <Mmsystem.h>
#include <mciapi.h>

#pragma comment(lib, "Winmm.lib")

void Sound_Init()
{
}

bool PlaySoundViaWinmm(const char* pszFilePath)
{
	FileHandle_t fp;
	int		size;
	void* data;

	fp = FILE_SYSTEM->Open(pszFilePath, "rb");
	if (!fp)
		return false;

	FILE_SYSTEM->Seek(fp, 0, FILESYSTEM_SEEK_TAIL);
	size = FILE_SYSTEM->Tell(fp);
	FILE_SYSTEM->Seek(fp, 0, FILESYSTEM_SEEK_HEAD);

	data = malloc(size);
	if (!data)
	{
		FILE_SYSTEM->Close(fp);
		return false;
	}

	if (!FILE_SYSTEM->Read(data, size, fp))
	{
		free(data);
		FILE_SYSTEM->Close(fp);
		return false;
	}

	FILE_SYSTEM->Close(fp);

	// play the sound by microsoft.
	PlaySound((char*)data, nullptr, SND_MEMORY | SND_ASYNC);

	free(data);
	return true;
}
