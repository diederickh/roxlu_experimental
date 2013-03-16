#ifndef ROXLU_PLATFORMH
#define ROXLU_PLATFORMH

// Info on predefs http://sourceforge.net/apps/mediawiki/predef/index.php?title=Operating_Systems

// Systems
#define ROXLU_WINDOWS 1
#define ROXLU_APPLE 2
#define ROXLU_IOS 3
#define ROXLU_LINUX 4

// Creative libs/GL-helpers, used for e.g. data paths
#define ROXLU_GLFW 1
#define ROXLU_OPENFRAMEWORKS 2
#define ROXLU_CINDER 3
#define ROXLU_COCOA 4


// Detect system
#ifdef _WIN32
#define ROXLU_PLATFORM ROXLU_WINDOWS
#elif __linux__
#define ROXLU_PLATFORM ROXLU_LINUX
#elif __APPLE__
 #if TARGET_OS_IPHONE    
   #define ROXLU_PLATFORM  ROXLU_IOS
 #else
   #define ROXLU_PLATFORM  ROXLU_APPLE
 #endif
#else
#error Unsupported operating system
#endif


// Include platform specifics
#if ROXLU_PLATFORM == ROXLU_WINDOWS
#include <roxlu/core/platform/windows/WindowsPlatform.h>
#elif ROXLU_PLATFORM == ROXLU_APPLE || ROXLU_PLATFORM == ROXLU_IOS
#include <roxlu/core/platform/osx/OSXPlatform.h>
#elif ROXLU_PLATFORM == ROXLU_LINUX
#include <roxlu/core/platform/linux/LinuxPlatform.h>
#endif

#endif
