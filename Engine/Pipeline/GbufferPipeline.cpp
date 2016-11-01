#include "GbufferPipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "Camera.h"


void GBufferEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	output_gbufferRT = rt_mgr.CreateRenderTargetFromPreset("gbuffer", "effect_gbuffer");
	for (auto& x : output_gbufferRT->m_attach_textures) {
		x.second->MSAASampleNum = MSAASampleNum;
	}
	output_gbufferRT->createInternalRes();
	auto& tm = TextureManager::getInstance();


	diffuse = tm.get("effect_gbuffer_tex1");
	
	specular = tm.get("effect_gbuffer_tex2");
	normal1 = tm.get("effect_gbuffer_tex3");
	normal2 = tm.get("effect_gbuffer_tex4");
	depth = tm.get("effect_gbuffer_tex5");
	
	gbuffer_pass=PassManager::getInstance().LoadPass("gbuffer_prog.json");
	gbuffer_pass->UseInstance = true;
}

void GBufferEffect::Render()
{
	//获取要画的场景
	SceneManager* scene = SceneContainer::getInstance().get(input_scenename);
	auto camera = scene->getCamera(input_cameraname);
	RenderSystem* mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	//获取摄像机内的物体，暂时取全部
	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);//暂时禁用了octree的剔除

	float near = camera->getNear();
	float far = camera->getFar();
	auto p = gbuffer_pass->mProgram;
	p->setProgramConstantData("near", &near, "float", sizeof(float));
	p->setProgramConstantData("far", &far, "float", sizeof(float));
	// render to a buffer
	mRenderSystem->RenderPass(camera, queue, gbuffer_pass, output_gbufferRT);

}
