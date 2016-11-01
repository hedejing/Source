#pragma once
#include "PreDefine.h"
#include <vector>
#include <string>
using namespace std;


class PipelineEffect{

public:
	virtual void Init() = 0;
	virtual void Render() = 0;

};