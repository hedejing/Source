#include "TemporalAA.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "Camera.h"
#include "RenderUtil.h"

void TemporalAAPipeline::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	input_scenename = "scene1";
	input_cameraname = "main";
	auto& tm = TextureManager::getInstance();
	mainwindow = rt_mgr.get("MainWindow");
	w = mainwindow->getWidth();
	h = mainwindow->getHeight();


	rt_current = rt_mgr.CreateRenderTargetFromPreset("RT_2RGBA32", "effect_taa_1");

	rt_history = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_taa_2");
	rt_mix = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_taa_3");
	current_tex = rt_current->m_attach_textures["color0"];
	depth_tex= rt_current->m_attach_textures["depth"];
	history_tex = rt_history->m_attach_textures["color0"];
	mix_tex = rt_mix->m_attach_textures["color0"];
	velocity_tex= rt_current->m_attach_textures["color1"];

	velocity_pass = PassManager::getInstance().LoadPass("TAA/velocity_prog.json");

	linearSampler = SamplerManager::getInstance().get("ClampLinear");
	taa_pass = PassManager::getInstance().LoadPass("TAA/TAA_prog.json");
	eShowtex.input_texture = mix_tex;
	eShowtex.Init();
}

void TemporalAAPipeline::Render()
{

	SceneManager* scene = SceneContainer::getInstance().get(input_scenename);

	auto camera = scene->getCamera(input_cameraname);
	RenderSystem* mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);
	
	UpdateJitter();
	frameCount++;
	Matrix4 Projection = camera->getProjectionMatrixDXRH();
	if (EnableTemporalAA) {
		Projection = offsetMatrix*Projection;
		//Projection.m[0][2] = CurrentJitter.x;
		//Projection.m[1][2] = CurrentJitter.y;
	}
	Matrix4 view= camera->getViewMatrix();
	auto p = velocity_pass->mProgram;
	p->setProgramConstantData("Projection", &Projection[0][0], "mat4", sizeof(Matrix4));
	p->setProgramConstantData("ViewMatrix", &view[0][0], "mat4", sizeof(Matrix4));
	p->setProgramConstantData("PrevView", &PrevView[0][0], "mat4", sizeof(Matrix4));
	auto t = view - PrevView;
	p->setProgramConstantData("PrevProjection", &PrevProjection[0][0], "mat4", sizeof(Matrix4));
	p->setProgramConstantData("jitterOffset", &jitterOffset[0], "vec2", sizeof(Vector2));
	Vector2 RTSize(w, h);
	p->setProgramConstantData("RTSize", &RTSize[0], "vec2", sizeof(Vector2));
	p->setProgramConstantData("LightColor", &LightColor[0], "vec3", sizeof(Vector3));
	p->setProgramConstantData("LightPosition", &LightPosition[0], "vec3", sizeof(Vector3));
	p->setProgramConstantData("EnableTemporalAA", &EnableTemporalAA, "int", sizeof(int));
	//// render to a buffer
	mRenderSystem->RenderPass(camera, queue, velocity_pass, rt_current);

	
		//test
		//Vector4 pos(1, -5, 0, 1);
		//Vector4 posCS = Projection* camera->getViewMatrix()*pos;

		//Vector4 prevPos = PrevProjection*PrevView*pos;

		//Vector4 posCS1 = posCS / posCS.w;
		//Vector4 prevPos1 = prevPos / prevPos.w;

		//Vector2 v1 = Vector2(posCS1.x - prevPos1.x, posCS1.y - prevPos1.y)*0.5;
		//Vector2 v2 = v1*RTSize;//-jitterOffset;
		//Vector2 v3 = v2 - jitterOffset;
		//int b;
	


	PrevProjection = Projection;
	PrevView = view;


	auto quad = RenderUtil::GetScreenQuad();
	taa_pass->mProgram->setProgramConstantData("current", current_tex);
	taa_pass->mProgram->setProgramConstantData("history", history_tex);
	taa_pass->mProgram->setProgramConstantData("VelocityTexture", velocity_tex);
	taa_pass->mProgram->setProgramConstantData("DepthTexture", depth_tex);
	taa_pass->mProgram->setSampler("current", linearSampler);
	taa_pass->mProgram->setSampler("VelocityTexture", linearSampler);
	taa_pass->mProgram->setSampler("history", linearSampler);

	mRenderSystem->RenderPass(NULL, quad, taa_pass, rt_mix);
	//rt_mix->BlitTexture(rt_history, "color0", "color0");
	eShowtex.input_texture = mix_tex;
	eShowtex.Render();
	SwapBuffer();
}

void TemporalAAPipeline::SwapBuffer()
{
	std::swap(rt_history, rt_mix);
	std::swap(history_tex, mix_tex);
}

void TemporalAAPipeline::UpdateJitter()
{
	Vector2 jitter = 0.0f;
	if (jittermode == JitterModes::Uniform2x)
	{
		jitter = frameCount % 2 == 0 ? -0.5f : 0.5f;
	}
	else if (jittermode == JitterModes::Hammersley4x)
	{
		int idx = frameCount % 4;
		jitter = Hammersley2D(idx, 4) * 2.0f - Vector2(1.0, 1.0);
	}
	else if (jittermode == JitterModes::Hammersley8x)
	{
		int idx = frameCount % 8;
		jitter = Hammersley2D(idx, 8) * 2.0f - Vector2(1.0, 1.0);
	}
	else if (jittermode == JitterModes::Hammersley16x)
	{
		int idx = frameCount % 16;
		jitter = Hammersley2D(idx, 16) * 2.0f - Vector2(1.0, 1.0);
	}

	jitter *= jittlescale;

	float offsetX = jitter.x * (1.0f / w);
	float offsetY = jitter.y * (1.0f / h);

	offsetMatrix.m[0][3] = offsetX;
	offsetMatrix.m[1][3] = offsetY;
	CurrentJitter = Vector2(offsetX, offsetY);
	jitterOffset = (jitter - prevJitter)*0.5;
	prevJitter = jitter;
}

float TemporalAAPipeline::RadicalInverseBase2(unsigned int bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10f; // / 0x100000000
}

HW::Vector2 TemporalAAPipeline::Hammersley2D(uint64_t sampleIdx, uint64_t numSamples)
{
	return Vector2(float(sampleIdx) / float(numSamples), RadicalInverseBase2(uint32_t(sampleIdx)));
}

