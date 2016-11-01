#include "SSRayTrace.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "Camera.h"


void SSRayTraceEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	auto& pm = PassManager::getInstance();

	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;
	output_rt = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_ssraytrace");
	output_rt->createInternalRes();
	
	output_reflection = tm.get("effect_ssraytrace_tex1");
	raytrace_pass = pm.LoadPass("ssraytrace/ssraytrace_prog.json");

	vector<string> e1 = { "ennis_c00.hdr", "ennis_c01.hdr", "ennis_c02.hdr", "ennis_c03.hdr", "ennis_c04.hdr", "ennis_c05.hdr" };
	vector<string> e2 = { "pisa_c00.hdr", "pisa_c01.hdr", "pisa_c02.hdr", "pisa_c03.hdr", "pisa_c04.hdr", "pisa_c05.hdr" };
	vector<string> e3 = { "cubemap/posx.jpg", "cubemap/negx.jpg", "cubemap/posy.jpg", "cubemap/negy.jpg", "cubemap/posz.jpg", "cubemap/negz.jpg" };
	vector<string> e4 = { "hdrmap/LightProbes/glacier_c00.hdr", "hdrmap/LightProbes/glacier_c01.hdr",
		"hdrmap/LightProbes/glacier_c02.hdr", "hdrmap/LightProbes/glacier_c03.hdr",
		"hdrmap/LightProbes/glacier_c04.hdr", "hdrmap/LightProbes/glacier_c05.hdr" };
	env = tm.CreateTextureCubeMap(e3, "pbr_envmap1");

	//test for cubemap empty
	//Texture* cubemap=tm.CreateTextureFromPreset("RT_CubeMap");
	//cubemap->name = "myempty";
	//int w, h;
	//mRenderSystem->GetWandH(w, h);
	//cubemap->width = w;
	//cubemap->height = h;
	//cubemap->createInternalRes();

	//RenderTarget* rt_cube = rt_mgr.CreateRenderTargetFromPreset("basic", "cube_test");
	//rt_cube->createInternalRes();
	//rt_cube->attachTextureCubeMapNow(cubemap, "color0", 0);
	//
	//rt_cube->attachTextureCubeMapNow(cubemap, "color0", 1);

	//rt_cube->attachTextureCubeMapNow(cubemap, "color0", 2);

	//rt_cube->attachTextureCubeMapNow(cubemap, "color0", 3);

	//rt_cube->attachTextureCubeMapNow(cubemap, "color0", 4);
}

void SSRayTraceEffect::Render()
{
	//auto& tm = TextureManager::getInstance();
	SceneManager* scene = SceneContainer::getInstance().get(input_scenename);
	auto camera = scene->getCamera(input_cameraname);
	if (needClipinfo){
		clipinfo = computeClipInfo(camera->getNear(), camera->getFar());
		needClipinfo = false;
	}

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);
	RenderQueue queue2;
	queue2.push_back(queue[0]);
	 
	auto p = raytrace_pass->mProgram;
	p->setProgramConstantData("clipInfo", &clipinfo[0], "vec3", sizeof(Vector3));
	p->setProgramConstantData("depth", in_depth);
	p->setProgramConstantData("color", in_color);
	p->setProgramConstantData("env_map_sampler", env);
	//raytrace_pass->mProgram->setSampler("Tex1", SamplerManager::getInstance().get("RepeatLinear"));
	mRenderSystem->RenderPass(camera, queue2, raytrace_pass, output_rt);
}

Vector3 SSRayTraceEffect::computeClipInfo(float zn, float zf)
{
	if (zf < -1e10) {
		return Vector3(zn, -1.0f, +1.0f);
	}
	else {
		return Vector3(zn  * zf, zn - zf, zf);
	}
}

void SSRayTraceTestPipeline::Init()
{
	//e_gbuffer.input_scenename = "scene1";
	//e_gbuffer.input_cameraname = "main";
	//e_gbuffer.Init();

	e_forward.input_scenename = "scene1";
	e_forward.input_cameraname = "main";
	e_forward.Init();

	e_ssraytrace.input_scenename = "scene1";
	e_ssraytrace.input_cameraname = "main";
	e_ssraytrace.in_color = e_forward.out_color;
	e_ssraytrace.in_depth = e_forward.out_depth;
	e_ssraytrace.Init();

	e_showtex.input_texture = e_ssraytrace.output_reflection;
	//e_showtex.input_texture = e_forward.out_color;
	e_showtex.Init();
}

void SSRayTraceTestPipeline::Render()
{
	e_forward.Render();
	e_ssraytrace.Render();
	e_showtex.Render();
}
