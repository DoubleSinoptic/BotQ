#if defined(_WIN32) || defined(_WIN64)
#	define BOTQEXPORT __declspec(dllexport)
#else
#	define BOTQEXPORT
#endif

extern "C" BOTQEXPORT const char* __cdecl GetSharedInformation()
{
	return "BotQGraphics, " __DATE__ ", " __TIME__;
}