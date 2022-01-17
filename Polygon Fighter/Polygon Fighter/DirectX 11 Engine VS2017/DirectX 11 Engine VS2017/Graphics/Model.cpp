//=============================================================================
//
// モデル処理 [Model.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "Model.h"
#include "../ErrorLogger.h"

using namespace DirectX;

#define ANIM_FIGHTIDLE				(4)
#define ANIM_STANDTOFIGHTIDLE		(5)
#define ANIM_BLOCKLEFT				(1)
#define ANIM_BLOCKRIGHT				(2)
#define ANIM_BLOCKIDLE				(3)
#define ANIM_CHARGEATTACK			(0)
#define ANIM_STANDIDLE				(6)
#define ANIM_DEADSTYLE1				(7)

//=============================================================================
// モデル初期化関数
//=============================================================================
bool Model::Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;

	//モデル専用の骨情報バッファ
	HRESULT hr = this->cb_bone_info.Initialize(this->device, this->deviceContext);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

	try
	{
		//FBX SDKからFBX情報を読み込む
		//しばらくバッグがありますので使えない
		//load mesh
		//if (StringHelper::IsContain(filePath, ".FBX")) {
		//	isFbxModel = true;
		//	m_fbxmodel = std::make_unique<FBX_Model>();
		//	m_fbxmodel->Create(device, deviceContext, cb_vs_vertexshader, filePath.c_str());
		//}
		//else {
			//assimp load model
			//assimpからモデル情報を読み込む
			//if (!this->LoadModel_Assimp(filePath))
			//	return false;
		//}

		//assimp load model
		//assimpからモデル情報を読み込む
		if (!this->LoadModel_Assimp(filePath))
			return false;
		
		//もしモデルは骨を付けているなら各骨の位置を表示する小箱を読み込む
		//load debug block for the bone 
		if (bHasBone)
		{
			LoadBoneDebugBlock(device, deviceContext, cb_vs_vertexshader);
		}
		
		//アニメーションを読み込む
		//load animation
		std::vector<std::string> pathes = {
			//action
			"Data\\Objects\\samurai\\Animations\\Action\\ChargeAttk.FBX",
			"Data\\Objects\\samurai\\Animations\\GetHit\\Block_GetHit_Left.FBX",
			"Data\\Objects\\samurai\\Animations\\GetHit\\Block_GetHit_Right.FBX",
			"Data\\Objects\\samurai\\Animations\\Basic\\BlockIdle.FBX",
			/*"Data\\Objects\\samurai\\Animations\\Action\\Dash_Back.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dash_Front.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dash_Left.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dash_Right.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dodge_Back.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dodge_Front.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dodge_Left.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dodge_Right.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\HardAttk.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\HitBlockLeft.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\HitBlockRight.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\LightAttk1.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\LightAttk2.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\LightAttk3.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\LightAttk4.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\MediumAttk1.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\MediumAttk2.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\MediumAttk3.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\RoundKick.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\SideKick.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Sprint_LightAttk.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Sprint_StrongAttk.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StabInChest_Attempt.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StabInChest_Hit.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StabOnFloor.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StrongAttk1.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StrongAttk2.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StrongAttk3.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StrongAttk4.FBX",*/

			//basic
			"Data\\Objects\\samurai\\Animations\\Basic\\FightIdle.FBX",
			"Data\\Objects\\samurai\\Animations\\Basic\\StandIdle_To_FightIdle.FBX",
			"Data\\Objects\\samurai\\Animations\\Basic\\StandIdle.FBX",

			//dead
			"Data\\Objects\\samurai\\Animations\\GetHit\\Dead1.FBX"
			
		};
		if (StringHelper::IsContain(filePath,"samurai"))
		{
			for (size_t i = 0; i < pathes.size(); i++)
			{
				if (!this->LoadAnimation_Assimp(pathes.at(i)))
				{
					return false;
				}
			}
		}
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

//=============================================================================
// モデル描画関数
//=============================================================================
void Model::Draw(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix)
{
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());
	this->deviceContext->VSSetConstantBuffers(1, 1, this->cb_bone_info.GetAddressOf());
	//if (isFbxModel)
	//{
	//	//fbx sdk draw
	//	//fbx sdkモデル描画
	//	//m_fbxmodel->Draw(worldMatrix, viewProjectionMatrix);
	//}
	//else
	//{
		//assimpモデル描画
		if (!m_showskeleton)//デバッグフラッグ
		{
			for (int i = 0; i < meshes.size(); i++)
			{
				//Update Constant buffer with WVP Matrix
				this->cb_vs_vertexshader->data.wvpMatrix = m_GlobalInverseTransform * meshes[i].GetTransformMatrix() * worldMatrix * viewProjectionMatrix;//
				this->cb_vs_vertexshader->data.worldMatrix = m_GlobalInverseTransform * meshes[i].GetTransformMatrix() * worldMatrix;//

				this->cb_vs_vertexshader->ApplyChanges();
				meshes[i].Draw();
			}
		}
		
		//デバッグモデル描画
		if (bHasBone == true && m_showskeleton == true)//
		{
			for (size_t i = 0; i < debugBlocks.size(); i++)
			{
				//Update Constant buffer with WVP Matrix
				this->cb_vs_vertexshader->data.wvpMatrix = debugBlocks[i].GetTransformMatrix() * worldMatrix * viewProjectionMatrix;
				this->cb_vs_vertexshader->data.worldMatrix = debugBlocks[i].GetTransformMatrix() * worldMatrix;

				this->cb_vs_vertexshader->ApplyChanges();
				debugBlocks[i].Draw();
			}
		}

		//collision debug block
		if (!(bodyco == nullptr))
		{
			if (bodyco->debugmeshflag)
			{
				for (size_t i = 0; i < bodyco->debugmesh.size(); i++)
				{
					this->cb_vs_vertexshader->data.wvpMatrix = bodyco->debugmesh.at(i).GetTransformMatrix() * worldMatrix * viewProjectionMatrix;
					this->cb_vs_vertexshader->data.worldMatrix = bodyco->debugmesh.at(i).GetTransformMatrix() * worldMatrix;
					this->cb_vs_vertexshader->ApplyChanges();
					bodyco->debugmesh.at(i).Draw();
				}
			}
		}
		
		if (!(bladeco == nullptr))
		{
			if (bladeco->debugmeshflag)
			{
				for (size_t i = 0; i < bladeco->debugmesh.size(); i++)
				{
					this->cb_vs_vertexshader->data.wvpMatrix = bladeco->debugmesh.at(i).GetTransformMatrix() * worldMatrix * viewProjectionMatrix;
					this->cb_vs_vertexshader->data.worldMatrix = bladeco->debugmesh.at(i).GetTransformMatrix() * worldMatrix;
					this->cb_vs_vertexshader->ApplyChanges();
					bladeco->debugmesh.at(i).Draw();
				}
			}
		}

		/*if (!(testco == nullptr))
		{
			if (testco->debugmeshflag)
			{
				for (size_t i = 0; i < testco->debugmesh.size(); i++)
				{
					this->cb_vs_vertexshader->data.wvpMatrix = testco->debugmesh.at(i).GetTransformMatrix() * worldMatrix * viewProjectionMatrix;
					this->cb_vs_vertexshader->data.worldMatrix = testco->debugmesh.at(i).GetTransformMatrix() * worldMatrix;
					this->cb_vs_vertexshader->ApplyChanges();
					testco->debugmesh.at(i).Draw();
				}
			}
		}*/
		
	//}
}

