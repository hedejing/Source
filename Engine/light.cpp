#include "Light.h"
#include "SceneNode.h"
#include "Camera.h"

// static members
NameGenerator* Light::m_NameGenerator = new NameGenerator("Light");

Light::Light(const string &name, LightType type, SceneManager * mcreator)
	: Movable(name, MT_LIGHT, mcreator),
	m_Type(type),
	m_MaximumDistance(INFINITY)
{
	Diffuse = Vector3::UNIT_SCALE;
	Specular = Vector3::ZERO;
	ConstantAttenuation = 1.0f;
	LinearAttenuation = 0.0f;
	QuadraticAttenuation = 0.0f;
	Position = Vector3::ZERO;
	Direction = Vector3::NEGATIVE_UNIT_Z;
	Exponent = 60.0f;
	Cutoff = 60.0f;
	mCastShadow = false;
	mState = true;
	ShadowBias = 0.0005;
	m_camera = mcreator->CreateCamera(name + "_camera");

	m_camera_valid = false;
	m_shadowmatrix_valid = false;

}

Light::Light() :Movable(), m_Type(LT_UNKNOWN)
{
	m_Name = m_NameGenerator->Generate();
	Diffuse = Vector3::UNIT_SCALE;
	Specular = Vector3::ZERO;
	ConstantAttenuation = 1.0f;
	LinearAttenuation = 0.0f;
	QuadraticAttenuation = 0.0f;
	Position = Vector3::ZERO;
	Direction = Vector3::NEGATIVE_UNIT_Z;
	Exponent = 60.0f;
	Cutoff = M_PI * 0.25;
	mCastShadow = true;
	mState = true;
}

void Light::CollectData(vector<Light*> lights, void*& _p, int& totalsize, int& number)
{
	light_struct* p;

	p = new light_struct[lights.size() + 1];
	number = lights.size();
	//float* p = new float[4 + number*sizeof(light_struct)/4];
	totalsize = sizeof(light_struct)*(number + 1);

	p[0].position[0] = number;
	for (int i = 0; i < lights.size(); i++) {
		auto l = lights[i];

		p[i + 1].position[0] = l->Position.x;
		p[i + 1].position[1] = l->Position.y;
		p[i + 1].position[2] = l->Position.z;
		p[i + 1].intensity[0] = l->Diffuse.x;
		p[i + 1].intensity[1] = l->Diffuse.y;
		p[i + 1].intensity[2] = l->Diffuse.z;
		p[i + 1].direction[0] = l->Direction.x;
		p[i + 1].direction[1] = l->Direction.y;
		p[i + 1].direction[2] = l->Direction.z;
		p[i + 1].exponent = l->Exponent;
		p[i + 1].cutoff = l->Cutoff;
		p[i + 1].maxdist = l->m_MaximumDistance;
		Matrix4 m = l->getShadowMatrix();
		for (int j = 0; j < 16; j++)
			p[i + 1].shadowmat[j] = m.ptr()[j];
	}
	_p = p;
}

void Light::CollectData(void*& _p, int& structsize)
{
	light_struct* p;

	p = new light_struct;

	structsize = sizeof(light_struct);

	auto l = this;
	p[0].position[0] = l->Position.x;
	p[0].position[1] = l->Position.y;
	p[0].position[2] = l->Position.z;
	p[0].intensity[0] = l->Diffuse.x;
	p[0].intensity[1] = l->Diffuse.y;
	p[0].intensity[2] = l->Diffuse.z;
	p[0].direction[0] = l->Direction.x;
	p[0].direction[1] = l->Direction.y;
	p[0].direction[2] = l->Direction.z;
	p[0].exponent = l->Exponent;
	p[0].cutoff = l->Cutoff;
	Matrix4 m = l->getShadowMatrix();
	for (int j = 0; j < 16; j++)
		p[0].shadowmat[j] = m.ptr()[j];

	_p = p;
}

void Light::CollectData(vector<Light*> lights, vector<float>& p, int& float_num, const string& name)
{
	if (name == "position") {
		float_num = 3 * lights.size();
		for (int i = 0; i < lights.size(); i++)
		{
			Light* l = lights[i];
			p.push_back(l->Position.x);
			p.push_back(l->Position.y);
			p.push_back(l->Position.z);

		}
		return;
	}
	if (name == "color") {
		float_num = 3 * lights.size();
		for (int i = 0; i < lights.size(); i++)
		{
			Light* l = lights[i];
			p.push_back(l->Diffuse.x);
			p.push_back(l->Diffuse.y);
			p.push_back(l->Diffuse.z);

		}
		return;
	}
	if (name == "direction") {
		float_num = 3 * lights.size();
		for (int i = 0; i < lights.size(); i++)
		{
			Light* l = lights[i];
			p.push_back(l->Direction.x);
			p.push_back(l->Direction.y);
			p.push_back(l->Direction.z);

		}
		return;
	}
	if (name == "other") {
		float_num = 3 * lights.size();
		for (int i = 0; i < lights.size(); i++)
		{
			Light* l = lights[i];
			p.push_back(l->Exponent);
			p.push_back(l->Cutoff);
			p.push_back(l->m_MaximumDistance);
		}
		return;
	}
}

