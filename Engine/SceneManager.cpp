#include "SceneManager.h"
#include "SceneNode.h"
#include "Entity.h"
#include "Camera.h"
#include "RaySceneQuery.h"
//#include "Effect.h"
#include "MeshManager.h"



unsigned int intersect = 0;
const int maximum_light_num = 80;
namespace HW
{



	SceneManager::SceneManager(const string& name, RenderSystem * render)
		: m_RenderSystem(render),
		name(name),
		m_EnvAmbient(Vector4::ZERO),
		m_BackgroundColor(Vector4::ZERO),
		mCastShadow(true),
		m_skyNodeEnable(false),
		m_skyNode(NULL),
		m_SceneRoot(NULL),
		m_useGlobalEffect(false),
		m_GlobalEffect(NULL),
		m_ScreenAlignedQuad(NULL),
		//m_animation(NULL),
		m_depthEffect(NULL),
		m_sky(NULL)
	{

	}

	RaySceneQuery * SceneManager::createRaySceneQuery()
	{
		return new RaySceneQuery(this);
	}

	SceneNode* SceneManager::CreateSceneNode(const string &name)
	{
		auto SceneNodeit = m_sceneNodeRefList.find(name);
		if (SceneNodeit == m_sceneNodeRefList.end())
			return CreateSceneNodeImp(name);
		else
			return NULL;


		//// in some case the sceneNode export from 3dsmax has the samename; 
		//// so we have to suppport them by rename it
		//SceneNode* node = NULL;
		//if (m_sceneNodeRefList.find(name) != m_sceneNodeRefList.end())
		//{
		//	node = CreateSceneNodeImp("");

		//}
		//else
		//	node = CreateSceneNodeImp(name);


		//// handle a reference by manager


		////	assert((m_sceneNodeRefList.find(name) == m_sceneNodeRefList.end())
		////		&& "scene node already exists.");

		//std::pair<std::map<string, SceneNode *>::iterator, bool > ret;
		//ret = m_sceneNodeRefList.insert(std::make_pair(name, node));
		//if (ret.second)
		//{
		//	//	printf("Node %s Insert Success",name.c_str());
		//}
		//else
		//	printf("Node %s Insert Failed", name.c_str());
		//return node;
	}

	SceneNode * SceneManager::CreateSceneNodeImp(const string & name)
	{
		//	printf("Create node %s",name.c_str());
		auto node = new SceneNode(name, this);
		m_sceneNodeRefList[name] = node;
		return node;
	}

	Light* SceneManager::CreateLight(const string &name, Light::LightType type)
	{
		Light *light = new Light(name, type, this);
		// handle a reference by manager
		if (m_lightRefList.find(light->getName()) != m_lightRefList.end())
		{

			printf("light %s already exists", name.c_str());
			assert(false && "light already exists.");
		}
		m_lightRefList.insert(std::make_pair(light->getName(), light));
		m_shadowRefList[light->getName()] = NULL;
		return light;

	}

	Entity* SceneManager::CreateEntity(const string &name)
	{
		Entity* ent = new Entity(name, this);
		if (m_entityRefList.find(name) != m_entityRefList.end())
		{

			printf("entity %s already exists", name.c_str());
			assert(false && "entity already exists.");
		}
		m_entityRefList.insert(std::make_pair(name, ent));
		ent->setVisible(true); // make this entity visible
		return ent;

	}

	Entity *SceneManager::CreateDynamicWeather(const string &name)
	{
		//ParticleSystem* ent = new DynamicWeather(name, this);
		//if (m_entityRefList.find("ParticleSystem") != m_entityRefList.end())
		//{

		//	printf("entity %s already exists", name.c_str());
		//	assert(false && "entity already exists.");
		//}
		//m_entityRefList.insert(std::make_pair(name, ent));
		//ent->setVisible(true); // make this entity visible
		//return ent;

		return NULL;
	}

