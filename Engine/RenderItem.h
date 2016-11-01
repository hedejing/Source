#pragma once
#include "PreDefine.h"
#include <vector>
#include "engine_struct.h"
namespace HW
{


	struct RenderQueueItem
	{
		Entity * entity=NULL;
		Renderable * renderable= NULL;
		as_Mesh*	asMesh = NULL;
		unsigned int sortkey=0;
		Pass * pass = NULL;
		Material * material = NULL;
		Geometry* geometry = NULL;
		RenderQueueItem(){
			entity = NULL;
			renderable = NULL;
			asMesh = NULL;
			sortkey = 0;
			pass = NULL;
			material = NULL;
			geometry = NULL;
		}
	};

	typedef std::vector<RenderQueueItem> RenderQueue;
	static bool namecompare(const RenderQueueItem & a, const RenderQueueItem & b)
	{
		if (a.asMesh == NULL || b.asMesh == NULL) return false;
		return a.asMesh->name < b.asMesh->name;  
	}
	static void SortByMeshName(RenderQueue& queue) {
		sort(queue.begin(), queue.end(), namecompare);
	}

	//inline bool CompDistRenderQueueItem(const RenderQueueItem & a,const RenderQueueItem & b)
	//{
	//	
	//	return a.sortkey < b.sortkey;
	//	
	//}
}
