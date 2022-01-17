//=============================================================================
//
// バーテックスクラス [Vertex.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include <DirectXMath.h>

struct Vertex2D
{
	Vertex2D() {}
	Vertex2D(float x, float y, float z, float u, float v)
		: pos(x, y, z), texCoord(u, v) {}

	DirectX::XMFLOAT3 pos;//位置
	DirectX::XMFLOAT2 texCoord;//座標
};

struct Vertex3D
{
	Vertex3D() {}
	Vertex3D(float x, float y, float z, float u, float v, float nx, float ny, float nz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

	DirectX::XMFLOAT3 pos;//位置
	DirectX::XMFLOAT2 texCoord;//座標
	DirectX::XMFLOAT3 normal;//ノーマル
	DirectX::XMFLOAT3 tangent;//正接
	DirectX::XMFLOAT4 bone_index;//骨インデックス
	DirectX::XMFLOAT4 bone_weights;//骨重り

};

struct Vertex3D_SkyBox
{
	Vertex3D_SkyBox() {}
	Vertex3D_SkyBox(float x, float y, float z)
		: pos(x, y, z) {}

	DirectX::XMFLOAT3 pos;//位置
	DirectX::XMFLOAT2 texCoord;//座標
};

//assimp bone info 
//未使用
struct VertexBoneInfo
{
	VertexBoneInfo(){}
	unsigned int bone_index[4];
	float weights[4];
	void AddBoneData(float Weight, unsigned int id)
	{
		for (unsigned int i = 0; i < 4; i++) {
			if (weights[i] == 0.0) {
				bone_index[i] = id;
				weights[i] = Weight;
				return;
			}
		}
	}
	unsigned int next_space = 0;
};