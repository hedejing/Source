#include "TextureManager.h"
#include "FreeImage.h"
#include "EngineUtil.h"
#include "Image.h"
#include "Sampler.h"
TextureManager::TextureManager()
{
	namegen = new NameGenerator("texture");
	
	CreateTextureFromImage("white_texture.jpg");
}

Texture* TextureManager::CreateTextureFromImage(string filename, string texname)
{

	Image* image = ImageManager::getInstance().CreateImageFromFile(filename);

	if (image == NULL) return NULL;

	Texture* texture = GlobalResourceManager::getInstance().m_TextureFactory->create();
	texture->image = image;
	texture->width = image->width;
	texture->height = image->height;
	if (texname == "")
		texture->name = image->name;
	else
		texture->name = texname;

	texture->setFormatFromImage(image);
	add(texture);
	return texture;
}

void TextureManager::LoadTexturePreset(string filename)
{
	string file_content = ReadFiletoString(filename);

	m_presets_doc.Parse(file_content.c_str());
	auto& ps = m_presets_doc["Texture_Presets"];
	
	for (auto it = ps.MemberBegin(); it != ps.MemberEnd(); it++){

		auto& v = it->value;
		string name=v["preset_name"].GetString();
		presets[name] = v;
	}
}

Texture* TextureManager::CreateTextureFromPreset(string preset_name)
{
	auto it = presets.find(preset_name);
	if (it == presets.end())
		return NULL;
	;
	Texture* tex = GlobalResourceManager::getInstance().m_TextureFactory->create();

	auto& v = it->second;
	string type=v["type"].GetString();
	if (type == "2D") tex->texture_type = Texture::Texture_2D;
	if (type == "Cube") tex->texture_type = Texture::Texture_Cube;
	if (type == "3D") tex->texture_type = Texture::Texture_3D;

	type = v["format"].GetString();
	if (type == "RGBA") tex->texture_internal_format=Texture::RGBA8;
	if (type == "Depth24") tex->texture_internal_format = Texture::DEPTH24_STENCIL8;
	if (type == "RGBA32F") tex->texture_internal_format = Texture::RGBA32F;
	if (type == "RGBA16UI") tex->texture_internal_format = Texture::RGBA16UI;
	if (type == "RG32UI") tex->texture_internal_format = Texture::RG32UI;
	{
		string w = "sampler";
		if (v.HasMember(w.c_str())){
			type = v[w.c_str()].GetString();
			if (type == "ClampPoint") tex->texture_sampler = SamplerManager::getInstance().get("ClampPoint");
			if (type == "RepeatPoint") tex->texture_sampler = SamplerManager::getInstance().get("RepeatPoint");
			if (type == "RepeatLinear") tex->texture_sampler = SamplerManager::getInstance().get("RepeatLinear");
			if (type == "ClampLinear") tex->texture_sampler = SamplerManager::getInstance().get("ClampLinear");
		}
	}
	string w = "mipmap_mode";
	if (v.HasMember(w.c_str())){
			type = v[w.c_str()].GetString();
			if (type == "auto") tex->mipmap_mode = Texture::TextureMipmap_Auto;
			if (type == "none") tex->mipmap_mode = Texture::TextureMipmap_None;
			if (type == "manual") tex->mipmap_mode = Texture::TextureMipmap_Manual;
		}
	
	return tex;
}

Texture* TextureManager::CreateTextureCubeMap(vector<string>& paths, string texname /*= ""*/)
{

	//auto images = ImageManager::getInstance().CreateImageListFromFiles(paths);
	auto func = [](string& p) { 
		return ImageManager::getInstance().CreateImageFromFile(p,false);
	};
	vector<Image*> images;
	transform(paths.begin(),paths.end(),back_inserter(images), func);


	if (images.size()!=6) return NULL;

	Texture* texture = GlobalResourceManager::getInstance().m_TextureFactory->create();
	texture->texture_type = Texture::Texture_Cube;
	texture->image_cube = images;
	
	auto first_image = images[0];
	texture->width = first_image->width;
	texture->height = first_image->height;
	if (texname == "")
		texture->name = first_image->name;
	else
		texture->name = texname;

	texture->setFormatFromImage(first_image);

	add(texture);
	return texture;
}

Texture* TextureManager::CreateTextureCubeMapEmpty(string texname)
{
	Texture* texture = GlobalResourceManager::getInstance().m_TextureFactory->create();
	texture->texture_type = Texture::Texture_Cube;
	texture->name = texname;
	add(texture);
	return texture;
}

