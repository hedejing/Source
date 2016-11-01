#ifndef HW_TECHNIQUE_H
#define HW_TECHNIQUE_H
#include<string>
#include "Pass.h"
namespace HW
{
	class Technique
	{
	public:
		string mName;

		std::vector<Pass *> mPasses;

		void addPass(Pass * pass)
		{
			mPasses.push_back(pass);
		}
		Pass * getPass(unsigned int id)
		{
			if(id >= mPasses.size())
				return NULL;
			return mPasses[id];
		}
		unsigned int numPass() const
		{
			return mPasses.size();
		}

		~Technique()
		{
			for(unsigned int  i = 0 ; i < mPasses.size();i++)
			{
				delete mPasses[i];
			}

			mPasses.clear();
		}

	};
}

#endif