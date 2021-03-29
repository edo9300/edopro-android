// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COSOperator.h"

#ifdef _IRR_WINDOWS_API_
#ifndef _IRR_XBOX_PLATFORM_
#include <windows.h>
#endif
#else
#include <string.h>
#include <unistd.h>
#ifndef _IRR_ANDROID_PLATFORM_
#include <sys/types.h>
#ifdef _IRR_OSX_PLATFORM_
#include <sys/sysctl.h>
#endif
#endif
#endif

#if defined(_IRR_COMPILE_WITH_X11_DEVICE_)
#include "CIrrDeviceLinux.h"
#endif
#if defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
#import <Cocoa/Cocoa.h>
#endif

#include "fast_atof.h"

namespace irr
{

#if defined(_IRR_COMPILE_WITH_X11_DEVICE_)
// constructor  linux
	COSOperator::COSOperator(const core::stringc& osVersion, CIrrDeviceLinux* device)
: OperatingSystem(osVersion), IrrDeviceLinux(device)
{
}
#endif

// constructor
COSOperator::COSOperator(const core::stringc& osVersion) : OperatingSystem(osVersion)
{
	#ifdef _DEBUG
	setDebugName("COSOperator");
	#endif
}


//! returns the current operating system version as string.
const core::stringc& COSOperator::getOperatingSystemVersion() const
{
	return OperatingSystem;
}

// UTF-16/UTF-32 to UTF-8
static int EncodeUTF8(char * dest, const wchar_t * src, int size) {
	char* pstr = dest;
	while(*src != 0 && (dest - pstr<size)) {
		if(*src < 0x80) {
			*dest = static_cast<char>(*src);
			++dest;
		} else if(*src < 0x800) {
			dest[0] = ((*src >> 6) & 0x1f) | 0xc0;
			dest[1] = ((*src) & 0x3f) | 0x80;
			dest += 2;
		} else if(*src < 0x10000 && (*src < 0xd800 || *src > 0xdfff)) {
			dest[0] = ((*src >> 12) & 0xf) | 0xe0;
			dest[1] = ((*src >> 6) & 0x3f) | 0x80;
			dest[2] = ((*src) & 0x3f) | 0x80;
			dest += 3;
		} else {
			if(sizeof(wchar_t) == 2) {
				unsigned unicode = 0;
				unicode |= (*src++ & 0x3ff) << 10;
				unicode |= *src & 0x3ff;
				unicode += 0x10000;
				dest[0] = ((unicode >> 18) & 0x7) | 0xf0;
				dest[1] = ((unicode >> 12) & 0x3f) | 0x80;
				dest[2] = ((unicode >> 6) & 0x3f) | 0x80;
				dest[3] = ((unicode) & 0x3f) | 0x80;
			} else {
				dest[0] = ((*src >> 18) & 0x7) | 0xf0;
				dest[1] = ((*src >> 12) & 0x3f) | 0x80;
				dest[2] = ((*src >> 6) & 0x3f) | 0x80;
				dest[3] = ((*src) & 0x3f) | 0x80;
			}
			dest += 4;
		}
		src++;
	}
	*dest = 0;
	return dest - pstr;
}
// UTF-8 to UTF-16/UTF-32
static int DecodeUTF8(wchar_t * dest, const char * src, int size) {
	const char* p = src;
	wchar_t* wp = dest;
	while(*p != 0 && (wp - dest<size)) {
		if((*p & 0x80) == 0) {
			*wp = *p;
			p++;
		} else if((*p & 0xe0) == 0xc0) {
			*wp = (((unsigned)p[0] & 0x1f) << 6) | ((unsigned)p[1] & 0x3f);
			p += 2;
		} else if((*p & 0xf0) == 0xe0) {
			*wp = (((unsigned)p[0] & 0xf) << 12) | (((unsigned)p[1] & 0x3f) << 6) | ((unsigned)p[2] & 0x3f);
			p += 3;
		} else if((*p & 0xf8) == 0xf0) {
			if(sizeof(wchar_t) == 2) {
				unsigned unicode = (((unsigned)p[0] & 0x7) << 18) | (((unsigned)p[1] & 0x3f) << 12) | (((unsigned)p[2] & 0x3f) << 6) | ((unsigned)p[3] & 0x3f);
				unicode -= 0x10000;
				*wp++ = (unicode >> 10) | 0xd800;
				*wp = (unicode & 0x3ff) | 0xdc00;
			} else {
				*wp = (((unsigned)p[0] & 0x7) << 18) | (((unsigned)p[1] & 0x3f) << 12) | (((unsigned)p[2] & 0x3f) << 6) | ((unsigned)p[3] & 0x3f);
			}
			p += 4;
		} else
			p++;
		wp++;
	}
	*wp = 0;
	return wp - dest;
}

//! copies text to the clipboard
void COSOperator::copyToClipboard(const wchar_t* _text) const
{
	if (wcslen(_text)==0)
		return;
#if !defined(_IRR_WCHAR_FILESYSTEM)
	size_t lenOld = wcslen(_text) * sizeof(wchar_t);
	char* text = new char[lenOld + 1];
	size_t len = EncodeUTF8(text, _text, lenOld);
	text[len] = 0;
#else
	const wchar_t* text = _text;
#endif

// Windows version
#if defined(_IRR_XBOX_PLATFORM_)
#elif defined(_IRR_WINDOWS_API_)
	if (!OpenClipboard(NULL) || _text == 0)
		return;

	EmptyClipboard();

	HGLOBAL clipbuffer;
#if defined(_IRR_WCHAR_FILESYSTEM)
	wchar_t * buffer;

	clipbuffer = GlobalAlloc(GMEM_DDESHARE, sizeof(wchar_t) * (wcslen(text) + 1));
	buffer = (wchar_t*)GlobalLock(clipbuffer);

	wcscpy(buffer, text);
#else
	char * buffer;

	clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(text) + 1);
	buffer = (char*)GlobalLock(clipbuffer);

