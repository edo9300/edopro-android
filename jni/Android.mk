LOCAL_PATH := $(call my-dir)/..

include $(CLEAR_VARS)
LOCAL_MODULE := EDOProClient

LOCAL_CFLAGS := -DYGOPRO_USE_SDL_MIXER -pipe -fno-rtti -fno-exceptions -fstrict-aliasing -D_ANDROID -fPIC -std=c++14 -DYGOPRO_BUILD_DLL -Wc++14-extensions
LOCAL_CPPFLAGS := -std=c++14 -Wc++14-extensions

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

ifneq ($(PICS_URL),)
LOCAL_CFLAGS += -DDEFAULT_PIC_URL=\"$(PICS_URL)\"
endif

ifneq ($(FIELDS_URL),)
LOCAL_CFLAGS += -DDEFAULT_FIELD_URL=\"$(FIELDS_URL)\"
endif

ifneq ($(COVERS_URL),)
LOCAL_CFLAGS += -DDEFAULT_COVER_URL=\"$(COVERS_URL)\"
endif

CLASSES_PATH := $(LOCAL_PATH)/deps
GFRAME_PATH := $(CLASSES_PATH)/gframe
LOCAL_C_INCLUDES := $(CLASSES_PATH)
LOCAL_C_INCLUDES += $(CLASSES_PATH)/irrlicht/include
LOCAL_C_INCLUDES += $(CLASSES_PATH)/freetype/include
LOCAL_C_INCLUDES += $(CLASSES_PATH)/sqlite3/include
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

LOCAL_SRC_FILES := $(GFRAME_PATH)/Android/COSAndroidOperator.cpp \
				$(GFRAME_PATH)/Android/porting_android.cpp \
				$(GFRAME_PATH)/CGUICustomCheckBox/CGUICustomCheckBox.cpp \
				$(GFRAME_PATH)/CGUICustomContextMenu/CGUICustomContextMenu.cpp \
				$(GFRAME_PATH)/CGUICustomComboBox/CGUICustomComboBox.cpp \
				$(GFRAME_PATH)/CGUICustomContextMenu/CGUICustomMenu.cpp \
				$(GFRAME_PATH)/CGUICustomTabControl/CGUICustomTabControl.cpp \
				$(GFRAME_PATH)/CGUICustomTable/CGUICustomTable.cpp \
				$(GFRAME_PATH)/CGUICustomText/CGUICustomText.cpp \
				$(GFRAME_PATH)/CGUIFileSelectListBox/CGUIFileSelectListBox.cpp \
				$(GFRAME_PATH)/CGUIImageButton/CGUIImageButton.cpp \
				$(GFRAME_PATH)/CGUISkinSystem/CConfigMap.cpp \
				$(GFRAME_PATH)/CGUISkinSystem/CGUIProgressBar.cpp \
				$(GFRAME_PATH)/CGUISkinSystem/CGUISkinSystem.cpp \
				$(GFRAME_PATH)/CGUISkinSystem/CImageGUISkin.cpp \
				$(GFRAME_PATH)/CGUITTFont/CGUITTFont.cpp \
				$(GFRAME_PATH)/CProgressBar/CProgressBar.cpp \
				$(GFRAME_PATH)/CXMLRegistry/CXMLNode.cpp \
				$(GFRAME_PATH)/CXMLRegistry/CXMLRegistry.cpp \
				$(GFRAME_PATH)/ResizeablePanel/ResizeablePanel.cpp \
				$(GFRAME_PATH)/client_card.cpp \
				$(GFRAME_PATH)/client_field.cpp \
				$(GFRAME_PATH)/client_updater.cpp \
				$(GFRAME_PATH)/core_utils.cpp \
				$(GFRAME_PATH)/custom_skin_enum.cpp \
				$(GFRAME_PATH)/data_manager.cpp \
				$(GFRAME_PATH)/data_handler.cpp \
				$(GFRAME_PATH)/deck_con.cpp \
				$(GFRAME_PATH)/deck_manager.cpp \
				$(GFRAME_PATH)/discord_wrapper.cpp \
				$(GFRAME_PATH)/dllinterface.cpp \
				$(GFRAME_PATH)/drawing.cpp \
				$(GFRAME_PATH)/duelclient.cpp \
				$(GFRAME_PATH)/event_handler.cpp \
				$(GFRAME_PATH)/game.cpp \
				$(GFRAME_PATH)/game_config.cpp \
				$(GFRAME_PATH)/generic_duel.cpp \
				$(GFRAME_PATH)/gframe.cpp \
				$(GFRAME_PATH)/image_downloader.cpp \
				$(GFRAME_PATH)/image_manager.cpp \
				$(GFRAME_PATH)/logging.cpp \
				$(GFRAME_PATH)/materials.cpp \
				$(GFRAME_PATH)/menu_handler.cpp \
				$(GFRAME_PATH)/netserver.cpp \
				$(GFRAME_PATH)/old_replay_mode.cpp \
				$(GFRAME_PATH)/readonlymemvfs.cpp \
				$(GFRAME_PATH)/replay_mode.cpp \
				$(GFRAME_PATH)/repo_manager.cpp \
				$(GFRAME_PATH)/replay.cpp \
				$(GFRAME_PATH)/server_lobby.cpp \
				$(GFRAME_PATH)/settings_window.cpp \
				$(GFRAME_PATH)/single_mode.cpp \
				$(GFRAME_PATH)/sound_manager.cpp \
				$(GFRAME_PATH)/sound_sdlmixer.cpp \
				$(GFRAME_PATH)/utils.cpp \
				$(GFRAME_PATH)/utils_gui.cpp \
				$(GFRAME_PATH)/windbot.cpp \
				$(GFRAME_PATH)/windbot_panel.cpp 

LOCAL_LDLIBS := -lEGL -llog -lGLESv1_CM -lGLESv2 -landroid -lOpenSLES

LOCAL_STATIC_LIBRARIES := Irrlicht
LOCAL_STATIC_LIBRARIES += android_native_app_glue
LOCAL_STATIC_LIBRARIES += libevent2
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
$(call import-module,irrlicht)
$(call import-module,libevent)
$(call import-module,sqlite3)
$(call import-module,freetype)
$(call import-module,gframe/lzma)
$(call import-module,fmt)
$(call import-module,libgit2)
$(call import-module,curl)
$(call import-module,SDL2)
$(call import-module,SDL_mixer)

$(call import-module,android/native_app_glue)
