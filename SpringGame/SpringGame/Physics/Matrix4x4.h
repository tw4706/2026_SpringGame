#pragma once
#include"Vector3.h"

class Matrix4x4
{
public:
	float x0_, x1_, x2_, x3_;
	float y0_, y1_, y2_, y3_;
	float z0_, z1_, z2_, z3_;
	float w0_, w1_, w2_, w3_;

	Matrix4x4();
	Matrix4x4(float x0, float x1, float x2, float x3,
			  float y0, float y1, float y2, float y3,
			  float z0, float z1, float z2, float z3,
		float w0, float w1, float w2, float w3);

	//’PˆÊs—ñ
	static Matrix4x4 Identity();

	//s—ñ‚Ì‘«‚µZ
	Matrix4x4 operator+(const Matrix4x4& other) const;

	//s—ñ‚Ìˆø‚«Z
	Matrix4x4 operator-(const Matrix4x4& other) const;

	//s—ñ‚ÌŠ|‚¯Z
	Matrix4x4 operator*(const Matrix4x4& other) const;

	//X²‰ñ“]
	static Matrix4x4 RotateX(float angle);

	//Y²‰ñ“]
	static Matrix4x4 RotateY(float angle);

	//Z²‰ñ“]
	static Matrix4x4 RotateZ(float angle);

	//•½sˆÚ“®
	static Matrix4x4 Translate(const Vector3& vec);

	//Šg‘åk¬
	static Matrix4x4 Scale(const Vector3& vec);

	//ƒxƒNƒgƒ‹‚Æs—ñ‚ÌŠ|‚¯Z
	Vector3 Transform(const Vector3& vec)const;

	//Dxlib‚Ìs—ñ‚É•ÏŠ·
	MATRIX ToDxlibMatrix()const;

};

