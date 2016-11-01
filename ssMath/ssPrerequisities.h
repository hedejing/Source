#ifndef ss_math_Prerequisities_H_
#define ss_math_Prerequisities_H_

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <cmath>
#include <cassert>

using namespace std;

#ifndef M_PI
#define M_PI		( 3.14159265359f )
#define M_PI_2		( 1.57079632680f )
#endif
#define M_HALF_PI   M_PI_2
#ifndef EPSILON
#define	EPSILON		1.0e-4f
#endif

#ifndef INFINITY
#define INFINITY	1.0e+8f
#endif

// fast global functions

static float ss_fast_InvSqrt(float t)
{
	float x = t;
	float xhalf = 0.5f*x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i >> 1);
	x = *(float*)&i;
	x = x*(1.5f - xhalf*x*x);
	return x;
}

template <typename T>
static T ss_fast_Clamp(T val, T minval, T maxval)
{
	assert (minval < maxval && "Invalid clamp range");
	return (std::max)((std::min)(val, maxval), minval);
}


namespace ssMath
{
	// forward declarations
	class Ray;
	class Plane;
	class Vector2;
	class Vector3;
	class Vector4;
	class Matrix3;
	class Matrix4;
	class Quaternion;
	class Point3;
	class BBox;
	class Sphere;
}
#endif