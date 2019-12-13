LOCAL_PATH := $(call my-dir)/..

include $(CLEAR_VARS)
LOCAL_MODULE := EdoproClient

LOCAL_CFLAGS := -D_IRR_ANDROID_PLATFORM_  -DYGOPRO_USE_SDL_MIXER -pipe -fno-rtti -fno-exceptions -fstrict-aliasing -D_ANDROID -fPIC -std=c++11
LOCAL_CPPFLAGS := -std=c++11 

ifndef NDEBUG
LOCAL_CFLAGS += -g -D_DEBUG 
else
LOCAL_CFLAGS += -fexpensive-optimizations -O3 
endif

ifeq ($(TARGET_ARCH_ABI),x86)
LOCAL_CFLAGS += -fno-stack-protector
endif

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
LOCAL_CFLAGS += -mno-unaligned-access
endif

CLASSES_PATH := $(LOCAL_PATH)/deps
GFRAME_PATH := $(CLASSES_PATH)/gframe
LOCAL_C_INCLUDES := $(CLASSES_PATH)
LOCAL_C_INCLUDES += $(CLASSES_PATH)/irrlicht/include
LOCAL_C_INCLUDES += $(CLASSES_PATH)/irrlicht/source/Irrlicht
LOCAL_C_INCLUDES += $(CLASSES_PATH)/irrlicht/source/Irrlicht/Android
LOCAL_C_INCLUDES += $(CLASSES_PATH)/freetype/include
LOCAL_C_INCLUDES += $(CLASSES_PATH)/sqlite3
LOCAL_C_INCLUDES += $(CLASSES_PATH)/libevent/include
LOCAL_C_INCLUDES += $(CLASSES_PATH)/ocgcore
LOCAL_C_INCLUDES += $(CLASSES_PATH)/fmt/include
LOCAL_C_INCLUDES += $(CLASSES_PATH)/nlohmannjson
LOCAL_C_INCLUDES += $(CLASSES_PATH)/libgit2/include
LOCAL_C_INCLUDES += $(CLASSES_PATH)/curl/include
LOCAL_C_INCLUDES += $(CLASSES_PATH)/curl/include
LOCAL_C_INCLUDES += $(GFRAME_PATH)/CGUICustomContextMenu
LOCAL_C_INCLUDES += $(GFRAME_PATH)/CGUICustomTabControl
LOCAL_C_INCLUDES += $(GFRAME_PATH)/CGUICustomText
LOCAL_C_INCLUDES += $(GFRAME_PATH)/CGUIFileSelectListBox
LOCAL_C_INCLUDES += $(GFRAME_PATH)/CGUIImageButton
LOCAL_C_INCLUDES += $(GFRAME_PATH)/CGUITTFont
LOCAL_C_INCLUDES += $(GFRAME_PATH)/CProgressBar
LOCAL_C_INCLUDES += $(GFRAME_PATH)/ResizeablePanel
LOCAL_C_INCLUDES += $(CLASSES_PATH)/SDL2/include
LOCAL_C_INCLUDES += $(CLASSES_PATH)/SDL2_mixer/include

