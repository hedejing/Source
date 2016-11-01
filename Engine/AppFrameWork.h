//#ifndef HW_APPFRAMEWORK_H
//#define HW_APPFRAMEWORK_H
//
//#ifdef  ANDROID
//#include <jni.h>
//#include <errno.h>
//#include <android/sensor.h>
//#include <android/log.h>
//#include <android_native_app_glue.h>
////#include "pch.h"
//struct saved_state
//{
//	unsigned int input_type;
//	float angle;
//	AInputEvent * event;
//	int x;
//	int y;
//	int width;
//	int height;
//	float move_x;
//	float move_y;
//	saved_state():event(NULL)
//	{
//
//	}
//};
//
//
//#endif
//#include "mathlib.h"
//#include "SceneNode.h"
//#include "Entity.h"
//#include "SceneManager.h"
//#include "MeshManager.h"
//#include "Camera.h"
//
//#include "EGLRenderSystem.h"
//#include "GlobalVariablePool.h"
//#include "RaySceneQuery.h"
//#include "RenderTarget.h"
//#include "EGLRenderWindow.h"
//
////#include "AndroidTouch.h"
//#include "Sysentry.h"
//
//#include "OctreeSceneManager.h"
//#include <sstream>
//#include "FrameListener.h"
//#include "Timer.h"
////#include "Animation.h"
//#include "Logger.h"
////#include "DX11RenderSystem.h"
//#include <thread>
//#include <chrono>
//using namespace std;
//namespace HW
//{
//	class AppFrameWork
//	{
//
//	public:
//		AppFrameWork()
//		{
//#ifdef ANDROID
//			app = NULL;
//			sensorManager = NULL;
//			accelerometerSensor = NULL;
//			sensorEventQueue = NULL;
//			android_touch = new AndroidTouch;
//			destroyed = false;
//#endif
//
//			render = NULL;
//			sysentry = NULL;
//			scene = NULL;
//			meshMgr = NULL;
//			camera = NULL;
//			renderwindow = NULL;
//			gp = NULL;
////#ifndef ANDROID
////			android_touch = NULL;
////#endif
//			
//		}
//
//		virtual void createEGLDisplay()
//		{
//#ifdef ANDROID
//				destroyed = false;
//#endif
//		
//			Logger::WriteLog("Create EGLDisplay()");
//			if (NULL == render)
//			{
//				auto eglrender = new EGLRenderSystem;
//				render = eglrender;
//#ifdef ANDROID
//				eglrender->app = app;
//#endif
//				render->SetWandH(1200,900);
//				
//			
//			}
//			render->Initialize();
//			render->GetWandH(w,h);
//		}
//		virtual void createEGLDisplay_for_mobileEnergyTest(int width,int height)
//		{
//#ifdef ANDROID
//			destroyed = false;
//#endif
//
//			Logger::WriteLog("Create EGLDisplay()");
//			if (NULL == render)
//			{
//				auto eglrender = new EGLRenderSystem;
//				render = eglrender;
//#ifdef ANDROID
//				eglrender->app = app;
//#endif
//				render->SetWandH(width, height);
//
//
//			}
//			render->InitFactory();
//			render->GetWandH(w, h);
//		}
//		virtual void createDX11Display(){
//			//Logger::WriteLog("Create DX11Display().\n");
//			//if (NULL == render){
//			//	render = new DX11RenderSystem();
//			//	render->SetWandH(1024, 768);
//			//}
//			//render->Initialize();
//			//render->GetWandH(w, h);
//		}
//
//		virtual void InitScene() = 0;
//		void Go()
//		{
//#ifdef ANDROID
//			if(NULL == app)
//				return ;
//#else
//			MSG msg;
//#endif
//			while (true)
//			{
//#ifdef ANDROID
//				//	LOGI("in the while 1");
//				int ident;
//				int events;
//				struct android_poll_source * source;
//
//				do
//				{
//					ident = ALooper_pollAll(0,NULL,&events,(void**)&source);
//					if(source != NULL)
//					{
//						//	LOGE("enter source process ident = %d",ident);
//						source->process(app,source);
//					}
//
//					//	printf("in the while poll");
//					if (LOOPER_ID_USER == ident)
//					{
//						static float time;
//						ASensorEvent event;
//						acceleration = Vector3();
//						int counter = 0;
//						while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
//							float delta = event.timestamp - time;
//							//LOGE("delta: %f", delta);
//							acceleration += Vector3(event.acceleration.x, event.acceleration.y, event.acceleration.z);
//							++counter;
//							time = event.timestamp;
//						}
//						acceleration /= counter;
//					}
//
//					if(app->destroyRequested != 0)
//					{
//
//
//						return;
//					}
//				}while(ident>=0 );
//
//				if(!destroyed)
//				{
//					if(android_touch)
//						android_touch->everyFrameProcessed();
//					Draw();
//					if(android_touch)
//						android_touch->ClearAction();
//
//				}
//#else
//				if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
//				{
//					if(msg.message == WM_QUIT)
//						break;
//					else
//					{
//						TranslateMessage(&msg);
//						DispatchMessage(&msg);
//					}
//				}else
//				{
//					static unsigned long frame = 0;
//					Draw();
//				}
//#endif
//			}
//			
//		}
//		void term_display()
//		{
//			Logger::WriteLog(" app term display");
//			if(NULL != render)
//			{
//				//shaderMgr->releaseInternalRes();
//				//Logger::WriteLog("shaderMgr release");
//				meshMgr->releaseInternalRes();
//				Logger::WriteLog("meshMgr release");
//				scene->releaseInternalRes();
//				Logger::WriteLog("sceneMgr release");
//				render->term_display();
//				Logger::WriteLog(" egl context release");
//
//#ifdef ANDROID
//				destroyed = true;
//#endif
//		
//			}
//		}
//		void destroy()
//		{
//			term_display();
//
//			delete meshMgr;
//			Logger::WriteLog("MeshManager delete");
//			delete scene;
//			Logger::WriteLog("SceneManager delete");
//			delete gp;
//			Logger::WriteLog("complete destroy");
//		}
//
//		~AppFrameWork()
//		{
//			destroy();
//			Logger::close();
//		//	out.close();
//		}
//
//#ifdef ANDROID
//		void ConfigChanged()
//		{
//			// it shoud be more easy to get the egl w and h;
//			// but the GetWandHEGL is not right on galaxy s4
//			// it seems that this situation happens on different phone
//			term_display();
//			createEGLDisplay();
//		//	Logger::WriteAndroidInfoLog("OUT Create EGLDisplay");
//			render->GetWandH(w,h);
//		//	Logger::WriteAndroidInfoLog("OUT Get W and H");
//			save.width =w;
//			save.height = h;
//		//	Logger::WriteAndroidInfoLog("Save Set %d",camera);
//
//			float fov = camera->getFov();
//			float near = camera->getNear();
//			float far = camera->getFar();
//			//Logger::WriteAndroidInfoLog("set Perspective");
//			camera->setPerspective(fov,w/(float)h,near,far);
//			// be Attention about this, in the android situation .when the phone is potrait or lanscape
//			// the with and height of display is changed .
//			// so we should change the renderttexture that has the same size of display .
//			// for temporaily the only texture need be changed is the rendertarget store in scenemgr's m_rendertargetlist.
//			// these target are supposed has the same dimension of display.
//			//Logger::WriteAndroidInfoLog("Enter change RT Size");
//			scene->changeRenderTargetSize(w,h);
//
//		}
//#endif
//
//	protected:
//		virtual void Draw() = 0;
//		void fireFrameListener()
//		{
//			for(std::vector<FrameListener *>::iterator it = framelistener_list.begin();it != framelistener_list.end();it++)
//			{
//				(*it)->frameStarted();
//			}
//		}
//		void unloadFrameListener()
//		{
//			for(std::vector<FrameListener *>::iterator it = framelistener_list.begin();it != framelistener_list.end();it++)
//			{
//				if((*it) != NULL)
//					(*it)->frameEnded();
//				else
//					Logger::WriteLog("FrameListener NuLL");
//			}
//		}
//		virtual void calculatedFPS()
//		{
//
//
//
//		}
//
//	public:
//		GlobalVariablePool * gp;
//
//
//		SceneManager * scene;
//		MeshManager * meshMgr;
//		Camera * camera;
//		SysEntry * sysentry;
//		RenderTarget * renderwindow;
//		std::vector<FrameListener *> framelistener_list;
//		Timer timer;
//		int w;
//		int h;
//
//	public:
//		Vector3 acceleration;
//		protected:
//
//	
//#ifdef ANDROID
//		bool destroyed;
//#endif
//	public:
//		RenderSystem * render;
//		//AndroidTouch * android_touch;
//#ifdef ANDROID
//		struct android_app * app;
//		ASensorManager * sensorManager;
//		const ASensor* accelerometerSensor;
//		ASensorEventQueue* sensorEventQueue;
//		saved_state save;
//		
//#endif
//		
//	};
//}
//
//#endif
