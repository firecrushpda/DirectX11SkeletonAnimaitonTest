//=============================================================================
//
// レンダオブジェクト処理 [RenderableGameObject.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "SkyBox.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

//=============================================================================
// 初期化
//=============================================================================
bool SkyBox::Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;

	this->SetPosition(0.0f, 0.0f, 0.0f);//位置
	this->SetRotation(0.0f, 0.0f, 0.0f);//回転
	this->UpdateMatrix();

	//Assimpライブラリをモデル情報読み込む
	Assimp::Importer importer;
	//importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	const aiScene* Scene = importer.ReadFile(filePath,
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_GenNormals |
		aiProcess_FixInfacingNormals |
		aiProcess_Triangulate |
		aiProcess_TransformUVCoords |
		aiProcess_ConvertToLeftHanded |
		aiProcess_SplitLargeMeshes |
		aiProcess_JoinIdenticalVertices
	);

	if (!Scene)
		return false;

	//ノード処理
	this->ProcessNode(Scene->mRootNode, Scene, DirectX::XMMatrixIdentity());

	HRESULT hr = this->vertexbuffer.Initialize(device, deviceContext, vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

	hr = this->indexbuffer.Initialize(device, indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");

	//texture
	std::vector<std::wstring> path = {	L"Data//Textures//Left_Tex.png",L"Data//Textures//Right_Tex.png",
										L"Data//Textures//Up_Tex.png", L"Data//Textures//Down_Tex.png",
										L"Data//Textures//Front_Tex.png", L"Data//Textures//Back_Tex.png"};
	CreateWICTexture2DCubeFromFile(this->device, this->deviceContext, path, nullptr, textureView1.GetAddressOf(),true);

	CreateDDSTextureFromFile(device, L"Data//Textures//skybox1.dds", 0, &textureView2);

	textureView = textureView1;

	this->SetPosition(0.0f, 0.0f, 0.0f);//位置
	this->SetRotation(0.0f, 0.0f, 0.0f);//回転
	this->UpdateMatrix();

	return true;
}

//=============================================================================
//描画
//=============================================================================
void SkyBox::Draw(const XMMATRIX & viewProjectionMatrix)
{
	//AdjustRotation(XMFLOAT3(0, 0.001,0));
	this->deviceContext->PSSetShaderResources(0, 1, textureView.GetAddressOf());
	
	//Update Constant buffer with WVP Matrix
	this->cb_vs_vertexshader->data.wvpMatrix = worldMatrix; //* viewProjectionMatrix;//m_GlobalInverseTransform
	this->cb_vs_vertexshader->data.worldMatrix =  worldMatrix;//m_GlobalInverseTransform
	this->cb_vs_vertexshader->ApplyChanges();
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());

	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexbuffer.GetAddressOf(), this->vertexbuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(this->indexbuffer.IndexCount(), 0, 0);
}

//=============================================================================
// マトリクス更新処理
//=============================================================================
void SkyBox::UpdateMatrix()
{
	this->worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}

//=============================================================================
// 更新処理
//=============================================================================
void SkyBox::Update(float dt, const XMMATRIX & viewProjectionMatrix)
{
	//model.Update(dt, this->worldMatrix, viewProjectionMatrix);
}

void SkyBox::ProcessNode(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix) 
{
	//geometry transformの値が全transformにかけします
	//we calculate geometry transform with the whole node tree
	DirectX::XMMATRIX nodeTransformMatrix = ToMatrix4f(&node->mTransformation) * parentTransformMatrix;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = pmScene->mMeshes[node->mMeshes[i]];

		DirectX::XMMATRIX temp = nodeTransformMatrix;
		this->ProcessMesh(mesh, pmScene, temp);
		break;
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], pmScene, nodeTransformMatrix);
	}

}

DirectX::XMMATRIX SkyBox::ToMatrix4f(const aiMatrix4x4 *ai_mat)
{
	DirectX::XMMATRIX a = DirectX::XMMatrixSet(
		ai_mat->a1, ai_mat->a2, ai_mat->a3, ai_mat->a4,
		ai_mat->b1, ai_mat->b2, ai_mat->b3, ai_mat->b4,
		ai_mat->c1, ai_mat->c2, ai_mat->c3, ai_mat->c4,
		ai_mat->d1, ai_mat->d2, ai_mat->d3, ai_mat->d4);
	DirectX::XMMATRIX b = DirectX::XMMatrixTranspose(a);
	return b;
}

