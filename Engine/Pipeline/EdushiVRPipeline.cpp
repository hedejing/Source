#include "EdushiVRPipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "EngineUtil.h"
#include "EGLRenderTarget.h"
#include "EGLUtil.h"
#include "Camera.h"
#include "GlobalVariablePool.h"

void EdushiVRPipeline::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	mainwindow = rt_mgr.get("MainWindow");
	w = mainwindow->width, h = mainwindow->height;
	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;
	rt_render_left = rt_mgr.CreateRenderTargetFromPreset("basic_float", "rt_edushi_render_left");
	for (auto& x : rt_render_left->m_attach_textures) {
		x.second->MSAASampleNum = 4;
	}
	rt_render_right = rt_mgr.CreateRenderTargetFromPreset("basic_float", "rt_edushi_render_right");
	for (auto& x : rt_render_right->m_attach_textures) {
		x.second->MSAASampleNum = 4;
	}
	rt_resolve_left = rt_mgr.CreateRenderTargetFromPreset("basic_float", "edushi_msaa_resolve_left");
	rt_resolve_left->createInternalRes();
	rt_render_left->createInternalRes();

	rt_resolve_right = rt_mgr.CreateRenderTargetFromPreset("basic_float", "edushi_msaa_resolve_right");
	rt_resolve_right->createInternalRes();
	rt_render_right->createInternalRes();

	lut = tm.CreateTextureFromImage("convolution_spec.tif", "pbr_lut");
	lut->mipmap_mode = Texture::TextureMipmap_None;
	ShadingPass = PassManager::getInstance().LoadPass("edushi/BasicShading_prog.json");

	auto e_pisa = GenCubemapList("hdrmap/LightProbes/pisa_c0", ".hdr");
	auto e_pisa_diffuse = GenCubemapList("hdrmap/diffuse/pisa_d_c0", ".hdr");
	auto e_uffizi = GenCubemapList("hdrmap/LightProbes/uffizi_c0", ".hdr");
	auto e_uffizi_diffuse = GenCubemapList("hdrmap/diffuse/uffizi_d_c0", ".hdr");

	//// 对一个 cubemap 进行 ggx filter 操作
	//pbr.PrefilterEnvMapSaveImages(env, "default_asserts/hdrmap/ggx_filtered/pisa");
	//pbr.LoadPrefilterEnvMap(env, "default_asserts/hdrmap/ggx_filtered/pisa");
	skybox.rt_result = rt_render_left;
	skybox.afterPass = ShadingPass;
	skybox.Init();

	env[0] = tm.CreateTextureCubeMap(e_pisa, "pisa");
	env_diffuse[0] = tm.CreateTextureCubeMap(e_pisa_diffuse, "pbr_env_diffuse_pisa");
	pbr.LoadPrefilterEnvMap(env[0], "default_asserts/hdrmap/ggx_filtered/pisa");

	env[1] = tm.CreateTextureCubeMap(e_uffizi, "uffizi");
	env_diffuse[1] = tm.CreateTextureCubeMap(e_uffizi_diffuse, "pbr_env_diffuse_uffizi");
	pbr.LoadPrefilterEnvMap(env[1], "default_asserts/hdrmap/ggx_filtered/uffizi");

	env[2] = skybox.env_map;
	auto e_sky = GenCubemapList("skybox/diffuse/s_mip4_face", ".hdr");
	env_diffuse[2] = tm.CreateTextureCubeMap(e_sky, "sky0");
	pbr.LoadPrefilterEnvMap(env[2], "default_asserts/skybox/diffuse/s");
	
	shadowmap.input_scenename = SceneName;
	shadowmap.input_cameraname = CameraName;
	shadowmap.LightNum = 1;
	shadowmap.LinearDepth = 1;
	shadowmap.blureffect.radius = 1.2;
	shadowmap.width = 2048;
	shadowmap.height = 2048;
	shadowmap.ReduceBleeding = 0.5;
	shadowmap.Init();

	LightCamera.LightPosition = LightPos[0];
	LightCamera.LightDir = -LightPos[0];
	LightCamera.Near = 1;
	LightCamera.Far = 15;
	LightCamera.width = 6;
	LightCamera.height = 4;
	LightCamera.perspective = false;
	LightCamera.bias = -0.001;
	shadowmap.SetLightCamera(0, LightCamera);

	fxaa.input_color = rt_render_left->m_attach_textures["color0"];
	fxaa.Init();

	gbuffer_left.input_scenename = SceneName;
	gbuffer_left.input_cameraname = CameraName;
	gbuffer_left.Init();

	gbuffer_right.input_scenename = SceneName;
	gbuffer_right.input_cameraname = CameraName;
	gbuffer_right.Init();

	ssao_left.depth = gbuffer_left.depth;
	ssao_left.normal = gbuffer_left.normal2;
	ssao_left.radius = 0.17;
	ssao_left.num_samples_ssao = 48;
	ssao_left.halfResolution = false;
	ssao_left.Init();

	ssao_right.depth = gbuffer_right.depth;
	ssao_right.normal = gbuffer_right.normal2;
	ssao_right.radius = 0.17;
	ssao_right.num_samples_ssao = 48;
	ssao_right.halfResolution = false;
	ssao_right.Init();

	tonemap_left.input_color = rt_resolve_left->m_attach_textures["color0"];
	tonemap_left.exposure = 1;
	tonemap_left.Init();

	tonemap_right.input_color = rt_resolve_right->m_attach_textures["color0"];
	tonemap_right.exposure = 1;
	tonemap_right.Init();

	showtex.input_texture = tonemap_left.out_tonemap;
	showtex.Init();
}

