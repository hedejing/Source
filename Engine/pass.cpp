#include "pass.h"
#include "engine_struct.h"
#include "VertexDataDesc.h"
#include "EngineUtil.h"
#include "GlobalVariableSemantics.h"
#include "GlobalVariablePool.h"
#include "GpuProgram.h"
#include "ResourceManager.h"

void ParseAttributeFromJson(rapidjson::Value& attributes, VertexDataDesc* mInputLayout)
{
	mInputLayout->clear();
	for (auto it = attributes.MemberBegin(); it != attributes.MemberEnd(); it++){
		VertexElement* att = new VertexElement;
		string varName = it->name.GetString();
		auto& v = it->value;

		att->name = varName;
		att->type = v["type"].GetString();
		att->semantic = v["semantic"].GetString();
		att->index = v["index"].GetInt();

		mInputLayout->add(att);
	}
}



void ParseUniformInputFromJson(rapidjson::Value& uniformmap, as_Pass* pass)
{
	map<unsigned int, string>& mSemanticVariableMap = pass->mUniformMap;
	std::set<string>&  mShaderManualData = pass->mManualData;
	string varName, semanticName;
	unsigned int elment_index = 1;
	for (auto it = uniformmap.MemberBegin(); it != uniformmap.MemberEnd(); it++){
		auto& q = it->value;
		varName = it->name.GetString();
		if (q.GetType() != Type::kObjectType){
			semanticName = it->value.GetString();
		}
		else{
			auto p = q.FindMember("index");
			if (p != q.MemberEnd()){
				semanticName = q["semantic"].GetString();
				elment_index = q["index"].GetInt();
			}
		}
		auto& stringMap = PassManager::getInstance().m_StringSemanticMap;
		if (semanticName == "GVS_MANUAL_PARA")
			mShaderManualData.insert(varName);
		else{

			std::map<string, unsigned int>::iterator it2 = stringMap.find(semanticName);
			if (it2 != stringMap.end())
			{
				mSemanticVariableMap.insert(std::make_pair(_global_makeFromkeyIndexandNum(GlobalVariableSemantic(it2->second), 0, elment_index - 1), varName));
			}
			else
			{
				Logger::WriteErrorLog("semantic %s not found!", semanticName.c_str());
			}
		}
	}
}

as_Pass* ParsePassFromJsonFile(string filename)
{

	string file_content = ReadFiletoString(filename);

	rapidjson::Document d;
	d.Parse(file_content.c_str());

	as_Pass* pass = new as_Pass;
	pass->name = d["name"].GetString();
	auto& grm = GlobalResourceManager::getInstance();
	rapidjson::Value shader_desc;
	if (grm.m_platform_info=="GL") 
		shader_desc = d["GLShader"];
	else
		shader_desc = d["DXShader"]; 
	GpuProgram* program = grm.m_GpuProgramFactory->CreateGpuProgram();
	pass->mProgram = program;
	//string vsName = (shader_desc["vertex shader"]["name"].GetString());
	//string fsName = (shader_desc["fragment shader"]["name"].GetString());
	string vsPath = (shader_desc["vertex shader"]["path"].GetString());
	string fsPath = (shader_desc["fragment shader"]["path"].GetString());

	program->attachShader(ShaderType::ST_VERTEX, vsPath, vsPath);
	
	program->attachShader(ShaderType::ST_PIXEL, fsPath, fsPath);
	
	program->CompileAndLink();
	
	//获取attribute

	ParseAttributeFromJson(d["input"]["attribute"], pass->mInputLayout);

	//获取uniform map
	ParseUniformInputFromJson(d["input"]["uniform"], pass);

	program->SetSemanticVariableMap(pass->mUniformMap);
	return pass;
}



as_Pass* PassManager::LoadPass(string path)
{
	as_Pass* p = ParsePassFromJsonFile(path);
	add(p);
	return p;
}

