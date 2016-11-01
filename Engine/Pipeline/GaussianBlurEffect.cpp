#include "GaussianBlurEffect.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"




void GaussianBlurEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	auto& pm = PassManager::getInstance();

	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;
	string presetname = BlurFloatTexture ? "basic_float" : "basic";
	blur1 = rt_mgr.CreateRenderTargetFromPreset(presetname, "effect_blur1");
	blur2 = rt_mgr.CreateRenderTargetFromPreset(presetname, "effect_blur2");
	blur_pass = pm.LoadPass("blur_prog.json");
	blur1_texture=blur1->m_attach_textures["color0"];
	out_blur = blur2->m_attach_textures["color0"];
}

void GaussianBlurEffect::Render()
{
	//get a screen quad
	auto quad = GlobalResourceManager::getInstance().as_meshManager.get("ScreenQuad.obj");
	RenderQueueItem item;
	item.asMesh = quad;
	RenderQueue queue2;
	queue2.push_back(item);

	auto p = blur_pass->mProgram;
	Vector2 a(1.0, 0), b(0, 1.0);

	p->setProgramConstantData("u_texture", input_color);
	p->setSampler("u_texture", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("dir", &a, "vec2", sizeof(Vector2));
	p->setProgramConstantData("radius", &radius, "float", sizeof(float));
	mRenderSystem->RenderPass(NULL, queue2, blur_pass, blur1);

	p->setProgramConstantData("u_texture", blur1_texture);
	p->setProgramConstantData("dir", &b, "vec2", sizeof(Vector2));
	mRenderSystem->RenderPass(NULL, queue2, blur_pass, blur2);
}
