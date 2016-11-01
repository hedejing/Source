#include "MeshManager.h"

#include "ElemIterator.h"
#include "OMesh.h"
#include "SubMesh.h"
#include "Texture.h"
#include "Engine/engine_struct.h"
#include "Engine/assimp_loader.h"
#include "Engine/ResourceManager.h"
#include "TextureManager.h"
#include <cassert>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#ifdef ANDROID
#include <unistd.h>
#include <fcntl.h>
#else
#include <io.h>
#endif
#include "EngineUtil.h"
#ifdef _WIN32
#include <sys/stat.h>
#endif
namespace HW
{
	MeshManager* MeshManager::m_meshMgr = NULL;

	//---------------------------------------------------------------
	MeshManager::MeshManager(){
	}

	MeshManager* MeshManager::getSingletonPtr()
	{
		if(!m_meshMgr)
			m_meshMgr = new MeshManager();
		return m_meshMgr;
	}

	MeshManager& MeshManager::getSingleton()
	{
		return *getSingletonPtr();
	}

	MeshPtr  MeshManager::loadMesh(const string& name,const string& filename,bool pathcontained /* = true */)
	{
		return MeshPtr(NULL);
	}

		HW::MeshPtr MeshManager::loadMesh_assimp(const string& name, const string& filepath)
		{
			vector<as_Geometry*> geos;
			vector<as_Skeleton*> skes;
			vector<as_SkeletonAnimation*> anis;
			vector<as_Material*> mats;
			vector<as_Mesh*> meshs;
			Vector3 sMax, sMin;
			auto& tm = TextureManager::getInstance();
			Import3DFromFile(filepath, meshs, geos, skes, anis, mats, sMax, sMin);
			
			string validpath = FileUtil::getSceneFileValidPath(filepath);
			
			string filedir = validpath.substr(0, validpath.find_last_of('/') + 1);
			FileUtil::addSceneFilePath(filedir);

			OMesh *mesh = new SubMesh(name);
			auto& g = GlobalResourceManager::getInstance();
			for (auto& x : geos) {
				g.as_geometryManager.add(x);
			}
			for (auto& x : skes) g.as_skeletonManager.add(x);
			for (auto& x : anis) g.as_skeletonAniManager.add(x);
			for (auto& x : mats) {
				g.as_materialManager.add(x);
				for (auto& t : x->textures){
					auto tex = t.second;
					g.as_textureManager.add(&tex);
					tm.CreateTextureFromImage(tex.path);
				}
				
			}
			for (auto& x : meshs){
				mesh->addSubMesh_as(x);
				g.as_meshManager.add(x);
				
			}

			mesh->m_modelBoundingBox.fromPoints(sMax, sMin);
			mesh->m_modelSphere.setCenter((sMax + sMin)*0.5f);
			mesh->m_modelSphere.setRadius((sMax - sMin).length()*0.5f);
			mesh->m_boundingBoxCurrent = true;
			mesh->m_boundingSphereCurrent = true;
			
			return MeshPtr(mesh);
		}

		HW::MeshPtr MeshManager::loadMesh_assimp_check(const string& name, const string& filename)
		{
			MeshPtr mesh = getMeshByName(filename);
			if (mesh.IsNull())
			{
				string fullpath = FileUtil::getSceneFileValidPath(filename);
				if (fullpath.empty()) return mesh;
				string purefilename = FileUtil::getPureFileName(fullpath);

				mesh = loadMesh_assimp(fullpath, fullpath);
				registerMesh(mesh);
			}
			return mesh;
		}

		MeshManager::~MeshManager()
		{

		}

		void MeshManager::releaseInternalRes()
		{
			std::map<string,MeshPtr>::iterator itr_mesh = m_MeshList.begin();
			for(;itr_mesh != m_MeshList.end();itr_mesh++)
			{
				// do invalid mesh's egl resource
				itr_mesh->second->releaseInternalRes();
			}
		}

		void MeshManager::initialize(TextureFactory * texturefactory)
		{
		}

		void MeshManager::registerMesh(MeshPtr mesh)
		{
			std::pair<std::map<string, MeshPtr>::iterator, bool> ret;
			ret = m_MeshList.insert(std::make_pair(mesh->getName(), mesh));
			if (ret.second)
			{
				//printf("Mesh %s insert success",mesh->getName().c_str());
			}
			else
				printf("Mesh %s Insert Failed", mesh->getName().c_str());
		}

		void MeshManager::unregisterMesh(const string& name)
		{
			std::map<string, MeshPtr>::iterator it = m_MeshList.find(name);
			if (it != m_MeshList.end())
				m_MeshList.erase(it);
		}

		void MeshManager::unregisterMesh(MeshPtr mesh)
		{
			assert(!mesh.IsNull() && "Null shared pointer"); // report a error
			unregisterMesh(mesh->getName());
		}

		void MeshManager::clearMeshes()
		{
			if (m_MeshList.empty())return;
			m_MeshList.clear();
		}

		HW::MeshPtr MeshManager::getMeshByName(const string &name)
		{
			std::map<string, MeshPtr>::iterator it = m_MeshList.find(name);
			if (it != m_MeshList.end())
				return it->second;
			return MeshPtr(0);
		}

}
