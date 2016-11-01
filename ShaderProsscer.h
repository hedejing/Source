#pragma once
#include "EngineUtil.h"
#include <string>
#include <map>
#include <regex>
#include <iostream>
#include <fstream>
using namespace std;

class ShaderProsscer {
public:
	void AddShaderFile(string filename) {
		if (ShaderCache.find(filename) == ShaderCache.end()) {
			ParseFile(filename);
		}
	}

	string GetShaderCode(string name) {

	}


private:
	void ParseFile(string filename) {
		//#include +((<[^>]+>)|("[^"]+"))
		std::regex word_regex("(\\S+)");
		auto words_begin =
			std::sregex_iterator(s.begin(), s.end(), word_regex);
		auto words_end = std::sregex_iterator();

		std::cout << "Found "
			<< std::distance(words_begin, words_end)
			<< " words\n";

		std::cout << "Words longer than " << N << " characters:\n";
		for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
			smatch match = *i;
			string match_str = match.str();
			cout << match.str();

		}
	}


	void ParseAndCacheFile(string path) {

	}

	map<string, string> ShaderCache;
};