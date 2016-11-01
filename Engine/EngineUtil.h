#pragma  once
#include<string>
#include "Logger.h"
#include <assert.h>
#include <set>
#include <vector>
#include <string>
using namespace std;
#ifdef ANDROID
#include <unistd.h>
#include <fcntl.h>
#define ShaderFilePath "sdcard/OpenGLES/"
#define SceneFilePath  "sdcard/OpenGLES/"
#else 
#include "io.h"
#define ShaderFilePath "shader/"
#define SceneFilePath  "maya_model/"
#endif

#define GetFullPath(x) FileUtil::getSceneFileValidPath(x);

namespace HW
{
	class FileUtil
	{
	public:
		static string getPureFileName(const string  & name)
		{
			
			auto pos1 = name.find_last_of('/');
			auto pos2 = name.find_last_of('\\');
			auto pos = pos1;

			if (pos2 != name.npos&&pos1 != name.npos&&pos2>pos1)
				pos = pos2;
			if (pos1 == name.npos&&pos2 != name.npos)
				pos = pos2;
			//if (pos2 > pos1) pos1 = pos2;

			return name.substr(pos +1);
		}
		static string getPathName(const string & name)
		{
			string::size_type pos = name.find_last_of('/');
			if(pos == name.npos)
			{
				pos = name.find_last_of('\\');
			}
			return name.substr(0,pos+1);
		}

		static string getShaderFileValidPath(const string & name)
		{
			if(fileValid(name))
			{
				valid_shader_path.insert(getPathName(name));
				return name;
			}
			if(fileValid(ShaderFilePath + name))
			{
				valid_shader_path.insert(ShaderFilePath+getPathName(name));
				return ShaderFilePath + name;
			}
			string pure_name = getPureFileName(name);
			if(fileValid(ShaderFilePath + pure_name))
			{
				return ShaderFilePath + pure_name;
			}

			for(std::set<string>::iterator itr = valid_shader_path.begin();itr != valid_shader_path.end();itr++)
			{
				if(fileValid(*itr +pure_name ))
				{
					return *itr+pure_name;
				}
				if(fileValid(ShaderFilePath + *itr+pure_name))
				{
					valid_shader_path.insert(ShaderFilePath + *itr);
					return ShaderFilePath + *itr+pure_name;
				}

			}

			Logger::WriteErrorLog("File %s Not Exist", name.c_str());
			return "";


		}

		static string getSceneFileValidPath(const string & _name)
		{
			auto name = _name;
			ReplaceAll(name, "\\", "/");
			if(fileValid(name))
			{
				valid_scene_path.insert(getPathName(name));
				return name;
			}
			if(fileValid(SceneFilePath + name))
			{
				valid_scene_path.insert(SceneFilePath+getPathName(name));
				return SceneFilePath + name;
			}
			string pure_name = getPureFileName(name);
			if(fileValid(SceneFilePath + pure_name))
			{
				return SceneFilePath + pure_name;
			}

			for(auto& path:valid_scene_path)
			{
				if (fileValid(path + pure_name))
				{
					return path + pure_name;
				}
				if (fileValid(path + name))
				{
					
					string fulpath = path + name;
					return fulpath;
				}
				if (fileValid(SceneFilePath + path + pure_name))
				{
					valid_scene_path.insert(SceneFilePath + path);
					return SceneFilePath + path + pure_name;
				}

			}

			Logger::WriteErrorLog("File %s Not Exist",name.c_str());
			return "";
		}

		static void addSceneFilePath(const string & name)
		{
			valid_scene_path.insert(name);
		}
		static bool fileValid(const string & name)
		{
			ifstream f(name.c_str());
			return f.good();
		}
		
		static inline void ReplaceAll(std::string &str, const std::string& from, const std::string& to) {
			size_t start_pos = 0;
			while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
				str.replace(start_pos, from.length(), to);
				start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
			}
		}

		static std::set<string> valid_shader_path;
		static std::set<string> valid_scene_path;
	};
}

string ReadFiletoString(string filename);
vector<string> GenCubemapList(string a, string b);