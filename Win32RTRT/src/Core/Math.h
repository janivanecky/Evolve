#pragma once
#include "Def.h"
#define Assert(value) ((value) ? 1 : *((char *)0) = 0)
#define PI 3.14159265359f
#define PI2 6.28318530718f
#define PIHALF 1.57079632679f

struct Vector2
{
    float x;
    float y;

	Vector2()
	{
		this->x = 0;
		this->y = 0;
	}

    Vector2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

	const Vector2 operator-(const Vector2 v2)
	{
		return Vector2(this->x - v2.x, this->y - v2.y);
	}

	const Vector2 operator-(const Vector2 v2) const
	{
		return Vector2(this->x - v2.x, this->y - v2.y);
	}

	const Vector2 operator-()
	{
		return Vector2(-this->x , -this->y);
	}

	const Vector2 operator*(const float v)
	{
		return Vector2(this->x * v, this->y * v);
	}

	const Vector2 operator*(const float v) const
	{
		return Vector2(this->x * v, this->y * v);
	}

	const Vector2 operator/(const float v)
	{
		return Vector2(this->x / v, this->y / v);
	}

	const Vector2 operator+(const Vector2 v2)
	{
		return Vector2(this->x + v2.x, this->y + v2.y);
	}

	const Vector2& operator+=(const Vector2 v2)
	{
		this->x += v2.x;
		this->y += v2.y;
		return *this;
	}

	bool operator!=(const Vector2 v2)
	{
		return !(this->x == v2.x && this->y == v2.y);
	}

	bool operator==(const Vector2 v2)
	{
		return this->x == v2.x && this->y == v2.y;
	}

	const Vector2& operator*=(const float x)
	{
		this->x *= x;
		this->y *= x;
		return *this;
	}

	const Vector2& operator/=(const float x)
	{
		this->x /= x;
		this->y /= x;
		return *this;
	}

	const Vector2 operator+(const Vector2 v2) const
	{
		return Vector2(this->x + v2.x, this->y + v2.y);
	}

};

Vector2 operator*(float x, Vector2 i);

struct IntVector2
{
	int x;
	int y;

	IntVector2()
	{
		this->x = 0;
		this->y = 0;
	}

	IntVector2(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	const IntVector2 operator+(const IntVector2 v2)
	{
		return IntVector2(this->x + v2.x, this->y + v2.y);
	}
};

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3 operator*(float x)
	{
		return Vector3(this->x * x, this->y * x, this->z * x);
	}

	Vector3 operator/(float x) 
	{
		return Vector3(this->x / x, this->y / x, this->z / x);
	}

	Vector3 operator+(Vector3 x) 
	{
		return Vector3(this->x + x.x, this->y + x.y, this->z + x.z);
	}

	Vector3 operator-(Vector3 x) 
	{
		return Vector3(this->x - x.x, this->y - x.y, this->z - x.z);
	}

	Vector3 operator-() 
	{
		return Vector3(-this->x, -this->y, -this->z);
	}

	Vector3& operator+=(Vector3 v2)
	{
		this->x += v2.x;
		this->y += v2.y;
		this->z += v2.z;
		return *this;
	}

	Vector3& operator*=(float x)
	{
		this->x *= x;
		this->y *= x;
		this->z *= x;
		return *this;
	}

};

Vector3 operator*(float x, Vector3 i);

struct Vector4
{
    float x;
    float y;
	float z;
	float w;

	Vector4()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 0;
	}

    Vector4(float x, float y, float z, float w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

	const Vector4 operator-()
	{
		return Vector4(-x, -y, -z, -w);
	}

	const Vector4 operator-(const Vector4 v)
	{
		return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}
};

struct Rectangle2
{
	float x;
	float y;
	float width;
	float height;
	Rectangle2()
	{
		this->x = 0;
		this->y = 0;
		this->width = 0;
		this->height = 0;
	}

	Rectangle2(float x, float y, float width, float height)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}
};

struct Box
{
	Vector3 center;
	float width;
	float height;
	float depth;

