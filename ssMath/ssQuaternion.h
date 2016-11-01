#ifndef ss_math_Quaternion_H_
#define ss_math_Quaternion_H_

#include "ssPrerequisities.h"
#include "ssMatrix3.h"
#include "ssVector3.h"
namespace ssMath
{
	class Quaternion
    {
    public:
        inline Quaternion (
            float fW = 1.0,
            float fX = 0.0, float fY = 0.0, float fZ = 0.0)
		{
			w = fW;
			x = fX;
			y = fY;
			z = fZ;
		}
        /// Construct a quaternion from a rotation matrix
        inline Quaternion(const Matrix3& rot)
        {
            this->fromRotationMatrix(rot);
        }
        /// Construct a quaternion from an angle/axis
        inline Quaternion(const float& rfAngle, const Vector3& rkAxis)
        {
            this->fromAngleAxis(rfAngle, rkAxis);
        }
        /// Construct a quaternion from 3 orthonormal local axes
        inline Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
        {
            this->fromAxes(xaxis, yaxis, zaxis);
        }
        /// Construct a quaternion from 3 orthonormal local axes
        inline Quaternion(const Vector3* akAxis)
        {
            this->fromAxes(akAxis);
        }
		/// Construct a quaternion from 4 manual w/x/y/z values
		inline Quaternion(float* valptr)
		{
			memcpy(&w, valptr, sizeof(float)*4);
		}

		/** Exchange the contents of this quaternion with another. 
		*/
		inline void swap(Quaternion& other)
		{
			std::swap(w, other.w);
			std::swap(x, other.x);
			std::swap(y, other.y);
			std::swap(z, other.z);
		}

		/// Array accessor operator
		inline float operator [] ( const size_t i ) const
		{
			assert( i < 4 );

			return *(&w+i);
		}

		/// Array accessor operator
		inline float& operator [] ( const size_t i )
		{
			assert( i < 4 );

			return *(&w+i);
		}

		/// Pointer accessor for direct copying
		inline float* ptr()
		{
			return &w;
		}

		/// Pointer accessor for direct copying
		inline const float* ptr() const
		{
			return &w;
		}

		void fromRotationMatrix (const Matrix3& kRot);
        void toRotationMatrix (Matrix3& kRot) const;
        void fromAngleAxis (float rfAngle, const Vector3& rkAxis);
        void toAngleAxis (float& rfAngle, Vector3& rkAxis) const;

        void fromAxes (const Vector3* akAxis);
        void fromAxes (const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
        void toAxes (Vector3* akAxis) const;
        void toAxes (Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;
        /// Get the local x-axis
        Vector3 xAxis(void) const;
        /// Get the local y-axis
        Vector3 yAxis(void) const;
        /// Get the local z-axis
        Vector3 zAxis(void) const;

        inline Quaternion& operator= (const Quaternion& rkQ)
		{
			w = rkQ.w;
			x = rkQ.x;
			y = rkQ.y;
			z = rkQ.z;
			return *this;
		}
        Quaternion operator+ (const Quaternion& rkQ) const;
        Quaternion operator- (const Quaternion& rkQ) const;
        Quaternion operator* (const Quaternion& rkQ) const;
        Quaternion operator* (float fScalar) const;
        friend Quaternion operator* (float fScalar,
            const Quaternion& rkQ);
        Quaternion operator- () const;
        inline bool operator== (const Quaternion& rhs) const
		{
			return (rhs.x == x) && (rhs.y == y) &&
				(rhs.z == z) && (rhs.w == w);
		}
        inline bool operator!= (const Quaternion& rhs) const
		{
			return !operator==(rhs);
		}
        // functions of a quaternion
        float dotProduct (const Quaternion& rkQ) const;  // dot product
        float norm () const;  // squared-length
        /// Normalizes this quaternion, and returns the previous length
        float normalize(void); 
        Quaternion inverse () const;  // apply to non-zero quaternion
        Quaternion unitInverse () const;  // apply to unit-length quaternion
        Quaternion exp () const;
        Quaternion log () const;

        // rotation of a vector by a quaternion
        Vector3 operator* (const Vector3& rkVector) const;

   		/** Calculate the local roll element of this quaternion.
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Y of the quaternion onto the X and
			Y axes, the angle between them is returned. If set to false though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. 
		*/
		float getRoll(bool reprojectAxis = true) const;
   		/** Calculate the local pitch element of this quaternion
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Z of the quaternion onto the X and
			Y axes, the angle between them is returned. If set to true though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. 
		*/
		float getPitch(bool reprojectAxis = true) const;
   		/** Calculate the local yaw element of this quaternion
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Z of the quaternion onto the X and
			Z axes, the angle between them is returned. If set to true though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. 
		*/
		float getYaw(bool reprojectAxis = true) const;		
		/// Equality with tolerance (tolerance is max angle difference)
		bool equals(const Quaternion& rhs, const float& tolerance) const;
		
	    // spherical linear interpolation
        static Quaternion slerp (float fT, const Quaternion& rkP,
            const Quaternion& rkQ, bool shortestPath = false);

        static Quaternion slerpExtraSpins (float fT,
            const Quaternion& rkP, const Quaternion& rkQ,
            int iExtraSpins);

        // setup for spherical quadratic interpolation
        static void intermediate (const Quaternion& rkQ0,
            const Quaternion& rkQ1, const Quaternion& rkQ2,
            Quaternion& rka, Quaternion& rkB);

        // spherical quadratic interpolation
        static Quaternion squad (float fT, const Quaternion& rkP,
            const Quaternion& rkA, const Quaternion& rkB,
            const Quaternion& rkQ, bool shortestPath = false);

        // normalized linear interpolation - faster but less accurate (non-constant rotation velocity)
        static Quaternion nlerp(float fT, const Quaternion& rkP, 
            const Quaternion& rkQ, bool shortestPath = false);

        // cutoff for sine near zero
        static const float ms_fEpsilon;

        // special values
        static const Quaternion ZERO;
        static const Quaternion IDENTITY;

		float w, x, y, z;

    };
}

#endif