LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := openal-soft
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libopenal.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := FLAC
LOCAL_STATIC_LIBRARIES := ogg
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libFLAC.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := ogg
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libogg.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := vorbisfile
LOCAL_STATIC_LIBRARIES := vorbis
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libvorbisfile.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := vorbis
LOCAL_STATIC_LIBRARIES := ogg
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libvorbis.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := sfAudio
LOCAL_STATIC_LIBRARIES := openal-soft ogg FLAC vorbisfile
LOCAL_MODULE_FILENAME := libsfaudio

LOCAL_SRC_FILES := src/ALCheck.cpp \
				   src/AlResource.cpp \
				   src/AudioDevice.cpp \
				   src/InputSoundFile.cpp \
				   src/Music.cpp \
				   src/Sound.cpp \
				   src/SoundBuffer.cpp \
				   src/SoundFileFactory.cpp \
				   src/SoundFileReaderFlac.cpp \
				   src/SoundFileReaderMp3_minimp3.cpp \
				   src/SoundFileReaderOgg.cpp \
				   src/SoundFileReaderWav.cpp \
				   src/SoundSource.cpp \
				   src/SoundStream.cpp \
				   src/System/FileInputStream.cpp \
				   src/System/MemoryInputStream.cpp \
				   src/System/Time.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
					$(LOCAL_PATH)/external/headers

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_EXPORT_LDLIBS := -lOpenSLES

LOCAL_CFLAGS += -std=c++14 -fexceptions -Wc++14-extensions

include $(BUILD_STATIC_LIBRARY)