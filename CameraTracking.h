#pragma once
#include "Camera.h"
#include <fstream>
#include <stdlib.h>
#include "KeyFrame.h"
#define PI 3.141592654
class CameraTrack {
public:
	vector<TranslationKeyFrame> keys;

	Vector3 pos = Vector3(0, 0, 0);
	Vector3 LastPos = Vector3(0, 0, 0);
	Vector3 d;
	Quaternion q = Quaternion::IDENTITY;
	int frame = 0;
	SceneNode* Node = NULL;

	void Track(Vector3 newpos) {
		frame++;
		LastPos = pos;
		pos = newpos;
		Vector3 tmpd = newpos - LastPos;
		if (tmpd.length() < 1e-5)
			return;
		d = newpos - LastPos;
		d.normalize();

	}
	void UpdateNode() {
		Node->setTranslation(GetPos());
		Node->setOrientation(GetOrientation());
	}
	Vector3 GetPos() {
		return pos;
	}

	Quaternion GetOrientation() {
		Vector3 d0 = Vector3(0, 0, 1);
		if ((d - d0).length() < 1e-5) {
			q.fromAngleAxis(0, Vector3(0, 1, 0));
		}
		else
		{
			float c = d.dotProduct(d0);
			float angle = acos(c);
			float clockwise = 1;
			if (d.x < 0)
				clockwise = -1;
			q.fromAngleAxis(clockwise*angle, Vector3(0, 1, 0));
		}
		return q;
	}
};
