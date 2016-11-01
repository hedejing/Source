#include "SSSPipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "OPENGL_Include.h"
#include "Texture.h"
#include <sstream>
#include "EGLTexture.h"
using namespace std;
using namespace HW;


void SSSPipeline::Init()
{
	//loadPreset(0);
	LoadConfigs();
	auto& rt_mgr = RenderTargetManager::getInstance();

	input_scenename = "scene1";
	input_cameraname = "main";
	rt_render = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_sssMain");
	rt_render->createInternalRes();

	blur_rt1 = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_sss_blur1");

	blur_rt2 = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_sss_blur2");

	//Render_RT2 = rt_mgr.CreateRenderTargetFromPreset("basic_float", "effect_forward");
	//Render_RT2->createInternalRes();
	auto& tm = TextureManager::getInstance();


	color = tm.get("effect_sssMain_tex1");
	depth = tm.get("effect_sssMain_tex2");

	e_shadowmap.input_scenename = "scene1";
	e_shadowmap.input_cameraname = "main";
	e_shadowmap.LightNum = N_LIGHTS;
	e_shadowmap.Init();

	vsm.input_scenename = "scene1";
	vsm.input_cameraname = "main";
	vsm.LightNum = 1;
	vsm.LinearDepth = 0;
	vsm.blureffect.radius = 0.1;
	vsm.width = 2048;
	vsm.height = 2048;
	vsm.Init();

	lights[1].color = Vector3(0.0);
	lights[2].color = Vector3(0.0);
	//LightCamera[0].LightPosition = lights[2].positon;
	//LightCamera[0].LightDir = LightCenter-lights[2].positon;
	//LightCamera[0].Near = 0.1;
	//LightCamera[0].Far = 50;
	//LightCamera[0].width = 6.5;
	//LightCamera[0].height = 4.3;
	//LightCamera[0].perspective = true;
	//vsm.SetLightCamera(0, LightCamera[0]);

	mainWindow = rt_mgr.get("MainWindow");
	Render2_pass = PassManager::getInstance().LoadPass("sss/ssss_main_prog.json");

	Render2_pass->mProgram->UseProgram();
	SetLightUbo();


	SSSBlurPass = PassManager::getInstance().LoadPass("sss/SSSBlur_prog.json");

	showtex.input_texture = e_shadowmap.out_ShadowMapTexture[0];
	//showtex.input_texture = vsm.out_ShadowMapTexture[0];
	showtex.Init();
	lutSampler=SamplerManager::getInstance().get("ClampPoint");
	beckman = tm.CreateTextureFromImage("BeckmannMap.bmp");
	beckman->mipmap_mode = Texture::TextureMipmap_None;
	vector<string> e8 = { "SSSAsserts/St_d_c00.bmp", "SSSAsserts/St_d_c01.bmp", "SSSAsserts/St_d_c02.bmp",
		"SSSAsserts/St_d_c03.bmp", "SSSAsserts/St_d_c04.bmp", "SSSAsserts/St_d_c05.bmp" };
	vector<string> e7 = { "SSSAsserts/St_c00.bmp", "SSSAsserts/St_c01.bmp", "SSSAsserts/St_c02.bmp",
		"SSSAsserts/St_c03.bmp", "SSSAsserts/St_c04.bmp", "SSSAsserts/St_c05.bmp" };
	envmap = tm.CreateTextureCubeMap(e7, "irradiance_map");


	DepthStencilState s1 = DepthStencilState::getDefault();
	s1.mStencilEnable = true;
	s1.mStencilWriteMask = 0xFF;
	s1.mStencilRef = 1;
	s1.mFrontStencilPassFunc = STENCIL_OP_REPLACE;
	s1.mFrontStencilFunc = COMP_ALWAYS;
	Render2_pass->mDepthStencilState = s1;


	DepthStencilState s2 = DepthStencilState::getDefault();
	s2.mStencilEnable = true;
	s2.mStencilWriteMask = 0x00;
	s2.mStencilRef = 1;
	s2.mFrontStencilFunc = COMP_EQUAL;
	SSSBlurPass->mDepthStencilState = s2;

	for (int i = 0; i < 6; i++) {
		stringstream m;
		m << "model/head/huawei/" << i << ".jpg";
		back[i] = tm.CreateTextureFromImage(m.str());
	}
	ComposePass = PassManager::getInstance().LoadPass("sss/back_compose_prog.json");


}