	Camera* SceneManager::CreateCamera(const string &name)
	{
		Camera* camera = new Camera(name, this);
		assert(m_cameraRefList.find(name) == m_cameraRefList.end() && "camera already exists.");
		m_cameraRefList[name]= camera;

		return camera;
	}

	// may have problem when the scenenode is a octreenode
	void SceneManager::destroy(SceneNode* node)
	{

		assert(node);
		std::map<string, SceneNode*>::iterator it = m_sceneNodeRefList.find(node->getName());
		if (it != m_sceneNodeRefList.end())
		{
			delete it->second;
			m_sceneNodeRefList.erase(it);
		}

	}

	void SceneManager::destroy(Entity* entity)
	{
		assert(entity);
		std::map<string, Entity*>::iterator it = m_entityRefList.find(entity->getName());
		if (it != m_entityRefList.end())
		{
			delete it->second;
			m_entityRefList.erase(it);
		}

	}

	void SceneManager::destroy(Light* light)
	{
		assert(light);
		std::map<string, Light*>::iterator it = m_lightRefList.find(light->getName());
		if (it != m_lightRefList.end())
		{
			delete it->second;
			m_lightRefList.erase(it);
		}
	}

	void SceneManager::destroy(Camera* camera)
	{
		assert(camera);
		std::map<string, Camera*>::iterator it = m_cameraRefList.find(camera->getName());
		if (it != m_cameraRefList.end())
		{
			delete it->second;
			m_cameraRefList.erase(it);
		}
	}

	Entity* SceneManager::getEntity(const string& name)
	{

		std::map<string, Entity*>::iterator it = m_entityRefList.find(name);
		if (it != m_entityRefList.end())
		{
			return it->second;
		}
		else
		{
			return NULL;
		}
	}

	Camera* SceneManager::getCamera(const string& name)
	{
		// try to convenient to get the default camera 
		if (name.empty())
		{
			if (m_cameraRefList.size() != 0)
				return m_cameraRefList.begin()->second;
		}

		std::map<string, Camera*>::iterator it = m_cameraRefList.find(name);
		if (it != m_cameraRefList.end())
		{
			return it->second;
		}
		else
		{
			return NULL;
		}
	}

	Light * SceneManager::getLight(const string &name)
	{
		if (name.empty())
		{
			if (m_lightRefList.size() != 0)
				return m_lightRefList.begin()->second;
		}
		std::map<string, Light*>::iterator it = m_lightRefList.find(name);
		if (it != m_lightRefList.end())
		{
			return it->second;
		}
		else
		{
			return NULL;
		}
	}

	SceneNode* SceneManager::getSceneNode(const string& name)
	{

		std::map<string, SceneNode*>::iterator it = m_sceneNodeRefList.find(name);
		if (it != m_sceneNodeRefList.end())
		{
			return it->second;
		}
		else
		{
			return NULL;
		}
	}

	void SceneManager::addChangedNode(SceneNode * node)
	{
		if (m_SceneRoot != node->getRoot())
			return;
		if (!influence_camera.empty())
		{
			influence_camera.clear();
			change_entity.clear();
		}

		const std::list<Entity *>& entity_list = node->getEntities();
		for (std::list<Entity *>::const_iterator move_it = entity_list.begin(); move_it != entity_list.end(); move_it++)
		{
			//std::map<String,Entity *>::iterator it = change_entity.find((*move_it)->getName());
			//if(change_entity.end() != it)
			//	return;

			// use the operator of [] to add or replace the entity;
			Entity * entity = dynamic_cast<Entity *>(*move_it);
			change_entity[entity->getName()] = entity;
		}

		if (node->hasChildNodes())
		{
			mapIterator<string, SceneNode *> children_begin = node->getChildBegin();
			mapIterator<string, SceneNode *> children_end = node->getChildEnd();
			for (; children_begin != children_end; children_begin++)
			{
				this->addChangedNode(children_begin.get());
			}
		}

	}

