#pragma once
#include "effect.h"
#include "PreDefine.h"
#include <string>
using namespace std;
using namespace HW;


class GBufferEffect :public PipelineEffect{
public:
	virtual void Init();
	virtual void Render();

	//input
	string input_scenename;
	string input_cameraname;
	int MSAASampleNum = 1;
	//output
	RenderTarget* output_gbufferRT;
	Texture* depth,*normal1,*diffuse,*normal2,*specular;

private:
	as_Pass* gbuffer_pass;
};