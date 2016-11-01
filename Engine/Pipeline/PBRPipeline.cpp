#include "PBRPipeline.h"

#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "EGLUtil.h"
#include <random>
#include "Image.h"
#include "Texture.h"

void PBREffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	mainwindow = rt_mgr.get("MainWindow");
	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;


	vector<string> e1 = { "ennis_c00.hdr", "ennis_c01.hdr", "ennis_c02.hdr", "ennis_c03.hdr", "ennis_c04.hdr", "ennis_c05.hdr" };
	vector<string> e2 = { "pisa_c00.hdr", "pisa_c01.hdr", "pisa_c02.hdr", "pisa_c03.hdr", "pisa_c04.hdr", "pisa_c05.hdr" };
	vector<string> e3 = { "cubemap/posx.jpg", "cubemap/negx.jpg",  "cubemap/negy.jpg","cubemap/posy.jpg", "cubemap/posz.jpg", "cubemap/negz.jpg" };
	vector<string> e4 = { "hdrmap/LightProbes/glacier_c00.hdr", "hdrmap/LightProbes/glacier_c01.hdr", 
		"hdrmap/LightProbes/glacier_c02.hdr", "hdrmap/LightProbes/glacier_c03.hdr", 
		"hdrmap/LightProbes/glacier_c04.hdr", "hdrmap/LightProbes/glacier_c05.hdr" };	
	vector<string> e5 = { "cubemap/rnl_cross/1.png","cubemap/rnl_cross/2.png", "cubemap/rnl_cross/3.png", "cubemap/rnl_cross/4.png",
		"cubemap/rnl_cross/5.png", "cubemap/rnl_cross/6.png" };
#ifdef ANDROID
	vector<string> e6 = { "cubemap/cloud_c00.jpg","cubemap/cloud_c01.jpg", "cubemap/cloud_c02.jpg", "cubemap/cloud_c03.jpg",
		"cubemap/cloud_c04.jpg", "cubemap/cloud_c05.jpg"

	};
#else
	vector<string> e6 = { "cubemap/cloud_c00.hdr","cubemap/cloud_c01.hdr", "cubemap/cloud_c02.hdr", "cubemap/cloud_c03.hdr",
		"cubemap/cloud_c04.hdr", "cubemap/cloud_c05.hdr"

};
#endif // ANDROID
	vector<string> e7 = { "cubemap/ennis_diffuse_c00.bmp", "cubemap/ennis_diffuse_c01.bmp", "cubemap/ennis_diffuse_c02.bmp", 
		"cubemap/ennis_diffuse_c03.bmp", "cubemap/ennis_diffuse_c04.bmp", "cubemap/ennis_diffuse_c05.bmp" };
	vector<string> e8 = { "ennis_c00.jpg", "ennis_c01.jpg", "ennis_c02.jpg", "ennis_c03.jpg", "ennis_c04.jpg", "ennis_c05.jpg" };

	vector<string> e9 = { "hdrmap/LightProbes/uffizi_c00.hdr", "hdrmap/LightProbes/uffizi_c01.hdr",
		"hdrmap/LightProbes/uffizi_c02.hdr", "hdrmap/LightProbes/uffizi_c03.hdr",
		"hdrmap/LightProbes/uffizi_c04.hdr", "hdrmap/LightProbes/uffizi_c05.hdr" };
	vector<string> e9_diffuse = { "hdrmap/diffuse/uffizi_d_c00.hdr", "hdrmap/diffuse/uffizi_d_c01.hdr",
		"hdrmap/diffuse/uffizi_d_c02.hdr", "hdrmap/diffuse/uffizi_d_c03.hdr",
		"hdrmap/diffuse/uffizi_d_c04.hdr", "hdrmap/diffuse/uffizi_d_c05.hdr" };
	vector<string> e10 = { "cubemap/rnl_c00.hdr", "cubemap/rnl_c01.hdr", "cubemap/rnl_c02.hdr",
		"cubemap/rnl_c03.hdr", "cubemap/rnl_c04.hdr", "cubemap/rnl_c05.hdr" };

	env = tm.CreateTextureCubeMap(e9, "pbr_envmap1");

	env->createInternalRes();

	env_filtered = tm.CreateTextureCubeMap(e9_diffuse, "pbr_env_filter");

	lut=tm.CreateTextureFromImage("convolution_spec.tga", "pbr_lut");	
	lut->mipmap_mode = Texture::TextureMipmap_None;
	pbr_pass=PassManager::getInstance().LoadPass("pbr/pbr_prog.json");
	createNoise();
	createSamples();
	createggxTexture();

	if (!isRendertoMainWindow){
		output_rt = rt_mgr.CreateRenderTargetFromPreset("basic","effect_pbr");
		output_rt->createInternalRes();
		color = tm.get("effect_pbr_tex1");
	}
}

