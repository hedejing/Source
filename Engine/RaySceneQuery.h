#ifndef RAYSCENEQUERY_H
#define RAYSCENEQUERY_H
#include <vector>
#include "mathlib.h"
namespace HW{

	class SceneManager;
	class Movable;
	struct RaySceneQueryResultElement
	{
		float distance;
		Movable * movable;

		bool operator < (const RaySceneQueryResultElement & rhs) const
		{
			return distance < rhs.distance;
		}
		
	};
	typedef std::vector<RaySceneQueryResultElement> RaySceneQueryResult;

	class RaySceneQuery
	{
	protected: 
		Ray mRay;
		bool mSortByDist;
		SceneManager * msceneMgr;
		RaySceneQueryResult mResult;
	public:
		RaySceneQuery(SceneManager * sceneMgr)
		{
			msceneMgr = sceneMgr;
			mSortByDist = true;
		}
		 ~RaySceneQuery(){}

		 void setRay(Ray & ray)
		 {
			 mRay = ray;
		 }

		  const  Ray& getRay() const
		 {
			return mRay;
		 }

		 void setSortByDistance(bool sort)
		 {
			 mSortByDist = sort;
		 }

		 RaySceneQueryResult & getResult()
		 {
			 return mResult;
		 }

		 RaySceneQueryResult & execute();


	};
}
#endif