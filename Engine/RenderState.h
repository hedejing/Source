#pragma once

#include "RenderSystemEnum.h"
#include<string>
#include "mathlib.h"
#include <stdlib.h>
namespace HW
{
/// �����˹�դ������״̬
	class RasterState
	{
	public:
		RasterState():mNumRects(0),mRects(0){}
		~RasterState(){if(mRects) delete [] mRects; }
		RasterState(const RasterState &other);
		RasterState& operator=(const RasterState& rhs);
		/// ���ϵͳĬ�ϵĹ�դ������״̬
		static RasterState getDefault();
		/** ���òü����Եĳ�����
		  * @param numRect ��Ҫ���Եĳ�������������
		  * @param rects һ�鳤��������
		  */
		void setScissorRects(unsigned int numRect,ScissorRect rects[]);
		/// set scissor test rect by index.
		void setScissorRectByIndex(unsigned int index,ScissorRect &rect);
		bool operator==(const RasterState& rhs)const;
		bool operator!=(const RasterState& rhs)const;
		/// defines how to fill the geometry primitive when rendering.
		FillMode mFillMode;
		/// defines how to cull the primitives.
		CullMode mCullMode;
		/// defines which side of a planar is front.
		/// some of the properties are not find corresponding item in opengL es yet
		bool     mFrontCounterClockwise;
		int      mDepthBias;
		float    mDepthBiasClamp;
		float    mSlopScaledDepthBias;
		bool     mDepthClipEnable;
		bool     mScissorEnable;
		bool     mMultiSampleEnable;
		bool     mAntialiasedLineEnable;
		// members for internal use.
		unsigned int mNumRects;
		// color mask
		bool mColorWriteMask[4];
		ScissorRect *mRects;
	};



	class BlendState
	{
	public:
		BlendState(){}
		~BlendState(){}
		BlendState(const BlendState& other);
		BlendState& operator=(const BlendState &other);
		/// get a system default BlendState instance.
		static BlendState getDefault();
		bool operator==(const BlendState &rhs)const;
		bool operator!=(const BlendState &rhs)const;
		bool mAlphaToConverageEnable;
		bool mIndependentBlendEnable;
		bool mBlendEnable;
		BlendOperand mSrcBlend;
		BlendOperand mDestBlend;
		BlendOperation    mBlendFunc;
		BlendOperand mSrcBlendAlpha;
		BlendOperand mDestBlendAlpla;
		BlendOperation    mBlendFuncAlpha;
		unsigned char mWriteMask;
		float        mFactor[4];
		unsigned int mSampleMask;
	};

	inline bool BlendState::operator!=(const BlendState& rhs)const
	{
		return !(*this == rhs);
	}

	
	class DepthStencilState
	{
	public:
		static DepthStencilState getDefault();
		unsigned int mStencilRef;
		bool mDepthEnable;
		bool mDepthWriteMask;
		ComparisonFunc mDepthFunc;
		bool mStencilEnable;
		unsigned char mStencilReadMask;
		unsigned char mStencilWriteMask;
		StencilOperation   mFrontStencilFailFunc;
		StencilOperation   mFrontStencilDepthFailFunc;
		StencilOperation   mFrontStencilPassFunc;
		ComparisonFunc mFrontStencilFunc;
		StencilOperation   mBackStencilFailFunc;
		StencilOperation   mBackStencilDepthFailFunc;
		StencilOperation   mBackStencilPassFunc;
		ComparisonFunc mBackStencilFunc;

	};

	class ClearState{
	public:
		static ClearState getDefault();
		bool mclearcolorbuffer;
		bool mcleardepthbuffer;
		bool mclearstencilbuffer;
		Vector4 mClearColor;
	};

	inline bool operator==(const DepthStencilState& lhs,const DepthStencilState& rhs)
	{
		return memcmp(&lhs,&rhs,sizeof(DepthStencilState))==0;
	}

	inline bool operator!=(const DepthStencilState& lhs,const DepthStencilState& rhs)
	{
		return !(lhs == rhs);
	}

	inline bool operator<(const DepthStencilState& lhs,const DepthStencilState& rhs)
	{
		return memcmp(&lhs,&rhs,sizeof(DepthStencilState))<0;
	}

}