void PassManager::initStringSemanticMap()
{
	m_StringSemanticMap.insert(std::make_pair("GVS_WORLD_MATRIX", GVS_WORLD_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_VIEW_MATRIX", GVS_VIEW_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_PROJECTION_MATRIX", GVS_PROJECTION_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_WORLD_VIEW_MATRIX", GVS_WORLD_VIEW_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_VIEW_PROJECTION_MATRIX", GVS_VIEW_PROJECTION_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_WORLD_VIEW_PROJECTION_MATRIX", GVS_WORLD_VIEW_PROJECTION_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_TRANSPOSE_WORLD_MATRIX", GVS_TRANSPOSE_WORLD_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_TRANSPOSE_VIEW_MATRIX", GVS_TRANSPOSE_VIEW_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_TRANSPOSE_PROJECTION_MATRIX", GVS_TRANSPOSE_PROJECTION_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_TRANSPOSE_WORLD_VIEW_MATRIX", GVS_TRANSPOSE_WORLD_VIEW_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_TRANSPOSE_VIEW_PROJECTION_MATRIX", GVS_TRANSPOSE_VIEW_PROJECTION_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX", GVS_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_INVERSE_WORLD_MATRIX", GVS_INVERSE_WORLD_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_INVERSE_VIEW_MATRIX", GVS_INVERSE_VIEW_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_INVERSE_PROJECTION_MATRIX", GVS_INVERSE_PROJECTION_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_INVERSE_WORLD_VIEW_MATRIX", GVS_INVERSE_WORLD_VIEW_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_INVERSE_VIEW_PROJECTION_MATRIX", GVS_INVERSE_VIEW_PROJECTION_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_INVERSE_WORLD_VIEW_PROJECTION_MATRIX", GVS_INVERSE_WORLD_VIEW_PROJECTION_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_MATERIAL_AMBIENT", GVS_MATERIAL_AMBIENT));
	m_StringSemanticMap.insert(std::make_pair("GVS_MATERIAL_DIFFUSE", GVS_MATERIAL_DIFFUSE));
	m_StringSemanticMap.insert(std::make_pair("GVS_MATERIAL_SPECULAR", GVS_MATERIAL_SPECULAR));
	m_StringSemanticMap.insert(std::make_pair("GVS_MATERIAL_EMISSIVE", GVS_MATERIAL_EMISSIVE));
	m_StringSemanticMap.insert(std::make_pair("GVS_MATERIAL_TRANSPARENT", GVS_MATERIAL_TRANSPARENT));
	m_StringSemanticMap.insert(std::make_pair("GVS_MATERIAL_OPACITY", GVS_MATERIAL_OPACITY));
	m_StringSemanticMap.insert(std::make_pair("GVS_MATERIAL_SHININESS", GVS_MATERIAL_SHININESS));
	m_StringSemanticMap.insert(std::make_pair("GVS_DIRECTIONAL_LIGHT_DIRECTION", GVS_DIRECTIONAL_LIGHT_DIRECTION));
	m_StringSemanticMap.insert(std::make_pair("GVS_DIRECTIONAL_LIGHT_DIFFUSE", GVS_DIRECTIONAL_LIGHT_DIFFUSE));
	m_StringSemanticMap.insert(std::make_pair("GVS_DIRECTIONAL_LIGHT_SPECULAR", GVS_DIRECTIONAL_LIGHT_SPECULAR));
	m_StringSemanticMap.insert(std::make_pair("GVS_POINT_LIGHT_POSITION", GVS_POINT_LIGHT_POSITION));
	m_StringSemanticMap.insert(std::make_pair("GVS_POINT_LIGHT_DIFFUSE", GVS_POINT_LIGHT_DIFFUSE));
	m_StringSemanticMap.insert(std::make_pair("GVS_POINT_LIGHT_SPECULAR", GVS_POINT_LIGHT_SPECULAR));
	m_StringSemanticMap.insert(std::make_pair("GVS_POINT_LIGHT_CONST_ATT", GVS_POINT_LIGHT_CONST_ATT));
	m_StringSemanticMap.insert(std::make_pair("GVS_POINT_LIGHT_LINEAR_ATT", GVS_POINT_LIGHT_LINEAR_ATT));
	m_StringSemanticMap.insert(std::make_pair("GVS_POINT_LIGHT_QUAD_ATT", GVS_POINT_LIGHT_QUAD_ATT));
	m_StringSemanticMap.insert(std::make_pair("GVS_SPOT_LIGHT_POSITION", GVS_SPOT_LIGHT_POSITION));
	m_StringSemanticMap.insert(std::make_pair("GVS_SPOT_LIGHT_DIRECTION", GVS_SPOT_LIGHT_DIRECTION));
	m_StringSemanticMap.insert(std::make_pair("GVS_SPOT_LIGHT_DIFFUSE", GVS_SPOT_LIGHT_DIFFUSE));
	m_StringSemanticMap.insert(std::make_pair("GVS_SPOT_LIGHT_SPECULAR", GVS_SPOT_LIGHT_SPECULAR));
	m_StringSemanticMap.insert(std::make_pair("GVS_SPOT_LIGHT_CONST_ATT", GVS_SPOT_LIGHT_CONST_ATT));
	m_StringSemanticMap.insert(std::make_pair("GVS_SPOT_LIGHT_LINEAR_ATT", GVS_SPOT_LIGHT_LINEAR_ATT));
	m_StringSemanticMap.insert(std::make_pair("GVS_SPOT_LIGHT_QUAD_ATT", GVS_SPOT_LIGHT_QUAD_ATT));
	m_StringSemanticMap.insert(std::make_pair("GVS_SPOT_LIGHT_EXP", GVS_SPOT_LIGHT_EXP));
	m_StringSemanticMap.insert(std::make_pair("GVS_SPOT_LIGHT_CUTOFF", GVS_SPOT_LIGHT_CUTOFF));
	m_StringSemanticMap.insert(std::make_pair("GVS_POINT_LIGHT_DISTANCE", GVS_POINT_LIGHT_DISTANCE));
	m_StringSemanticMap.insert(std::make_pair("GVS_SPOT_LIGHT_DISTANCE", GVS_SPOT_LIGHT_DISTANCE));
	m_StringSemanticMap.insert(std::make_pair("GVS_TEXTURE_OPACITY", GVS_TEXTURE_OPACITY));
	m_StringSemanticMap.insert(std::make_pair("GVS_TEXTURE_DIFFUSE", GVS_TEXTURE_DIFFUSE));
	m_StringSemanticMap.insert(std::make_pair("GVS_TEXTURE_AMBIENT", GVS_TEXTURE_AMBIENT));
	m_StringSemanticMap.insert(std::make_pair("GVS_TEXTURE_SPECULAR", GVS_TEXTURE_SPECULAR));
	m_StringSemanticMap.insert(std::make_pair("GVS_TEXTURE_EMISSIVE", GVS_TEXTURE_EMISSIVE));
	m_StringSemanticMap.insert(std::make_pair("GVS_TEXTURE_HEIGHT", GVS_TEXTURE_HEIGHT));
	m_StringSemanticMap.insert(std::make_pair("GVS_TEXTURE_NORMAL", GVS_TEXTURE_NORMAL));
	m_StringSemanticMap.insert(std::make_pair("GVS_TEXTRUE_SHININESS", GVS_TEXTRUE_SHININESS));
	m_StringSemanticMap.insert(std::make_pair("GVS_TEXTURE_DISPLACEMENT", GVS_TEXTURE_DISPLACEMENT));
	m_StringSemanticMap.insert(std::make_pair("GVS_TEXTURE_LIGHTMAP", GVS_TEXTURE_LIGHTMAP));
	m_StringSemanticMap.insert(std::make_pair("GVS_TEXTURE_REFLECTION", GVS_TEXTURE_REFLECTION));
	m_StringSemanticMap.insert(std::make_pair("GVS_TEXTURE_UNKNOWN", GVS_TEXTURE_UNKNOWN));
	m_StringSemanticMap.insert(std::make_pair("GVS_SYSTEM_ELAPSE_TIME", GVS_SYSTEM_ELAPSE_TIME));
	m_StringSemanticMap.insert(std::make_pair("GVS_SYSTEM_DELTA_TIME", GVS_SYSTEM_DELTA_TIME));
	m_StringSemanticMap.insert(std::make_pair("GVS_CAMERA_POSITION", GVS_CAMERA_POSITION));
	m_StringSemanticMap.insert(std::make_pair("GVS_CAMERA_DIRECTION", GVS_CAMERA_DIRECTION));
	m_StringSemanticMap.insert(std::make_pair("GVS_CAMERA_UP", GVS_CAMERA_UP));
	m_StringSemanticMap.insert(std::make_pair("GVS_ENV_AMBIENT", GVS_ENV_AMBIENT));
	m_StringSemanticMap.insert(std::make_pair("GVS_SHADOW_MATRIX", GVS_SHADOW_MATRIX));
	m_StringSemanticMap.insert(std::make_pair("GVS_SHADOW_MAP", GVS_SHADOW_MAP));
	m_StringSemanticMap.insert(std::make_pair("GVS_FRAME_ANIMATION", GVS_FRAME_ANIMATION));
	m_StringSemanticMap.insert(std::make_pair("GVS_INTER_FACTOR", GVS_INTER_FACTOR));
	m_StringSemanticMap.insert(std::make_pair("GVS_SHADOW_BIAS", GVS_SHADOW_BIAS));
	m_StringSemanticMap.insert(std::make_pair("GVS_POST_TEXTURE", GVS_POST_TEXTURE));
	m_StringSemanticMap.insert(std::make_pair("GVS_BONE_MATRICES", GVS_BONE_MATRICES));
	m_StringSemanticMap.insert(std::make_pair("GVS_MANUAL_PARA", GVS_MANUAL_PARA));
	m_StringSemanticMap.insert(std::make_pair("GVS_ACTIVATE_LIGHTNUM", GVS_ACTIVATE_LIGHTNUM));
	m_StringSemanticMap.insert(std::make_pair("GVS_HAS_TEXTURE_DIFFUSE", GVS_HAS_TEXTURE_DIFFUSE));
	m_StringSemanticMap.insert(std::make_pair("GVS_DeferredShading_TEXTURE_POSITION", GVS_DeferredShading_TEXTURE_POSITION));
	m_StringSemanticMap.insert(std::make_pair("GVS_DeferredShading_TEXTURE_DIFFUSE", GVS_DeferredShading_TEXTURE_DIFFUSE));
	m_StringSemanticMap.insert(std::make_pair("GVS_DeferredShading_TEXTURE_NORMAL", GVS_DeferredShading_TEXTURE_NORMAL));
	m_StringSemanticMap.insert(std::make_pair("GVS_DeferredShading_TEXTURE_TEXCORD", GVS_DeferredShading_TEXTURE_TEXCORD));

}

void as_Pass::validateUniformMap()
{
}
