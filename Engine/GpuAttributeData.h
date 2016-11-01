#ifndef _GPU_ATTRIBUTE_DATA_H_
#define _GPU_ATTRIBUTE_DATA_H_

namespace HW{
	class RenderSystem;
	class Renderable;
	class InputLayout;

	/* This Class is used to store Vertices data and Indices data
	**/
	class GpuAttributeData{
	public:
		GpuAttributeData(){ m_bResourceValid = false; };
		virtual ~GpuAttributeData(){};
		// Create internal resources for Specific platform, EX. ID3D11VertexBuffer* for DX11
		virtual bool CreateInternalRes(RenderSystem* renderSystem, Renderable* renderable, InputLayout* layout) = 0;
		virtual void ReleaseInternalRes() = 0;
		// Draw the vertices with indices
		virtual void Draw() = 0;
		// Update the Vertex Data during rendering
		virtual bool UpdateInternalRes(RenderSystem* renderSystem, Renderable* renderable) = 0;

	protected:
		bool m_bResourceValid;
	};
}

#endif