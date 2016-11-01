#include "GpuProgram.h"
#include <assert.h>
namespace HW
{
	NameGenerator *GpuProgram::m_NameGenerator = new NameGenerator("GpuProgram");

	GpuProgram::GpuProgram():m_RenderSystem(0)
	{
		m_valid = false;
		m_Name = m_NameGenerator->Generate();

	}

	GpuProgram::GpuProgram(const string &name):m_Name(name),m_RenderSystem(0)
	{
		m_valid = false;
	}

	//void GpuProgram::BindShader(Shader * shader)
	//{
	//	m_ShaderList[shader->getShaderType()] = shader;
	//}

	//Shader* GpuProgram::GetShader(ShaderType type)
	//{
	//	return m_ShaderList[type];
	//}

	GpuProgram::~GpuProgram()
	{
		m_valid = false;
		for (std::map<string,ProgramData>::iterator itr = m_ProgramData.begin();itr != m_ProgramData.end();itr++)
		{
			if(itr->second.type == SDT_CONSTANT)
				::operator delete(itr->second.data);
		}
	}

	void GpuProgram::setProgramConstantData(const string & name, const Texture * val, bool int_flag /*= false*/)
	{
		if (val == NULL) return;
		ProgramData pd;
		pd.type = SDT_TEXTURE;
		assert(val);
		pd.data = (void *)val;
		pd.int_flag = true;
		pd.size = 1;
		m_ProgramData[name] = pd;
	}

	void GpuProgram::setProgramConstantData(const string & name, const void * val, string datetype, unsigned int size, bool int_flag /*= false*/)
	{
		std::map<string, ProgramData>::iterator itr = m_ProgramData.find(name);
		if (itr != m_ProgramData.end())
		{
			ProgramData & pd = (itr->second);
			if (pd.size < size) {
				delete pd.data;
				pd.data = ::operator new(size);
			}
			memcpy(pd.data, val, size);
			pd.datetype = datetype;
			pd.int_flag = int_flag;
			pd.size = size;

		}
		else
		{
			ProgramData pd;

			pd.type = SDT_CONSTANT;
			assert(val && size);
			if (!pd.data)
				pd.data = ::operator new(size);
			memcpy(pd.data, val, size);
			pd.datetype = datetype;
			pd.int_flag = int_flag;
			pd.size = size;
			m_ProgramData[name] = pd;
		}
	}


}