//=============================================================================
//
// エラーローグ [ErrorLogger.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//未使用
//=============================================================================
#include "FBXLoader.h"

FBX_Model::FBX_Model()
{

}

FBX_Model::~FBX_Model()
{
	Destroy();
}

void FBX_Model::Draw(
	const DirectX::XMMATRIX& world,
	const DirectX::XMMATRIX& viewproj)
{
	// ----- Animation -----
	timeCount += FrameTime;
	if (timeCount > stop) timeCount = start;

	for (size_t j = 0; j < m_meshes.size(); j++)
	{
		auto meshnode = m_meshes.at(j)->m_meshNode;
		auto mesh = m_meshes.at(j)->m_mesh;

		if (hasAnimation && j == 0)//
		{


			// 移動、回転、拡大のための行列を作成
			FbxMatrix globalPosition = meshnode->EvaluateGlobalTransform(timeCount);
			FbxVector4 t0 = meshnode->GetGeometricTranslation(FbxNode::eSourcePivot);
			FbxVector4 r0 = meshnode->GetGeometricRotation(FbxNode::eSourcePivot);
			FbxVector4 s0 = meshnode->GetGeometricScaling(FbxNode::eSourcePivot);
			FbxAMatrix geometryOffset = FbxAMatrix(t0, r0, s0);

			// 各頂点に掛けるための最終的な行列の配列
			FbxMatrix *clusterDeformation = new FbxMatrix[mesh->GetControlPointsCount()];
			memset(clusterDeformation, 0, sizeof(FbxMatrix) * mesh->GetControlPointsCount());

			FbxSkin *skinDeformer = (FbxSkin *)mesh->GetDeformer(0, FbxDeformer::eSkin);
			int clusterCount = skinDeformer->GetClusterCount();
			// 各クラスタから各頂点に影響を与えるための行列作成
			for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) {
				// クラスタ(ボーン)の取り出し
				FbxCluster *cluster = skinDeformer->GetCluster(clusterIndex);
				FbxMatrix vertexTransformMatrix;
				FbxAMatrix referenceGlobalInitPosition;
				FbxAMatrix clusterGlobalInitPosition;
				FbxMatrix clusterGlobalCurrentPosition;
				FbxMatrix clusterRelativeInitPosition;
				FbxMatrix clusterRelativeCurrentPositionInverse;
				cluster->GetTransformMatrix(referenceGlobalInitPosition);
				referenceGlobalInitPosition *= geometryOffset;
				cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);
				clusterGlobalCurrentPosition = cluster->GetLink()->EvaluateGlobalTransform(timeCount);
				clusterRelativeInitPosition = clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition;
				clusterRelativeCurrentPositionInverse = globalPosition.Inverse() * clusterGlobalCurrentPosition;
				vertexTransformMatrix = clusterRelativeCurrentPositionInverse * clusterRelativeInitPosition;
				// 上で作った行列に各頂点毎の影響度(重み)を掛けてそれぞれに加算
				for (int i = 0; i < cluster->GetControlPointIndicesCount(); i++) {
					int index = cluster->GetControlPointIndices()[i];
					double weight = cluster->GetControlPointWeights()[i];
					FbxMatrix influence = vertexTransformMatrix * weight;
					clusterDeformation[index] += influence;
				}
			}

			// 最終的な頂点座標を計算しVERTEX3Dに変換
			for (int i = 0; i < mesh->GetControlPointsCount(); i++) {
				//vertex
				FbxVector4 outVertex = clusterDeformation[i].MultNormalize(mesh->GetControlPointAt(i));
				m_meshes.at(j)->vertices[i].pos.x = (FLOAT)outVertex[0];
				m_meshes.at(j)->vertices[i].pos.y = (FLOAT)outVertex[1];
				m_meshes.at(j)->vertices[i].pos.z = (FLOAT)outVertex[2];


				//normal
				auto enormal = mesh->GetElementNormal();
				FbxVector4 outNormal = enormal->GetDirectArray().GetAt(i);
				m_meshes.at(j)->vertices[i].normal.x = (FLOAT)outNormal[0];
				m_meshes.at(j)->vertices[i].normal.y = (FLOAT)outNormal[1];
				m_meshes.at(j)->vertices[i].normal.z = (FLOAT)outNormal[2];

				//uv
				auto pVertexUV = mesh->GetElementUV(0);
				auto UVmap = pVertexUV->GetMappingMode();
				m_meshes.at(j)->vertices[i].texCoord.x = pVertexUV->GetDirectArray().GetAt(i)[0];
				m_meshes.at(j)->vertices[i].texCoord.y = pVertexUV->GetDirectArray().GetAt(i)[1];

				//tangent todo

				//assimp bone structure 
				//not use in this parse
				m_meshes.at(j)->vertices[i].bone_index = { 0,0,0,0 };
				m_meshes.at(j)->vertices[i].bone_weights = { -1,-1,-1,-1 };

			}
			// ---------------------
			delete[] clusterDeformation;

			// パラメータの受け渡し(頂点)
			D3D11_MAPPED_SUBRESOURCE pdata;
			this->deviceContext->Map(m_meshes.at(j)->VerBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
			memcpy_s(pdata.pData, pdata.RowPitch, m_meshes.at(j)->vertices, sizeof(Vertex3D) * mesh->GetControlPointsCount());
			this->deviceContext->Unmap(m_meshes.at(j)->VerBuffer, 0);
		}
		

			// パラメータの受け渡し(定数)
			this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());
			cb_vs_vertexshader->data.worldMatrix = world;
			cb_vs_vertexshader->data.wvpMatrix = world * viewproj;
			cb_vs_vertexshader->ApplyChanges();

			//texture 

			//rebind
			UINT stride = sizeof(Vertex3D);
			UINT offset = 0;
			this->deviceContext->IASetVertexBuffers(0, 1, &m_meshes.at(j)->VerBuffer, &stride, &offset);
			this->deviceContext->IASetIndexBuffer(m_meshes.at(j)->IndBuffer, DXGI_FORMAT_R32_UINT, 0);
			// 描画実行
			this->deviceContext->DrawIndexed(mesh->GetPolygonVertexCount(), 0, 0);
	}
}

