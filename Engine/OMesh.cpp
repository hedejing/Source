#include "OMesh.h"

namespace HW
{
	OMesh::OMesh(const string &name)
		: m_Name(name),
		m_boundingBoxCurrent(false),
		m_boundingSphereCurrent(false)
	{
		//std::cout<<"Omesh "<<m_Name<<" Construct"<<std::endl;
	}

	OMesh::~OMesh()
	{
		//std::cout<<"Omesh "<<m_Name<<" Destruct"<<std::endl;
		deleteAllSubMeshes();
	}

	BoundingBox& OMesh::getModelBoundingBox()
	{
		if(m_boundingBoxCurrent)
			return m_modelBoundingBox;
		//if(!m_SubMeshList.empty())
		//{
		//	//Vector3 maxp = m_SubMeshList.begin()->second->getPositions()[0];
		//	//Vector3 mimp = maxp;
		//	for (std::map<String,Renderable*>::iterator it = m_SubMeshList.begin(); it != m_SubMeshList.end(); it++)
		//	{
		//		m_modelBoundingBox.merge(it->second->bbox);
		//	//	const std::vector<Vector3>& mlist = it->second->getPositions();
		//	//	for (std::vector<Vector3>::const_iterator iter = mlist.begin(); iter != mlist.end(); iter++)
		//	//	{
		//	//		maxp.makeCeil(*iter);
		//	//		mimp.makeFloor(*iter);
		//	//	}
		//	}
		//	//m_modelBoundingBox.fromPoints(maxp,mimp);
		//}
		m_boundingBoxCurrent = true;
		return m_modelBoundingBox;
	}

	Sphere& OMesh::getModelBoundingSphere()
	{
		//if(m_boundingSphereCurrent)
		//	return m_modelSphere;
		//if(!m_SubMeshList.empty())
		//{
		//	Vector3 maxp = m_SubMeshList.begin()->second->getPositions()[0];
		//	Vector3 mimp = maxp;
		//	for (std::map<String,Renderable*>::iterator it = m_SubMeshList.begin(); it != m_SubMeshList.end(); it++)
		//	{
		//		const std::vector<Vector3>& mlist = it->second->getPositions();
		//		for (std::vector<Vector3>::const_iterator iter = mlist.begin(); iter != mlist.end(); iter++)
		//		{
		//			maxp.makeCeil(*iter);
		//			mimp.makeFloor(*iter);
		//		}
		//	}
		//	Vector3 tCenter = (maxp + mimp)*0.5f;
		//	float  tRadius = (maxp - mimp).length()*0.5f;
		//	m_modelSphere.setCenter(tCenter);
		//	m_modelSphere.setRadius(tRadius);
		//}
		//m_boundingSphereCurrent = true;
		return m_modelSphere;
	}

	/*void OMesh::setMaterialGpuProgram(unsigned int passid,GpuProgramPtr program)
	{
		//��ʱΪ����ͨ��ע�ͣ���֪�Ƿ����ã�Ӧ��û��
		if(!m_SubMeshList.empty())
		{
			for (auto it = m_SubMeshList.begin(); it != m_SubMeshList.end(); it++)
			{
				it->second->getMaterial()->GetPassByIndex(passid)->bindGpuProgram(program);
			}
		}
	}*/

	void OMesh::setWireFrame(bool set /* = true */)
	{
		//��ʱΪ����ͨ��ע�ͣ���֪�Ƿ����ã�Ӧ��û��
		/*if(!m_SubMeshList.empty())
		{
			for (auto it = m_SubMeshList.begin(); it != m_SubMeshList.end(); it++)
			{
				it->second->getMaterial()->ApplyWireFrame(set);  //material �޴˶���
			}
		}*/
	}

	void OMesh::releaseInternalRes()
	{
		//std::map<String,Renderable*>::iterator itr_renderable = m_SubMeshList.begin();
		//for(;itr_renderable != m_SubMeshList.end();itr_renderable++)
		//{
		//	// invalid renderable egl resource;
		//	itr_renderable->second->releaseInternalRes();
		//}
	}

	void OMesh::setLocalEffect(Effect * effect)
	{
		//std::map<String,Renderable *>::iterator itr_render = m_SubMeshList.begin();
		//for(;itr_render != m_SubMeshList.end();itr_render++)
		//{
		//	itr_render->second->getMaterial()->setMaterialEffect(effect);
		//}
	}

}