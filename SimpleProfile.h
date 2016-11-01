#pragma once
#include <string>
#include <chrono>
#include <vector>
#include <sstream>
using namespace std;


class SimpleProfile {
public:
	SimpleProfile() {
		enable = true;
	}
	void NewFrame() {
		//if (!enable) return;		
		timePoints.clear();
		durations.clear();
		pointNames.clear();
		timePoints.push_back(chrono::steady_clock::now());
		pointNames.push_back("");
	}

	void Tick(string s) {
		//if (!enable) return;
		timePoints.push_back(chrono::steady_clock::now());
		pointNames.push_back(s);
		int i = timePoints.size() - 1;
		chrono::duration<double> elapsed_seconds = timePoints[i] - timePoints[i - 1];
		double t = 1000 * (elapsed_seconds).count();
		durations.push_back(t);
		//float scale = 0.03;
		//if (history.size() < i)
		//	history.push_back(t);
		//else
		//	history[i - 1] = t*scale +(1-scale)*history[i-1];
	}
	double GetTime(string s) {
		if (!enable) return 0;
		double t = 0;
		for (int i = 1; i < timePoints.size(); i++) {
			if (pointNames[i] == s) {
				t=durations[i - 1];
				break;
			}
		}
		return t;
	}
	double GetTotal() {
		//if (!enable) return 0;
		int i = timePoints.size() - 1;
		double t = 0;
		if (i >= 0) {
			chrono::duration<double> elapsed_seconds = timePoints[i] - timePoints[0];
			t = 1000*(elapsed_seconds).count();
		}
		return t;
	}

	string ToString() {
		//if (!enable) return "";
		stringstream ss;
		ss.precision(4);
		double total = GetTotal();
		ss << "TotalTime " << total << "ms" << endl;
		for (int i = 1; i < pointNames.size(); i++) {
			ss << pointNames[i] << " " << durations[i-1] << "ms" << endl;
		}
		return ss.str();
	}


	bool enable = true;

	vector<chrono::steady_clock::time_point> timePoints;
	vector<string> pointNames;
	vector<double> durations;
	vector<double> history;
};