void SkyBox::ProcessMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix)
{
	//バーテックスとインデックス情報
	// Data to fill

	//バーテックス情報処理
	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex3D_SkyBox vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		//バーテックス情報がここで直接マティアスを交換させDirectXの座標系を合わせます
		//we transform vertex position in the begining of impliment so that it can fit DirectX coordination
		//auto temp = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vertex.pos), transformMatrix);
		//DirectX::XMStoreFloat3(&vertex.pos, temp);

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
			//auto temp3 = XMVector2Normalize(DirectX::XMVector2Transform(DirectX::XMLoadFloat2(&vertex.texCoord), transformMatrix));
			//DirectX::XMStoreFloat2(&vertex.texCoord, temp3);
		}

		vertices.push_back(vertex);

	}

	//インデックス情報
	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			int index = face.mIndices[j];
			indices.push_back(face.mIndices[j]);
		}
	}

}

HRESULT SkyBox::CreateWICTexture2DCubeFromFile(
	ID3D11Device * d3dDevice,
	ID3D11DeviceContext * d3dDeviceContext,
	const std::wstring & cubeMapFileName,
	ID3D11Texture2D ** textureArray,
	ID3D11ShaderResourceView ** textureCubeView,
	bool generateMips)
{

	if (!d3dDevice || !d3dDeviceContext || !(textureArray || textureCubeView))
		return E_INVALIDARG;

	// ******************
	// read skybox
	//

	ID3D11Texture2D* srcTex = nullptr;
	ID3D11ShaderResourceView* srcTexSRV = nullptr;

	HRESULT hResult = CreateWICTextureFromFile(d3dDevice,
		(generateMips ? d3dDeviceContext : nullptr),
		cubeMapFileName.c_str(),
		(ID3D11Resource**)&srcTex,
		(generateMips ? &srcTexSRV : nullptr));

	if (FAILED(hResult))
	{
		return hResult;
	}

	D3D11_TEXTURE2D_DESC texDesc, texArrayDesc;
	srcTex->GetDesc(&texDesc);


	if (texDesc.Width * 3 != texDesc.Height * 4)
	{
		srcTex->Release();
		srcTexSRV->Release();
		return E_FAIL;
	}

	// ******************
	// 6tex
	//

	UINT squareLength = texDesc.Width / 4;
	texArrayDesc.Width = squareLength;
	texArrayDesc.Height = squareLength;
	texArrayDesc.MipLevels = (generateMips ? texDesc.MipLevels - 2 : 1);
	texArrayDesc.ArraySize = 6;
	texArrayDesc.Format = texDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* texArray = nullptr;
	hResult = d3dDevice->CreateTexture2D(&texArrayDesc, nullptr, &texArray);
	if (FAILED(hResult))
	{
		srcTex->Release();
		srcTexSRV->Release();
		return hResult;
	}

	// ******************
	//copy
	//

	D3D11_BOX box;
	// box cordinating
	//    front
	//   / 
	//  /_____right
	//  |
	//  |
	//  bottom
	box.front = 0;
	box.back = 1;

	for (UINT i = 0; i < texArrayDesc.MipLevels; ++i)
	{
		// +X
		box.left = squareLength * 2;
		box.top = squareLength;
		box.right = squareLength * 3;
		box.bottom = squareLength * 2;
		d3dDeviceContext->CopySubresourceRegion(
			texArray,
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_POSITIVE_X, texArrayDesc.MipLevels),
			0, 0, 0,
			srcTex,
			i,
			&box);

		// -X
		box.left = 0;
		box.top = squareLength;
		box.right = squareLength;
		box.bottom = squareLength * 2;
		d3dDeviceContext->CopySubresourceRegion(
			texArray,
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_NEGATIVE_X, texArrayDesc.MipLevels),
			0, 0, 0,
			srcTex,
			i,
			&box);

		// +Y
		box.left = squareLength;
		box.top = 0;
		box.right = squareLength * 2;
		box.bottom = squareLength;
		d3dDeviceContext->CopySubresourceRegion(
			texArray,
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_POSITIVE_Y, texArrayDesc.MipLevels),
			0, 0, 0,
			srcTex,
			i,
			&box);


		// -Y
		box.left = squareLength;
		box.top = squareLength * 2;
		box.right = squareLength * 2;
		box.bottom = squareLength * 3;
		d3dDeviceContext->CopySubresourceRegion(
			texArray,
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_NEGATIVE_Y, texArrayDesc.MipLevels),
			0, 0, 0,
			srcTex,
			i,
			&box);

		// +Z
		box.left = squareLength;
		box.top = squareLength;
		box.right = squareLength * 2;
		box.bottom = squareLength * 2;
		d3dDeviceContext->CopySubresourceRegion(
			texArray,
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_POSITIVE_Z, texArrayDesc.MipLevels),
			0, 0, 0,
			srcTex,
			i,
			&box);

		// -Z
		box.left = squareLength * 3;
		box.top = squareLength;
		box.right = squareLength * 4;
		box.bottom = squareLength * 2;
		d3dDeviceContext->CopySubresourceRegion(
			texArray,
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_NEGATIVE_Z, texArrayDesc.MipLevels),
			0, 0, 0,
			srcTex,
			i,
			&box);

		// set next mipLevel
		squareLength /= 2;
	}


	// ******************
	// create SRV
	//
	if (textureCubeView)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		viewDesc.TextureCube.MostDetailedMip = 0;
		viewDesc.TextureCube.MipLevels = texArrayDesc.MipLevels;

		hResult = d3dDevice->CreateShaderResourceView(texArray, &viewDesc, textureCubeView);
	}

	//
	if (textureArray)
	{
		*textureArray = texArray;
	}
	else
	{
		texArray->Release();
	}

	//srcTex->Release();
	//srcTexSRV->Release();

	return hResult;
}