//=============================================================================
// モデル読み込む関数
//=============================================================================
bool Model::LoadModel_Assimp(const std::string & filePath)
{
	//Assimpライブラリをモデル情報読み込む
	Assimp::Importer importer;
	//importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	const aiScene* Scene = importer.ReadFile(filePath,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_TransformUVCoords |
		aiProcess_ConvertToLeftHanded |
		aiProcess_SplitLargeMeshes |
		aiProcess_JoinIdenticalVertices
		);

	if (!Scene)
		return false;

	//今回のモデルにはZ軸を上にいます
	//そのために実装する前自分で回転させます
	//this model export with Z-Up setting 
	//so I change it manual
	m_GlobalInverseTransform = DirectX::XMMatrixSet(
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, -1, 0, 0,
		0, 0, 0, 1
	);

	//ノード処理
	this->ProcessNode(Scene->mRootNode, Scene, DirectX::XMMatrixIdentity());

	//骨情報処理
	this->ProcessBone(Scene->mRootNode, Scene);

	//ルート骨処理
	this->ProcessRootBone();

	//骨を再バインド
	this->ReBindBone(Scene->mRootNode, Scene, DirectX::XMMatrixIdentity());

	//Assimpシーンフリー
	importer.FreeScene();
	return true;
}

//=============================================================================
// アニメーション読み込む関数
//=============================================================================
bool Model::LoadAnimation_Assimp(const std::string & filePath)
{
	//Assimpライブラリをアニメーション情報読み込む
	Assimp::Importer importer;

	//importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

	const aiScene* Scene = importer.ReadFile(filePath,
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals|
		aiProcess_GenNormals|
		aiProcess_FixInfacingNormals|
		aiProcess_Triangulate |
		aiProcess_TransformUVCoords|
		aiProcess_SplitLargeMeshes |
		aiProcess_JoinIdenticalVertices
		| aiProcess_ConvertToLeftHanded
		);// 
	
	if (!Scene)
		return false;

	//アニメーションを処理します。
	this->ProcessAnimations(Scene);

	//Assimpシーンフリー
	importer.FreeScene();
}

//=============================================================================
// ノード読み込む関数
//assimpのノード階層には2種類があります。
//その一つが
//*************************
//Body
// Body_$AssimpFbx$_GeometricTranslation
//	Body_$AssimpFbx$_Scaling
//   Body_$AssimpFbx$_Rotation
//    Body_$AssimpFbx$_Translation
//Fingers
//*************************
//その二つが
//*************************
//Body
//Fingers
//*************************
//もしこの二つモードを交換したいならAI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTSをtrueにする
//今回falseにします。
//Assimpライブラリがダミーノードを自動的に付けています
//このノードにはノードの親関係を記録しています、特別処理をやらなければならない
//今回のやり方が"Parentname*_*ChildName" という形名前を保存し情報を記録します。
//=============================================================================
//process assimp node in heirachy
//there is two different type node name parsed by assimp
//the first one be like 
//*************************
//Body
// Body_$AssimpFbx$_GeometricTranslation
//	Body_$AssimpFbx$_Scaling
//   Body_$AssimpFbx$_Rotation
//    Body_$AssimpFbx$_Translation
//Fingers
//*************************
//the another be like
//*************************
//Body
//Fingers
//*************************
//if you wanna switch these two just make sure the import setting 
//AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS option is false
//I chose the first one cus it can easily get GeometricTranslation of the node
//Assimp create dummy node which name is ""
//this node contain only parent-child relation and a transform
//I threat this node as "Parentname*_*ChildName" and store all the information;
void Model::ProcessNode(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix)
{
	 BoneNode* tgnode;
	if (node->mName.data && *node->mName.data != '\0')
	{
		//AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTSオプションをfalseになりましたら
		//Assimpのノード名前は"ノード名前_$AssimpFbx$_Transform"という形にします。
		std::vector<std::string>nameSplit;
		StringHelper::Split(node->mName.data, nameSplit, "_$AssimpFbx$_");

		tgnode = FindBoneNodeByName(nameSplit[0]);
		if (tgnode == nullptr)
		{
			tgnode = new BoneNode(nameSplit[0]); 
			mBoneNodeInfo.emplace_back(tgnode);
			tgnode->transform = ToMatrix4f(&node->mTransformation);
		}
		else
		{
			if (StringHelper::IsContain(node->mName.data, "Geometric"))
			{
				tgnode->bIsGeoNode = true;
				tgnode->geotransform *= ToMatrix4f(&node->mTransformation);
			}
			else
			{
				tgnode->transform *= ToMatrix4f(&node->mTransformation);
			}
		}
	}
	else
	{
		//ダミーノードについての特別処理
		//empty node temp solution
		aiString name;
		name.Append(node->mParent->mName.data);
		name.Append("*_*");
		name.Append(node->mChildren[0]->mName.data);
		tgnode = new BoneNode(name.data);
		tgnode->transform = ToMatrix4f(&node->mTransformation);
		mBoneNodeInfo.emplace_back(tgnode);
	}

	//親子関係を設置します
	//set parent child relation
	if (node->mParent != nullptr)
	{
		std::string pname = node->mParent->mName.data;
		if (node->mParent->mName.data && *node->mParent->mName.data != '\0')
		{
			if (StringHelper::IsContain(node->mParent->mName.data, "_$AssimpFbx$_"))
			{
				std::vector<std::string>pnameSplit;
				StringHelper::Split(node->mParent->mName.data, pnameSplit, "_$AssimpFbx$_");
				pname = pnameSplit[0];
			}

			auto pnode = FindBoneNodeByName(pname);
			if (pname != tgnode->name)
			{
				if (!pnode->CheckIfContainChild(tgnode->name))
				{
					pnode->AddChild(tgnode);
					tgnode->parentnode = pnode;
				}
			}
		}
		else
		{
			//ダミーノードの特別処理
			//empty node add parent child relation
			auto pnode = mBoneNodeInfo.at(mBoneNodeInfo.size() - 2);
			pnode->AddChild(tgnode);
			tgnode->parentnode = pnode;
		}
	}

	//geometry transformの値が全transformにかけします
	//we calculate geometry transform with the whole node tree
	DirectX::XMMATRIX nodeTransformMatrix = ToMatrix4f(&node->mTransformation) * parentTransformMatrix;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = pmScene->mMeshes[node->mMeshes[i]];

		DirectX::XMMATRIX temp = nodeTransformMatrix;
		meshes.push_back(this->ProcessMesh(mesh, pmScene, temp));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], pmScene, nodeTransformMatrix);
	}
	
}

