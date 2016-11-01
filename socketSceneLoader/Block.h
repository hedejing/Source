#pragma once

#include "AppFrameWork2.h"
#include "OctreeSceneManager.h"


using namespace HW;

#define PI 3.141592654
struct BlockDef
{
	string name;
	string type;
	string size;
	int orientation;
	vector<int> meshID;
	vector<Vector3> scale;
	vector<float> orientations;
	vector<Vector3> translate;
	
	META(N(name), N(size),N(type),N(orientation),N( meshID), N(scale),N(translate),N(orientations));
};

class BlockHelper {
public:
	map<string, BlockDef> blockPresets;
	vector<MeshPtr> meshs;
	SceneManager* scene=NULL;
	MeshManager* meshMgr=NULL;
	int blocknum = 20;
	void FillBlockData() {
		BlockDef b;

		for (int i = 1; i <= blocknum; i++) {
			serializeLoad("model/uniform/data/block" + to_string(i) + ".txt", b);
			blockPresets[b.name] = b;

		}
		vector<string> names = { "xcross","tcross","lcross","xcross_greenlight",
			"tcross_greenlight","lcross_greenlight","xcross_redlight",
			"tcross_redlight","lcross_redlight","xcross_yellowlight",
			"tcross_yellowlight","lcross_yellowlight","street","park","station" };
		for (auto x : names) {
			serializeLoad("model/uniform/data/" + x + ".txt", b);
			blockPresets[b.name] = b;
		}

	}
	void SaveBlock(string fname,string bname,string bsize,string btype,int borien) {
		auto nodes = scene->GetSceneRoot()->getAllChildNodes();
		vector<SceneNode*> bnodes;
		for (auto& x : nodes) {
			if (x->getName().find("node_b") != string::npos) {
				bnodes.push_back(x);
			}
		}
		BlockDef b;
		b.name = bname;
		b.size = bsize;
		b.type = btype;
		b.orientation = borien;
		for (auto& node : bnodes) {
			auto t = node->getLocalTranslation();
			auto s = node->getLocalScale();
			auto r = node->getOrientation();
			float rad=0;
			Vector3 axis;
			r.toAngleAxis(rad, axis);
			int id = -1;
			for (int i = 0; i < meshs.size();i++) {
				OMesh* meshptr = meshs[i].InterPtr();
				if (scene->getEntity(node->getName())->getMesh().InterPtr() == meshptr)
				{
					id = i; 
					b.meshID.push_back(id);
					b.scale.push_back(s);
					b.translate.push_back(t);
					b.orientations.push_back(rad);
					break;;
				}
			}
		}
		b.orientation = 1;
		b.type = "block";
		serializeSave(fname, b);
	}
	string LoadBlockPreset(string fname) {
		BlockDef b;
		serializeLoad(fname, b);
		blockPresets[b.name] = b;
		return b.name;
	}
	void Init(SceneManager* _scene,MeshManager* _meshmgr ) {
		scene = _scene;
		meshMgr = _meshmgr;
		map<int, string> modellist;

		for (int i = 1; i <= 9; i++) {
			modellist[i] = "building0" + to_string(i) + ".obj";
		}
		for (int i = 10; i <= 15; i++) {
			modellist[i] = "building" + to_string(i) + ".obj";
		}
		modellist[16] = "road1.obj";
		modellist[17] = "road2.obj";
		modellist[18] = "lightGreen2.obj";
		modellist[19] = "lightRed2.obj";
		modellist[20] = "lightYellow2.obj";
		modellist[21] = "roadpark.obj";
		modellist[22] = "busStation.obj";

		int end = 23;
		for (int i = 16; i <= 25; i++) {
			modellist[end] = "building" + to_string(i) + ".obj";
			end++;
		}

		for (int i = 1; i <= 35; i++) {
			modellist[end] = "v2/b" + to_string(i) + ".obj";
			end++;
		}

		for (int i = 1; i < end; i++) {
			MeshPtr mesh = meshMgr->loadMesh_assimp_check("", "model/uniform/" + modellist[i]);
			meshs.push_back(mesh);
		}

		FillBlockData();
	}
	void AttachBlock(string blockname, string nodename, Vector3 pos) {
		SceneNode* root=scene->GetSceneRoot();
		auto blockit = blockPresets.find(blockname);
		if (blockit == blockPresets.end())
			return;
		auto block = blockit->second;
		//avoid repeat node
		for (int i = 0; i < block.meshID.size(); i++) {
			string name = nodename + to_string(i);
			auto snode = scene->getSceneNode(name);
			if (snode != NULL) {
				removeNode(name);
			}
			snode = scene->getSceneNode(name);
			auto entity = scene->CreateEntity(name);
			entity->setMesh(meshs[block.meshID[i]]);
			snode = scene->CreateSceneNode(name);
			snode->attachMovable(entity);
			root->attachNode(snode);

			snode->setTranslation(block.translate[i] + pos);
			snode->setScale(block.scale[i]);
			Quaternion q;
			q.fromAngleAxis(block.orientations[i], Vector3(0, 1, 0));
			snode->setOrientation(q);
		}
	}
	void removeNode(string nodename) {
		auto node = scene->getSceneNode(nodename);
		auto entity = scene->getEntity(nodename);
		if (node != NULL) {
			node->getParent()->detachNode(node);
			scene->destroy(node);
		}
		if (entity != NULL)
			scene->destroy(entity);
	}
	void attachAllBlock() {
		int i = 0;
		for (auto& b:blockPresets) {
			i++;
			int x = i % 8 * 4, y = i / 8 * 4;
			AttachBlock(b.first, "block_demo" + to_string(i), Vector3(x, 0, y));
		}

	}

	void attachAllMesh() {
		SceneNode* root = scene->GetSceneRoot();
		for (int i = 0; i < meshs.size(); i++) {
			string name = "attach all mesh" + to_string(i);
			auto snode = scene->getSceneNode(name);
			if (snode != NULL) {
				removeNode(name);
			}
			auto entity = scene->CreateEntity(name);
			entity->setMesh(meshs[i]);
			snode = scene->CreateSceneNode(name);
			snode->attachMovable(entity);
			int x = i % 8 * 4, y = i / 8 * 4;
			snode->setTranslation(Vector3(x, 0, y));
			root->attachNode(snode);
		}
	}
};