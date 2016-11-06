#include "VRInstanceForwardPipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "GlobalVariablePool.h"
#include "Camera.h"

void VRInstanceForwardEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();

	mainwindow = rt_mgr.get("MainWindow");

	rt_render = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_forward");
	rt_render->createInternalRes();

	auto& tm = TextureManager::getInstance();

	out_color = tm.get("effect_forward_tex1");
	out_depth = tm.get("effect_forward_tex2");

	forward_pass = PassManager::getInstance().LoadPass("vr_instance_forward_prog.json");
}

void VRInstanceForwardEffect::Render()
{
	SceneManager* scene = SceneContainer::getInstance().get(input_scenename);

	Camera *camera = scene->getCamera(input_cameraname);
	RenderSystem* mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);

	forward_pass->mProgram->setProgramConstantData("LightColor", &LightColor[0], "vec3", sizeof(Vector3));
	forward_pass->mProgram->setProgramConstantData("LightPosition", &LightPosition[0], "vec3", sizeof(Vector3));
	
	//forward_pass->mProgram->setProgramConstantData("ProjectionLeft", &projLeft, "mat4", sizeof(Matrix4));
	//forward_pass->mProgram->setProgramConstantData("ProjectionRight", &projRight, "mat4", sizeof(Matrix4));
	//forward_pass->mProgram->setProgramConstantData("ViewMatrixLeft", &viewLeft, "mat4", sizeof(Matrix4));
	//forward_pass->mProgram->setProgramConstantData("ViewMatrixRight", &viewRight, "mat4", sizeof(Matrix4));
	GlobalVariablePool* gp = GlobalVariablePool::GetSingletonPtr();
	gp->SetProjectMatrix(projLeft);
	gp->SetViewMatrix(viewLeft);

	mRenderSystem->RenderPass(NULL, queue, forward_pass, rt_render);
} 

void VRInstanceForwardPipeline::Init()
{
	eForward.input_scenename = "scene1";
	eForward.input_cameraname = "main";
	eForward.Init();
	//!!! use left eye result as display
	eShowtex.input_texture = eForward.out_color;
	eShowtex.Init();
}

void VRInstanceForwardPipeline::Render()
{
	eForward.Render();
	eShowtex.Render();
}

void VRInstanceForwardPipeline::GetStereoTex(GLuint & tex)
{
	tex = ((EGLTexture*)(eForward.out_color))->texture_id;
}
