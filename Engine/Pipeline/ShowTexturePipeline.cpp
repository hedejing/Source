#include "ShowTexturePipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "RenderUtil.h"


void ShowTextureEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	mainwindow = rt_mgr.get("MainWindow");
	quad_pass = PassManager::getInstance().LoadPass("quad_prog.json");
}

void ShowTextureEffect::Render()
{
	RenderSystem* mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	//get a screen quad
	auto quad = RenderUtil::GetScreenQuad();


	if (input_texture!=NULL)
		quad_pass->mProgram->setProgramConstantData("test_tex", input_texture);
	if (input_texture_cube!=NULL)
		quad_pass->mProgram->setProgramConstantData("test_tex_cube", input_texture_cube);
	
	mRenderSystem->RenderPass(NULL, quad, quad_pass, mainwindow);
}
