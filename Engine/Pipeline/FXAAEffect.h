#pragma once
#include "effect.h"
#include "PreDefine.h"
using namespace HW;

class FXAAEffect :public PipelineEffect{
public:
	virtual void Init();
	virtual void Render();

	//input
	Texture *input_color;
	//output
	Texture* out_fxaa;
	RenderTarget* output_rt = NULL;
private:
	as_Pass* fxaa_pass;
	RenderSystem* mRenderSystem;
};