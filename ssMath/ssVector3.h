#ifndef ss_math_Vector3_H_
#define ss_math_Vector3_H_

#include "ssPrerequisities.h"

namespace ssMath
{
	class Vector3
	{
	public:
		// constructors 
		inline explicit Vector3()
			: x(0), y(0), z(0)
		{
		}

		inline explicit Vector3(float _x, float _y, float _z)
			: x(_x), y(_y), z(_z) 
		{
		}

		inline explicit Vector3(float scalar)
			: x(scalar), y(scalar), z(scalar)
		{
		}

		inline explicit Vector3(float *const r)
			: x(r[0]), y(r[1]), z(r[2])
		{

		}

		inline explicit Vector3(const float coord[3])
			: x(coord[0]), y(coord[1]), z(coord[2])
		{

		}

		inline explicit Vector3(const int coord[3])
		{
			x = (float)coord[0];
			y = (float)coord[1];
			z = (float)coord[2];
		}
		// copy constructor
		inline Vector3(const Vector3 &vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
		}

		// operator override
		inline Vector3 &operator=(const Vector3 &v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}

		inline Vector3& operator = ( const float fScaler )
		{
			x = fScaler;
			y = fScaler;
			z = fScaler;

			return *this;
		}
		inline friend Vector3 operator+(const Vector3 &lhs,const Vector3 &rhs)
		{
			return Vector3(lhs.x + rhs.x,
							lhs.y + rhs.y,
							lhs.z + rhs.z);
		}

// 		inline Vector3 operator+(const Vector3 &v) const 
// 		{
// 			return Vector3(x + v.x, y + v.y, z + v.z);
// 		}

		inline Vector3& operator+=(const Vector3 &v) 
		{
			x += v.x; y += v.y; z += v.z;
			return *this;
		}

		inline friend Vector3 operator-(const Vector3 &lhs,const Vector3 &rhs)
		{
			return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
		}

// 		inline Vector3 operator-(const Vector3 &v) const 
// 		{
// 			return Vector3(x - v.x, y - v.y, z - v.z);
// 		}

		inline Vector3& operator-=(const Vector3 &v) 
		{
			x -= v.x; y -= v.y; z -= v.z;
			return *this;
		}

		inline friend Vector3 operator*(const Vector3 &lhs,const Vector3 &rhs)
		{
			return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
		}

		inline friend Vector3 operator*(const Vector3 &lhs,float f)
		{
			return Vector3(lhs.x * f, lhs.y * f, lhs.z * f);
		}

		inline friend Vector3 operator*(float f,const Vector3 &rhs)
		{
			return rhs*f;
		}

		inline Vector3 &operator*=(float f) 
		{
			x *= f; y *= f; z *= f;
			return *this;
		}

		inline Vector3 &operator*=(const Vector3 &rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}

		inline Vector3 operator/(float f) const 
		{
			float inv = 1.f / f;
			return Vector3(x * inv, y * inv, z * inv);
		}
		inline Vector3 &operator/=(float f)
		{
			float inv = 1.f / f;
			x *= inv; y *= inv; z *= inv;
			return *this;
		}
		inline Vector3 operator-() const 
		{
			return Vector3(-x, -y, -z);
		}
		// logic operation
		inline bool operator < ( const Vector3& rhs ) const
		{
			if( x < rhs.x && y < rhs.y && z < rhs.z )
				return true;
			return false;
		}

		inline bool operator > ( const Vector3& rhs ) const
		{
			if( x > rhs.x && y > rhs.y && z > rhs.z )
				return true;
			return false;
		}

		inline bool operator == ( const Vector3& rkVector ) const
		{
			return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
		}

