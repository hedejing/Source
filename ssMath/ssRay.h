#ifndef ss_math_Ray_H_
#define ss_math_Ray_H_
#include "ssPrerequisities.h"
#include "ssPoint3.h"
#include "ssVector3.h"
namespace ssMath
{
	class Ray 
	{
	public:
		// Ray Constructor Declarations
		Ray(): mint(EPSILON), maxt(INFINITY), time(0.) {}
		Ray(const Point3 &origin, const Vector3 &direction,
			float start = EPSILON, float end = INFINITY, float t = 0.)
			: O(origin), D(direction), mint(start), maxt(end), time(t) {
		}
		// Ray Method Declarations
		Point3 getPoint( float t) const { return O + D * t; }
		Point3 operator()(float t) const { return O + D * t; }
		// test whether this ray intersects the given object.
		bool  intersect(const Plane &plane, float &t) const;
		bool  intersect(const BBox &bbox, float &t0,float &t1) const;
		bool  intersect(const Sphere &sphere, float &t1, float& t2) const;
		// Ray Public Data
		Point3 O;
		Vector3 D;
		float mint, maxt;
		float time;
	};
}
#endif