//=============================================================================
//メッシュ処理関数
//バーテックス情報とインデックス情報を保存します。
//メッシュに関わる骨の情報も含めています。
//=============================================================================
//process mesh 
//we store vertices indices and texture for the mesh 
//and we create new bone when it is necessary
Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix)
{
	//バーテックスとインデックス情報
	// Data to fill
	std::vector<Vertex3D> vertices;
	std::vector<DWORD> indices;

	//バーテックス情報処理
	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex3D vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		//バーテックス情報がここで直接マティアスを交換させDirectXの座標系を合わせます
		//we transform vertex position in the begining of impliment so that it can fit DirectX coordination
		auto temp = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vertex.pos), transformMatrix);
		DirectX::XMStoreFloat3(&vertex.pos, temp);

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		//ノーマルも
		//as well as normal
		//auto temp1 = XMVector3Normalize(DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vertex.normal), transformMatrix));
		//DirectX::XMStoreFloat3(&vertex.normal, temp1);

		if (mesh->mTextureCoords[0])
		{
			//正接情報
			//Get tangents
			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;

			auto temp2 = XMVector3Normalize(DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vertex.tangent), transformMatrix));
			DirectX::XMStoreFloat3(&vertex.tangent, temp2);

			//we dont use bitangent read by assimp, we calculate in hlsl file
			//Assimpから読み込んだbitangentデータは使えない
			//vertex.bitangent.x = mesh->mBitangents[i].x;
			//vertex.bitangent.y = mesh->mBitangents[i].y;
			//vertex.bitangent.z = mesh->mBitangents[i].z;

			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
			//auto temp3 = XMVector2Normalize(DirectX::XMVector2Transform(DirectX::XMLoadFloat2(&vertex.texCoord), transformMatrix));
			//DirectX::XMStoreFloat2(&vertex.texCoord, temp3);
			
		}
		
		//ここのboneindexにはshaderに入力する骨の情報です。
		//もし下のbone_weights値が-1じゃないならない情報が有効になります。
		//４つbone_weightsの値を掛け算で合わせてメッシュの最後変化値を導き出す(HLSLshaderファイルに行います。)
		vertex.bone_index.w = -1;
		vertex.bone_index.x = -1;
		vertex.bone_index.y = -1;
		vertex.bone_index.z = -1;

		//we judge bone transform by bone weight if bone weight is -1 we dont calculate in shader transform
		vertex.bone_weights.w = -1;
		vertex.bone_weights.x = -1;
		vertex.bone_weights.y = -1;
		vertex.bone_weights.z = -1;

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

	//骨情報（リグ）
	//もし骨のノードは全ノードと違います
	//骨のノードが各メッシュと違います。
	//各メッシュをひっくり返す捜索し登録しない骨を登録します。
	if (mesh->HasBones())
	{
		this->bHasBone = true;

		//iterating all the bones in meshes
		for (unsigned int i = 0; i < mesh->mNumBones; i++) {
			unsigned int BoneIndex = 0;
			std::string BoneName(mesh->mBones[i]->mName.data);

			if (mBoneMaping.find(BoneName) == mBoneMaping.end()) {
				BoneIndex = mNumBones;
				mNumBones++;
				Bone* bi = new Bone();
				mBoneInfo.push_back(bi);
			}
			else {
				BoneIndex = mBoneMaping[BoneName];
			}
			mBoneMaping[BoneName] = BoneIndex;

			//set bone name
			mBoneInfo[BoneIndex]->name = mesh->mBones[i]->mName.data;
			
			//set bone id and weight in vertex 
			for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
				unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
				float Weight = mesh->mBones[i]->mWeights[j].mWeight;
				
				//４つ
				if (vertices.at(VertexID).bone_weights.x != -1)
				{
					if (vertices.at(VertexID).bone_weights.y != -1)
					{
						if (vertices.at(VertexID).bone_weights.z != -1)
						{
							if (vertices.at(VertexID).bone_weights.w != -1)
							{
								//full
							}
							else
							{
								vertices.at(VertexID).bone_index.w = (float)BoneIndex;
								vertices.at(VertexID).bone_weights.w = (float)Weight;
							}
						}
						else
						{
							vertices.at(VertexID).bone_index.z = (float)BoneIndex;
							vertices.at(VertexID).bone_weights.z = (float)Weight;
						}
					}
					else
					{
						vertices.at(VertexID).bone_index.y = (float)BoneIndex;
						vertices.at(VertexID).bone_weights.y = (float)Weight;
					}
				}
				else
				{
					vertices.at(VertexID).bone_index.x = (float)BoneIndex;
					vertices.at(VertexID).bone_weights.x = (float)Weight;
				}
			}
		}
	}

	//テクスチャ情報
	//get texture information with assimp
	std::vector<Texture> textures;
	aiMaterial* material = pmScene->mMaterials[mesh->mMaterialIndex];

	std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, pmScene);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

	std::vector<Texture> specularTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_SPECULAR, pmScene);
	textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());

	std::vector<Texture> normalTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_HEIGHT, pmScene);
	textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());

	std::vector<Texture> depthTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DISPLACEMENT, pmScene);
	textures.insert(textures.end(), depthTextures.begin(), depthTextures.end());

	//instantiate mesh 
	return Mesh(this->device, this->deviceContext, vertices, indices, textures, DirectX::XMMatrixIdentity(),mesh->mName.data);
}

//=============================================================================
//デイバッグメッシュを読み込む
//=============================================================================
//debug block is a simple suqare mesh which represent a bone node position
Mesh Model::ProcessDebugMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix)
{
	// Data to fill
	std::vector<Vertex3D> vertices;
	std::vector<DWORD> indices;

	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{ 
		Vertex3D vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertex.bone_index.w = -1;
		vertex.bone_index.x = -1;
		vertex.bone_index.y = -1;
		vertex.bone_index.z = -1;

		vertex.bone_weights.w = -1;
		vertex.bone_weights.x = -1;
		vertex.bone_weights.y = -1;
		vertex.bone_weights.z = -1;

		vertices.push_back(vertex);

	}

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

	std::vector<Texture> textures;
	aiMaterial* material = pmScene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, pmScene);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

	std::vector<Texture> specularTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_SPECULAR, pmScene);
	textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());

	std::vector<Texture> normalTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_HEIGHT, pmScene);
	textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());

	std::vector<Texture> depthTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DISPLACEMENT, pmScene);
	textures.insert(textures.end(), depthTextures.begin(), depthTextures.end());

	return Mesh(this->device, this->deviceContext, vertices, indices, textures, transformMatrix, mesh->mName.data);
}

//=============================================================================
//ルートボンには全ての骨の親です。
//Assimpライブラリがこの骨を本物に扱っていない、つまりメッシュの中で関連していない
//そこで、まえメッシュを繰り返し捜索する時この骨を見つからない
//ダミーノードのためここの解析がちょっとおかしい。
//****************************
//	 dummy node
//   Hips(parent node)
//		dummy node
//		Spine(child node)
//		dummy node
//		Pelvis(child node)
//****************************
//本物のtransform情報がダミーノードで保存しているから
//三回の探せばtransform情報を手に入れます。
//=============================================================================
//the root bone is all bones father 
//Assimp not creat it as a real bone cus it dont move as an animation node in parsed animation file
//so I parse it in manul

//this structure maybe looks weild but it is not my falut
//Assimp create dummy node which name is ""
//this node contain only parent-child relation and a transform
//so the structure of node be like
//****************************
//	 dummy node
//   Hips(parent node)
//		dummy node
//		Spine(child node)
//		dummy node
//		Pelvis(child node)
//****************************
//the real transform information is recorded in dummy node
//so we have to find three times to get the real Hips transform for the rootbone
void Model::ProcessRootBone() {
	rootbone = new Bone();
	if (bHasBone) {
		for (size_t i = 0; i < mBoneInfo.size(); i++)
		{
			if (mBoneInfo.at(i)->parent_index == -1) {
				auto node = FindBoneNodeByName(mBoneInfo.at(i)->name);
				auto pnode = node->parentnode->parentnode->parentnode;

				rootbone->name = "Hips";//node->parentnode->parentnode->name;
				rootbone->Nodetransform = pnode->transform;
				rootbone->children.push_back(mBoneInfo.at(i));
			}
		}
	}
}

