LOCAL_PATH:= $(call my-dir)

local_c_includes := \
	$(LOCAL_PATH)/ \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../../openssl/include
	
local_src_files:= \
	agent.c \
	bcrypt_pbkdf.c \
	blowfish.c \
	channel.c \
	comp.c \
	crypt.c \
	global.c \
	hostkey.c \
	keepalive.c \
	kex.c \
	knownhost.c \
	mac.c \
	misc.c \
	packet.c \
	pem.c \
	publickey.c \
	scp.c \
	session.c \
	sftp.c \
	transport.c \
	userauth.c \
	version.c \
	openssl.c


include $(CLEAR_VARS)

LOCAL_MODULE := ssh2
LOCAL_MODULE_FILENAME := libssh2
LOCAL_STATIC_LIBRARIES := libssl_static
LOCAL_CFLAGS := -DLIBSSH2_OPENSSL -DOPENSSL_NO_ENGINE
LOCAL_SRC_FILES += $(local_src_files)
LOCAL_C_INCLUDES += $(local_c_includes)

include $(BUILD_STATIC_LIBRARY)
