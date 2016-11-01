#pragma once
#include "SSSPipeline.h"
#include "AppFrameWork2.h"
#include <sstream>

using namespace std;
class App_SSS :public App{

public:
	bool shutdown = false;

	bool use_huawei_head = false;
	int head_index = 0;
	float modelscale = 0.003;
	Vector3 modelTranslate = Vector3(-1.6,0.5,-1.6);
	vector<SceneNode*> lightpoint= vector<SceneNode*>(5,nullptr);
	void Init() {
		w = 1280;
		h = 720;
		if (use_huawei_head) {
			if (head_index == 0) {
				w = 800;
				h = 920;
			}
			if (head_index == 1) {
				w = 540;
				h = 960;
			}
			if (head_index == 2) {
				w = 1000;
				h = 600;
			}
			if (head_index == 3) {
				w = 420;
				h = 360;
			}
			if (head_index == 4) {
				w = 280;
				h = 440;
			}
			if (head_index == 5 ) {
				w = 380;
				h = 520;
			}
		}


		render = new EGLRenderSystem;
		render->SetWandH(w, h);
		render->Initialize();
		InitDevice();
		InitPipeline();
		CreateScene();


	}
	void LoadModel(string path) {
		SceneManager* scene = SceneContainer::getInstance().get("scene1");
		SceneNode* root = scene->GetSceneRoot();
		SceneNode* s;

		s = LoadMeshtoSceneNode(path, path);
		if (s == NULL) {
			cout << "cant load model:" + path << endl; return;
		}
		s->setScale(Vector3(modelscale));
		s->setTranslation(modelTranslate);
		if (s != NULL)
			root->attachNode(s);
		auto b = root->getBoundingBox();
		auto v=b.getHalfSize();
		cout<<v.x;
	}
	void RemoveAllNodes() {
		SceneManager* scene = SceneContainer::getInstance().get("scene1");
		SceneNode* root = scene->GetSceneRoot();
		root->detachAllNodes();
	}
	void InitPipeline()
	{
		auto mpipeline = new SSSPipeline;
		mpipeline->use_huawei_head = use_huawei_head;
		mpipeline->head_index = head_index;
		mpipeline->Init();
		pipeline = mpipeline;
	}

	void Render() {
		UpdateGUI();
		pipeline->Render();
	}
	void SetCameraPers() {
		camera->setProjectionType(Camera::PT_PERSPECTIVE);
			camera->lookAt(Vector3(0, 0, 1.5), Vector3(0, 0, 0), Vector3(0, 1, 0));
			camera->setPerspective(45, float(w) / float(h), 0.01, 50);
	}
	void SetCameraOrth() {
		float width = w *modelscale, height = h *modelscale;
		float x = 1.2, y = -1.2;
		//x = 0; y = 0;
		camera->lookAt(Vector3(x, y, 1000 * modelscale), Vector3(x, y, 0), Vector3(0, 1, 0));
		camera->setProjectionType(Camera::PT_ORTHOGONAL);
		camera->setFrustum(-width / 2, width / 2, -height / 2, height / 2, 0.1, 3000*modelscale);
	}
	void UpdateGUI() {
#ifndef ANDROID

		GUI::NewFrame();
		{
			ImGui::Begin("Params");
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			auto p = (SSSPipeline*)pipeline;
			ImGui::DragFloat("bumpiness",&p->bumpiness,0.002, 0.0, 1.0);
			ImGui::DragFloat("specularIntensity", &p->specularIntensity, 0.002, 0.0, 10.0);
			ImGui::DragFloat("specularRoughness", &p->specularRoughness, 0.002, 0.0, 1.0);
			ImGui::DragFloat("specularFresnel", &p->specularFresnel, 0.002, 0.0, 1.0);
			ImGui::DragFloat("translucency", &p->translucency, 0.002, 0.0, 1.0);
			ImGui::DragFloat("sssBlurWidth", &p->sssWidth, 0.0001, 0.0, 0.1);
			
			//ImGui::DragFloat("environment", &p->environment, 0.002, 0.0, 1.0);
			static bool f=false;
			ImGui::Checkbox("Transmittance Enable", &f); p->sssEnabled = f ? 1 : 0;
			static char file[100]="model/RelightingData/0.obj";
			ImGui::InputText("Model Path", file, 100);
			string tmp(file);
			if (ImGui::Button("Load")) LoadModel(tmp);
			if (ImGui::Button("RemoveAll")) RemoveAllNodes();
			ImGui::DragFloat("Load model scale", &modelscale, 0.001, 0.003, 1.0);
			static bool f2 = false;
			ImGui::Checkbox("Use Normal map", &f2); p->normalenable = f2 ? 1 : 0;
			if(ImGui::Button("PersCamera")) SetCameraPers();
			if (ImGui::Button("OrthCamera")) SetCameraOrth();
			ImGui::Combo("Shading mode", &p->mode, "shading\0shadowMap0\0shadowMap1\0shadowMap2\0shadowFactor0\0shadowFactor1\0shadowFactor2\0");
			static int cameraid = 0;
			
			if (ImGui::Button("Go to Light Camera")) {
				auto s= &p->e_shadowmap.cameras[cameraid];
				camera->setProjectionType(Camera::PT_PERSPECTIVE);
				camera->lookAt(s->getPosition(), s->getDirection(), Vector3(0, 1, 0));
				camera->setPerspective(45, float(w) / float(h), s->getNear(), s->getFar());
			}
			ImGui::InputInt("Light ID (0-3)", &cameraid);
			ImGui::DragFloat3("CameraPos", &camera->m_Position[0]);
			static bool showlightwindow = 1;
			if (ImGui::Button("Light Window")) showlightwindow = !showlightwindow;
			if (showlightwindow) {
				ImGui::Begin("Light");
				ImGui::DragFloat3("Light Center", &p->LightCenter[0]);
				ImGui::DragFloat("Light Near", &p->lightnear,0.01,0.01);
				ImGui::DragFloat("Light Far", &p->lightfar,0.01);
				ImGui::DragFloat("ambient light", &p->ambient, 0.002, 0.0, 1.0);
				

				ImGui::Text("hard shadowmap");
				ImGui::DragFloat3("Light0 position", &p->lights[0].positon[0], 0.1);
				ImGui::DragFloat3("Light0 color", &p->lights[0].color[0], 0.01);
				ImGui::DragFloat("ShadowCameraWidth0", &p->sw[0]);

				ImGui::DragFloat3("Light1 position", &p->lights[1].positon[0], 0.1);
				ImGui::DragFloat3("Light1 color", &p->lights[1].color[0], 0.01);
				ImGui::DragFloat("ShadowCameraWidth1", &p->sw[1]);

				ImGui::Text("soft shadowmap");
				ImGui::DragFloat("Blur radius", &p->vsm.blureffect.radius, 0.001, 0.003, 3.0);
				ImGui::DragFloat("ReduceBleeding", &p->vsm.ReduceBleeding, 0.001, 0.003, 1.0);
				ImGui::DragFloat3("Light2 position", &p->lights[2].positon[0], 0.1);
				ImGui::DragFloat3("Light2 color", &p->lights[2].color[0], 0.01);
				ImGui::DragFloat("ShadowCameraWidth2", &p->sw[2]);
				for (int i = 0; i < 4; i++) {
					auto dir = p->LightCenter - p->lights[i].positon;
					dir.normalize();
					p->lights[i].direction = dir;
					if (lightpoint[i] != nullptr)
						lightpoint[i]->setTranslation(p->lights[i].positon);
				}
				if (ImGui::Button("LoadPreset")) p->LoadConfigs();
				ImGui::End();
			}
			//static int f3 = 1;
			//ImGui::Combo("Camera Mode", &f3, "orth\0pers\0");
			//if (f3 == 0) SetCameraOrth(); else SetCameraPers();

			ImGui::End();
		}
#endif
	}

