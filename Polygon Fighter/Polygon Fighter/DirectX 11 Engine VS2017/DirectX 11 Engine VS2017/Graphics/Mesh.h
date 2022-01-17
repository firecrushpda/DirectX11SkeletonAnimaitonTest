//=============================================================================
//
// メッシュクラス[Mesh.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Texture.h"

class Mesh
{
public:
	Mesh();
	Mesh(ID3D11Device * device, ID3D11DeviceContext * deviceContext, 
		std::vector<Vertex3D> & vertices, std::vector<DWORD> & indices, 
		std::vector<Texture> & textures, 
		const DirectX::XMMATRIX & transformMatrix, 
		std::string name);
	Mesh(const Mesh & mesh);
	std::vector<Vertex3D> vertices;//バーテックス情報
	std::vector<DWORD> indices;//インデックス情報
	DirectX::XMMATRIX transformMatrix;//位置
	bool isActive = true;//表しますか

	void Draw();
	const DirectX::XMMATRIX & GetTransformMatrix();
	//DirectX::XMMATRIX BoneTransform(float TimeInSeconds, std::vector<DirectX::XMMATRIX>& Transforms);
	void RefreshVerTexBuffer(ID3D11DeviceContext* devicecontext, std::vector<Vertex3D> varray, UINT vertexCount);

	ID3D11Buffer* GetVertexBuffer();//VertexBuffer<Vertex3D> 
	ID3D11Buffer* GetIndexBuffer();//IndexBuffer*
private:
	std::string name;//名前
	VertexBuffer<Vertex3D> vertexbuffer;//バーテックスバッファ
	IndexBuffer indexbuffer;//インデックスバッファ
	ID3D11DeviceContext * deviceContext;
	std::vector<Texture> textures;//テクスチャ
	
	std::vector<VertexBoneInfo> vertexBoneInfo;//骨情報アレー(未使用
	
};