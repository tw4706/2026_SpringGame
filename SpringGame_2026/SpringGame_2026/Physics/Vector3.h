#pragma once
class Vector3
{
public:
	float x_, y_, z_;

	Vector3();
	Vector3(float x, float y, float z);
	virtual ~Vector3() {};

	//ベクトルの足し算
	Vector3 operator+(const Vector3& other) const;

	//ベクトルの引き算
	Vector3 operator-(const Vector3& other) const;

	//ベクトルの掛け算
	Vector3 operator*(float scalar) const;

	//ベクトルの割り算
	Vector3 operator/(float scalar) const;

	//ベクトルの正規化
	Vector3 Normalize() const;

	//ベクトルの長さを返す
	float Length() const;

	//ベクトルの長さの二乗を返す
	float LengthSq() const;

	//ベクトルの内積を返す
	float Dot(const Vector3& other) const;

	//ベクトルの外積を返す
	Vector3 Cross(const Vector3& other) const;
};