void FBX_Model::Create(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader,
	const char* fbxfile_path)
{

	this->device = device;
	this->deviceContext = context;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;

	// <FBX読み込み>
	FBX_Import(fbxfile_path);
	// <頂点データの取り出し>
	//FBX_GetVertex();
	// <頂点データ用バッファの設定>
	FBX_SetVertexData(device);

}

void FBX_Model::Destroy()
{
	m_fbxScene->Destroy();
	m_fbxManager->Destroy();
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		m_meshes.at(i)->VerBuffer->Release();
		m_meshes.at(i)->IndBuffer->Release();
		if (m_meshes.at(i)->vertices)
		{
			delete[] m_meshes.at(i)->vertices;
		}
	}

	
}

void FBX_Model::FBX_Import(const char* fbxfile_path)
{
	// <FBX読み込み>
	m_fbxManager = FbxManager::Create();
	m_fbxScene = FbxScene::Create(m_fbxManager, "fbxscene");
	FbxString FileName(fbxfile_path);
	FbxImporter *fbxImporter = FbxImporter::Create(m_fbxManager, "imp");
	fbxImporter->Initialize(FileName.Buffer(), -1, m_fbxManager->GetIOSettings());
	fbxImporter->Import(m_fbxScene);
	fbxImporter->Destroy();

	FbxArray<FbxString*> AnimStackNameArray;
	m_fbxScene->FillAnimStackNameArray(AnimStackNameArray);
	if (AnimStackNameArray!= NULL)
	{
		hasAnimation = true;
		FbxAnimStack *AnimationStack = m_fbxScene->FindMember<FbxAnimStack>(AnimStackNameArray[AnimStackNumber]->Buffer());
		m_fbxScene->SetCurrentAnimationStack(AnimationStack);

		FbxTakeInfo *takeInfo = m_fbxScene->GetTakeInfo(*(AnimStackNameArray[AnimStackNumber]));
		start = takeInfo->mLocalTimeSpan.GetStart();
		stop = takeInfo->mLocalTimeSpan.GetStop();
		FrameTime.SetTime(0, 0, 0, 1, 0, m_fbxScene->GetGlobalSettings().GetTimeMode());
		timeCount = start;
	}
	else
	{
		hasAnimation = false;
	}
	
	for (int i = 0; i < m_fbxScene->GetRootNode()->GetChildCount(); i++) {
		if (m_fbxScene->GetRootNode()->GetChild(i)->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh) {
			m_meshNode = m_fbxScene->GetRootNode()->GetChild(i);
			m_mesh = m_meshNode->GetMesh();

			MeshFBX* mesh = new MeshFBX();
			mesh->m_mesh = m_meshNode->GetMesh();
			mesh->m_meshNode = m_meshNode;
			m_meshes.push_back(mesh);
		}
	}
}

