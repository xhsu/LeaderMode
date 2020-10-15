/*

Remastered Date: Mar 27 2020

Modern Warfare Dev Team
Code - Luna the Reborn

*/

#pragma once

constexpr float SND_DISTANCEFACTOR = 3.28f;          // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.

void Sound_Init();
void Sound_Think(double flTime);
void Sound_Exit();

void PlaySound(const char* szSound);
void Play3DSound(const char* szSound, float flMinDist, float flMaxDist, const Vector& vecOrigin);
