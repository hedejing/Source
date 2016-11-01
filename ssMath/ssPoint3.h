#ifndef ss_math_Point3_H_
#define ss_math_Point3_H_

#include "ssPrerequisities.h"
#include "ssVector3.h"
namespace ssMath
{
	class Point3 {
	public:
		// Point3 Methods
		Point3(float _x=0, float _y=0, float _z=0)
			: x(_x), y(_y), z(_z) 
		{
		}
		Point3(const Vector3 &p):x(p.x),y(p.y),z(p.z)
		{
		}
		 operator Vector3() const
		{
			return Vector3(x, y, z);
		}
		inline Point3 operator+(const Vector3 &v) const 
		{
			return Point3(x + v.x, y + v.y, z + v.z);
		}
		inline Point3 &operator+=(const Vector3 &v) 
		{
			x += v.x; y += v.y; z += v.z;
			return *this;
		}
		inline Vector3 operator-(const Point3 &p) const 
		{
			return Vector3(x - p.x, y - p.y, z - p.z);
		}
		inline Point3 operator-(const Vector3 &v) const 
		{
			return Point3(x - v.x, y - v.y, z - v.z);
		}
		inline Point3 &operator-=(const Vector3 &v) 
		{
			x -= v.x; y -= v.y; z -= v.z;
			return *this;
		}
		inline Point3 &operator+=(const Point3 &p) 
		{
			x += p.x; y += p.y; z += p.z;
			return *this;
		}
		inline Point3 operator+(const Point3 &p) 
		{
			return Point3(x + p.x, y + p.y, z + p.z);
		}
		inline Point3 operator* (float f) const 
		{
			return Point3(f*x, f*y, f*z);
		}
		inline Point3 &operator*=(float f) 
		{
			x *= f; y *= f; z *= f;
			return *this;
		}
		inline Point3 operator/ (float f) const 
		{
			float inv = 1.f/f;
			return Point3(inv*x, inv*y, inv*z);
		}
		inline Point3 &operator/=(float f) {
			float inv = 1.f/f;
			x *= inv; y *= inv; z *= inv;
			return *this;
		}

		inline bool operator==(const Point3 &rhs) const
		{
			if(fabs(x - rhs.x) > 1e-6)
				return false;
			if(fabs(y - rhs.y) > 1e-6)
				return false;
			if(fabs(z - rhs.z) > 1e-6)
				return false;
			return true;
		}

		inline bool operator!=(const Point3 &rhs) const 
		{
			return !(*this == rhs);
		}

		inline float length() const { return sqrtf(x*x + y*y + z*z); }
		inline float squaredLength() const { return x*x + y*y + z*z; }
		
		inline float operator[](int i) const { return (&x)[i]; }
		inline float &operator[](int i) { return (&x)[i]; }
		// Point3 Public Data
		float x,y,z;
	};
}
#endif