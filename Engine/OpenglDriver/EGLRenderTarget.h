#pragma once
#include "OPENGL_Include.h"

#include "RenderTarget.h"


namespace HW
{
	class EGLRenderTarget :
		public RenderTarget
	{
	public:
		EGLRenderTarget();
		~EGLRenderTarget();

		virtual void createInternalRes();
		virtual void releaseInternalRes();
		virtual void bindTarget(int index = 0);

		virtual void attachTextureCubeMapNow(Texture* cubetexture, string attach_component, int face);
		virtual void attachTexture(Texture*, string attach_component);

		virtual void RebindAttachTextures_Internal();
		virtual void BlitTexture(RenderTarget* out, string SrcAttach, string DstAttach);
		GLuint m_framebuffer_id;
	private:
		
		void createFrameBuffer();
		GLenum getAttach(string attach);

	};

}
class  EGLRenderTargetFactory:public RenderTargetFactory
{
public:
	virtual RenderTarget * create() {
		return new EGLRenderTarget();
	}

};