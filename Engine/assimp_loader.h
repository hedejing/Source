#pragma once
// assimp include files. These three are usually needed.
//#include "assimp/Importer.hpp"	//OO version Header!
//#include "assimp/PostProcess.h"
//#include "assimp/Scene.h"
//#include "assimp/DefaultLogger.hpp"
//#include "assimp/LogStream.hpp"

#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/ai_assert.h>
#include <assimp/cfileio.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>
#include "engine_struct.h"
#include <stdio.h>



//declare
void LoadMeshBasicAttributes(const aiMesh* mesh, as_Geometry* geometry);

void createAILogger()
{
	//Assimp::Logger::LogSeverity severity = Assimp::Logger::NORMAL;
	Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;

	// Create a logger instance for Console Output
	Assimp::DefaultLogger::create("", severity, aiDefaultLogStream_STDOUT);

	// Create a logger instance for File Output (found in project folder or near .exe)
	Assimp::DefaultLogger::create("assimp_log.txt", severity, aiDefaultLogStream_FILE);

	// Now I am ready for logging my stuff
	Assimp::DefaultLogger::get()->info("this is my info-call");
}
void logDebug(const char* logString)
{
	//Will add message to File with "debug" Tag
	Assimp::DefaultLogger::get()->debug(logString);
}
void destroyAILogger()
{
	// Kill it after the work is done
	Assimp::DefaultLogger::kill();
}

void logInfo(std::string logString)
{
	//Will add message to File with "info" Tag
	Assimp::DefaultLogger::get()->info(logString.c_str());
}



Matrix4 convert_assimp_matrix(aiMatrix4x4 m) {
	return Matrix4(
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4
		).transpose();
}

// assume one scene contains only one model,but allow multi mesh
// so load and combine bones to one skeleton
// input:scene output: skeleton
bool LoadSceneSkeleton(const aiScene* scene,as_Skeleton* skeleton){
	//create mesh skeleton
	map<string, aiBone*> bone_map;
	map<string, string>  parent_map;//用于记录节点的父子关系
	aiNode* rootnode = scene->mRootNode;
	aiNode* skeleton_root = NULL;
	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		aiMesh* mesh = scene->mMeshes[m_i];
		if (mesh->HasBones()){
			skeleton->name = mesh->mName.C_Str();
			for (int i = 0; i < mesh->mNumBones; i++){
				aiBone* bone = mesh->mBones[i];
				bone_map[bone->mName.C_Str()] = bone;
				aiNode* node = rootnode->FindNode(bone->mName);
				while (true){
					aiNode* parent = node->mParent;
					if (parent == rootnode){
						skeleton_root = node;
						break;
					}
					parent_map[node->mName.C_Str()] = parent->mName.C_Str();
					node = parent;
				}
			}
		}
	}
	if (!(skeleton_root != NULL&& bone_map.size() > 0))
		return false;
	assert(skeleton_root != NULL);
	assert(bone_map.size() > 0);
	as_Bone* root = new as_Bone(skeleton_root->mName.C_Str(), Matrix4::IDENTITY, Matrix4::IDENTITY, 0, NULL);
	skeleton->root = root;
	skeleton->bones.push_back(root);
	int boneid = 0;
	skeleton->bone_map[root->name] = root;
	for (auto it : bone_map){
		boneid++;
		auto aibone = it.second;
		as_Bone* bone = new as_Bone(aibone->mName.C_Str(), convert_assimp_matrix(aibone->mOffsetMatrix), Matrix4::IDENTITY, boneid, NULL);
		skeleton->bone_map[it.first] = bone;
		skeleton->bones.push_back(bone);
	}
	skeleton->num_bones = skeleton->bones.size();
	//find their parent
	for (int i = 1; i < skeleton->bones.size(); i++){
		auto bone = skeleton->bones[i];
		as_Bone* parent = skeleton->bone_map[parent_map[bone->name]];
		bone->parent = parent;
		parent->children.push_back(bone);
	}
	return true;
}
//input:model file
//output: geometry,skeleton,animation,material



