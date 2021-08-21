#ifndef BISCUITOS_STRING_H
#define BISCUITOS_STRING_H

char tolower(char s1);
int strncmp(const char* str1, const char* str2, int n);
int strnlen_terminator(const char* str, int max, char terminator);
int istrncmp(const char* s1, const char* s2, int n);
int strlen (const char* ptr);
int strnlen (const char* ptr, int max);
char* strcpy(char* dest, const char* src );
char* strncpy ( char* dest, const char* src, int count);
int isdigit(char c);
int tonumericdigit(char c);
char* strtok(char* str, const char* delimiters);
#endif
