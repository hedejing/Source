#pragma once
#include "ForwardPipeline.h"
#include "AppFrameWork2.h"
#include "Interpolation.h"
#include "pattern.h"

#include <fstream>
class App_VR_Experience :public App {
public:
	bool free = true;
	int mode = 0;	//0:speed 1:interpolation
	vector<InterpData> marks;
	Pattern* pattern;

	void Init() {
		render = new EGLRenderSystem;
		render->SetWandH(w, h);
		render->Initialize();
		InitDevice();
		InitPipeline();
		CreateScene();
		//LoadMarks("test2.txt");

		pattern = new SpeedTest(camera, 0, 0, 2, 0);

		//pattern = new InterpolationTest(camera, marks);
	}

	void InitPipeline()
	{
		pipeline = new ForwardPipeline;
		pipeline->Init();
		auto p = static_cast<ForwardPipeline*>(pipeline);
		p->eForward.LightPosition = Vector3(90, 150, 0);
	}

	void Render() {
		UpdateGUI();
		double duration;
		static bool first = true;
		static double lastTime;
		if (!free) {
			if (first) {
				duration = 0;
				lastTime = AbsolutTime;
				first = false;
			}
			else {
				duration = AbsolutTime - lastTime;
				lastTime = AbsolutTime;
			}
			pattern->UpdateCamera(duration);
		}
		else {
			lastTime = AbsolutTime;
		}
		pipeline->Render();


	}
	void UpdateGUI() {
		GUI::NewFrame();
		ImGui::Begin("Params");
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		if (ImGui::TreeNode("Setting"))
		{
			if (ImGui::Button("Reset"))
				pattern->Reset();
			ImGui::SameLine();
			if (ImGui::Button("Start/Stop"))
				free = !free;

			const char* items[] = { "Speed", "Interpolation" };
			ImGui::Combo("Mode", &mode, items, 2);

			if (mode == 0) {
				auto p = static_cast<SpeedTest*>(pattern);
				ImGui::DragFloat("Pitch", &(p->PitchSpeed), 0.1);
				ImGui::DragFloat("Yaw", &(p->YawSpeed), 0.1);
				ImGui::DragFloat("Linear", &(p->LinearSpeed), 0.1);
				ImGui::DragFloat("Accelerate", &(p->Acceleration), 0.01);
			}

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Camera&Light"))
		{
			auto p = static_cast<ForwardPipeline*>(pipeline);
			static Vector3 pos, dir;
			pos = camera->getPosition();
			dir = camera->getDirection();
			ImGui::DragFloat3("CameraPos", &pos[0], 0.05);
			ImGui::DragFloat3("CameraDir", &dir[0], 0.05);

			ImGui::DragFloat3("LightColor", &p->eForward.LightColor[0], 0.05);
			ImGui::DragFloat3("LightPosition", &p->eForward.LightPosition[0], 0.05);

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Marks"))
		{
			if (mode == 0) {
				if (ImGui::Button("Clear"))
					marks.clear();
				ImGui::SameLine();
				if (ImGui::Button("Push"))
					marks.push_back(InterpData(AbsolutTime, camera->getPosition(), camera->getDirection()));
				ImGui::SameLine();
				if (ImGui::Button("Pop") && !marks.empty())
					marks.pop_back();

				//display
				for (auto &m : marks) {
					ImGui::PushItemWidth(80);
					ImGui::InputFloat("", &m.Time);
					ImGui::PopItemWidth();

					ImGui::SameLine();

					ImGui::PushItemWidth(150);
					ImGui::InputFloat3("", &m.Position[0]);
					ImGui::PopItemWidth();

					ImGui::SameLine();

					ImGui::PushItemWidth(150);
					ImGui::InputFloat3("", &m.Direction[0]);
					ImGui::PopItemWidth();
				}
			}
			//save&load
			static char name[50] = "";
			ImGui::InputText("File Name", name, 50);
			if (ImGui::Button("Load"))
				LoadMarks(name);
			ImGui::SameLine();
			if (ImGui::Button("Save"))
				SaveMarks(name);
			ImGui::TreePop();
		}
		ImGui::End();
	}
	void CreateScene() {
		scene = new OctreeSceneManager("scene1", render);
		SceneContainer::getInstance().add(scene);
		SceneNode* pRoot = scene->GetSceneRoot();
		camera = scene->CreateCamera("main");
		camera->lookAt(Vector3(-7, 1, 0), Vector3(0, 1, 0), Vector3(0, 1, 0));
		float fovy = 45;
		camera->setPerspective(fovy, float(w) / float(h), 0.01, 100);
		loadModels();
	}
	void loadModels() {
		SceneNode* root = scene->GetSceneRoot();
		SceneNode* s = LoadMeshtoSceneNode("model/16.10/sponza/sponza.obj", "sponza");
		root->attachNode(s);
	}

	void LoadMarks(string filename)
	{
		ifstream input("..\\..\\Workspace\\VRExperience\\" + filename);
		if (input.fail()){
			cout << "\"" << filename << "\"" << "doesn't exist" << endl;
			return;
		}
		marks.clear();
		double t;
		Vector3 pos, dir;
		while (input >> t >> pos.x >> pos.y >> pos.z >> dir.x >> dir.y >> dir.z)
			marks.push_back(InterpData(t, pos, dir));
		input.close();
		delete pattern;
		pattern = new InterpolationTest(camera, marks);
	}

	void SaveMarks(string filename)
	{
		if (marks.size() == 0)
			return;
		double start = marks[0].Time;
		for (int i = 0; i < marks.size(); i++)
			marks[i].Time -= start;

		ofstream output("..\\..\\Workspace\\VRExperience\\" + filename);
		output.clear();
		
		for (auto &m:marks) {
			auto pos = m.Position;
			auto dir = m.Direction;
			output << m.Time << "\t";
			output << pos.x << "\t" << pos.y << "\t" << pos.z << "\t";
			output << dir.x << "\t" << dir.y << "\t" << dir.z << endl;
		}
		output.close();
	}
};