void PBREffect::Render()
{
	SceneManager* scene = SceneContainer::getInstance().get("scene1");
	auto camera = scene->getCamera("main");

	//get a screen quad
	auto quad = GlobalResourceManager::getInstance().as_meshManager.get("ScreenQuad.obj");
	RenderQueueItem item;
	item.asMesh = quad;
	RenderQueue queue2;
	queue2.push_back(item);

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);//暂时禁用了octree的剔除

	auto& tm = TextureManager::getInstance();
	GpuProgram* p = pbr_pass->mProgram;
	p->setProgramConstantData("gbuffer_depth", input_depth);
	p->setProgramConstantData("gbuffer_normal", input_normal1);
	p->setProgramConstantData("gbuffer_diffuse", input_diffuse);
	p->setProgramConstantData("gbuffer_specular", input_specular);
	p->setProgramConstantData("gbuffer_normal2", input_normal2);

	p->setProgramConstantData("env_map", env);
	p->setSampler("env_map", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("env_map_filtered", env_filtered);
	p->setSampler("env_map_filtered", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("random_direction_3x3_sampler", tex_noise);
	
	p->setSampler("lut_sampler", SamplerManager::getInstance().get("ClampPoint"));
	p->setProgramConstantData("lut_sampler", lut);
	
	//p->setSampler("ggx_sampler", SamplerManager::getInstance().get("ClampPoint"));
	p->setProgramConstantData("ggx_sampler", tex_ggx);
	

	p->setProgramConstantData("cosine_hemisphere_sampler", &samples_IBL_cosine[0][0], "vec3", num_samples_IBL*sizeof(Vector3));
	p->setProgramConstantData("metalness", &metalness, "float", sizeof(float));
	p->setProgramConstantData("roughness", &roughness, "float", sizeof(float));
	p->setProgramConstantData("num_samples", &num_samples_IBL, "int", sizeof(int));
	p->setProgramConstantData("sun_light_direction", &light_dir[0], "vec3", sizeof(Vector3));

	if (shadow_map != NULL) {
		p->setProgramConstantData("shadow_map_sampler", shadow_map);
		//p->setSampler("shadow_map_sampler",SamplerManager::getInstance().get("ShadowMap"));
		p->setProgramConstantData("shadow_mat", &shadow_mat[0][0], "mat4", sizeof(Matrix4));
	}
	if (ao_map != NULL) {
		p->setProgramConstantData("ao_map_sampler", ao_map);
	}

	if (isRendertoMainWindow)
		mRenderSystem->RenderPass(camera, queue, pbr_pass, mainwindow);
	else
		mRenderSystem->RenderPass(camera, queue2, pbr_pass, output_rt);
}

void PBREffect::changeSampleNum(int num)
{
	num_samples_IBL = num;
	createSamples();
	refreshGgxTexture();
}

void PBREffect::createNoise()
{
	std::vector<Vector4> noise(3 * 3);
	for (int i = 0; i != noise.size(); ++i) {
		float angle = 2.f * M_PI * ((uniform_1d_sample() * noise.size() + uniform_1d_sample()) / noise.size());
		noise[i].x = cos(angle);
		noise[i].y = sin(angle);
	}
	auto& tm = TextureManager::getInstance();
	Texture* t = tm.CreateTextureFromPreset("image_noise_repeat");
	t->width = 3;
	t->height = 3;
	t->name = "pbr_noise_texture";
	tm.add(t);
	Image* im = new Image;
	im->CreateImageFromData(Image::FLOAT_32_32_32_32, 3, 3, &noise[0][0]);
	t->image = im;

	t->createInternalRes();
	tex_noise = t;
}

float PBREffect::uniform_1d_sample()
{
	std::mt19937_64 engine;
	std::uniform_real_distribution<float> dist;
	return dist(engine);
}

Vector3 PBREffect::sample_hemisphere_ggx(Vector2 u2d, float alpha)
{
	float cosphi = cos(2.f * M_PI * u2d.x);
	float costheta = sqrt((1.f - u2d.y) / (1.f + (alpha * alpha - 1.f) * u2d.y));
	float sinphi = sin(2.f * M_PI * u2d.x);
	float sintheta = sqrt(1.f - costheta * costheta);
	return Vector3(sintheta * cosphi, sintheta * sinphi, costheta);
}

Vector3 PBREffect::sample_hemisphere_cosine(Vector2 u2d)
{
	float cosphi = cos(2.f * M_PI * u2d.x);
	float costheta = sqrt(u2d.y);
	float sinphi = sin(2.f * M_PI * u2d.x);
	float sintheta = sqrt(1.f - costheta * costheta);
	return Vector3(sintheta * cosphi, sintheta * sinphi, costheta);
}

void PBREffect::stratified_2d_samples(int nx, int ny, vector<Vector2> &samples)
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

void PBREffect::createSamples()
{
	
	std::vector<Vector2> u2d;
	stratified_2d_samples(num_samples_IBL, num_samples_IBL, u2d);
	samples_IBL_cosine.clear();
	samples_IBL_ggx.clear();
	for (int i = 0; i != num_samples_IBL; ++i)
		samples_IBL_cosine.push_back(sample_hemisphere_cosine(u2d[i]));

	for (int j = 0; j != 100; ++j) {
		float r = (j + .5f) / 100;
		float a = r * r;
		for (int i = 0; i != num_samples_IBL; ++i)
			samples_IBL_ggx.push_back(sample_hemisphere_ggx(u2d[i], a));
	}
}

void PBREffect::createggxTexture()
{
	auto& tm = TextureManager::getInstance();
	Texture* t = tm.CreateTextureFromPreset("image_noise_repeat");
	t->name = "ggx_texture";
	t->mipmap_mode = Texture::TextureMipmap_None;

	Image* im = new Image;
	im->CreateImageFromData(Image::FLOAT_32_32_32, num_samples_IBL, 100, &samples_IBL_ggx[0][0]);
	t->image = im;
	t->setFormatFromImage(im);
	t->setFormat(Texture::RGB16F);
	tm.add(t);
	t->createInternalRes();
	tex_ggx = t;
}

void PBREffect::refreshGgxTexture()
{
	tex_ggx->releaseInternalRes();
	tex_ggx->width = num_samples_IBL;
	tex_ggx->height = 100;
	tex_ggx->image->CreateImageFromData(Image::FLOAT_32_32_32, num_samples_IBL, 100, &samples_IBL_ggx[0][0]);
	tex_ggx->createInternalRes();
}

void PBRPipeline::Init()
{
	gbuffer_effect.input_scenename = "scene1";
	gbuffer_effect.input_cameraname = "main";
	gbuffer_effect.Init();

	pbr_effect.input_depth = gbuffer_effect.depth;
	pbr_effect.input_diffuse = gbuffer_effect.diffuse;
	pbr_effect.input_normal1 = gbuffer_effect.normal1;
	pbr_effect.input_normal2 = gbuffer_effect.normal2;
	pbr_effect.input_specular = gbuffer_effect.specular;
	pbr_effect.isRendertoMainWindow = true;
	pbr_effect.Init();
}

void PBRPipeline::Render()
{
	gbuffer_effect.Render();
	pbr_effect.Render();
}


void PBREffect2::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	mainwindow = rt_mgr.get("MainWindow");
	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	rt1 = rt_mgr.CreateRenderTargetFromPreset("basic_float", "effect_pbr");
	color = rt1->m_attach_textures["color0"];
	depth = rt1->m_attach_textures["depth"];

	pbr_pass = PassManager::getInstance().LoadPass("pbr/pbr_prog.json");
	env_pass = PassManager::getInstance().LoadPass("pbr/pbr_render_env_prog.json");
	filter_env_pass= PassManager::getInstance().LoadPass("pbr/pbr_filter_envmap_prog.json");
	integrateBRDF_pass= PassManager::getInstance().LoadPass("pbr/pbr_IntegrateBRDF_prog.json");
	pbr_use_prefilter_pass= PassManager::getInstance().LoadPass("pbr/pbr_use_prefilter_prog.json");

	auto e_pisa = GenCubemapList("hdrmap/LightProbes/pisa_c0", ".hdr");
	auto e_pisa_diffuse= GenCubemapList("hdrmap/diffuse/pisa_d_c0", ".hdr");
	auto e_uffizi= GenCubemapList("hdrmap/LightProbes/uffizi_c0", ".hdr");
	auto e_uffizi_diffuse= GenCubemapList("hdrmap/diffuse/uffizi_d_c0", ".hdr");

	env = tm.CreateTextureCubeMap(e_pisa, "pisa");
	env_diffuse = tm.CreateTextureCubeMap(e_pisa_diffuse, "pbr_env_filter");

	// 对一个 cubemap 进行 ggx filter 操作
	//PrefilterEnvMapSaveImages(env, "default_asserts/hdrmap/ggx_filtered/pisa");
	LoadPrefilterEnvMap(env, "hdrmap/ggx_filtered/pisa");



	lut = tm.CreateTextureFromImage("convolution_spec.tif", "pbr_lut");
	lut->mipmap_mode = Texture::TextureMipmap_None;
	base_color = Vector3(1.0, 1.0, 1.0);
	//e_showtex.input_texture = tm.CreateTextureFromImage("hdrmap/LightProbes/pisa_c00.hdr");
	//e_showtex.Init();
}

