#if defined(_M_X64)
#define ARCHITECTURE "x64"
#elif defined(_M_IX86)
#define ARCHITECTURE "x86"
#elif defined(_M_ARM64)
#define ARCHITECTURE "ARM64"
#else
#error "Unsupported architecture (only x64, x86 and arm64 supported)"
#endif

#ifdef _DEBUG
#define CONFIGURATION "Debug"
#else
#define CONFIGURATION "Release"
#endif

#define STATIC_LIBRARY_EXTENSION    ".lib"
#define STATIC_LIBRARY_SUFFIX       "." ARCHITECTURE "." CONFIGURATION STATIC_LIBRARY_EXTENSION




#pragma comment(lib, "CoTigraphyLib" STATIC_LIBRARY_SUFFIX)
