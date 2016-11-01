#pragma once
#include "Pipeline.h"
#include "PreDefine.h"
#include "mathlib.h"
#include <vector>

#include "GbufferPipeline.h"
#include "ShowTexturePipeline.h"
using namespace HW;

class PBREffect :public PipelineEffect{
public:
	virtual void Init();
	virtual void Render();

	void changeSampleNum(int num);
	void createNoise();
	float uniform_1d_sample();


	void createSamples();
	void createggxTexture();
	void refreshGgxTexture();
	Vector3 sample_hemisphere_cosine(Vector2 u2d);

	Vector3 sample_hemisphere_ggx(Vector2 u2d, float alpha);
	void stratified_2d_samples(int nx, int ny, vector<Vector2> &samples);

	int num_samples_IBL = 32;
	int max_samplers_IBL = 128;
	vector<Vector3> samples_IBL_cosine;
	vector<Vector3> samples_IBL_ggx;
	//vector<uint16_t> 
	float metalness = 0.1;
	float roughness = 0.5;
	

	//input
	Texture* input_depth = NULL, *input_normal1 = NULL, *input_diffuse = NULL, *input_specular = NULL, *input_normal2 = NULL;
	bool isRendertoMainWindow=true;
	Texture* shadow_map = NULL, *ao_map = NULL;
	Matrix4 shadow_mat;
	Vector3 light_dir;
	//output
	RenderTarget* output_rt = NULL;
	Texture* color = NULL;
private:
	as_Pass* pbr_pass;
	RenderTarget* mainwindow;
	Texture* lut, *env, *tex_ggx, *tex_noise,*env_filtered;
	RenderSystem* mRenderSystem;

};
class PBRPipeline :public Pipeline{
public:
	virtual void Init();
	virtual void Render();

	PBREffect pbr_effect;
	GBufferEffect gbuffer_effect;
	
};

class PBREffect2 :public PipelineEffect {
public:
	virtual void Init();
	virtual void Render();

	void PrefilterEnvMapSaveImages(Texture* env,string path);
	void PrefilterEnvMapLatitude(Texture* env, string path);
	void LoadPrefilterEnvMap(Texture* env,string path);
	Vector3 base_color;

	//
	string in_scenename = "scene1";
	string in_cameraname = "main";
	RenderTarget* in_RT = NULL;


private:
	
	as_Pass* pbr_pass,*env_pass,*pbr_use_prefilter_pass;
	as_Pass *filter_env_pass, *integrateBRDF_pass;
	RenderTarget* mainwindow,*rt1;
	Texture* lut, *env, *tex_ggx, *tex_noise, *env_diffuse,*env_filterd;
	Texture* color, *depth;
	RenderSystem* mRenderSystem;

};

class PBRPipeline2 :public Pipeline {
public:
	virtual void Init();
	virtual void Render();

	PBREffect2 ePBr;
};