	Box(Vector3 center, float width, float height, float depth)
	{
		this->center = center;
		this->width = width;
		this->height = height;
		this->depth = depth;
	}
};

struct Plane
{
	Vector3 normal;
	float d;
};

struct Ray
{
	Vector3 start;
	Vector3 dir;

	Ray()
	{
	}

	Ray(Vector3 start, Vector3 dir)
	{
		this->start = start;
		this->dir = dir;
	}
};

struct Matrix4x4
{
	union
	{
		float x[16];
		struct
		{
			Vector4 v1;
			Vector4 v2;
			Vector4 v3;
			Vector4 v4;
		};
	};

	Matrix4x4():
		x{0}
	{

	}

	const float& operator[](int index) const
	{
		return x[index];
	}

	float& operator[](int index)
	{
		return x[index];
	}

	const Matrix4x4 operator *(const Matrix4x4 m)
	{
		Matrix4x4 result;
		result[0] = x[0] * m[0] + x[4] * m[1] + x[8] * m[2] + x[12] * m[3];
		result[1] = x[1] * m[0] + x[5] * m[1] + x[9] * m[2] + x[13] * m[3];
		result[2] = x[2] * m[0] + x[6] * m[1] + x[10] * m[2] + x[14] * m[3];
		result[3] = x[3] * m[0] + x[7] * m[1] + x[11] * m[2] + x[15] * m[3];
		
		result[4] = x[0] * m[4] + x[4] * m[5] + x[8] * m[6] + x[12] * m[7];
		result[5] = x[1] * m[4] + x[5] * m[5] + x[9] * m[6] + x[13] * m[7];
		result[6] = x[2] * m[4] + x[6] * m[5] + x[10] * m[6] + x[14] * m[7];
		result[7] = x[3] * m[4] + x[7] * m[5] + x[11] * m[6] + x[15] * m[7];

		result[8] = x[0] * m[8] + x[4] * m[9] + x[8] * m[10] + x[12] * m[11];
		result[9] = x[1] * m[8] + x[5] * m[9] + x[9] * m[10] + x[13] * m[11];
		result[10] = x[2] * m[8] + x[6] * m[9] + x[10] * m[10] + x[14] * m[11];
		result[11] = x[3] * m[8] + x[7] * m[9] + x[11] * m[10] + x[15] * m[11];

		result[12] = x[0] * m[12] + x[4] * m[13] + x[8] * m[14] + x[12] * m[15];
		result[13] = x[1] * m[12] + x[5] * m[13] + x[9] * m[14] + x[13] * m[15];
		result[14] = x[2] * m[12] + x[6] * m[13] + x[10] * m[14] + x[14] * m[15];
		result[15] = x[3] * m[12] + x[7] * m[13] + x[11] * m[14] + x[15] * m[15];
		return result;
	}

	const Matrix4x4 operator *(const Matrix4x4 m) const
	{
		Matrix4x4 result;
		result[0] = x[0] * m[0] + x[4] * m[1] + x[8] * m[2] + x[12] * m[3];
		result[1] = x[1] * m[0] + x[5] * m[1] + x[9] * m[2] + x[13] * m[3];
		result[2] = x[2] * m[0] + x[6] * m[1] + x[10] * m[2] + x[14] * m[3];
		result[3] = x[3] * m[0] + x[7] * m[1] + x[11] * m[2] + x[15] * m[3];
		
		result[4] = x[0] * m[4] + x[4] * m[5] + x[8] * m[6] + x[12] * m[7];
		result[5] = x[1] * m[4] + x[5] * m[5] + x[9] * m[6] + x[13] * m[7];
		result[6] = x[2] * m[4] + x[6] * m[5] + x[10] * m[6] + x[14] * m[7];
		result[7] = x[3] * m[4] + x[7] * m[5] + x[11] * m[6] + x[15] * m[7];

		result[8] = x[0] * m[8] + x[4] * m[9] + x[8] * m[10] + x[12] * m[11];
		result[9] = x[1] * m[8] + x[5] * m[9] + x[9] * m[10] + x[13] * m[11];
		result[10] = x[2] * m[8] + x[6] * m[9] + x[10] * m[10] + x[14] * m[11];
		result[11] = x[3] * m[8] + x[7] * m[9] + x[11] * m[10] + x[15] * m[11];

		result[12] = x[0] * m[12] + x[4] * m[13] + x[8] * m[14] + x[12] * m[15];
		result[13] = x[1] * m[12] + x[5] * m[13] + x[9] * m[14] + x[13] * m[15];
		result[14] = x[2] * m[12] + x[6] * m[13] + x[10] * m[14] + x[14] * m[15];
		result[15] = x[3] * m[12] + x[7] * m[13] + x[11] * m[14] + x[15] * m[15];
		return result;
	}

