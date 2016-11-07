#pragma once
#include "ForwardPipeline.h"
#include "AppFrameWork2.h"
#include "pattern.h"
#include "openvr.h"
#include "VRInstanceForwardPipeline.h"
#include <fstream>
#include "VRForwardPipeline.h"
#include "SimpleProfile.h"
#include "EdushiVRPipeline.h"

#define VR
//#define VRInstance
#define Edushi
class App_VR_Experience :public App {
public:
	bool free = true;
	int PatternMode = 0;	//0:speed 1:interpolation
	vector<InterpData> marks;
	Pattern* pattern;
	SimpleProfile profile;

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
#ifdef VR
		VRInit();
#endif // VR
	}

	void InitPipeline()
	{
#ifdef VR
	#ifdef VRInstance
			pipeline = new VRInstanceForwardPipeline;
			auto p = static_cast<VRInstanceForwardPipeline*>(pipeline);
	#else
		#ifdef Edushi
			pipeline = new EdushiVRPipeline;
			auto p = static_cast<EdushiVRPipeline*>(pipeline);
		#else
			pipeline = new VRForwardPipeline;
			auto p = static_cast<VRForwardPipeline*>(pipeline);
		#endif // EdushiVRPipeline
	#endif
#else
		pipeline = new ForwardPipeline;
		auto p = static_cast<ForwardPipeline*>(pipeline);
#endif // VR
		pipeline->Init();
		//p->eForward.LightPosition = Vector3(90, 150, 0);
		sysentry->setPipeline(pipeline);
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


#ifdef VR
		UpdateProjectView();
#endif // VR
		pipeline->Render();
#ifdef VR
		VRSubmitTexture();
#endif // VR
	}
	void UpdateGUI() {
		GUI::NewFrame();
		ImGui::Begin("Params");
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
#ifdef Edushi
		auto p = static_cast<EdushiVRPipeline*>(pipeline);
		if (ImGui::TreeNode("Profile"))
		{
			ImGui::TextWrapped(p->profile.ToString().c_str());
			ImGui::TreePop();
		}
#endif
		if (ImGui::TreeNode("Setting"))
		{
			if (ImGui::Button("Reset"))
				pattern->Reset();
			ImGui::SameLine();
			if (ImGui::Button("Start/Stop"))
				free = !free;

			const char* items[] = { "Speed", "Interpolation" };
			ImGui::Combo("Mode", &PatternMode, items, 2);

			if (PatternMode == 0) {
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
			if (PatternMode == 0) {
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

	void VRInit()
	{
		//******************************************************************************
		// openvr init
		//******************************************************************************
		//Loading the SteamVR Runtime
		bool success = true;
		vr::EVRInitError eError = vr::VRInitError_None;
		m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

		if (eError != vr::VRInitError_None)
		{
			m_pHMD = NULL;
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
			success = false;
		}

		m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);
		if (!m_pRenderModels)
		{
			m_pHMD = NULL;
			vr::VR_Shutdown();

			char buf[1024];
			sprintf_s(buf, sizeof(buf), "Unable to get render model interface: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
			success = false;
		}

		//init compositor
		vr::EVRInitError peError = vr::VRInitError_None;

		if (!vr::VRCompositor())
		{
			printf("Compositor initialization failed. See log file for details\n");
			success = false;
		}

		nearClip = 0.1f;
		farClip = 30.0f;
		m_pHMD->GetRecommendedRenderTargetSize(&renderWidth, &renderHeight);

		//init not success, shut down
		if (!success)
			if (m_pHMD)
			{
				vr::VR_Shutdown();
				m_pHMD = NULL;
			}

		//init projection mat and transition matrix between eyes and HMD
		SetupCameras();
	}

	void SetupCameras()
	{
		projLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
		projRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
		eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
		eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
	}

	Matrix4 GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye)
	{
		if (!m_pHMD)
			return Matrix4();

		vr::HmdMatrix44_t mat = m_pHMD->GetProjectionMatrix(nEye, nearClip, farClip, vr::API_OpenGL);

		return Matrix4(
			mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3],
			mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3],
			mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3],
			mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]
		);
	}

	Matrix4 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye)
	{
		if (!m_pHMD)
			return Matrix4();

		vr::HmdMatrix34_t matEyeRight = m_pHMD->GetEyeToHeadTransform(nEye);
		Matrix4 matrixObj(
			matEyeRight.m[0][0], matEyeRight.m[0][1], matEyeRight.m[0][2], matEyeRight.m[0][3],
			matEyeRight.m[1][0], matEyeRight.m[1][1], matEyeRight.m[1][2], matEyeRight.m[1][3],
			matEyeRight.m[2][0], matEyeRight.m[2][1], matEyeRight.m[2][2], matEyeRight.m[2][3],
			0, 0, 0, 1.0f
		);

		return matrixObj.inverse();
	}

	Matrix4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose)
	{
		Matrix4 matrixObj(
			matPose.m[0][0], matPose.m[0][1], matPose.m[0][2], matPose.m[0][3],
			matPose.m[1][0], matPose.m[1][1], matPose.m[1][2], matPose.m[1][3],
			matPose.m[2][0], matPose.m[2][1], matPose.m[2][2], matPose.m[2][3],
			0, 0, 0, 1.0f
		);
		return matrixObj;
	}

	void VRSubmitTexture()
	{
		//submit texture
#ifdef VRInstance	
		GLuint texID;
		static_cast<VRInstanceForwardPipeline*>(pipeline)->GetStereoTex(texID);
		vr::Texture_t EyeTexture = { (void*)texID, vr::API_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &EyeTexture);
		vr::VRCompositor()->Submit(vr::Eye_Right, &EyeTexture);
#else
		GLuint leftID, rightID;

#ifdef Edushi
		static_cast<EdushiVRPipeline*>(pipeline)->GetStereoTex(leftID, rightID);
		//((EdushiVRPipeline*)(this->sysentry->m_pipeline))->GetStereoTex(leftID, rightID);
#else
		static_cast<VRForwardPipeline*>(pipeline)->GetStereoTex(leftID, rightID);
#endif // Edushi
		vr::Texture_t leftEyeTexture = { (void*)leftID, vr::API_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);

		vr::Texture_t rightEyeTexture = { (void*)rightID, vr::API_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
#endif
		glFlush();
		//glFinish();

		//UpdateHMDMatrixPose
		vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

		for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
		{
			if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
			{
				devicePose[nDevice] = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
			}
		}

		if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
		{
			HMDPos = devicePose[vr::k_unTrackedDeviceIndex_Hmd].inverse();
		}
	}

	void UpdateProjectView()
	{
#ifdef Edushi
		/*auto p = static_cast<EdushiVRPipeline*>(pipeline);
		p->projLeft = projLeft;
		p->projRight = projRight;
		p->viewLeft = eyePosLeft * HMDPos;
		p->viewRight = eyePosRight * HMDPos;*/
		((EdushiVRPipeline*)(this->sysentry->m_pipeline))->projLeft = projLeft;
		((EdushiVRPipeline*)(this->sysentry->m_pipeline))->projRight = projRight;

		((EdushiVRPipeline*)(this->sysentry->m_pipeline))->viewLeft = eyePosLeft * HMDPos;
		((EdushiVRPipeline*)(this->sysentry->m_pipeline))->viewRight = eyePosRight * HMDPos;
#else
#ifdef VRInstance
		auto p = static_cast<VRInstanceForwardPipeline*>(pipeline);
#else
		auto p = static_cast<VRForwardPipeline*>(pipeline);
#endif // VRInstance
		p->eForward.projLeft = projLeft;
		p->eForward.projRight = projRight;
		p->eForward.viewLeft = eyePosLeft * HMDPos;
		p->eForward.viewRight = eyePosRight * HMDPos;
#endif
	}

	private:
		//VR config
		vr::IVRSystem *m_pHMD;
		vr::IVRRenderModels *m_pRenderModels;//model of devices
		vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
		Matrix4 devicePose[vr::k_unMaxTrackedDeviceCount];
		uint32_t renderWidth;
		uint32_t renderHeight;
		Matrix4 projLeft;
		Matrix4 projRight;
		Matrix4 eyePosLeft;
		Matrix4 eyePosRight;
		Matrix4 HMDPos;
		float nearClip;
		float farClip;
};