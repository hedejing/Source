#pragma once

class Pipeline{
public:
	virtual void Init() = 0;
	virtual void Render() = 0;
};

class SimplePipeline :public Pipeline{
public:
	virtual void Init();
	virtual void Render();

};