HW::Matrix4 Light::getShadowMatrix()
{
	if (m_camera_valid == false || m_shadowmatrix_valid == false) {
		Camera* camera = getCamera();
		Matrix4 bias(
			0.5, 0, 0, 0.5,
			0, 0.5, 0, 0.5,
			0, 0, 0.5, 0.5,
			0, 0, 0, 1);

		ShadowMatrix = bias *camera->getProjectionMatrixDXRH() * camera->getViewMatrix();
		ShadowMatrix = ShadowMatrix.transpose();
		m_shadowmatrix_valid = true;
	}
	return ShadowMatrix;
}

Camera* Light::getCamera()
{
	if (m_camera_valid == false) {
		m_camera->lookAt(Position, Position + Direction, Vector3(0, 0, 1));
		float fov = min(90.0, 2.2 * acos(Cutoff) * 180.0 / M_PI);
		m_camera->setPerspective(fov, 1.0, 1.0, 1000);
		m_camera_valid = true;
	}
	return m_camera;
}

float Light::calcMaxdistance()
{
	return m_MaximumDistance;
}

void Light::CalcScissorRect(Camera* camera, float r, Vector4& rect)
{
	// Create a bounding sphere for the light., based on the position
	// and range
	Matrix4 ViewMatrix = camera->getViewMatrix();
	Matrix4 ProjMatrix = camera->getProjectionMatrixDXRH();
	Vector4 centerWS = Vector4(Position.x, Position.y, Position.z, 1.0f);
	float radius = r;
	// Transform the sphere center to view space
	Vector4 centerVS = ViewMatrix * centerWS;
	// Figure out the four points at the top, bottom, left, and
	// right of the sphere
	Vector4 topVS = centerVS + Vector4(0.0f, radius, 0.0f, 0.0f);
	Vector4 bottomVS = centerVS - Vector4(0.0f, radius, 0.0f, 0.0f);
	Vector4 leftVS = centerVS - Vector4(radius, 0.0f, 0.0f, 0.0f);
	Vector4 rightVS = centerVS + Vector4(radius, 0.0f, 0.0f, 0.0f);
	// Figure out whether we want to use the top and right from quad
	// tangent to the front of the sphere, or the back of the sphere
	leftVS.z = leftVS.x < 0.0f ? leftVS.z + radius : leftVS.z - radius;
	rightVS.z = rightVS.x < 0.0f ? rightVS.z - radius : rightVS.z + radius;
	topVS.z = topVS.y < 0.0f ? topVS.z - radius : topVS.z + radius;
	bottomVS.z = bottomVS.y < 0.0f ? bottomVS.z + radius : bottomVS.z - radius;

	float m_fFarClip = -camera->getNear();
	float m_fNearClip = -camera->getFar();
	// Clamp the z coordinate to the clip planes
	leftVS.z = Clamp(leftVS.z, m_fNearClip, m_fFarClip);
	rightVS.z = Clamp(rightVS.z, m_fNearClip, m_fFarClip);
	topVS.z = Clamp(topVS.z, m_fNearClip, m_fFarClip);
	bottomVS.z = Clamp(bottomVS.z, m_fNearClip, m_fFarClip);
	// Figure out the rectangle in clip-space by applying the
	// perspective transform. We assume that the perspective
	// transform is symmetrical with respect to X and Y.
	float rectLeftCS = -leftVS.x * ProjMatrix[0][0] / leftVS.z;
	float rectRightCS = -rightVS.x * ProjMatrix[0][0] / rightVS.z;
	float rectTopCS = -topVS.y * ProjMatrix[1][1] / topVS.z;
	float rectBottomCS = -bottomVS.y * ProjMatrix[1][1] / bottomVS.z;

	//Vector4 ql = ProjMatrix*leftVS, qr = ProjMatrix*rightVS, qt = ProjMatrix*topVS,qb=ProjMatrix*bottomVS;
	//float rectLeftCS = ql.x/ql.w;
	//float rectRightCS = qr.x/qr.w;
	//float rectTopCS = qt.y/qt.w;
	//float rectBottomCS = qb.y/qb.w;
	// Clamp the rectangle to the screen extents
	rectTopCS = Clamp(rectTopCS, -1.0f, 1.0f);
	rectBottomCS = Clamp(rectBottomCS, -1.0f, 1.0f);
	rectLeftCS = Clamp(rectLeftCS, -1.0f, 1.0f);
	rectRightCS = Clamp(rectRightCS, -1.0f, 1.0f);

	rect = Vector4(rectLeftCS, rectBottomCS, rectRightCS, rectTopCS);
	// Now we convert to screen coordinates by applying the

}
