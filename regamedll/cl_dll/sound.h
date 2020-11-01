/*

Remastered Date: Mar 27 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#pragma once

extern double g_flClientTime;
extern double g_flClientTimeDelta;

constexpr float SND_DISTANCEFACTOR = 39.3701f;          // Units per meter. I.e feet would = 3.28. centimeters would = 100. inch would 39.3701f.

void Sound_Init();
void Sound_Think(double flDeltaTime);
void Sound_Exit();

void PlaySound(const char* szSound, int iPitch = 100);
void Play3DSound(const char* szSound, float flMinDist, float flMaxDist, const Vector& vecOrigin, int iPitch = 100);