void debug_fill_diffusecolor(aiMaterial* mat, as_Geometry* geometry){
	aiColor3D color(0.f, 0.f, 0.f);
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	for (int i = 0; i < geometry->mNumVertices; i++){
		geometry->att_debug.push_back(color.r);
		geometry->att_debug.push_back(color.g);
		geometry->att_debug.push_back(color.b);

	}
}
//根据之前的skeleton,构造出每个顶点对应的boneId和weight
void FillBoneIdWeights(const aiScene*scene, as_Skeleton* skeleton, vector<as_Geometry*>& geo){
	//create boneId and bone Weights;
	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		aiMesh* mesh = scene->mMeshes[m_i];
		as_Geometry* geometry = geo[m_i];
		geometry->boneID = vector<int>(geometry->mNumVertices*MaxBonePerVertex, 0);
		geometry->boneWeight = vector<float>(geometry->mNumVertices*MaxBonePerVertex, 0);
		vector<int> offset = vector<int>(geometry->mNumVertices, 0);//代表vertex已经填充了几个weight
		if (mesh->HasBones()){
			for (int i = 0; i < mesh->mNumBones; i++){
				aiBone* bone = mesh->mBones[i];
				for (int j = 0; j < bone->mNumWeights; j++){
					const aiVertexWeight & weight = bone->mWeights[j];
					int vertexId = weight.mVertexId;
					int offset_t = offset[vertexId];
					offset[vertexId] += 1;
					geometry->boneID[vertexId * 4 + offset_t] = skeleton->bone_map[bone->mName.C_Str()]->id;
					geometry->boneWeight[vertexId * 4 + offset_t] = weight.mWeight;

				}
			}
		}
		//debug_fill_diffusecolor(scene->mMaterials[mesh->mMaterialIndex], geometry);
	}
}

Vector3 aiVector3D_to_Vector3(aiVector3D& v){
	return Vector3(v.x, v.y, v.z);
}
Quaternion aiQuaternion_to_quanternion(aiQuaternion& q){
	Quaternion q2;
	q2.w = q.w;
	q2.x = q.x;
	q2.y = q.y;
	q2.z = q.z;
	return q2;
}
// load mesh position,normal,indices
void LoadMeshBasicAttributes(const aiMesh* mesh, as_Geometry* geometry,Vector3& sMax,Vector3& sMin){

	geometry->mNumVertices = mesh->mNumVertices;
	geometry->name = mesh->mName.C_Str();
	for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
		if (mesh->HasPositions()) {
			const aiVector3D* vp = &(mesh->mVertices[v_i]);
			geometry->position.push_back(vp->x);
			geometry->position.push_back(vp->y);
			geometry->position.push_back(vp->z);
			if (vp->x < sMin.x) sMin.x = vp->x; if (vp->x > sMax.x) sMax.x = vp->x;
			if (vp->y < sMin.y) sMin.y = vp->y; if (vp->y > sMax.y) sMax.y = vp->y;
			if (vp->z < sMin.z) sMin.z = vp->z; if (vp->z > sMax.z) sMax.z = vp->z;
		}
		if (mesh->HasNormals()) {
			const aiVector3D* vn = &(mesh->mNormals[v_i]);
			geometry->normal.push_back(vn->x);
			geometry->normal.push_back(vn->y);
			geometry->normal.push_back(vn->z);

		}
		if (mesh->HasTangentsAndBitangents()) {
			auto vn = &(mesh->mTangents[v_i]);
			geometry->tangent.push_back(vn->x);
			geometry->tangent.push_back(vn->y);
			geometry->tangent.push_back(vn->z);

			vn = &(mesh->mBitangents[v_i]);
			geometry->bitangent.push_back(vn->x);
			geometry->bitangent.push_back(vn->y);
			geometry->bitangent.push_back(vn->z);

		}
		if (mesh->HasTextureCoords(0)) {
			const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
			geometry->texcoord.push_back(vt->x);
			geometry->texcoord.push_back(vt->y);

		}

	}
	if (mesh->HasFaces()){
		geometry->mNumFaces = mesh->mNumFaces;
		for (int i = 0; i < mesh->mNumFaces; i++){
			aiFace* face = &mesh->mFaces[i];
			geometry->indices.push_back(face->mIndices[0]);
			geometry->indices.push_back(face->mIndices[1]);
			geometry->indices.push_back(face->mIndices[2]);
		}
	}
}

