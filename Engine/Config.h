#pragma once
#include "mathlib.h"
#include <fstream>
#include <string>
using namespace std;
using namespace HW;


class Config
{
public:
	void Load() {
#ifdef ANDROID
		configs = ifstream("sdcard/OpenGLES/renderconfig.txt");
#else
		configs = ifstream("renderconfig.txt");
#endif
		if (configs.is_open()) {
			string name;
			configs >> name >> b_drawbox;
			configs >> name >> ce.x >> ce.y >> ce.z;
			configs >> name >> hl.x >> hl.y >> hl.z;
			configs >> name >> b_drawText;
			configs >> name >> width;
			configs >> name >> height;
			configs >> name >> fov;
			configs >> name >> b_profile;
			configs >> name >> b_logCamera;
			configs >> name >> b_textpos;
			configs >> name >> camerapos.x >> camerapos.y >> camerapos.z;
			configs >> name >> cameradir.x >> cameradir.y >> cameradir.z;
			configs >> name >> mobile_suqare_res;
		}
		valid = true;
	}

	int mobile_suqare_res = 0;
	Vector3 camerapos;
	Vector3 cameradir = Vector3(0, 0, -1);
	int b_textpos = 0;
	int b_logCamera = 0;
	bool valid = 0;
	bool b_drawbox = 0;
	bool b_drawText = 0;
	int width = 1024;
	int height = 1024;
	float fov = 45;
	int b_profile = 0;
	Vector3 ce;
	Vector3 hl;

	ifstream configs;


	//µ¥Àýº¯Êýfor c++ 11
	static Config& instance()
	{
		static Config *instance = new Config();
		return *instance;
	}

private:
	Config() {}
};