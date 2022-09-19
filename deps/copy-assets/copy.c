#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <android/log.h>
#include <sys/stat.h>
#include <stdint.h>
#include <android/asset_manager_jni.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_DEBUG, "EDOPro copy assets", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "EDOPro copy assets", __VA_ARGS__);

#define JPARAMS(...)  "(" __VA_ARGS__ ")"
#define JARRAY(...) "[" __VA_ARGS__
#define JSTRING "Ljava/lang/String;"
#define JINT "I"
#define JVOID "V"
#define JBYTE "B"
#define JBOOL "Z"

#if defined(__cplusplus)
#define CALL_ENV(env,function,...) env->function(__VA_ARGS__)
#define EXPORT extern "C" JNIEXPORT
#else
#define CALL_ENV(env,function,...) (*env)->function(env,__VA_ARGS__)
#define EXPORT JNIEXPORT
#endif

static char* JstringToString(JNIEnv* env, const jstring jnistring) {
	if(!jnistring)
		return NULL;

	const jclass stringClass = CALL_ENV(env, GetObjectClass, jnistring);
	const jmethodID getBytes = CALL_ENV(env, GetMethodID, stringClass, "getBytes", JPARAMS(JSTRING)JARRAY(JBYTE));
	jstring UTF8_STRING = CALL_ENV(env, NewStringUTF, "UTF-8");
	const jbyteArray stringJbytes = (jbyteArray)(CALL_ENV(env, CallObjectMethod, jnistring, getBytes, UTF8_STRING));

	size_t length = (size_t)CALL_ENV(env, GetArrayLength, stringJbytes);
	jbyte* pBytes = CALL_ENV(env, GetByteArrayElements, stringJbytes, NULL);

	char* ret = (char*)malloc(length + 1);
	if(ret != NULL) {
		memcpy(ret, pBytes, length);
		ret[length] = 0;
	}
	
	CALL_ENV(env, ReleaseByteArrayElements, stringJbytes, pBytes, JNI_ABORT);

	CALL_ENV(env, DeleteLocalRef, stringJbytes);
	CALL_ENV(env, DeleteLocalRef, stringClass);
	CALL_ENV(env, DeleteLocalRef, UTF8_STRING);
	return ret;
}
static jboolean FileExists(const char* file) {
	struct stat sb;
	return stat(file, &sb) != -1 && S_ISREG(sb.st_mode) != 0;
}
EXPORT jboolean JNICALL Java_io_github_edo9300_edopro_AssetCopy_makeDirectory(JNIEnv* env, jclass thiz, jstring path) {
	(void)thiz;
	char* dir_path = JstringToString(env, path);
	if(!dir_path)
		return JNI_FALSE;
	jboolean ret = mkdir(dir_path, 0777) == 0 || errno == EEXIST;
	if(!ret)
		LOGE("failed to create folder: %s\n", strerror(errno));
	free(dir_path);
	return ret;
}
EXPORT jboolean JNICALL Java_io_github_edo9300_edopro_AssetCopy_fileDelete(JNIEnv* env, jclass thiz, jstring file) {
	(void)thiz;
	char* filename = JstringToString(env, file);
	if(!filename)
		return JNI_FALSE;
	if(!FileExists(filename)) {
		free(filename);
		return JNI_TRUE;
	}
	jboolean ret = remove(filename) == 0;
	if(!ret)
		LOGE("failed to delete file: %s\n", strerror(errno));
	free(filename);
	return ret;
}
EXPORT jboolean JNICALL Java_io_github_edo9300_edopro_AssetCopy_copyAssetToDestination(JNIEnv* env, jclass thiz, jobject assetManager, jstring source, jstring destination) {
	(void)thiz;
	char* source_asset = JstringToString(env, source);
	if(!source_asset)
		return JNI_FALSE;
	char* out_filename = JstringToString(env, destination);
	if(!out_filename) {
		free(source_asset);
		return JNI_FALSE;
	}
	AAsset* asset = AAssetManager_open(AAssetManager_fromJava(env, assetManager), source_asset, AASSET_MODE_STREAMING);
	free(source_asset);
	FILE* out = fopen(out_filename, "wb");
	if(out == NULL) {
		LOGE("fopen failed open file for writing: %s (%s)\n", out_filename, strerror(errno));
		AAsset_close(asset);
		free(out_filename);
		return JNI_FALSE;
	}
	uint8_t buffer[1024];
	off_t nread;
	jboolean ret = JNI_TRUE;
	while((nread = AAsset_read(asset, buffer, sizeof(buffer))) > 0) {
		const uint8_t* out_ptr = buffer;
		do {
			size_t nwritten = fwrite(out_ptr, 1, (size_t)nread, out);
			if(nwritten != 0) {
				nread -= nwritten;
				out_ptr += nwritten;
			} else if(errno != EINTR) {
				LOGE("failed to write output file %s: %s\n", out_filename, strerror(errno));
				ret = JNI_FALSE;
				goto on_error;
			}
		} while(nread > 0);
	}
	on_error:
	fclose(out);
	AAsset_close(asset);
	free(out_filename);
	return ret;
}
