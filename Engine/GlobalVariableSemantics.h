#ifndef LOONG_GLOBALVARIABLE_SEMANTICS_H
#define LOONG_GLOBALVARIABLE_SEMANTICS_H
#include <cassert>
namespace HW
{
	/** we assign a semantic to each variable in this shading system.
		semantics are used to update shader variables.
		semantics mode specification:
		enumeration is unsigned int type,say 32 bits,we use 
		the high 16 bits as the semantic key,and low 16 bits as index
		for example, sometime we may pass two kinds of lights' parameters
		into the shader.we use index to indicate the parameters' ownership.

	*/
	enum GlobalVariableSemantic
	{
		// class 1 matrix
		GVS_WORLD_MATRIX = 0x10000,
		GVS_VIEW_MATRIX = 0x20000,
		GVS_PROJECTION_MATRIX = 0x30000,
		GVS_WORLD_VIEW_MATRIX = 0x40000,
		GVS_VIEW_PROJECTION_MATRIX = 0x50000,
		GVS_WORLD_VIEW_PROJECTION_MATRIX = 0x60000,
		GVS_TRANSPOSE_WORLD_MATRIX = 0x70000,
		GVS_TRANSPOSE_VIEW_MATRIX = 0x80000,
		GVS_TRANSPOSE_PROJECTION_MATRIX = 0x90000,
		GVS_TRANSPOSE_WORLD_VIEW_MATRIX = 0xA0000,
		GVS_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0xB0000,
		GVS_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0xC0000,
		GVS_INVERSE_WORLD_MATRIX = 0xD0000,
		GVS_INVERSE_VIEW_MATRIX = 0xE0000,
		GVS_INVERSE_PROJECTION_MATRIX = 0xF0000,
		GVS_INVERSE_WORLD_VIEW_MATRIX = 0x100000,
		GVS_INVERSE_VIEW_PROJECTION_MATRIX = 0x110000,
		GVS_INVERSE_WORLD_VIEW_PROJECTION_MATRIX = 0x120000,

		// class 2 material
		GVS_MATERIAL_AMBIENT = 0x130000,
		GVS_MATERIAL_DIFFUSE = 0x140000,
		GVS_MATERIAL_SPECULAR = 0x150000,
		GVS_MATERIAL_EMISSIVE = 0x160000,
		GVS_MATERIAL_TRANSPARENT = 0x170000,
		GVS_MATERIAL_OPACITY = 0x180000,
		GVS_MATERIAL_SHININESS = 0x190000,

