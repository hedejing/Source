#pragma once
#include "OPENGL_Include.h"
#include "../mathlib.h"
#include <stdexcept>
#include <map>
#include "GpuProgram.h"
#include "PreDefine.h"
#include "ShaderProcessor.h"
using namespace HW;
using namespace std;


namespace GLSLShader {
	enum GLSLShaderType {
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		//GEOMETRY = GL_GEOMETRY_SHADER,
		//TESS_CONTROL = GL_TESS_CONTROL_SHADER,
		//TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
		//COMPUTE = GL_COMPUTE_SHADER
	};
};

class GLGpuProgram:public GpuProgram
{
public:
	//继承而来的抽象方法
	virtual void SetProgramTexture(const string &name, Texture* texture, int index);
	virtual bool  CompileAndLink();
	virtual void  UseProgram();
	virtual void UpdateGlobalVariable(const map<unsigned int, string> &uniformMap);
	virtual void updateProgramData(const std::map<string, ProgramData> & program_data);
	virtual void releaseInternalRes(){}
	virtual void UpdateConstantVariable(const string &name, unsigned int size, void* val, bool int_flag);

	static ShaderProsscer GShaderProsscer;
	//自有方法
	GLProgramDataSetter* setter;
	int  programID;
	bool linked;
	
	std::map<string, int> uniformLocations;

	GLint  getUniformLocation(const char * name);
	bool fileExists(const string & fileName);
	string getExtension(const char * fileName);

	// Make these private in order to make the object non-copyable
	GLGpuProgram(const GLGpuProgram & other) { }
	GLGpuProgram & operator=(const GLGpuProgram &other) { return *this; }


	GLGpuProgram();
	~GLGpuProgram();

	void   compileShader(const char *fileName);
	void   compileShader(const char * fileName, GLSLShader::GLSLShaderType type);
	void   compileShader(const string & source, GLSLShader::GLSLShaderType type,
		const char *fileName = NULL);

	void   link();
	void   validate();
    void   use();

	int    getHandle();
	bool   isLinked();

	//not used
	void   bindAttribLocation(GLuint location, const char * name);
	//void   bindFragDataLocation(GLuint location, const char * name);

	//important use
	void   setUniform(const char *name, float x, float y, float z);
	void   setUniform(const char *name, const Vector2 & v);
	void   setUniform(const char *name, const Vector3 & v);
	void   setUniform(const char *name, const Vector4 & v);
	void   setUniform(const char *name, const Matrix4 & m);
	void   setUniform(const char *name, const Matrix3 & m);
	void   setUniform(const char *name, float val);
	void   setUniform(const char *name, int val);
	void   setUniform(const char *name, bool val);
	void   setUniform(const char *name, GLuint val);


	//not available
	void   printActiveUniforms();
	void   printActiveUniformBlocks();
	void   printActiveAttribs();

	const char * getTypeString(GLenum type);
};
class GLGpuProgramFactory : public GpuProgramFactory
{
public:
	GLGpuProgramFactory() :GpuProgramFactory(){}
	virtual ~GLGpuProgramFactory(){}
	virtual GpuProgram* CreateGpuProgram(const string& program_name)
	{
		GpuProgram* p = new GLGpuProgram();
		p->SetName(program_name);
		return p;
	}
	virtual GpuProgram* CreateGpuProgram(){ return new GLGpuProgram(); }
};