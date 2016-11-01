#ifndef HW_OCTREENODE_H
#define HW_OCTREENODE_H
#include "SceneNode.h"
#include<string>
namespace HW
{
	// octree node is a scenenode attached on an octree 
	// it is a subclass of scenenode,so it has tranformation
	// and entity attached on itself ,  scenenode  has 
	// children node ,but in octreenode ,it's children node 
	// should not be used

	class Octree;
	class SceneManager;

	class OctreeNode :public SceneNode
	{
	public:
		OctreeNode();
		OctreeNode(const string & m_name,SceneManager * creator);
		~OctreeNode();
		void setOctree(Octree * oct);
		Octree * getOctree();

	private:
		// the octree this node attached on
		Octree * m_octree;

	};
}
#endif