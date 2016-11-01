#pragma once
#include "AppFrameWork2.h"
#include <fstream>
#include <stdlib.h>
#define PI 3.141592654
class CarTrackingData {
public:
	Vector3 pos=Vector3(0,0,0);
	Vector3 LastPos = Vector3(0, 0, 0);
	Vector3 d;
	Quaternion q=Quaternion::IDENTITY;
	int frame = 0;
	SceneNode* Node=NULL;

	void Track(Vector3 newpos) {
		frame++;
		LastPos = pos;
		pos = newpos;
		Vector3 tmpd = newpos - LastPos;
		if (tmpd.length() < 1e-5)
			return;
		d = newpos - LastPos;
		d.normalize();

	}
	void UpdateNode() {
		Node->setTranslation(GetPos());
		Node->setOrientation(GetOrientation());
	}
	Vector3 GetPos() {
		return pos;
	}

	Quaternion GetOrientation() {
		Vector3 d0 = Vector3(0, 0, 1);
		if ((d - d0).length() < 1e-5) {
			q.fromAngleAxis(0, Vector3(0, 1, 0));
		}
		else 
		{
			float c = d.dotProduct(d0);
			float angle = acos(c);
			float clockwise = 1;
			if (d.x < 0) 
				clockwise = -1;
			q.fromAngleAxis(clockwise*angle, Vector3(0, 1, 0));
		}
		return q;
	}
};


class CarSimulation {
public:
	App* app=NULL;
	SceneManager* scene;
	vector<MeshPtr> CarList;
	fstream f;
	map<string,CarTrackingData> carmap;
	int MaxCarNum=100;
	int CarTypeNum = 7;
	int RunningCarNum = 1;
	float carDensity = 0.3;
	string prefix = "carNode";
	void Init(App* _app) {
		this->app = _app;
		scene=app->scene;
		f = fstream("CarSim/CarData.txt");
		LoadCarMeshes();
		//for (int i = 0; i < RunningCarNum;i++) {
		//	AddCar(to_string(i), i%CarTypeNum);
		//}

	}
	void LoadCarMeshes() {
		for (int i = 1; i <= CarTypeNum; i++) {
			string prefix = i < 10 ? "car0" : "car";
			string name = prefix + to_string(i);
			MeshPtr mesh = app->meshMgr->loadMesh_assimp_check(name, "model/uniform/car/" + name + ".obj");
			CarList.push_back(mesh);
		}
	}
	void GenRandomCars(vector<Vector3>& pos, vector<Quaternion>& orients) {
		float scale = 1.7;
		float carProb = 0.9;
		for (int i = 0; i < pos.size(); i++) {
			if (rand() % 1000 / 1000.0>carProb) continue;

			string name = prefix + to_string(i);
			AddCar(name, i%CarTypeNum);
			Quaternion q(0.5*PI, Vector3(0, 1, 0));
			carmap[name].Node->scale(Vector3(scale));
			
			Matrix3 m;
			orients[i].toRotationMatrix(m);
			Vector3 offset;
			offset.x = (rand() % 1000) / 1000.0 *0.8+0.1;
			
			offset.z = (rand() % 4 * 0.25 + 0.125)*0.3333+0.3333;
			offset.y = 0.01;
			carmap[name].Node->setOrientation(q*orients[i]);
			carmap[name].Node->setTranslation(pos[i]+m*offset);
			
			
		}
	} 
	void AddCar(string name,int type) {
		CarTrackingData cd;
		AttachCarNode(cd, name, type);
		carmap[name] = cd;
	}
	void RemoveCar(string name) {
		auto snode = scene->getSceneNode(name);
		if (snode != NULL) {
			RemoveNode(name);
		}
		carmap.erase(name);
	}
	void RemoveNode(string nodename) {
		auto scene = app->scene;
		auto node = scene->getSceneNode(nodename);
		auto entity = scene->getEntity(nodename);
		if (node != NULL) {
			node->getParent()->detachNode(node);
			scene->destroy(node);
		}
		if (entity != NULL)
			scene->destroy(entity);
	}

	void AttachCarNode(CarTrackingData& cardata, string name, int meshId) {
		auto snode = app->scene->getSceneNode(name);
		if (snode != NULL) {
			RemoveNode(name);
		}
		snode = scene->CreateSceneNode(name);
		auto mesh = CarList[meshId];
		Entity* entity = scene->CreateEntity(name);
		entity->setMesh(mesh);
		snode->attachMovable(entity);
		snode->setScale(0.01, 0.01, 0.01);
		scene->GetSceneRoot()->attachNode(snode);
		cardata.Node = snode;
	}
	void UpdateCars() {
		Vector3 pos;
		f  >> pos.x >> pos.y >> pos.z;
		//test
		pos.y = 0.05;
		if (f.eof()) {
			f.clear();
			f.seekg(0, ios::beg);
		}

		//for (int i = 0; i < RunningCarNum; i++) {
		//	Vector3 offset(0.04*i, 0, -0.04*i);
		//	carmap[to_string(i)].Track(pos+offset);
		//	carmap[to_string(i)].UpdateNode();
		//}

	}

	void Update() {
		int step = 1;
		static int i = 0;
		i++;
		if (i == step) {
			i = 0;
			UpdateCars();
		}
	}
};