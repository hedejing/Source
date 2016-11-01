#pragma once
#include "AppFrameWork2.h"
#include "VRPipeline.h"
#include "socketSceneLoader\SceneLoader.h"
#include <mutex>
#include <map>
#include <rapidjson\rapidjson.h>
using namespace Block;
using namespace rapidjson;
using std::mutex;
using std::map;
class App_VR :public App {

public:

	void Init() {
		render = new EGLRenderSystem;
		render->SetWandH(w, h);
		render->Initialize();
		InitDevice();
		InitPipeline();
		CreateScene();

		//sceneLoader
		sl = new SceneLoader(scene, meshMgr, 32, 16);
		sl->loadMesh();
		sl->InitServer("10.76.1.77", "56025");
	}

	void InitPipeline()
	{
		pipeline = new VRPipeline;
		pipeline->Init();
	}

	void Render() {
		UpdateGUI();
		//sceneLoader
		sl->UpdateScene();
		
		pipeline->Render();
	}

	void CreateScene() {
		scene = new OctreeSceneManager("scene1", render);
		SceneContainer::getInstance().add(scene);
		SceneNode* pRoot = scene->GetSceneRoot();
		camera = scene->CreateCamera("main");
		camera->lookAt(Vector3(0, 2.5, 2.5), Vector3(0, 0, 0), Vector3(0, 1, 0));
		float fovy = 45;
		camera->setPerspective(fovy, float(w) / float(h), 0.01, 100);
	}


	~App_VR()
	{
		delete sl;
	}

private :
	SceneLoader* sl;
};