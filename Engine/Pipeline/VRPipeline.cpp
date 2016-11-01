#include "VRPipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"




void VRPipeline::Init()
{
	eForward.input_scenename = "scene1";
	eForward.input_cameraname = "main";
	eForward.Init();
	eShowtex.input_texture = eForward.out_color;
	eShowtex.Init();
}

void VRPipeline::Render()
{
	eForward.Render();
	eShowtex.Render();
}
