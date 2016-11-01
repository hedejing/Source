#include "SSAOPipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "EGLUtil.h"
#include "Image.h"
#include <random>
void SSAOEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	mainwindow = rt_mgr.get("MainWindow");
	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;


	rt_mgr.CreateRenderTargetFromPreset("basic", "ssao_raw");
	output_rt=rt_mgr.CreateRenderTargetFromPreset("basic_linear", "ssao_blur");
	output_rt->createInternalRes();

	output_ao = tm.get("ssao_blur_tex1");
	//output_ao= tm.get("ssao_raw_tex1");
	PassManager::getInstance().LoadPass("ssao/old/ssao_prog.json");
	PassManager::getInstance().LoadPass("ssao/old/ssao_blur_prog.json");
	createSample();
	createNoise();
}

void SSAOEffect::Render()
{
	SceneManager* scene = SceneContainer::getInstance().get("scene1");
	auto camera = scene->getCamera("main");
	//draw a screen quad
	auto quad = GlobalResourceManager::getInstance().as_meshManager.get("ScreenQuad.obj");
	RenderQueueItem item;
	item.asMesh = quad;
	RenderQueue queue2;
	queue2.push_back(item);

	RenderTarget* rt2 = RenderTargetManager::getInstance().get("ssao_raw");
	as_Pass* ssao_pass = PassManager::getInstance().get("ssao pass");
	auto& tm = TextureManager::getInstance();

	ssao_pass->mProgram->setProgramConstantData("gbufferDepth", depth);
	ssao_pass->mProgram->setSampler("gbufferDepth", SamplerManager::getInstance().get("ClampPoint"));
	ssao_pass->mProgram->setProgramConstantData("gbufferNormal", normal);
	ssao_pass->mProgram->setProgramConstantData("noise", noise_tex); 
	ssao_pass->mProgram->setSampler("noise", SamplerManager::getInstance().get("RepeatPoint"));
	ssao_pass->mProgram->setProgramConstantData("radius", &radius, "float", sizeof(float));
	//ssao_pass->mProgram->setProgramConstantData("g_random", random_tex);
	if(num_samples_ssao>0)
		ssao_pass->mProgram->setProgramConstantData("samples", &samples_ssao[0][0], "vec3", num_samples_ssao*sizeof(Vector3));
	ssao_pass->mProgram->setProgramConstantData("num_samples", &num_samples_ssao, "int", sizeof(int));
	mRenderSystem->RenderPass(camera, queue2, ssao_pass, rt2);

	as_Pass* blur_pass = PassManager::getInstance().get("ssao_blur pass");
	
	blur_pass->mProgram->setProgramConstantData("image_acc", tm.get("ssao_raw_tex1"));
	blur_pass->mProgram->setSampler("image_acc", SamplerManager::getInstance().get("ClampLinear"));
	mRenderSystem->RenderPass(NULL, queue2, blur_pass, output_rt);
}

void SSAOEffect::createSample()
{
	//std::mt19937 engine;
	//std::uniform_real_distribution<float> dist;
	//samples_ssao.clear();
	//for (int i = 0; i != num_samples_ssao; ++i) {
	//	Vector2 u2d = hammersley2d(i, num_samples_ssao);
	//	Vector3 s = sample_hemisphere_uniform(u2d);
	//	float attenuation = 1.f - sqrt((i + dist(engine)) / num_samples_ssao);
	//	samples_ssao.push_back(attenuation * s);
	//}
	samples_ssao.clear();
	std::vector<float> u1ds;
	std::vector<Vector2> u2ds;
	stratified_1d_samples(num_samples_ssao, u1ds);
	stratified_2d_samples(num_samples_ssao, num_samples_ssao, u2ds);
	for (int i = 0; i != num_samples_ssao; ++i) {
		float u1d = u1ds[i];
		Vector2 u2d = u2ds[i];
		Vector3 s = sample_hemisphere_uniform(u2d);
		float attenuation = 1.f - sqrt(u1d);
		samples_ssao.push_back(attenuation * s);
	}

}

float SSAOEffect::radicalInverse_VdC(unsigned bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10f;
}

Vector2 SSAOEffect::hammersley2d(unsigned i, unsigned N)
{
	return Vector2(float(i) / float(N), radicalInverse_VdC(i));
}

Vector3 SSAOEffect::sample_hemisphere_uniform(Vector2 u2d)
{
	float cosphi = cos(2.f * M_PI * u2d.x);
	float costheta = u2d.y;
	float sinphi = sin(2.f * M_PI * u2d.x);
	float sintheta = sqrt(1.f - costheta * costheta);
	return Vector3(sintheta * cosphi, sintheta * sinphi, costheta);
}

void SSAOEffect::createNoise()
{

	std::vector<float> u1ds_dir;
	stratified_1d_samples(9, u1ds_dir);
	std::vector<Vector3> noise(3 * 3);
	for (int i = 0; i != noise.size(); ++i) {
		float angle = 2.f * M_PI * u1ds_dir[i];
		noise[i].x = cos(angle);
		noise[i].y = sin(angle);
		noise[i].z = 0;
	}
	auto& tm = TextureManager::getInstance();
	Texture* t = tm.CreateTextureFromPreset("image_noise_repeat");

	Image* im = new Image;
	im->CreateImageFromData(Image::FLOAT_32_32_32, 3, 3, &noise[0][0]);
	t->image = im;
	t->setFormatFromImage(im);
	t->setFormat(Texture::RGB16F);

	t->createInternalRes();
	noise_tex = t;

}

