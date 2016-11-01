#include "Light.h"
#include "Entity.h"
#include "Camera.h"
#include "GlobalVariablePool.h"
#include "TextureManager.h"
#define AddSemanticToGlobal(x,y,z) {temp = new _global_variable_descrip(sizeof(x));\
	(y).insert(sem_desc((z), temp)); }
#include "EngineUtil.h"
namespace HW
{
	_global_variable_descrip::_global_variable_descrip(
		unsigned int elemsize):m_ElemSize(elemsize),
		m_NumElems(0),memptr(0)
	{

	}

	//--------------------------------------------------------------
	_global_variable_descrip::~_global_variable_descrip()
	{
		if(memptr)
			::operator delete(memptr);
	}

	//--------------------------------------------------------------
	void _global_variable_descrip::SetVal(unsigned int semantic,unsigned int size,const void *valptr,unsigned int element_num )
	{
		if(size != m_ElemSize) return;

		// extract the index
		/*unsigned int index = (0x0000FFFF)&semantic;*/
		unsigned int index = _global_getSemanticIndex(semantic);

		// check if the memory has been allocated.
		if(memptr)
		{
			// check if we need to reallocate memory
			if(index + element_num > m_NumElems)
			{
				void *tmp = memptr;
				memptr = ::operator new((index + element_num)*m_ElemSize);
				memcpy(memptr,tmp,m_NumElems*m_ElemSize);
				::operator delete(tmp);
				m_NumElems = index + element_num;
			}
		}
		else
		{
			memptr = ::operator new((index + element_num)*m_ElemSize);
			m_NumElems = index + element_num;
		}

		memcpy(static_cast<unsigned char*>(memptr) + index*m_ElemSize,valptr,m_ElemSize* element_num);
	}


	//--------------------------------------------------------------
	void _global_variable_descrip::GetVal(unsigned int semantic,unsigned int size,void *valptr,unsigned int element_count)
	{
		if(size != m_ElemSize) return ;

		/*unsigned int index = (0x0000FFFF)&semantic;*/
		unsigned int index = _global_getSemanticIndex(semantic);

		unsigned int num_element = 1;
		if(index + element_count > m_NumElems)
		{
			//std::cout<<"warning :semantic "<<semantic<<" has less element than required "<<std::endl;
			num_element = m_NumElems - index;
		}
		memcpy(valptr,static_cast<unsigned char*>(memptr) + index*m_ElemSize,m_ElemSize * num_element);

	}


	GlobalVariablePool::GlobalVariablePool()
	{
	}

	GlobalVariablePool::~GlobalVariablePool()
	{
		if(!m_SemanticValueMap.empty())
		{
			std::map<GlobalVariableSemantic,_global_variable_descrip*>::iterator it = m_SemanticValueMap.begin();
			for (; it!= m_SemanticValueMap.end(); ++it)
				delete it->second;
		}
		m_SemanticValueMap.clear();
	}

