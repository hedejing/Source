#ifndef SHARED_POINTER_H
#define SHARED_POINTER_H

namespace HW
{
	template<typename T>
	class SharedPointer
	{
	public:
		explicit SharedPointer(T *p = 0)
		{
			m_interP = p;
			m_UseCount = 0;
			if(p)
			{
				m_UseCount = new int(0);
				(*m_UseCount) = 0;
				(*m_UseCount)++;
			}
		}

		SharedPointer(const SharedPointer& cpy)
		{
			m_interP = cpy.m_interP;
			m_UseCount = cpy.m_UseCount;
			if(m_interP)
			{
				(*m_UseCount)++;
			}
		}

		virtual ~SharedPointer()
		{
			if(m_interP && m_UseCount)
			{
				(*m_UseCount)--;
				if(!(*m_UseCount))
				{
					delete m_interP;
					delete m_UseCount; 
				}
			}
		}

		SharedPointer& operator=(const SharedPointer &cpy)
		{
			/// 
			if(m_interP && m_UseCount)
			{
				(*m_UseCount)--;
				if(!(*m_UseCount))
				{
					delete m_interP;
					delete m_UseCount;
				}
			}
			m_interP = cpy.m_interP;
			m_UseCount = cpy.m_UseCount;
			if(m_interP)
			{
				(*m_UseCount)++;
			}
			return *this;
		}

		T* operator->() const { return m_interP; }

		T& operator*() const  { return *m_interP; }

		bool IsNull() const { return !m_interP; }

		T* InterPtr() const { return m_interP; }

		void Release()
		{
			delete this;
		}

	protected:

		T* m_interP;
		int *m_UseCount;
	};
}
#endif