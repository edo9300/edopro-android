#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef _WIN32

#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#ifdef _MSC_VER
#define mywcsncasecmp _wcsnicmp
#define mystrncasecmp _strnicmp
#else
#define mywcsncasecmp wcsncasecmp
#define mystrncasecmp strncasecmp
#endif

#define socklen_t int

#else //_WIN32
#ifdef _IRR_ANDROID_PLATFORM_
#include <android_native_app_glue.h>
#include <android/android_tools.h>
#endif
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <locale.h>

#define SD_BOTH 2
#define SOCKET int
#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKADDR_IN sockaddr_in
#define SOCKADDR sockaddr
#define SOCKET_ERRNO() (errno)

#include <cwchar>
#define mywcsncasecmp wcsncasecmp
#define mystrncasecmp strncasecmp
inline int _wtoi(const wchar_t * s) {
	wchar_t * endptr;
	return (int)wcstol(s, &endptr, 10);
}
#endif

#ifndef TEXT
#ifdef UNICODE
#define TEXT(x) L##x
#else
#define TEXT(x) x
#endif // UNICODE
#endif


#include <irrlicht.h>
#include <cstdlib>
#ifdef YGOPRO_USE_IRRKLANG
#include <irrKlang.h>
#endif
#ifdef _IRR_ANDROID_PLATFORM_
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES/glplatform.h>
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory.h>
#include <ctime>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#ifdef _IRR_ANDROID_PLATFORM_
#include <android/bufferio_android.h>
#else
#include "bufferio.h"
#endif
#include <mutex>
#include "mysignal.h"
#include <thread>
#include <common.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include "utils.h"
#ifndef YGOPRO_BUILD_DLL
#include <ocgapi.h>
#else
#include "dllinterface.h"
#endif
#ifdef _IRR_ANDROID_PLATFORM_
#include <android/CustomShaderConstantSetCallBack.h>
#endif

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

extern unsigned short PRO_VERSION;
extern int enable_log;
extern bool exit_on_return;
extern bool open_file;
extern path_string open_file_name;

#ifdef _IRR_ANDROID_PLATFORM_
#define MATERIAL_GUARD(f) do {mainGame->driver->enableMaterial2D(true); f; mainGame->driver->enableMaterial2D(false);} while(false);
#else
#define MATERIAL_GUARD(f) do {f;} while(false);
#endif


#endif
