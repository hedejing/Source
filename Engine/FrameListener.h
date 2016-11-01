#pragma once

class FrameListener
{

public:
	FrameListener()
	{

	}
	virtual bool frameStarted()
	{
		return true;
	}
	virtual bool frameEnded()
	{
		return true;
	}

};

