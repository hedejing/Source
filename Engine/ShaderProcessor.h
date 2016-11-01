#pragma once
#include <string>
#include <map>
#include <regex>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "EngineUtil.h"
using namespace std;


//keep a map :(name - text) ,for shaders,parse simple include

class ShaderProsscer {
public:
	void AddShaderFile(string filename) {
		if (!HasShader(filename)) {
			ParseFile(filename);
		}
	}

	string GetShaderCode(string name) {
		if (ShaderCache.find(name) != ShaderCache.end())
			return ShaderCache[name];
		else
			return "";
	}

	bool HasShader(string name) {
		if (ShaderCache.find(name) != ShaderCache.end())
			return true;
		else
			return false;
	}

	void UpdateShaderFile(string name) {
		auto it = ShaderCache.find(name);
		if (it != ShaderCache.end())
			ShaderCache.erase(it);
		AddShaderFile(name);
	}
private:

	string GetIncludeName(string s) {
		auto start = s.find_first_of("\"");
		auto last = s.find_last_of("\"");
		return s.substr(start + 1, last - start - 1);
	}
	string Replace(string text, string include, string context) {
		regex pattern(include);
		string newtext = regex_replace(text, pattern, context);
		return newtext;
	}

	void ParseFile(string filename) {
		string fullpath = HW::FileUtil::getSceneFileValidPath(filename);
		if (fullpath == "") {
			cout << "cant find file" + filename << endl;
			exit(0);
		}
		string text = ReadFiletoString(fullpath);
		string mp = R"(#include +("[^"]+"))";
		regex word_regex(mp);

		//get include file list
		vector<string> IncludeList;
		auto words_begin =
			sregex_iterator(text.begin(), text.end(), word_regex);
		auto words_end = sregex_iterator();
		for (auto i = words_begin; i != words_end; ++i) {
			smatch match = *i;
			string match_str = match.str();
			IncludeList.push_back(match_str);
		}


		//replace include to text
		for (auto& x : IncludeList) {
			string name= GetIncludeName(x);
			if (!HasShader(name))
				ParseFile(name);
			text = Replace(text, x, GetShaderCode(name));
		}

		ShaderCache[filename] = text;
	}

	string ReadFiletoString(string filepath)
	{

		ifstream file(filepath.c_str(), std::ios::in);
		assert(file.good());
		file.seekg(0, std::ios::end);
		size_t size = file.tellg();
		string buffer(size, ' ');
		file.seekg(0);
		file.read(&buffer[0], size);
		return buffer;
	}

	map<string, string> ShaderCache;
};
