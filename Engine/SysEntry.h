#ifndef HW_SYSENTRY_H
#define HW_SYSENTRY_H

#include <vector>
using namespace std;
#include "PreDefine.h"
namespace HW
{
	enum RenderStage
	{
		RS_Depth = 0x0,
		RS_Light,
		RS_PostEffect,
		RS_Count
	};
	 
	class SysEntry
	{ 
	public:
		static SysEntry * getSingletonPtr();
		static SysEntry& getSingleton();
		RenderSystem * getRenderSystem();
		void setRenderSystem(RenderSystem * render);
		void startRenderLoop();

		// load some default effect;
		// like light depth shdow;
		void Initialize();

		// For test Unicorn : 2015/09/09
		void InitializeForwardShading();
		void DXDoForwardShading();

		void updateMainView();

		void prepareShadow();
		void prepareShadowDeferred();
		void renderShadow(vector<Light*> light_list, SceneManager* scene);
		void doForwardLighting();
		void doForwardShading();
		void doDeferredShading();
		void PostEffect();
		void DebugShadow();
		void DebugTexture(Texture* texture);
		void addScene(SceneManager * scene);
		void setMainWindow(RenderTarget * rendertarget);
		void setPipeline(Pipeline* pipeline);
		void initDeferredShading();
		~SysEntry();
	private:
		SysEntry();
		
		std::vector<SceneManager *> mSceneList;
		RenderSystem * mRenderSystem;
		RenderTarget * mMainWindow;
		RenderTarget * rtforpost;
		RenderTarget * m_blur1;
		RenderTarget * m_blur2;

		//EGLRenderTarget* RT_gbuffer;
		//EGLRenderTarget* RT_shadowMap[20];

		Effect * m_light_depth[3];
		Effect * m_light_shadow[3];
		Effect * m_light;


		static SysEntry * msingleEntry;
	public:
		Camera* camera;
		Pipeline* m_pipeline;
		Effect * postEffect;
		Effect * lightEffect;
		RenderStage mrender_stage;
		EGLBufferObject* lightbuffer,*buf_lightpos,*buf_lightdir,*buf_lightcolor,*buf_lightparam,*buf_quadpos;
		EGLVertexAttributeObject* lightvao;
		int version;


	};
}
#endif