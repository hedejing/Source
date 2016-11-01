#pragma once
#include "ShadowMapGenerateEffect.h"
#include "GaussianBlurEffect.h"
using namespace std;
using namespace HW;


class VarianceShadowMapGenerateEffect :public PipelineEffect {
public:
	virtual void Init();
	virtual void Render();

	void CalcShadowMat(Camera& camera, Matrix4& out_shadow_mat);
	void SetLightCamera(Camera& camera, Vector3 lightpos, Vector3 lookcenter, float width, float height, float near, float far, bool perspective = true);
	void SetLightCamera(int index, LightCameraParam& p);
	//input
	int width = 2048, height = 2048;
	string input_scenename;
	string input_cameraname;
	int LightNum = 5;
	vector<Camera> cameras;
	int LinearDepth = 1;
	float ReduceBleeding = 0.2;
	//output
	vector<Texture*> out_ShadowMapTexture;
	Texture* out_blur;
	vector<Matrix4> out_ShadowMatrix;
	GaussianBlurEffect blureffect;
private:
	
	void createShadowRT();
	vector<RenderTarget*> ShadowMapRT;
	Matrix4 scale_bias;
	as_Pass* shadowmap_pass;
	RenderSystem* mRenderSystem;

};
