#pragma once
#include "Pipeline.h"
#include "PreDefine.h"
#include "mathlib.h"
#include <vector>
#include "effect.h"
#include "GbufferPipeline.h"
#include "ShowTexturePipeline.h"
using namespace HW;
class SSAOEffect :public PipelineEffect{
public:
	virtual void Init();
	virtual void Render();

	void createSample();
	void createNoise();
	void setSampleNum(int num);
	float uniform_1d_sample();
	void stratified_1d_samples(int n, std::vector<float> &samples);
	void stratified_2d_samples(int nx, int ny, vector<Vector2> &samples);
	float radicalInverse_VdC(unsigned bits);
	Vector2 hammersley2d(unsigned i, unsigned N);
	Vector3 sample_hemisphere_uniform(Vector2 u2d);
	int num_samples_ssao = 32;
	vector<Vector3> samples_ssao;

	//param
	float radius=0.1;


	//input
	Texture *depth, *normal;
	bool isRendertoMainWindow = true;

	//output
	Texture* output_ao;
	RenderTarget* output_rt = NULL;
private:
	Texture* noise_tex;
	RenderTarget *mainwindow;
	RenderSystem* mRenderSystem;
};

class SSAOEffect2 :public PipelineEffect {
public:
	virtual void Init();
	virtual void Render();

	void createSample();
	void createNoise();
	void setSampleNum(int num);

	int num_samples_ssao = 32;
	vector<Vector3> samples_ssao;
	bool halfResolution = false;
	int w, h;

	//param
	float radius = 0.1;
	//input
	Texture *depth, *normal;
	bool isRendertoMainWindow = true;

	float Near = 0.1;
	float Far = 10;

	//output
	Texture* output_ao,*raw_ao;
	RenderTarget* output_rt = NULL;



private:
	Texture* noise_tex;
	RenderTarget *mainwindow,*rt_raw_ao;
	RenderSystem* mRenderSystem;
	as_Pass* ssao_pass,*blur_pass;
};


class SSAOPipeline :public Pipeline{
public:
	virtual void Init();
	virtual void Render();

	GBufferEffect effect_gbuf;
	SSAOEffect effect_ao;
	ShowTextureEffect effect_show_tex;
};