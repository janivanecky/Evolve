#include "Math.h"
#include <cstdlib>

const Matrix4x4 Math::GetOrthographicsProjectionLH(const float left, const float right,
										   const float bottom, const float top,
										   const float near, const float far)
{
	Matrix4x4 result;
	result[0] = 2.0f / (right - left);
	result[5] = 2.0f / (top - bottom);
	result[10] = 1.0f / (far - near);
	result[12] = - (right + left) / (right - left);
	result[13] = - (top + bottom) / (top - bottom);
	result[14] = - (near) / (far - near);
	result[15] = 1;
	return result;
}

const Matrix4x4 Math::Transpose(const Matrix4x4& m)
{
	/*
		0 4 8  12
		1 5 9  13
		2 6 10 14
		3 7 11 15
	*/
	Matrix4x4 r = m;
	
	r[1] = m[4];
	r[2] = m[8];
	r[3] = m[12];
	
	r[4] = m[1];
	r[6] = m[9];
	r[7] = m[13];

	r[8] = m[2];
	r[9] = m[6];
	r[11] = m[14];

	r[12] = m[3];
	r[13] = m[7];
	r[14] = m[11];

	return r;
}

const Matrix4x4 Math::GetTranslation(const float dx, const float dy, const float dz)
{
	Matrix4x4 result;

	result[0] = 1;
	result[5] = 1;
	result[10] = 1;
	result[12] = dx;
	result[13] = dy;
	result[14] = dz;
	result[15] = 1;

	return result;
}

const Matrix4x4 Math::GetRotation(const float angle, Vector3 axis)
{
	float c = Cos(angle);
	float s = Sin(angle);

	Matrix4x4 result;
	float ax = axis.x;
	float ay = axis.y;
	float az = axis.z;
	result[0] = c + (1 - c) * ax * ax;
	result[1] = (1-c) * ax * ay + s * az;
	result[2] = (1-c) * ax * az - s * ay;
	result[3] = 0;

	result[4] = (1 - c) * ax * ay - s * az;
	result[5] = c + (1 - c) * ay * ay;
	result[6] = (1 - c) * ay * az + s * ax;
	result[7] = 0;

	result[8] = (1 - c) * ax * az + s * ay;
	result[9] = (1 - c) * ay * az - s * ax;
	result[10] = c + (1 - c) * az * az;
	result[11] = 0;
	
	result[12] = 0;
	result[13] = 0;
	result[14] = 0;
	result[15] = 1;

	return result;
}

const Matrix4x4 Math::GetIdentity()
{
	Matrix4x4 result;
	result.v1 = Vector4(1,0,0,0);
	result.v2 = Vector4(0,1,0,0);
	result.v3 = Vector4(0,0,1,0);
	result.v4 = Vector4(0,0,0,1);
	return result;
}

const Matrix4x4 Math::GetReflectionY()
{
	Matrix4x4 result = GetIdentity();
	result.v2 = -result.v2;
	return result;
}

const Matrix4x4 Math::GetScale(const float sx, const float sy, const float sz)
{
	Matrix4x4 result = GetIdentity();
	result[0] = sx;
	result[5] = sy;
	result[10] = sz;
	return result;
}

const Matrix4x4 Math::LookAtRH(Vector3 eye, Vector3 target, Vector3 up)
{
	Matrix4x4 matrix;
	Vector3 x, y, z;

	z = eye - target;
	z = Math::Normalize(z);

	x = Math::Cross(up, z);
	x = Math::Normalize(x);
	y = Math::Cross(z, x);

	y = Math::Normalize(y);

	matrix.v1 = Vector4(x.x, y.x, z.x, 0);
	matrix.v2 = Vector4(x.y, y.y, z.y, 0);
	matrix.v3 = Vector4(x.z, y.z, z.z, 0);
	matrix.v4 = Vector4(-Math::Dot(x, eye),
		-Math::Dot(y, eye),
		-Math::Dot(z, eye),
		1.0f);
	return matrix;
}


