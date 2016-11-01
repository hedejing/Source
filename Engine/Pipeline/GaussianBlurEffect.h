#pragma once
#include "effect.h"
#include "PreDefine.h"
using namespace HW;

class GaussianBlurEffect :public PipelineEffect{
public:
	virtual void Init();
	virtual void Render();

	//input
	Texture *input_color;
	bool BlurFloatTexture = false;
	float radius = 1;
	//output
	Texture* out_blur;
	
private:

	RenderTarget* blur1=NULL,*blur2 = NULL;
	Texture* blur1_texture;
	as_Pass* blur_pass;
	RenderSystem* mRenderSystem;
};