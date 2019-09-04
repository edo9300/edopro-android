/*
Minetest
Copyright (C) 2014 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef __ANDROID__
#error This file may only be compiled for android!
#endif

#include "porting_android.h"

#include <sstream>
#include <exception>
#include <cstdlib>

extern int main(int argc, char *argv[]);

void android_main(android_app *app)
{
	int retval = 0;
	porting::app_global = app;

// 	Thread::setName("Main");

	try {
		app_dummy();
		main(0, nullptr);
	} catch (std::exception &e) {
// 		errorstream << "Uncaught exception in main thread: " << e.what() << std::endl;
		retval = -1;
	} catch (...) {
		// errorstream << "Uncaught exception in main thread!" << std::endl;
		retval = -1;
	}

	porting::cleanupAndroid();
	// infostream << "Shutting down." << std::endl;
	exit(retval);
}

/* handler for finished message box input */
/* Intentionally NOT in namespace porting */
/* TODO this doesn't work as expected, no idea why but there's a workaround   */
/* for it right now */
extern "C" {
	JNIEXPORT void JNICALL Java_net_minetest_EpNativeActivity_putMessageBoxResult(
			JNIEnv * env, jclass thiz, jstring text)
	{
		// errorstream << "Java_net_minetest_EpNativeActivity_putMessageBoxResult got: "
// 				<< std::string((const char*)env->GetStringChars(text,0))
// 				<< std::endl;
	}
}