	const Vector4 operator *(const Vector4 v)
	{
		Vector4 result;
		result.x = v.x * this->x[0] + v.y * this->x[4] + v.z * this->x[8] + v.w * this->x[12];
		result.y = v.x * this->x[1] + v.y * this->x[5] + v.z * this->x[9] + v.w * this->x[13];
		result.z = v.x * this->x[2] + v.y * this->x[6] + v.z * this->x[10] + v.w * this->x[14];
		result.w = v.x * this->x[3] + v.y * this->x[7] + v.z * this->x[11] + v.w * this->x[15];
		return result;
	}
};

namespace Math
{
	const float Sin(const float t);
	const float Cos(const float t);
	float Fmod(float x, float d);
	float Abs(float x);
	float Floor(float x);
	Vector2 Floor(Vector2 x);
	float Ceil(float x);
	float Sqrt(float x);
	float Min(float a, float b);
	uint32 Min(uint32 a, uint32 b);
	float Max(float a, float b);
	uint32 Max(uint32 a, uint32 b);
	float Pow(float base, float exponent);
	float Clamp(float x, float lower, float higher);
	float Smoothstep(float edge1, float edge2, float x);
	Vector2 Bezier(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t);
	float EaseInOutQuadraticParameter(float t);
	float EaseInOutQuadraticParameterWithBounce(float t);

	float EaseInOutCubicParameter(float t);
	float EaseInOutQuarticParameter(float t);

	float Length(Vector2 v);
	float Length(Vector3 v);
	float Dot(Vector2 v1, Vector2 v2);
	float Dot(Vector3 v1, Vector3 v2);
	Vector3 Cross(Vector3 x, Vector3 y);
	Vector2 Normalize(Vector2 v);
	Vector3 Normalize(Vector3 v);
	float Distance(Vector2 v1, Vector2 v2);

	float AngularDistance(float alpha, float beta);

	float ExponentialFunction(float amplitude, float coef, float t);
	
	Vector2 RotatePoint(Vector2 v, float angle);

	const Matrix4x4 GetOrthographicsProjectionLH(const float left, const float right,
										 const float bottom, const float top,
										 const float near, const float far);
	const Matrix4x4 Transpose(const Matrix4x4& m);
	const Matrix4x4 GetTranslation(const float dx, const float dy, const float dz);
	const Matrix4x4 GetRotation(const float angle, Vector3 axis);
	const Matrix4x4 GetIdentity();
	const Matrix4x4 GetReflectionY();
	const Matrix4x4 GetScale(const float sx, const float sy, const float sz);
	const Matrix4x4 LookAtLH(Vector3 eye, Vector3 target, Vector3 up);
	const Matrix4x4 LookAtRH(Vector3 eye, Vector3 target, Vector3 up);
	Matrix4x4 Inverse(Matrix4x4);

	bool PlaneRayIntersection(Plane plane, Ray ray, float *t);
	bool BoxRayIntersection(Box box, Ray ray, Vector3 *point);

	bool RectanglePointIntersection(Rectangle2 rect, Vector2 point);

	float ExponentialDistribution(float lambda);

	uint32 Random(uint32 max);
}


