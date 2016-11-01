#pragma once
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
#include "serialize.h"

namespace ssMath {
	template<class Archive>
	void serialize(Archive & archive,
		Vector3 & m)
	{
		float& x=m.x,&y=m.y,&z=m.z;
		archive(N(x), N(y), N(z));
	}

	template<class Archive>
	void serialize(Archive & archive,
		Vector4 & m)
	{
		float& x=m.x,&y=m.y,&z=m.z,&w=m.w;
		archive(N(x), N(y), N(z),N(w));
	}
}