/*
* This file suppose included after all entity class is declared.
* Aug 21 2021.
*/

#ifndef _SERVER_UTILS_HPP_
#define _SERVER_UTILS_HPP_
#pragma once

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






































#endif	// _SERVER_UTILS_HPP_