void PBREffect2::Render()
{
	SceneManager* scene = SceneContainer::getInstance().get("scene1");
	auto camera = scene->getCamera("main");

	//get a screen quad
	auto quad = GlobalResourceManager::getInstance().as_meshManager.get("ScreenQuad.obj");
	RenderQueueItem item;
	item.asMesh = quad;
	RenderQueue queue2;
	queue2.push_back(item);

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);//暂时禁用了octree的剔除

	auto& tm = TextureManager::getInstance();
	auto render_pass = pbr_pass;
	render_pass = pbr_use_prefilter_pass;
	GpuProgram* p = render_pass->mProgram;
	pbr_pass->mClearState.mClearColor = Vector4(0.3,0.3,0.3,1.0);
	p->setProgramConstantData("env_map", env);
	p->setSampler("env_map", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("env_map_filtered", env_diffuse);
	p->setSampler("env_map_filtered", SamplerManager::getInstance().get("ClampLinear"));
	p->setProgramConstantData("IntegrateBRDFSampler", lut);
	p->setSampler("IntegrateBRDFSampler", SamplerManager::getInstance().get("ClampPoint"));
	p->setProgramConstantData("BaseColor", &base_color[0], "vec3", sizeof(Vector3));
	int b = 0;
	if (b)
		mRenderSystem->RenderPass(camera, queue, render_pass, mainwindow);
	else {
		mRenderSystem->RenderPass(camera, queue, render_pass, rt1);

		p = env_pass->mProgram;
		p->setProgramConstantData("env_map", env);
		p->setSampler("env_map", SamplerManager::getInstance().get("ClampLinear"));
		p->setProgramConstantData("depth", depth);
		p->setSampler("depth", SamplerManager::getInstance().get("ClampPoint"));
		p->setProgramConstantData("front", color);
		p->setSampler("front", SamplerManager::getInstance().get("ClampPoint"));

		if(in_RT==NULL)
			mRenderSystem->RenderPass(camera, queue2, env_pass, mainwindow);
		else
			mRenderSystem->RenderPass(camera, queue2, env_pass, in_RT);
	}
	
	//mRenderSystem->RenderPass(NULL, queue2, integrateBRDF_pass, mainwindow);



	//e_showtex.Render();
}

