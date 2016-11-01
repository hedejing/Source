#pragma once
#include "PBRPipeline.h"
#include "AppFrameWork2.h"

class App_PBR :public App {

public:

	void Init() {
		render = new EGLRenderSystem;
		render->SetWandH(w, h);
		render->Initialize();
		InitDevice();
		InitPipeline();
		CreateScene();
	}
	void InitPipeline()
	{
		pipeline = new PBRPipeline2;
		pipeline->Init();
	}
	void Render() {
		UpdateGUI();
		pipeline->Render();
	}

	void CreateScene() {
		scene = new OctreeSceneManager("scene1", render);
		SceneContainer::getInstance().add(scene);
		SceneNode* pRoot = scene->GetSceneRoot();
		camera = scene->CreateCamera("main");
		camera->lookAt(Vector3(0, 0, 2.5), Vector3(0, 0, 0), Vector3(0, 1, 0));
		float fovy = 45;
		camera->setPerspective(fovy, float(w) / float(h), 0.01, 100);


		loadModels();
	}
	void loadModels() {
		SceneNode* root = scene->GetSceneRoot();
		SceneNode* s;

		for (int i = 0; i < 8; i++) {
			s = LoadMeshtoSceneNode("model/pbr_test/sphere"+to_string(i)+ ".obj", "sphere" + to_string(i));
			s->scale(0.15, 0.15, 0.15);
			s->translate(i*0.33 - 1.1, -0.2, 0);
			root->attachNode(s);
		}
		for (int i = 0; i < 8; i++) {

			s = LoadMeshtoSceneNode("model/pbr_test/sphere" + to_string(i) + ".obj", "sphere" + to_string(i)+"d");
			s->scale(0.15, 0.15, 0.15);
			s->translate(i*0.33 - 1.1, 0.2, 0);
			root->attachNode(s);
		}

	}
};