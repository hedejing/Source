#pragma once
#include "Camera.h"
#include "mathlib.h"
#include "Interpolation.h"

using namespace HW;

class Pattern {
public:
	Pattern(Camera *_camera)
		:camera(_camera), backup(*_camera) {}
	virtual void UpdateCamera(double t) = 0;
	virtual ~Pattern(){}
	virtual void Reset() {
		*camera = backup;
	}
protected:
	Camera *camera;
	Camera backup;

};

class AngularSpeedTest :public Pattern{
public:
	float PitchSpeed;
	float YawSpeed;
	AngularSpeedTest(Camera *camera, double pitchSpeed, double yawSpeed)
		:Pattern(camera), PitchSpeed(pitchSpeed), YawSpeed(yawSpeed) {};

	void UpdateCamera(double t) {
		camera->RotatePitchYaw(t*PitchSpeed, t*YawSpeed);
	}
};

class LinearSpeedTest :public Pattern {
public:
	float Speed;
	LinearSpeedTest(Camera *camera, float speed)
		:Pattern(camera), Speed(speed) {};

	void UpdateCamera(double t) {
		Vector3 Pos = camera->getPosition();
		Vector3 Dir = camera->getDirection();
		Dir.normalize();
		Pos += Speed*t*Dir;
		camera->setPosition(Pos);
	}
};

class SpeedTest :public Pattern {
public:
	float PitchSpeed;
	float YawSpeed;
	float LinearSpeed;
	float Acceleration;

	SpeedTest(Camera *camera, double pitchSpeed, double yawSpeed, float linearSpeed, float acceleration)
		:Pattern(camera), PitchSpeed(pitchSpeed), YawSpeed(yawSpeed), LinearSpeed(linearSpeed), Acceleration(acceleration){}
	void UpdateCamera(double t) {
		camera->RotatePitchYaw(t*PitchSpeed, t*YawSpeed);
		Vector3 Pos = camera->getPosition();
		Vector3 Dir = camera->getDirection();
		Dir.normalize();
		LinearSpeed += Acceleration*t;
		Pos += LinearSpeed*t*Dir;
		camera->setPosition(Pos);
	}
};

struct InterpData {
	float Time;
	Vector3 Position;
	Vector3 Direction;

	InterpData(float time, Vector3 position, Vector3 direction)
		:Time(time), Position(position), Direction(direction) {}
};

class InterpolationTest :public Pattern {
public:
	InterpolationTest(Camera *camera, const vector<InterpData>& datas)
		:Pattern(camera) {
		int n = datas.size();
		vector<double> times(n, 0);
		vector<Vector3> positions(n), directions(n);
		for (int i = 0; i < n;i++) {
			times[i] = datas[i].Time;
			positions[i] = datas[i].Position;
			directions[i] = datas[i].Direction;
		}

		PosInterp.SetData(times, positions);
		DirInterp.SetData(times, directions);
		AbsolutTime = 0;
		hasDir = true;
	}

	InterpolationTest(Camera *camera, const vector<double>& times, const vector<Vector3> &positions, const vector<Vector3> &directions)
		:Pattern(camera){
		
		assert(positions.size() == directions.size() && times.size() == positions.size());

		PosInterp.SetData(times, positions);
		DirInterp.SetData(times, directions);
		AbsolutTime = 0;
		hasDir = true;
	}

	InterpolationTest(Camera *camera, const vector<Vector3> &positions, const vector<Vector3> &directions)
		:Pattern(camera) {

		assert(positions.size() == directions.size());

		int n = positions.size();
		vector<double> times(n, 0);
		for (int i = 1; i < n; i++)
			times[i] = times[i - 1] + 1;

		PosInterp.SetData(times, positions);
		DirInterp.SetData(times, directions);
		AbsolutTime = 0;
		hasDir = true;
	}

	InterpolationTest(Camera *camera, const vector<Vector3> &positions)
		:Pattern(camera) {

		int n = positions.size();
		vector<double> times(n, 0);
		for (int i = 1; i < n; i++)
			times[i] = times[i - 1] + 1;

		PosInterp.SetData(times, positions);
		AbsolutTime = 0;
		hasDir = false;
	}

	void UpdateCamera(double t) {
		AbsolutTime += t;
		camera->setPosition(PosInterp.GetInterpolation(AbsolutTime));
		if (hasDir)
			camera->setDirection(DirInterp.GetInterpolation(AbsolutTime));
		else
			camera->setDirection(PosInterp.GetDerivative(AbsolutTime));
	}

	void Reset() {
		AbsolutTime = 0;
		*camera = backup;
	}

private:
	Interpolation PosInterp, DirInterp;
	bool hasDir;
	double AbsolutTime;
};