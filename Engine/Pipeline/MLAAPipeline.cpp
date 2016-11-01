#include "MLAAPipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "EGLUtil.h"

#include "Image.h"

void MLAAEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	auto& pm = PassManager::getInstance();
	mainwindow = rt_mgr.get("MainWindow");
	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	areamap8 = tm.CreateTextureFromImage("areaMap/AreaMap900.tga", "areamap8");
	areamap8->mipmap_mode = Texture::TextureMipmap_None;
	areamap8->createInternalRes();
	areamap16 = tm.CreateTextureFromImage("areaMap/AreaMap1700.tga", "areamap16");
	areamap16->mipmap_mode = Texture::TextureMipmap_None;
	areamap16->createInternalRes();
	areamap32 = tm.CreateTextureFromImage("areaMap/AreaMap3300.tga", "areamap32");
	areamap32->mipmap_mode = Texture::TextureMipmap_None;
	areamap32->createInternalRes();
	areamap64 = tm.CreateTextureFromImage("areaMap/AreaMap6500.tga", "areamap64");
	areamap64->mipmap_mode = Texture::TextureMipmap_None;
	areamap64->createInternalRes();
	areamap128 = tm.CreateTextureFromImage("areaMap/AreaMap12900.tga", "areamap128");
	areamap128->mipmap_mode = Texture::TextureMipmap_None;
	areamap128->createInternalRes();
	rt_mgr.CreateRenderTargetFromPreset("basic", "rt_edge");
	rt_mgr.CreateRenderTargetFromPreset("basic", "rt_mlaablendweight");

	output_rt = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_mlaa");
	output_rt->createInternalRes();
	out_mlaa = tm.get("effect_mlaa_tex1");
	out_edge = tm.get("rt_edge_tex1");
	out_weight = tm.get("rt_mlaablendweight_tex1");

	blend_pass=pm.LoadPass("mlaa/mlaablend_prog.json");
	blend_weight_pass=pm.LoadPass("mlaa/mlaablendweight_prog.json");
	edge_detect_pass=pm.LoadPass("mlaa/edgedetect_prog.json");

	areamap = areamap32;

}

void MLAAEffect::Render()
{
	auto& tm = TextureManager::getInstance();

	//get a screen quad
	auto quad = GlobalResourceManager::getInstance().as_meshManager.get("ScreenQuad.obj");
	RenderQueueItem item;
	item.asMesh = quad;
	RenderQueue queue2;
	queue2.push_back(item);
	
	//edge detect
	RenderTarget * rt2 = RenderTargetManager::getInstance().get("rt_edge");	
	edge_detect_pass->mProgram->setProgramConstantData("image_sampler", input_texture);
	edge_detect_pass->mProgram->setSampler("image_sampler", SamplerManager::getInstance().get("ClampLinear"));
	edge_detect_pass->mProgram->setProgramConstantData("depth_sampler", input_depth);
	edge_detect_pass->mProgram->setSampler("depth_sampler", SamplerManager::getInstance().get("ClampLinear"));
	mRenderSystem->RenderPass(NULL, queue2, edge_detect_pass, rt2);

	//weight calculation
	RenderTarget * rt3 = RenderTargetManager::getInstance().get("rt_mlaablendweight");
	Texture* edge = tm.get("rt_edge_tex1");
	GpuProgram* p = blend_weight_pass->mProgram;
	p->setProgramConstantData("edge_sampler", edge);
	p->setSampler("edge_sampler", SamplerManager::getInstance().get("ClampLinear"));

	p->setProgramConstantData("edge_sampler_N", edge);
	p->setSampler("edge_sampler_N", SamplerManager::getInstance().get("ClampPoint"));

	p->setProgramConstantData("area_sampler", areamap);
	p->setSampler("area_sampler", SamplerManager::getInstance().get("ClampPoint"));
	p->setProgramConstantData("max_search_steps", &num_step, "int", sizeof(int));
	p->setProgramConstantData("max_distance", &max_dist, "int", sizeof(int));
	mRenderSystem->RenderPass(NULL, queue2, blend_weight_pass, rt3);

	//mlaa blend
	blend_pass->mProgram->setProgramConstantData("image_in_sampler", input_texture);
	blend_pass->mProgram->setSampler("image_in_sampler", SamplerManager::getInstance().get("ClampLinear"));
	blend_pass->mProgram->setProgramConstantData("weight_sampler", tm.get("rt_mlaablendweight_tex1"));
	blend_pass->mProgram->setSampler("weight_sampler", SamplerManager::getInstance().get("ClampPoint"));
	mRenderSystem->RenderPass(NULL, queue2, blend_pass, output_rt);


}

void MLAAEffect::testTex()
{
	out_mlaa->releaseInternalRes();
	out_mlaa->createInternalRes();
	output_rt->attachTexture(out_mlaa, "color0");
}

//void MLAAEffect::setSearchStep(search_step s)
//{
//
//	switch (s)
//	{
//	case MLAAEffect::step_4: {
//		areamap = areamap8;
//		num_step = 4;
//	}
//		break;
//	case MLAAEffect::step_8: {
//		areamap = areamap16;
//		num_step = 8;
//	}
//		break;
//	case MLAAEffect::step_16: {
//		areamap = areamap32;
//		num_step = 16;
//	}
//		break;
//	case MLAAEffect::step_32: {
//		areamap = areamap64;
//		num_step = 32;
//	}
//		break;
//	case MLAAEffect::step_64: {
//		areamap = areamap128;
//		num_step = 64;
//	}
//		break;
//	default:
//		break;
//	}
//
//}

void MLAAEffect::setSearchStep(int s)
{
	switch (s)
	{
	case 4: {
		areamap = areamap8;
		num_step = 4;
	}
							 break;
	case 8: {
		areamap = areamap16;
		num_step = 8;
	}
							 break;
	case 16: {
		areamap = areamap32;
		num_step = 16;
	}
							  break;
	case 32: {
		areamap = areamap64;
		num_step = 32;
	}
							  break;
	case 64: {
		areamap = areamap128;
		num_step = 64;
	}
							  break;
	default:
		areamap = areamap64;
		num_step = 32;
		break;
	}
	max_dist = 2 * num_step;
}

void MLAAPipiline::Init()
{
	e_forward.input_scenename = "scene1";
	e_forward.input_cameraname = "main";
	e_forward.Init();

	e_mlaa.input_texture = e_forward.out_color;
	e_mlaa.input_depth = e_forward.out_depth;
	e_mlaa.Init();

	e_show_tex.input_texture = e_mlaa.out_mlaa;
	e_show_tex.Init();
//	e_mlaa.setSearchStep(64);
}

void MLAAPipiline::Render()
{
	e_forward.Render();
	e_mlaa.Render();
	e_show_tex.Render();
}