const Matrix4x4 Math::LookAtLH(Vector3 eye, Vector3 target, Vector3 up)
{
	Vector3 z = Math::Normalize(target - eye);
	Vector3 x = Math::Normalize(Cross(up, z));
	Vector3 y = Math::Normalize(Cross(z, x));

	Vector4 v1 = Vector4(x.x, y.x, z.x, 0);
	Vector4 v2 = Vector4(x.y, y.y, z.y, 0);
	Vector4 v3 = Vector4(x.z, y.z, z.z, 0);
	Vector4 v4 = Vector4(-Dot(eye, x), -Dot(eye, y), -Dot(eye, z), 1);

	Matrix4x4 result;
	result.v1 = v1;
	result.v2 = v2;
	result.v3 = v3;
	result.v4 = v4;
	return result;
}

Matrix4x4 Math::Inverse(Matrix4x4 m)
{
    Matrix4x4 inv;
	float det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    det = 1.0f / det;

    for (i = 0; i < 16; i++)
        inv[i] = inv[i] * det;

	return inv;
}

float Abs(float x)
{
	return x < 0 ? -x : x;
}

short Floor(float x)
{
	return (short)(x + 32768.0f) - 32768;
}

short Ceil(float x)
{
	return Floor(x) + 1;
}

#include <math.h>
const float Math::Sin(const float t)
{
	float B = 1.27323954474f;
	float C = -0.40528473456f;
	float P = 0.225f; 
	float sign = t < 0 ? -1.0f : 1.0f;
	float x = t - Floor((t + PI * sign) / PI2) * PI2;
	float y = B * x + C * x * Abs(x);
	y = P * (y * Abs(y) - y) + y;
	return sinf(t);
}

const float Math::Cos(const float t)
{
	return Sin(t + PIHALF);
}

float Math::Fmod(float x, float d)
{
	if (d == 0)
	{
		return 0;
	}
	while (x < 0)
	{
		x = x + d;
	}
	while (x >= d)
	{
		x -= d;
	}
	return x;

}

float Math::AngularDistance(float alpha, float beta)
{
	float d = Abs(alpha - beta);
    if (d > PI)
        d = PI2 - d;
    return d;
}

float Math::Length(Vector2 v)
{
	float result = sqrtf(v.x * v.x + v.y * v.y);
	return result;
}

float Math::Length(Vector3 v)
{
	float result = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return result;
}

float Math::Sqrt(float x)
{
	return sqrtf(x);
}

#ifdef WP
#include <arm_neon.h>
float Math::Min(float a, float b)
{
	__n64 nA;
	nA.n64_f32[0] = a;
	__n64 nB;
	nB.n64_f32[0] = b;
	__n64 result = vmin_f32(nA, nB);
	return result.n64_f32[0];
}

uint32 Math::Min(uint32 a, uint32 b)
{
	__n64 nA;
	nA.n64_u32[0] = a;
	__n64 nB;
	nB.n64_u32[0] = b;
	__n64 result = vmin_u32(nA, nB);
	return result.n64_u32[0];
}

float Math::Max(float a, float b)
{
	__n64 nA;
	nA.n64_f32[0] = a;
	__n64 nB;
	nB.n64_f32[0] = b;
	__n64 result = vmax_f32(nA, nB);
	return result.n64_f32[0];
}

uint32 Math::Max(uint32 a, uint32 b)
{
	__n64 nA;
	nA.n64_u32[0] = a;
	__n64 nB;
	nB.n64_u32[0] = b;
	__n64 result = vmax_u32(nA, nB);
	return result.n64_u32[0];
}

float Math::Abs(float x)
{
	__n64 input;
	input.n64_f32[0] = x;
	__n64 result = vabs_f32(input);
	return result.n64_f32[0];
}
#else
#include <intrin.h>
float Math::Min(float a, float b)
{
	__m128 inputA;
	inputA.m128_f32[0] = a;
	__m128 inputB;
	inputB.m128_f32[0] = b;
	__m128 result = _mm_min_ss(inputA, inputB);
	return result.m128_f32[0];
}

uint32 Math::Min(uint32 a, uint32 b)
{
	__m128 inputA;
	inputA.m128_u32[0] = a;
	__m128 inputB;
	inputB.m128_u32[0] = b;
	__m128 result = _mm_min_ss(inputA, inputB);
	return result.m128_u32[0];
}

