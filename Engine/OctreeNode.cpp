#include "OctreeNode.h"
#include "Octree.h"

namespace HW
{
	OctreeNode::OctreeNode():SceneNode(),
		m_octree(NULL)
	{

	}

	OctreeNode::OctreeNode(const string & m_name,SceneManager * creator):SceneNode(m_name,creator),
		m_octree(NULL)
	{

	}
	void OctreeNode::setOctree(Octree * oct)
	{
		m_octree = oct;
	}

	Octree * OctreeNode::getOctree()
	{
		return m_octree;
	}

	
}