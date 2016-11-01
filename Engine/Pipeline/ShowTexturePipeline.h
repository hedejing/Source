#pragma once
#include "effect.h"
#include "PreDefine.h"
#include <string>
using namespace std;
using namespace HW;


class ShowTextureEffect :public PipelineEffect{
public:
	virtual void Init();
	virtual void Render();

	//input
	Texture* input_texture=NULL;
	Texture* input_texture_cube=NULL;

	as_Pass* quad_pass;
private:
	

	RenderTarget* mainwindow;
};