HRESULT SkyBox::CreateWICTexture2DCubeFromFile(
	ID3D11Device * d3dDevice,
	ID3D11DeviceContext * d3dDeviceContext,
	const std::vector<std::wstring>& cubeMapFileNames,
	ID3D11Texture2D ** textureArray,
	ID3D11ShaderResourceView ** textureCubeView,
	bool generateMips)
{

	UINT arraySize = (UINT)cubeMapFileNames.size();

	if (!d3dDevice || !d3dDeviceContext || arraySize < 6 || !(textureArray || textureCubeView))
		return E_INVALIDARG;


	HRESULT hResult;
	std::vector<ID3D11Texture2D*> srcTexVec(arraySize, nullptr);
	std::vector<ID3D11ShaderResourceView*> srcTexSRVVec(arraySize, nullptr);
	std::vector<D3D11_TEXTURE2D_DESC> texDescVec(arraySize);

	for (UINT i = 0; i < arraySize; ++i)
	{
		hResult = CreateWICTextureFromFile(d3dDevice,
			(generateMips ? d3dDeviceContext : nullptr),
			cubeMapFileNames[i].c_str(),
			(ID3D11Resource**)&srcTexVec[i],
			(generateMips ? &srcTexSRVVec[i] : nullptr));

		if (hResult != S_OK)
			return hResult;

		srcTexVec[i]->GetDesc(&texDescVec[i]);


		if (texDescVec[i].MipLevels != texDescVec[0].MipLevels || texDescVec[i].Width != texDescVec[0].Width ||
			texDescVec[i].Height != texDescVec[0].Height || texDescVec[i].Format != texDescVec[0].Format)
		{
			for (UINT j = 0; j < i; ++j)
			{
				srcTexVec[j]->Release();
				srcTexSRVVec[j]->Release();
			}
			return E_FAIL;
		}
	}


	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texDescVec[0].Width;
	texArrayDesc.Height = texDescVec[0].Height;
	texArrayDesc.MipLevels = (generateMips ? texDescVec[0].MipLevels : 1);
	texArrayDesc.ArraySize = arraySize;
	texArrayDesc.Format = texDescVec[0].Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* texArray = nullptr;
	hResult = d3dDevice->CreateTexture2D(&texArrayDesc, nullptr, &texArray);

	if (FAILED(hResult))
	{
		for (UINT i = 0; i < arraySize; ++i)
		{
			srcTexVec[i]->Release();
			srcTexSRVVec[i]->Release();
		}

		return hResult;
	}


	texArray->GetDesc(&texArrayDesc);

	for (UINT i = 0; i < arraySize; ++i)
	{
		for (UINT j = 0; j < texArrayDesc.MipLevels; ++j)
		{
			d3dDeviceContext->CopySubresourceRegion(
				texArray,
				D3D11CalcSubresource(j, i, texArrayDesc.MipLevels),
				0, 0, 0,
				srcTexVec[i],
				j,
				nullptr);
		}
	}


	if (textureCubeView)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		viewDesc.TextureCube.MostDetailedMip = 0;
		viewDesc.TextureCube.MipLevels = texArrayDesc.MipLevels;

		hResult = d3dDevice->CreateShaderResourceView(texArray, &viewDesc, textureCubeView);
	}

	if (textureArray)
	{
		*textureArray = texArray;
	}
	else
	{
		texArray->Release();
	}

	for (UINT i = 0; i < arraySize; ++i)
	{
		srcTexVec[i] = 0;
		srcTexSRVVec[i] = 0;
	}

	return hResult;
}


void SkyBox::ChangeSkyBoxSRV() {
	if (textureView == textureView1)
	{
		textureView = textureView2;
	}
	else if (textureView == textureView2) {
		textureView = textureView1;
	}
}
