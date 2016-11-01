#pragma once
#include "effect.h"
#include "PreDefine.h"
using namespace HW;

class SkyBoxEffect :public PipelineEffect{
public:
	virtual void Init();
	virtual void Render();

	//input
	Texture *input_depth,*input_color;
	int isHdrSky = 0;
	float boxsize = 100;
	Camera* camera = NULL;
	Texture* env_map = NULL;
	float w, h;
	//output
	Texture* out_skybox;
	

	//state 2
	bool state2 = true;
	RenderTarget* rt_result=NULL;
	as_Pass* afterPass;
private:

	RenderTarget* rt_sky = NULL, *mainwindow;
	as_Pass* skybox_pass;
	RenderSystem* mRenderSystem;
};