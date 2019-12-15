// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_ANDROID_TOOLS_H__
#define __IRR_ANDROID_TOOLS_H__

#include <irrlicht.h>
#include <android_native_app_glue.h>
#include <signal.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "ygomobile-native", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "ygomobile-native", __VA_ARGS__))

namespace irr {
namespace android {

extern s32 handleInput(ANDROID_APP app, AInputEvent* androidEvent);

}
}

#endif // __IRR_ANDROID_TOOLS_H__
