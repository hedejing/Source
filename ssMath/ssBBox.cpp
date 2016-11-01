#include "ssBBox.h"
#include "ssVector3.h"
#include "ssRay.h"
#include "ssSphere.h"
#include "ssPlane.h"
namespace ssMath
{
	//-----------------------------------------------------------------
	BBox Union(const BBox &b, const Point3 &p)
	{
		BBox tmp = b;
		tmp.merge(p);
		return tmp;
	}

	//-----------------------------------------------------------------
	BBox Union(const BBox &b, const BBox &b2)
	{
		BBox tmp = b;
		tmp.merge(b2);
		return tmp;
	}
	
	//-----------------------------------------------------------------
	BBox Intersection(const BBox &b1, const BBox &b2)
	{
		BBox tmp = b1;
		tmp.intersection(b2);
		return tmp;
	}
	
	//-----------------------------------------------------------------
	bool BBox::intersect(const Ray &ray, float &hitt0, float &hitt1) const
	{
		// Initialize parametric interval
		float t0 = ray.mint, t1 = ray.maxt;
		// Check X slab
		float invRayDir = 1.f / ray.D.x;
		float tNear = (m_Min.x - ray.O.x) * invRayDir;
		float tFar  = (m_Max.x - ray.O.x) * invRayDir;
		// Update parametric interval
		if (tNear > tFar) swap(tNear, tFar);
		t0 = max(tNear, t0);
		t1 = min(tFar,  t1);
		if (t0 > t1) return false;
		// Check Y slab
		invRayDir = 1.f / ray.D.y;
		tNear = (m_Min.y - ray.O.y) * invRayDir;
		tFar  = (m_Max.y - ray.O.y) * invRayDir;
		// Update parametric interval
		if (tNear > tFar) swap(tNear, tFar);
		t0 = max(tNear, t0);
		t1 = min(tFar,  t1);
		if (t0 > t1) return false;
		// Check Z slab
		invRayDir = 1.f / ray.D.z;
		tNear = (m_Min.z - ray.O.z) * invRayDir;
		tFar  = (m_Max.z - ray.O.z) * invRayDir;
		// Update parametric interval
		if (tNear > tFar) swap(tNear, tFar);
		t0 = max(tNear, t0);
		t1 = min(tFar,  t1);
		if (t0 > t1) return false;
		hitt0 = t0;
		hitt1 = t1;
		return true;
	}

	//-----------------------------------------------------------------
	bool BBox::intersect(const Sphere &sphere) const
	{
		return sphere.intersect(*this);
	}

	//-----------------------------------------------------------------
	bool BBox::intersect(const Plane &plane) const 
	{
		return (plane.getSide(*this) == Plane::BOTH_SIDE);
	}

	

	
}