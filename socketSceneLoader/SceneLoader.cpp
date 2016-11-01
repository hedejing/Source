#include "SceneLoader.h"
#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>
#include <fstream>
#include <sstream>

using namespace Block;
using namespace rapidjson;
using std::pair;
using std::cout;
using std::endl;
using std::pair;
using std::ifstream;
using std::stringstream;

#define PI 3.141592654

SceneLoader::SceneLoader(SceneManager * scene, MeshManager * meshMgr, int width, int height, int l, int r, int t, int b)
	: scene(scene), meshMgr(meshMgr), width(width), height(height), l(l), r(r), t(t), b(b)
{
	this->layoutMatrix = new Entry[width * height];
	this->rotateFlag = false;

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			this->layoutMatrix[i * width + j].SetX(j);
			this->layoutMatrix[i * width + j].SetY(i);
		}
	}

	for (int i = 0; i < 200; ++i)
		carNames[i] = 0;

	InitSceneNode();
	InitFloor();
	//InitCar();

	bh.Init(scene, meshMgr);
}

void SceneLoader::ParseScene(string json, float time)
{
	//cout << json << endl;

	//parse json into sceneMatrix
	Document root;
	root.Parse(json.c_str());
	if (!root.IsObject())
	{
		cout << "json size: " << json.size() << endl;
		cout << "json str: " << json << endl;
		return;
	}
	assert(root.IsObject());
	Value &blockSize = root["blockSize"];
	Value &rotateFlag = root["rotate"];
	this->rotateFlag = rotateFlag.GetBool();
	
	//set all to NULL
	for (int i = 0; i < width * height; ++i)
	{
		layoutMatrix[i].IsEmpty() = true;
		layoutMatrix[i].IsDrawable() = false;

		layoutMatrix[i].GetBlock().clear();
	}
	if (!crosses.empty())
	{
		//cout << crosses.size() << std::endl;
		crosses.clear();
	}
	 
	//clear street info
	streetPos.clear();
	streetOrien.clear();

	Value &entry = root["entry"];
	assert(entry.IsArray());
	
	for (SizeType i = 0; i < entry.Size(); ++i)
	{
		int x = entry[i]["x"].GetInt();
		int y = entry[i]["y"].GetInt();
		if (x < l || x >= r) continue;//clip sides

		if (!(x >= 0 && x < width && y >= 0 && y < height)) continue;//discarded
		string type = entry[i]["type"].GetString();
		
		this->layoutMatrix[y * width + x].IsEmpty() = false;
		this->layoutMatrix[y * width + x].IsDrawable() = true;

		if("BLOCK" == type)
		{
			Entry::EntryType type;
			string name = entry[i]["id"].GetString();
			BlockDef block = bh.blockPresets[name];
			if (name.substr(1, 5) == "cross")
			{
				crosses.push_back(y * width + x);
				if (name == "xcross" || name == "xcross_greenlight")
					type = Entry::EntryType::XCROSS;
				else if(name == "tcross" || name == "tcross_greenlight")
					type = Entry::EntryType::TCROSS;
				else if(name == "lcross" || name == "lcross_greenlight")
					type = Entry::EntryType::LCROSS;
			}
			else if (name == "park" || name == "station")
				type = Entry::EntryType::AFFL;
			else
				type = Entry::EntryType::BLOCK;

			//span
			string size = block.size;
			int x_pos = size.find_first_of("x");
			string spanX = size.substr(0, x_pos);
			string spanY = size.substr(x_pos + 1, size.size());

			//modify matrix
			int orientation = entry[i]["orientation"].GetInt();
			if (name == "lcross") orientation = (orientation + 2) % 4;
			else if (name == "lcross_greenlight") orientation = (orientation + 3) % 4;
			else if (name == "tcross") orientation = (orientation + 1) % 4;
			if (!orientation) orientation = 4;

			block.orientation = orientation;
			this->layoutMatrix[y * width + x].GetBlock().push_back(block);
			this->layoutMatrix[y * width + x].SetEntryType(type);

			int rotated_spanX, rotated_spanY;
			if (orientation % 2 == 1)
			{
				rotated_spanX = atoi(spanX.c_str());
				rotated_spanY = atoi(spanY.c_str());
			}
			else
			{
				rotated_spanX = atoi(spanY.c_str());
				rotated_spanY = atoi(spanX.c_str());
			}
			for (int i = 0; i < rotated_spanX; ++i)
				for (int j = 0; j < rotated_spanY; ++j)
				{
					if ((y + j) < height && (x + i) < width)
					{
						this->layoutMatrix[(y + j) * width + x + i].IsEmpty() = false;
						this->layoutMatrix[(y + j) * width + x + i].SetEntryType(type);
					}
				}
		}
	}

	//cars
	Value &cars = root["cars"];
	assert(cars.IsArray());

	if (cars.Size() > 0)
		cout << json << endl;

	for (SizeType i = 0; i < cars.Size(); ++i)
	{
		float posX = cars[i]["position"]["x"].GetInt();
		float posZ = cars[i]["position"]["z"].GetInt();

		int dirX = cars[i]["direction"]["x"].GetInt();
		int dirZ = cars[i]["direction"]["z"].GetInt();

		float speed = cars[i]["speed"].GetFloat();
		int mesh = cars[i]["mesh"].GetInt();

		PushCar(Vector3(posX, 0, posZ), Vector3(0, 0, 0), speed, time, mesh);
	}

	//generate street
	pair<int, int> dirs[4];
	dirs[0] = pair<int, int>(0, 1); 
	dirs[1] = pair<int, int>(1, 0);
	dirs[2] = pair<int, int>(0, -1);
	dirs[3] = pair<int, int>(-1, 0);

	for (int i = 0; i < crosses.size(); ++i)
	{
		int fork = layoutMatrix[crosses[i]].GetEntryType();
		for (int k = 0; k < fork; ++k)//expand the cross 
		{
			int crossOrien = layoutMatrix[crosses[i]].GetBlock()[0].orientation;
			pair<int, int> dir = dirs[(k - 1 + crossOrien) % 4];

			for (int x = layoutMatrix[crosses[i]].GetX() + dir.first, y = layoutMatrix[crosses[i]].GetY() + dir.second; x >= 0 && x < width && y >= 0 && y < height;
				x += dir.first, y += dir.second)
			{
				if (this->layoutMatrix[y * width + x].IsEmpty())//NULL
				{
					this->layoutMatrix[y * width + x].IsEmpty() = false;
					this->layoutMatrix[y * width + x].IsDrawable() = true;
					string name = "street";
					BlockDef bd = bh.blockPresets[name];
					bd.orientation = (k + crossOrien) % 2 + 1;
					this->layoutMatrix[y * width + x].GetBlock().push_back(bd);
					this->layoutMatrix[y * width + x].SetEntryType(Entry::EntryType::STREET);

					//push back street pos and orien
					this->streetPos.push_back(Vector3(x, 0, y));
					Quaternion quaternion((bd.orientation - 1) * 0.5f * PI, Vector3(0, 1, 0));
					this->streetOrien.push_back(quaternion);
				}

				else if (this->layoutMatrix[y * width + x].GetEntryType() == Entry::EntryType::BLOCK)//BLOCK
					break;

				else if (this->layoutMatrix[y * width + x].GetEntryType() == Entry::EntryType::AFFL || this->layoutMatrix[y * width + x].GetEntryType() == Entry::EntryType::STREET)
				{
					this->layoutMatrix[y * width + x].IsEmpty() = false;
					this->layoutMatrix[y * width + x].IsDrawable() = true;
					string name = "street";
					BlockDef bd = bh.blockPresets[name];
					bd.orientation = (k + crossOrien) % 2 + 1;
					this->layoutMatrix[y * width + x].GetBlock().push_back(bd);
					this->layoutMatrix[y * width + x].SetEntryType(Entry::EntryType::STREET);
				}

				else//CROSS
				{
					Entry::EntryType type = this->layoutMatrix[y * width + x].GetEntryType();
					BlockDef bd = this->layoutMatrix[y * width + x].GetBlock()[0];
					//TCROSS
					if (type == Entry::EntryType::TCROSS && bd.orientation % 4 == (k + 1 + crossOrien) % 4)
						break;
					//LCROSS
					else if (type == Entry::EntryType::LCROSS
						&& bd.orientation % 4 == (k + 1 + crossOrien) % 4 || bd.orientation % 4 == (k + 2 + crossOrien) % 4)
						break;
					else
						continue;
				}
			}
		}
	}
}

