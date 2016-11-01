#pragma once
#include "Camera.h"
#include "mathlib.h"
#include "SceneNode.h"
#include "Entity.h"
#include "SceneManager.h"
#include "MeshManager.h"
#include "Camera.h"
#include "OctreeSceneManager.h"
#include "EGLRenderSystem.h"
#include "GlobalVariablePool.h"
#include "RenderTarget.h"
#include "EGLRenderWindow.h"
#include "TextureManager.h"
#include "EngineUtil.h"
#include "SysEntry.h"
#ifndef ANDROID
#include "GUIWrapper.h"
#endif

class App {
public:
	int w = 1024;
	int h = 756;

	Pipeline* pipeline;
	GlobalVariablePool * gp;
	RenderSystem * render;
	SceneManager * scene;
	MeshManager * meshMgr;
	RenderTarget * renderwindow;
	Camera* camera;
	SysEntry * sysentry;
	double AbsolutTime=0;
	virtual void Init() = 0;
	virtual void Render() = 0;
	virtual void UpdateGUI() {
#ifndef ANDROID
		GUI::NewFrame();
#endif
	}
	 
	SceneNode* LoadMeshtoSceneNode(string pfile, string name) {
		MeshPtr mesh = meshMgr->loadMesh_assimp_check(name, pfile);
		if (mesh.IsNull()) return NULL;
		Entity* entity = scene->getEntity(name);
		if(entity ==NULL)
			entity = scene->CreateEntity(name);
		entity->setMesh(mesh);
		SceneNode* snode = scene->getSceneNode(name);
		if (snode == NULL) {
			snode = scene->CreateSceneNode(name);
			snode->attachMovable(entity);
		}
		return snode;
	}

	void InitDevice()
	{
		Logger::WriteLog("Into  Initialize");
		gp = GlobalVariablePool::GetSingletonPtr();
		gp->initialize();
		Logger::WriteLog("gp  Initialize");

		meshMgr = MeshManager::getSingletonPtr();
		//meshMgr->initialize(new EGLTextureFactory);

		Logger::WriteLog("wh  Initialize");

		sysentry = SysEntry::getSingletonPtr();
		sysentry->setRenderSystem(render);
		sysentry->setMainWindow(renderwindow);

		InitDefaultResources();
	}

	void InitDefaultResources() {

#ifdef ANDROID
		FileUtil::addSceneFilePath("sdcard/OpenGLES/shader/");
		FileUtil::addSceneFilePath("sdcard/OpenGLES/default_asserts/");
		FileUtil::addSceneFilePath("sdcard/OpenGLES/");
#else
#ifdef WIN32
		FileUtil::addSceneFilePath("shader/");
		FileUtil::addSceneFilePath("default_asserts/");
#else
		FileUtil::addSceneFilePath("../");
		FileUtil::addSceneFilePath("../shader/");
		FileUtil::addSceneFilePath("../default_asserts/");
#endif 
#endif 

		// init texture settings;
		auto& tm = TextureManager::getInstance();
		tm.LoadTexturePreset("texture_preset.json");

		// init rendertarget settings;
		RenderTargetManager::getInstance().LoadRenderTargetPreset("RenderTarget_preset.json");
		renderwindow = new EGLRenderWindow("MainWindow", render);
		Logger::WriteLog("Window  Initialize");
		auto& rtm = RenderTargetManager::getInstance();
		rtm.add(renderwindow);

		//get ScreenQuad
		meshMgr->loadMesh_assimp_check("screenquad", "default_asserts/ScreenQuad.obj");
		auto& grm = GlobalResourceManager::getInstance();

	}
};