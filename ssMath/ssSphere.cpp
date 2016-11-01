#include "ssSphere.h"
#include "ssPlane.h"
#include "ssBBox.h"
namespace ssMath
{
	bool Sphere::intersect(const BBox& box) const
	{
		if (box.isNull()) return false;
		if (box.isInfinite()) return true;

		// Use splitting planes
		//const Vector3& center = sphere.getCenter();
		//Real radius = sphere.getRadius();
		const Vector3& min = box.getMinimum();
		const Vector3& max = box.getMaximum();

		// Arvo's algorithm
		float s, d = 0;
		for (int i = 0; i < 3; ++i)
		{
			if (mCenter.ptr()[i] < min.ptr()[i])
			{
				s = mCenter.ptr()[i] - min.ptr()[i];
				d += s * s; 
			}
			else if(mCenter.ptr()[i] > max.ptr()[i])
			{
				s = mCenter.ptr()[i] - max.ptr()[i];
				d += s * s; 
			}
		}
		return d <= mRadius * mRadius;
	}

	//------------------------------------------------------
	bool Sphere::intersect(const Plane& plane)const 
	{
		return (
			fabs(plane.getDistance(mCenter))
			<= mRadius );
	}
}