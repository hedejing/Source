#ifndef ss_math_BBox_H_
#define ss_math_BBox_H_

#include "ssPrerequisities.h"
#include "ssPoint3.h"
#include "ssVector3.h"
#include "ssMatrix4.h"
#include <iostream>
#include <stdlib.h>
namespace ssMath
{
	class BBox 
	{
	public:
		// 3 states of the BBox.
		enum Extent
		{
			EXTENT_NULL,
			EXTENT_FINITE,
			EXTENT_INFINITE
		};

	public:
		// BBox Constructors
		// initialize the box to null.
		BBox() 
		{
			m_Min = Point3(Vector3::ZERO);
			m_Max = Point3(Vector3::UNIT_SCALE);
			m_enumExtent = EXTENT_NULL;
		}

		BBox(Extent e)
		{
			m_Min = Point3(-0.5,-0.5,-0.5);
			m_Max = Point3(0.5,0.5,0.5);
			m_enumExtent = e;
		}

		BBox(const Point3 &p1, const Point3 &p2) {
			fromPoints(p1,p2);
		}
		// copy constructor
		BBox(const BBox& box)
		{
			if(box.isNull()) setNull();
			if(box.isInfinite()) setInfinite();
			if(box.isFinite())
			{
				m_Max = box.m_Max;
				m_Min = box.m_Min;
				m_enumExtent = box.m_enumExtent;
			}
		}
		const Point3& getMaximum() const 
		{
			return m_Max;
		}
		const Point3& getMinimum() const
		{
			return m_Min;
		}
		Point3 getCenter() const
		{
			assert(m_enumExtent == EXTENT_FINITE);
			return (m_Max + (Vector3)m_Min)*0.5f;
		}
		// BBox Method Declarations
		friend BBox Union(const BBox &b, const Point3 &p);
		friend BBox Union(const BBox &b, const BBox &b2);
		friend BBox Intersection(const BBox &b1, const BBox &b2);

		inline BBox intersection(const BBox& b2) const
		{
			if (this->isNull() || b2.isNull())
			{
				return BBox();
			}
			else if (this->isInfinite())
			{
				return b2;
			}
			else if (b2.isInfinite())
			{
				return *this;
			}

			Vector3 intMin = m_Min;
			Vector3 intMax = m_Max;

			intMin.makeCeil(b2.getMinimum());
			intMax.makeFloor(b2.getMaximum());

			// Check intersection isn't null
			if (intMin.x < intMax.x &&
				intMin.y < intMax.y &&
				intMin.z < intMax.z)
			{
				return BBox(intMin, intMax);
			}

			return BBox();
		}
		// test whether this box is totally contained by the given one.
		bool Overlaps(const BBox &b) const 
		{
			if(this->isNull() || b.isInfinite())
				return true;
			if(this->isInfinite() || b.isNull())
				return false;
			bool x = (m_Max.x >= b.m_Min.x) && (m_Min.x <= b.m_Max.x);
			bool y = (m_Max.y >= b.m_Min.y) && (m_Min.y <= b.m_Max.y);
			bool z = (m_Max.z >= b.m_Min.z) && (m_Min.z <= b.m_Max.z);
			return (x && y && z);
		}

		// test whether the given point is inside the volume.
		bool contains(const Point3 &v) const 
		{
			if (isNull())
				return false;
			if (isInfinite())
				return true;

			return m_Min.x <= v.x && v.x <= m_Max.x &&
				m_Min.y <= v.y && v.y <= m_Max.y &&
				m_Min.z <= v.z && v.z <= m_Max.z;
		}
		// test whether this box entirely contains the given box.
		bool contains(const BBox& other) const
		{
			if (other.isNull() || this->isInfinite())
				return true;

			if (this->isNull() || other.isInfinite())
				return false;

			return this->m_Min.x <= other.m_Min.x &&
				this->m_Min.y <= other.m_Min.y &&
				this->m_Min.z <= other.m_Min.z &&
				other.m_Max.x <= this->m_Max.x &&
				other.m_Max.y <= this->m_Max.y &&
				other.m_Max.z <= this->m_Max.z;
		}