//=============================================================================
//骨処理関数
//これは第二回骨の情報を読み込む
//名前、マトリクス情報、骨関係を保存します
//offsetはReBindBone関数に再設置
//=============================================================================
//process bones store in meshes
//we store name, transform, geotransform and set bone relation
//the bone offset is set by ReBindBone function
void Model::ProcessBone(aiNode * node, const aiScene * pmScene) {
	
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = pmScene->mMeshes[node->mMeshes[i]];
		//detail
		//auto geotransform = FindBoneNodeByName(mesh->mName.data)->geotransform;

		if (mesh->HasBones())
		{
			this->bHasBone = true;
			for (unsigned int j = 0; j < mesh->mNumBones; j++) {
				std::string BoneName(mesh->mBones[j]->mName.data);
				
				unsigned int BoneIndex = mBoneMaping[BoneName];
				auto node = FindBoneNodeByName(BoneName);
				
				assert(BoneIndex + 1);

				mBoneInfo[BoneIndex]->Nodetransform = node->transform;
				//mBoneInfo[BoneIndex]->GeoTransform = geotransform;
				//mBoneInfo[BoneIndex]->BoneOffset = ToMatrix4f(&mesh->mBones[j]->mOffsetMatrix);

				mBoneInfo[BoneIndex]->name = mesh->mBones[j]->mName.data;
				mBoneInfo[BoneIndex]->parent_index = FindBoneParentInAllNode(mesh->mBones[j]->mName.data);
				if (mBoneInfo[BoneIndex]->parent_index != -1) {
					auto pbone = mBoneInfo[mBoneInfo[BoneIndex]->parent_index];
					if (!pbone->CheckIfHaveBoneChild(mBoneInfo[BoneIndex]))
					{
						pbone->children.push_back(mBoneInfo[BoneIndex]);
					}
				}
			}
		}
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessBone(node->mChildren[i], pmScene);
	}
}

//=============================================================================
//骨を再バインディング関数
//Assimpから提供されていてmOffsetMatrix引数が正しくないと思います。
//ここの再バインディングとはモデルのT-Poseから各ノードのマトリクス情報をInverse化本物のmOffsetMatrixとして保存します
//=============================================================================
//in my experiment the mOffsetMatrix of Class aiBone is not exactly correct
//it showes the inverse bind matrix which is not transformed by geometry transform
//so it really cause problem when you want to show a geometry transformmed model
//I dont use this parameter in my work 
//I inverse the name matching node transform in geometry transformmed model and store them as new bone offset 
//it worked!
void Model::ReBindBone(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix)
{
	DirectX::XMMATRIX nodeTransformMatrix = ToMatrix4f(&node->mTransformation) * parentTransformMatrix;

	for (size_t j = 0; j < mBoneInfo.size(); j++)
	{
		auto bonename = mBoneInfo.at(j)->name;

		if (bonename == node->mName.data)
		{
			mBoneInfo.at(j)->BoneOffset = DirectX::XMMatrixInverse(nullptr, nodeTransformMatrix);
		}
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ReBindBone(node->mChildren[i], pmScene, nodeTransformMatrix);
	}
}

//=============================================================================
//アニメーター情報を保存します
//=============================================================================
//process animations store all the animation info in different AnimChannel
void Model::ProcessAnimations(const aiScene* ai_scene) {

	for (unsigned int i = 0; i < ai_scene->mNumAnimations; i++)
	{
		aiAnimation* ai_animation = ai_scene->mAnimations[i];

		Animation* animation = new Animation();
		animation->SetName(ai_animation->mName.C_Str());
		animation->SetTickRate(ai_animation->mTicksPerSecond);
		animation->SetDuration(ai_animation->mDuration);
		mAnimations.push_back(animation);
		curanim = mAnimations.size() - 1;

		for (unsigned int j = 0; j < ai_animation->mNumChannels; j++)
		{
			aiNodeAnim* ai_channel = ai_animation->mChannels[j];

			//Assimpからもらったアニメーションチャンネル情報は_$AssimpFbx$_を付ける名前もあります。
			//ここで特別処理します
			if (StringHelper::IsContain(ai_channel->mNodeName.data, "_$AssimpFbx$_"))
			{
				std::vector<std::string>nameSplit;
				StringHelper::Split(ai_channel->mNodeName.data, nameSplit, "_$AssimpFbx$_");

				AnimChannel* channel = FineAnimChannelByName(nameSplit[0]);
				if (channel == nullptr)
				{
					channel = new AnimChannel();
					channel->name = nameSplit[0];
					animation->AddChannel(channel);
				}

				if (nameSplit[1] == "Translation")
				{
					for (unsigned int k = 0; k < ai_channel->mNumPositionKeys; k++)
					{
						aiVectorKey ai_key = ai_channel->mPositionKeys[k];
						VectorKey position_key = { ai_key.mTime ,DirectX::XMFLOAT3(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z) }; //{ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z } };
						channel->AddPositionKey(position_key);
					}
				}
				else if (nameSplit[1] == "Rotation")
				{
					for (unsigned int k = 0; k < ai_channel->mNumRotationKeys; k++)
					{
						aiQuatKey ai_key = ai_channel->mRotationKeys[k];
						QuaternionKey rotation_key = { ai_key.mTime,DirectX::XMFLOAT4(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z,ai_key.mValue.w) };//{ ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z,ai_key.mValue.w  } };
						channel->AddRotationKey(rotation_key);
					}
				}
				else if (nameSplit[1] == "Scaling")
				{
					for (unsigned int k = 0; k < ai_channel->mNumScalingKeys; k++)
					{
						aiVectorKey ai_key = ai_channel->mScalingKeys[k];
						VectorKey scaling_key = { ai_key.mTime ,DirectX::XMFLOAT3(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z) };//{ ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z } };
						channel->AddScalingKey(scaling_key);
					}
				}
			}
			else
			{
				//普通処理
				AnimChannel* channel = FineAnimChannelByName(ai_channel->mNodeName.data);
				if (channel == nullptr)
				{
					channel = new AnimChannel();
					channel->name = ai_channel->mNodeName.data;
					animation->AddChannel(channel);
				}

				for (unsigned int k = 0; k < ai_channel->mNumPositionKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mPositionKeys[k];
					VectorKey position_key = { ai_key.mTime ,DirectX::XMFLOAT3(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z) }; //DirectX::XMFLOAT3(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z)
					channel->AddPositionKey(position_key);
				}

				for (unsigned int k = 0; k < ai_channel->mNumRotationKeys; k++)
				{
					aiQuatKey ai_key = ai_channel->mRotationKeys[k];
					QuaternionKey rotation_key = { ai_key.mTime,DirectX::XMFLOAT4(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z,ai_key.mValue.w) };//{ ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z,ai_key.mValue.w  } };
					channel->AddRotationKey(rotation_key);
				}

				for (unsigned int k = 0; k < ai_channel->mNumScalingKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mScalingKeys[k];
					VectorKey scaling_key = { ai_key.mTime ,DirectX::XMFLOAT3(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z) };//{ ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z } };
					channel->AddScalingKey(scaling_key);
				}
			}
		}
	}
}

//=============================================================================
//テクスチャ情報処理
//=============================================================================
TextureStorageType Model::DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType)
{
	if (pMat->GetTextureCount(textureType) == 0)
		return TextureStorageType::None;

	aiString path;
	pMat->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();
	//Check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}
	//Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}
	//Lastly check if texture is a filepath by checking for period before extension name
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None; // No texture exists
}

