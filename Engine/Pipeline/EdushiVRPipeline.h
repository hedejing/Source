#pragma once
#include "effect.h"
#include "PreDefine.h"
#include <string>
#include "Pipeline.h"
#include "PBRPipeline.h"
#include "ShowTexturePipeline.h"
#include "FXAAEffect.h"
#include "GbufferPipeline.h"
#include "SSAOPipeline.h"
#include "ToneMappingPipeline.h"
#include "SimpleProfile.h"
#include "ShadowMapGenerateEffect.h"
#include "VarianceShadowMap.h"
#include "SkyBoxEffect.h"
#include "EGLTexture.h"
using namespace std;
using namespace HW;


class EdushiVRPipeline :public Pipeline {
public:
	virtual void Init();
	virtual void Render();

	//VR related functions
	void GetStereoTex(GLuint& l, GLuint& r);

	SSAOEffect2 ssao_left, ssao_right;
	ToneMappingEffect tonemap_left, tonemap_right;
	int w, h;

	float roughness = 0.8;
	float metalness = 0.04;


	int lightnum = 2;
	int envMapId = 2;
	int mode = 1;
	bool TonemapOn = true;
	void SetToneMap(bool f);
	Vector3 LightColor[2] = { Vector3(1,1,1),Vector3(0.5,0.5,0.7) };

	Vector3 LightPos[2] = { Vector3(5),Vector3(-100,100,-100) };
	SimpleProfile profile;
	VarianceShadowMapGenerateEffect shadowmap;
	LightCameraParam LightCamera;
	SkyBoxEffect skybox;
	Matrix4 projLeft, viewLeft, projRight, viewRight;
private:
	PBREffect2 pbr;
	Texture* env_diffuse[5], *env[5], *lut;
	string SceneName = "scene1";
	string CameraName = "main";
	as_Pass* ShadingPass;
	ShowTextureEffect showtex;
	FXAAEffect fxaa;
	GBufferEffect gbuffer_left, gbuffer_right;
	RenderSystem* mRenderSystem;
	RenderTarget* mainwindow, *rt_render_left, *rt_render_right, *rt_resolve_left, *rt_resolve_right;

	//VR related attr
	Texture* colorLeft, *colorRight;
};