//
void readAnimation(aiAnimation* ai_anim, as_SkeletonAnimation* my_anim){
	my_anim->name = ai_anim->mName.C_Str();
	my_anim->duration = ai_anim->mDuration;
	my_anim->tickPerSecond = ai_anim->mTicksPerSecond;
	my_anim->numChannels = ai_anim->mNumChannels;
	for (int i = 0; i < ai_anim->mNumChannels; i++){
		auto node = ai_anim->mChannels[i];
		as_NodeAnimation* mynode = new as_NodeAnimation;

		mynode->name = node->mNodeName.C_Str();
		mynode->positionKeys = vector<vectorKey>(node->mNumPositionKeys);
		mynode->rotateKeys = vector<quaterionKey>(node->mNumRotationKeys);
		mynode->scaleKeys = vector<vectorKey>(node->mNumScalingKeys);
		mynode->numPositionkeys = node->mNumPositionKeys;
		mynode->numRotatekeys = node->mNumRotationKeys;
		mynode->numScalekeys = node->mNumScalingKeys;
		for (int i = 0; i < node->mNumPositionKeys; i++){
			mynode->positionKeys[i].time = node->mPositionKeys[i].mTime;
			mynode->positionKeys[i].value = aiVector3D_to_Vector3(node->mPositionKeys[i].mValue);
		}
		for (int i = 0; i < node->mNumRotationKeys; i++){
			mynode->rotateKeys[i].time = node->mRotationKeys[i].mTime;
			mynode->rotateKeys[i].quat = aiQuaternion_to_quanternion(node->mRotationKeys[i].mValue);
		}
		for (int i = 0; i < node->mNumScalingKeys; i++){
			mynode->scaleKeys[i].time = node->mScalingKeys[i].mTime;
			mynode->scaleKeys[i].value = aiVector3D_to_Vector3(node->mScalingKeys[i].mValue);
		}
		my_anim->channels.push_back(mynode);
	}
}
Vector3 aiColortoVector3(aiColor3D& c){
	return Vector3(c.r, c.g, c.b);
}
string getPureFileName(string name){

	auto pos1 = name.find_last_of('/');
	auto pos2 = name.find_last_of('\\');
	auto pos = pos1;

	if (pos2 != name.npos&&pos1 != name.npos&&pos2>pos1)
		pos = pos2;
	if (pos1 == name.npos&&pos2 != name.npos)
		pos = pos2;
	//if (pos2 > pos1) pos1 = pos2;

	return name.substr(pos + 1);
}
void readMaterial(aiMaterial* mat,as_Material* my_mat){
	aiColor3D color(0.f, 0.f, 0.f);
	float s;
	aiString name; 
	mat->Get(AI_MATKEY_NAME, name); 
	my_mat->name = name.C_Str();
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	my_mat->diffuse = aiColortoVector3(color);
	mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
	my_mat->specular = aiColortoVector3(color);
	mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
	my_mat->ambient = aiColortoVector3(color);
	mat->Get(AI_MATKEY_COLOR_TRANSPARENT, color);
	my_mat->transparent = aiColortoVector3(color);
	mat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
	my_mat->emissive = aiColortoVector3(color);

	mat->Get(AI_MATKEY_SHININESS, my_mat->shininess);
	mat->Get(AI_MATKEY_OPACITY, my_mat->opacity);
	
	int d;
	aiString path;
	vector<aiTextureType> textype = { aiTextureType_DIFFUSE ,aiTextureType_SPECULAR,aiTextureType_AMBIENT,aiTextureType_OPACITY,
		aiTextureType_HEIGHT ,aiTextureType_EMISSIVE ,aiTextureType_NORMALS,aiTextureType_SHININESS,aiTextureType_DISPLACEMENT,
		aiTextureType_REFLECTION ,aiTextureType_LIGHTMAP };

	vector<string>  textype2 = { "diffuse" ,"specular" ,"ambient","opacity","height","emissive" ,"normal" ,"shininess" ,"displacement"
	,"reflection" ,"lightmap" };
	for (int i = 0; i < textype.size(); i++) {
		if (aiReturn_SUCCESS == mat->GetTexture(textype[i], 0, &path)) {
			
			my_mat->textures[textype2[i]] = as_Texture(string(name.C_Str()) + textype2[i]);
			my_mat->textures[textype2[i]].path = path.C_Str();
		}
	}

	
}
bool  Import3DFromFile(const std::string& filepath, vector<as_Mesh*>& meshs,vector<as_Geometry*>& geo, vector<as_Skeleton*>& skeletons,vector<as_SkeletonAnimation*>& ani,
	vector<as_Material*>& materials,Vector3& sMax,Vector3& sMin)
{
	const aiScene* scene;
	Assimp::Importer importer;
	createAILogger();
	//post process steps
	unsigned int ppsteps = aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
		aiProcess_JoinIdenticalVertices | // join identical vertices/ optimize indexing
		aiProcess_ValidateDataStructure | // perform a full validation of the loader's output
		aiProcess_ImproveCacheLocality | // improve the cache locality of the output vertices
		aiProcess_RemoveRedundantMaterials | // remove redundant materials
		//aiProcess_FindDegenerates | // remove degenerated polygons from the import
		aiProcess_FindInvalidData | // detect invalid model data, such as invalid normal vectors
		//aiProcess_GenUVCoords | // convert spherical, cylindrical, box and planar mapping to proper UVs
		aiProcess_TransformUVCoords | // preprocess UV transformations (scaling, translation ...)
		//aiProcess_FindInstances | // search for instanced meshes and remove them by references to one master
		aiProcess_LimitBoneWeights | // limit bone weights to 4 per vertex
		aiProcess_OptimizeMeshes | // join small meshes, if possible;
		//aiProcess_SplitByBoneCount | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
		0;
	float g_smoothAngle = 80.f;
	bool nopointslines = false;

	aiPropertyStore* props = aiCreatePropertyStore();
	aiSetImportPropertyInteger(props, AI_CONFIG_IMPORT_TER_MAKE_UVS, 1);
	aiSetImportPropertyFloat(props, AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, g_smoothAngle);
	aiSetImportPropertyInteger(props, AI_CONFIG_PP_SBP_REMOVE, nopointslines ? aiPrimitiveType_LINE | aiPrimitiveType_POINT : 0);

	aiSetImportPropertyInteger(props, AI_CONFIG_GLOB_MEASURE_TIME, 1);
	//aiSetImportPropertyInteger(props,AI_CONFIG_PP_PTV_KEEP_HIERARCHY,1);

	// Call ASSIMPs C-API to load the file
	auto q = filepath.c_str();
	scene = (aiScene*)aiImportFileExWithProperties(filepath.c_str(),

		ppsteps | /* configurable pp steps */
		aiProcess_GenSmoothNormals | // generate smooth normal vectors if not existing
		//aiProcess_SplitLargeMeshes | // split large, unrenderable meshes into submeshes
		aiProcess_Triangulate | // triangulate polygons with more than 3 edges
		//aiProcess_ConvertToLeftHanded | // convert everything to D3D left handed space
		//aiProcess_SortByPType | // make 'clean' meshes which consist of a single typ of primitives
		0,
		NULL,
		props);
	
	//unsigned int ppsteps = aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
	//	aiProcess_JoinIdenticalVertices | // join identical vertices/ optimize indexing
	//	aiProcess_ValidateDataStructure | // perform a full validation of the loader's output
	//	aiProcess_ImproveCacheLocality | // improve the cache locality of the output vertices
	//	aiProcess_RemoveRedundantMaterials | // remove redundant materials
	//	aiProcess_FindDegenerates | // remove degenerated polygons from the import
	//	aiProcess_FindInvalidData | // detect invalid model data, such as invalid normal vectors
	//	aiProcess_GenUVCoords | // convert spherical, cylindrical, box and planar mapping to proper UVs
	//	aiProcess_TransformUVCoords | // preprocess UV transformations (scaling, translation ...)
	//	aiProcess_FindInstances | // search for instanced meshes and remove them by references to one master
	//	aiProcess_LimitBoneWeights | // limit bone weights to 4 per vertex
	//	aiProcess_OptimizeMeshes | // join small meshes, if possible;
	//	aiProcess_SplitByBoneCount | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
	//	aiProcess_GenSmoothNormals | // generate smooth normal vectors if not existing
	//	aiProcess_SplitLargeMeshes | // split large, unrenderable meshes into submeshes
	//	aiProcess_Triangulate | // triangulate polygons with more than 3 edges
	//	//aiProcess_ConvertToLeftHanded | // convert everything to D3D left handed space
	//	aiProcess_SortByPType | // make 'clean' meshes which consist of a single typ of primitives);
	//	//aiProcess_FlipWindingOrder |
	//	0;
	////aiPropertyStore* props = aiCreatePropertyStore();
	////aiSetImportPropertyInteger(props, AI_CONFIG_IMPORT_TER_MAKE_UVS, 1);
	////aiSetImportPropertyFloat(props,AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE,g_smoothAngle);
	////aiSetImportPropertyInteger(props,AI_CONFIG_PP_SBP_REMOVE,nopointslines ? aiPrimitiveType_LINE | aiPrimitiveType_POINT : 0 );

	////aiSetImportPropertyInteger(props,AI_CONFIG_GLOB_MEASURE_TIME,1);
	////aiSetImportPropertyInteger(props,AI_CONFIG_PP_PTV_KEEP_HIERARCHY,1);

	//scene = const_cast<aiScene*>(importer.ReadFile(filepath, ppsteps));
	// If the import failed, report it
	if (!scene)
	{
		logInfo(importer.GetErrorString());
		fprintf(stderr, "ERROR: reading mesh \n");
		return NULL;
	}

	// Now we can access the file's contents.
	logInfo("Import of scene " + filepath + " succeeded.");

	printf("  %i animations\n", scene->mNumAnimations);
	printf("  %i cameras\n", scene->mNumCameras);
	printf("  %i lights\n", scene->mNumLights);
	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);
	sMax = Vector3(-1E15, -1E15, -1E15);
	sMin = Vector3(1E15, 1E15, 1E15);
	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* aimesh = scene->mMeshes[m_i];
		printf("    %i vertices in mesh\n", aimesh->mNumVertices);
		as_Geometry* geometry =new as_Geometry;;
		LoadMeshBasicAttributes(aimesh, geometry, sMax, sMin);

		//如果mesh没有名字，则自动根据文件名生成名字
		{
			char buffer[200];
			string filename = getPureFileName(filepath);
			if (m_i>0)
				sprintf(buffer, "%s_%d", filename.c_str(), m_i);
			else
				sprintf(buffer, "%s", filename.c_str());
			//sprintf(buffer, "%s_%d", filename.c_str(), m_i);
			//if (geometry->name == "") geometry->name = string(buffer);
			geometry->name = string(buffer);
		}
		geo.push_back(geometry);
		as_Mesh* mymesh = new as_Mesh;
		mymesh->name = geometry->name;
		mymesh->geometry = geometry;
		mymesh->materialID = aimesh->mMaterialIndex;
		meshs.push_back(mymesh);
	}
	if (scene->HasAnimations()){
		as_Skeleton* skeleton = new as_Skeleton;
		bool ok=LoadSceneSkeleton(scene, skeleton);
		if (!ok)
			goto READ_MATERIAL;
		skeletons.push_back(skeleton);
		FillBoneIdWeights(scene, skeleton, geo);

		for (auto& g : geo){
			g->mSkeleton = skeleton;
			g->has_skeleton = 1;
		}

		for (int i = 0; i < scene->mNumAnimations; i++){
			as_SkeletonAnimation* animation = new as_SkeletonAnimation;
			readAnimation(scene->mAnimations[i], animation);
			ani.push_back(animation);
		}
	}
	READ_MATERIAL:
	if (scene->HasMaterials()){
		for (int i = 0; i < scene->mNumMaterials; i++){
			as_Material* mat = new as_Material;
			readMaterial(scene->mMaterials[i], mat);
			materials.push_back(mat);
		}
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			meshs[i]->material = materials[meshs[i]->materialID];
		}
	}

	return true;
}

