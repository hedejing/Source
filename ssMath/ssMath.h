#ifndef _ss_math_H_
#define _ss_math_H_
#include "ssVector2.h"
#include "ssVector3.h"
#include "ssVector4.h"
#include "ssBBox.h"
#include "ssMatrix3.h"
#include "ssMatrix4.h"
#include "ssPlane.h"
#include "ssPoint3.h"
#include "ssQuaternion.h"
#include "ssRay.h"
#include "ssSphere.h"
#ifndef ANDROID
#include "ssMathSerialize.h"
#endif // !ANDROID


using std::min;
using std::max;

// global functions for fast use.
inline void CoordinateSystem0(const ssMath::Vector3 &v1, ssMath::Vector3 &v2, ssMath::Vector3 &v3)
{
    if(v1.y != 1 && v1.y != -1)
    {
        v2 = ssMath::Vector3(v1.z, 0, -v1.x).hat();
        v3 = v1.crossProduct(v2);//Cross(v1, *v2);
    }
    else
    {
        v2 = ssMath::Vector3(1, 0, 0);
        v3 = ssMath::Vector3(0, 0, 1);
    }
}

inline void CoordinateSystem1(const ssMath::Vector3 &v1, ssMath::Vector3 &v2, ssMath::Vector3 &v3)
{
    if (fabsf(v1.x) > fabsf(v1.y)) {
        float invLen = 1.f / sqrtf(v1.x*v1.x + v1.z*v1.z);
        v2 = ssMath::Vector3(-v1.z * invLen, 0.f, v1.x * invLen);
    }
    else {
        float invLen = 1.f / sqrtf(v1.y*v1.y + v1.z*v1.z);
        v2 = ssMath::Vector3(0.f, v1.z * invLen, -v1.y * invLen);
    }
    v3 = v1.crossProduct(v2);//Cross(v1, *v2);
}

inline void CoordinateSystem(const ssMath::Vector3 &v1, ssMath::Vector3 &v2, ssMath::Vector3 &v3) {
    
    /* using stereographi projection parametrization */
    /* notice it could be the singular point (0, -1, 0) */
    
    if((v1.y + 1.f) < 1.0e-8)
        v2 = ssMath::Vector3(0, 0, 0);
    else
        v2 = ssMath::Vector3((v1.y + 1.f) * (v1.y + 1.f) + v1.z * v1.z - v1.x * v1.x,
                      -2.f * v1.x * (v1.y + 1.f),
                      -2.f * v1.x * v1.z).hat();
    v3 = v1.crossProduct(v2);//Cross(v1, *v2);
    
}

inline float Distance(const ssMath::Point3 &p1, const ssMath::Point3 &p2) {
    return (p1 - p2).length();
}
inline float Clamp(const float &x, const float &minval, const float &maxval) {
	if (x < minval) return minval;
	if (x > maxval) return maxval;
	return x;
}
inline float DistanceSquared(const ssMath::Point3 &p1, const ssMath::Point3 &p2) {
    return (p1 - p2).squaredLength();
}


inline ssMath::Vector3 SphericalDirection(float sintheta, float costheta,
                                  float phi) {
    return ssMath::Vector3(sintheta * cosf(phi),
                   sintheta * sinf(phi), costheta);
}

inline ssMath::Vector3 SphericalDirection(float sintheta, float costheta,
                                  float phi, const ssMath::Vector3 &x, const ssMath::Vector3 &y,
                                  const ssMath::Vector3 &z) {
    return sintheta * cosf(phi) * x +
    sintheta * sinf(phi) * y + costheta * z;
}

inline float SphericalTheta(const ssMath::Vector3 &v) {
    return acosf(v.z);
}

inline float SphericalPhi(const ssMath::Vector3 &v) {
    return atan2f(v.y, v.x) + (float)M_PI;
}


#endif