void EdushiVRPipeline::Render()
{
	profile.NewFrame();
	SceneManager* scene = SceneContainer::getInstance().get(SceneName);
	auto camera = scene->getCamera(CameraName);
	if (skybox.camera == NULL)
		skybox.camera = camera;

	LightCamera.LightPosition = LightPos[0];
	LightCamera.LightDir = -LightPos[0];
	shadowmap.SetLightCamera(0, LightCamera);

	//**********************************************************************************************************************
	//************************						LEFT										 ***************************
	//**********************************************************************************************************************
	GlobalVariablePool* gp = GlobalVariablePool::GetSingletonPtr();
	gp->SetProjectMatrix(projLeft);
	gp->SetViewMatrix(viewLeft);

	shadowmap.Render();
	profile.Tick("Shadow");
	////MSAA render scene and resolve to usual texture
	gbuffer_left.Render();
	//gbuffer_left.Render();
	profile.Tick("Gbuffer");

	ssao_left.Near = camera->getNear();
	ssao_left.Far = camera->getFar();
	ssao_left.Render();
	profile.Tick("SSAO");

	//skybox.Render();

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);

	auto p = ShadingPass->mProgram;
	p->setProgramConstantData("env_map", env[envMapId]);
	p->setSampler("env_map", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("ao_map", ssao_left.output_ao);
	p->setSampler("ao_map", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("env_map_diffuse", env_diffuse[envMapId]);
	p->setSampler("env_map_filtered", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("IntegrateBRDFSampler", lut);
	p->setSampler("IntegrateBRDFSampler", SamplerManager::getInstance().get("ClampPoint"));
	p->setProgramConstantData("w", &w, "int", sizeof(int));
	p->setProgramConstantData("h", &h, "int", sizeof(int));
	p->setProgramConstantData("mode", &mode, "int", sizeof(int));
	p->setProgramConstantData("roughness", &roughness, "float", sizeof(float));
	p->setProgramConstantData("metalness", &metalness, "float", sizeof(float));
	p->setProgramConstantData("LightColor", &LightColor[0][0], "vec3", sizeof(Vector3)*lightnum);
	p->setProgramConstantData("LightPos", &LightPos[0][0], "vec3", sizeof(Vector3)*lightnum);
	p->setProgramConstantData("shadowMaps[0]", shadowmap.out_blur);
	p->setSampler("shadowMaps[0]", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("shadowMat[0]", &shadowmap.out_ShadowMatrix[0][0][0], "mat4", sizeof(Matrix4));
	p->setProgramConstantData("bias", &LightCamera.bias, "float", sizeof(float));

	p->setProgramConstantData("ReduceBleeding", &shadowmap.ReduceBleeding, "float", sizeof(float));
	ShadingPass->UseInstance = true;
	// render to a buffer
	mRenderSystem->RenderPass(NULL, queue, ShadingPass, rt_render_left);

	profile.Tick("Shading");
	rt_render_left->BlitTexture(rt_resolve_left, "color0", "color0");
	rt_render_left->BlitTexture(rt_resolve_left, "depth", "depth");
	profile.Tick("Msaa resolve");

	tonemap_left.Render();

	//**********************************************************************************************************************
	//************************						right										 ***************************
	//**********************************************************************************************************************
	gp->SetProjectMatrix(projRight);
	gp->SetViewMatrix(viewRight);

	profile.Tick("Shadow");
	////MSAA render scene and resolve to usual texture
	gbuffer_right.Render();
	profile.Tick("Gbuffer");

	ssao_right.Near = camera->getNear();
	ssao_right.Far = camera->getFar();
	ssao_right.Render();
	profile.Tick("SSAO");

	//skybox.Render();

	p->setProgramConstantData("env_map", env[envMapId]);
	p->setSampler("env_map", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("ao_map", ssao_right.output_ao);
	p->setSampler("ao_map", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("env_map_diffuse", env_diffuse[envMapId]);
	p->setSampler("env_map_filtered", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("IntegrateBRDFSampler", lut);
	p->setSampler("IntegrateBRDFSampler", SamplerManager::getInstance().get("ClampPoint"));
	p->setProgramConstantData("w", &w, "int", sizeof(int));
	p->setProgramConstantData("h", &h, "int", sizeof(int));
	p->setProgramConstantData("mode", &mode, "int", sizeof(int));
	p->setProgramConstantData("roughness", &roughness, "float", sizeof(float));
	p->setProgramConstantData("metalness", &metalness, "float", sizeof(float));
	p->setProgramConstantData("LightColor", &LightColor[0][0], "vec3", sizeof(Vector3)*lightnum);
	p->setProgramConstantData("LightPos", &LightPos[0][0], "vec3", sizeof(Vector3)*lightnum);
	p->setProgramConstantData("shadowMaps[0]", shadowmap.out_blur);
	p->setSampler("shadowMaps[0]", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("shadowMat[0]", &shadowmap.out_ShadowMatrix[0][0][0], "mat4", sizeof(Matrix4));
	p->setProgramConstantData("bias", &LightCamera.bias, "float", sizeof(float));

	p->setProgramConstantData("ReduceBleeding", &shadowmap.ReduceBleeding, "float", sizeof(float));
	ShadingPass->UseInstance = true;
	// render to a buffer
	mRenderSystem->RenderPass(NULL, queue, ShadingPass, rt_render_right);

	profile.Tick("Shading");
	rt_render_right->BlitTexture(rt_resolve_right, "color0", "color0");
	rt_render_right->BlitTexture(rt_resolve_right, "depth", "depth");
	profile.Tick("Msaa resolve");

	tonemap_right.Render();

	//**********************************************************************************************************************
	//************************						showtex										 ***************************
	//**********************************************************************************************************************
	profile.Tick("Tonemap");
	showtex.Render();
	profile.Tick("ShowTex");

}

void EdushiVRPipeline::SetToneMap(bool f)
{
	if (f) {
		showtex.input_texture = tonemap_left.out_tonemap;
	}
	else
		showtex.input_texture = rt_resolve_left->m_attach_textures["color0"];
}

void EdushiVRPipeline::GetStereoTex(GLuint& l, GLuint& r)
{
	l = ((EGLTexture*)(tonemap_left.out_tonemap))->texture_id;
	r = ((EGLTexture*)(tonemap_right.out_tonemap))->texture_id;
}