#include "EdushiPipeline.h"
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

void EdushiPipeline::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	mainwindow = rt_mgr.get("MainWindow");
	w = mainwindow->width, h = mainwindow->height;
	mRenderSystem= GlobalResourceManager::getInstance().m_RenderSystem;
	rt_render=rt_mgr.CreateRenderTargetFromPreset("basic_float", "rt_edushi_render");
	for (auto& x : rt_render->m_attach_textures) {
		x.second->MSAASampleNum = 4;
	}
	rt_resolve = rt_mgr.CreateRenderTargetFromPreset("basic_float", "edushi_msaa_resolve");
	rt_resolve->createInternalRes();
	rt_render->createInternalRes();


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
	skybox.rt_result = rt_render;
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
	env_diffuse[2]= tm.CreateTextureCubeMap(e_sky, "sky0");
	pbr.LoadPrefilterEnvMap(env[2], "default_asserts/skybox/diffuse/s");
	//pbr.Init();
	
	




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

	fxaa.input_color= rt_render->m_attach_textures["color0"];
	fxaa.Init();

	gbuffer.input_scenename = SceneName;
	gbuffer.input_cameraname = CameraName;
	gbuffer.Init();


	ssao.depth = gbuffer.depth;
	ssao.normal = gbuffer.normal2;
	ssao.radius = 0.17;
	ssao.num_samples_ssao = 48;
	ssao.halfResolution = false;
	ssao.Init();

	//showtex.input_texture = rt_gbuffer->m_attach_textures["color0"];
	//showtex.input_texture = gbuffer.diffuse;
	//Texture* test = tm.CreateTextureFromImage("default_asserts/hdrmap/LightProbes/pisa_c01.hdr");


	
	
	//skybox.input_color= rt_resolve->m_attach_textures["color0"];
	//skybox.input_depth=rt_resolve->m_attach_textures["depth"];

	
	//pbr.PrefilterEnvMapSaveImages(skybox.env_map, "default_asserts/skybox/diffuse/s");
	tonemap.input_color = rt_resolve->m_attach_textures["color0"];
	//tonemap.input_color = skybox.out_skybox;
	tonemap.exposure = 1;
	tonemap.Init();


	showtex.input_texture = ssao.output_ao;
	showtex.input_texture = rt_resolve->m_attach_textures["color0"];
	showtex.input_texture = tonemap.out_tonemap;
	//showtex.input_texture = shadowmap.out_ShadowMapTexture[0];
	showtex.Init();
}

void EdushiPipeline::Render()
{
	profile.NewFrame();
	SceneManager* scene = SceneContainer::getInstance().get(SceneName);
	auto camera = scene->getCamera(CameraName);
	if(skybox.camera==NULL)
		skybox.camera = camera;

	LightCamera.LightPosition = LightPos[0];
	LightCamera.LightDir = -LightPos[0];
	shadowmap.SetLightCamera(0, LightCamera);
	shadowmap.Render();
	profile.Tick("Shadow");
	////MSAA render scene and resolve to usual texture
	gbuffer.Render(); 
	//gbuffer.output_gbufferRT->BlitTexture(rt_gbuffer, "color0", "color0");
	//gbuffer.output_gbufferRT->BlitTexture(rt_gbuffer, "color3", "color3");
	//gbuffer.output_gbufferRT->BlitTexture(rt_gbuffer, "depth", "depth");
	profile.Tick("Gbuffer");

	ssao.Near = camera->getNear();
	ssao.Far = camera->getFar();
	ssao.Render(); 
	profile.Tick("SSAO");
	

	//skybox.Render();


	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);
	
	auto p = ShadingPass->mProgram;
	p->setProgramConstantData("env_map", env[envMapId]);
	p->setSampler("env_map", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("ao_map", ssao.output_ao);
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
	mRenderSystem->RenderPass(camera, queue, ShadingPass, rt_render); 
	
	profile.Tick("Shading");
	rt_render->BlitTexture(rt_resolve, "color0", "color0"); 
	rt_render->BlitTexture(rt_resolve, "depth", "depth");
	profile.Tick("Msaa resolve");

	

	tonemap.Render(); 

	profile.Tick("Tonemap");
	showtex.Render();
	profile.Tick("ShowTex");

}

void EdushiPipeline::SetToneMap(bool f)
{
	if (f) {
		showtex.input_texture = tonemap.out_tonemap;
	}
	else
		showtex.input_texture = rt_resolve->m_attach_textures["color0"];
}

//
////{
//int w = mainwindow->getWidth(), h = mainwindow->getHeight();
//EGLRenderTarget* rt1 = (EGLRenderTarget*)(gbuffer.output_gbufferRT);
////	EGLRenderTarget* rt2 = (EGLRenderTarget*)(rt_gbuffer);
////	EGLRenderTarget* rt3 = (EGLRenderTarget*)(rt_render);
////	
//glBindFramebuffer(GL_READ_FRAMEBUFFER, rt1->m_framebuffer_id);
////	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rt2->m_framebuffer_id);
////	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//glReadBuffer(GL_COLOR_ATTACHMENT0);
////	glDrawBuffer(GL_COLOR_ATTACHMENT0);
////	glBlitFramebuffer(0, 0,w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//
////	glReadBuffer(GL_COLOR_ATTACHMENT3);
//	glDrawBuffer(GL_COLOR_ATTACHMENT3);
//	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//	GL_CHECK();
//}
//auto tex1 = gbuffer.output_gbufferRT->m_attach_textures["color0"];
//EGLTexture* tex2 =(EGLTexture*) rt_gbuffer->m_attach_textures["color0"];
//glBindFramebuffer(GL_READ_FRAMEBUFFER, rt1->m_framebuffer_id);
//glReadBuffer(GL_COLOR_ATTACHMENT0);
//tex2->useTexture();
//glCopyTexSubImage2D(tex2->texture_target, 0, 0, 0, 0, 0, w, h);