void SceneLoader::GetRandomCars(vector<Vector3>& streetPos, vector<Quaternion>& streetOrien)
{
	streetPos = this->streetPos;
	streetOrien = this->streetOrien;
}

void SceneLoader::PrintLayout()
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
			if (!this->layoutMatrix[i * width + j].IsEmpty())
				printf("1 ");
			else
				printf("0 ");
		printf("\n");
	}
}

void SceneLoader::InitClient(string ip, string port)
{
	client.Connect(ip, port);
}

void SceneLoader::UpdateScene(float curTime)
{
	string json;
	mutex m;

	if (show_json)
	{
		ParseScene(staticJson, curTime);
		UpdateSceneNodes(curTime);
	}
	else
	{
		m.lock();
		string json = client.clientJson;
		m.unlock();
		
		if ("" != json)
		{
			ParseScene(json, curTime);
			UpdateSceneNodes(curTime);
		}
	} 
}

//Init scene nodes and entities
void SceneLoader::InitSceneNode()
{
	entityRoot = scene->CreateSceneNode("entityRoot");
	SceneManager* scene_instance = SceneContainer::getInstance().get("scene1");
	SceneNode* root = scene_instance->GetSceneRoot();
	root->attachNode(entityRoot);

	//sceneNodes
	sceneNodes = new SceneNode*[width * height];
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			stringstream name;
			name.str("");
			name << "node" << i << " " << j;
			sceneNodes[i * width + j] = this->scene->CreateSceneNode(name.str());
		}
	}

}