		inline bool operator != ( const Vector3& rkVector ) const
		{
			return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
		}
		// exchange content with another vector
		inline void swap(Vector3& other)
		{
			std::swap(x, other.x);
			std::swap(y, other.y);
			std::swap(z, other.z);
		}
		inline float* ptr()
		{
			return &x;
		}
		inline const float* ptr()const
		{
			return &x;
		}
		inline float distance(const Vector3 &v )
		{
			return (*this - v).length();
		}
		inline float distSquared(const Vector3 &v)
		{
			return (*this - v).squaredLength();
		}
		inline float dotProduct(const Vector3& vec) const
		{
			return x * vec.x + y * vec.y + z * vec.z;
		}
		inline float absDotProduct(const Vector3 &vec) const
		{
			return abs(x *vec.x) + abs(y *vec.y) + abs(z* vec.z);
		}
		inline float normalize() 
		{
			float flen = length();
			if(flen > 1e-6)
			{
				float invlen = 1.0f/flen;
				x *= invlen;
				y *= invlen;
				z *= invlen;
			}
			return flen;
		}
		/** Sets this vector's components to the minimum of its own and the
            ones of the passed in vector.
            @remarks
                'Minimum' in this case means the combination of the lowest
                value of x, y and z from both vectors. Lowest is taken just
                numerically, not magnitude, so -1 < 0.
        */
		inline void makeFloor( const Vector3& cmp )
		{
			if( cmp.x < x ) x = cmp.x;
			if( cmp.y < y ) y = cmp.y;
			if( cmp.z < z ) z = cmp.z;
		}
		/** Sets this vector's components to the maximum of its own and the
            ones of the passed in vector.
            @remarks
                'Maximum' in this case means the combination of the highest
                value of x, y and z from both vectors. Highest is taken just
                numerically, not magnitude, so 1 > -3.
        */
		inline void makeCeil( const Vector3& cmp )
		{
			if( cmp.x > x ) x = cmp.x;
			if( cmp.y > y ) y = cmp.y;
			if( cmp.z > z ) z = cmp.z;
		}
		/** Generates a vector perpendicular to this vector (eg an 'up' vector).
            @remarks
                This method will return a vector which is perpendicular to this
                vector. There are an infinite number of possibilities but this
                method will guarantee to generate one of them. If you need more
                control you should use the Quaternion class.
        */
		inline Vector3 perpendicular(void) const
        {
            static const float fSquareZero = (float)(1e-06 * 1e-06);

            Vector3 perp = this->crossProduct( Vector3::UNIT_X );

            // Check length
            if( perp.squaredLength() < fSquareZero )
            {
                /* This vector is the Y axis multiplied by a scalar, so we have
                   to use another axis.
                */
                perp = this->crossProduct( Vector3::UNIT_Y );
            }
			perp.normalize();

            return perp;
        }
		/** Gets the angle between 2 vectors.
		@remarks
			Vectors do not have to be unit-length but must represent directions.
			Angle unit is radian.
		*/
		inline float angleBetween(const Vector3& dest)
		{
			float lenProduct = length() * dest.length();

			// Divide by zero check
			if(lenProduct < 1e-6f)
				lenProduct = 1e-6f;

			float f = dotProduct(dest) / lenProduct;

			f = ss_fast_Clamp(f, float(-1.0), (float)1.0);
			
			return acos(f);
		}
		/** Returns true if this vector is zero length. */
		inline bool isZeroLength(void) const
		{
			float sqlen = (x * x) + (y * y) + (z * z);
			return (sqlen < (1e-06 * 1e-06));

		}
		inline Vector3 crossProduct( const Vector3& rkVector ) const
		{
			return Vector3(
				y * rkVector.z - z * rkVector.y,
				z * rkVector.x - x * rkVector.z,
				x * rkVector.y - y * rkVector.x);
		}
		/** Calculates a reflection vector to the plane with the given normal .
        @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
        */
        inline Vector3 reflect(const Vector3& normal) const
        {
            return Vector3( *this - (   normal*2 * this->dotProduct(normal) ) );
        }
		/** Returns whether this vector is within a positional tolerance
			of another vector.
		@param rhs The vector to compare with
		@param tolerance The amount that each element of the vector may vary by
			and still be considered equal
		*/
		inline bool positionEquals(const Vector3& rhs, float tolerance = 1e-03) const
		{
			if( fabs(x-rhs.x) > tolerance)
				return false;
			if( fabs(y-rhs.y) > tolerance)
				return false;
			if( fabs(z-rhs.z) > tolerance)
				return false;
			return true;

		}
		/** Returns whether this vector is within a directional tolerance
			of another vector.
		@param rhs The vector to compare with
		@param tolerance The maximum angle by which the vectors may vary and
			still be considered equal
		@note Both vectors should be normalised.
		*/
		inline bool directionEquals(const Vector3& rhs,
			float tolerance) const
		{
			float dot = dotProduct(rhs);
			float angle = acos(dot);

			return abs(angle) <= tolerance;

		}

		inline Vector3 midPoint( const Vector3& vec ) const
		{
			return Vector3(
				( x + vec.x ) * 0.5f,
				( y + vec.y ) * 0.5f,
				( z + vec.z ) * 0.5f );
		}
		// extract element
		inline float operator[](int i) const { return (&x)[i]; }
		inline float &operator[](int i) { return (&x)[i]; }
		// square length
		inline float squaredLength() const { return x*x + y*y + z*z; }

		inline float length() const { return sqrtf( squaredLength() ); }

		inline Vector3 hat() const { return (*this)*ss_fast_InvSqrt(squaredLength());}//return (*this)/len(); }
		// Vector3 Public Data
		float x, y, z;

		// static members for convenience
		static const Vector3 ZERO; // (0,0,0)
		static const Vector3 UNIT_X;
		static const Vector3 UNIT_Y;
		static const Vector3 UNIT_Z;
		static const Vector3 NEGATIVE_UNIT_X;
		static const Vector3 NEGATIVE_UNIT_Y;
		static const Vector3 NEGATIVE_UNIT_Z;
		static const Vector3 UNIT_SCALE; // (1,1,1)
	};
}
#endif