	void SceneManager::addInfluenceCamera(Camera * cam)
	{
		influence_camera[cam->getName()] = cam;
	}

	bool SceneManager::doneInfluence(Camera * came)
	{
		if (came)
		{
			return influence_camera.find(came->getName()) != influence_camera.end();
		}
		else
			return false;
	}

	Sphere SceneManager::getSceneBoundingSphere()
	{
		Sphere s;
		s.setRadius(0);
		bool first = true;
		std::map<string, SceneNode*>::const_iterator it_scenenode = m_sceneNodeRefList.begin();
		for (; it_scenenode != m_sceneNodeRefList.end(); it_scenenode++)
		{
			if (m_SceneRoot == it_scenenode->second->getRoot())
			{
				if (first)
				{
					s = it_scenenode->second->getBoundingSphere();
					first = false;
				}
				else
				{
					SceneNode * sn = it_scenenode->second;
					if (sn->getEntities().empty() == false || sn->hasChildNodes())
						s.merge(it_scenenode->second->getBoundingSphere());
				}

			}

		}
		return s;
	}



	BoundingBox SceneManager::getSceneBoundingBox()
	{
		BoundingBox s;
		bool first = true;
		std::map<string, SceneNode*>::const_iterator it_scenenode = m_sceneNodeRefList.begin();
		for (; it_scenenode != m_sceneNodeRefList.end(); it_scenenode++)
		{
			if (m_SceneRoot == it_scenenode->second->getRoot())
			{
				if (first)
				{
					s = it_scenenode->second->getBoundingBox();
					first = false;
				}
				else
				{
					SceneNode * sn = it_scenenode->second;
					if (sn->getEntities().empty() == false || sn->hasChildNodes())
						s.merge(it_scenenode->second->getBoundingBox());
				}

			}

		}
		return s;
	}

	void SceneManager::setGlobalEffect(Effect * effect)
	{
		m_GlobalEffect = effect;
	}

	Effect * SceneManager::getGlobalEffect()
	{
		return m_GlobalEffect;
	}

	void SceneManager::setDepthEffect(Effect * effect)
	{
		m_depthEffect = effect;
	}

	Effect * SceneManager::getDepthEffect()
	{
		return m_depthEffect;
	}

	SceneManager::~SceneManager()
	{
		printf("sceneManager release");


		for (std::map<string, SceneNode*>::iterator it = m_sceneNodeRefList.begin(); it != m_sceneNodeRefList.end(); it++)
		{
			delete it->second;
		}
		m_sceneNodeRefList.clear();
		for (std::map<string, Light*>::iterator it = m_lightRefList.begin(); it != m_lightRefList.end(); it++)
		{
			delete it->second;
		}

		std::map<string, RenderTarget *>::iterator itr_rt = m_shadowRefList.begin();
		for (; itr_rt != m_shadowRefList.end(); itr_rt++)
		{
			if (itr_rt->second)
				delete itr_rt->second;;
		}
		m_lightRefList.clear();
		for (std::map<string, Camera*>::iterator it = m_cameraRefList.begin(); it != m_cameraRefList.end(); it++)
		{
			delete it->second;
		}
		m_cameraRefList.clear();

		for (std::map<string, Entity*>::iterator itr_entity = m_entityRefList.begin(); itr_entity != m_entityRefList.end(); itr_entity++)
		{
			delete itr_entity->second;
		}
		m_entityRefList.clear();
		//if (m_animation)
		//	delete m_animation;
		if (m_skyNode)
		{
			delete m_skyNode;
		}
		if (m_SceneRoot)
			delete m_SceneRoot;




	}

	SceneNode * SceneManager::createSkyNode(string mname /* = "skysphere" */)
	{
		m_skyNode = new SceneNode(mname, this);
		return m_skyNode;

	}

	Entity * SceneManager::getSkyEntity()
	{
		// skynode only has one entity;
		return dynamic_cast<Entity *>(*(m_skyNode->getEntities().begin()));
	}

