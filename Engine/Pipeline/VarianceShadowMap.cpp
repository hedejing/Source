#include "VarianceShadowMap.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "Camera.h"
#include "OPENGL_Include.h"
#include <sstream>

void VarianceShadowMapGenerateEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	auto& pm = PassManager::getInstance();

	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	Matrix4 bias(
		0.5, 0, 0, 0.5,
		0, 0.5, 0, 0.5,
		0, 0, 0.5, 0.5,
		0, 0, 0, 1);
	scale_bias = bias;

	createShadowRT();
	Camera c;
	for (int i = 0; i < LightNum; i++) {
		cameras.push_back(c);
	}
	shadowmap_pass = pm.LoadPass("shadowmap/VSM_prog.json");
	shadowmap_pass->UseInstance = true;
	blureffect.BlurFloatTexture = true;
	blureffect.radius = 1.0;
	blureffect.input_color = out_ShadowMapTexture[0];
	blureffect.Init();

	out_blur = blureffect.out_blur;

}
void VarianceShadowMapGenerateEffect::createShadowRT()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	ShadowMapRT = vector<RenderTarget*>(LightNum, NULL);
	out_ShadowMapTexture = vector<Texture*>(LightNum, NULL);
	out_ShadowMatrix = vector<Matrix4>(LightNum);
	for (int i = 0; i < LightNum; i++) {
		ShadowMapRT[i] = rt_mgr.CreateRenderTargetFromPreset("basic_float", "effect_vsm"+to_string(i));
		ShadowMapRT[i]->width = width;
		ShadowMapRT[i]->height = height;
		for (auto tex : ShadowMapRT[i]->m_attach_textures) {
			tex.second->width = width;
			tex.second->height = height;			
			tex.second->mipmap_mode = Texture::TextureMipmap_None;
		}
		out_ShadowMapTexture[i] = ShadowMapRT[i]->m_attach_textures["color0"];
		out_ShadowMapTexture[i]->setFormat(Texture::RG32F);
		ShadowMapRT[i]->createInternalRes();
		
	} 
	
}


void VarianceShadowMapGenerateEffect::Render()
{
	SceneManager* scene = SceneContainer::getInstance().get(input_scenename);
	auto MainCamera = scene->getCamera(input_cameraname);

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(MainCamera, queue);
	for (int i = 0; i < LightNum; i++) {
		Matrix4 view_light = cameras[i].getViewMatrix();
		Matrix4 proj_light = cameras[i].getProjectionMatrixDXRH();
		shadowmap_pass->mProgram->setProgramConstantData("proj_light", &proj_light[0][0], "mat4", sizeof(Matrix4));
		shadowmap_pass->mProgram->setProgramConstantData("view_light", &view_light[0][0], "mat4", sizeof(Matrix4));
		shadowmap_pass->mProgram->setProgramConstantData("LinearDepth", &LinearDepth, "int", sizeof(int));

		mRenderSystem->RenderPass(NULL, queue, shadowmap_pass, ShadowMapRT[i]);
		//out_ShadowMapTexture[i]->GenerateMipmap();
		out_ShadowMatrix[i] = scale_bias*proj_light * view_light;
	}
	blureffect.Render();
}



void VarianceShadowMapGenerateEffect::CalcShadowMat(Camera& camera, Matrix4& out_shadow_mat)
{
	auto view_light = camera.getViewMatrix();
	auto proj_light = camera.getProjectionMatrixDXRH();
	out_shadow_mat = scale_bias * proj_light * view_light;
}

void VarianceShadowMapGenerateEffect::SetLightCamera(Camera& camera, Vector3 lightpos, Vector3 lookcenter, float width, float height, float tnear, float tfar, bool perspective)
{
	Vector3 up(0, 1, 0);
	if (up == lookcenter)
		up = Vector3(1, 0, 0);
	camera.lookAt(lightpos, lookcenter, up);
	camera.setFrustum(-width / 2, width / 2, -height / 2, height / 2, tnear, tfar);
	if (perspective)
		camera.setProjectionType(Camera::PT_PERSPECTIVE);
	else
		camera.setProjectionType(Camera::PT_ORTHOGONAL);
}

void VarianceShadowMapGenerateEffect::SetLightCamera(int index, LightCameraParam& p)
{
	SetLightCamera(cameras[index], p.LightPosition, p.LightPosition + p.LightDir, p.width, p.height, p.Near, p.Far, p.perspective);
}
