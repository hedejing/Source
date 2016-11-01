#ifndef LOONG_RENDERSYSTEMENUM_H
#define LOONG_RENDERSYSTEMENUM_H

namespace HW
{

	enum PrimType
	{
		PRIM_TYPE_POINTS = 0x34,
		PRIM_TYPE_LINES = 0x35,
		PRIM_TYPE_LINE_LOOP = 0x36,
		PRIM_TYPE_LINE_STRIP = 0x37,
		PRIM_TYPE_TRIANGLES = 0x38,
		PRIM_TYPE_TRIANGLE_STRIP = 0x39,
		PRIM_TYPE_TRIANGLE_FAN = 0x3A
	};

	/**enumeration of fill mode
	*/
	enum FillMode
	{
		FM_WIREFRAME,
		FM_SOLID
	};
	/** enumerations in this file is used by the render system
	*/

	/** enumerations for cull model 
	*/
	enum CullMode
	{
		CULL_MODE_NONE = 				0x1,
		CULL_MODE_FRONT = 				0x2,
		CULL_MODE_BACK = 				0x3,
		CULL_MODE_FRONT_AND_BACK
	};

	/** blend operand
	*/
	enum BlendOperand
	{
		BLEND_ZERO = 			0x4,
		BLEND_ONE = 			0x5,
		BLEND_SRC_COLOR = 		0x6,
		BLEND_INV_SRC_COLOR = 	0x7,
		BLEND_SRC_ALPHA = 		0x8,
		BLEND_INV_SRC_ALPHA = 	0x9,
		BLEND_DEST_ALPHA = 		0xA,
		BLEND_INV_DEST_ALPHA = 	0xB,
		BLEND_DEST_COLOR = 		0xC,
		BLEND_INV_DEST_COLOR = 	0xD,
		BLEND_SRC_ALPHA_SAT = 	0xE,
		BLEND_BLEND_FACTOR = 	0xF,
		BLEND_INV_BLEND_FACTOR = 0x10,
		BLEND_SRC1_COLOR = 		0x11,
		BLEND_INV_SRC1_COLOR = 	0x12,
		BLEND_SRC1_ALPHA = 		0x13,
		BLEND_INV_SRC1_ALPHA = 	0x14
	};

	enum BUFFER_STORE
	{
		BS_STATIC = 0x0,
		BS_DYNAMIC = 0x1,
		BS_STREAM = 0x2

	};
	enum BlendOperation
	{
		BLEND_OP_ADD,
		BLEND_OP_SUBTRACT,
		BLEND_OP_REV_SUBTRACT,
		BLEND_OP_MIN,
		BLEND_OP_MAX
	};
	/** 
	*/
	enum DepthWriteMask//DEPTH_WRITE_MASK
	{
		DEPTH_WRITE_MASK_ZERO = 	0x22,
		DEPTH_WRITE_MASK_ALL = 	0x23
	};

	enum StencilOperation//STENCIL_OPERATION
	{
		STENCIL_OP_KEEP = 		0x24,
		STENCIL_OP_ZERO = 		0x25,
		STENCIL_OP_REPLACE = 		0x26,
		STENCIL_OP_INCR_SAT = 	0x27,
		STENCIL_OP_DECR_SAT = 	0x28,
		STENCIL_OP_INVERT = 		0x29,
		STENCIL_OP_INCR = 		0x2A,
		STENCIL_OP_DECR = 		0x2B
	};

	enum FrameBuffer
	{
		FRAME_BUFFER_ALL = 0x0,
		FRAME_BUFFER_COLOR = 0x1,
		FRAME_BUFFER_DEPTH = 0x2,
		FRAME_BUFFER_STENCIL = 0x4
	};
	enum HardwareUsage
	{
		HARDWARE_USAGE_DEFAULT = 1,
		HARDWARE_USAGE_DYNAMIC = 2,
		HARDWARE_USAGE_IMMUTABLE = 3,
		HARDWARE_USAGE_STAGING = 4
	};

	enum SamplerFilter
	{
		FILTER_POINT = 1,
		FILTER_LINEAR = 2,
		FILTER_ANISOTROPIC = 3
	};

	enum SamplerAddressMode
	{
		ADDRESS_MODE_WRAP = 1,
		ADDRESS_MODE_MIRROR = 2,
		ADDRESS_MODE_CLAMP = 3,
		ADDRESS_MODE_BORDER = 4,
		ADDRESS_MODE_MIRROR_ONCE = 5
	};

	enum ComparisonFunc
	{
		COMP_NEVER = 				0x1A,
		COMP_LESS = 				0x1B,
		COMP_EQUAL = 				0x1C,
		COMP_LESS_EQUAL = 		0x1D,
		COMP_GREATER = 			0x1E,
		COMP_NOT_EQUAL= 			0x1F,
		COMP_GREATER_EQUAL = 		0x20,
		COMP_ALWAYS = 			0x21
	};

	struct ScissorRect
	{
		int left; 
		int top;
		int right;
		int bottom;
	};
}
#endif