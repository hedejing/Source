#pragma once
#include "effect.h"
#include "PreDefine.h"
#include <string>
#include "Pipeline.h"
#include "mathlib.h"
#include "ShowTexturePipeline.h"
#include "EGLTexture.h"
using namespace std;
using namespace HW;


class VRInstanceForwardEffect :public PipelineEffect {
public:
	virtual void Init();
	virtual void Render();

	//input
	string input_scenename;
	string input_cameraname;

	//output
	RenderTarget* mainwindow, *rt_render;
	Texture* out_depth, *out_color;

	Vector3 LightPosition = Vector3(1.0, 20.0, 30.0);
	Vector3 LightColor = Vector3(1, 1, 1);

	Matrix4 projLeft, viewLeft, projRight, viewRight;
private:
	as_Pass* forward_pass;
};



class VRInstanceForwardPipeline :public Pipeline {
public:
	virtual void Init();
	virtual void Render();

	VRInstanceForwardEffect eForward;
	ShowTextureEffect eShowtex;

	void GetStereoTex(GLuint & tex);
};