//=============================================================================
//マテリアル情報処理
//=============================================================================
std::vector<Texture> Model::LoadMaterialTextures(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene) //const aiScene * pScene
{
	std::vector<Texture> materialTextures;
	TextureStorageType storetype = TextureStorageType::Invalid;
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0) //If there are no textures
	{
		storetype = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		switch (textureType)
		{
		case aiTextureType_DIFFUSE:
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
			if (aiColor.IsBlack()) //If color = black, just use grey
			{
				materialTextures.push_back(Texture(this->device, Colors::UnloadedTextureColor, textureType));
				return materialTextures;
			}
			materialTextures.push_back(Texture(this->device, Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType));
			return materialTextures;
		case aiTextureType_SPECULAR:
			materialTextures.push_back(Texture(this->device, Color(255, 255, 255), textureType));
			return materialTextures;

		case aiTextureType_HEIGHT:
			materialTextures.push_back(Texture(this->device, Color(128, 128, 255), textureType));
			return materialTextures;

		case aiTextureType_DISPLACEMENT:
			materialTextures.push_back(Texture(this->device, Color(0, 0, 0), textureType));
			return materialTextures;
		}
	}
	else
	{
		for (UINT i = 0; i < textureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			TextureStorageType storetype = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
			switch (storetype)
			{
			case TextureStorageType::EmbeddedIndexCompressed:
			{
				int index = GetTextureIndex(&path);
				Texture embeddedIndexedTexture(	this->device,
												reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),
												pScene->mTextures[index]->mWidth,
												textureType);
				materialTextures.push_back(embeddedIndexedTexture);
				break;
			}
			case TextureStorageType::EmbeddedCompressed:
			{
				const aiTexture * pTexture = pScene->GetEmbeddedTexture(path.C_Str());
				Texture embeddedTexture(this->device,
										reinterpret_cast<uint8_t*>(pTexture->pcData),
										pTexture->mWidth,
										textureType);
				materialTextures.push_back(embeddedTexture);
				break;
			}
			case TextureStorageType::Disk:
			{
				std::string filename = this->directory + '\\' + path.C_Str();
				Texture diskTexture(this->device, filename, textureType);
				materialTextures.push_back(diskTexture);
				break;
			}
			}
		}
	}

	if (materialTextures.size() == 0)
	{
		materialTextures.push_back(Texture(this->device, Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
	}
	return materialTextures;

}

//=============================================================================
//テクスチャインデックス情報
//=============================================================================
int Model::GetTextureIndex(aiString * pStr)
{
	assert(pStr->length >= 2);
	return atoi(&pStr->C_Str()[1]);
}

//=============================================================================
//ツール関数
//AssimpマトリクスからDirectXマトリクスに
//=============================================================================
//we cant change aiMatrix4x4 matrix smoothly into DirectX::XMMATRIX
DirectX::XMMATRIX Model::ToMatrix4f(const aiMatrix4x4 *ai_mat)
{
	DirectX::XMMATRIX a = DirectX::XMMatrixSet(
		ai_mat->a1, ai_mat->a2, ai_mat->a3, ai_mat->a4,
		ai_mat->b1, ai_mat->b2, ai_mat->b3, ai_mat->b4,
		ai_mat->c1, ai_mat->c2, ai_mat->c3, ai_mat->c4,
		ai_mat->d1, ai_mat->d2, ai_mat->d3, ai_mat->d4);
	DirectX::XMMATRIX b = DirectX::XMMatrixTranspose(a);
	return b;
}

//=============================================================================
//デイバッグメッシュ関数
//=============================================================================
//load debug block (obj file)
void Model::LoadBoneDebugBlock(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader) {
	Assimp::Importer importer;

	const aiScene* Scene = importer.ReadFile("Data\\Objects\\debugBlock.obj",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded|
		aiProcess_JoinIdenticalVertices
		);

	if (!Scene)
		return ;

	ProcessDebugNode(Scene->mRootNode, Scene, DirectX::XMMatrixIdentity());
	
	ProcessCollsion(Scene->mRootNode, Scene);
}

//=============================================================================
//デイバッグメッシュ関数
//=============================================================================
//create debug block fit the bone array size
void Model::ProcessDebugNode(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix)
{
	DirectX::XMMATRIX nodeTransformMatrix;
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = pmScene->mMeshes[node->mMeshes[i]];

		//debugblockmesh = this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity());
		for (size_t j = 0; j < mBoneInfo.size(); j++)
		{
			nodeTransformMatrix = mBoneInfo.at(j)->Nodetransform;
			debugBlocks.push_back(this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity()));
		}
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessDebugNode(node->mChildren[i], pmScene, nodeTransformMatrix);
	}
}

//=============================================================================
//当たり判定ブロック
//=============================================================================
void Model::ProcessCollsion(aiNode * node, const aiScene * pmScene) {
	//body collision
	bodyco = new CollsionObject();
	bodyco->boneindex = 1;//pelvis index
	bodyco->ct = CollsionType::Player;
	bodyco->debugmeshflag = false;
	auto cnode = node->mChildren[0];
	aiMesh* mesh = pmScene->mMeshes[cnode->mMeshes[0]];
	bodyco->debugmesh.push_back(this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity()));
	bodyco->aabb = BoundingBox();
	bodyco->oritransform = DirectX::XMMatrixScaling(150.0f, 50.0f, 50.0f);

	//blade collision
	bladeco = new CollsionObject();
	bladeco->boneindex = 51;//right blade index
	bladeco->ct = CollsionType::Blade;
	bladeco->debugmeshflag = false;
	bladeco->debugmesh.push_back(this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity()));
	bladeco->aabb = BoundingBox();
	auto bScale = DirectX::XMMatrixScaling(10.0f, 10.0f, 100.0f);
	auto bTrans = DirectX::XMMatrixTranslation(0.0f, 0.0f, 50.0f);
	bladeco->oritransform = bScale * bTrans;

	//test collision
	/*testco = new CollsionObject();
	testco->boneindex = -1;
	testco->ct = CollsionType::Enemy;
	testco->debugmeshflag = true;
	testco->debugmesh.push_back(this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity()));
	testco->aabb = BoundingBox();
	auto tScale = DirectX::XMMatrixScaling(50.0f, 150.0f, 50.0f);
	auto tTrans = DirectX::XMMatrixTranslation(0.0f, 75.0f, -350.0f);
	testco->oritransform = tScale * tTrans;*/
}

//=============================================================================
//デイバッグノードツリー関数
//=============================================================================
//debug display the structure of Assimp node
void Model::DisplayNode(const aiScene * scene)
{
	//meshes
	unsigned int mnum = scene->mNumMeshes;
	for (unsigned int i = 0; i < mnum; i++)
	{
		auto mesh = scene->mMeshes[i];
		auto mbones = mesh->mNumBones;
		auto hbones = mesh->HasBones();

		//bones
		for (unsigned int k = 0; k < mbones; k++)
		{
			auto bonename = mesh->mBones[k]->mName;
			auto wnum = mesh->mBones[k]->mNumWeights;
			for (unsigned int j = 0; j < wnum; j++)
			{
				auto tweight = mesh->mBones[k]->mWeights[j];
				auto wVertexId = tweight.mVertexId;
				auto mWeight = tweight.mWeight;
			}

		}
	}

	//node
	aiNode* nodenum = scene->mRootNode;
	auto name = nodenum->mName;
	auto transform = nodenum->mTransformation;
	auto mNumMeshes = nodenum->mNumMeshes;
	for (unsigned int i = 0; i < nodenum->mNumMeshes; i++)
	{
		unsigned int meshID = nodenum->mMeshes[i];
	}
	unsigned int nodecount = 1;
	nodecount = GetChildNodeCount(nodenum, nodecount);

	//animation
	auto numanim = scene->mNumAnimations;
	for (unsigned int i = 0; i < numanim; i++)
	{
		aiAnimation* anim = scene->mAnimations[i];
		auto name = anim->mName;
		auto duration = anim->mDuration;
		auto numchannel = anim->mNumChannels;
		for (unsigned int j = 0; j < numchannel; j++)
		{
			aiNodeAnim* channel = anim->mChannels[j];
			auto nodename = channel->mNodeName;
			aiAnimBehaviour posstate = channel->mPostState;
			aiAnimBehaviour prestate = channel->mPreState;
		}

		auto nummeshchannel = anim->mNumMeshChannels;
		for (unsigned int j = 0; j < nummeshchannel; j++)
		{
			aiMeshAnim* channel = anim->mMeshChannels[j];
			auto name = channel->mName;

		}

		auto nummorpchannel = anim->mNumMorphMeshChannels;
		for (unsigned int j = 0; j < nummorpchannel; j++)
		{
			aiMeshMorphAnim* channel = anim->mMorphMeshChannels[j];
		}
	}
}