void SSAOEffect::setSampleNum(int num)
{
	num_samples_ssao = num;
	createSample();
}

float SSAOEffect::uniform_1d_sample()
{
	std::mt19937_64 engine;
	std::uniform_real_distribution<float> dist;
	return dist(engine);
}

void SSAOEffect::stratified_1d_samples(int n, std::vector<float> &samples)
{
	std::mt19937_64 engine;
	std::uniform_real_distribution<float> dist;
	for (int i = 0; i != n; ++i) {
		float x = (i + dist(engine)) / n;
		samples.emplace_back(x);
	}
	std::shuffle(samples.begin(), samples.end(), engine);
}

void SSAOEffect::stratified_2d_samples(int nx, int ny, vector<Vector2> &samples)
{
	std::mt19937_64 engine;
	std::uniform_real_distribution<float> dist;
	for (int j = 0; j != ny; ++j) {
		for (int i = 0; i != nx; ++i) {
			float y = (j + dist(engine)) / ny;
			float x = (i + dist(engine)) / nx;
			samples.push_back(Vector2(x, y));
		}
	}
	std::shuffle(samples.begin(), samples.end(), engine);
}

void SSAOPipeline::Init()
{
	effect_gbuf.input_scenename = "scene1";
	effect_gbuf.input_cameraname = "main";
	effect_gbuf.Init();

	effect_ao.depth = effect_gbuf.depth;
	effect_ao.normal = effect_gbuf.normal2;
	effect_ao.Init();

	effect_show_tex.input_texture = effect_ao.output_ao;
	effect_show_tex.Init();
}

void SSAOPipeline::Render()
{
	effect_gbuf.Render();
	effect_ao.Render();
	effect_show_tex.Render();
}
float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void SSAOEffect2::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	mainwindow = rt_mgr.get("MainWindow");
	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	w = mainwindow->width, h = mainwindow->height;
	if (halfResolution) {
		w = w / 2; h = h / 2;
	}
	rt_raw_ao=rt_mgr.CreateRenderTargetFromPreset("basic", "ssao_raw",w,h);	
	output_rt = rt_mgr.CreateRenderTargetFromPreset("basic_linear", "ssao_blur",w,h);

	raw_ao = rt_raw_ao->m_attach_textures["color0"];
	output_ao = output_rt->m_attach_textures["color0"];

	ssao_pass=PassManager::getInstance().LoadPass("ssao/ssao_prog.json");
	blur_pass=PassManager::getInstance().LoadPass("ssao/ssao_blur_prog.json");
	createSample();
	createNoise();
}

void SSAOEffect2::Render()
{

	//draw a screen quad
	auto quad = GlobalResourceManager::getInstance().as_meshManager.get("ScreenQuad.obj");
	RenderQueueItem item;
	item.asMesh = quad;
	RenderQueue queue2;
	queue2.push_back(item);

	auto p = ssao_pass->mProgram;
	p->setProgramConstantData("gbufferDepth", depth);
	p->setSampler("gbufferDepth", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("gbufferNormal", normal);
	p->setSampler("gbufferNormal", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("noise", noise_tex);
	p->setSampler("noise", SamplerManager::getInstance().get("RepeatPoint"));
	p->setProgramConstantData("radius", &radius, "float", sizeof(float));
	p->setProgramConstantData("near", &Near, "float", sizeof(float));
	p->setProgramConstantData("far", &Far, "float", sizeof(float));
	p->setProgramConstantData("w", &w, "int", sizeof(int));
	p->setProgramConstantData("h", &h, "int", sizeof(int));
	//ssao_pass->mProgram->setProgramConstantData("g_random", random_tex);
	if (num_samples_ssao > 0)
		p->setProgramConstantData("samples", &samples_ssao[0][0], "vec3", num_samples_ssao*sizeof(Vector3));
	p->setProgramConstantData("num_samples", &num_samples_ssao, "int", sizeof(int));
	mRenderSystem->RenderPass(NULL, queue2, ssao_pass, rt_raw_ao);


	blur_pass->mProgram->setProgramConstantData("ssaoInput", raw_ao);
	blur_pass->mProgram->setSampler("ssaoInput", SamplerManager::getInstance().get("ClampLinear"));
	mRenderSystem->RenderPass(NULL, queue2, blur_pass, output_rt);
}

void SSAOEffect2::createSample()
{
	uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // Ëæ»ú¸¡µãÊý£¬·¶Î§0.0 - 1.0
	default_random_engine generator;
	for (GLuint i = 0; i < 64; ++i)
	{
		Vector3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
			);
		sample.normalize();
		sample *= randomFloats(generator);
		GLfloat scale = GLfloat(i) / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		samples_ssao.push_back(sample);
	}
}

void SSAOEffect2::createNoise()
{
	uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	default_random_engine generator;
	vector<Vector3> ssaoNoise;
	for (GLuint i = 0; i < 16; i++)
	{
		Vector3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}


	auto& tm = TextureManager::getInstance();
	Texture* t = tm.CreateTextureFromPreset("image_noise_repeat");

	Image* im = new Image;
	im->CreateImageFromData(Image::FLOAT_32_32_32, 4, 4, &ssaoNoise[0][0]);
	t->image = im;
	t->setFormatFromImage(im);
	t->setFormat(Texture::RGB16F);

	t->createInternalRes();
	noise_tex = t;
}

void SSAOEffect2::setSampleNum(int num)
{
	num_samples_ssao = num;
	createSample();
}
