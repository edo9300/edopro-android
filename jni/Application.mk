APP_ABI := armeabi-v7a arm64-v8a #x86 x86_64
APP_PLATFORM := android-16
APP_STL := c++_static
APP_CPPFLAGS := -std=c++14 -Wc++14-extensions -fno-rtti -fexceptions
APP_CFLAGS := -Wno-error=format-security -fpermissive -fPIC -pipe -fno-stack-protector
APP_OPTIM := release