//=============================================================================
//デイバッグノードツリー関数
//=============================================================================
unsigned int Model::GetChildNodeCount(aiNode* node, unsigned int nodecount)
{
	unsigned int rtn = nodecount;
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		auto child = node->mChildren[i];
		//infomation
		auto transform = child->mTransformation;
		auto name = child->mName;
		auto mNumMeshes = child->mNumMeshes;
		for (unsigned int i = 0; i < child->mNumMeshes; i++)
		{
			unsigned int meshID = child->mMeshes[i];
		}

		if (child->mNumChildren != 0)
		{
			rtn = GetChildNodeCount(child, rtn);
		}
		
		if (i == node->mNumChildren - 1)
		{
			rtn += node->mNumChildren;
		}
	}
	return rtn;
}

//=============================================================================
//デイバッグノードツリー関数
//=============================================================================
//update bone transform
void Model::BoneTransform(float TimeInSeconds, std::vector<DirectX::XMMATRIX>& Transforms)
{
	//現在使ているアニメーション
	Animation* mAnimation = this->mAnimations.at(curanim);
	
	//アニメーションのtickrate(フレーム単位)
	float TicksPerSecond = mAnimation->GetTickRate() != 0 ?
		mAnimation->GetTickRate() : 25.0f;
	float TimeInTicks = TimeInSeconds * TicksPerSecond / 10.0f;
	animtimer += TimeInTicks * animplayrate;

	if (animationPlayStyle == AnimationPlayStyle::PlayOnce)
	{
		if (animtimer >= mAnimation->GetDuration() - 1)
		{
			animtimer = mAnimation->GetDuration() - 1;
			//set animation end function

			if (curanim == ANIM_STANDTOFIGHTIDLE)
			{
				PlayAnimaition(ANIM_FIGHTIDLE, AnimationPlayStyle::PlayLoop);
			}
			if (curanim == ANIM_BLOCKLEFT)
			{
				PlayAnimaition(ANIM_BLOCKIDLE, AnimationPlayStyle::PlayLoop);
			}
			if (curanim == ANIM_BLOCKRIGHT)
			{
				PlayAnimaition(ANIM_BLOCKIDLE, AnimationPlayStyle::PlayLoop);
			}
			if (curanim == ANIM_CHARGEATTACK)
			{
				PlayAnimaition(ANIM_FIGHTIDLE, AnimationPlayStyle::PlayLoop);
			}
		}
	}
	else if(animationPlayStyle == AnimationPlayStyle::PlayLoop)
	{

	}
	float AnimationTime = fmod(animtimer, mAnimation->GetDuration());
	
	//経由した時間でアニメーションの骨情報を更新します
	ReadBoneHeirarchy(AnimationTime, rootbone, DirectX::XMMatrixIdentity());//m_GlobalInverseTransform

	//デイバッグポイントを更新します
	Transforms.resize(mBoneInfo.size());
	for (unsigned int i = 0; i < mBoneInfo.size(); i++) {
		Transforms[i] = mBoneInfo[i]->FinalTransformation;
		debugBlocks.at(i).transformMatrix = DirectX::XMMatrixInverse(nullptr, mBoneInfo.at(i)->BoneOffset) * Transforms[i] * m_GlobalInverseTransform;
	}

	//骨情報をshaderファイルに入力
	//this->deviceContext->VSSetConstantBuffers(1, 1, this->cb_bone_info.GetAddressOf());
	for (unsigned int i = 0; i < Transforms.size(); i++)
	{
		this->cb_bone_info.data.bones[i] = Transforms.at(i);

	}
	this->cb_bone_info.ApplyChanges();

}

//=============================================================================
//骨ツリー更新関数
//経由した時間でアニメーションの骨情報を更新します
//=============================================================================
//iterating bone heirarchy
//set bone transform by animation interpolating time
void Model::ReadBoneHeirarchy(float AnimationTime, Bone* bone, const DirectX::XMMATRIX& ParentTransform)
{
	//全ノードからターゲットノードを探します
	auto node = FindBoneNodeByName(bone->name);
	DirectX::XMMATRIX NodeTransformation = node->parentnode->transform;
	DirectX::XMMATRIX GlobalTransformation;

	//もしアニメーションチャンネルを保存したら
	AnimChannel* channel = FineAnimChannelByName(bone->name);
	if (channel) {
		//当時点のScalingを取ります
		// Interpolate scaling and generate scaling transformation matrix
		DirectX::XMFLOAT3 Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, channel);
		DirectX::XMVECTOR ScalingV = DirectX::XMLoadFloat3(&Scaling);
		DirectX::XMMATRIX ScalingM = DirectX::XMMatrixScalingFromVector(ScalingV);

		//当時点のRotationを取ります
		// Interpolate rotation and generate rotation transformation matrix
		DirectX::XMFLOAT4 RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, channel);
		DirectX::XMVECTOR RotationV = DirectX::XMLoadFloat4(&RotationQ);
		RotationV = DirectX::XMVector4Normalize(RotationV);
		DirectX::XMMATRIX RotationM = DirectX::XMMatrixRotationQuaternion(RotationV);

		//当時点のTranslationを取ります
		// Interpolate translation and generate translation transformation matrix
		DirectX::XMFLOAT3 Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, channel);
		DirectX::XMVECTOR TranslationV = DirectX::XMLoadFloat3(&Translation);
		DirectX::XMMATRIX TranslationM = DirectX::XMMatrixTranslationFromVector(TranslationV);

		//合併
		// Combine the above transformations
		NodeTransformation = ScalingM * RotationM * TranslationM;//srt   
		
	}

	//親マトリクスを掛け算します
	GlobalTransformation = NodeTransformation * ParentTransform;

	//最後の変更マトリクスを計算します
	if (mBoneMaping.find(bone->name) != mBoneMaping.end()) {
		unsigned int BoneIndex = mBoneMaping[bone->name];

		mBoneInfo[BoneIndex]->FinalTransformation = mBoneInfo[BoneIndex]->BoneOffset * (GlobalTransformation);
		
	}

	//子骨を繰り返し
	for (size_t i = 0; i < bone->children.size(); i++)
	{
		auto cbone = bone->children.at(i);
		ReadBoneHeirarchy(AnimationTime, cbone, GlobalTransformation);
	}
}

//=============================================================================
//親骨を探し関数
//=============================================================================
//find root bone in bone array
//this bone is not the real root bone 
//we have to find the real on in all node
int Model::FindBoneParentInAllNode(std::string childname)
{
	auto cnode = FindBoneNodeByName(childname);
	auto pnode = cnode->parentnode;
	if (pnode!= nullptr && StringHelper::IsContain(pnode->name, "*_*"))
	{
		pnode = pnode->parentnode;
	}
	if (mBoneMaping.find(pnode->name) == mBoneMaping.end()) 
	{
		return -1;
	}
	else {
		return mBoneMaping[pnode->name];
	}

}

