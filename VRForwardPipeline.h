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


class VRForwardEffect :public PipelineEffect {
public:
	virtual void Init();
	virtual void Render();

	//input
	string input_scenename;
	string input_cameraname;

	//output
	RenderTarget* mainwindow, *rt_render_left, *rt_render_right;
	Texture* out_depth_left, *out_depth_right, *out_color_left, *out_color_right;

	Vector3 LightPosition = Vector3(1.0, 20.0, 30.0);
	Vector3 LightColor = Vector3(1, 1, 1);

	Matrix4 projLeft, viewLeft, projRight, viewRight;
private:
	as_Pass* forward_pass;
};



class VRForwardPipeline :public Pipeline {
public:
	virtual void Init();
	virtual void Render();

	VRForwardEffect eForward;
	ShowTextureEffect eShowtex;

	void GetStereoTex(GLuint& l, GLuint& r);
};