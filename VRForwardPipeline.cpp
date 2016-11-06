#include "VRForwardPipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "GlobalVariablePool.h"
#include "Camera.h"

void VRForwardEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	
	mainwindow = rt_mgr.get("MainWindow");

	rt_render_left = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_forward_left");
	for (auto& x : rt_render_left->m_attach_textures) {
		x.second->MSAASampleNum = 4;
	}
	rt_render_right = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_forward_right");
	for (auto& x : rt_render_right->m_attach_textures) {
		x.second->MSAASampleNum = 4;
	}
	rt_resolve_left = rt_mgr.CreateRenderTargetFromPreset("basic", "msaa_resolve_left");
	rt_resolve_left->createInternalRes();
	rt_render_left->createInternalRes();

	rt_resolve_right = rt_mgr.CreateRenderTargetFromPreset("basic", "msaa_resolve_right");
	rt_resolve_right->createInternalRes();

	rt_render_right->createInternalRes();

	auto& tm = TextureManager::getInstance();

	out_color_left = tm.get("msaa_resolve_left_tex1");
	out_depth_left = tm.get("msaa_resolve_left_tex2");

	out_color_right = tm.get("msaa_resolve_right_tex1");
	out_depth_right = tm.get("msaa_resolve_right_tex2");

	forward_pass = PassManager::getInstance().LoadPass("forward_prog.json");
}

void VRForwardEffect::Render()
{
	SceneManager* scene = SceneContainer::getInstance().get(input_scenename);

	Camera *camera = scene->getCamera(input_cameraname);
	RenderSystem* mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);

	forward_pass->mProgram->setProgramConstantData("LightColor", &LightColor[0], "vec3", sizeof(Vector3));
	forward_pass->mProgram->setProgramConstantData("LightPosition", &LightPosition[0], "vec3", sizeof(Vector3));

	//**********************************************************************************************************************
	//************************						LEFT										 ***************************
	//**********************************************************************************************************************
	GlobalVariablePool* gp = GlobalVariablePool::GetSingletonPtr();
	gp->SetProjectMatrix(projLeft);
	gp->SetViewMatrix(viewLeft);
	/*forward_pass->mProgram->setProgramConstantData("Projection", &projLeft, "mat4", sizeof(Matrix4));
	forward_pass->mProgram->setProgramConstantData("ViewMatrix", &viewLeft, "mat4", sizeof(Matrix4));*/

	mRenderSystem->RenderPass(NULL, queue, forward_pass, rt_render_left);
	rt_render_left->BlitTexture(rt_resolve_left, "color0", "color0");
	rt_render_left->BlitTexture(rt_resolve_left, "depth", "depth");
	//**********************************************************************************************************************
	//************************						RIGHT										 ***************************
	//**********************************************************************************************************************
	gp->SetProjectMatrix(projRight);
	gp->SetViewMatrix(viewRight);
	/*forward_pass->mProgram->setProgramConstantData("Projection", &projRight, "mat4", sizeof(Matrix4));
	forward_pass->mProgram->setProgramConstantData("ViewMatrix", &viewRight, "mat4", sizeof(Matrix4));*/
	rt_render_right->BlitTexture(rt_resolve_right, "color0", "color0");
	rt_render_right->BlitTexture(rt_resolve_right, "depth", "depth");
	mRenderSystem->RenderPass(NULL, queue, forward_pass, rt_render_right);
}

void VRForwardPipeline::Init()
{
	eForward.input_scenename = "scene1";
	eForward.input_cameraname = "main";
	eForward.Init();
	//!!! use left eye result as display
	eShowtex.input_texture = eForward.out_color_left; 
	eShowtex.Init();
}

void VRForwardPipeline::Render()
{
	eForward.Render();
	eShowtex.Render();
}

void VRForwardPipeline::GetStereoTex(GLuint & l, GLuint & r)
{
	l = ((EGLTexture*)(eForward.out_color_left))->texture_id;
	r = ((EGLTexture*)(eForward.out_color_right))->texture_id;
}
