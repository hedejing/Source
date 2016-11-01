#pragma once
#include <fstream>

#ifdef ANDROID
#include <android/log.h>
#include <errno.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, " ", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, " ", __VA_ARGS__))
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR," ",__VA_ARGS__)
#define printf(...) __android_log_print(ANDROID_LOG_ERROR," ",__VA_ARGS__)
#endif



class Logger
{
public:
	static void WriteLog(const char * format, ...);
	static void WriteErrorLog(const char * format, ...);

	static void WriteAndroidInfoLog(const char * format, ...);
	static void WriteAndroidErrorLog(const char * format, ...);
	static void close()
	{
		out.close();
	}
	static std::ofstream out;
};


#define	LOG(x) Logger::WriteLog(x);