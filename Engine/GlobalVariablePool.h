#pragma once
#include "Mathlib.h"
#include "GlobalVariableSemantics.h"
#include <vector>
#include <map>
#include "engine_struct.h"
#include "PreDefine.h"
namespace HW
{

	struct _gbvStruct
	{
		// normal
		Matrix4 WorldMatrix;
		Matrix4 ViewMatrix;
		Matrix4 ProjectMatrix;
		Matrix4 WorldViewMatrix;
		Matrix4 ViewProjMatrix;
		Matrix4 WorldViewProjMatrix;
		// transpose
		Matrix4 TrsWorldMatrix;
		Matrix4 TrsViewMatrix;
		Matrix4 TrsProjectMatrix;
		Matrix4 TrsWorldViewMatrix;
		Matrix4 TrsViewProjMatrix;
		Matrix4 TrsWorldViewProjMatrix;
		// inverse
		Matrix4 InvWorldMatrix;
		Matrix4 InvViewMatrix;
		Matrix4 InvProjectMatrix;
		Matrix4 InvWorldViewMatrix;
		Matrix4 InvViewProjMatrix;
		Matrix4 InvWorldViewProjMatrix;

		//float    ProgramDurationTime; // 程序持续时间;
		//float    ElapseDeltaTime;   // delta 时间;

		bool     WorldViewMatrixDirty;
		bool     ViewProjMatrixDirty;
		bool     WorldViewProjMatrixDirty;

		bool     TrsWorldMatrixDirty;
		bool     TrsViewMatrixDirty;
		bool     TrsProjectMatrixDirty;
		bool     TrsWorldViewMatrixDirty;
		bool     TrsViewProjMatrixDirty;
		bool     TrsWorldViewProjMatrixDirty;

		bool     InvWorldMatrixDirty;
		bool	 InvViewMatrixDirty;
		bool	 InvProjectMatrixDirty;
		bool     InvWorldViewMatrixDirty;
		bool     InvViewProjMatrixDirty;
		bool     InvWorldViewProjMatrixDirty;

	};
	/** one semantic has a block of memory.
	*/
	class _global_variable_descrip
	{
	public:
		/** constructor with element size
		*/
		explicit _global_variable_descrip(unsigned int elemsize);
		/** destructor
		*/
		~_global_variable_descrip();
		/** set the value of this element
			@remarks
				one semantic can be used to identify several variables with different indices.
			for example,SV_TARGET is a semantic,but we can identify a variable using 'SV_TARGET0',
			and identify another variable using 'SV_TARGET1'.here we also support this convention.
			@param semantic the semantic id of this variable,the low 16 bits indicate the index.
			@param size  the size of the value
			@param valptr a pointer to the address of this value.
		*/
		void SetVal(unsigned int semantic,unsigned int size,const void* valptr,unsigned int element_num = 1);
		/** get the value of this element
		*/
		void GetVal(unsigned int semantic,unsigned int size,void *valptr,unsigned int element_count = 1);
	private:
		/** number of elements for this semantic
		*/
		unsigned int m_NumElems;
		/** the size of each elements,all elements should has the same size
		*/
		unsigned int m_ElemSize;
		/** a pointer to a block of memory.
		*/
		void *memptr;

	};


	class GlobalVariablePool
	{
	public:
		/** for singleton pattern
		*/
		static GlobalVariablePool* GetSingletonPtr();
		static GlobalVariablePool& GetSingleton();
		/** destructor
		*/
		~GlobalVariablePool();
		/** get variable value by semantic
		*/
		void GetVal(unsigned int semantic,unsigned int size,void *valptr,unsigned int element_count = 1) const;

		/** set current world matrix,this function is just for convenience
		*/
		void SetWorldMatrix(const Matrix4 &mat);
		/** set current view matrix,this function is just for convenience
		*/
		void SetViewMatrix(const Matrix4& mat);
		/** set current projection matrix,this function is just for convenience
		*/
		void SetProjectMatrix(const Matrix4 &mat);
		/** a common function for setting values for the variables
		*/
		void SetVal(unsigned int semantic,unsigned int size,void *valptr);
		void SetMaterial(as_Material * material);
		void SetMaterialTexture(GlobalVariableSemantic sem, string tex_type, as_Material * material);
		void SetMaterialValue(GlobalVariableSemantic sem, Vector4 v);
		void SetMaterialValue(GlobalVariableSemantic sem, int v);
		void SetLight(Light * light, unsigned int index);
		void setEntity(Entity * entity);
		void setMatrices(vector<Matrix4>&  mats);
		void SetCamera(Camera * camera);
		void initialize();
	private:
		/** constructor 
		*/
		GlobalVariablePool();
		/** 
		*/
		static GlobalVariablePool* m_globalpool;
		/** a struct stores matrices
		*/
		mutable _gbvStruct m_VariablePool;
		/** 
		*/
		std::map<GlobalVariableSemantic,_global_variable_descrip*> m_SemanticValueMap;
	};

}
