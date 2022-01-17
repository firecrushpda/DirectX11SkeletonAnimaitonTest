//=============================================================================
//
//バーテックスバッファ [VertexBuffer.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#ifndef VertexBuffer_h__
#define VertexBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <vector>
#include "Vertex.h"

template<class T>
class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;//バッファ
	UINT stride = sizeof(T);//ストライド
	UINT vertexCount = 0;//バーテックス数

public:
	VertexBuffer() {}

	VertexBuffer(const VertexBuffer<T>& rhs)
	{
		this->buffer = rhs.buffer;
		this->vertexCount = rhs.vertexCount;
		this->stride = rhs.stride;
	}

	VertexBuffer<T> & operator=(const VertexBuffer<T>& a)
	{
		this->buffer = a.buffer;
		this->vertexCount = a.vertexCount;
		this->stride = a.stride;
		return *this;
	}

	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	UINT VertexCount() const
	{
		return this->vertexCount;
	}

	const UINT Stride() const
	{
		return this->stride;
	}

	const UINT * StridePtr() const
	{
		return &this->stride;
	}

	//初期化
	HRESULT Initialize(ID3D11Device *device,ID3D11DeviceContext* devicecontext, T * data, UINT vertexCount)
	{
		if (buffer.Get() != nullptr)
			buffer.Reset();

		this->vertexCount = vertexCount;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;//D3D11_USAGE_DEFAULT D3D11_USAGE_DYNAMIC
		vertexBufferDesc.ByteWidth = stride * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//D3D11_CPU_ACCESS_WRITE
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->buffer.GetAddressOf());
		return hr;
	}


	//not use
	//動的にバーテックス情報を更新
	//未使用
	void Refresh(ID3D11DeviceContext* devicecontext, T* data, UINT vertexCount)// std::vector<Vertex3D> varray
	{

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		//  Disable GPU access to the vertex buffer data.
		//auto m_d3dContext = device;
		HRESULT hr = devicecontext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (SUCCEEDED(hr))
		{
			//  Update the vertex buffer here.
			//memcpy(mappedResource.pData, varray.data(), sizeof(Vertex3D) * varray.size());
			memcpy(mappedResource.pData, data, sizeof(Vertex3D) * vertexCount);//stride * vertexCount sizeof(data)
			//  Reenable GPU access to the vertex buffer data.
			devicecontext->Unmap(buffer.Get(), 0);
		}
	}
};

#endif // VertexBuffer_h__