	void SceneManager::findVisibleNode(Camera * cam, SceneNode * node)
	{
		/// we just ignore the null pointer
		/// and do not report an error.
		if (!node) return;
		/// test if this node has entity.
		if (node->hasEntities())
		{
			/// test bounding sphere
			//	if(cam->intersects(node->getBoundingSphere()))
			//	{
			/// continue test bounding box
			if (cam->intersects(node->getBoundingBox()))
			{

				const std::list<Entity*>& entities = node->getEntities();
				for (std::list<Entity*>::const_iterator it = entities.begin(); it != entities.end(); it++)
				{
					// test if this entity is visible
					//	cam->insertMoveableToRenderqueue(*it);
					cam->insertMoveableToRenderqueue(*it);

				}
			}

			intersect++;
			//	}
		}

		/// child node process
		if (node->hasChildNodes())
		{
			SceneNode::NodeIterator it = node->getChildBegin();
			SceneNode::NodeIterator end = node->getChildEnd();
			for (; it != end; it++)
			{
				findVisibleNode(cam, it.get());
			}
		}
	}

	void SceneManager::findVisibleNode(Camera *cam, SceneNode *node, RenderQueue &render_queue)
	{
		if (!node)
			return;
		if (node->hasEntities())
		{
			int cull = cam->intersects(node->getBoundingBox());
			if (cull == 0)
				return;
			const std::list<Entity*>& entities = node->getEntities();
			for (std::list<Entity*>::const_iterator it = entities.begin(); it != entities.end(); it++)
			{
				(*it)->updateRenderqueue(cam, render_queue);

			}
		}

		if (node->hasChildNodes())
		{
			SceneNode::NodeIterator it = node->getChildBegin();
			SceneNode::NodeIterator end = node->getChildEnd();
			for (; it != end; it++)
			{
				findVisibleNode(cam, it.get(), render_queue);
			}
		}
	}

	void SceneManager::getVisibleRenderQueue(Camera * cam)
	{
		//cam->v_render_queue.clear();
		if (true)
		{
			//	std::cout<<cam->getName()<<std::endl;
			findVisibleNode(cam, m_SceneRoot);

		}
		else
		{
			if (!change_entity.empty() && !this->doneInfluence(cam))
			{
				std::map<string, Entity*>::iterator entity_it = change_entity.begin();
				for (; entity_it != change_entity.end(); entity_it++)
				{
					cam->insertMoveableToRenderqueue(entity_it->second);
				}


			}
		}
		addInfluenceCamera(cam);
		cam->establishRenderQueue(true);
		//	std::cout<<cam->getName()<<" intersects "<<intersect<<" entity num = "<<m_entityRefList.size()<<std::endl;
		intersect = 0;
	}

	void SceneManager::getVisibleRenderQueue(Camera * cam, RenderQueue & render_queue)
	{
		findVisibleNode(cam, m_SceneRoot, render_queue);
	}



	void SceneManager::LoadScene(const string & scenefile_name, SceneNode* pRoot, bool castShadow)
	{
		//TseParser sceneParser(this, scenefile_name, pRoot, castShadow);
	}

	Entity * SceneManager::getScreeAlignedQuad()
	{
		if (!m_ScreenAlignedQuad)
			m_ScreenAlignedQuad = this->CreateEntity("ScreenAlignedQuad");
		return m_ScreenAlignedQuad;
	}

	void SceneManager::printStatistic()
	{
		printf("Node size = %d ,Entity size = %d", m_sceneNodeRefList.size(), m_entityRefList.size());
		for (std::map<string, SceneNode*>::iterator itr_sceneNode = m_sceneNodeRefList.begin(); itr_sceneNode != m_sceneNodeRefList.end(); itr_sceneNode++)
		{
			printf("sceneNode = %s", itr_sceneNode->second->getName().c_str());
		}
	}