void FBX_Model::FBX_GetVertex()
{
	// <頂点データの取り出し>
	//for (size_t i = 0; i < m_meshes.size(); i++)
	//{
	//	for (int i = 0; i < m_fbxScene->GetRootNode()->GetChildCount(); i++) {
	//		if (m_fbxScene->GetRootNode()->GetChild(i)->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh) {
	//			//m_meshes.at(i) = m_fbxScene->GetRootNode()->GetChild(i)->GetMesh();
	//			break;
	//		}
	//	}
	//	//this->vertices = new Vertex3D[m_mesh->GetControlPointsCount()];

	//	for (int i = 0; i < m_mesh->GetControlPointsCount(); i++) {
	//		this->vertices[i].pos.x = static_cast<FLOAT>(m_mesh->GetControlPointAt(i)[0]);
	//		this->vertices[i].pos.y = static_cast<FLOAT>(m_mesh->GetControlPointAt(i)[1]);
	//		this->vertices[i].pos.z = static_cast<FLOAT>(m_mesh->GetControlPointAt(i)[2]);
	//	}
	//}
	
}

void FBX_Model::FBX_SetVertexData(ID3D11Device* device)
{
	
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		auto meshstruct = m_meshes.at(i);

		meshstruct->vertices = new Vertex3D[m_mesh->GetControlPointsCount()];
		
		for (int j = 0; j < m_mesh->GetControlPointsCount(); j++) {
			//vertex
			//m_meshes.at(j)
			FbxVector4 outVertex = m_mesh->GetControlPointAt(j);
			meshstruct->vertices[j].pos.x = (FLOAT)outVertex[0];
			meshstruct->vertices[j].pos.y = (FLOAT)outVertex[1];
			meshstruct->vertices[j].pos.z = (FLOAT)outVertex[2];

			//normal
			auto enormal = m_mesh->GetElementNormal();
			auto refmode = enormal->GetReferenceMode();
			auto dfg = enormal->GetMappingMode();
			FbxVector4 outNormal = enormal->GetDirectArray().GetAt(j);
			meshstruct->vertices[j].normal.x = (FLOAT)outNormal[0];
			meshstruct->vertices[j].normal.y = (FLOAT)outNormal[1];
			meshstruct->vertices[j].normal.z = (FLOAT)outNormal[2];

			//uv
			auto pVertexUV = m_mesh->GetElementUV(0);
			auto UVmap = pVertexUV->GetMappingMode();
			meshstruct->vertices[j].texCoord.x = pVertexUV->GetDirectArray().GetAt(j)[0];
			meshstruct->vertices[j].texCoord.y = pVertexUV->GetDirectArray().GetAt(j)[1];

			//tangent todo

			//assimp bone structure 
			//not use in this parse
			meshstruct->vertices[j].bone_index = { 0,0,0,0 };
			meshstruct->vertices[j].bone_weights = { -1,-1,-1,-1 };

		}

		// バーテックスデータの取り出しとバッファの設定
		D3D11_BUFFER_DESC bd_vertex;
		bd_vertex.ByteWidth = sizeof(Vertex3D) * meshstruct->m_mesh->GetControlPointsCount();
		bd_vertex.Usage = D3D11_USAGE_DYNAMIC;
		bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd_vertex.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd_vertex.MiscFlags = 0;
		bd_vertex.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vertexBufferData;
		vertexBufferData.pSysMem = meshstruct->vertices;
		if (FAILED(device->CreateBuffer(&bd_vertex, &vertexBufferData, &meshstruct->VerBuffer))) {
			assert(false && "Missing !");
		}


		// インデックスデータの取り出しとバッファの設定
		D3D11_BUFFER_DESC bd_index;
		bd_index.ByteWidth = sizeof(int) * meshstruct->m_mesh->GetPolygonVertexCount();
		bd_index.Usage = D3D11_USAGE_DEFAULT;
		bd_index.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd_index.CPUAccessFlags = 0;
		bd_index.MiscFlags = 0;
		bd_index.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA data_index;
		data_index.pSysMem = meshstruct->m_mesh->GetPolygonVertices();
		(device->CreateBuffer(&bd_index, &data_index, &meshstruct->IndBuffer));
	}
}

