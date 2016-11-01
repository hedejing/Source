#include "SceneNode.h"
#include "SceneManager.h"
#include "Entity.h"

namespace HW
{
	NameGenerator* SceneNode::m_nameGenerator = new NameGenerator("SceneNode");

	//--------------------------------------------------------------------------
	SceneNode::SceneNode()
		: m_parent(0),
		m_BBoxCurrent(false),
		m_orientationCurrent(false),
		m_transCurrent(false),
		m_scaleCurrent(false),
		m_translation(Vector3::ZERO),
		m_orientation(Quaternion::IDENTITY),
		m_scale(Vector3::UNIT_SCALE),
		m_BSphereCurrent(false),
		children_changed(false),
		m_globaBBox_current(false),
		m_globalSphere_current(false),
		m_bShadow(false)
	{
		
		m_Name = m_nameGenerator->Generate();
		m_boundingSphere.setRadius(0);
		m_globalSphere.setRadius(0);
		//std::cout<<"SceneNode "<<m_Name<<" Construct"<<std::endl;
	}

	//--------------------------------------------------------------------------
	SceneNode::SceneNode(const string& name,SceneManager * creater):m_parent(0),
		m_BBoxCurrent(false),
		m_orientationCurrent(false),
		m_transCurrent(false),
		m_scaleCurrent(false),
		m_translation(Vector3::ZERO),
		m_orientation(Quaternion::IDENTITY),
		m_scale(Vector3::UNIT_SCALE),
		m_Name(name),
		m_BSphereCurrent(false),
		children_changed(false),
		m_creater(creater),
		m_globaBBox_current(false),
		m_globalSphere_current(false)
	{
		m_boundingSphere.setRadius(0);
		m_globalSphere.setRadius(0);
		if(m_Name.empty())
			m_Name = m_nameGenerator->Generate();
		//std::cout<<"SceneNode "<<m_Name<<" Construct"<<std::endl;
	}

	//--------------------------------------------------------------------------
	void SceneNode::detachMovable(const string &name)
	{
		std::map<string,Movable*>::iterator it = m_movables.find(name);
		if( it != m_movables.end())
		{
			Movable::MovableType type = it->second->getMovableType();
			if(type == Movable::MT_ENTITY)
			{
				for (std::list<Entity*>::iterator iter = m_entities.begin(); iter!=m_entities.end(); iter++)
				{
					if(!(*iter)->getName().compare( name))
					{
						m_entities.erase(iter);
					}
				}
			}
			m_movables.erase(it);
		}
	}

	void SceneNode::thisNodeMoved()
	{
		WorldMatrixDirty = true;
		m_transCurrent = false;
		m_scaleCurrent = false;
		m_orientationCurrent = false;
		m_BBoxCurrent = false;
		m_BSphereCurrent = false;
		notifyChildForMove();
		
		notifyMovablesForMove();
		
		m_creater->addChangedNode(this);
		
	}

	const BoundingBox& SceneNode::getBoundingBox()
	{
		if(m_BBoxCurrent)
			return m_boundingBox;
		// merge bounding box of the movables

		if(!m_entities.empty())
		{
			std::list<Entity*>::iterator it = m_entities.begin();
			m_boundingBox = (*it)->getBoundingBox();
			it++; 

			while( it != m_entities.end())
			{
				m_boundingBox.merge((*it)->getBoundingBox());
				it++;
			}
		}
		else
		{
			m_boundingBox.setNull();
		}
		m_BBoxCurrent = true;
		return m_boundingBox;
	}

	float SceneNode::getBoundingRadius()
	{
		if(m_BSphereCurrent)
			return m_boundingSphere.getRadius();
		if( !m_entities.empty())
		{
			getBoundingSphere();
			return m_boundingSphere.getRadius();
		}
		else
		{
			return 0;
		}
	}

	float SceneNode::getGlobalBoudingRadius()
	{
		if (m_globalSphere_current)
			return m_globalSphere.getRadius();
		if (!m_entities.empty() ||!m_childNodes.empty())
		{
			m_globalSphere = getBoundingSphere();
			std::map<string, SceneNode *>::iterator it = m_childNodes.begin();
			while (it != m_childNodes.end())
			{
				m_boundingSphere.merge((it->second)->getGlobalBoundingSphere());
				it++;
			}
			m_globalSphere_current = true;
			return m_globalSphere.getRadius();
		}
		else
		{
			return 0;
		}
	}

	const Sphere& SceneNode::getBoundingSphere()
	{
		if(m_BSphereCurrent)
			return m_boundingSphere;
		if( !m_entities.empty())
		{
			std::list<Entity*>::iterator it = m_entities.begin();
			m_boundingSphere = (*it)->getBoundingSphere();
			it++;
			while( it != m_entities.end())
			{
				m_boundingSphere.merge((*it)->getBoundingSphere());
				it ++;
			}
			m_BSphereCurrent = true;
			return m_boundingSphere;
		}
		else
		{
			return m_boundingSphere;
		}
	}

	const Sphere& SceneNode::getGlobalBoundingSphere()
	{
		if (m_globalSphere_current)
			return m_globalSphere;
		if (!m_entities.empty() || !m_childNodes.empty())
		{
			
			m_globalSphere = getBoundingSphere();
			std::map<string, SceneNode *>::iterator it = m_childNodes.begin();
			while (it != m_childNodes.end())
			{
				m_boundingSphere.merge((it->second)->getGlobalBoundingSphere());
				it++;
			}
			m_globalSphere_current = true;
			return m_globalSphere;
		}
		else
		{
			return m_globalSphere;
		}
	}


	const Vector3& SceneNode::getBoundingCenter()
	{
		if(m_BSphereCurrent)
			return m_boundingSphere.getCenter();
		if( !m_entities.empty())
		{
			getBoundingSphere();
			return m_boundingSphere.getCenter();
		}
		else
		{
			assert(false);
			return Vector3::ZERO;
		}
	}


	const Vector3& SceneNode::getGlobalBoundingCenter()
	{
		if (m_globalSphere_current)
			return m_globalSphere.getCenter();
		if (!m_entities.empty() || !m_childNodes.empty())
		{
			m_globalSphere = getBoundingSphere();
			for (std::map<string, SceneNode *>::iterator itr = m_childNodes.begin(); itr != m_childNodes.end(); itr++)
			{
				m_globalSphere.merge(itr->second->getGlobalBoundingSphere());
			}

			return m_globalSphere.getCenter();
		}
		else
		{
			assert(false);
			return Vector3::ZERO;
		}
	}

	void SceneNode::attachMovable( Entity* movable )
	{
		m_entities.push_back(movable);
		attachMovable(static_cast<Movable *>(movable));
	}

	//--------------------------------------------------------------------------

}