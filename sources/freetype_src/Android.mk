# this is now the default FreeType build for Android
#
ifndef USE_FREETYPE
USE_FREETYPE := 2.10.0
endif

ifeq ($(USE_FREETYPE),2.10.0)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# compile in ARM mode, since the glyph loader/renderer is a hotspot
# when loading complex pages in the browser
#
#LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES:= \
    src/autofit/autofit.c \
    src/base/ftbase.c \
    src/base/ftbbox.c \
    src/base/ftbitmap.c \
    src/base/ftdebug.c \
    src/base/ftgasp.c \
    src/base/ftglyph.c \
    src/base/ftinit.c \
    src/base/ftlcdfil.c \
    src/base/ftmm.c \
    src/base/ftfntfmt.c \
    src/base/ftpatent.c \
    src/base/ftsynth.c \
    src/base/ftstroke.c \
    src/base/ftsystem.c \
    src/bdf/bdf.c \
    src/cff/cff.c \
    src/cid/type1cid.c \
    src/gzip/ftgzip.c \
    src/lzw/ftlzw.c \
    src/pcf/pcf.c \
    src/pfr/pfr.c \
    src/psaux/psaux.c \
    src/pshinter/pshinter.c \
    src/psnames/psnames.c \
    src/raster/raster.c \
    src/sfnt/sfnt.c \
    src/smooth/smooth.c \
    src/truetype/truetype.c \
    src/type1/type1.c \
    src/type42/type42.c \
    src/winfonts/winfnt.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/builds \
	$(LOCAL_PATH)/include

LOCAL_CFLAGS += -W -Wall
LOCAL_CFLAGS += -fPIC -DPIC
LOCAL_CFLAGS += -DFT2_BUILD_LIBRARY

LOCAL_CFLAGS += -O2

LOCAL_MODULE:= libft2

include $(BUILD_STATIC_LIBRARY)
endif
