#include "EngineUtil.h"
using namespace HW;
namespace HW
{
	std::set<string> FileUtil::valid_shader_path;
	std::set<string> FileUtil::valid_scene_path;
}

std::string ReadFiletoString(string filename)
{
	string valid_file_name = FileUtil::getSceneFileValidPath(filename);
	std::ifstream file(valid_file_name.c_str(), std::ios::in);
	assert(file.good());

	string filedir = valid_file_name.substr(0, valid_file_name.find_last_of('/') + 1);
	FileUtil::addSceneFilePath(filedir);


	//http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
	// Jerry's Solution
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	std::string buffer(size, ' ');
	file.seekg(0);
	file.read(&buffer[0], size);
	return buffer;
}

vector<string> GenCubemapList(string a, string b)
{
	vector<string> n;
	for (int i = 0; i < 6; i++) {
		n.push_back(a + to_string(i) + b);
	}
	return n;
}
