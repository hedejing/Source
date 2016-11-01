#include "Logger.h"
#include <stdarg.h>

#ifdef ANDROID
	std::ofstream Logger::out("sdcard/out.txt");
#else
	std::ofstream Logger::out("out.txt");
#endif
	

	void Logger::WriteLog(const char * format,...)
	{
		char strBuffer[2048];
		va_list arguments;
		va_start(arguments,format);
		vsprintf(strBuffer,format,arguments);
		va_end(arguments);

		out<<"Info : "<<strBuffer<<std::endl;
#ifdef ANDROID
		LOGI("%s",strBuffer);
#endif
	
		
	}

	void Logger::WriteErrorLog(const char * format,...)
	{
		
		char strBuffer[2048];
		va_list arguments;
		va_start(arguments,format);
		vsprintf(strBuffer,format,arguments);
		va_end(arguments);

		out<<"Error : "<<strBuffer<<std::endl;
#ifdef ANDROID
		LOGE("%s",strBuffer);
#endif

	}

	void Logger::WriteAndroidInfoLog(const char * format, ...)
	{
#ifdef ANDROID
		char strBuffer[2048];
		va_list arguments;
		va_start(arguments, format);
		vsprintf(strBuffer, format, arguments);
		va_end(arguments);

		LOGI("%s", strBuffer);
#endif

	}

	void Logger::WriteAndroidErrorLog(const char * format, ...)
	{

		char strBuffer[2048];
		va_list arguments;
		va_start(arguments, format);
		vsprintf(strBuffer, format, arguments);
		va_end(arguments);
#ifdef ANDROID
		LOGE("%s", strBuffer);
#endif

	}
