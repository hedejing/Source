#pragma once
#include<string>
#include "SharedPointer.h"
#include "NameGenerator.h"
#include <map>
#include <vector>
#include "PreDefine.h"
#ifdef ANDROID
#include <android/log.h>
#endif


using namespace std;
namespace HW
{

	enum ShaderType
	{
		ST_VERTEX = 0,
		ST_DOMAIN = 1,
		ST_HULL = 2,
		ST_GEOMETRY = 3,
		ST_PIXEL = 4,
		ST_SHADER_TYPE_COUNT
	};

	enum ShaderDataType
	{
		SDT_CONSTANT = 0x0,
		SDT_TEXTURE,
	};
	struct ProgramData{

		void * data;
		string datetype;
		unsigned int size;
		bool int_flag;
		ShaderDataType type;
		ProgramData()
		{
			data = NULL;
		}
	};

	class GpuProgram
	{
	public:
		
		GpuProgram();

		GpuProgram(const string &name);

		virtual ~GpuProgram();

		void SetName(const string &name) { m_Name = name; }

		const string& GetName() const { return m_Name; }
		/** notify the gpu program the render system it rely on.
		*/
		void NotifyRenderSystem(RenderSystem *render) { m_RenderSystem = render; }


		// compile and link program
		virtual bool  CompileAndLink()=0;
		virtual unsigned int GetErrorMessageSize()const{ return 0; };
		virtual void  GetErrorMessage(void* msg,unsigned int size){};
		virtual void  UseProgram()=0;


		//ÐÂ·½·¨
		virtual void SetProgramTexture(const string &name, Texture* texture, int index){}

		virtual void UpdateGlobalVariable(const map<unsigned int, string> &uniformMap){}
		
		/** update constant buffer used by this program at rendering time
		*/
		virtual void UpdateConstantVariable(const string &name, unsigned int size, void* val, bool int_flag) = 0;

		void SetSemanticVariableMap(const std::map<unsigned int ,string> &svmap) { m_SemanticVariableMap = svmap; }

		const std::map<unsigned int,string> & getSemanticVariableMap() const { return m_SemanticVariableMap; }

		// release internal resource
		virtual void releaseInternalRes() = 0;

		void setProgramConstantData(const string & name, const void * val, string datetype, unsigned int size, bool int_flag = false);

		void setProgramConstantData(const string & name,const Texture * val,bool int_flag = false);
		/** update the programData into program's uniform;
			@remarks the program_data could be the program defualt data in m_ProgramData,or pass by material's
			@param program_data is an map<string,ProgramData> which contain the name and the value of 
				uniform parameters;
		*/
		virtual void updateProgramData(const std::map<string, ProgramData> & program_data) = 0;

		//virtual void bindUBO(EGLBufferObject* ubo, const String& name){};
		void setSampler(string name,Sampler* s){
			m_SamplerMap[name] = s;
		}
		Sampler* getSampler(string name){
			auto it = m_SamplerMap.find(name);
			if (it != m_SamplerMap.end())
				return it->second;
			else
				return NULL;
		}
		void attachShader(ShaderType t,string path,string name){
			m_ShaderPath[t] = path;
			m_ShaderName[t] = name;
		}
		string getShaderPath(ShaderType t){
			return m_ShaderPath[t];
		}
		string getShaderName(ShaderType t){
			return m_ShaderName[t];
		}
		std::map<string, ProgramData> m_ProgramData;
		int texture_count = 0;
	protected:
		
		RenderSystem *m_RenderSystem;

		string m_Name;

		vector<string> m_ShaderPath=vector<string>(5,"");
		vector<string> m_ShaderName = vector<string>(5, "");

		static NameGenerator *m_NameGenerator;

		map<unsigned int,string> m_SemanticVariableMap;
		map<string, Sampler*> m_SamplerMap;
		bool m_valid;
	};

	
	typedef SharedPointer<GpuProgram> GpuProgramPtr;

	// gpu factory
	class GpuProgramFactory
	{
	public:
		// create with a name 
		GpuProgramFactory(){}
		virtual ~GpuProgramFactory(){}
		virtual GpuProgram* CreateGpuProgram(const string& name) = 0;
		// not used.
		virtual GpuProgram* CreateGpuProgram() = 0;
	};

}
