#pragma once
#include "Pipeline.h"
#include "PreDefine.h"
#include "mathlib.h"
#include <vector>
#include "effect.h"
#include "ForwardPipeline.h"
#include "ShowTexturePipeline.h"
using namespace HW;


class MLAAEffect :public PipelineEffect{
public:
	virtual void Init();
	virtual void Render();

	enum search_step {
		step_4,
		step_8,
		step_16,
		step_32,
		step_64
	};
	void testTex();
	//void setSearchStep(search_step);
	void setSearchStep(int s);
	//input
	Texture *input_texture,*input_depth;
	//output
	Texture* out_mlaa;
	Texture* out_weight, *out_edge;
	RenderTarget* output_rt = NULL;
	int num_step = 32;
private:
	
	int max_dist = 64;
	Texture* areamap, *areamap8, *areamap16, *areamap32, *areamap64, *areamap128;
	as_Pass* edge_detect_pass, *blend_weight_pass,*blend_pass;
	RenderTarget* mainwindow;
	RenderSystem* mRenderSystem;
};

class MLAAPipiline :public Pipeline{
public:
	virtual void Init();
	virtual void Render();

	ForwardEffect e_forward;
	MLAAEffect e_mlaa;
	ShowTextureEffect e_show_tex;

};