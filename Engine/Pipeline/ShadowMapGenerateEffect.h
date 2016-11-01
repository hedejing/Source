#pragma once
#include "effect.h"
#include "PreDefine.h"
#include <string>
#include "mathlib.h"
#include <vector>
#include "ShowTexturePipeline.h"
#include "Pipeline.h"
#include "Camera.h"
using namespace std;
using namespace HW;


struct LightCameraParam {
	Vector3 LightPosition;
	Vector3 LightDir;
	float Near;
	float Far;
	float width;
	float height;
	float bias = 0;
	bool perspective;
};

class ShadowMapGenerateEffect :public PipelineEffect {
public:
	virtual void Init();
	virtual void Render();

	void CalcShadowMat(Camera& camera,Matrix4& out_shadow_mat);
	void SetLightCamera(Camera& camera, Vector3 lightpos,Vector3 lookcenter,float width, float height,float near,float far, bool perspective = true);
	void SetLightCamera(int index, LightCameraParam& p);
	//input
	int width = 2048, height = 2048;
	string input_scenename;
	string input_cameraname;
	int LightNum = 5;
	vector<Camera> cameras;
	int LinearDepth = 1;
	//output
	vector<Texture*> out_ShadowMapTexture;
	vector<Texture*> out_LightPosTexture;
	vector<Matrix4> out_ShadowMatrix;
	vector<Matrix4> out_LightViewMatrix;
private:

	void createShadowRT();
	
	vector<RenderTarget*> ShadowMapRT;
	Matrix4 scale_bias;
	as_Pass* shadowmap_pass;
	RenderSystem* mRenderSystem;

};

class ShadowMapTest :public Pipeline {
public:
	virtual void Init();
	virtual void Render();


	ShowTextureEffect e_showtex;
	ShadowMapGenerateEffect e_shadowmap;

	
	//input
	string input_scenename;
	string input_cameraname;

private:
	as_Pass* shadowmapDraw_pass;
	RenderSystem* mRenderSystem;
	RenderTarget* mainwindow;

};