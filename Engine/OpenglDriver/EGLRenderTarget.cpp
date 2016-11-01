#include "EGLRenderTarget.h"
#include "EGLUtil.h"
#include "EGLTexture.h"
namespace HW{
	EGLRenderTarget::EGLRenderTarget() :RenderTarget()
	{
	}


	EGLRenderTarget::~EGLRenderTarget()
	{
	}


	void EGLRenderTarget::createInternalRes()
	{
		glGenFramebuffers(1, &m_framebuffer_id);
		
		RebindAttachTextures_Internal();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
	}

	void EGLRenderTarget::releaseInternalRes()
	{
		glDeleteFramebuffers(1, &m_framebuffer_id);
	}

	void EGLRenderTarget::bindTarget(int index /*= 0*/)
	{
		if (!m_valid)
			createInternalRes();
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_id);
		GL_CHECK();
	}

	void EGLRenderTarget::createFrameBuffer()
	{
		glGenFramebuffers(1, &m_framebuffer_id);

	}

	GLenum EGLRenderTarget::getAttach(string attach)
	{
		GLenum result;
		if (attach == "color0") result = GL_COLOR_ATTACHMENT0;
		if (attach == "color1") result = GL_COLOR_ATTACHMENT1;
		if (attach == "color2") result = GL_COLOR_ATTACHMENT2;
		if (attach == "color3") result = GL_COLOR_ATTACHMENT3;
		if (attach == "color5") result = GL_COLOR_ATTACHMENT5;
		if (attach == "color6") result = GL_COLOR_ATTACHMENT6;
		if (attach == "color7") result = GL_COLOR_ATTACHMENT7;
		if (attach == "color8") result = GL_COLOR_ATTACHMENT8;
		if (attach == "depth")  result = GL_DEPTH_ATTACHMENT;
		return result;
	}

	void EGLRenderTarget::attachTextureCubeMapNow(Texture* cubetexture, string attach_component, int face)
	{
		attachTexture(cubetexture, attach_component);
		bindTarget();
		cubetexture->useTexture();
		EGLTexture* texture = static_cast<EGLTexture*>(cubetexture);
		GLenum attach = getAttach(attach_component);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attach, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture->texture_id, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		GL_CHECK();
	}

	void EGLRenderTarget::attachTexture(Texture* tex, string attach_component)
	{
		RenderTarget::attachTexture(tex, attach_component);
		//RebindAttachTextures_Internal();
	}

	void EGLRenderTarget::RebindAttachTextures_Internal()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_id);
		vector<GLenum> DrawBuffers;
		for (auto it : m_attach_textures) {
			EGLTexture* texture = static_cast<EGLTexture*>(it.second);
			string name = texture->name;

			GLenum attach = getAttach(it.first);
			if (attach != GL_DEPTH_ATTACHMENT) {
				DrawBuffers.push_back(attach);
			}
			texture->mipmap_mode = Texture::TextureMipmap_None;
			
			auto AttachTarget = GL_TEXTURE_2D;
			if (texture->MSAASampleNum > 1) 
				AttachTarget = GL_TEXTURE_2D_MULTISAMPLE;

			texture->useTexture();
			//Õë¶Ôdepth stencil texture
			if (attach == GL_DEPTH_ATTACHMENT) {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, AttachTarget, texture->texture_id, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, AttachTarget, texture->texture_id, 0);
			}
			else
				glFramebufferTexture2D(GL_FRAMEBUFFER, attach, AttachTarget, texture->texture_id, 0);
		}
		//DrawBuffers.push_back(GL_NONE);
		glDrawBuffers(DrawBuffers.size(), &DrawBuffers[0]);
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		m_valid = true;
		GL_CHECK();
	}

	void EGLRenderTarget::BlitTexture(RenderTarget* out, string SrcAttach, string DstAttach)
	{
#ifndef ANDROID

		EGLRenderTarget* rt = (EGLRenderTarget*)(out);
		int w = out->width,h=out->height;
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer_id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rt->m_framebuffer_id);
		if (SrcAttach == "depth" &&DstAttach == "depth") {
			glBlitFramebuffer(0, 0, width, height, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			return;
		}
		GLenum attach1 = getAttach(SrcAttach);
		GLenum attach2 = getAttach(DstAttach);
		glReadBuffer(attach1);
		glDrawBuffer(attach2);
		glBlitFramebuffer(0, 0, width, height, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
#endif
	}

};