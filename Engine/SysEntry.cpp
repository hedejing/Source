#include "SysEntry.h"
#include "RenderSystem.h"
#include "RenderTarget.h"
#include "EGLRenderWindow.h"
#include "GlobalVariablePool.h"
#include "SceneManager.h"
#include "MeshManager.h"
#include "Camera.h"

#include "Timer.h"
#include <sstream>
#include "Entity.h"
#include<string>
#include "EGLRenderTarget.h"

#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#ifndef ANDROID
//#include <Windows.h>
#endif
#include "Pipeline.h"
namespace HW
{
	SysEntry * SysEntry::msingleEntry = NULL;

	SysEntry::SysEntry()
	{
		mRenderSystem = NULL;
		mMainWindow = NULL;
		camera = NULL;
		postEffect = NULL;
		rtforpost = NULL;
		lightEffect = NULL;
		for (unsigned int i = 0; i < 3; i++)
		{
			m_light_shadow[i] = m_light_depth[i] = NULL;
		}
		m_light = NULL;
		 
		m_blur1 = m_blur2 = NULL;
	}
	SysEntry::~SysEntry()
	{
		if (rtforpost)
			delete rtforpost;

	}

	void SysEntry::Initialize()
	{

	}

	SysEntry * SysEntry::getSingletonPtr()
	{
		if (!msingleEntry)
		{
			msingleEntry = new SysEntry();
			msingleEntry->Initialize();
		}

		return msingleEntry;
	}

	SysEntry & SysEntry::getSingleton()
	{
		return *getSingletonPtr();
	}

	void SysEntry::setRenderSystem(RenderSystem * render)
	{
		mRenderSystem = render;

	}

	RenderSystem * SysEntry::getRenderSystem()
	{
		return mRenderSystem;
	}

	void SysEntry::addScene(SceneManager * scene)
	{
		mSceneList.push_back(scene);
	}

	void SysEntry::setMainWindow(RenderTarget * rendertarget)
	{
		mMainWindow = rendertarget;
	}

	void SysEntry::startRenderLoop()
	{
#ifndef ANDROID

		MSG msg;
		while (true)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message   == WM_QUIT)
					return;
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else
			{
				updateMainView();
			}
		}