	strcpy(buffer, text);
#endif

	GlobalUnlock(clipbuffer);
#if defined(_IRR_WCHAR_FILESYSTEM)
	SetClipboardData(CF_UNICODETEXT, clipbuffer);
#else
	SetClipboardData(CF_TEXT, clipbuffer);
#endif
	CloseClipboard();

// MacOSX version
#elif defined(_IRR_COMPILE_WITH_OSX_DEVICE_)

	NSString *str = nil;
    NSPasteboard *board = nil;

    if ((text != NULL) && (strlen(text) > 0))
    {
        str = [NSString stringWithUTF8String:text];
        board = [NSPasteboard generalPasteboard];
        [board declareTypes:[NSArray arrayWithObject:NSStringPboardType] owner:NSApp];
        [board setString:str forType:NSStringPboardType];
    }

#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
    if ( IrrDeviceLinux )
        IrrDeviceLinux->copyToClipboard(text);
#endif
#if !defined(_IRR_WCHAR_FILESYSTEM)
	if(text)
		delete[] text;
#endif
}


//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
const wchar_t* COSOperator::getTextFromClipboard() const {
	const wchar_t* buffer = 0;
#if !defined(_IRR_WCHAR_FILESYSTEM)
	static core::stringw wstring;
	const char * cbuffer = 0;
#endif
#if defined(_IRR_XBOX_PLATFORM_)
	return 0;
#elif defined(_IRR_WINDOWS_API_)
	if(!OpenClipboard(NULL))
		return 0;

#if defined(_IRR_WCHAR_FILESYSTEM)
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	buffer = (wchar_t*)GlobalLock(hData);
#else
	HANDLE hData = GetClipboardData(CF_TEXT);
	cbuffer = (char*)GlobalLock(hData);
#endif
	GlobalUnlock(hData);
	CloseClipboard();

#elif defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
	NSString* str = nil;
    NSPasteboard* board = nil;

    board = [NSPasteboard generalPasteboard];
    str = [board stringForType:NSStringPboardType];

    if (str != nil)
        cbuffer = (char*)[str UTF8String];

#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
	if(IrrDeviceLinux)
		cbuffer = IrrDeviceLinux->getTextFromClipboard();
#else
	return 0;
#endif
#if !defined(_IRR_WCHAR_FILESYSTEM)
	if(cbuffer) {
		size_t lenOld = strlen(cbuffer);
		wchar_t *ws = new wchar_t[lenOld + 1];
		size_t len = DecodeUTF8(ws, cbuffer, lenOld);
		ws[len] = 0;
		wstring = ws;
		delete[] ws;
		buffer = wstring.c_str();
	}
#endif
	return buffer;
}


