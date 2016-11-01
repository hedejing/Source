#pragma once
#include "Pipeline.h"
#include "PreDefine.h"
#include "mathlib.h"
#include <vector>
#include "effect.h"
using namespace HW;
#include "ForwardPipeline.h"
#include "ShowTexturePipeline.h"


class ToneMappingEffect :public PipelineEffect{
public:
	virtual void Init();
	virtual void Render();


	//input
	Texture *input_color;
	float exposure=2;
	int ToneMapOn=1;
	//output
	Texture* out_tonemap;
	RenderTarget* output_rt = NULL;

private:
	as_Pass* tonemap_pass;
	RenderTarget* mainwindow;
	RenderSystem* mRenderSystem;
};

class ToneMappingPipeline :public Pipeline{
public:
	virtual void Init();
	virtual void Render();



	ToneMappingEffect effect_tone;
	ForwardEffect  effect_forward;
	ShowTextureEffect effect_show_tex;
};