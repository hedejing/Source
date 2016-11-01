#include "ForwardPipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"



void ForwardEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	output_RT = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_forward");
	output_RT->createInternalRes();
	auto& tm = TextureManager::getInstance();


	out_color = tm.get("effect_forward_tex1");

	out_depth = tm.get("effect_forward_tex2");

	forward_pass = PassManager::getInstance().LoadPass("forward_prog.json");
}

void ForwardEffect::Render()
{

	SceneManager* scene = SceneContainer::getInstance().get(input_scenename);

	auto camera = scene->getCamera(input_cameraname);
	RenderSystem* mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);
	
	forward_pass->mProgram->setProgramConstantData("LightColor",&LightColor[0] , "vec3", sizeof(Vector3));
	forward_pass->mProgram->setProgramConstantData("LightPosition", &LightPosition[0], "vec3", sizeof(Vector3));
	// render to a buffer
	mRenderSystem->RenderPass(camera, queue, forward_pass, output_RT);

}

void ForwardPipeline::Init()
{
	eForward.input_scenename = "scene1";
	eForward.input_cameraname = "main";
	eForward.Init();
	eShowtex.input_texture = eForward.out_color;
	eShowtex.Init();
}

void ForwardPipeline::Render()
{
	eForward.Render();
	eShowtex.Render();
}
