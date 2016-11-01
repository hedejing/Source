#pragma once

#include "Texture.h"
#include "NameGenerator.h"
#include "ResourceManager.h"
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
using namespace rapidjson;
using namespace HW;
using namespace std;


class TextureManager :public ResourceManager<Texture*>{
public:
	static TextureManager& getInstance()
	{
		static TextureManager    instance;
		return instance;
	}

	Texture* CreateTextureFromImage(string path,string texname="");
	void LoadTexturePreset(string filename);
	Texture* CreateTextureFromPreset(string preset_name);
	Texture* CreateTextureCubeMap(vector<string>& paths, string texname = "");
	Texture* CreateTextureCubeMapEmpty(string texname);
	
private:
	rapidjson::Document m_presets_doc;
	map<string, rapidjson::Value> presets;
	TextureManager();
	TextureManager(TextureManager const&);              // Don't Implement.
	void operator=(TextureManager const&); // Don't implement
	NameGenerator* namegen;
};