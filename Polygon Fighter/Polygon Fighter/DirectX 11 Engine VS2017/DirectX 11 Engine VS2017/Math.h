//=============================================================================
//
// 数学クラス[Math.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <DirectXMath.h>

DirectX::XMMATRIX ScaleCardinal(const DirectX::XMMATRIX &a, const DirectX::XMFLOAT3 &direction)
{
	//temp
	DirectX::XMMATRIX result = a;
	DirectX::XMFLOAT4X4 des;
	XMStoreFloat4x4(&des, result);

	des._11 *= direction.x;
	des._12 *= direction.x;
	des._13 *= direction.x;
	des._14 *= direction.x;

	des._21 *= direction.y;
	des._22 *= direction.y;
	des._23 *= direction.y;
	des._24 *= direction.y;

	des._31 *= direction.z;
	des._32 *= direction.z;
	des._33 *= direction.z;
	des._34 *= direction.z;

	result = XMLoadFloat4x4(&des);
	return result;
}

//QuatToMat4
DirectX::XMMATRIX QuatToMat4(const DirectX::XMFLOAT4 &q)
{
	DirectX::XMMATRIX result;
	DirectX::XMFLOAT4X4 des;
	XMStoreFloat4x4(&des, result);

	float qxx = (q.x * q.x);
	float qyy = (q.y * q.y);
	float qzz = (q.z * q.z);
	float qxz = (q.x * q.z);
	float qxy = (q.x * q.y);
	float qyz = (q.y * q.z);
	float qwx = (q.w * q.x);
	float qwy = (q.w * q.y);
	float qwz = (q.w * q.z);

	des._11 = 1 - (2) * (qyy + qzz);
	des._12 = (2) * (qxy + qwz);
	des._13 = (2) * (qxz - qwy);

	des._21 = (2) * (qxy - qwz);
	des._22 = (1) - (2) * (qxx + qzz);
	des._23 = (2) * (qyz + qwx);

	des._31 = (2) * (qxz + qwy);
	des._32 = (2) * (qyz - qwx);
	des._33 = (1) - (2) * (qxx + qyy);

	result = XMLoadFloat4x4(&des);
	return result;
}

//Translate
DirectX::XMMATRIX Translate(const DirectX::XMMATRIX &a, const DirectX::XMFLOAT3 &translation)
{
	DirectX::XMMATRIX result = a;
	/*DirectX::XMFLOAT4X4 des;
	XMStoreFloat4x4(&des, result);*/

	DirectX::XMMATRIX temp = DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);

	//des._31 = translation.x;// DirectX::XMFLOAT4(translation.x, translation.y, translation.z, 1); //* a;
	//des._32 = translation.y;
	//des._33 = translation.z;
	//des._34 = 1;

	result = DirectX::XMMatrixMultiply(temp, result);

	return result;
}

template<typename T>
inline constexpr T Lerp(const T &a, const T &b, const T &t)
{
	return a * (static_cast<T>(1.0) - t) + b * t;
}

inline DirectX::XMFLOAT3 Lerp(const DirectX::XMFLOAT3 &a, const DirectX::XMFLOAT3 &b, const float &t)
{
	DirectX::XMFLOAT3 result = DirectX::XMFLOAT3(
		a.x + t * (b.x - a.x),
		a.y + t * (b.y - a.y),
		a.z + t * (b.z - a.z)
	);

	return result;
}


template<typename T>
inline constexpr int Sign(const T &val)
{
	return (static_cast<T>(0.0) < val) - (val < static_cast<T>(0));
}

DirectX::XMFLOAT4 Slerp(const DirectX::XMFLOAT4 &a, const DirectX::XMFLOAT4 &b, const float &t)
{
	DirectX::XMVECTOR an = DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&a));
	DirectX::XMVECTOR bn = DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&b));
	//DirectX::XMFLOAT4 an = DirectX::XMVector4Normalize(a);
	//DirectX::XMFLOAT4 bn = Normalize(b);
	DirectX::XMFLOAT4 anf4;
	DirectX::XMStoreFloat4(&anf4, an);
	DirectX::XMFLOAT4 bnf4;
	DirectX::XMStoreFloat4(&bnf4, bn);
	float d = anf4.x * bnf4.x + anf4.y * bnf4.y + anf4.z * bnf4.z + anf4.w * bnf4.w;
	float tinv = 1.0f - t;
	int ds = Sign(d);

	DirectX::XMFLOAT4 result;
	result.x = anf4.x * tinv + ds * t * bnf4.x;
	result.y = anf4.y * tinv + ds * t * bnf4.y;
	result.z = anf4.z * tinv + ds * t * bnf4.z;
	result.w = anf4.w * tinv + ds * t * bnf4.w;
	DirectX::XMVECTOR resultn = DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&result));
	DirectX::XMStoreFloat4(&result, resultn);
	return result;
}

DirectX::XMFLOAT3 GetRotateVectorByMatrix(const DirectX::XMMATRIX& matrix) 
{
	
}