namespace porting {

std::string path_storage = "sdcard";

android_app* app_global;
JNIEnv*      jnienv;
jclass       nativeActivity;

jclass findClass(std::string classname, JNIEnv* env = nullptr)
{
	env = env ? env : jnienv;
	if (env == 0) {
		return 0;
	}

	jclass nativeactivity = env->FindClass("android/app/NativeActivity");
	jmethodID getClassLoader =
	env->GetMethodID(nativeactivity,"getClassLoader",
					"()Ljava/lang/ClassLoader;");
	jobject cls =
	env->CallObjectMethod(app_global->activity->clazz, getClassLoader);
	jclass classLoader = env->FindClass("java/lang/ClassLoader");
	jmethodID findClass =
			env->GetMethodID(classLoader, "loadClass",
					"(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName =
			env->NewStringUTF(classname.c_str());
	return (jclass) env->CallObjectMethod(cls, findClass, strClassName);
}

void copyAssets()
{
	jmethodID assetcopy = jnienv->GetMethodID(nativeActivity,"copyAssets","()V");

	if (assetcopy == 0) {
		assert("porting::copyAssets unable to find copy assets method" == 0);
	}

	jnienv->CallVoidMethod(app_global->activity->clazz, assetcopy);
}

void initAndroid()
{
	porting::jnienv = NULL;
	JavaVM *jvm = app_global->activity->vm;
	JavaVMAttachArgs lJavaVMAttachArgs;
	lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	lJavaVMAttachArgs.name = "Edopro NativeThread";
	lJavaVMAttachArgs.group = NULL;
#ifdef NDEBUG
	// This is a ugly hack as arm v7a non debuggable builds crash without this
	// printf ... if someone finds out why please fix it!
	// infostream << "Attaching native thread. " << std::endl;
#endif
	if ( jvm->AttachCurrentThread(&porting::jnienv, &lJavaVMAttachArgs) == JNI_ERR) {
		// errorstream << "Failed to attach native thread to jvm" << std::endl;
		exit(-1);
	}

	nativeActivity = findClass("io/github/edo9300/edopro/EpNativeActivity");
	if (nativeActivity == 0) {
		// errorstream <<
// 			"porting::initAndroid unable to find java native activity class" <<
// 			std::endl;
	}
}

void cleanupAndroid()
{
	JavaVM *jvm = app_global->activity->vm;
	jvm->DetachCurrentThread();
}

static std::string javaStringToUTF8(jstring js)
{
	std::string str;
	// Get string as a UTF-8 c-string
	const char *c_str = jnienv->GetStringUTFChars(js, NULL);
	// Save it
	str = c_str;
	// And free the c-string
	jnienv->ReleaseStringUTFChars(js, c_str);
	return str;
}

// Calls static method if obj is NULL
static std::string getAndroidPath(jclass cls, jobject obj, jclass cls_File,
		jmethodID mt_getAbsPath, const char *getter)
{
	// Get getter method
	jmethodID mt_getter;
	if (obj)
		mt_getter = jnienv->GetMethodID(cls, getter,
				"()Ljava/io/File;");
	else
		mt_getter = jnienv->GetStaticMethodID(cls, getter,
				"()Ljava/io/File;");

	// Call getter
	jobject ob_file;
	if (obj)
		ob_file = jnienv->CallObjectMethod(obj, mt_getter);
	else
		ob_file = jnienv->CallStaticObjectMethod(cls, mt_getter);

	// Call getAbsolutePath
	jstring js_path = (jstring) jnienv->CallObjectMethod(ob_file,
			mt_getAbsPath);

	return javaStringToUTF8(js_path);
}

void initializePathsAndroid()
{
 	// Get Environment class
 	jclass cls_Env = jnienv->FindClass("android/os/Environment");
 	// Get File class
 	jclass cls_File = jnienv->FindClass("java/io/File");
 	// Get getAbsolutePath method
 	jmethodID mt_getAbsPath = jnienv->GetMethodID(cls_File,
 				"getAbsolutePath", "()Ljava/lang/String;");
 
//  	path_cache   = getAndroidPath(nativeActivity, app_global->activity->clazz,
// 			cls_File, mt_getAbsPath, "getCacheDir");
 	path_storage = getAndroidPath(cls_Env, NULL, cls_File, mt_getAbsPath,
 			"getExternalStorageDirectory");
// 	path_user    = path_storage + DIR_DELIM + PROJECT_NAME_C;
// 	path_share   = path_storage + DIR_DELIM + PROJECT_NAME_C;
}

void showInputDialog(const std::string& acceptButton, const  std::string& hint,
		const std::string& current, int editType)
{
	jmethodID showdialog = jnienv->GetMethodID(nativeActivity,"showDialog",
		"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");

	if (showdialog == 0) {
		assert("porting::showInputDialog unable to find java show dialog method" == 0);
	}

	jstring jacceptButton = jnienv->NewStringUTF(acceptButton.c_str());
	jstring jhint         = jnienv->NewStringUTF(hint.c_str());
	jstring jcurrent      = jnienv->NewStringUTF(current.c_str());
	jint    jeditType     = editType;

	jnienv->CallVoidMethod(app_global->activity->clazz, showdialog,
			jacceptButton, jhint, jcurrent, jeditType);
}

int getInputDialogState()
{
	jmethodID dialogstate = jnienv->GetMethodID(nativeActivity,
			"getDialogState", "()I");

	if (dialogstate == 0) {
		assert("porting::getInputDialogState unable to find java dialog state method" == 0);
	}

	return jnienv->CallIntMethod(app_global->activity->clazz, dialogstate);
}

std::string getInputDialogValue()
{
	jmethodID dialogvalue = jnienv->GetMethodID(nativeActivity,
			"getDialogValue", "()Ljava/lang/String;");

	if (dialogvalue == 0) {
		assert("porting::getInputDialogValue unable to find java dialog value method" == 0);
	}

	jobject result = jnienv->CallObjectMethod(app_global->activity->clazz,
			dialogvalue);

	const char* javachars = jnienv->GetStringUTFChars((jstring) result,0);
	std::string text(javachars);
	jnienv->ReleaseStringUTFChars((jstring) result, javachars);

	return text;
}

float getDisplayDensity()
{
	static bool firstrun = true;
	static float value = 0;

	if (firstrun) {
		jmethodID getDensity = jnienv->GetMethodID(nativeActivity, "getDensity",
					"()F");

		if (getDensity == 0) {
			assert("porting::getDisplayDensity unable to find java getDensity method" == 0);
		}

		value = jnienv->CallFloatMethod(app_global->activity->clazz, getDensity);
		firstrun = false;
	}
	return value;
}

std::pair<int,int> getDisplaySize()
{
	static bool firstrun = true;
	static std::pair<int,int> retval;

	if (firstrun) {
		jmethodID getDisplayWidth = jnienv->GetMethodID(nativeActivity,
				"getDisplayWidth", "()I");

		if (getDisplayWidth == 0) {
			assert("porting::getDisplayWidth unable to find java getDisplayWidth method" == 0);
		}

		retval.first = jnienv->CallIntMethod(app_global->activity->clazz,
				getDisplayWidth);

		jmethodID getDisplayHeight = jnienv->GetMethodID(nativeActivity,
				"getDisplayHeight", "()I");

		if (getDisplayHeight == 0) {
			assert("porting::getDisplayHeight unable to find java getDisplayHeight method" == 0);
		}

		retval.second = jnienv->CallIntMethod(app_global->activity->clazz,
				getDisplayHeight);

		firstrun = false;
	}
	return retval;
}
int downloadFile(const char* url, const char* path) {
	JNIEnv* jni = 0;
	JavaVM *jvm = app_global->activity->vm;
	jvm->AttachCurrentThread(&jni,nullptr);
// 	jclass nativeactivity = findClass("io/github/edo9300/edopro/EpNativeActivity", jni);
	jobject lNativeActivity = app_global->activity->clazz;
	jclass ClassNativeActivity = jni->GetObjectClass(lNativeActivity);
	jmethodID MethodGetApp = jni->GetMethodID(ClassNativeActivity,
											  "getApplication", "()Landroid/app/Application;");
	jobject application = jni->CallObjectMethod(lNativeActivity, MethodGetApp);
	jclass classApp = jni->GetObjectClass(application);
	jmethodID downloadFileMethod = jni->GetMethodID(classApp, "downloadFile", "(Ljava/lang/String;Ljava/lang/String;)I");
	jstring urlstring = jni->NewStringUTF(url);
	jstring pathstring = jni->NewStringUTF(path);
	jint result = jni->CallIntMethod(application, downloadFileMethod, urlstring, pathstring);
	if (urlstring) {
		jni->DeleteLocalRef(urlstring);
	}
	if (pathstring) {
		jni->DeleteLocalRef(pathstring);
	}
	jni->DeleteLocalRef(classApp);
	jni->DeleteLocalRef(ClassNativeActivity);
	jvm->DetachCurrentThread();
	return result;
}
}