LOCAL_SRC_FILES := $(CLASSES_PATH)/android/android_tools.cpp \
				$(CLASSES_PATH)/android/xstring.cpp \
				$(CLASSES_PATH)/android/TouchEventTransferAndroid.cpp \
				$(CLASSES_PATH)/android/CAndroidGUIEditBox.cpp \
				$(CLASSES_PATH)/android/CAndroidGUIComboBox.cpp \
				$(CLASSES_PATH)/android/CAndroidGUIListBox.cpp \
				$(CLASSES_PATH)/android/CAndroidGUISkin.cpp \
				$(CLASSES_PATH)/android/CustomShaderConstantSetCallBack.cpp \
				$(GFRAME_PATH)/CGUICustomContextMenu/CGUICustomContextMenu.cpp \
				$(GFRAME_PATH)/CGUICustomContextMenu/CGUICustomMenu.cpp \
				$(GFRAME_PATH)/CGUICustomTabControl/CGUICustomTabControl.cpp \
				$(GFRAME_PATH)/CGUICustomText/CGUICustomText.cpp \
				$(GFRAME_PATH)/CGUIFileSelectListBox/CGUIFileSelectListBox.cpp \
				$(GFRAME_PATH)/CGUIImageButton/CGUIImageButton.cpp \
				$(GFRAME_PATH)/CGUITTFont/CGUITTFont.cpp \
				$(GFRAME_PATH)/CProgressBar/CProgressBar.cpp \
				$(GFRAME_PATH)/ResizeablePanel/ResizeablePanel.cpp \
				$(GFRAME_PATH)/client_card.cpp \
				$(GFRAME_PATH)/CGUIButton.cpp \
				$(GFRAME_PATH)/CGUIComboBox.cpp \
				$(GFRAME_PATH)/CGUIEditBox.cpp \
				$(GFRAME_PATH)/client_field.cpp \
				$(GFRAME_PATH)/data_manager.cpp \
				$(GFRAME_PATH)/deck_con.cpp \
				$(GFRAME_PATH)/deck_manager.cpp \
				$(GFRAME_PATH)/dllinterface.cpp \
				$(GFRAME_PATH)/drawing.cpp \
				$(GFRAME_PATH)/duelclient.cpp \
				$(GFRAME_PATH)/event_handler.cpp \
				$(GFRAME_PATH)/game.cpp \
				$(GFRAME_PATH)/generic_duel.cpp \
				$(GFRAME_PATH)/gframe.cpp \
				$(GFRAME_PATH)/image_manager.cpp \
				$(GFRAME_PATH)/materials.cpp \
				$(GFRAME_PATH)/menu_handler.cpp \
				$(GFRAME_PATH)/netserver.cpp \
				$(GFRAME_PATH)/old_replay_mode.cpp \
				$(GFRAME_PATH)/porting_android.cpp \
				$(GFRAME_PATH)/repo_manager.cpp \
				$(GFRAME_PATH)/readonlymemvfs.cpp \
				$(GFRAME_PATH)/replay_mode.cpp \
				$(GFRAME_PATH)/replay.cpp \
				$(GFRAME_PATH)/single_mode.cpp \
				$(GFRAME_PATH)/sound_manager.cpp \
				$(GFRAME_PATH)/sound_sdlmixer.cpp \
				$(GFRAME_PATH)/utils.cpp 
# 				$(LOCAL_PATH)/jni/cn_garymb_ygomobile_core_IrrlichtBridge.cpp

LOCAL_LDLIBS := -lEGL -llog -lGLESv1_CM -lGLESv2 -landroid -lOpenSLES

LOCAL_STATIC_LIBRARIES := Irrlicht
LOCAL_STATIC_LIBRARIES += android_native_app_glue
LOCAL_STATIC_LIBRARIES += libssl_static
LOCAL_STATIC_LIBRARIES += libcrypto_static
LOCAL_STATIC_LIBRARIES += libevent2
LOCAL_STATIC_LIBRARIES += libocgcore_static
LOCAL_STATIC_LIBRARIES += liblua5.3
LOCAL_STATIC_LIBRARIES += clzma
LOCAL_STATIC_LIBRARIES += sqlite3
LOCAL_STATIC_LIBRARIES += libft2
LOCAL_STATIC_LIBRARIES += fmt_static
LOCAL_STATIC_LIBRARIES += git2
LOCAL_STATIC_LIBRARIES += curl
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_SHARED_LIBRARIES += SDL2_mixer

include $(BUILD_SHARED_LIBRARY)
$(call import-add-path,$(CLASSES_PATH))
$(call import-add-path,$(CLASSES_PATH)/irrlicht/source)
$(call import-add-path,$(CLASSES_PATH)/android)
$(call import-module,Irrlicht/Android/jni)
$(call import-module,openssl)
$(call import-module,libevent)
$(call import-module,sqlite3)
$(call import-module,ocgcore)
$(call import-module,lua)
$(call import-module,freetype)
$(call import-module,gframe/lzma)
$(call import-module,fmt)
$(call import-module,libgit2)
$(call import-module,libssh2)
$(call import-module,curl)
$(call import-module,SDL2)
$(call import-module,SDL_mixer)

$(call import-module,android/native_app_glue)