void PBREffect2::PrefilterEnvMapSaveImages(Texture* env,string path)
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	
	auto quadmesh = GlobalResourceManager::getInstance().as_meshManager.get("ScreenQuad.obj");
	RenderQueueItem item;
	item.asMesh = quadmesh;
	RenderQueue quad;
	quad.push_back(item);

	int w = env->width, h = env->height;
	int ImageMip = env->CalcMipmapLevel(w, h);
	assert(ImageMip >= 10);

	for (int mip = 1; mip <= 5; mip++) {
		w /= 2; h /= 2;
		RenderTarget* rt_filter = rt_mgr.CreateRenderTargetFromPreset("basic_float", "prefilter_cubemap_fbo_mip" + to_string(mip)
			,w,h);
		rt_filter->createInternalRes();
		float r = 0.2*mip;
		for (int face = 0; face < 6; face++)
		{
			auto p = filter_env_pass->mProgram;
			p->setProgramConstantData("env_map", env);
			p->setSampler("env_map", SamplerManager::getInstance().get("ClampLinear"));		
			p->setProgramConstantData("roughness", &r, "float", sizeof(float));
			p->setProgramConstantData("face", &face, "int", sizeof(int));
			mRenderSystem->RenderPass(NULL, quad, filter_env_pass, rt_filter);
			string ImageName = path + "_mip" + to_string(mip) + "_face" + to_string(face);
			rt_filter->m_attach_textures["color0"]->SaveTextureToImage(ImageName, Texture::ST_FLOAT);
			//rt_filter->m_attach_textures["color0"]->SaveTextureToImage("s1", Texture::ST_UNSIGNED_BYTE);
		}

	}

}