//Init floar nodes and entities
void SceneLoader::InitFloor()
{
	//floarNodes
	floarNodes = new SceneNode*[height * width];
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; ++j) {
			stringstream name;
			name.str("");
			name << "floar" << i << " " << j;
			floarNodes[i * width + j] = this->scene->CreateSceneNode(name.str());
		}
	}

	//floarEntities
	floarEntities = new Entity*[height * width];
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			stringstream name;
			name << "floar" << i << " " << j;
			floarEntities[i * width + j] = this->scene->CreateEntity(name.str());
			floarNodes[i * width + j]->attachMovable(floarEntities[i * width + j]);
		}
	}

}

//Init car root node
void SceneLoader::InitCar()
{
	SceneManager* scene_instance = SceneContainer::getInstance().get("scene1");
	SceneNode* root = scene_instance->GetSceneRoot();
	carRoot = scene->CreateSceneNode("carRoot");
	root->attachNode(carRoot);
}

void SceneLoader::AttachFloar()
{
	SceneManager* scene_instance = SceneContainer::getInstance().get("scene1");
	SceneNode* root = scene_instance->GetSceneRoot();
	SceneNode* floorRoot = scene->CreateSceneNode("floorRoot");
	root->attachNode(floorRoot);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; ++j) {
			if (j < l || j >= r) continue;//clip sides

			floarEntities[i * width + j]->setMesh(floarFragment);
			floarNodes[i * width + j]->setTranslation(j, 0, i);
			floorRoot->attachNode(floarNodes[i * width + j]);
		}
	}
}

void SceneLoader::UpdateSceneNodes(float curTime)
{
	SceneManager* scene = SceneContainer::getInstance().get("scene1");
	SceneNode* root = scene->GetSceneRoot();
	entityRoot->detachAllNodes();

	//MoveCars(curTime);

	for (int i = 0; i < height; i++) {

		for (int j = 0; j < width; ++j)
		{
			if (j < l || j >= r) continue;//clip sides

			if (layoutMatrix[i * width + j].IsDrawable())
			{
				sceneNodes[i * width + j]->detachAllNodes();
				int blockSize = layoutMatrix[i * width + j].GetBlock().size();
				for (int m = 0; m < blockSize; ++m)
				{
					string subEntryName = "entry" + to_string(i) + " " + to_string(j) + "block" + to_string(m);
					SceneNode* subEntryNode = scene->getSceneNode(subEntryName);
					if (NULL != subEntryNode)
						scene->destroy(subEntryNode);
					subEntryNode = scene->CreateSceneNode(subEntryName);

					BlockDef &block = layoutMatrix[i * width + j].GetBlock()[m];
					string id = block.name;

					int orien = block.orientation;
					string span = block.size;
					int xPos = span.find_first_of("x"); 
					int spanX = atoi(span.substr(0, xPos).c_str());
					int spanY = atoi(span.substr(xPos + 1, span.size()).c_str());
					int xCalibration = 0, yCalibration = 0;
					switch (orien)
					{
					case 2:
						yCalibration = spanX;
						break;
					case 3:
						xCalibration = spanX;
						yCalibration = spanY;
						break;
					case 4:
						xCalibration = spanY;
						break;
					}

					Quaternion quaternion((orien - 1) * 0.5f * PI, Vector3(0, 1, 0));
					subEntryNode->setOrientation(quaternion);
					subEntryNode->setTranslation(j + xCalibration, 0, i + yCalibration);
					sceneNodes[i * width + j]->attachNode(subEntryNode);

					for (int k = 0; k < block.meshID.size(); ++k)
					{
						string name = "entry" + to_string(i) + " " + to_string(j) + "block" + to_string(m) + "mesh" + to_string(k);
						SceneNode* node = scene->getSceneNode(name);
						if (NULL != node)
							scene->destroy(node);
						node = scene->CreateSceneNode(name);

						Entity* entity = scene->getEntity(name);
						if (NULL != entity)
							scene->destroy(entity);
						entity = scene->CreateEntity(name);

						entity->setMesh(bh.meshs[block.meshID[k]]);
						node->attachMovable(entity);
						node->setScale(block.scale[k]);
						//rotation
						if (block.orientations.size() > k)
						{
							Quaternion q(block.orientations[k], Vector3(0, 1, 0));
							node->setOrientation(q);
						}
						node->setTranslation(block.translate[k]);
						subEntryNode->attachNode(node);
					}

					entityRoot->attachNode(sceneNodes[i * width + j]);
				}
			}
		}
	}

}

