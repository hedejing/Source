#pragma once

#include "Pipeline.h"
#include "PreDefine.h"
#include "ForwardPipeline.h"
#include "GbufferPipeline.h"
#include "PBRPipeline.h"
#include "MLAAPipeline.h"
#include "ToneMappingPipeline.h"
#include "ShowTexturePipeline.h"
#include "TextureManager.h"
#include "FXAAEffect.h"
#include "ShadowMapGenerateEffect.h"
#include "SSAOPipeline.h"
#include "RenderTextPipeline.h"

class TestPipeline :public Pipeline{
public:
	virtual void Init();
	virtual void Render();

	void setParamTest();
	void reLink();
	void DrawParam();
	ForwardEffect e_forward;
	GBufferEffect e_gbuffer;
	PBREffect e_pbr;
	MLAAEffect e_mlaa;
	SSAOEffect e_ssao;
	ToneMappingEffect e_tonemap;
	ShowTextureEffect e_showtex;
	FXAAEffect e_fxaa;
	ShadowMapGenerateEffect e_shadowmap;

	RenderTextPipeline e_RenderText;
};
