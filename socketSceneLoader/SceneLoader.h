#pragma once
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <iomanip>
#include <mutex>
#include "connection.h"

#include <sstream>
#include <map>
#include "AppFrameWork2.h"
#include "Block.h"
using std::string;
using std::stringstream;
using std::vector;
using std::list;
using std::cout;
using std::mutex;
using std::map;
using std::pair;

#define LEFT 6
#define RIGHT 26
#define TOP 16
#define BOTTOM 0
#define CARNUM 11

namespace Block
{
	
	class Entry
	{
	public :
		enum EntryType
		{
			STREET = 0,
			XCROSS = 4,
			LCROSS = 2,
			TCROSS = 3,
			BLOCK = 5,
			AFFL = 6
		};

	protected :
		EntryType entryType;
		int x, y;
		bool drawable;//true if marker exist
		bool empty;//true if object cover
		vector<BlockDef> blocks;

	public :
		Entry()
		{
			empty = true;
			drawable = false;
		}

		~Entry()
		{
			
		}

		void SetEntryType(EntryType entryType)
		{
			this->entryType = entryType;
		}

		EntryType GetEntryType()
		{
			return this->entryType;
		}

		void SetX(int x)
		{
			this->x = x;
		}

		void SetY(int y)
		{
			this->y = y;
		}

		int GetX()
		{
			return this->x;
		}

		int GetY()
		{
			return this->y;
		}

		bool& IsEmpty()
		{
			return this->empty;
		}

		bool& IsDrawable()
		{
			return this->drawable;
		}

		vector<BlockDef>& GetBlock()
		{
			return this->blocks;
		}

	private:
		Entry(const Entry&);
		Entry& operator= (const Entry&);
	};

	class Car
	{
	public:
		Car() {}
		Car(Vector3 position, Vector3 direction, float speed = 20.0f, SceneNode* sceneNode = NULL, float startTime = 0.0f, int name = 0)
			: position(position), direction(direction), speed(speed), carNode(carNode), startTime(startTime), name(name)
		{
			carNode->setTranslation(position);
		}

		void Move(float deltaTime);
		bool IsOutOfBound(int l, int r, int t, int b);
		int GetName()
		{
			return name;
		}
		Vector3 GetPosition()
		{
			return this->position;
		}

	private:
		Vector3 position;
		Vector3 direction;
		float speed;
		SceneNode* carNode;
		float startTime;
		int name;
	};

	class SceneLoader
	{

	public:
		bool show_json = false;
		bool rotateFlag;
		//default 32 * 16
		SceneLoader(SceneManager * scene, MeshManager * meshMgr, int width, int height, int l, int r, int t, int b);

		~SceneLoader()
		{
			printf("delete SceneLoader\n");
			delete [] layoutMatrix;
			delete sceneNodes;
			delete floarNodes;
			delete floarEntities;
		}

		//Scene Management
		void ParseScene(string json, float time);//from json to layoutMatrix
		void LoadJson();//static load json from file
		void UpdateScene(float curTime);
		void loadMesh();
		void InitSceneNode();
		void InitFloor();
		void InitCar();
		void AttachFloar();
		void UpdateSceneNodes(float curTime);
		void GetRandomCars(vector<Vector3>& roadPos, vector<Quaternion>& roadOrien);
		int GetCarName();
		void MoveCars(float curTime);

		//Getters $ Setters
		int GetWidth()
		{
			return this->width;
		}

		int GetHeight()
		{
			return this->height;
		}

		bool IsEmpty(int x, int y)
		{
			return layoutMatrix[y * width + x].IsEmpty();
		}

		Entry* GetMatrix()
		{
			return layoutMatrix;
		}

		//helper functions
		void PrintLayout();
		
		//Server
		void InitClient(string ip, string port);

		//cars
		void PushCar(Vector3 position, Vector3 direction, float speed, float startTime, int meshID);

		//block
		BlockHelper bh;
	private :
		int width, height, l, r, t, b;//width & height of the layoutMatrix
		Entry* layoutMatrix;
		vector<int> crosses;
		vector<Vector3> streetPos;
		vector<Quaternion> streetOrien;
		list<Car> cars;
		bool carNames[200];

		SceneManager * scene;
		MeshManager * meshMgr;
		MeshPtr floarFragment;
		vector<MeshPtr> carMeshes;
		SceneNode** sceneNodes;
		SceneNode** floarNodes;
		Entity** floarEntities;
		SceneNode* carRoot;
		SceneNode* entityRoot;

		string staticJson;

		//network
		Client client;

		//invalid invoke
		SceneLoader(const SceneLoader&);
		SceneLoader& operator= (const SceneLoader&);
	};

}

