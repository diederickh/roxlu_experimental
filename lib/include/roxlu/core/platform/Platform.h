#ifndef ROXLU_PLATFORMH
#define ROXLU_PLATFORMH

// Info on predefs http://sourceforge.net/apps/mediawiki/predef/index.php?title=Operating_Systems

// Systems
#define ROXLU_WINDOWS 	1
#define ROXLU_APPLE	2

// Detect system
#ifdef _MSC_VER  
	#define ROXLU_PLATFORM	ROXLU_WINDOWS
#elif defined(_WIN32)
	#define ROXLU_PLATFORM	ROXLU_WINDOWS
#elif defined(__APPLE__)
	#define ROXLU_PLATFORM	ROXLU_APPLE
#else
	#error Unsupported operating system
#endif


// Include platform specifics
#if ROXLU_PLATFORM == ROXLU_WINDOWS
	#include <roxlu/core/platform/windows/WindowsPlatform.h>
#elif ROXLU_PLATFORM == ROXLU_APPLE
	#include <roxlu/core/platform/osx/OSXPlatform.h>
#endif


#endif
