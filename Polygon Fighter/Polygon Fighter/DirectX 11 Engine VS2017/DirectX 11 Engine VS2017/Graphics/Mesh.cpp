//=============================================================================
//
// メッシュ [Mesh.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "Mesh.h"
#include<DirectXMath.h>
#include<DirectXPackedVector.h>
using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

//=============================================================================
// 初期化
//=============================================================================
Mesh::Mesh(ID3D11Device * device, ID3D11DeviceContext * deviceContext, std::vector<Vertex3D>& vertices, std::vector<DWORD>& indices, std::vector<Texture> & textures, const DirectX::XMMATRIX & transformMatrix,std::string name)
{
	this->deviceContext = deviceContext;
	this->textures = textures;
	this->transformMatrix = transformMatrix;
	this->vertices = vertices;
	this->indices = indices;
	this->name = name;

	HRESULT hr = this->vertexbuffer.Initialize(device, deviceContext, vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

	hr = this->indexbuffer.Initialize(device, indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
	
}

//=============================================================================
// 初期化
//=============================================================================
Mesh::Mesh(const Mesh & mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->indexbuffer = mesh.indexbuffer;
	this->vertexbuffer = mesh.vertexbuffer;
	this->textures = mesh.textures;
	this->transformMatrix = mesh.transformMatrix;
	this->vertices = mesh.vertices;
	this->indices = mesh.indices;
	this->name = mesh.name;
}

//=============================================================================
// 描画
//=============================================================================
void Mesh::Draw()
{
	if(isActive){
		UINT offset = 0;

		for (int i = 0; i < textures.size(); i++)
		{
			if (textures[i].GetType() == aiTextureType::aiTextureType_DIFFUSE)
			{
				this->deviceContext->PSSetShaderResources(0, 1, textures[i].GetTextureResourceViewAddress());
				break;
			}
			else if (textures[i].GetType() == aiTextureType::aiTextureType_SPECULAR)
			{
				this->deviceContext->PSSetShaderResources(1, 1, textures[i].GetTextureResourceViewAddress());
				break;
			}
			else if (textures[i].GetType() == aiTextureType::aiTextureType_HEIGHT)
			{
				this->deviceContext->PSSetShaderResources(2, 1, textures[i].GetTextureResourceViewAddress());
				break;
			}
			else if (textures[i].GetType() == aiTextureType::aiTextureType_DISPLACEMENT)
			{
				this->deviceContext->PSSetShaderResources(3, 1, textures[i].GetTextureResourceViewAddress());
				break;
			}
		}

		this->deviceContext->IASetVertexBuffers(0, 1, this->vertexbuffer.GetAddressOf(), this->vertexbuffer.StridePtr(), &offset);
		this->deviceContext->IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		this->deviceContext->DrawIndexed(this->indexbuffer.IndexCount(), 0, 0);
	}
}

//=============================================================================
// マトリクス取る
//=============================================================================
const DirectX::XMMATRIX & Mesh::GetTransformMatrix()
{
	return this->transformMatrix;
}

//=============================================================================
// バーテックスバッファ更新
//=============================================================================
void Mesh::RefreshVerTexBuffer(ID3D11DeviceContext* devicecontext, std::vector<Vertex3D> varray, UINT vertexCount)
{
	this->vertexbuffer.Refresh(devicecontext, varray.data(), vertexCount);
}

ID3D11Buffer* Mesh::GetVertexBuffer()
{
	return vertexbuffer.Get();
}

ID3D11Buffer* Mesh::GetIndexBuffer() {
	return indexbuffer.Get();
}