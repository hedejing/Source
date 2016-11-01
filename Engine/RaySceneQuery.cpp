#include "RaySceneQuery.h"
#include "SceneManager.h"
#include "Entity.h"
namespace HW
{

	RaySceneQueryResult & RaySceneQuery::execute()
	{
		mResult.clear();
		const std::map<string,Entity *>& entity_list = msceneMgr->getEntityList();
		std::map<string,Entity *>::const_iterator entity_it = entity_list.begin();

		for(;entity_it != entity_list.end();entity_it++)
		{
			float t0,t1;
			if(entity_it->second->getParent()->getRoot() == msceneMgr->GetSceneRoot())
			{
				if(mRay.intersect(entity_it->second->getBoundingBox(),t0,t1))
				{
					RaySceneQueryResultElement ele;
					ele.distance = t0;
					ele.movable = entity_it->second;
					mResult.push_back(ele);
				}
			}
		}

		if(mSortByDist)
		{
			std::sort(mResult.begin(),mResult.end());
		}

		return mResult;

	}
}