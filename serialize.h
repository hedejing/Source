#pragma once

#include <cereal/types/vector.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>
#include <functional>
using namespace std;

#define N(x) CEREAL_NVP(x)
#define  META(...)   template <class Archive> void serialize(Archive & ar){ar( __VA_ARGS__);}

//template<typename T>
//class Serialize {
//public:
	static auto serializeSave = [](string fname, auto& x) {
		std::ofstream os(fname, std::ios::binary);
		if (os.good()) {
			cereal::JSONOutputArchive archive(os);
			archive(x);
		}
	};
	static auto serializeLoad = [](string fname, auto& x) {
		std::ifstream os(fname, std::ios::binary);
		if (os.good()) {
			cereal::JSONInputArchive archive(os);
			archive(x);
		}
	};
//};



//void serializeSave(string fname,auto x) {
//		std::ofstream os(fname, std::ios::binary);
//		cereal::JSONOutputArchive archive(os);
//		archive(x);
//}

//struct MyRecord
//{
//	uint8_t x, y;
//	shared_ptr<float> z;
//
//
//	META(N(x), N(y), N(z));
//};
//
//struct SomeData
//{
//	int32_t id;
//	std::shared_ptr<std::unordered_map<uint32_t, MyRecord>> data;
//
//	META(data);
//};
//
//int main()
//{
//	std::ofstream os("out.txt", std::ios::binary);
//	// std::ifstream os2("out.txt", std::ios::binary);
//	cereal::JSONOutputArchive archive(os);
//	//  cereal::JSONInputArchive a2(os2);
//	SomeData myData;
//	// archive( myData );
//	int x = 1, y = 2;
//
//	MyRecord m, m2;
//	float c = 5.4;
//	m.x = 8; m.y = 1; m.z = make_shared<float>(c);
//	archive(m);
//	//auto t = make_tuple(m.x, m.y, m.z);
//	//a2(m2);
//
//	return 0;
//}