#endif
	}

	void SysEntry::updateMainView()
	{
		m_pipeline->Render();
		
	}

	void SysEntry::prepareShadowDeferred()
	{
		//GlobalVariablePool * gp = GlobalVariablePool::GetSingletonPtr();
		////firt we generate shadowmap for all the scene
		//for (auto& scene : mSceneList)
		//{	
		//	const std::vector<Light *> & light_list = scene->getActiveLightVetcor();
		//	vector<Light*> shadowlightlist;
		//	for (auto& l : light_list){
		//		if (l->isCastShadow())
		//			shadowlightlist.push_back(l);
		//	}
		//	for (int i = 0; i < shadowlightlist.size(); i++)
		//	{
		//		auto light = light_list[i];
		//		RT_shadowMap[i] = new EGLRenderTarget;
		//		RT_shadowMap[i]->setDimension(1024, 1024);
		//		//RT_shadowMap[i]->init_as_depthbuffer();
		//	}
		//}
	}

	void SysEntry::renderShadow(vector<Light*> light_list,SceneManager* scene)
	{
			//for (int i = 0; i < light_list.size(); i++)
			//{
			//	auto light = light_list[i];
			//	Camera* camera=light->getCamera();
			//	RenderQueue queue;
			//	scene->getVisibleRenderQueue(camera, queue);
			//	mRenderSystem->RenderPass(camera, queue, storeDepth_pass, RT_shadowMap[i]);
			//}
	}

	void SysEntry::prepareShadow()
	{
		////	Logger::WriteAndroidInfoLog("Shadow Prepare begin");
		//GlobalVariablePool * gp = GlobalVariablePool::GetSingletonPtr();

		//std::vector<SceneManager *>::iterator it_scene = mSceneList.begin();
		//for (; it_scene != mSceneList.end(); it_scene++)
		//{
		//	ShaderManager * shaderMgr = ShaderManager::getSingletonPtr();
		//	SceneManager * scene = *it_scene;
		//	scene->getAnimation()->animate();

		//	Light::LightType light_type = Light::LT_UNKNOWN;

		//	if ((scene)->CastShadow())
		//	{
		//		const std::vector<Light *> & light_list = scene->getActiveLightVetcor();
		//		std::vector<Light* >::const_iterator it_light = light_list.begin();
		//		std::map<String, RenderTarget *> & shadow_list = scene->getShadowList();

		//		for (; it_light != light_list.end(); it_light++)
		//		{
		//			Light * light = *it_light;
		//			std::map<String, RenderTarget*>::iterator it_shadow;
		//			if (light->isCastShadow() && light->isLightOn())
		//			{
		//				light_type = light->getLightType();
		//				assert(light_type != Light::LT_UNKNOWN);
		//				Effect * effect = scene->getDepthEffect();
		//				if (effect == NULL)
		//					effect = m_light_depth[light_type - 1];
		//				scene->g_effect.push(std::make_pair(true, effect));
		//				it_shadow = shadow_list.find(light->getName());

		//				if (!it_shadow->second)
		//				{
		//					EGLRenderTexture * rendertexture;
		//					it_shadow->second = rendertexture = new EGLRenderTexture(it_shadow->first + "shadow_texture", mRenderSystem);

		//					switch (light->getLightType())
		//					{
		//					case Light::LT_DIRECTIONAL:
		//					case Light::LT_SPOT:break;
		//					case Light::LT_POINT:rendertexture->setActiveBuffer(EGLRenderTexture::UB_CUBE_MAP);
		//					}
		//					rendertexture->setDimension(1024, 1024);
		//					//	rendertexture->createInternalRes();

		//					//setup light camera and globalvariable
		//					switch (light->getLightType())
		//					{
		//					case Light::LT_DIRECTIONAL:
		//					{
		//												  // first get the bounding sphere of whole scene;

		//												  Camera * camera_ortho = scene->CreateCamera("camera_directionlight" + light->getName());

		//												  Texture * texture = rendertexture->getDepthTexture();
		//												  gp->SetVal(GVS_SHADOW_MAP, sizeof(Texture *), &texture);
		//												  //	float shader_bias = 0.0005;
		//												  //	gp->SetVal(GVS_SHADOW_BIAS,sizeof(float),&shader_bias);

		//												  rendertexture->addCamera(camera_ortho);

		//					}break;
		//					case Light::LT_POINT:
		//					{


		//											Texture * texture = rendertexture->getCubeMapTexture();
		//											gp->SetVal(GVS_SHADOW_MAP, sizeof(Texture*), &texture);
		//											//	float depth_bias = 5.0;
		//											//	gp->SetVal(GVS_SHADOW_BIAS,sizeof(float),&depth_bias);


		//											Vector3 camera_dir[6] = {
		//												Vector3(1, 0, 0), Vector3(-1, 0, 0), Vector3(0, 1, 0), Vector3(0, -1, 0),
		//												Vector3(0, 0, 1), Vector3(0, 0, -1)
		//											};
		//											Vector3 camera_up[6] = {
		//												Vector3(0, -1, 0), Vector3(0, -1, 0), Vector3(0, 0, 1), Vector3(0, 0, -1),
		//												Vector3(0, -1, 0), Vector3(0, -1, 0)
		//											};
		//											for (int i = 0; i < 6; i++)
		//											{
		//												std::string index;
		//												std::stringstream a;
		//												a << i;
		//												a >> index;
		//												Camera * camera_point = scene->CreateCamera("camera_point" + index);
		//												camera_point->lookAt(light->getPosition(), light->getPosition() + camera_dir[i], camera_up[i]);
		//												camera_point->setPerspective(90.0, 1.0, 0.1, 1000.0);
		//												rendertexture->addCamera(camera_point);
		//											}
		//											//	Effect * effect	= scene->getDepthEffect();
		//											//	scene->setUseGlobalEffect(true);
		//											//	scene->setGlobalEffect(effect);
		//											//	GpuProgram * program = effect->technique(0)->getPass(0)->getGpuProgram();
		//											//GpuProgramPtr program = shaderMgr->getGpuProgramByName("StoreDepth");
		//											//	rendertexture->setProgram(GpuProgramPtr(program));

		//					}break;
		//					case Light::LT_SPOT:
		//					{
		//										   Camera * camera_spot = scene->CreateCamera("camera_spot");
		//										   Texture * texture = rendertexture->getDepthTexture();
		//										   gp->SetVal(GVS_SHADOW_MAP, sizeof(Texture*), &texture);
		//										   //	float shader_bias = 0.0005;
		//										   //	gp->SetVal(GVS_SHADOW_BIAS,sizeof(float),&shader_bias);

		//										   rendertexture->addCamera(camera_spot);
		//										   //	GpuProgram * program = shaderMgr->getGpuProgramByName("depth");
		//										   //	rendertexture->setProgram(program);

		//					}break;
		//					}

		//				}
		//				if (light->isMoved() || light->getLightType() == Light::LT_DIRECTIONAL)
		//				{
		//					switch (light->getLightType())
		//					{
		//					case Light::LT_SPOT:
		//					{
		//										   std::vector<Camera *> & camera_list = it_shadow->second->getCamera();
		//										   for (std::vector<Camera *>::iterator it_camera = camera_list.begin(); it_camera != camera_list.end(); it_camera++)
		//										   {
		//											   Camera * camera_spot = *it_camera;
		//											   camera_spot->lookAt(light->getPosition(), light->getPosition() + light->getDirection(), Vector3(0, 1, 0));
		//											   float fov = min(90.0, 2.2 * acos(light->Cutoff) * 180.0 / M_PI);
		//											   camera_spot->setPerspective(fov, 1.0, 1.0, 1000);
		//											   Matrix4 bias(
		//												   0.5, 0, 0, 0,
		//												   0, 0.5, 0, 0,
		//												   0, 0, 0.5, 0,
		//												   0.5, 0.5, 0.5, 1);

		//											   Matrix4 BiasdepthMVP = bias.transpose() *camera_spot->getProjectionMatrixDXRH() * camera_spot->getViewMatrix();

		//											   gp->SetVal(GVS_SHADOW_MATRIX, sizeof(Matrix4), BiasdepthMVP.ptr());
		//											   //float shader_bias = 0.00005;
		//											   //	gp->SetVal(GVS_SHADOW_BIAS,sizeof(float),&shader_bias);

		//											   light->ShadowMatrix = BiasdepthMVP;
		//											   //	light->ShadowBias = shader_bias;
		//										   }
		//					}break;
		//					case Light::LT_DIRECTIONAL:
		//					{
		//												  std::vector<Camera *> &camera_list = it_shadow->second->getCamera();
		//												  for (std::vector<Camera *>::iterator it_camera = camera_list.begin(); it_camera != camera_list.end(); it_camera++)
		//												  {
		//													  Camera * camera_ortho = *it_camera;
		//													  Sphere sphere = scene->getSceneCastShadowSphere();
		//													  float distance = sphere.getRadius();
		//													  Vector3 eye_pos = sphere.getCenter() - light->getDirection() * distance;
		//													  Vector3 center = sphere.getCenter();
		//													  //	std::cout<<"<"<<eye_pos.x<<" "<<eye_pos.y<<" "<<eye_pos.z<<">"<<"radius = "<<distance<<std::endl;
		//													  //	std::cout<<"<"<<center.x<<" "<<center.y<<" "<<center.z<<">"<<std::endl;
		//													  camera_ortho->lookAt(eye_pos, sphere.getCenter(), Vector3(0, 1, 0));
		//													  camera_ortho->setFrustum(-distance, distance, -distance, distance, 1.0, 5000.0);
		//													  camera_ortho->setProjectionType(Camera::PT_ORTHOGONAL);
		//													  Matrix4 bias(
		//														  0.5, 0, 0, 0,
		//														  0, 0.5, 0, 0,
		//														  0, 0, 0.5, 0,
		//														  0.5, 0.5, 0.5, 1);
		//													  Matrix4 BiasdepthMVP = bias.transpose()* camera_ortho->getProjectionMatrixDXRH()*camera_ortho->getViewMatrix();

		//													  //	float shader_bias = 0.00001;  //this is for the newtreasure
		//													  //	shader_bias = 0.0000;   // acrnival
		//													  //	shader_bias = 0.0003;   // hangzhou
		//													  //	shader_bias = 0.0001;
		//													  //	gp->SetVal(GVS_SHADOW_BIAS,sizeof(float),&shader_bias);
		//													  light->ShadowMatrix = BiasdepthMVP;
		//													  //	light->ShadowBias = shader_bias;
		//												  }
		//					}break;
		//					case Light::LT_POINT:
		//					{
		//											std::vector<Camera *> & camera_list = it_shadow->second->getCamera();
		//											//	Camera * camera_point = *camera_list.begin();
		//											//	camera_point->lookAt(light->getPosition(),light->getPosition() + Vector3(0,0,-1),Vector3(0,1,0));
		//											//	camera_point->setPerspective(90.0,1.0,1.0,4000.0);

		//											Vector3 camera_dir[6] = {
		//												Vector3(1, 0, 0), Vector3(-1, 0, 0), Vector3(0, 1, 0), Vector3(0, -1, 0),
		//												Vector3(0, 0, 1), Vector3(0, 0, -1)
		//											};
		//											Vector3 camera_up[6] = {
		//												Vector3(0, -1, 0), Vector3(0, -1, 0), Vector3(0, 0, 1), Vector3(0, 0, -1),
		//												Vector3(0, -1, 0), Vector3(0, -1, 0)
		//											};

		//											for (int i = 0; i < camera_list.size(); i++)
		//											{
		//												camera_list[i]->lookAt(light->getPosition(), light->getPosition() + camera_dir[i], camera_up[i]);
		//												camera_list[i]->setPerspective(90.0, 1.0, 0.1, 1000.0);
		//											}

		//											//	float shader_bias = 0.0005;
		//											//	gp->SetVal(GVS_SHADOW_BIAS,sizeof(float),&shader_bias);
		//											//	light->ShadowBias = shader_bias;
		//					}break;
		//					}
		//					light->completeMove();

		//				}

		//				//to do a optimized job we need to update the shadow only when it is necessary 
		//				//	Logger::WriteLog("About To Render Shadow");
		//				mrender_stage = RS_Depth;
		//				RenderQueue renderqueue;
		//				//mMainWindow->getCamera()[0] = it_shadow->second->getCamera()[0];
		//				mRenderSystem->Render(it_shadow->second, light, renderqueue, true);
		//				//Logger::WriteAndroidErrorLog("Render Shadow Complete");
		//				//scene->clearGlobalEffect();
		//				scene->g_effect.pop();
		//			}
		//		}

		//	}
		//}
		//	Logger::WriteLog("Shadow Prepared");
	}

	void SysEntry::doForwardLighting()
	{
		//RenderTarget * light_rt = NULL;
		//if (!postEffect)
		//{
		//	light_rt = mMainWindow;
		//}
		//else
		//{
		//	if (!rtforpost)
		//	{
		//		EGLRenderTexture * finalText = new EGLRenderTexture("lighting_texture", mRenderSystem);
		//		(*mSceneList.begin())->registerRenderTarget(finalText);
		//		rtforpost = finalText;
		//		finalText->setActiveBuffer(EGLRenderTexture::UB_COLOR_BUFFER | EGLRenderTexture::UB_DEPTH_STENCIL_BUFFER);
		//		finalText->setDimension(mMainWindow->getWidth(), mMainWindow->getHeight());
		//		finalText->createInternalRes();
		//		finalText->addCamera(*mMainWindow->getCamera().begin());


		//	}
		//	light_rt = rtforpost;
		//}

		//mrender_stage = RS_Light;
		//GlobalVariablePool * gp = GlobalVariablePool::GetSingletonPtr();
		//std::vector<SceneManager *>::iterator it_scene = mSceneList.begin();
		//ShaderManager * shaderMgr = ShaderManager::getSingletonPtr();
		//for (; it_scene != mSceneList.end(); it_scene++)
		//{
		//	SceneManager * scene = *it_scene;
		//	const vector<Light *> & active_lightlist = (*it_scene)->getActiveLightVetcor();
		//	std::map<String, RenderTarget *> & shadow_list = scene->getShadowList();
		//	if (scene->g_effect.empty())
		//	{
		//		Effect * default_effect = NULL;
		//		if (active_lightlist.empty())
		//			default_effect = m_light;
		//		else
		//			default_effect = m_light_shadow[1];
		//		scene->g_effect.push(std::make_pair(false, default_effect));
		//	}
		//	std::map<String, RenderTarget*>::iterator it_shadow;
		//	unsigned int size = active_lightlist.size();
		//	gp->SetVal(GVS_ACTIVATE_LIGHTNUM, sizeof(unsigned int), &(size));
		//	for (int light_index = 0; light_index < active_lightlist.size(); light_index++)
		//	{
		//		Light * light = active_lightlist[light_index];
		//		if (scene->CastShadow() && (light)->isCastShadow())
		//		{
		//			it_shadow = shadow_list.find(light->getName());
		//			Texture * texture = NULL;
		//			Matrix4 shadowMatrix;
		//			if (light->getLightType() == Light::LT_POINT)
		//			{
		//				texture = dynamic_cast<EGLRenderTexture*>(it_shadow->second)->getCubeMapTexture();
		//			}
		//			else
		//			{
		//				texture = dynamic_cast<EGLRenderTexture *>(it_shadow->second)->getDepthTexture();
		//				shadowMatrix = light->ShadowMatrix;
		//			}
		//			gp->SetVal(_global_makeFromkeyIndex(GVS_SHADOW_MAP, light_index), sizeof(Texture*), &texture);
		//			gp->SetVal(_global_makeFromkeyIndex(GVS_SHADOW_BIAS, light_index), sizeof(float), &light->ShadowBias);
		//			gp->SetVal(_global_makeFromkeyIndex(GVS_SHADOW_MATRIX, light_index), sizeof(Matrix4), shadowMatrix.ptr());
		//		}
		//		gp->SetLight(light, light_index);
		//	}

		//	bool buffer_cleared = false;
		//	if (scene->useSkyNode() && scene->getSky() != NULL)
		//	{
		//		mRenderSystem->RenderSky(light_rt, NULL, true);
		//		buffer_cleared = true;
		//	}
		//	RenderQueue queue;
		//	mRenderSystem->Render(light_rt, NULL, queue, !buffer_cleared, true);



		//}
	}

	void SysEntry::PostEffect()
	{
//		// memory leak
//		//static EGLRenderTexture* pEdgeTexture = new EGLRenderTexture("EdgeTexture", mRenderSystem);
//		//static EGLRenderTexture* pWeightTexture = new EGLRenderTexture("WeightTexture", mRenderSystem);
//
//		if (!postEffect) return;
//
//		Technique *pTechnique = postEffect->technique(0);
//		Entity* pQuadEntity = (*mSceneList.begin())->getScreeAlignedQuad();
//		Renderable* pQuadRenderable = pQuadEntity->getMesh()->getSubMeshBegin().get();
//		//Vector2 vScreenScale(1.0f / rtforpost->getWidth(), 1.0f / rtforpost->getHeight());
//		RenderQueueItem item;
//		item.entity = pQuadEntity;
//		item.renderable = pQuadRenderable;
//		item.material = pQuadRenderable->getMaterial().InterPtr(); // possibly buggy
//
//		// Depth Effect
//		item.material->pTextureDiffuse = dynamic_cast<EGLRenderTexture *>(rtforpost)->getColorTexture();
//		item.material->pTextureAmbient = dynamic_cast<EGLRenderTexture *>(rtforpost)->getDepthTexture();
//		item.pass = pTechnique->getPass(0);
//		mMainWindow->bindTarget();
//		mRenderSystem->ClearBuffer(0);
//		RenderQueue rq0(1, item);
////		mRenderSystem->Render(rq0);
	}

	void SysEntry::DebugShadow()
	{

//		Entity * ent = (*mSceneList.begin())->getScreeAlignedQuad();
//		Renderable * renderable = ent->getMesh()->getSubMeshBegin().get();
//		RenderQueueItem item;
//		item.renderable = renderable;
//		renderable->getMaterial()->pTextureDiffuse = dynamic_cast<EGLRenderTexture*>((*mSceneList.begin())->getShadowList().begin()->second)->getDepthTexture();
//		item.entity = ent;
//		item.pass = item.renderable->getMaterial()->getMaterialEffect()->technique(0)->getPass(0);
//		item.sortkey = 0;
//		item.material = item.renderable->getMaterial().InterPtr();
//		mMainWindow->bindTarget();
//		//mRenderSystem->ClearBuffer(0);
//		RenderQueue rq(1, item);
////		mRenderSystem->Render(rq);


	}
	void SysEntry::DebugTexture(Texture* texture)
	{

//		Entity * ent = (*mSceneList.begin())->getScreeAlignedQuad();
//		Renderable * renderable = ent->getMesh()->getSubMeshBegin().get();
//		RenderQueueItem item;
//		item.renderable = renderable;
//		renderable->getMaterial()->pTextureDiffuse =  texture;
//		item.entity = ent;
//		item.pass = item.renderable->getMaterial()->getMaterialEffect()->technique(0)->getPass(0);
//		item.sortkey = 0;
//		item.material = item.renderable->getMaterial().InterPtr();
//		mMainWindow->bindTarget();
//
//		RenderQueue rq(1, item);
////		mRenderSystem->Render(rq);
		

	}
	void SysEntry::doDeferredShading()
	{
		//doForwardShading(); return;
		
		//auto c = mMainWindow->getCamera();
		//camera=c[0];

		//RenderTarget * rt_final = mMainWindow;
		//mrender_stage = RS_Light;
		//GlobalVariablePool * gp = GlobalVariablePool::GetSingletonPtr();

		////ShaderManager * shaderMgr = ShaderManager::getSingletonPtr();
		//for (auto& scene:mSceneList)
		//{
		//	const vector<Light *> & active_lightlist = scene->getActiveLightVetcor();
		//	//std::map<String, RenderTarget *> & shadow_list = scene->getShadowList();

		//	//std::map<String, RenderTarget*>::iterator it_shadow;
		//	unsigned int size = active_lightlist.size();

		//	RT_gbuffer->bindTarget();
		//	RenderQueue queue;
		//	scene->getVisibleRenderQueue(camera, queue);
		//	

		//	
		//	mRenderSystem->RenderPass(camera, queue, geometry_pass, RT_gbuffer);

			
//			DebugTexture(RT_gbuffer->m_textures[4]);
			//if (version == 7){
			//	light_pass->getGpuProgram()->setProgramConstantData("g_diffuse", RT_gbuffer->m_textures[0], sizeof(Texture*));
			//	light_pass->getGpuProgram()->setProgramConstantData("g_normal", RT_gbuffer->m_textures[1], sizeof(Texture*));
			//	light_pass->getGpuProgram()->setProgramConstantData("g_specular", RT_gbuffer->m_textures[2], sizeof(Texture*));
			//	light_pass->getGpuProgram()->setProgramConstantData("g_normal2", RT_gbuffer->m_textures[3], sizeof(Texture*));
			//	light_pass->getGpuProgram()->setProgramConstantData("g_depth", RT_gbuffer->m_textures[4], sizeof(Texture*));
			//}
			//stringstream ss;
			////get the different light lists
			//vector<Light*> no_shadowlightlist;
			//vector<Light*> has_shadowlightlist;
			//for (auto& l: active_lightlist){
			//	if (l->isCastShadow())
			//		has_shadowlightlist.push_back(l);
			//	else
			//		no_shadowlightlist.push_back(l);
			//}
			//
			////no shadow light pass
			//BlendState b = BlendState::getDefault();
			//b.mBlendEnable = true;
			//b.mBlendFunc = BLEND_OP_ADD;
			//b.mDestBlend = BLEND_ONE;
			//b.mDestBlendAlpla = BLEND_ONE;

			//light_pass->mBlendState = b;
			//light_pass_shadow->mBlendState = b;

			//ClearState c = ClearState::getDefault();
			//c.mclearcolorbuffer = false;

			//light_pass->mClearState = c;
			//light_pass_shadow->mClearState = c;
			//light_pass->mDepthStencilState.mDepthEnable = false;
			//light_pass_shadow->mDepthStencilState.mDepthEnable = false;

			////prepare light geometrys
			//lightvao->bind();
			//vector<float> lightposition, lightdirection, lightcolor, lightparam;
			//vector<float> lightposition2, lightdirection2, lightcolor2, lightparam2,quadpos;
			//int size_pos, size_dir, size_color, size_param;
			//no_shadowlightlist[0]->CollectData(no_shadowlightlist, lightposition, size_pos, "position");
			//no_shadowlightlist[0]->CollectData(no_shadowlightlist, lightcolor, size_color, "color");
			//no_shadowlightlist[0]->CollectData(no_shadowlightlist, lightdirection, size_dir, "direction");
			//no_shadowlightlist[0]->CollectData(no_shadowlightlist, lightparam, size_param, "other");
			//for (int light_index = 0; light_index < no_shadowlightlist.size(); light_index++)
			//{
			//	Light * light = no_shadowlightlist[light_index];
			//	Vector4 rect;
			//	light->CalcScissorRect(camera, light->getMaxDistance(), rect);
			//	//6¸ö¶¥µã
			//	quadpos.push_back(rect.x); quadpos.push_back(rect.y);				
			//	quadpos.push_back(rect.z); quadpos.push_back(rect.y);
			//	quadpos.push_back(rect.x); quadpos.push_back(rect.w);
			//	quadpos.push_back(rect.x); quadpos.push_back(rect.w);
			//	quadpos.push_back(rect.z); quadpos.push_back(rect.y);
			//	quadpos.push_back(rect.z); quadpos.push_back(rect.w);
			//	for (int i = 0; i < 6; i++){
			//		for (int j = 0; j < 3; j++)
			//		{
			//			lightposition2.push_back(lightposition[3 * light_index + j]);
			//			lightdirection2.push_back(lightdirection[3 * light_index + j]);
			//			lightcolor2.push_back(lightcolor[3 * light_index + j]);
			//			lightparam2.push_back(lightparam[3 * light_index + j]);
			//		}
			//	}
			//}
			//lightvao->bind();
			//buf_quadpos->bufferFloats(&quadpos[0], quadpos.size(), GL_DYNAMIC_DRAW);
			//buf_lightpos->bufferFloats(&lightposition2[0], lightposition2.size(),GL_DYNAMIC_DRAW);
			//buf_lightdir->bufferFloats(&lightdirection2[0], lightdirection2.size(), GL_DYNAMIC_DRAW);
			//buf_lightcolor->bufferFloats(&lightcolor2[0], lightcolor2.size(), GL_DYNAMIC_DRAW);
			//buf_lightparam->bufferFloats(&lightparam2[0], lightparam2.size(), GL_DYNAMIC_DRAW);
			//lightvao->m_vertex_num = quadpos.size() / 2;
			//lightvao->addArrayBuffer(buf_quadpos, 0,2);
			//lightvao->addArrayBuffer(buf_lightpos, 1, 3);
			//lightvao->addArrayBuffer(buf_lightdir, 2, 3);
			//lightvao->addArrayBuffer(buf_lightcolor, 3, 3);
			////lightvao->addArrayBuffer(buf_lightparam, 4, 3);

			////render lights geometrys
			//mRenderSystem->Render(camera, lightvao, light_pass, rt_final);

			//render light with shadows
			//if (has_shadowlightlist.size()>0)
			//{
			//	renderShadow(has_shadowlightlist, scene);
			//	if (version == 7){
			//		light_pass_shadow->getGpuProgram()->setProgramConstantData("g_diffuse", RT_gbuffer->m_textures[0], sizeof(Texture*));
			//		light_pass_shadow->getGpuProgram()->setProgramConstantData("g_normal", RT_gbuffer->m_textures[1], sizeof(Texture*));
			//		light_pass_shadow->getGpuProgram()->setProgramConstantData("g_specular", RT_gbuffer->m_textures[2], sizeof(Texture*));
			//		light_pass_shadow->getGpuProgram()->setProgramConstantData("g_normal2", RT_gbuffer->m_textures[3], sizeof(Texture*));
			//		light_pass_shadow->getGpuProgram()->setProgramConstantData("g_depth", RT_gbuffer->m_textures[4], sizeof(Texture*));
			//		for (int i = 0; i < has_shadowlightlist.size(); i++){
			//			stringstream ss;
			//			//ss << "ShadowMap[" << i + 1 << "]";
			//			ss << "ShadowMap";
			//			string name = ss.str();
			//			light_pass_shadow->getGpuProgram()->setProgramConstantData(name.c_str(), RT_shadowMap[i]->m_textures[0], sizeof(Texture*));
			//		}
			//	}
			//	Entity * ent = (*mSceneList.begin())->getScreeAlignedQuad();
			//	Renderable * renderable = ent->getMesh()->getSubMeshBegin().get();
			//	RenderQueueItem item;
			//	item.renderable = renderable;
			//	item.entity = ent;
			//	item.pass = light_pass_shadow;
			//	item.sortkey = 0;
			//	item.material = item.renderable->getMaterial().InterPtr();
			//	mMainWindow->bindTarget();
			//	RenderQueue rq(1, item);

			//	Light * light = has_shadowlightlist[0];
			//	int lightsize, num;
			//	void* p = 0;
			//	light->CollectData(has_shadowlightlist,p, lightsize,num);
			//	lightbuffer->bufferData(p, lightsize);
			//	light_pass_shadow->getGpuProgram()->bindUBO(lightbuffer, "Lights_block");

			//	mRenderSystem->Render(camera, rq, light_pass_shadow, rt_final);
			//}
		//}
	}

	void SysEntry::initDeferredShading()
	{


		//RT_gbuffer = new EGLRenderTarget();
		//RT_gbuffer->setDimension(mMainWindow->getWidth(), mMainWindow->getHeight());
		////RT_gbuffer->init_as_gbuffer();
		//


		//lightbuffer = new EGLBufferObject;
		//lightbuffer->createUniformBuffer();

		//buf_quadpos = new EGLBufferObject;
		//buf_quadpos->createArrayBuffer();

		//buf_lightpos = new EGLBufferObject;
		//buf_lightpos->createArrayBuffer();

		//buf_lightdir = new EGLBufferObject;
		//buf_lightdir->createArrayBuffer();

		//buf_lightcolor = new EGLBufferObject;
		//buf_lightcolor->createArrayBuffer();

		//buf_lightparam = new EGLBufferObject;
		//buf_lightparam->createArrayBuffer();

		//prepareShadowDeferred();
		//lightvao = new EGLVertexAttributeObject();
		//lightvao->create();

		//Logger::WriteLog("function:%s ,line: %d ,gl error %d", __FUNCTION__, __LINE__, glGetError());
	}

	void SysEntry::doForwardShading()
	{

	}

	void SysEntry::setPipeline(Pipeline* pipeline)
	{
		m_pipeline = pipeline;
	}

}
