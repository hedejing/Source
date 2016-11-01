#pragma once
#include "effect.h"
#include "PreDefine.h"
#include <string>
#include "Pipeline.h"
#include "mathlib.h"
#include "ForwardPipeline.h"
#include "ShowTexturePipeline.h"
using namespace std;
using namespace HW;



class TemporalAAPipeline :public Pipeline {
public:
	enum class JitterModes
	{
		None = 0,
		Uniform2x = 1,
		Hammersley4x = 2,
		Hammersley8x = 3,
		Hammersley16x = 4,

		NumValues
	};

	virtual void Init();
	virtual void Render();

	string input_scenename, input_cameraname;
	Vector3 LightPosition = Vector3(1.0, 20.0, 30.0);
	Vector3 LightColor = Vector3(1, 1, 1);
	JitterModes jittermode= JitterModes::Hammersley4x;
	int EnableTemporalAA = 1;
	float jittlescale= 1.0;

	
	uint64_t frameCount=0;
	int w, h;


	RenderTarget* rt_history, *rt_current, *mainwindow,*rt_mix;
	Texture* current_tex, *history_tex,*mix_tex,*velocity_tex,*depth_tex;
	Sampler* linearSampler;
	as_Pass* forward_pass, *velocity_pass,*taa_pass;
	ForwardEffect eforward;
	ShowTextureEffect eShowtex;
private:
	void SwapBuffer();
	Vector2 jitterOffset, prevJitter;
	Matrix4 PrevProjection = Matrix4::IDENTITY;
	Matrix4 offsetMatrix = Matrix4::IDENTITY;
	Vector2 CurrentJitter=Vector2(0,0);
	Matrix4 PrevView = Matrix4::IDENTITY;
	void UpdateJitter();
	float RadicalInverseBase2(unsigned int bits);

	Vector2 Hammersley2D(uint64_t sampleIdx, uint64_t numSamples);

};