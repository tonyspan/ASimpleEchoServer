#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__NT__)
	#define PLATFORM_WINDOWS
	#ifdef _WIN64
	#else
		#error "x86 Builds are not supported!"
#endif
#elif __linux__
	#define PLATFORM_LINUX
#else
	#error "Unknown platform!"
#endif

#ifdef PLATFORM_WINDOWS
	#define DEBUG_BREAK __debugbreak()
#elif defined PLATFORM_LINUX
	#include <signal.h>
	#define DEBUG_BREAK raise(SIGTRAP)
#else
	#define DEBUG_BREAK
#endif