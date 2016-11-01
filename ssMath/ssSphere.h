#ifndef ss_math_Sphere_H_
#define ss_math_Sphere_H_

#include "ssPrerequisities.h"
#include "ssVector3.h"

namespace ssMath
{
	class Sphere
    {
    protected:
        float mRadius;
        Vector3 mCenter;
    public:
        /** Standard constructor - creates a unit sphere around the origin.*/
        Sphere() : mRadius(1.0), mCenter(Vector3::ZERO) {}
        /** Constructor allowing arbitrary spheres. 
            @param center The center point of the sphere.
            @param radius The radius of the sphere.
        */
        Sphere(const Vector3& center, float radius)
            : mRadius(radius), mCenter(center) {}

        /** Returns the radius of the sphere. */
        float getRadius(void) const { return mRadius; }

        /** Sets the radius of the sphere. */
        void setRadius(float radius) { mRadius = radius; }

        /** Returns the center point of the sphere. */
        const Vector3& getCenter(void) const { return mCenter; }

        /** Sets the center point of the sphere. */
        void setCenter(const Vector3& center) { mCenter = center; }

		/** Returns whether or not this sphere intersects another sphere. */
		bool intersect(const Sphere& s) const
		{
            return (s.mCenter - mCenter).squaredLength() <=
                (s.mRadius + mRadius)*(s.mRadius + mRadius);
		}
		/** Returns whether or not this sphere intersects a box. */
		bool intersect(const BBox& box) const;
		/** Returns whether or not this sphere intersects a plane. */
		bool intersect(const Plane& plane) const;
		/** Returns whether or not this sphere intersects a point. 
			in other words,it test whether this sphere contains the 
			given point.
		*/
		bool intersects(const Vector3& v) const
		{
            return ((v - mCenter).squaredLength() <= mRadius*mRadius);
		}
        
		void merge(const Sphere& oth)
		{
		/*	if(this->mCenter == other.mCenter)
				return;
			Vector3 newCenter = (this->mCenter + other.mCenter)*0.5f;
			float d = (this->mCenter - other.mCenter).length()*0.5f;
			float newRadius = d + (std::max)(this->mRadius,other.mRadius);
			this->mCenter = newCenter;
			this->mRadius = newRadius;*/
			Vector3 diff =  oth.getCenter() - mCenter;
			float lengthSq = diff.squaredLength();
			float radiusDiff = oth.getRadius() - mRadius;

			// Early-out
			if ((radiusDiff * radiusDiff) >= lengthSq) 
			{
				// One fully contains the other
				if (radiusDiff <= 0.0f) 
					return; // no change
				else 
				{
					mCenter = oth.getCenter();
					mRadius = oth.getRadius();
					return;
				}
			}

			float length = sqrt(lengthSq);

			Vector3 newCenter;
			float newRadius;
			if ((length + oth.getRadius()) > mRadius) 
			{
				float t = (length + radiusDiff) / (2.0f * length);
				newCenter = mCenter + diff * t;
			} 
			// otherwise, we keep our existing center

			newRadius = 0.5f * (length + mRadius + oth.getRadius());

			mCenter = newCenter;
			mRadius = newRadius;
		}
    };
}
#endif