//=============================================================================
//Rotation探し関数
//線形補間限定
//=============================================================================
//Interpolate Rotation by AnimationTime
void Model::CalcInterpolatedRotation(DirectX::XMFLOAT4& Out, float AnimationTime, AnimChannel* channel)
{
	// we need at least two values to interpolate...
	auto num = channel->NumRotationKeys();
	if (num == 1) {
		Out = channel->GetRotationKey(0).value;
		return;
	}

	if (num == 0)
	{
		Out = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f,1.0f);
		return;
	}
	unsigned int RotationIndex = FindRotation(AnimationTime, channel);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	auto mrrk = channel->GetRotationKey(RotationIndex);
	auto mnrk = channel->GetRotationKey(NextRotationIndex);
	assert(NextRotationIndex < channel->NumRotationKeys());
	float DeltaTime = mnrk.time - mrrk.time;
	float Factor = (AnimationTime - mrrk.time) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	//XMQuaternionSlerp
	
	DirectX::XMFLOAT4 StartRotationQ = mrrk.value;
	DirectX::XMFLOAT4 EndRotationQ = mnrk.value;
	auto StartRotationV = DirectX::XMLoadFloat4(&StartRotationQ);
	auto EndRotationV = DirectX::XMLoadFloat4(&EndRotationQ);
	DirectX::XMVECTOR result = DirectX::XMQuaternionSlerp(StartRotationV, EndRotationV, Factor);
	//result = DirectX::XMVector4Normalize(result);
	DirectX::XMStoreFloat4(&Out,result);
}

//=============================================================================
//Rotationキー探し関数
//=============================================================================
//find Rotation key by AnimationTime
unsigned int Model::FindRotation(float AnimationTime, AnimChannel* pAnimChannel)
{
	assert(pAnimChannel->NumRotationKeys() > 0);
	for (unsigned int i = 0; i < pAnimChannel->NumRotationKeys() - 1; i++) {
		if (AnimationTime < (float)pAnimChannel->GetRotationKey(1 + i).time) {
			return i;
		}
	}
	assert(0);
}

//=============================================================================
//Scaling探し関数
//線形補間限定
//=============================================================================
//Interpolate Scaling by AnimationTime
void Model::CalcInterpolatedScaling(DirectX::XMFLOAT3& Out, float AnimationTime, AnimChannel* channel)
{
	auto num = channel->NumScalingKeys();

	if (num == 1) {
		Out = channel->GetScalingKey(0).value;
		return;
	}
	if (num == 0)
	{
		Out = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, channel);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < num);
	auto mrrk = channel->GetScalingKey(ScalingIndex);
	auto mnrk = channel->GetScalingKey(NextScalingIndex);
	float DeltaTime = mnrk.time - mrrk.time;
	float Factor = (AnimationTime - mrrk.time)/ DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	DirectX::XMFLOAT3 Start = mrrk.value;
	DirectX::XMFLOAT3 End = mnrk.value;
	DirectX::XMFLOAT3 Delta = DirectX::XMFLOAT3((End.x - Start.x), (End.y - Start.y), (End.z - Start.z));
	DirectX::XMFLOAT3 result = DirectX::XMFLOAT3((Start.x + Factor * Delta.x), (Start.y + Factor * Delta.y), (Start.z + Factor * Delta.z));
	Out = DirectX::XMFLOAT3(result.x,result.y, result.z);
}

//=============================================================================
//Scalingキー探し関数
//=============================================================================
//find Scaling key by AnimationTime
unsigned int Model::FindScaling(float AnimationTime, AnimChannel* panimchannel)
{
	assert(panimchannel->NumScalingKeys() > 0);

	for (unsigned int i = 0; i < panimchannel->NumScalingKeys() - 1; i++) {
		if (AnimationTime < (float)panimchannel->GetScalingKey(i + 1).time) {
			return i;
		}
	}
	assert(0);
	return 0;
}

//=============================================================================
//Position探し関数
//線形補間限定
//=============================================================================
//Interpolate Position by AnimationTime
void Model::CalcInterpolatedPosition(DirectX::XMFLOAT3& Out, float AnimationTime, AnimChannel* channel)
{
	auto num = channel->NumPositionKeys();
	if (channel->NumPositionKeys() == 1) {
		Out = channel->GetPositionKey(0).value;
		return;
	}
	if (num == 0)
	{
		Out = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		return;
	}
	unsigned int PositionIndex = FindPosition(AnimationTime, channel);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < num);
	auto mrrk = channel->GetPositionKey(PositionIndex);
	auto mnrk = channel->GetPositionKey(NextPositionIndex);
	float DeltaTime = mnrk.time - mrrk.time;
	float Factor = (AnimationTime - (float)mrrk.time) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);

	DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&mrrk.value);
	DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&mnrk.value);
	DirectX::XMVECTOR temp = DirectX::XMVectorLerp(p0, p1, Factor);
	DirectX::XMStoreFloat3(&Out, temp);
}

//=============================================================================
//Positionキー探し関数
//=============================================================================
//find Position key by AnimationTime
unsigned int Model::FindPosition(float AnimationTime, AnimChannel* channel)
{
	for (unsigned int i = 0; i < channel->NumPositionKeys() - 1; i++) {
		if (AnimationTime < (float)channel->GetPositionKey(i + 1).time) {
			return i;
		}
	}

	assert(0);

	return 0;
}

//=============================================================================
//モデル更新処理関数
//=============================================================================
//update
void Model::Update(float dt, const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix)
{
	if (this->IfHasBone())
	{
		this->UpdateAnimation(dt);
		this->UpdateCollisionBox(worldMatrix, viewProjectionMatrix);
		this->UpdateAnimationPosition();
	}
}

//=============================================================================
//アニメーション更新処理関数
//=============================================================================
//update bone animation
void Model::UpdateAnimation(float dt)
{
	float RunningTime = dt / 100.0f;

	this->BoneTransform(RunningTime, mworkBone);


}

//=============================================================================
//衝突更新処理関数
//=============================================================================
void Model::UpdateCollisionBox(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix)
{
	if (bodyco->boneindex != -1)//pelvis transformation
	{
		auto bodymatrix = debugBlocks.at(bodyco->boneindex).transformMatrix;
		//auto trans = bodymatrix * m_GlobalInverseTransform;

		for (size_t i = 0; i < bodyco->debugmesh.size(); i++) 
		{
			bodyco->debugmesh.at(i).transformMatrix = bodyco->oritransform * bodymatrix;
			bodyco->aabb.Center = XMFLOAT3(0, 0, 0);
			bodyco->aabb.Extents = XMFLOAT3(1.0f, 1.0f, 1.0f);
			auto collisonWorldMatrix = bodyco->debugmesh.at(i).transformMatrix * worldMatrix * viewProjectionMatrix;
			bodyco->aabb.Transform(bodyco->aabb, collisonWorldMatrix);
		}
	}

	if (bladeco->boneindex != -1)//pelvis transformation
	{
		auto bladematrix = debugBlocks.at(bladeco->boneindex).transformMatrix;
		//auto trans = bladematrix * m_GlobalInverseTransform;
		for (size_t i = 0; i < bladeco->debugmesh.size(); i++)
		{
			bladeco->debugmesh.at(i).transformMatrix = bladeco->oritransform * bladematrix; //*trans;
			bladeco->aabb.Center = XMFLOAT3(0, 0, 0);
			bladeco->aabb.Extents = XMFLOAT3(1.0f, 1.0f, 1.0f);
			auto collisonWorldMatrix = bladeco->debugmesh.at(i).transformMatrix * worldMatrix * viewProjectionMatrix;
			bladeco->aabb.Transform(bladeco->aabb, collisonWorldMatrix);
		}
	}

	//test collision
	/*for (size_t i = 0; i < testco->debugmesh.size(); i++)
	{
		testco->debugmesh.at(i).transformMatrix = testco->oritransform;
		testco->aabb.Center = XMFLOAT3(0, 0, 0);
		testco->aabb.Extents = XMFLOAT3(1.0f, 1.0f, 1.0f);
		testco->aabb.Transform(testco->aabb, testco->debugmesh.at(i).transformMatrix);
	}*/

	//blade - test
	//DirectX::ContainmentType coresult = bladeco->aabb.Contains(testco->aabb);
	//if (precoreslut == 0 && coresult != 0)
	//{
	//	//hit
	//	bladeco->debugmeshflag = true;
	//	testco->debugmeshflag = false;
	//}
	//if (precoreslut != 0 && coresult == 0)
	//{
	//	bladeco->debugmeshflag = false;
	//	testco->debugmeshflag = true;
	//}
	//precoreslut = coresult;
}

