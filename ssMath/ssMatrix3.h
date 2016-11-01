#ifndef ss_math_Matrix3_H_
#define ss_math_Matrix3_H_

#include "ssPrerequisities.h"

namespace ssMath
{
	class Matrix3
    {
    public:
		inline Matrix3 () {}
        inline explicit Matrix3 (const float arr[3][3])
		{
			memcpy(m,arr,9*sizeof(float));
		}
        inline Matrix3 (const Matrix3& rkMatrix)
		{
			memcpy(m,rkMatrix.m,9*sizeof(float));
		}
		// ÐÐÖ÷Ðò;
        Matrix3 (float fEntry00, float fEntry01, float fEntry02,
                    float fEntry10, float fEntry11, float fEntry12,
                    float fEntry20, float fEntry21, float fEntry22)
		{
			m[0][0] = fEntry00;
			m[0][1] = fEntry01;
			m[0][2] = fEntry02;
			m[1][0] = fEntry10;
			m[1][1] = fEntry11;
			m[1][2] = fEntry12;
			m[2][0] = fEntry20;
			m[2][1] = fEntry21;
			m[2][2] = fEntry22;
		}

		/** Exchange the contents of this matrix with another. 
		*/
		inline void swap(Matrix3& other)
		{
			std::swap(m[0][0], other.m[0][0]);
			std::swap(m[0][1], other.m[0][1]);
			std::swap(m[0][2], other.m[0][2]);
			std::swap(m[1][0], other.m[1][0]);
			std::swap(m[1][1], other.m[1][1]);
			std::swap(m[1][2], other.m[1][2]);
			std::swap(m[2][0], other.m[2][0]);
			std::swap(m[2][1], other.m[2][1]);
			std::swap(m[2][2], other.m[2][2]);
		}

        // member access, allows use of construct mat[r][c]
        inline float* operator[] (size_t iRow) const
		{
			return (float*)m[iRow];
		}
        /*inline operator Real* ()
		{
			return (Real*)m[0];
		}*/
        Vector3 getColumn (size_t iCol) const;
        void setColumn(size_t iCol, const Vector3& vec);
        void fromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

        // assignment and comparison
        inline Matrix3& operator= (const Matrix3& rkMatrix)
		{
			memcpy(m,rkMatrix.m,9*sizeof(float));
			return *this;
		}
        bool operator== (const Matrix3& rkMatrix) const;
        inline bool operator!= (const Matrix3& rkMatrix) const
		{
			return !operator==(rkMatrix);
		}

        // arithmetic operations
        Matrix3 operator+ (const Matrix3& rkMatrix) const;
        Matrix3 operator- (const Matrix3& rkMatrix) const;
        Matrix3 operator* (const Matrix3& rkMatrix) const;
        Matrix3 operator- () const;

        // matrix * vector [3x3 * 3x1 = 3x1]
        Vector3 operator* (const Vector3& rkVector) const;

        // vector * matrix [1x3 * 3x3 = 1x3]
        friend Vector3 operator* (const Vector3& rkVector,
            const Matrix3& rkMatrix);

        // matrix * scalar
        Matrix3 operator* (float fScalar) const;

        // scalar * matrix
        friend Matrix3 operator* (float fScalar, const Matrix3& rkMatrix);

        // utilities
        Matrix3 transpose () const;
        bool inverse (Matrix3& rkInverse, float fTolerance = 1e-06) const;
        Matrix3 inverse (float fTolerance = 1e-06) const;
        float determinant () const;

        // singular value decomposition
        void singularValueDecomposition (Matrix3& rkL, Vector3& rkS,
            Matrix3& rkR) const;
        void singularValueComposition (const Matrix3& rkL,
            const Vector3& rkS, const Matrix3& rkR);

        // Gram-Schmidt orthonormalization (applied to columns of rotation matrix)
        void orthonormalize ();

        // orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
        void QDUDecomposition (Matrix3& rkQ, Vector3& rkD,
            Vector3& rkU) const;

        float spectralNorm () const;

        // matrix must be orthonormal
        void toAxisAngle (Vector3& rkAxis, float& rfAngle) const;
		// 
        void fromAxisAngle (const Vector3& rkAxis, float fRadians);

        // The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
        // where yaw is rotation about the Up vector, pitch is rotation about the
        // Right axis, and roll is rotation about the Direction axis.
        bool toEulerAnglesXYZ (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool toEulerAnglesXZY (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool toEulerAnglesYXZ (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool toEulerAnglesYZX (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool toEulerAnglesZXY (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        bool toEulerAnglesZYX (float& rfYAngle, float& rfPAngle,
            float& rfRAngle) const;
        void fromEulerAnglesXYZ (float fYAngle, float fPAngle, float fRAngle);
        void fromEulerAnglesXZY (float fYAngle, float fPAngle, float fRAngle);
        void fromEulerAnglesYXZ (float fYAngle, float fPAngle, float fRAngle);
        void fromEulerAnglesYZX (float fYAngle, float fPAngle, float fRAngle);
        void fromEulerAnglesZXY (float fYAngle, float fPAngle, float fRAngle);
        void fromEulerAnglesZYX (float fYAngle, float fPAngle, float fRAngle);
        // eigensolver, matrix must be symmetric
        void eigenSolveSymmetric (float afEigenvalue[3],
            Vector3 akEigenvector[3]) const;

        static void tensorProduct (const Vector3& rkU, const Vector3& rkV,
            Matrix3& rkProduct);

		/** Determines if this matrix involves a scaling. */
		inline bool hasScale() const
		{
			// check magnitude of column vectors (==local axes)
			float t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
			if(fabs(t - 1.0) > 1e-4)
			/*if (!Math::RealEqual(t, 1.0, (Real)1e-04))*/
				return true;
			t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
			if(fabs(t - 1.0) > 1e-4)
			/*if (!Math::RealEqual(t, 1.0, (Real)1e-04))*/
				return true;
			t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
			if(fabs(t - 1.0) > 1e-4)
			/*if (!Math::RealEqual(t, 1.0, (Real)1e-04))*/
				return true;

			return false;
		}

		/** Function for writing to a stream.
		*/
		inline friend std::ostream& operator <<
			( std::ostream& o, const Matrix3& mat )
		{
			o << "Matrix3(" << mat[0][0] << ", " << mat[0][1] << ", " << mat[0][2] << ", " 
                            << mat[1][0] << ", " << mat[1][1] << ", " << mat[1][2] << ", " 
                            << mat[2][0] << ", " << mat[2][1] << ", " << mat[2][2] << ")";
			return o;
		}

        static const Matrix3 ZERO;
        static const Matrix3 IDENTITY;

    protected:
        // support for eigensolver
        void tridiagonal (float afDiag[3], float afSubDiag[3]);
        bool QLAlgorithm (float afDiag[3], float afSubDiag[3]);

        // support for singular value decomposition
        static const float ms_fSvdEpsilon;
        static const unsigned int ms_iSvdMaxIterations;
        static void bidiagonalize (Matrix3& kA, Matrix3& kL,
            Matrix3& kR);
        static void golubKahanStep (Matrix3& kA, Matrix3& kL,
            Matrix3& kR);

        // support for spectral norm
        static float maxCubicRoot (float afCoeff[3]);

        float m[3][3];

        // for faster access
        friend class Matrix4;
    };
}
#endif