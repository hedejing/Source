#define  _WINSOCKAPI_
#include "App_SSS.h"
#include "app_PBR.h"
#include "app_edushi.h"
#include "app_forward.h"
#include "app_TAA.h"
#include "app_VR_Experience.h"
//#include "app_VR.h"
#include "GLFWFramework.h"
//#include "edushiKey.h"
//#include "connection.h"
//#pragma comment(lib,"LibConnection.lib")
//#include "socketSceneLoader/SceneLoader.h"

std::function< void (int,int)> f;

void main() {
	GLFWFrameWork fw;
	auto app = new App_SSS;
	app->use_huawei_head = false;
	app->head_index = 5;

	auto app2 = new App_PBR;

//	auto app3 = new App_VR;
	
	auto app4 = new App_edushi;

	auto app5 = new App_Forward;

	auto app6 = new App_TAA;
	fw.app = new App_VR_Experience;
	fw.Init();

	//edushi_key ek;
	//ek.app = app4;
	//f = std::bind(&edushi_key::SwitchLogCam, ek, std::placeholders::_1, std::placeholders::_2);
	//InteractionControler::KeyCallBacks.push_back(f);

	fw.Loop();

	return;
}