void SceneLoader::loadMesh() {

	//street
	stringstream ss, ss1;

	//floor
	ss.str("");
	ss1.str("");
	ss << "model/uniform/floor.obj";
	ss1 << "floor";
	floarFragment = meshMgr->loadMesh_assimp_check(ss1.str(), ss.str());

	AttachFloar();

	for (int i = 1; i <= CARNUM; ++i)
	{
		ss.str("");
		ss << "model/uniform/car/car" << std::setw(2) << std::setfill('0') << i;
		cout << ss.str() << endl;
		ss1.str("");
		ss1 << "car" + to_string(i);
		MeshPtr carMesh = meshMgr->loadMesh_assimp_check(ss1.str(), ss.str());
		carMeshes.push_back(carMesh);
	}
}

void SceneLoader::LoadJson()
{
	ifstream stream, stream1;
	stringstream ss;

	ss.str("");
	stream1.open("model/uniform/data/block.json");
	if (stream1.is_open())
	{
		ss << stream1.rdbuf();
		staticJson = ss.str();
	}
}

int SceneLoader::GetCarName()//find first name not used
{
	int name = 0;
	while (carNames[name++]);
	carNames[name] = true;
	return name;
}

void SceneLoader::PushCar(Vector3 position, Vector3 direction, float speed, float startTime, int meshID)
{
	int name = GetCarName();
	SceneNode* carNode = this->scene->CreateSceneNode("car" + to_string(name));
	carNode->setScale(0.01, 0.01, 0.01);
	this->carRoot->attachNode(carNode);
	MeshPtr mesh = this->carMeshes[meshID];

	Entity* entity = this->scene->CreateEntity("car" + to_string(name));
	carNode->attachMovable(entity);
	entity->setMesh(mesh);

	Car car(position, direction, speed, carNode, startTime, name);
	this->cars.push_back(car);
}

void SceneLoader::MoveCars(float curTime)
{
	for (list<Car>::iterator it = cars.begin(); it != cars.end(); )
	{
		it->Move(curTime);

		Vector3 carPos = it->GetPosition();
		int x = carPos[0];
		int z = carPos[2];
		Entry::EntryType type = this->layoutMatrix[z * width + x].GetEntryType();
		if (it->IsOutOfBound(l, r, t, b) || Entry::EntryType::STREET != type)//out of bound OR not street
		{
			int name = it->GetName();
			this->carNames[name] = false;//name free
			it = cars.erase(it);

			this->carRoot->detachNode("car" + to_string(name));

			SceneNode* carNode = scene->getSceneNode("car" + to_string(name));
			if (NULL != carNode)
				scene->destroy(carNode);

			Entity* carEntity = scene->getEntity("car" + to_string(name));
			if (NULL != carEntity)
				scene->destroy(carEntity);
		}
		else
		{
			++it;
		}
	}
}

void Car::Move(float curTime)
{
	float deltaTime = curTime - startTime;
	carNode->translate(direction * deltaTime);
}

bool Car::IsOutOfBound(int l, int r, int t, int b)
{
	Vector3 currentPosition = carNode->getTranslation();
	int x = currentPosition[0];
	int z = currentPosition[2];

	if (x < r && x >= l && z < t && z >= b)
		return false;
	else
		return true;
}