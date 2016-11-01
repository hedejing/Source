#include "ToneMappingPipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "EGLUtil.h"


void ToneMappingEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	mainwindow = rt_mgr.get("MainWindow");
	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	//tm.CreateTextureFromImage("ennis_c02.hdr");
	//vector<string> f = { "ennis_c00.hdr", "ennis_c01.hdr", "ennis_c02.hdr", "ennis_c03.hdr", "ennis_c04.hdr", "ennis_c05.hdr" };
	//Texture* t=tm.CreateTextureCubeMap(f);

	//t->createInternalRes();
	//rt_mgr.CreateRenderTargetFromPreset("basic", "luminance_buf");
	//tone_lum_pass=PassManager::getInstance().LoadPass("tonemap_luminance_prog.json");

	output_rt = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_tonemap");
	output_rt->createInternalRes();
	out_tonemap = output_rt->m_attach_textures["color0"];
	tonemap_pass=PassManager::getInstance().LoadPass("tonemap_prog.json");

}

void ToneMappingEffect::Render()
{

	//get a screen quad
	auto quad = GlobalResourceManager::getInstance().as_meshManager.get("ScreenQuad.obj");
	RenderQueueItem item;
	item.asMesh = quad;
	RenderQueue queue2;
	queue2.push_back(item);

	//RenderTarget * rt2 = RenderTargetManager::getInstance().get("luminance_buf");
	//auto& tm = TextureManager::getInstance();
	//tone_lum_pass->mProgram->setProgramConstantData("image", input_color);
	//mRenderSystem->RenderPass(NULL, queue2, tone_lum_pass, rt2);

	////tm.get("luminance_buf_tex1")->GenerateMipmap();

	auto p = tonemap_pass->mProgram;
	p->setProgramConstantData("linearTex", input_color);
	p->setProgramConstantData("exposure", &exposure, "float", sizeof(float));
	p->setProgramConstantData("UseTonemap", &ToneMapOn, "int", sizeof(int));
	mRenderSystem->RenderPass(NULL, queue2, tonemap_pass, output_rt);
}

void ToneMappingPipeline::Init()
{
	effect_forward.input_scenename = "scene1";
	effect_forward.input_cameraname = "main";
	effect_forward.Init();

	effect_tone.input_color = effect_forward.out_color;
	effect_tone.Init();

	effect_show_tex.input_texture = effect_tone.out_tonemap;
	effect_show_tex.Init();
}

void ToneMappingPipeline::Render()
{
	effect_forward.Render();
	effect_tone.Render();
	effect_show_tex.Render();
}
