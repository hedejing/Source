#pragma once
#include<string>
#include "mathlib.h"
#include "RenderSystemEnum.h"
#include "SceneManager.h"
#include "Logger.h"
#include "Geometry.h"
#include "RenderState.h"
#include "PreDefine.h"

namespace HW
{
	class RenderSystem
	{
	public:
		RenderSystem(){}

		virtual ~RenderSystem() {}

		/** Set the width and height of client window
		*/
		virtual void SetWandH(int w, int h) = 0;
		virtual void GetWandH(int& w, int& h) = 0;

		/** initialize the render system. create device and device context.
		*/
		virtual bool  Initialize() = 0;

		virtual void InitFactory() = 0;
		/** bind an input layout to the render system.
			@remarks
				we should tell the system the variable layout of the input vertex data
			before vertex data are streamed into the pipeline.
			@param layout  a struct describes the input variables' layout in memory.
		*/
		virtual void  BindInputLayout(InputLayout *layout) = 0;
		/** set primitive type for the to be rendered geometry.
		*/
		virtual void  SetPrimType(PrimType geoType = PRIM_TYPE_TRIANGLES) = 0;
		/** bind a gpu program to the system
		*/
		virtual void  BindGpuProgram(GpuProgram *program) = 0;
		/** a switch to decide if wire frame is applied.
			@param op  a bool value if it is true then wire frame is enabled,vice versa.
		*/
		virtual void EnableWireFrame(bool op = false)=0; 	// if op == true then wire frame enabled,vice versa
		/** set cull model. #see CullModel (file: LoongRenderSystemEnum.h)
		*/
		virtual void SetCullMode(CullMode md = CULL_MODE_BACK)=0;
		/** set which sides of a face will be thought as front face.
			@param clockwise  the direction for a face
		*/
		virtual void SetFaceFront(bool clockwise = false)=0;
		/** decide if multi-sample technique should be applied
		*/
		virtual void EnableMultiSample(bool ms = false)=0;	// if ms == true then multi-sample enabled,vice versa
		/** a switch function to turn on/off the scissor test.
			@param teston  if it is true,then scissor test may be applied when rendering.
				the default value is false.
		*/
		virtual void EnableScissorTest(bool teston = false) = 0;
		/** parameters for scissor test.
		*/
		virtual void SetScissorRect(int tplx,int tply,int btrx,int btry)=0;
		/** set raster state to default.
		*/
		virtual void RasterParamDefault()=0;
		/** set view port parameters
		*/
		virtual void SetViewport(float topLeftX,float topLeftY,float width,float height,float mindepth = 0.0f,float maxdepth = 1.0f)=0;
		/** 
		*/
		virtual void EnableBlendAlphaCoverage(bool bac = false)=0;
		/** enable/disable independent blend when multi-render target is applied.
		*/
		virtual void EnableIndependentBlend(bool ib = false)=0; 	// enable independent blend between multi-render target
		/** set blend parameters
		*/
		virtual void SetBlendParam(unsigned int target_index,BlendOperand srcBlend,
			BlendOperand destBlend,BlendOperation blendOp,BlendOperand srcAlpha,
			BlendOperand destAlpha,BlendOperation alphaOp,unsigned char rtMask)=0;
		/** turn on / off blend for a specific render target.
		*/
		virtual void EnableBlend(unsigned int rendertarget_index,bool blend = false) =0;
		/** set blend parameters to default values.this function is used 
			to recovery the render state.
		*/
		virtual void BlendParamDefault() = 0;
		/** set blend factor.
		*/
		virtual void SetBlendFactor(float const bf[4],unsigned int sampleMask)=0;
		/** set depth test parameter
		*/
		virtual void SetDepthParameters(ComparisonFunc func,bool mask)=0; 
		/** enable/disable depth test
		*/
		virtual void EnableDepthTest(bool teston = true) = 0;
		/** set stencil parameters when stencil test is applied.
		*/
		virtual void SetStencilParameters(unsigned char readmask,unsigned char writemask,StencilOperation frontFailOp,
			StencilOperation frontdepthfailOp,StencilOperation frontPassOp,ComparisonFunc front,
			StencilOperation backFailOp,StencilOperation backdepthfailOp,StencilOperation backPassOp,
			ComparisonFunc back,unsigned refv)=0;
		/** enable/disable stencil test
		*/
		virtual void EnableStencil(bool teston = false) = 0; 
		/** set depth and stencil buffer parameters to default.
		*/
		virtual void DepthStencilDefault()=0;
		/** set all parameters within this pipeline to default values.
		*/
		virtual void AllParamDefault()=0;
		/** set back ground color.the default value is Vector4f::ZERO
		*/
		virtual void SetBackground(const Vector4& background) = 0;
		/** 
		*/
		virtual void DepthClearParam(float depth = 1.0f,unsigned char stencil = 0)=0;
		/** do some internal initialization before actual rendering.
		*/
		virtual bool PreRender()=0;
		/** swap back buffer to the front.and present the frame buffer to the monitor.
		*/
		virtual void BufferSwap()=0;
		/** clear frame buffer,input bitwise #FrameBuffer
		*/
		virtual void ClearBuffer(ClearState& c)=0;
		/** do some internal recovery after rendering.
		*/
		virtual bool PostRender()=0; 
		/** return the name of this render system.
		*/
		virtual string GetName() const = 0;
		/** push back a render target to a stack.
		*/
		virtual void PushRenderTarget(RenderTarget *rt)=0;
		/** pop a render target from the stack.
		*/
		virtual void PopRenderTarget()=0;
		/** 
		*/
		virtual RenderTarget* CurrentRenderTarget()const =0;

		virtual void ClearColor(float r, float g, float b, float a) = 0;

		virtual void setClearState(const ClearState &clearstate) = 0;
		virtual void setBlendState(const BlendState &blendstate) = 0;
		virtual void setRasterState(const RasterState & rasterstate) =0 ;
		virtual void setDepthStencilState(const DepthStencilState & depthstencilstate)=0;

		virtual void CreateGeometryGpuData(as_Geometry* rawdata, VertexDataDesc* inputlayout, Geometry*& geo){}
		virtual void DrawGeometry(Geometry* geo){}
		virtual void DrawGeometryInstance(Geometry* geo,int num) {}
		virtual void SetTexture(Texture* texture, int index){}

		void RenderPass(Camera* camera, RenderQueue & renderqueue, as_Pass* pass, RenderTarget * rt);
		virtual void setPolygonOffset(bool enable, float offset_factor = 0.0, float offset_unit = 0.0)=0;

		virtual void term_display() = 0;
		
	};
}
