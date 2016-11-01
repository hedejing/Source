#include "ShadowMapGenerateEffect.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "Camera.h"
#include "OPENGL_Include.h"
#include <sstream>

void ShadowMapGenerateEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	auto& pm = PassManager::getInstance();

	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	Matrix4 bias(
		0.5, 0, 0, 0.5,
		0, 0.5, 0, 0.5,
		0, 0, 0.5, 0.5,
		0, 0, 0, 1);
	//Matrix4 bias_2(
	//	0.5, 0, 0, 0.5,
	//	0, 0.5, 0, 0.5,
	//	0, 0, 1.0, 0,
	//	0, 0, 0, 1);
	scale_bias = bias;

	createShadowRT();
	Camera c;
	for (int i = 0; i < LightNum; i++) {
		cameras.push_back(c);
	}
	shadowmap_pass = pm.LoadPass("shadowmap/shadowmap_prog.json");

}
void ShadowMapGenerateEffect::createShadowRT()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	ShadowMapRT = vector<RenderTarget*>(LightNum, NULL);
	out_ShadowMapTexture = vector<Texture*>(LightNum, NULL);
	out_LightPosTexture= vector<Texture*>(LightNum, NULL);
	out_ShadowMatrix = vector<Matrix4>(LightNum);
	out_LightViewMatrix = vector<Matrix4>(LightNum);
	for (int i = 0; i < LightNum; i++) {
		stringstream ss;
		ss << "effect_shadowmap" << i;
		ShadowMapRT[i]= rt_mgr.CreateRenderTargetFromPreset("basic_float", ss.str());
		ShadowMapRT[i]->width = width;
		ShadowMapRT[i]->height = height;
		for (auto tex : ShadowMapRT[i]->m_attach_textures) {
			tex.second->width = width;
			tex.second->height = height;
			tex.second->mipmap_mode = Texture::TextureMipmap_None;
		}
		ShadowMapRT[i]->createInternalRes();
		out_ShadowMapTexture[i] = ShadowMapRT[i]->m_attach_textures["depth"];
		out_LightPosTexture[i] = ShadowMapRT[i]->m_attach_textures["color0"];
	}
}


void ShadowMapGenerateEffect::Render()
{
	SceneManager* scene = SceneContainer::getInstance().get(input_scenename);
	auto MainCamera = scene->getCamera(input_cameraname);

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(MainCamera, queue);
	for (int i = 0; i < LightNum; i++) {
		Matrix4 view_light =cameras[i].getViewMatrix();
		Matrix4 proj_light = cameras[i].getProjectionMatrixDXRH();
		Vector4 a(0, 0, 0, 1);
		auto b = view_light*a;
		float farZ = cameras[i].getFar();
		float nearZ = cameras[i].getNear();
		shadowmap_pass->mProgram->setProgramConstantData("proj_light", &proj_light[0][0], "mat4", sizeof(Matrix4));
		shadowmap_pass->mProgram->setProgramConstantData("view_light", &view_light[0][0], "mat4", sizeof(Matrix4));
		shadowmap_pass->mProgram->setProgramConstantData("Far", &farZ, "float", sizeof(float));
		shadowmap_pass->mProgram->setProgramConstantData("Near", &nearZ, "float", sizeof(float));
		shadowmap_pass->mProgram->setProgramConstantData("LinearDepth", &LinearDepth, "int", sizeof(int));
		mRenderSystem->RenderPass(MainCamera, queue, shadowmap_pass, ShadowMapRT[i]);
		out_ShadowMatrix[i]= scale_bias*proj_light * view_light;
		out_LightViewMatrix[i] = view_light;
	}
}

 

void ShadowMapGenerateEffect::CalcShadowMat(Camera& camera, Matrix4& out_shadow_mat)
{
	auto view_light = camera.getViewMatrix();
	auto proj_light = camera.getProjectionMatrixDXRH();
	out_shadow_mat = scale_bias * proj_light * view_light;
}

void ShadowMapGenerateEffect::SetLightCamera(Camera& camera,Vector3 lightpos, Vector3 lookcenter, float width, float height, float tnear, float tfar,bool perspective)
{
	Vector3 up(0,1,0);
	if (up == lookcenter)
		up = Vector3(1, 0, 0);
	camera.lookAt(lightpos, lookcenter,up);
	
	
	if (perspective) {
		camera.setPerspective(45, 1, tnear, tfar);
		camera.setFrustum(-width / 2, width / 2, -height / 2, height / 2, tnear, tfar);
		camera.setProjectionType(Camera::PT_PERSPECTIVE);
	}
	else {
		camera.setFrustum(-width / 2, width / 2, -height / 2, height / 2, tnear, tfar);
		camera.setProjectionType(Camera::PT_ORTHOGONAL);
	}

	//test
	//Matrix4 proj = camera.getProjectionMatrixDXRH();

	//float p = proj[2][2], q = proj[2][3];
	//Vector4 a(0.5, 0, -0.2, 1),b(0,0,-2,1);

	//float z1 = -0.2;
	//auto c = proj*a;
	//auto d = proj*b;

	//float z = (z1*p + q)/-z1;
	//
	//float z2 = q / (-p - z);
	//c.z /= c.w;
	//c.z = c.z*0.5 + 0.5;
	//c.z *= c.w;
	//d.z /= d.w;


	


}

void ShadowMapGenerateEffect::SetLightCamera(int index, LightCameraParam& p)
{
	SetLightCamera(cameras[index], p.LightPosition, p.LightPosition + p.LightDir, p.width, p.height, p.Near, p.Far, p.perspective);
}

void ShadowMapTest::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	auto& pm = PassManager::getInstance();
	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;
	mainwindow = rt_mgr.get("MainWindow");

	input_scenename = "scene1";
	input_cameraname = "main";

	e_shadowmap.input_scenename = input_scenename;
	e_shadowmap.input_cameraname = input_cameraname;
	e_shadowmap.Init();

	shadowmapDraw_pass = pm.LoadPass("shadowmap/shadowmapDraw_prog.json");
}

void ShadowMapTest::Render()
{

	e_shadowmap.Render();

	SceneManager* scene = SceneContainer::getInstance().get(input_scenename);
	auto camera = scene->getCamera(input_cameraname);
	RenderSystem* mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);

	auto p = shadowmapDraw_pass->mProgram;

	//p->setProgramConstantData("shadow_map_sampler", e_shadowmap.out_shadowmap);
	//p->setSampler("shadow_map_sampler",SamplerManager::getInstance().get("ShadowMap"));
	//p->setProgramConstantData("shadow_mat", &e_shadowmap.out_shadow_mat[0][0], "mat4", sizeof(Matrix4));


	// render to a buffer
	mRenderSystem->RenderPass(camera, queue, shadowmapDraw_pass, mainwindow);
}
