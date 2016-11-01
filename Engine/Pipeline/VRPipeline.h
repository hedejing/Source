#pragma once
#include "effect.h"
#include "PreDefine.h"
#include <string>
#include "Pipeline.h"
#include "ForwardPipeline.h"
#include "ShowTexturePipeline.h"

using namespace std;
using namespace HW;



class VRPipeline :public Pipeline {
public:
	virtual void Init();
	virtual void Render();

	ForwardEffect eForward;
	ShowTextureEffect eShowtex;
};