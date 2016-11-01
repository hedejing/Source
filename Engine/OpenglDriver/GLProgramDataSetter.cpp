#include "GLProgramDataSetter.h"
#include "gl_program.h"
#include "Texture.h"
#include "GlobalVariablePool.h"
#include "Logger.h"
#include "EGLUtil.h"


void GLProgramDataSetter::SetProgramTexture(const string &name, Texture* texture, int index)
{
	int location = program->getUniformLocation(name.c_str());
	if (location != -1)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		if (texture == NULL)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
			texture->useTexture();
		Sampler* s=program->getSampler(name);
		if (s != NULL){
			s->bind(index);
		}
		else{
			texture->texture_sampler->bind(index);
		}
		GLenum errorno = glGetError();
		if (errorno != 0)
		{
			Logger::WriteErrorLog("Program %d %s %s failed updateProgramData %d", 0, name.c_str(), name.c_str(), errorno);
		}
		glUniform1i(location, index);
		GL_CHECK();
	}
	else
	{
		//Logger::WriteErrorLog("cant bind texture %s,%s", program->GetName().c_str(), name.c_str());
	}
}

void GLProgramDataSetter::UpdateGlobalVariable(const map<unsigned int, string> &uniformMap)
{
	int tmp_textureCount=program->texture_count;
	auto glob = GlobalVariablePool::GetSingletonPtr();
	for(auto i = uniformMap.begin(); i != uniformMap.end(); i++)
	{
		GlobalVariableSemantic key = _global_getSemanticKey(i->first);
		unsigned int element_num = _global_getSemanticNum(i->first);
		string name = i->second;
		switch (key)
		{
			// matrix type
		case GVS_WORLD_MATRIX:
		case GVS_VIEW_MATRIX:
		case GVS_PROJECTION_MATRIX:
		case GVS_WORLD_VIEW_MATRIX:
		case GVS_VIEW_PROJECTION_MATRIX:
		case GVS_WORLD_VIEW_PROJECTION_MATRIX:
		case GVS_TRANSPOSE_WORLD_MATRIX:
		case GVS_TRANSPOSE_VIEW_MATRIX:
		case GVS_TRANSPOSE_PROJECTION_MATRIX:
		case GVS_TRANSPOSE_WORLD_VIEW_MATRIX:
		case GVS_TRANSPOSE_VIEW_PROJECTION_MATRIX:
		case GVS_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX:
		case GVS_INVERSE_WORLD_MATRIX:
		case GVS_INVERSE_VIEW_MATRIX:
		case GVS_INVERSE_PROJECTION_MATRIX:
		case GVS_INVERSE_WORLD_VIEW_MATRIX:
		case GVS_INVERSE_VIEW_PROJECTION_MATRIX:
		case GVS_INVERSE_WORLD_VIEW_PROJECTION_MATRIX:
		case GVS_SHADOW_MATRIX:
		{
			Matrix4 temp;
			glob->GetVal(i->first, sizeof(Matrix4), const_cast<float*>(&temp[0][0]));
			temp = temp.transpose();
			program->setUniform(name.c_str(), temp);
			break;
		}	
		case GVS_BONE_MATRICES:
		{
			int location = program->getUniformLocation(i->second.c_str());
			if (location != -1)
			{
				GLint size[1];
				GLenum type[1];
				element_num = 100;
				std::vector<Matrix4> val(element_num, Matrix4::IDENTITY);
				glob->GetVal(i->first, sizeof(Matrix4), &(val[0][0][0]), element_num);
				glUniformMatrix4fv(location, element_num, 0, &(val[0][0][0]));
			}

		}break;
		// vector type
		case GVS_MATERIAL_AMBIENT:
		case GVS_MATERIAL_DIFFUSE:
		case GVS_MATERIAL_SPECULAR:
		case GVS_MATERIAL_EMISSIVE:
		case GVS_MATERIAL_TRANSPARENT:
		case GVS_DIRECTIONAL_LIGHT_DIFFUSE:
		case GVS_DIRECTIONAL_LIGHT_SPECULAR:
		case GVS_POINT_LIGHT_DIFFUSE:
		case GVS_POINT_LIGHT_SPECULAR:
		case GVS_SPOT_LIGHT_DIFFUSE:
		case GVS_SPOT_LIGHT_SPECULAR:
		{
			float data[sizeof(Vector4) / sizeof(float)];
			glob->GetVal(GlobalVariableSemantic(i->first), sizeof(Vector4), data);
			UpdateConstantVariable(name,"vec4",sizeof(Vector4), data);
		}
			break;
		case GVS_DIRECTIONAL_LIGHT_DIRECTION:
		case GVS_POINT_LIGHT_POSITION:
		case GVS_SPOT_LIGHT_POSITION:
		case GVS_SPOT_LIGHT_DIRECTION:
		case GVS_CAMERA_POSITION:
		case GVS_CAMERA_DIRECTION:
		case GVS_CAMERA_UP:
		{
			float data[sizeof(Vector3) / sizeof(float)];
			glob->GetVal(GlobalVariableSemantic(i->first), sizeof(Vector3), data);
			UpdateConstantVariable(name, "vec3",sizeof(Vector3), data);
		}break;
		case GVS_MATERIAL_OPACITY:
		case GVS_MATERIAL_SHININESS:
		case GVS_POINT_LIGHT_CONST_ATT:
		case GVS_POINT_LIGHT_LINEAR_ATT:
		case GVS_POINT_LIGHT_QUAD_ATT:
		case GVS_SPOT_LIGHT_CONST_ATT:
		case GVS_SPOT_LIGHT_LINEAR_ATT:
		case GVS_SPOT_LIGHT_QUAD_ATT:
		case GVS_SPOT_LIGHT_EXP:
		case GVS_SPOT_LIGHT_CUTOFF:
		case GVS_POINT_LIGHT_DISTANCE:
		case GVS_SPOT_LIGHT_DISTANCE:
		case GVS_SYSTEM_ELAPSE_TIME:
		case GVS_SYSTEM_DELTA_TIME:
		case GVS_INTER_FACTOR:
		case GVS_SHADOW_BIAS:
		{
			float data[1];
			glob->GetVal(i->first, sizeof(float), data);
			UpdateConstantVariable(name, "float",sizeof(float), data);
		}
		break;
		case GVS_FRAME_ANIMATION:
		case GVS_HAS_TEXTURE_DIFFUSE:
		case GVS_ACTIVATE_LIGHTNUM:
		{
			int data[1];
			glob->GetVal(i->first, sizeof(int), data);
			UpdateConstantVariable(name, "int",sizeof(int), data);
		}break;
		case GVS_TEXTURE_DIFFUSE:
		case GVS_TEXTURE_AMBIENT:
		case GVS_TEXTURE_SPECULAR:
		case GVS_TEXTURE_HEIGHT:
		case GVS_SHADOW_MAP:
		case GVS_TEXTURE_NORMAL:
		case GVS_POST_TEXTURE:
		{
			
			Texture * texture[1] = { NULL };
			glob->GetVal(i->first, sizeof(Texture *), texture);
			if (texture[0] == NULL){
				//string s = "texture null :" + name;
				//LOG(s.c_str());
				break;
			}
			tmp_textureCount++;
			SetProgramTexture(i->second, texture[0], tmp_textureCount);
		}break;
		default:;
		}
	}

	// update manual data;

	//updateManualProgramData(program->m_ProgramData);
}