float Math::Max(float a, float b)
{
	__m128 inputA;
	inputA.m128_f32[0] = a;
	__m128 inputB;
	inputB.m128_f32[0] = b;
	__m128 result = _mm_max_ss(inputA, inputB);
	return result.m128_f32[0];
}

uint32 Math::Max(uint32 a, uint32 b)
{
	__m128 inputA;
	inputA.m128_u32[0] = a;
	__m128 inputB;
	inputB.m128_u32[0] = b;
	__m128 result = _mm_max_ss(inputA, inputB);
	return result.m128_u32[0];
}

float Math::Abs(float x)
{
	static const __m128 SIGNMASK = 
               _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
	__m128 val;
	val.m128_f32[0] = x;
	__m128 absval = _mm_andnot_ps(SIGNMASK, val); 
	return absval.m128_f32[0];
}
#endif

float Math::Dot(Vector2 v1, Vector2 v2)
{
	float result = v1.x * v2.x + v1.y * v2.y;
	return result;
}

float Math::Dot(Vector3 v1, Vector3 v2)
{
	float result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

float Math::Distance(Vector2 v1, Vector2 v2)
{
	float result = Math::Length(v1 - v2);
	return result;
}

Vector2 Math::Normalize(Vector2 v)
{
	Vector2 result = v / Length(v);
	return result;
}

Vector3 Math::Normalize(Vector3 v)
{
	float length = Length(v);
	if (length == 0.0f)
		return v;
	Vector3 result = v / length;
	return result;
}

float Math::Floor(float x)
{
	return floorf(x);
}

Vector2 Math::Floor(Vector2 x)
{
	Vector2 result;
	result.x = Math::Floor(x.x);
	result.y = Math::Floor(x.y);
	return result;
}

float Math::Ceil(float x)
{
	return ceilf(x);
}

float Math::Pow(float base, float exponent)
{
	float result = powf(base, exponent);
	return result;
}

float Math::Smoothstep(float edge1, float edge2, float x)
{
    x = Clamp((x - edge1)/(edge2 - edge1), 0.0, 1.0); 
    return x*x*(3 - 2*x);
}

float Math::ExponentialFunction(float amplitude, float coef, float t)
{
	float result = amplitude * Math::Pow(2.0f, coef * t);
	return result;
}

float Math::Clamp(float x, float lower, float higher)
{
	if (x < lower)
		return lower;
	if (x > higher)
		return higher;
	return x;
}

float Math::EaseInOutQuadraticParameter(float t)
{
	t = Math::Clamp(t, 0, 1);
	if (t < 0.5f)
	{
		return 2 * t * t;
	} 
	t--;
	return 1 - 2 * t * t;
}

float Math::EaseInOutQuadraticParameterWithBounce(float t)
{
	t = Math::Clamp(t, 0, 1);
	if (t < 0.5f)
	{
		return 2 * t * t;
	} 

	return -5 * t * t + 8.5f * t - 2.5f;
}

float Math::EaseInOutCubicParameter(float t)
{
	t = Math::Clamp(t, 0, 1);
	if (t < 0.5f)
	{
		return 4 * t * t * t;
	} 
	t = Math::Abs(t - 1);
	return 1 - 4 * t * t * t;
}

float Math::EaseInOutQuarticParameter(float t)
{
	t = Math::Clamp(t, 0, 1);
	float t4 = t * t * t * t;
	if (t < 0.5f)
	{
		return  8.0f * t4;
	} 
	t--;
	return 1 - 8.0f * t * t * t * t;
}

Vector2 Math::Bezier(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;
	float invT = 1.0f - t;
	float invT2 = invT * invT;
	float invT3 = invT2 * invT;

	Vector2 result = p0 * invT3 + p1 * 3 * invT2 * t  + p2 * 3 * invT * t2 + p3 * t3;
	return result;
}

Vector2 Math::RotatePoint(Vector2 v, float angle)
{
	Vector2 result;
	result.x = Math::Cos(angle) * v.x - Math::Sin(angle) * v.y;
	result.y = Math::Cos(angle) * v.y + Math::Sin(angle) * v.x;
	return result;
}

Vector3 Math::Cross(Vector3 x, Vector3 y)
{
	Vector3 result;
	result.x = x.y * y.z - x.z * y.y;
	result.y = x.z * y.x - x.x * y.z;
	result.z = x.x * y.y - x.y * y.x;
	return result;
}

bool Math::PlaneRayIntersection(Plane plane, Ray ray, float *t)
{
	ray.dir = Math::Normalize(ray.dir);
	plane.normal = Math::Normalize(plane.normal);
	float denominator = Dot(ray.dir, plane.normal);
	if (denominator == 0)
	{
		return false;
	}
	float numerator = -(Dot(plane.normal, ray.start) + plane.d);
	*t = numerator / denominator;
	return true;
}

bool Math::BoxRayIntersection(Box box, Ray ray, Vector3 *point)
{
	ray.dir = Math::Normalize(ray.dir);
	if (ray.dir.x != 0)
	{
		Plane xPlane;
		if (ray.dir.x > 0)
		{
			xPlane.normal = Vector3(-1,0,0);
			xPlane.d = box.center.x - box.width / 2.0f;
		}
		else if (ray.dir.x < 0)
		{
			xPlane.normal = Vector3(1,0,0);
			xPlane.d = -(box.center.x + box.width / 2.0f);
		}
		float t;
		if (Math::PlaneRayIntersection(xPlane, ray, &t))
		{
			Vector3 intersectionPosition = ray.start + ray.dir * t;
			if (intersectionPosition.y < box.center.y + box.height / 2.0f &&
				intersectionPosition.y > box.center.y - box.height / 2.0f &&
				intersectionPosition.z < box.center.z + box.depth / 2.0f &&
				intersectionPosition.z > box.center.z - box.depth / 2.0f)
			{
				*point = intersectionPosition;
				return true;
			}
		}
	}

	if (ray.dir.y != 0)
	{
		Plane yPlane;
		if (ray.dir.y > 0)
		{
			yPlane.normal = Vector3(0,-1,0);
			yPlane.d = box.center.y - box.height / 2.0f;
		}
		else if (ray.dir.y < 0)
		{
			yPlane.normal = Vector3(0,1,0);
			yPlane.d = -(box.center.y + box.height / 2.0f);

		}
		float t;
		if (Math::PlaneRayIntersection(yPlane, ray, &t))
		{
			Vector3 intersectionPosition = ray.start + ray.dir * t;
			if (intersectionPosition.x < box.center.x + box.width / 2.0f &&
				intersectionPosition.x > box.center.x - box.width / 2.0f &&
				intersectionPosition.z < box.center.z + box.depth / 2.0f &&
				intersectionPosition.z > box.center.z - box.depth / 2.0f)
			{
				*point = intersectionPosition;
				return true;
			}
		}
	}

	if (ray.dir.z != 0)
	{
		Plane zPlane;
		if (ray.dir.z > 0)
		{
			zPlane.normal = Vector3(0,0, -1);
			zPlane.d = box.center.z - box.depth / 2.0f;
		}
		else if (ray.dir.z < 0)
		{
			zPlane.normal = Vector3(0,0,1);
			zPlane.d = -(box.center.z + box.depth / 2.0f);

		}
		float t;
		if (Math::PlaneRayIntersection(zPlane, ray, &t))
		{
			Vector3 intersectionPosition = ray.start + ray.dir * t;
			if (intersectionPosition.y < box.center.y + box.height / 2.0f &&
				intersectionPosition.y > box.center.y - box.height / 2.0f &&
				intersectionPosition.x < box.center.x + box.width / 2.0f &&
				intersectionPosition.x > box.center.x - box.width / 2.0f)
			{
				*point = intersectionPosition;
				return true;
			}
		}
	}

	return false;
}

bool Math::RectanglePointIntersection(Rectangle2 rect, Vector2 point)
{
	return point.x > rect.x && point.x < rect.x + rect.width &&
		point.y > rect.y && point.y < rect.y + rect.height;
}

uint32 Math::Random(uint32 max)
{
	uint32 result = rand() % max;
	return result;
}

#include <math.h>
float Math::ExponentialDistribution(float lambda)
{
	float random = (rand() % 1000) / 1000.0f;
	float x = -log(random) / lambda;
	return x;
}

Vector2 operator*(float x, Vector2 i)
{
	return i * x;
}

Vector3 operator*(float x, Vector3 i)
{
	return i * x;
}