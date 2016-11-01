#include "SkyBoxEffect.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "Camera.h"
#include "MeshManager.h"


void SkyBoxEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	auto& pm = PassManager::getInstance();

	MeshManager::getSingletonPtr()->loadMesh_assimp_check("box", "default_asserts/skysphere.obj");
	mainwindow = rt_mgr.get("MainWindow");
	w = mainwindow->width;
	h = mainwindow->height;
	if (env_map == NULL) {
		auto skymapnames=GenCubemapList("default_asserts/skybox/sky0", ".png");
		env_map = tm.CreateTextureCubeMap(skymapnames);
	}
	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;
	rt_sky = rt_mgr.CreateRenderTargetFromPreset("basic_float", "effect_blur1");
	skybox_pass = pm.LoadPass("skybox/skybox_prog.json");
	out_skybox = rt_sky->m_attach_textures["color0"];
}

void SkyBoxEffect::Render()
{
	//get a screen quad
	auto quad = GlobalResourceManager::getInstance().as_meshManager.get("skysphere.obj");
	RenderQueueItem item;
	item.asMesh = quad;
	RenderQueue queue2;
	queue2.push_back(item);

	auto p = skybox_pass->mProgram;
	Matrix4 world = Matrix4::IDENTITY;
	world.setScale(Vector3(boxsize));
	p->setProgramConstantData("depth", input_depth);
	p->setProgramConstantData("front", input_color);
	p->setProgramConstantData("boxsize", &boxsize, "float", sizeof(float));
	p->setProgramConstantData("w", &w, "float", sizeof(float));
	p->setProgramConstantData("h", &h, "float", sizeof(float));
	p->setProgramConstantData("isHdrSky", &isHdrSky, "int", sizeof(int));
	p->setProgramConstantData("env_map", env_map);
	p->setSampler("env_map", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("WorldMatrix", &world, "mat4", sizeof(Matrix4));
	if(!state2)
		mRenderSystem->RenderPass(camera, queue2, skybox_pass, rt_sky);
	else {
		afterPass->mClearState.mclearcolorbuffer = false;
		mRenderSystem->RenderPass(camera, queue2, skybox_pass, rt_result);
	}
}
