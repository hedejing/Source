#include "Movable.h"

namespace HW
{
	NameGenerator* Movable::m_NameGenerator = new NameGenerator("Movable");

	Movable::Movable()
		: m_MovableType(MT_UNKNOWN),
		m_Visible(false),
		m_parent(0),
		m_moved(true)
	{
		m_Name = m_NameGenerator->Generate();
		//std::cout<<"Moveable "<<m_Name<<" Construct"<<std::endl;
	}

	Movable::Movable(const string& name,MovableType type,SceneManager * m_creator)
		:	m_MovableType(type),
		m_Name(name),
		m_Visible(false),
		m_parent(0),
		m_moved(true),
		m_SceneMgr(m_creator)
	{
		if(m_Name.empty())
			m_Name = m_NameGenerator->Generate();
		//printf("Moveable %s Constructor",m_Name.c_str());
	}

}