void PBREffect2::PrefilterEnvMapLatitude(Texture* env, string path)
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();

	auto quadmesh = GlobalResourceManager::getInstance().as_meshManager.get("ScreenQuad.obj");
	RenderQueueItem item;
	item.asMesh = quadmesh;
	RenderQueue quad;
	quad.push_back(item);

	int w = env->width, h = env->height;
	int ImageMip = env->CalcMipmapLevel(w, h);
	assert(ImageMip >= 10);
	w = 4096;
	h = 2048;
	for (int mip = 1; mip < 10; mip++) {
		w /= 2; h /= 2;
		RenderTarget* rt_filter = rt_mgr.CreateRenderTargetFromPreset("basic_float", "prefilter_cubemap_fbo_mip" + to_string(mip)
			, w, h);
		rt_filter->createInternalRes();
		float r = 0.1*mip;
		int UseLatitude = 1;
		auto p = filter_env_pass->mProgram;
		p->setProgramConstantData("env_map", env);
		p->setSampler("env_map", SamplerManager::getInstance().get("ClampLinear"));
		p->setProgramConstantData("roughness", &r, "float", sizeof(float));
		p->setProgramConstantData("UseLatitude", &UseLatitude, "int", sizeof(int));
		p->setProgramConstantData("textureSize", &w, "int", sizeof(int));
		mRenderSystem->RenderPass(NULL, quad, filter_env_pass, rt_filter);
		string ImageName = path + "_mip" + to_string(mip);
		rt_filter->m_attach_textures["color0"]->SaveTextureToImage(ImageName, Texture::ST_FLOAT);
	}
}

void PBREffect2::LoadPrefilterEnvMap(Texture* env, string path)
{

	int w = env->width, h = env->height;
	int ImageMip = env->CalcMipmapLevel(w, h);
	assert(ImageMip >= 10);
	env->useTexture();
	for (int mip = 1; mip <= 5; mip++) {
		w /= 2; h /= 2;
		for (int face = 0; face < 6; face++)
		{
			Image im;			
			string ImageName = path + "_mip" + to_string(mip) + "_face" + to_string(face)+".hdr";
			im.LoadImage(ImageName, true);
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,mip, 0, 0, w, h, GL_RGB, GL_FLOAT, &im.pdata[0]);
		}

	}
	GL_CHECK();
}

void PBRPipeline2::Init()
{
	ePBr.Init();
}

void PBRPipeline2::Render()
{
	ePBr.Render();
}
