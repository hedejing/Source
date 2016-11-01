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

	//����:camera����Ҫ���й���׷�ٵ������б��Լ�֮ǰ�ĳ�����ɫ�����
	string input_scenename;
	string input_cameraname;
	RenderQueue* in_raytrace_objects;
	Texture* in_color, *in_depth;
	//�����������texture

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