		// @deprecated function derived from the earlier version.
		bool inside(const Point3 &pt) const
		{
			return (pt.x >= m_Min.x && pt.x <= m_Max.x &&
				pt.y >= m_Min.y && pt.y <= m_Max.y &&
				pt.z >= m_Min.z && pt.z <= m_Max.z);
		}

		void expand(float delta) 
		{
			m_Min -= Vector3(delta, delta, delta);
			m_Max += Vector3(delta, delta, delta);
		}

		inline float volume() const 
		{
			if(m_enumExtent == EXTENT_FINITE)
			{
				Vector3 d = m_Max - m_Min;
				return d.x * d.y * d.z;
			}
			else if(m_enumExtent == EXTENT_INFINITE)
			{
				return INFINITY;
			}
			else if(m_enumExtent == EXTENT_NULL)
			{
				return 0.0f;
			}
			
		}
		//test whether this BBox intersects with the given one.
		inline bool intersect(const BBox &box) const
		{
			// Early-fail for nulls
			if (this->isNull() || box.isNull())
				return false;

			// Early-success for infinites
			if (this->isInfinite() || box.isInfinite())
				return true;

			// Use up to 6 separating planes
			if (m_Max.x < box.m_Min.x)
				return false;
			if (m_Max.y < box.m_Min.y)
				return false;
			if (m_Max.z < box.m_Min.z)
				return false;

			if (m_Min.x > box.m_Max.x)
				return false;
			if (m_Min.y > box.m_Max.y)
				return false;
			if (m_Min.z > box.m_Max.z)
				return false;

			// otherwise, must be intersecting
			return true;
		}

		inline void setMinimumX(float x)
		{
			m_Min.x = x;
		}

		inline void setMinimumY(float y)
		{
			m_Min.y = y;
		}

		inline void setMinimumZ(float z)
		{
			m_Min.z = z;
		}

		inline void setMaximumX( float x )
		{
			m_Max.x = x;
		}

		inline void setMaximumY( float y )
		{
			m_Max.y = y;
		}

		inline void setMaximumZ( float z )
		{
			m_Max.z = z;
		}

 		void merge( const BBox& rhs )
		{
			// Do nothing if rhs null, or this is infinite
			if ((rhs.m_enumExtent == EXTENT_NULL) || (m_enumExtent == EXTENT_INFINITE))
			{
				return;
			}
			// Otherwise if rhs is infinite, make this infinite, too
			else if (rhs.m_enumExtent == EXTENT_INFINITE)
			{
				m_enumExtent = EXTENT_INFINITE;
			}
			// Otherwise if current null, just take rhs
			else if (m_enumExtent == EXTENT_NULL)
			{
				setExtents(rhs.m_Max,rhs.m_Min);
			}
			// Otherwise merge
			else
			{
				Vector3 min = m_Min;
				Vector3 max = m_Max;
				max.makeCeil(rhs.m_Max);
				min.makeFloor(rhs.m_Min);

				setExtents(max, min);
			}

		}

		inline void merge( const Point3 &p)
		{
			switch(m_enumExtent)
			{
			case EXTENT_FINITE:
				{
					Vector3 vec = m_Min;
					vec.makeFloor(p);
					m_Min = vec;
					vec = m_Max;
					vec.makeCeil(p);
					m_Max = vec;
				}
				return;
			case EXTENT_INFINITE:
				return;
			case EXTENT_NULL:
				setExtents(p,p);
				return;
			}
			assert(false);
		}

		bool intersect(const Ray &ray, float &hitt0, float &hitt1) const;

		bool intersect(const Sphere &sphere) const;

		bool intersect(const Plane &plane) const;

		inline Vector3 getSize() const
		{
			switch(m_enumExtent)
			{
			case EXTENT_FINITE:
				return m_Max - m_Min;
			case EXTENT_INFINITE:
				return Vector3(INFINITY,INFINITY,INFINITY);
			case EXTENT_NULL:
				return Vector3::ZERO;
			default:
				return Vector3::ZERO; // never reach
			}
		}

		Vector3 getHalfSize(void) const
		{
			switch (m_enumExtent)
			{
			case EXTENT_NULL:
				return Vector3::ZERO;

			case EXTENT_FINITE:
				return (m_Max - m_Min) * 0.5;

			case EXTENT_INFINITE:
				return Vector3(
					INFINITY,
					INFINITY,
					INFINITY);

			default: // shut up compiler
				assert( false && "Never reached" );
				return Vector3::ZERO;
			}
		}