void SSSPipeline::Render()
{
	RefreshConfigs();
	for (int i = 0; i < 2; i++) {
		e_shadowmap.SetLightCamera(e_shadowmap.cameras[i], lights[i].positon, lights[i].positon + lights[i].direction, sw[i], sw[i], lightnear, lightfar,true);
	}
	for (int i = 2; i < 3; i++) {
		vsm.SetLightCamera(vsm.cameras[i-2], lights[i].positon, lights[i].positon + lights[i].direction, sw[i], sw[i], lightnear, lightfar, true);
	}
	e_shadowmap.Render();
	vsm.Render();
	SceneManager* scene = SceneContainer::getInstance().get(input_scenename);
	auto camera = scene->getCamera(input_cameraname);
	RenderSystem* mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);

	//get a screen quad
	auto quadmesh = GlobalResourceManager::getInstance().as_meshManager.get("ScreenQuad.obj");
	RenderQueueItem item;
	item.asMesh = quadmesh;
	RenderQueue quad;
	quad.push_back(item);
	
	{
		auto p=Render2_pass->mProgram;
		p->setProgramConstantData("beckmannTex", beckman);
		p->setSampler("beckmannTex", lutSampler);
		for (int i = 0; i < 2; i++) {
			stringstream mapname,matname,viewname, lightposname,lightview,lightproj;
			mapname << "shadowMaps[" << i << "]";
			matname << "shadowMat[" << i << "]";
			p->setProgramConstantData(mapname.str(), e_shadowmap.out_ShadowMapTexture[i]);
			p->setProgramConstantData(matname.str(), &e_shadowmap.out_ShadowMatrix[i][0][0], "mat4", sizeof(Matrix4));

		}
		for (int i = 2; i < 3; i++) {
			stringstream mapname, matname, viewname, lightposname, lightview, lightproj;
			mapname << "shadowMaps[" << i << "]";
			matname << "shadowMat[" << i << "]";
			p->setProgramConstantData(mapname.str(), vsm.out_blur);
			p->setSampler(mapname.str(), SamplerManager::getInstance().get("ClampLinear"));
			p->setProgramConstantData(matname.str(), &vsm.out_ShadowMatrix[i-2][0][0], "mat4", sizeof(Matrix4));
		}
		p->setProgramConstantData("NormalMapEnabled", &normalenable, "int", sizeof(int));
		p->setProgramConstantData("irradianceTex", envmap);
		p->setProgramConstantData("bumpiness", &bumpiness, "float", sizeof(float));
		p->setProgramConstantData("specularIntensity", &specularIntensity, "float", sizeof(float));
		p->setProgramConstantData("specularRoughness", &specularRoughness, "float", sizeof(float));
		p->setProgramConstantData("specularFresnel", &specularFresnel, "float", sizeof(float));
		p->setProgramConstantData("translucency", &translucency, "float", sizeof(float));
		p->setProgramConstantData("sssEnabled", &sssEnabled, "int", sizeof(int));
		p->setProgramConstantData("sssWidth", &sssWidth, "float", sizeof(float));
		p->setProgramConstantData("ambient", &ambient, "float", sizeof(float));
		p->setProgramConstantData("n", &lightnear, "float", sizeof(float));
		p->setProgramConstantData("f", &lightfar, "float", sizeof(float));
		p->setProgramConstantData("ReduceBleeding", &vsm.ReduceBleeding, "float", sizeof(float));
		p->setProgramConstantData("mode", &mode, "int", sizeof(int));

		mRenderSystem->RenderPass(camera, queue, Render2_pass, rt_render);
	}

	
	Vector2 a(1.0, 0), b(0, 1.0);
	//first blur pass
	SSSBlurPass->mProgram->setProgramConstantData("colortex", color);
	SSSBlurPass->mProgram->setSampler("colortex", SamplerManager::getInstance().get("ClampPoint"));
	SSSBlurPass->mProgram->setProgramConstantData("depthtex", depth);
	SSSBlurPass->mProgram->setSampler("depthtex", SamplerManager::getInstance().get("ClampPoint"));
	SSSBlurPass->mProgram->setProgramConstantData("BlurDirection", &a, "vec2", sizeof(Vector2));
	SSSBlurPass->mProgram->setProgramConstantData("sssWidth", &sssWidth, "float", sizeof(float));
	mRenderSystem->RenderPass(NULL, quad, SSSBlurPass, blur_rt1);

	//blur_rt1->m_attach_textures["color0"]->SaveTextureToImage("s1", Texture::ST_FLOAT);
	//second blur pass
	SSSBlurPass->mProgram->setProgramConstantData("colortex", blur_rt1->m_attach_textures["color0"]);
	SSSBlurPass->mProgram->setProgramConstantData("BlurDirection", &b, "vec2", sizeof(Vector2));

	mRenderSystem->RenderPass(NULL, quad, SSSBlurPass, mainWindow);
	//if (!use_huawei_head) {
	//	mRenderSystem->RenderPass(NULL, quad, SSSBlurPass, mainWindow);
	//}
	//else {
	//	mRenderSystem->RenderPass(NULL, quad, SSSBlurPass, blur_rt2);
	//	ComposePass->mProgram->setProgramConstantData("back", back[head_index]);
	//	ComposePass->mProgram->setProgramConstantData("front", blur_rt2->m_attach_textures["color0"]);
	//	mRenderSystem->RenderPass(NULL, quad, ComposePass, mainWindow);
	//}

	//showtex.Render();
}
void InVector(ifstream& f, Vector3& v) {
	f >> v.x >> v.y >>v.z;
}
void SSSPipeline::LoadConfigs(int i)
{
	string config;
	if(i==0)
		config = "SSSConfig.txt";
	else
		config = "SSSConfig"+to_string(i)+".txt";
	ifstream ConfigFile(config);
	auto& f = ConfigFile;
	if (f.is_open()) {
		string name;
		f >> name >> bumpiness;
		f >> name >> specularIntensity;
		f >> name >> specularRoughness;
		f >> name >> specularFresnel;
		f >> name >> translucency;
		f >> name >> sssEnabled;
		f >> name >> sssWidth;
		f >> name >> ambient;
		f >> name >> environment;

		for (int i = 0; i < N_LIGHTS; i++) {
			int a;
			f >> name >> a;
			f >> name; InVector(f, lights[i].positon);
			f >> name; InVector(f, lights[i].direction);
			f >> name >> lights[i].falloffStart;
			f >> name >> lights[i].falloffWidth;
			f >> name; InVector(f, lights[i].color);
			f >> name >> lights[i].attenuation;
			f >> name >> lights[i].farPlane;
			f >> name >> lights[i].bias;
			lights[i].nearPlane = 0.1;
		}

	}
}


void SSSPipeline::SetLightUbo()
{
	//set light ubo

	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, N_LIGHTS * sizeof(Light_SSS), &lights, GL_DYNAMIC_DRAW);

	GLGpuProgram* p = static_cast<GLGpuProgram*>(Render2_pass->mProgram);
	int bind_index = 2;
	int ub0_index = glGetUniformBlockIndex(p->programID, "ub0");
	glUniformBlockBinding(p->programID, ub0_index, bind_index);
	glBindBufferBase(GL_UNIFORM_BUFFER, bind_index, ubo);
}

void SSSPipeline::RefreshConfigs()
{
	//LoadConfigs();
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, N_LIGHTS * sizeof(Light_SSS), &lights);
}

