#include "Pipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"



void SimplePipeline::Init()
{
	//default pass
	PassManager::getInstance().LoadPass("forward_prog.json");
}
void SimplePipeline::Render()
{
	SceneManager* scene = SceneContainer::getInstance().get("scene1");
	auto camera = scene->getCamera("main");

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);//暂时禁用了octree的剔除


	RenderTarget * mainwindow = RenderTargetManager::getInstance().get("MainWindow");
	as_Pass* forward_pass = PassManager::getInstance().get("forward pass");

	RenderSystem* mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;
	mRenderSystem->RenderPass(camera, queue, forward_pass, mainwindow);
}



