#pragma once
#include "mathlib.h"
using namespace HW;

template<typename T>
T CosineInterpolate(
	T y1, T y2,
	double mu)
{
	double mu2;

	mu2 = (1 - cos(mu*PI)) / 2;
	return(y1*(1 - mu2) + y2*mu2);
}

template<typename T>
T CubicInterpolate(
	T y0, T y1,
	T y2, T y3,
	double mu)
{
	T a0, a1, a2, a3;
	double mu2;

	mu2 = mu*mu;
	a0 = y3 - y2 - y0 + y1;
	a1 = y0 - y1 - a0;
	a2 = y2 - y0;
	a3 = y1;

	return(a0*mu*mu2 + a1*mu2 + a2*mu + a3);
}
template<typename T>
T CatmullRomspline(
	T y0, T y1,
	T y2, T y3,
	double mu)
{
	double a0, a1, a2, a3, mu2;

	mu2 = mu*mu;
	a0 = -0.5*y0 + 1.5*y1 - 1.5*y2 + 0.5*y3;
	a1 = y0 - 2.5*y1 + 2 * y2 - 0.5*y3;
	a2 = -0.5*y0 + 0.5*y2;
	a3 = y1;

	return(a0*mu*mu2 + a1*mu2 + a2*mu + a3);
}
template<typename T>
T HermiteInterpolate(
	T y0, T y1,
	T y2, T y3,
	double mu,
	double tension,
	double bias)
{
	T m0, m1;
	double mu2, mu3;
	double a0, a1, a2, a3;

	mu2 = mu * mu;
	mu3 = mu2 * mu;
	m0 = (y1 - y0)*(1 + bias)*(1 - tension) / 2;
	m0 += (y2 - y1)*(1 - bias)*(1 - tension) / 2;
	m1 = (y2 - y1)*(1 + bias)*(1 - tension) / 2;
	m1 += (y3 - y2)*(1 - bias)*(1 - tension) / 2;
	a0 = 2 * mu3 - 3 * mu2 + 1;
	a1 = mu3 - 2 * mu2 + mu;
	a2 = mu3 - mu2;
	a3 = -2 * mu3 + 3 * mu2;

	return(a0*y1 + a1*m0 + a2*m1 + a3*y2);
}

class Interpolation {
public:
	void SetData(vector<double>& x, vector<Vector3>& y){
		this->x = x;
		this->y = y;
		n = x.size();
		if (n < 2) {
			cout << "error:interp  must have multi points";
			exit(1);
		}
	}
	Vector3 GetCosineInterpolation(double t) {
		if (t <= x[0]) return y[0];
		if (t >= x[n - 1]) return y[n - 1];
		int i;
		for (i = 0; i < n; i++) {
			if (x[i] > t) break;
		}
		double t2 = (t - x[i - 1]) / (x[i] - x[i - 1]);
		Vector3 r = CosineInterpolate<Vector3>(y[i - 1], y[i], t2);
		return r;
	}
	Vector3 GetInterpolation(double t) {
		if (t <= x[0]) return y[0];
		if (t >= x[n - 1]) return y[n - 1];
		int i;
		for (i = 0; i < n; i++) {
			if (x[i] > t) break;
		}
		double t2 = (t - x[i - 1]) / (x[i] - x[i - 1]);
		Vector3 y0 = i - 2 >= 0 ? y[i - 2] : y[0];
		Vector3 y3 = i + 1 < n ? y[i + 1] : y[n - 1];
		Vector3 r = HermiteInterpolate(y0, y[i - 1], y[i], y3, t2, 0, 0);
		return r;
	}
	Vector3 GetDerivative(double t, double step=0.001) {
		if (t <= x[0]+step) return (GetInterpolation(x[0]+step)-y[0])/step;
		if (t >= x[n - 1]-step) return (y[n - 1]-GetInterpolation(x[n-1]-step))/step;
		auto v0=GetInterpolation(t - step);
		auto v1=GetInterpolation(t + step);
		return (v1 - v0)/step*0.5;
	}
private:
	int n;
	vector<double> x;
	vector<Vector3> y;
};