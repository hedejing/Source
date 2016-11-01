#include "ssRay.h"
#include "ssPlane.h"
#include "ssBBox.h"
#include "ssSphere.h"
namespace ssMath
{
	bool Ray::intersect(const Sphere &sphere, float &t1, float &t2) const
	{
		float radius = sphere.getRadius();
		float a = D.squaredLength();
		float b = ((Vector3)O - sphere.getCenter()).dotProduct(D)*2;
		float c = ((Vector3)O - sphere.getCenter()).squaredLength() - radius*radius;

		float delta = b*b - 4*a*c;
		if(delta < -1e-6)
			return false;
		if(delta < 1e-6)
		{
			t1 = -0.5f*b/a;
			t2 = t1;
			return true;
		}

		t1 = -(b + sqrt(delta))*0.5f/a;
		t2 = (sqrtf(delta) - b)*0.5f/a;
		if(t1 > t2) swap(t1,t2);
		return true;
	}


	bool Ray::intersect(const BBox &bbox, float &t0,float &t1) const
	{
		return bbox.intersect(*this,t0,t1);
	}

	bool Ray::intersect(const Plane &plane, float &t) const
	{
		// test if the origin of ray is on the plane.
		if(plane.getSide(O) == Plane::NO_SIDE)
		{ 
			t = 0;
			return true;
		}
		float dotp = plane.normal.dotProduct(D);
		if( fabs(dotp) < 1e-6)
			return false;
		float tt = (-plane.d - plane.normal.dotProduct(O))/dotp;
		if( tt < std::min(mint,maxt))
			return false;
		if( tt > std::max(mint,maxt))
			return false;
		t = tt;
		return true;
	}
}