		// class 3 light
		GVS_ACTIVATE_LIGHTNUM = 0xEF0000,
		 // directional
		GVS_DIRECTIONAL_LIGHT_DIRECTION = 0x1A0000,
		GVS_DIRECTIONAL_LIGHT_DIFFUSE = 0x1B0000,
		GVS_DIRECTIONAL_LIGHT_SPECULAR = 0x1C0000,
		 // point light
		GVS_POINT_LIGHT_POSITION = 0x1D0000,
		GVS_POINT_LIGHT_DIFFUSE = 0x1E0000,
		GVS_POINT_LIGHT_SPECULAR = 0x1F0000,
		GVS_POINT_LIGHT_CONST_ATT = 0x200000,
		GVS_POINT_LIGHT_LINEAR_ATT = 0x210000,
		GVS_POINT_LIGHT_QUAD_ATT = 0x220000,
		 //spot
		GVS_SPOT_LIGHT_POSITION = 0x230000,
		GVS_SPOT_LIGHT_DIRECTION = 0x240000,
		GVS_SPOT_LIGHT_DIFFUSE = 0x250000,
		GVS_SPOT_LIGHT_SPECULAR = 0x260000,
		GVS_SPOT_LIGHT_CONST_ATT = 0x270000,
		GVS_SPOT_LIGHT_LINEAR_ATT = 0x280000,
		GVS_SPOT_LIGHT_QUAD_ATT = 0x290000,
		GVS_SPOT_LIGHT_EXP = 0x2A0000,
		GVS_SPOT_LIGHT_CUTOFF = 0x2B0000,
		GVS_POINT_LIGHT_DISTANCE = 0x2C0000,
		GVS_SPOT_LIGHT_DISTANCE = 0x2D0000,
		// class 4 texture map
		GVS_TEXTURE_OPACITY = 0x2E0000,
		GVS_TEXTURE_DIFFUSE = 0x2F0000,
		GVS_TEXTURE_AMBIENT = 0x300000,
		GVS_TEXTURE_SPECULAR = 0x310000,
		GVS_TEXTURE_EMISSIVE = 0x320000,
		GVS_TEXTURE_HEIGHT = 0x330000,
		GVS_TEXTURE_NORMAL = 0x340000,
		GVS_TEXTRUE_SHININESS = 0x350000,
		GVS_TEXTURE_DISPLACEMENT = 0x360000,
		GVS_TEXTURE_LIGHTMAP = 0x370000,
		GVS_TEXTURE_REFLECTION = 0x380000,
		GVS_TEXTURE_UNKNOWN = 0x390000,
		// class 5 system time
		GVS_SYSTEM_ELAPSE_TIME = 0x3A0000,
		GVS_SYSTEM_DELTA_TIME = 0x3B0000,
		// class 6 camera
		GVS_CAMERA_POSITION = 0x3C0000,
		GVS_CAMERA_DIRECTION = 0x3D0000,
		GVS_CAMERA_UP = 0x3E0000,
		GVS_ENV_AMBIENT = 0x3F0000,

		//class 7 shadowMap;
		GVS_SHADOW_MAP = 0x400000,
		GVS_SHADOW_MATRIX = 0x410000,
		GVS_SHADOW_BIAS = 0x420000,

		//Entity factor ,mainly about animation
		GVS_FRAME_ANIMATION = 0x430000,
		GVS_INTER_FACTOR = 0x440000,
		GVS_BONE_MATRICES = 0x450000,
		
		// Post_Effect parameter;
		GVS_POST_TEXTURE = 0x460000,
		//Deferred Shading Texture
		GVS_DeferredShading_TEXTURE_POSITION = 0x470000,
		GVS_DeferredShading_TEXTURE_DIFFUSE = 0x480000,
		GVS_DeferredShading_TEXTURE_NORMAL = 0x490000,
		GVS_DeferredShading_TEXTURE_TEXCORD = 0x4A0000,

		GVS_HAS_TEXTURE_DIFFUSE,
		//MANUAL_PARAMETERS;
		GVS_MANUAL_PARA

	};
	// helper function 
	// retrieve the key of the semantics.
	inline GlobalVariableSemantic _global_getSemanticKey(unsigned int semantic)
	{
		return GlobalVariableSemantic(semantic&(0xFFFF0000));
	}
	// make semantic from a key value and the index value.
	inline unsigned int _global_makeFromkeyIndex(GlobalVariableSemantic key,unsigned int index)
	{
		assert(index < 0x000000FF);
		index = index & 0x000000FF;
		return (key|index);
	}
	// retrieve index from the semantic
	inline unsigned int _global_getSemanticIndex(unsigned int semantic)
	{
		return (semantic&0x000000FF);
	}

	inline unsigned int _global_makeFromkeyNum(GlobalVariableSemantic key,unsigned int num)
	{
		assert(num < 0x000000FF);
		num = (num & 0x000000FF)<<8;
		return (key|num);
	}

	inline unsigned int _global_getSemanticNum(unsigned int semantic)
	{
		return (semantic & 0x0000ff00)>>8;
	}

	inline unsigned int _global_makeFromkeyIndexandNum(GlobalVariableSemantic key,unsigned int index,unsigned int num)
	{
		assert(index < 0x000000FF);
		index = index & 0x000000FF;
		assert(num < 0x000000FF);
		num = (num & 0x000000FF)<<8;
		return (key|num|index);
	}
	// merely for debugging
}
#endif