bool COSOperator::getProcessorSpeedMHz(u32* MHz) const
{
	if (MHz)
		*MHz=0;
#if defined(_IRR_WINDOWS_API_) && !defined(_WIN32_WCE ) && !defined (_IRR_XBOX_PLATFORM_)
	LONG Error;

	HKEY Key;
	Error = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			__TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),
			0, KEY_READ, &Key);

	if(Error != ERROR_SUCCESS)
		return false;

	DWORD Speed = 0;
	DWORD Size = sizeof(Speed);
	Error = RegQueryValueEx(Key, __TEXT("~MHz"), NULL, NULL, (LPBYTE)&Speed, &Size);

	RegCloseKey(Key);

	if (Error != ERROR_SUCCESS)
		return false;
	else if (MHz)
		*MHz = Speed;
	return true;

#elif defined(_IRR_OSX_PLATFORM_)
	struct clockinfo CpuClock;
	size_t Size = sizeof(clockinfo);

	if (!sysctlbyname("kern.clockrate", &CpuClock, &Size, NULL, 0))
		return false;
	else if (MHz)
		*MHz = CpuClock.hz;
	return true;
#else
	// read from "/proc/cpuinfo"
	FILE* file = fopen("/proc/cpuinfo", "r");
	if (file)
	{
		char buffer[1024];
		size_t r = fread(buffer, 1, 1023, file);
		buffer[r] = 0;
		buffer[1023]=0;
		core::stringc str(buffer);
		s32 pos = str.find("cpu MHz");
		if (pos != -1)
		{
			pos = str.findNext(':', pos);
			if (pos != -1)
			{
				while ( str[++pos] == ' ' );
				*MHz = core::fast_atof(str.c_str()+pos);
			}
		}
		fclose(file);
	}
	return (MHz && *MHz != 0);
#endif
}

bool COSOperator::getSystemMemory(u32* Total, u32* Avail) const
{
#if defined(_IRR_WINDOWS_API_) && !defined (_IRR_XBOX_PLATFORM_)

    #if (_WIN32_WINNT >= 0x0500)
	MEMORYSTATUSEX MemoryStatusEx;
 	MemoryStatusEx.dwLength = sizeof(MEMORYSTATUSEX);

	// cannot fail
	GlobalMemoryStatusEx(&MemoryStatusEx);

	if (Total)
		*Total = (u32)(MemoryStatusEx.ullTotalPhys>>10);
	if (Avail)
		*Avail = (u32)(MemoryStatusEx.ullAvailPhys>>10);
	return true;
	#else
	MEMORYSTATUS MemoryStatus;
	MemoryStatus.dwLength = sizeof(MEMORYSTATUS);

 	// cannot fail
	GlobalMemoryStatus(&MemoryStatus);

 	if (Total)
		*Total = (u32)(MemoryStatus.dwTotalPhys>>10);
 	if (Avail)
		*Avail = (u32)(MemoryStatus.dwAvailPhys>>10);
    return true;
	#endif

#elif defined(_IRR_POSIX_API_) && !defined(__FreeBSD__)
#if defined(_SC_PHYS_PAGES) && defined(_SC_AVPHYS_PAGES)
        long ps = sysconf(_SC_PAGESIZE);
        long pp = sysconf(_SC_PHYS_PAGES);
        long ap = sysconf(_SC_AVPHYS_PAGES);

	if ((ps==-1)||(pp==-1)||(ap==-1))
		return false;

	if (Total)
		*Total = (u32)((ps*(long long)pp)>>10);
	if (Avail)
		*Avail = (u32)((ps*(long long)ap)>>10);
	return true;
#else
	// TODO: implement for non-availability of symbols/features
	return false;
#endif
#elif defined(_IRR_OSX_PLATFORM_)
	int mib[2];
	int64_t physical_memory;
	size_t length;

	// Get the Physical memory size
	mib[0] = CTL_HW;
	mib[1] = HW_MEMSIZE;
	length = sizeof(int64_t);
	sysctl(mib, 2, &physical_memory, &length, NULL, 0);
	return true;
#else
	// TODO: implement for others
	return false;
#endif
}


} // end namespace