	//--------------------------------------------------------------
	void GlobalVariablePool::initialize()
	{
		printf("enter gp initialize ");
		// initialize the matrices hints
		memset(&m_VariablePool,0,sizeof(m_VariablePool));
		m_VariablePool.WorldViewMatrixDirty = false;
		m_VariablePool.ViewProjMatrixDirty= false;
		m_VariablePool.WorldViewProjMatrixDirty = false;

		m_VariablePool.TrsWorldMatrixDirty = false;
		m_VariablePool.TrsViewMatrixDirty = false;
		m_VariablePool.TrsProjectMatrixDirty = false;
		m_VariablePool.TrsWorldViewMatrixDirty = false;
		m_VariablePool.TrsViewProjMatrixDirty= false;
		m_VariablePool.TrsWorldViewProjMatrixDirty = false;

		m_VariablePool.InvWorldMatrixDirty = false;
		m_VariablePool.InvViewMatrixDirty = false;
		m_VariablePool.InvProjectMatrixDirty = false;
		m_VariablePool.InvWorldViewMatrixDirty = false;
		m_VariablePool.InvViewProjMatrixDirty= false;
		m_VariablePool.InvWorldViewProjMatrixDirty = false;


		typedef std::pair<GlobalVariableSemantic,
			_global_variable_descrip*> sem_desc;
		// initialize the semantic values map
		// 1. material ambient
		printf("begin insert");
		_global_variable_descrip * temp = new _global_variable_descrip(sizeof(Vector4));
		printf("new vector 4 map size = %d temp = %d",m_SemanticValueMap.size(),temp);
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_MATERIAL_AMBIENT,temp));
		printf(" end insetr GVS_MATERIAL_AMBIENT");
		// 2. material diffuse
		temp = new _global_variable_descrip(sizeof(Vector4));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_MATERIAL_DIFFUSE,temp));
		// 3. material specular
		temp = new _global_variable_descrip(sizeof(Vector4));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_MATERIAL_SPECULAR,temp));
		// 4. material emissive
		temp = new _global_variable_descrip(sizeof(Vector4));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_MATERIAL_EMISSIVE,temp));
		// 5. material transparent
		temp = new _global_variable_descrip(sizeof(Vector4));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_MATERIAL_TRANSPARENT,temp));
		// 6. material opacity
		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_MATERIAL_OPACITY,temp));
		// 7. material shininess
		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_MATERIAL_SHININESS,temp));
		// 8. directional light direction
		temp = new _global_variable_descrip(sizeof(Vector3));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_DIRECTIONAL_LIGHT_DIRECTION,temp));
		// 9. directional light diffuse
		temp = new _global_variable_descrip(sizeof(Vector4));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_DIRECTIONAL_LIGHT_DIFFUSE,temp));
		// 10. directional light specular
		temp = new _global_variable_descrip(sizeof(Vector4));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_DIRECTIONAL_LIGHT_SPECULAR,temp));
		// 11. point light position
		temp = new _global_variable_descrip(sizeof(Vector3));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_POINT_LIGHT_POSITION,temp));
		// 12. point light diffuse
		temp = new _global_variable_descrip(sizeof(Vector4));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_POINT_LIGHT_DIFFUSE,temp));
		// 13. point light specular
		temp = new _global_variable_descrip(sizeof(Vector4));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_POINT_LIGHT_SPECULAR,temp));
		// 14. point light const att
		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_POINT_LIGHT_CONST_ATT,temp));
		// 15. point light linear att
		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_POINT_LIGHT_LINEAR_ATT,temp));
		// 16. point light quadratic att
		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_POINT_LIGHT_QUAD_ATT,temp));
		// 17. spot light position
		temp = new _global_variable_descrip(sizeof(Vector3));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_SPOT_LIGHT_POSITION,temp));
		// 18. spot light direction
		temp = new _global_variable_descrip(sizeof(Vector3));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_SPOT_LIGHT_DIRECTION,temp));
		// 19. spot light diffuse
		temp = new _global_variable_descrip(sizeof(Vector4));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_SPOT_LIGHT_DIFFUSE,temp));
		// 20. spot light specular
		temp = new _global_variable_descrip(sizeof(Vector4));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_SPOT_LIGHT_SPECULAR,temp));
		// 21. spot light const att
		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_SPOT_LIGHT_CONST_ATT,temp));
		// 22. spot light linear att
		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_SPOT_LIGHT_LINEAR_ATT,temp));
		// 23. spot light quadratic att
		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_SPOT_LIGHT_QUAD_ATT,temp));
		// 24. spot light exponent
		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_SPOT_LIGHT_EXP,temp));
		// 25. spot light cut off
		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_SPOT_LIGHT_CUTOFF,temp));
		AddSemanticToGlobal(unsigned int, m_SemanticValueMap, GVS_ACTIVATE_LIGHTNUM)
		AddSemanticToGlobal(int, m_SemanticValueMap, GVS_HAS_TEXTURE_DIFFUSE)
		// 26. system elapse time
		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_SYSTEM_ELAPSE_TIME,temp));
		// 27. system 
		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_SYSTEM_DELTA_TIME,temp));
		// 28. camera position
		temp = new _global_variable_descrip(sizeof(Vector3));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_CAMERA_POSITION,temp));
		// 29. camera direction
		temp = new _global_variable_descrip(sizeof(Vector3));
		m_SemanticValueMap.insert(std::pair<GlobalVariableSemantic,
			_global_variable_descrip*>(GVS_CAMERA_DIRECTION,temp));
		// 30. camera up vector
		temp = new _global_variable_descrip(sizeof(Vector3));
		m_SemanticValueMap.insert(std::make_pair(GVS_CAMERA_UP,temp));
		// 31. environment ambient
		temp = new _global_variable_descrip(sizeof(Vector4));
		m_SemanticValueMap.insert(std::make_pair(GVS_ENV_AMBIENT,temp));

		temp = new _global_variable_descrip(sizeof(Matrix4));
		m_SemanticValueMap.insert(std::make_pair(GVS_WORLD_VIEW_PROJECTION_MATRIX,temp));

		temp = new _global_variable_descrip(sizeof(Matrix4));
		m_SemanticValueMap.insert(std::make_pair(GVS_SHADOW_MATRIX,temp));

		temp = new _global_variable_descrip(sizeof(Texture *));
		m_SemanticValueMap.insert(std::make_pair(GVS_SHADOW_MAP,temp));

		temp = new _global_variable_descrip(sizeof(float));
		m_SemanticValueMap.insert(std::make_pair(GVS_SHADOW_BIAS,temp));

		temp = new _global_variable_descrip(sizeof(Texture *));
		m_SemanticValueMap.insert(std::make_pair(GVS_POST_TEXTURE,temp));

		printf("end insert ");

	}

	//----------------------------------------------------------------------------------------------
	GlobalVariablePool* GlobalVariablePool::m_globalpool = NULL;

	GlobalVariablePool* GlobalVariablePool::GetSingletonPtr()
	{
		if(!m_globalpool)
		{
			m_globalpool = new GlobalVariablePool();
		}
		return m_globalpool;
	}

	//----------------------------------------------------------------------------------------------
	GlobalVariablePool& GlobalVariablePool::GetSingleton()
	{
		if(!m_globalpool)
		{
			m_globalpool = new GlobalVariablePool();
		}
		return *m_globalpool;
	}

	void GlobalVariablePool::GetVal(unsigned int semantic,unsigned int size,void *valptr,unsigned int element_count) const
	{
		// extract the 'real' semantic
		GlobalVariableSemantic key = _global_getSemanticKey(semantic);
		switch(key)
		{
		case GVS_WORLD_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				memcpy(valptr,m_VariablePool.WorldMatrix.ptr(),size * element_count);
			}
			break;

		case GVS_VIEW_MATRIX :
			{
				if(size != sizeof(Matrix4))
					return;
				memcpy(valptr,m_VariablePool.ViewMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_PROJECTION_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				memcpy(valptr,m_VariablePool.ProjectMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_WORLD_VIEW_MATRIX :
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.WorldViewMatrixDirty)
				{
					m_VariablePool.WorldViewMatrix = m_VariablePool.ViewMatrix*m_VariablePool.WorldMatrix;
					m_VariablePool.WorldViewMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.WorldViewMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_VIEW_PROJECTION_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.ViewProjMatrixDirty)
				{
					m_VariablePool.ViewProjMatrix = m_VariablePool.ProjectMatrix*m_VariablePool.ViewMatrix;
					m_VariablePool.ViewProjMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.ViewProjMatrix.ptr(),size* element_count);
			}
			break;

		case GVS_WORLD_VIEW_PROJECTION_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.WorldViewProjMatrixDirty)
				{
					m_VariablePool.WorldViewProjMatrix = m_VariablePool.ProjectMatrix*
						m_VariablePool.ViewMatrix*m_VariablePool.WorldMatrix;
					m_VariablePool.WorldViewProjMatrixDirty =false;
				}
				memcpy(valptr,m_VariablePool.WorldViewProjMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_TRANSPOSE_WORLD_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.TrsWorldMatrixDirty)
				{
					m_VariablePool.TrsWorldMatrix = m_VariablePool.WorldMatrix.transpose();
					m_VariablePool.TrsWorldMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.TrsWorldMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_TRANSPOSE_VIEW_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.TrsViewMatrixDirty)
				{
					m_VariablePool.TrsViewMatrix = m_VariablePool.ViewMatrix.transpose();
					m_VariablePool.TrsViewMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.TrsViewMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_TRANSPOSE_PROJECTION_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.TrsProjectMatrixDirty)
				{
					m_VariablePool.TrsProjectMatrix = m_VariablePool.ProjectMatrix.transpose();
					m_VariablePool.TrsProjectMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.TrsProjectMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_TRANSPOSE_WORLD_VIEW_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.TrsWorldViewMatrixDirty)
				{
					m_VariablePool.TrsWorldViewMatrix = (m_VariablePool.ViewMatrix*m_VariablePool.WorldMatrix).transpose();
					m_VariablePool.TrsWorldViewMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.TrsWorldViewMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_TRANSPOSE_VIEW_PROJECTION_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.TrsViewProjMatrixDirty)
				{
					m_VariablePool.TrsViewProjMatrix = (m_VariablePool.ProjectMatrix*m_VariablePool.ViewMatrix).transpose();
					m_VariablePool.TrsViewProjMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.TrsViewProjMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.TrsWorldViewProjMatrixDirty)
				{
					m_VariablePool.TrsWorldViewProjMatrix = (m_VariablePool.ProjectMatrix*m_VariablePool.ViewMatrix
						*m_VariablePool.WorldMatrix).transpose();
					m_VariablePool.TrsWorldViewProjMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.TrsWorldViewProjMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_INVERSE_WORLD_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.InvWorldMatrixDirty)
				{
					m_VariablePool.InvWorldMatrix = m_VariablePool.WorldMatrix.inverse();
					m_VariablePool.InvWorldMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.InvWorldMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_INVERSE_VIEW_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.InvViewMatrixDirty)
				{
					m_VariablePool.InvViewMatrix = m_VariablePool.ViewMatrix.inverse();
					m_VariablePool.InvViewMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.InvViewMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_INVERSE_PROJECTION_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.InvProjectMatrixDirty)
				{
					m_VariablePool.InvProjectMatrix = m_VariablePool.ProjectMatrix.inverse();
					m_VariablePool.InvProjectMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.InvProjectMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_INVERSE_WORLD_VIEW_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.InvWorldViewMatrixDirty)
				{
					m_VariablePool.InvWorldViewMatrix = (m_VariablePool.ViewMatrix*m_VariablePool.WorldMatrix).inverse();
					m_VariablePool.InvWorldViewMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.InvWorldViewMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_INVERSE_VIEW_PROJECTION_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.InvViewProjMatrixDirty)
				{
					m_VariablePool.InvViewProjMatrix = (m_VariablePool.ProjectMatrix*m_VariablePool.ViewMatrix).inverse();
					m_VariablePool.InvViewProjMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.InvViewProjMatrix.ptr(),size* element_count);
			}
			break;
		case GVS_INVERSE_WORLD_VIEW_PROJECTION_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				if(m_VariablePool.InvWorldViewProjMatrixDirty)
				{
					m_VariablePool.InvWorldViewProjMatrix = (m_VariablePool.ProjectMatrix*m_VariablePool.ViewMatrix
						*m_VariablePool.ProjectMatrix).inverse();
					m_VariablePool.InvWorldViewProjMatrixDirty = false;
				}
				memcpy(valptr,m_VariablePool.InvWorldViewProjMatrix.ptr(),size* element_count);
			}
			break;
			// class 2 material
		case GVS_MATERIAL_AMBIENT:
			{
				if(size != sizeof(Vector4))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_MATERIAL_DIFFUSE:
			{
				if(size != sizeof(Vector4))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_MATERIAL_SPECULAR:
			{
				if(size != sizeof(Vector4))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_MATERIAL_EMISSIVE:
		case GVS_MATERIAL_TRANSPARENT :
			{
				if(size != sizeof(Vector4))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_MATERIAL_OPACITY :
			{
				if(size != sizeof(float))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_MATERIAL_SHININESS :
		case GVS_INTER_FACTOR:
		case GVS_SHADOW_BIAS:
			{
				if(size != sizeof(float))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_TEXTURE_DIFFUSE:
		case GVS_TEXTURE_NORMAL:
		case GVS_TEXTURE_SPECULAR:
		case GVS_TEXTURE_AMBIENT:
		case GVS_SHADOW_MAP:
		case GVS_POST_TEXTURE:
			{
				if(size != sizeof(Texture *))
					return ;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}break;
			// class 3 light
			// directional
		case GVS_DIRECTIONAL_LIGHT_DIRECTION:
			// point light
		case GVS_POINT_LIGHT_POSITION:

		case GVS_SPOT_LIGHT_POSITION :
		case GVS_SPOT_LIGHT_DIRECTION :
			{
				if(size != sizeof(Vector3))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_POINT_LIGHT_CONST_ATT:
		case GVS_POINT_LIGHT_LINEAR_ATT:
		case GVS_POINT_LIGHT_QUAD_ATT:
			{
				if(size != sizeof(float))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
			//spot
		case GVS_SPOT_LIGHT_DIFFUSE:
		case GVS_SPOT_LIGHT_SPECULAR:
		case GVS_DIRECTIONAL_LIGHT_DIFFUSE:
		case GVS_DIRECTIONAL_LIGHT_SPECULAR :
		case GVS_POINT_LIGHT_DIFFUSE:
		case GVS_POINT_LIGHT_SPECULAR:
			{
				if(size != sizeof(Vector4))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_SPOT_LIGHT_CONST_ATT :
		case GVS_SPOT_LIGHT_LINEAR_ATT :
		case GVS_SPOT_LIGHT_QUAD_ATT:
		case GVS_SPOT_LIGHT_EXP:
		case GVS_SPOT_LIGHT_CUTOFF:
		case GVS_POINT_LIGHT_DISTANCE :
		case GVS_SPOT_LIGHT_DISTANCE :
			{
				if(size != sizeof(float))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
			// class 5 system time
		case GVS_SYSTEM_ELAPSE_TIME:
		case GVS_SYSTEM_DELTA_TIME:
			{
				if(size != sizeof(float))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
			// class 6 camera
		case GVS_CAMERA_POSITION :
		case GVS_CAMERA_DIRECTION :
		case GVS_CAMERA_UP :
			{
				if(size != sizeof(Vector3))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_ENV_AMBIENT:
			{
				if(size != sizeof(Vector4))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::const_iterator it = m_SemanticValueMap.find(key);
				if( it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_SHADOW_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip *>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}break;
		case GVS_BONE_MATRICES:
			{
				if(size != sizeof(Matrix4))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip *>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr,element_count);
				}
			}break;
		case  GVS_FRAME_ANIMATION:
		case GVS_HAS_TEXTURE_DIFFUSE:
		case GVS_ACTIVATE_LIGHTNUM:
			{
				if(size != sizeof(unsigned int))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip *>::const_iterator it = m_SemanticValueMap.find(key);
				if(it != m_SemanticValueMap.end())
				{
					it->second->GetVal(semantic,size,valptr);
				}
			}break;
		default:break;
		}
	}

	//----------------------------------------------------------------------------------------------
	void GlobalVariablePool::SetWorldMatrix(const Matrix4 &mat)
	{
		m_VariablePool.WorldMatrix = mat;

		m_VariablePool.WorldViewMatrixDirty = true;
		m_VariablePool.WorldViewProjMatrixDirty = true;

		m_VariablePool.TrsWorldMatrixDirty = true;
		m_VariablePool.TrsWorldViewProjMatrixDirty = true;
		m_VariablePool.TrsWorldViewMatrixDirty = true;

		m_VariablePool.InvWorldMatrixDirty = true;
		m_VariablePool.InvWorldViewMatrixDirty = true;
		m_VariablePool.InvWorldViewProjMatrixDirty = true;
	}

	//----------------------------------------------------------------------------------------------
	void GlobalVariablePool::SetViewMatrix(const Matrix4& mat)
	{
		m_VariablePool.ViewMatrix = mat;

		m_VariablePool.WorldViewMatrixDirty = true;
		m_VariablePool.WorldViewProjMatrixDirty = true;

		m_VariablePool.TrsViewMatrixDirty = true;
		m_VariablePool.TrsWorldViewMatrixDirty = true;
		m_VariablePool.TrsWorldViewProjMatrixDirty = true;

		m_VariablePool.InvViewMatrixDirty = true;
		m_VariablePool.InvWorldViewMatrixDirty = true;
		m_VariablePool.InvWorldViewProjMatrixDirty = true;
	}

	//----------------------------------------------------------------------------------------------
	void GlobalVariablePool::SetProjectMatrix(const Matrix4 &mat)
	{
		m_VariablePool.ProjectMatrix = mat;

		m_VariablePool.ViewProjMatrixDirty = true;
		m_VariablePool.WorldViewProjMatrixDirty = true;

		m_VariablePool.TrsProjectMatrixDirty = true;
		m_VariablePool.TrsViewProjMatrixDirty = true;
		m_VariablePool.TrsWorldViewProjMatrixDirty = true;

		m_VariablePool.InvProjectMatrixDirty = true;
		m_VariablePool.InvViewProjMatrixDirty = true;
		m_VariablePool.InvWorldViewProjMatrixDirty = true;
	}

	//----------------------------------------------------------------------------------------------
	void GlobalVariablePool::SetVal(unsigned int semantic,unsigned int size,void *valptr)
	{
		GlobalVariableSemantic sem = _global_getSemanticKey(semantic);
		switch(sem)
		{
			// class 1 matrix
		case GVS_WORLD_MATRIX :
			{
				if(size != sizeof(Matrix4))
					return;
				Matrix4 tmpma;
				memcpy(const_cast<float*>(tmpma.ptr()),valptr,sizeof(Matrix4));
				SetWorldMatrix(tmpma);
			}
			break;
		case GVS_VIEW_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				Matrix4 tmpma;
				memcpy(const_cast<float*>(tmpma.ptr()),valptr,sizeof(Matrix4));
				SetViewMatrix(tmpma);
			}
			break;
		case GVS_PROJECTION_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				Matrix4 tmpma;
				memcpy(const_cast<float*>(tmpma.ptr()),valptr,sizeof(Matrix4));
				SetProjectMatrix(tmpma);
			}
			break;
		case GVS_WORLD_VIEW_PROJECTION_MATRIX :
		case GVS_WORLD_VIEW_MATRIX:
		case GVS_VIEW_PROJECTION_MATRIX:
		case GVS_TRANSPOSE_WORLD_MATRIX :
		case GVS_TRANSPOSE_VIEW_MATRIX:
		case GVS_TRANSPOSE_PROJECTION_MATRIX :
		case GVS_TRANSPOSE_WORLD_VIEW_MATRIX:
		case GVS_TRANSPOSE_VIEW_PROJECTION_MATRIX :
		case GVS_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX:
		case GVS_INVERSE_WORLD_MATRIX:
		case GVS_INVERSE_VIEW_MATRIX:
		case GVS_INVERSE_PROJECTION_MATRIX:
		case GVS_INVERSE_WORLD_VIEW_MATRIX:
		case GVS_INVERSE_VIEW_PROJECTION_MATRIX:
		case GVS_INVERSE_WORLD_VIEW_PROJECTION_MATRIX:
			break;
			// class 2 material
		case GVS_DIRECTIONAL_LIGHT_DIRECTION:

			// point light
		case GVS_POINT_LIGHT_POSITION:

		case GVS_SPOT_LIGHT_POSITION :
		case GVS_SPOT_LIGHT_DIRECTION:
		
		case GVS_CAMERA_POSITION:
		case GVS_CAMERA_DIRECTION:
		case GVS_CAMERA_UP:
			{
				if(size != sizeof(Vector3))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::iterator it = m_SemanticValueMap.find(sem);
				if( it != m_SemanticValueMap.end())
				{
					it->second->SetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_MATERIAL_AMBIENT:
		case GVS_MATERIAL_DIFFUSE:
		case GVS_MATERIAL_SPECULAR:
		case GVS_MATERIAL_EMISSIVE:
		case GVS_DIRECTIONAL_LIGHT_DIFFUSE:
		case GVS_DIRECTIONAL_LIGHT_SPECULAR:
		case GVS_POINT_LIGHT_DIFFUSE :
		case GVS_POINT_LIGHT_SPECULAR :
		case GVS_SPOT_LIGHT_DIFFUSE:
		case GVS_SPOT_LIGHT_SPECULAR:
		case GVS_MATERIAL_TRANSPARENT:
			{
				if(size != sizeof(Vector4))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::iterator it = m_SemanticValueMap.find(sem);
				if( it != m_SemanticValueMap.end())
				{
					it->second->SetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_MATERIAL_OPACITY:
		case GVS_POINT_LIGHT_CONST_ATT :
		case GVS_POINT_LIGHT_LINEAR_ATT :
		case GVS_POINT_LIGHT_QUAD_ATT :
		case GVS_SPOT_LIGHT_CONST_ATT:
		case GVS_SPOT_LIGHT_LINEAR_ATT:
		case GVS_SPOT_LIGHT_QUAD_ATT:
		case GVS_SPOT_LIGHT_EXP:
		case GVS_SPOT_LIGHT_CUTOFF:
		case GVS_POINT_LIGHT_DISTANCE:
		case GVS_SPOT_LIGHT_DISTANCE:
		case GVS_SYSTEM_ELAPSE_TIME:
		case GVS_SYSTEM_DELTA_TIME:
		case GVS_MATERIAL_SHININESS:
		case GVS_SHADOW_BIAS:
			{
				if(size != sizeof(float))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::iterator it = m_SemanticValueMap.find(sem);
				if( it != m_SemanticValueMap.end())
				{
					it->second->SetVal(semantic,size,valptr);
				}
			}
			break;
		case GVS_ENV_AMBIENT:
			{
				if(size != sizeof(Vector4))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip*>::iterator it = m_SemanticValueMap.find(sem);
				if( it != m_SemanticValueMap.end())
				{
					it->second->SetVal(semantic,size,valptr);
				}
		   } break;
		case GVS_SHADOW_MATRIX:
			{
				if(size != sizeof(Matrix4))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip *>::iterator it = m_SemanticValueMap.find(sem);
				if(it != m_SemanticValueMap.end())
				{
					it->second->SetVal(semantic,size,valptr);
				}
		    } break;
		case  GVS_SHADOW_MAP:
		case GVS_POST_TEXTURE:
			{
				if(size != sizeof(Texture *))
					return;
				std::map<GlobalVariableSemantic,_global_variable_descrip *>::iterator it =m_SemanticValueMap.find(sem);
				if( it != m_SemanticValueMap.end())
				{
					it->second->SetVal(semantic,size,valptr);
				}
		   } break;
		case GVS_ACTIVATE_LIGHTNUM:
		   {
			   if (size != sizeof(unsigned))
				   return;
			   std::map<GlobalVariableSemantic, _global_variable_descrip *>::iterator it = m_SemanticValueMap.find(sem);
			   if (it != m_SemanticValueMap.end())
			   {
				   it->second->SetVal(semantic, size, valptr);
			   }
		   } break;
		default:;

		}
	}

	void GlobalVariablePool::SetMaterialTexture(GlobalVariableSemantic sem, string tex_type, as_Material * material)
	{
		auto it = m_SemanticValueMap.find(sem);
		auto tex = material->textures.find(tex_type);
		if (tex != material->textures.end()) {//找到了texture后的处理，此处需要简化
			auto& tm = TextureManager::getInstance();
			string purename = FileUtil::getPureFileName(tex->second.path);
			Texture* t = tm.get(purename);

			if (it != m_SemanticValueMap.end())
			{
				it->second->SetVal(sem, sizeof(Texture *), &(t));
			}
			else
			{
				_global_variable_descrip * temp = new _global_variable_descrip(sizeof(Texture *));
				temp->SetVal(sem, sizeof(Texture *), &(t));
				Texture * texture[1];
				temp->GetVal(sem, sizeof(Texture *), texture);

				m_SemanticValueMap.insert(std::make_pair(sem, temp));
			}
		}
		else {
			auto& tm = TextureManager::getInstance();
			Texture* t = tm.get("white_texture.jpg");
			if (it != m_SemanticValueMap.end())
			{
				it->second->SetVal(sem, sizeof(Texture *), &(t));
			}
			else
			{
				_global_variable_descrip * temp = new _global_variable_descrip(sizeof(Texture *));
				temp->SetVal(sem, sizeof(Texture *), &(t));
				Texture * texture[1];
				temp->GetVal(sem, sizeof(Texture *), texture);

				m_SemanticValueMap.insert(std::make_pair(sem, temp));
			}
		}
	}

	void GlobalVariablePool::SetMaterialValue(GlobalVariableSemantic sem, Vector4 v)
	{

		auto it = m_SemanticValueMap.find(sem);
		if (it != m_SemanticValueMap.end())
		{
			it->second->SetVal(sem, sizeof(Vector4), v.ptr());
		}
		else
			assert(false && "set material diffuse error");
	}
	void GlobalVariablePool::SetMaterialValue(GlobalVariableSemantic sem, int v)
	{

		auto it = m_SemanticValueMap.find(sem);
		if (it != m_SemanticValueMap.end())
		{
			it->second->SetVal(sem, sizeof(int), &v);
		}
		else
			assert(false && "set material value error");
	}
	void GlobalVariablePool::SetMaterial(as_Material * material)
	{
		if (material == NULL) return;
		Vector4 diffuse_color = Vector4(material->diffuse);
		auto d = material->textures.find("diffuse");
		if (d == material->textures.end())
			diffuse_color.w = 0;
		else
			diffuse_color.w = 1;

		SetMaterialValue(GVS_MATERIAL_DIFFUSE, diffuse_color);
		SetMaterialTexture(GVS_TEXTURE_DIFFUSE, "diffuse", material);
		//int has_diffuse = 1;
		//auto d = material->textures["diffuse"];
		//if (d == NULL) has_diffuse = 0;
		//SetMaterialValue(GVS_HAS_TEXTURE_DIFFUSE, has_diffuse);
		SetMaterialValue(GVS_MATERIAL_SPECULAR, Vector4(material->specular));
		SetMaterialTexture(GVS_TEXTURE_SPECULAR, "specular", material);
		SetMaterialValue(GVS_MATERIAL_AMBIENT, Vector4(material->ambient));
		SetMaterialTexture(GVS_TEXTURE_AMBIENT, "ambient", material);

		SetMaterialTexture(GVS_TEXTURE_HEIGHT, "height", material);
		SetMaterialTexture(GVS_TEXTURE_NORMAL, "normal", material);
	}

	void GlobalVariablePool::SetLight(Light * light,unsigned int index)
	{
		if(light->getLightType() == Light::LT_DIRECTIONAL)
		{
			this->SetVal(_global_makeFromkeyIndex(GVS_DIRECTIONAL_LIGHT_DIFFUSE,index),
				sizeof(Vector4),light->Diffuse.ptr());
			this->SetVal(_global_makeFromkeyIndex(GVS_DIRECTIONAL_LIGHT_DIRECTION,index),
				sizeof(Vector3),(light)->getDirection().ptr());
			this->SetVal(_global_makeFromkeyIndex(GVS_DIRECTIONAL_LIGHT_SPECULAR,index),
				sizeof(Vector4),(light)->Specular.ptr());
		}
		else if(light->getLightType() == Light::LT_POINT)
		{
			this->SetVal(_global_makeFromkeyIndex(GVS_POINT_LIGHT_CONST_ATT,index),
				sizeof(float),&(light)->ConstantAttenuation);
			this->SetVal(_global_makeFromkeyIndex(GVS_POINT_LIGHT_DIFFUSE,index),
				sizeof(Vector4),(light)->Diffuse.ptr());
			float dist = (light)->getMaxDistance();
			this->SetVal(_global_makeFromkeyIndex(GVS_POINT_LIGHT_DISTANCE,index),
				sizeof(float),&dist);
			this->SetVal(_global_makeFromkeyIndex(GVS_POINT_LIGHT_LINEAR_ATT,index),
				sizeof(float),&(light)->LinearAttenuation);
			this->SetVal(_global_makeFromkeyIndex(GVS_POINT_LIGHT_POSITION,index),
				sizeof(Vector3),(light)->getPosition().ptr());
			this->SetVal(_global_makeFromkeyIndex(GVS_POINT_LIGHT_QUAD_ATT,index),
				sizeof(Vector3),&(light)->QuadraticAttenuation);
			this->SetVal(_global_makeFromkeyIndex(GVS_POINT_LIGHT_SPECULAR,index),
				sizeof(Vector3),(light)->Specular.ptr());
		}
		else if((light)->getLightType() == Light::LT_SPOT)
		{
			this->SetVal(GVS_SPOT_LIGHT_POSITION|index,sizeof(Vector3),
				(light)->getPosition().ptr());
			this->SetVal(GVS_SPOT_LIGHT_DIRECTION|index,sizeof(Vector3),
				(light)->getDirection().ptr());
			this->SetVal(GVS_SPOT_LIGHT_DIFFUSE|index,sizeof(Vector4),
				(light)->Diffuse.ptr());
			this->SetVal(GVS_SPOT_LIGHT_SPECULAR|index,sizeof(Vector3),
				const_cast<float*>((light)->Specular.ptr()));
			this->SetVal(GVS_SPOT_LIGHT_CONST_ATT|index,sizeof(float),
				&((light)->ConstantAttenuation));
			this->SetVal(GVS_SPOT_LIGHT_LINEAR_ATT|index,sizeof(float),
				&((light)->LinearAttenuation));
			this->SetVal(GVS_SPOT_LIGHT_QUAD_ATT|index,sizeof(float),
				&((light)->QuadraticAttenuation));
			this->SetVal(GVS_SPOT_LIGHT_EXP|index,sizeof(float),
				&((light)->Exponent));
			this->SetVal(GVS_SPOT_LIGHT_CUTOFF|index,sizeof(float),
				&((light)->Cutoff));
			float dist = (light)->getMaxDistance();
			this->SetVal(GVS_SPOT_LIGHT_DISTANCE|index,sizeof(float),
				&dist);
		}
	}

	void GlobalVariablePool::setEntity(Entity * entity)
	{
	/*	GlobalVariableSemantic sem = GVS_FRAME_ANIMATION;
		std::map<GlobalVariableSemantic,_global_variable_descrip *>::iterator it = m_SemanticValueMap.find(sem);
		unsigned int  val = entity->hasAnimation();
		if(it != m_SemanticValueMap.end())
		{
			
			it->second->SetVal(sem,sizeof(unsigned int),&val);
		}
		else
		{
			_global_variable_descrip * temp = new _global_variable_descrip(sizeof(unsigned int));
			temp->SetVal(sem,sizeof(unsigned int),&(val));
			m_SemanticValueMap.insert(std::make_pair(sem,temp));
		}

		
		switch(val)
		{
		case 1:{
			float weight = entity->getAnimationweight();
			sem =GVS_INTER_FACTOR;

			it = m_SemanticValueMap.find(sem);

			if(it != m_SemanticValueMap.end())
			{
				it->second->SetVal(sem,sizeof(float),&weight);
			}
			else
			{
				_global_variable_descrip * temp = new _global_variable_descrip(sizeof(float));
				temp->SetVal(sem,sizeof(float),&(weight));
				m_SemanticValueMap.insert(std::make_pair(sem,temp));			
			}
			   }break;
		case 2:
			{
				sem = GVS_BONE_MATRICES;
				it = m_SemanticValueMap.find(sem);
				Md5Mesh * md5mesh = dynamic_cast<Md5Mesh *>(entity->getMesh().InterPtr());
				const std::vector<Matrix4> & meshmatrix = md5mesh->getBoneMatrix();

				std::vector<Matrix4> boneMatrices(meshmatrix.size());
				for(unsigned int i = 0; i < boneMatrices.size();i++)
				{
					boneMatrices[i] = meshmatrix[i].transpose();
				}
				if(it != m_SemanticValueMap.end())
				{
					it->second->SetVal(sem,sizeof(Matrix4),&(boneMatrices[0][0][0]),boneMatrices.size());
				}
				else
				{
					_global_variable_descrip * temp = new _global_variable_descrip(sizeof(Matrix4));
					temp->SetVal(sem,sizeof(Matrix4),&(boneMatrices[0][0][0]),boneMatrices.size());
					m_SemanticValueMap.insert(std::make_pair(sem,temp));
				}
			}break;
		}
*/
		 

	}

	void GlobalVariablePool::SetCamera(Camera * camera)
	{
		SetViewMatrix(camera->getViewMatrix());
		SetProjectMatrix(camera->getProjectionMatrixDXRH());
		GlobalVariableSemantic sem = GVS_CAMERA_POSITION;
		std::map<GlobalVariableSemantic,_global_variable_descrip *>::iterator it = m_SemanticValueMap.find(sem);
		if(it != m_SemanticValueMap.end())
		{
			it->second->SetVal(sem,sizeof(Vector3),camera->getPosition().ptr());
		}else
		{
			assert(false &&" GP Set Camera Pos failed");
			printf("GP Set Camera Pos failed\n");
		}
	}

	void GlobalVariablePool::setMatrices(vector<Matrix4>& mats)
	{
		auto sem = GVS_BONE_MATRICES;
		auto it = m_SemanticValueMap.find(sem);

		std::vector<Matrix4> boneMatrices(mats.size());
		for (unsigned int i = 0; i < boneMatrices.size(); i++)
		{
			boneMatrices[i] = mats[i].transpose();
		}
		if (it != m_SemanticValueMap.end())
		{
			it->second->SetVal(sem, sizeof(Matrix4), &(boneMatrices[0][0][0]), boneMatrices.size());
		}
		else
		{
			_global_variable_descrip * temp = new _global_variable_descrip(sizeof(Matrix4));
			temp->SetVal(sem, sizeof(Matrix4), &(boneMatrices[0][0][0]), boneMatrices.size());
			m_SemanticValueMap.insert(std::make_pair(sem, temp));
		}
	}


}