	void CreateScene() {
		scene = new OctreeSceneManager("scene1", render);
		SceneContainer::getInstance().add(scene);
		SceneNode* pRoot = scene->GetSceneRoot();
		camera = scene->CreateCamera("main");
		camera->lookAt(Vector3(0, 0, 1.5), Vector3(0, 0, 0), Vector3(0, 1, 0));
		camera->setPerspective(45, float(w) / float(h), 0.01, 50);


		//scene1 light
		{
			Light * sunLight;
			sunLight = scene->CreateLight("light1", Light::LT_SPOT);
			sunLight->setDirection(Vector3(1.0, -1.0, -1.0));
			sunLight->setPosition(Vector3(-1, 2, 6));
			sunLight->SetMaxDistance(9);
			sunLight->Exponent = 3;
			sunLight->Cutoff = 0.8;
			sunLight->setCastShadow(true);
			sunLight->Diffuse = Vector3(1.0, 1.0, 1.0);
			scene->setActiveLightVector(sunLight, 0);
		}

		//FrameListener * camera_op = new FrameListenerDebug(camera, pRoot, scene, h, w, sysentry->m_pipeline);
		//framelistener_list.push_back(camera_op);

		loadModels();
	}
	void loadModels() {
		SceneManager* scene = SceneContainer::getInstance().get("scene1");
		SceneNode* root = scene->GetSceneRoot();
		SceneNode* s;

		for (int i = 0; i < 3; i++) {
			s = LoadMeshtoSceneNode("model/pbr_test/sphere7d.obj", "light point" + to_string(i));
			s->scale(0.03, 0.03, 0.03);
			lightpoint[i] = s;
			root->attachNode(s);
		}

		if (!use_huawei_head) {
			s = LoadMeshtoSceneNode("model/head/head5.obj", "Head");
			s->scale(3, 3, 3);
		}
		//else 
		//{
		//	float scale = 300;
		//	float width = w / scale, height = h / scale;

		//	stringstream mesh_name;
		//	mesh_name << "model/head/huawei/" << head_index << ".obj";
		//	s = LoadMeshtoSceneNode(mesh_name.str(), "Head");
		//	s->translate(-width / 2 - 0.5 / scale, height / 2 - 0.5 / scale, 0);
		//	//camera->lookAt(Vector3(0, 0, 1000 / scale), Vector3(0, 0, 0), Vector3(0, 1, 0));
		//	//camera->setProjectionType(Camera::PT_ORTHOGONAL);
		//	//camera->setFrustum(-width / 2, width / 2, -height / 2, height / 2, 0.1, 3000 / scale);

		//	s->scale(1 / scale, 1 / scale, 1 / scale);
		//}
		root->attachNode(s);

	}

};