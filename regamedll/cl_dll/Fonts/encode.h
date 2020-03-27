#pragma once

#ifndef ENCODE_H
#define ENCODE_H

wchar_t *UTF8ToUnicode(const char *str);
wchar_t *ANSIToUnicode(const char *str);
char *UnicodeToUTF8(const wchar_t *str);
char *UnicodeToANSI(const wchar_t *str);

#endif