void GLProgramDataSetter::UpdateConstantVariable(const string &name, string datatype,unsigned int size, void* val)
{
	int location = program->getUniformLocation(name.c_str());
	if (location == -1) return;


	if (datatype == "mat4")
	{
		int count = size / 64;
		vector<Matrix4> Matrices(count);
		for (unsigned int i = 0; i < Matrices.size(); i++)
		{
			Matrices[i] = ((Matrix4*)val)[i].transpose();
		}
		glUniformMatrix4fv(location, count, 0, (GLfloat *)&Matrices[0][0][0]);
	}
	if (datatype == "vec4")
	{
		int count = size / 16;
		glUniform4fv(location, count, (GLfloat *)val);
		return;
	}
	if (datatype == "ivec4")
	{
		int count = size / 16;
		glUniform4iv(location, count, (GLint *)val);
		return;
	}
	if (datatype == "vec3")
	{
		int count = size / 12;
		//test
		//vector<float> v;
		//for (int i = 0; i < 32; i++){
		//	v.push_back(((GLfloat *)val)[i]);
		//}
		glUniform3fv(location, count, (GLfloat *)val);
		return;
	}
	if (datatype == "ivec3")
	{
		int count = size / 12;
		glUniform3iv(location, count, (GLint *)val);
		return;
	}
	if (datatype == "vec2")
	{
		int count = size / 8;
		glUniform2fv(location, count, (GLfloat *)val);
		return;
	}
	if (datatype == "ivec2")
	{
		int count = size / 8;
		glUniform2iv(location, count, (GLint *)val);
		return;
	}
	if (datatype == "float")
	{
		int count = size / 4;
		glUniform1fv(location, count, (GLfloat *)val);
		return;
	}
	if (datatype == "int")
	{
		int count = size / 4;
		glUniform1iv(location, count, (GLint *)val);
		return;
	}

}

void GLProgramDataSetter::updateManualProgramData(const map<string, ProgramData> & program_data)
{
	for (std::map<string, ProgramData>::const_iterator itr = program_data.begin(); itr != program_data.end(); itr++)
	{
		const ProgramData & pd = itr->second;
		if (pd.type == SDT_TEXTURE)
		{
			program->texture_count++;
			Texture * texture = (Texture *)pd.data;
			SetProgramTexture(itr->first, texture, program->texture_count);
		}
		else if (pd.type == SDT_CONSTANT)
		{
			UpdateConstantVariable(itr->first, pd.datetype,pd.size, pd.data);
		}
	}
}
