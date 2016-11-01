#pragma once
#include "TemporalAA.h"
#include "AppFrameWork2.h"
#include "Interpolation.h"
class App_TAA :public App {

public:
	SceneNode* node, *mid;
	SceneNode* cube;
	Interpolation interp;
	void Init() {
		w = 1280;
		h = 720;
		render = new EGLRenderSystem;
		render->SetWandH(w, h);
		render->Initialize();
		InitDevice();
		InitPipeline();
		CreateScene();


	}
	void InitPipeline()
	{
		pipeline = new TemporalAAPipeline;
		pipeline->Init();
	}
	void Render() {
		static float t = 0;
		t += 0.02;
		Vector3 pos = interp.GetInterpolation(t);
		Vector3 dir = interp.GetDerivative(t);
		dir.normalize();
		//camera->lookAt(pos, pos + dir, Vector3(0, 1, 0));
		UpdateGUI();
		pipeline->Render();
	}
	void UpdateGUI() {
		GUI::NewFrame();
		ImGui::Begin("Params");
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		auto p = static_cast<TemporalAAPipeline*>(pipeline);
		ImGui::DragFloat3("LightColor", &p->LightColor[0], 0.05);
		ImGui::DragFloat3("LightPosition", &p->LightPosition[0], 0.05);
		static Vector3 pos, dir;
		pos = camera->getPosition();
		dir = camera->getDirection();
		ImGui::DragFloat3("CameraPos", &pos[0], 0.05);
		static bool EnableTemporalAA=true;
		ImGui::Checkbox("EnableTemporalAA", &EnableTemporalAA); p->EnableTemporalAA = EnableTemporalAA ? 1 : 0;
		ImGui::End();
	}
	void CreateScene() {
		scene = new OctreeSceneManager("scene1", render);
		SceneContainer::getInstance().add(scene);
		SceneNode* pRoot = scene->GetSceneRoot();
		camera = scene->CreateCamera("main");
		camera->lookAt(Vector3(-9, 4, -1), Vector3(0, 0, 0), Vector3(0, 1, 0));
		float fovy = 45;
		camera->setPerspective(fovy, float(w) / float(h), 0.01, 100);


		loadModels();
	}
	void loadModels() {
		SceneNode* root = scene->GetSceneRoot();
		SceneNode* s;

		//s = LoadMeshtoSceneNode("model/pbr_test/sphere0.obj", "sphere0");
		//mid = scene->CreateSceneNode("mid");
		//mid->attachNode(s);
		//s->translate(-3, -0.2, 0);
		//node = s;
		//root->attachNode(mid);

		//s = LoadMeshtoSceneNode("model/test/cube.obj", "cube");
		//cube = s;
		//root->attachNode(s);

		//s = LoadMeshtoSceneNode("model/bus/bus_station.obj", "bus");
		////s->setScale(Vector3(0.1));
		////s->setTranslation(y[0]);
		//root->attachNode(s);

		s = LoadMeshtoSceneNode("model/16.10/sponza/sponza.obj", "sponza");
		//s->setScale(Vector3(0.1));
		//s->setTranslation(y[0]);
		root->attachNode(s);

		//interpolation test
		vector<double> x = { 0.1,2,4,6,8 };
		vector<Vector3> y = { Vector3(-9,4,-1),Vector3(-4,2,1),Vector3(0,3.5,2),Vector3(5.5,5,-1),Vector3(8,7.5,-2) };
		interp.SetData(x, y);

		s = LoadMeshtoSceneNode("model/test/cube.obj", "s1");
		s->setScale(Vector3(0.01));
		s->setTranslation(y[0]);
		root->attachNode(s);

		s = LoadMeshtoSceneNode("model/test/cube.obj", "s2");
		s->setScale(Vector3(0.01));
		s->setTranslation(y[1]);
		root->attachNode(s);

		s = LoadMeshtoSceneNode("model/test/cube.obj", "s3");
		s->setScale(Vector3(0.01));
		s->setTranslation(y[2]);
		root->attachNode(s);

		s = LoadMeshtoSceneNode("model/test/cube.obj", "s4");
		s->setScale(Vector3(0.01));
		s->setTranslation(y[3]);
		root->attachNode(s);

		s = LoadMeshtoSceneNode("model/test/cube.obj", "s5");
		s->setScale(Vector3(0.01));
		s->setTranslation(y[4]);
		root->attachNode(s);
		//for (int i = 0; i < 8; i++) {

		//	s = LoadMeshtoSceneNode("model/pbr_test/sphere" + to_string(i) + ".obj", "sphere" + to_string(i) + "d");
		//	s->scale(0.15, 0.15, 0.15);
		//	s->translate(i*0.33 - 1.1, 0.2, 0);
		//	root->attachNode(s);
		//}

	}
};
