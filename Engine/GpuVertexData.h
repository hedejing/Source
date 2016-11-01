#pragma once
#include "engine_struct.h"

namespace HW{
	class GpuVertexData{
	public:
		GpuVertexData(){
			m_bResourceValid = false;
		}
		virtual void Create(as_Geometry* geo, VertexDataDesc* inputlayout){}
		virtual void Draw(){}
		virtual void Update(as_Geometry* geo, VertexDataDesc* inputlayout){}


		bool m_bResourceValid;
	};

}