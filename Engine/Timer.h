#ifndef HW_TIMER_H
#define HW_TIMER_H

#ifdef ANDROID
#include <sys/time.h>
#else
#include <Windows.h>
#endif

namespace HW
{
	class Timer
	{
	public:
		Timer()
		{
			this->restart();
		}
		void restart()
		{
#ifdef ANDROID
			timeval val;
			gettimeofday(&val,NULL);
			m_start_time = double(val.tv_sec)* 1000.0f + double(val.tv_usec)/1000.0f;
#else
			m_start_time = GetTickCount();
#endif
			m_last_time=m_start_time;
			
		}
		double elapsed() const
		{
			return this->current_time() - m_start_time;
		}
		double elapsed2()
		{
			double t=current_time()-m_last_time;
			m_last_time=current_time();
			return t;
		}
		double current_time() const
		{
			double time = 0;
#ifdef ANDROID
			timeval val;
			gettimeofday(&val,NULL);
			time =  double(val.tv_sec)* 1000.0f + double(val.tv_usec)/1000.0f;
			
#else
			time = GetTickCount();
#endif
			return time;
		}

	private:
		double m_start_time;
		double m_last_time;

	};
}
#endif