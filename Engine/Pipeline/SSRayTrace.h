#pragma once
#include "Pipeline.h"
#include "PreDefine.h"
#include "effect.h"
#include "mathlib.h"
#include <vector>
#include "RenderItem.h"
#include "GbufferPipeline.h"
#include "ForwardPipeline.h"
#include "ShowTexturePipeline.h"
using namespace HW;

class SSRayTraceEffect :public PipelineEffect{
public:

	//输入:camera和需要进行光线追踪的物体列表，以及之前的场景颜色和深度
	string input_scenename;
	string input_cameraname;
	RenderQueue* in_raytrace_objects;
	Texture* in_color, *in_depth;
	//输出：反射结果texture

	Texture* output_reflection;
	RenderTarget* output_rt = NULL;

	virtual void Init();
	virtual void Render();
private:
	as_Pass* raytrace_pass;
	RenderSystem* mRenderSystem;
	bool needClipinfo=true;
	Texture* env;
	Vector3 clipinfo;
	Vector3 computeClipInfo(float zn, float zf);

};

class SSRayTraceTestPipeline :public Pipeline{
public:
	virtual void Init();
	virtual void Render();


	ForwardEffect e_forward;
	GBufferEffect e_gbuffer;
	SSRayTraceEffect e_ssraytrace;
	ShowTextureEffect e_showtex;

};
