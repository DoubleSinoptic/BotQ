#include "Log.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <stdexcept>

const char* GetTimeString() 
{
	static char formedtime[sizeof "[30.30.2000 30:60:10]"];
	time_t t = time(NULL);
	tm* tms = localtime(&t);
	sprintf(formedtime, "[%d.%d.%d %d:%d:%d] ", tms->tm_mday + 1, tms->tm_mon + 1, tms->tm_year + 1900, tms->tm_hour, tms->tm_min, tms->tm_sec);
	return formedtime;
}

void Log(const char * format, ...)
{
	va_list vl;
	va_start(vl, format);
	printf(GetTimeString());
	vprintf(format, vl);
	putchar('\n');
	va_end(vl);
    fflush(stdout);

}

void LogRewrite(const char * format, ...)
{
	va_list vl;
	va_start(vl, format);
	putchar('\r');
	printf(GetTimeString());
	vprintf(format, vl);
	va_end(vl);
    fflush(stdout);
}

void Warrning(const char * format, ...)
{
	va_list vl;
	va_start(vl, format);
	printf(GetTimeString());
	printf("Warrning: ");
	vprintf(format, vl);
	putchar('\n');
	va_end(vl);
    fflush(stdout);
}
