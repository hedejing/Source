#ifndef HW_KEYFRAME_H
#define HW_KEYFRAME_H

#include "mathlib.h"
namespace HW
{

	class KeyFrame 
	{
	public:
		KeyFrame( float time):m_keytime(time)
		{

		}
		float getTime()
		{
			return m_keytime;
		}
	protected:
		float m_keytime;
	};

	class TranslationKeyFrame :public KeyFrame
	{
	public:
		TranslationKeyFrame(float time,Vector3 trans):KeyFrame(time),m_translation(trans)
		{}
		Vector3 getTrans()
		{
			return m_translation;
		}
	protected:
		Vector3 m_translation;
	};

	class ScaleKeyFrame :public KeyFrame
	{
	public: 
		ScaleKeyFrame(float time ,Vector3 scale):KeyFrame(time),m_scale(scale){}
		Vector3 getScale()
		{
			return m_scale;
		}
	protected:
		Vector3 m_scale;
	};

	class RotationKeyFrame :public KeyFrame
	{
	public:
		RotationKeyFrame(float time, Quaternion q):KeyFrame(time),m_rotate(q){}
		Quaternion getRotation()
		{
			return m_rotate;
		}
	protected:
		Quaternion m_rotate;
	};

	// this class is used for store movement that only need to be specified by a single value
	// like roll angle in the target camera's situation
	class NumericKeyFrame :public KeyFrame
	{
	public:
		NumericKeyFrame(float time, float value):KeyFrame(time),m_value(value)
		{

		}
		float value()
		{
			return m_value;
		}
	protected:
		float m_value;
	};

}
#endif