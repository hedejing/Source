#include "TestPipeline.h"
#include <sstream>
#include "Config.h"

void TestPipeline::Init()
{

	//e_forward.input_scenename = "scene1";
	//e_forward.input_cameraname = "main";
	//e_forward.Init();
	//e_showtex.input_texture = e_forward.out_color;
	//e_showtex.Init();


	e_gbuffer.input_scenename = "scene1";
	e_gbuffer.input_cameraname = "main";
	e_gbuffer.Init();

	e_ssao.depth = e_gbuffer.depth;
	e_ssao.normal = e_gbuffer.normal1;
	e_ssao.Init();

	//e_shadowmap.input_scenename = "scene1";
	//e_shadowmap.input_cameraname = "main";
	//e_shadowmap.Init();

	e_pbr.input_depth = e_gbuffer.depth;
	e_pbr.input_diffuse = e_gbuffer.diffuse;
	e_pbr.input_normal1 = e_gbuffer.normal1;
	e_pbr.input_normal2 = e_gbuffer.normal2;
	e_pbr.input_specular = e_gbuffer.specular;
	e_pbr.isRendertoMainWindow = false;
	e_pbr.ao_map = e_ssao.output_ao;
	//e_pbr.light_dir = e_shadowmap.light_direction;
	//e_pbr.shadow_map = e_shadowmap.out_shadowmap;
	//e_pbr.shadow_mat = e_shadowmap.out_shadow_mat;
	e_pbr.Init();

	e_mlaa.input_texture = e_pbr.color;
	e_mlaa.input_depth = e_gbuffer.depth;
	e_mlaa.Init();

	if (Config::instance().b_drawText)
		e_RenderText.Init();
	////e_fxaa.input_color = e_pbr.color;
	////e_fxaa.Init();

	//e_tonemap.input_color = e_mlaa.out_mlaa;
	////e_tonemap.Init();

	////e_mlaa.testTex();
	//e_showtex.input_texture = e_ssao.output_ao;

	//e_showtex.input_texture = e_mlaa.out_edge;
	//e_showtex.input_texture = e_mlaa.out_weight;
	//e_showtex.input_texture = e_shadowmap.out_shadowmap;
	//////e_showtex.input_texture = e_fxaa.out_fxaa;
	//e_showtex.input_texture = e_pbr.color;
	////e_showtex.input_texture = e_gbuffer.specular;
	e_showtex.input_texture = e_mlaa.out_mlaa;
	//e_showtex.input_texture = e_forward.out_color;
	//e_showtex.input_texture = e_gbuffer.normal2;
	e_showtex.Init();

	setParamTest();
}

void TestPipeline::Render()
{
	//e_forward.Render();
	e_shadowmap.Render();
	e_gbuffer.Render();
	e_ssao.Render();
	e_pbr.Render();
	e_mlaa.Render();

	////////e_fxaa.Render();
	////e_tonemap.Render();
	e_showtex.Render();
	if (Config::instance().b_drawText)
		DrawParam();
}
void TestPipeline::DrawParam()
{
	if (Config::instance().b_drawText <= 0) return;
	int h = Config::instance().height;
	int w = Config::instance().width;
	stringstream s1;
	Vector3 color = Vector3(1.0, 1.0, 1.0);
	float posx = 25;
	float fontsize = 0.5;
	float linedist = 25;
	float h_start_pos = h - 35;
	if (Config::instance().b_textpos == 1)
		posx = w / 2 + 25;
	s1 << "Shadowmap Resolution: " << e_shadowmap.width;
	e_RenderText.RenderText(s1.str(), posx, h_start_pos, fontsize, color);
	s1.str("");
	s1 << "AO Sample Num: " << e_ssao.num_samples_ssao;
	e_RenderText.RenderText(s1.str(), posx, h_start_pos - linedist, fontsize, color);
	s1.str("");
	s1 << "PBR  Sample Num: " << e_pbr.num_samples_IBL;
	e_RenderText.RenderText(s1.str(), posx, h_start_pos - linedist * 2, fontsize, color);
	s1.str("");
	s1 << "MLAA Max Search Step: " << e_mlaa.num_step;
	e_RenderText.RenderText(s1.str(), posx, h_start_pos - linedist * 3, fontsize, color);
	s1.str("");

}
void TestPipeline::setParamTest()
{
//	e_shadowmap.changeResolution(2048, 2048);
	reLink();
	e_pbr.changeSampleNum(4);
	e_mlaa.setSearchStep(64);
	e_ssao.setSampleNum(32);

	BoundingBox b;
	b.setMaximum(Vector3(282.859, 165.18, 801.142));
	b.setMinimum(Vector3(-17.1412 ,-134.82, 501.142));
	auto ce=b.getCenter();
	auto hl=b.getHalfSize();
	//e_shadowmap.changeResolution(2048, 2048);
	//reLink();
	//e_pbr.changeSampleNum(32);
	//e_mlaa.setSearchStep(64);
	//e_ssao.setSampleNum(16);
}

void TestPipeline::reLink()
{
//	e_pbr.shadow_map = e_shadowmap.out_shadowmap;
}


