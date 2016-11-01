#pragma once
#include "effect.h"
#include "PreDefine.h"
#include <string>
#include "Pipeline.h"
#include "ShowTexturePipeline.h"
#include "ShadowMapGenerateEffect.h"
#include "VarianceShadowMap.h"
#include "mathlib.h"
using namespace std;
using namespace HW;

#define N_LIGHTS 5
#define N_ShadowMap 2
#define N_VSM 2
class camera_SSS {
public:
	float distance;
	Vector2 angle, panPosition, angularVelocity;
	Vector3 eyePosition, lookAtPosition,direction;

	friend std::istream& operator >>(std::istream &is, camera_SSS &camera) {
		is >> camera.distance;
		is >> camera.angle.x >> camera.angle.y;
		is >> camera.panPosition.x >> camera.panPosition.y;
		is >> camera.angularVelocity.x >> camera.angularVelocity.y;
		return is;
	}
};

struct Light_SSS {
	Vector3 positon; float nearPlane;
	Vector3 direction; float pad1;
	Vector3 color;float falloffStart;
	float falloffWidth;
	float attenuation;
	float farPlane;
	float bias;
};
class SSSEffect :public PipelineEffect {
public:
	virtual void Init(){}
	virtual void Render(){}

private:
	as_Pass *SSSBlurPass1, *SSSBlurPass2;
};

class SSSPipeline :public Pipeline {
public:
public:
	virtual void Init();
	virtual void Render();

	void LoadConfigs(int i=0);
	void SetLightUbo();
	void RefreshConfigs();
	//void loadPreset(int i);

	//input
	string input_scenename;
	string input_cameraname;

	//output
	RenderTarget* Render_RT;
	Texture* depth, *color;

	
//SSS param
public:
	VarianceShadowMapGenerateEffect vsm;
	ShadowMapGenerateEffect e_shadowmap;
	int mode = 0;
	vector<float> sw = { 0.2f,0.2f,0.2f };
	float bumpiness = 0.9;
	float specularIntensity = 1.88;
	float specularRoughness = 0.3;
	float specularFresnel = 0.82;
	float translucency = 0.83;
	int sssEnabled = 1;
	float sssWidth = 0.012;
	float ambient = 0.05;
	float environment = 0.55;
	int normalenable = 1;
	float lightnear = 0.06, lightfar = 100;
	Light_SSS lights[N_LIGHTS];
	Vector3 LightCenter=Vector3(0.0);
	bool use_huawei_head=false;
	int head_index = 0;
private:
	Sampler* lutSampler;
	uint32_t ubo;
	Texture* beckman,*envmap;
	RenderTarget* rt_render,*blur_rt1,*blur_rt2;
	RenderTarget* mainWindow;
	SSSEffect sss;
	
	ShowTextureEffect showtex;
	as_Pass* render_pass,*Render2_pass;
	as_Pass* SSSBlurPass;
	as_Pass* ComposePass;
	Texture* back[6];
};