//=============================================================================
//アニメーションから位置を移動する関数
//アニメーションを再生する時モデルを移動させるから
//もし何らかの理由があって途中でアニメーションを中止しましたら
//この結果を元の位置に加えたら正しい位置を得る
//=============================================================================
void Model::UpdateAnimationPosition() {
	auto pelvisbonetrans = debugBlocks.at(bodyco->boneindex).transformMatrix;
	DirectX::XMFLOAT4X4 mat44;
	XMStoreFloat4x4(&mat44, pelvisbonetrans);
	curanimpos.x = mat44._41;
	curanimpos.y = mat44._42;
}

//=============================================================================
//モデル骨があるの確認関数
//=============================================================================
bool Model::IfHasBone() 
{
	return this->bHasBone;
}

//=============================================================================
//ターゲットノード探し関数
//=============================================================================
//find node in all node
BoneNode* Model::FindBoneNodeByName(std::string bonename)
{
	for (unsigned int i = 0; i < this->mBoneNodeInfo.size(); i++)
	{
		if (mBoneNodeInfo.at(i)->name == bonename)
		{
			return mBoneNodeInfo.at(i);
		}
	}
	return nullptr;
}


//=============================================================================
//アニメーションチャンネル探し関数
//=============================================================================
AnimChannel* Model::FineAnimChannelByName(std::string bonename)
{
	Animation* animation = mAnimations.at(curanim);
	if (animation->HasChannel(bonename))
	{
		return &animation->GetChannel(bonename);
	}
	return nullptr;
}

//=============================================================================
//アニメーションプレイ関数
//=============================================================================
void Model::PlayAnimaition(const int num, AnimationPlayStyle aps)
{
	animationPlayStyle = aps;
	curanim = num;
	animtimer = 0.0f;

	preanimpos = curanimpos;
	curanimpos = { 0,0,0 };
}

//=============================================================================
//現在再生するアニメーション
//=============================================================================
unsigned int Model::GetCurrentAnimationClipIndex()
{
	return curanim;
}

//=============================================================================
//全てのアニメーション数
//=============================================================================
unsigned int Model::GetAnimationClipSize() {
	return mAnimations.size();
}

//=============================================================================
//アニメーション再生速度設置
//=============================================================================
void Model::SetAnimationSpeed(float speed) {
	animplayrate = speed;
}

//=============================================================================
//アニメーション再生速度取る
//=============================================================================
float Model::GetAnimationSpeed() {
	return animplayrate;
}

//-------------------------------
//Animation Function(アニメーションクラス関数)
//-------------------------------
//名前設置
void Animation::SetName(std::string name) 
{
	this->name = name;
}

//名前ゲット
std::string Animation::GetName()
{
	return this->name;
}

//TickRate設置
void Animation::SetTickRate(double rate)
{
	this->tick_rate = rate;
}

//TickRateゲット
double Animation::GetTickRate()
{
	return this->tick_rate;
}

//チャンネル増加
void Animation::AddChannel(AnimChannel * channel)
{
	channels_mapping[channel->name] = channels.size();
	channels.emplace_back(std::move(channel));
}

//チャンネルチェック
bool Animation::HasChannel(std::string name)
{
	auto found = channels_mapping.find(name);
	return (found != channels_mapping.end());
}

//チャンネルとる
AnimChannel& Animation::GetChannel(std::string name)
{
	auto found = channels_mapping.find(name);

	assert(found != channels_mapping.end());

	return *channels[found->second];
}

//時間帯設置
void Animation::SetDuration(double ticks)
{
	this->duration = ticks;
}

//時間帯とる
double Animation::GetDuration()
{
	return this->duration;
}


//-------------------------------
//AnimChannel Function(アニメーションチャンネル関数)
//-------------------------------
//Positionキー添加
void AnimChannel::AddPositionKey(VectorKey key)
{
	position_keys.push_back(key);
}

//Rotationキー添加
void AnimChannel::AddRotationKey(QuaternionKey key)
{
	rotation_keys.push_back(key);
}

//Scalingキー添加
void AnimChannel::AddScalingKey(VectorKey key)
{
	scaling_keys.push_back(key);
}

//Positionキー数を取る
unsigned int AnimChannel::NumPositionKeys()
{
	return position_keys.size();
}

//Rotationキー数を取る
unsigned int AnimChannel::NumRotationKeys()
{
	return rotation_keys.size();
}

//Scalingキー数を取る
unsigned int AnimChannel::NumScalingKeys()
{
	return scaling_keys.size();
}

//Positionキーを取る
VectorKey& AnimChannel::GetPositionKey(unsigned int index)
{
	return position_keys[index];
}

//Rotationキーを取る
QuaternionKey& AnimChannel::GetRotationKey(unsigned int index)
{
	return rotation_keys[index];
}

//Scalingキーを取る
VectorKey& AnimChannel::GetScalingKey(unsigned int index)
{
	return scaling_keys[index];
}

//-------------------------------
//BoneNode Function(ノードクラス関数)
//-------------------------------
//名前設置
BoneNode::BoneNode(std::string name)
{
	this->name = name;
}

//子ノード添加
void BoneNode::AddChild(BoneNode* child)
{
	this->children.emplace_back(child);
}

//メッシュインデックス設置
void BoneNode::AddMeshIndex(unsigned int index)
{
	this->meshesIndex.emplace_back(index);
}

//子ノードたちの中であるかどうか
bool BoneNode::CheckIfContainChild(std::string cname) 
{
	for (unsigned int i = 0; i < this->children.size(); i++)
	{
		if (this->children.at(i)->name == cname)
			return true;
	}
	return false;
}

//-------------------------------
//Bone Function(骨クラス関数)
//-------------------------------
//子ノードたちの中であるかどうか
bool Bone::CheckIfHaveBoneChild(Bone* cbone) 
{
	for (unsigned int i = 0; i < this->children.size(); i++)
	{
		if (this->children.at(i)->name == cbone->name)
			return true;
	}
	return false;
}

//-------------------------------
//CollsionObject Function(衝突判定関数)
//-------------------------------
CollsionObject::CollsionObject(const CollsionObject & co) {
	this->aabb = co.aabb;
	this->ct = co.ct;
	this->debugmesh = co.debugmesh;
	this->debugmeshflag = co.debugmeshflag;
}

CollsionObject::CollsionObject() {

}

CollsionObject* Model::GetBladeCollsionObject() 
{
	return bladeco;
}

CollsionObject* Model::GetBodyCollsionObject() 
{
	return bodyco;
}

//-------------------------------------------
//mesh(メッシュ情報関数)
//-------------------------------------------
std::vector<Mesh> Model::GetMesh()
{
	return this->meshes;
}