	void SceneManager::releaseInternalRes()
	{
		std::map<string, RenderTarget *>::iterator itr_rt = m_shadowRefList.begin();
		for (; itr_rt != m_shadowRefList.end(); itr_rt++)
		{
			if (itr_rt->second)
				itr_rt->second->releaseInternalRes();
		}

		itr_rt = m_rendertargetList.begin();
		for (; itr_rt != m_rendertargetList.end(); itr_rt++)
		{
			if (itr_rt->second)
				itr_rt->second->releaseInternalRes();
		}
	}

	Sky * SceneManager::getSky()
	{
		return m_sky;

	}

	Sky* SceneManager::CreateSky(SkyTech tech, float radius_scale)
	{
		//switch (tech)
		//{
		//case ST_SkyScatter:
		//	m_sky = new SkyScattering(this, radius_scale);
		//	break;
		//case ST_SkyBox:
		//	m_sky = new SkyBox(this);
		//	break;
		//default:
		//	break;
		//}
		return m_sky;

	}


	HW::Sphere SceneManager::getSceneCastShadowSphere()
	{

		Sphere s;
		s.setRadius(0);
		bool first = true;
		std::map<string, SceneNode*>::const_iterator it_scenenode = m_sceneNodeRefList.begin();
		for (; it_scenenode != m_sceneNodeRefList.end(); it_scenenode++)
		{
			if (m_SceneRoot == it_scenenode->second->getRoot())
			{
				SceneNode * sn = it_scenenode->second;

				//if (!sn->isCastShadow()) continue; //
				const std::list<Entity*> & entities = sn->getEntities();
				for (std::list<Entity*>::const_iterator itr_ent = entities.begin(); itr_ent != entities.end(); itr_ent++)
				{
					if ((*itr_ent)->visible() && (*itr_ent)->isCastShadow())
					{
						if (first)
						{
							s = (*itr_ent)->getBoundingSphere();
							first = false;
						}
						else
							s.merge((*itr_ent)->getBoundingSphere());
					}
				}


			}

		}
		return s;
	}

	void SceneManager::setActiveLightVector(Light * light, int index)
	{
		if (light)
		{
			if (index < m_active_light_list.size())
				m_active_light_list[index] = light;
			else if (index < maximum_light_num)
				m_active_light_list.push_back(light);

		}
		else
		{
			if (index < m_active_light_list.size())
				m_active_light_list.erase(m_active_light_list.begin() + index);
		}
	}

	const vector<Light *> & SceneManager::getActiveLightVetcor() const
	{
		return m_active_light_list;
	}

	const vector<Light *> & SceneManager::getShadowLightVetcor() const
	{

		////m_has_shadow_light_list.clear();
		//for (auto& l : m_active_light_list){
		//	if (l->isCastShadow()){
		//		m_has_shadow_light_list.push_back(l);
		//	}
		//}
		return m_has_shadow_light_list;
	}

	void SceneManager::getVisibleRenderQueue_as(Camera * cam, RenderQueue & renderQueue)
	{
		//暂时先获取全部mesh for test
		for (auto&x : m_SceneRoot->getAllChildNodes()) {
			auto entitys = x->getEntities();
			for (auto& entity : entitys) {
				for (auto& y : entity->getMesh()->m_SubMeshList_as) {
					auto mesh = y.second;

					RenderQueueItem item;
					item.entity = entity;
					item.asMesh = mesh;
					renderQueue.push_back(item);
				}
			}
		}
		//for (auto& x : m_entityRefList){
		//	auto entity = x.second;
		//	auto parent = entity->getParent();
		//	if (parent == NULL) continue;
		//	for (auto& y : entity->getMesh()->m_SubMeshList_as){
		//		auto mesh = y.second;

		//		RenderQueueItem item;
		//		item.entity = entity;
		//		item.asMesh = mesh;
		//		renderQueue.push_back(item);
		//	}
		//	
		//}
	}

}

SceneContainer::SceneContainer()
{

}