		inline void setMinimum(const Point3 mini)
		{
			m_enumExtent = EXTENT_FINITE;
			m_Min = mini;
		}

		inline void setMaximum(const Point3 maxi)
		{
			m_enumExtent = EXTENT_FINITE;
			m_Max = maxi;
		}

		inline void setExtents(const Point3 &maxi,const Point3 &mini)
		{
			m_Max = maxi;
			m_Min = mini;
			m_enumExtent = EXTENT_FINITE;
		}

		inline BBox& fromPoints(const Point3 &p1,const Point3 &p2)
		{
			setMaximum(Point3( (std::max)( p1.x, p2.x ),
							   (std::max)( p1.y, p2.y ),
							   (std::max)( p1.z, p2.z )));
			setMinimum(Point3( (std::min)( p1.x, p2.x ),
							   (std::min)( p1.y, p2.y ),
							   (std::min)( p1.z, p2.z )));
			m_enumExtent = EXTENT_FINITE;
			return *this;
		}

		inline bool isNull() const
		{
			return (m_enumExtent == EXTENT_NULL);
		}

		inline bool isInfinite() const 
		{
			return (m_enumExtent == EXTENT_INFINITE);
		}

		inline bool isFinite() const
		{
			return (m_enumExtent == EXTENT_FINITE);
		}

		inline void setNull(void)
		{
			m_enumExtent = EXTENT_NULL;
		}

		inline void setInfinite(void)
		{
			m_enumExtent = EXTENT_INFINITE;
		}

		inline void setFinite(void)
		{
			m_enumExtent = EXTENT_FINITE;
		}

		bool operator== (const BBox& rhs) const
		{
			if (this->m_enumExtent != rhs.m_enumExtent)
				return false;

			if (!this->isFinite())
				return true;

			return this->m_Min == rhs.m_Min &&
				this->m_Max == rhs.m_Max;
		}

		bool operator!=(const BBox& rhs) const
		{
			return !(*this == rhs);
		}

		inline BBox& operator=(const BBox& box)
		{
			if(box.isNull())
			{
				setNull();
				return *this;
			}
			if(box.isInfinite())
			{
				setInfinite();
				return *this;
			}
			if(box.isFinite())
			{
				setMinimum(box.getMinimum());
				setMaximum(box.getMaximum());
				return *this;
			}
			assert(false && "never reach.");
			return *this;
		}

		inline void transformAffine(const Matrix4& m)
		{
			assert(m.isAffine());

			// Do nothing if current null or infinite
			if( m_enumExtent != EXTENT_FINITE )
				return;

			Vector3 center = getCenter();
			Vector3 halfSize = getHalfSize();

			Vector3 newCenter = m.transformAffine(center);
			Vector3 newHalfSize(
				abs(m[0][0]) * halfSize.x + abs(m[0][1]) * halfSize.y + abs(m[0][2]) * halfSize.z, 
				abs(m[1][0]) * halfSize.x + abs(m[1][1]) * halfSize.y + abs(m[1][2]) * halfSize.z,
				abs(m[2][0]) * halfSize.x + abs(m[2][1]) * halfSize.y + abs(m[2][2]) * halfSize.z);

			setExtents( newCenter + newHalfSize, newCenter - newHalfSize);
		}

		// output
		friend std::ostream & operator<<(std::ostream & out ,const BBox & bbox)
			{
		out<<"BondingBox: max("<<bbox.m_Max.x<<" "<<bbox.m_Max.y<<" "<<bbox.m_Max.z<<") "
			<<"min("<<bbox.m_Min.x<<" "<<bbox.m_Min.y<<" "<<bbox.m_Min.z<<") size("<<bbox.getSize().x<<
			" "<<bbox.getSize().y<<" "<<bbox.getSize().z<<"\n";
		return out;
	}

		// BBox Public Data
	protected:
		Point3 m_Min, m_Max;
		Extent m_enumExtent;
	};

	BBox Union(const BBox &b, const Point3 &p);
	BBox Union(const BBox &b, const BBox &b2);
	BBox Intersection(const BBox &b1, const BBox &b2);
}
#endif