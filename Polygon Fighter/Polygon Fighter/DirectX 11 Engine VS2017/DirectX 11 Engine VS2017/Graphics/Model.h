//=============================================================================
//
// グラフィックス [Graphics.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "Mesh.h"
#include <map>
//#include "../FBXLoader.h"
#include "../Timer.h"
#include <DirectXCollision.h>


using namespace DirectX;

//ベクトルキー
struct VectorKey {
	double time;
	DirectX::XMFLOAT3 value;
};

//クオータニオンキー
struct QuaternionKey {
	double time;
	DirectX::XMFLOAT4 value;
};

//ノード情報
class BoneNode
{
public:
	BoneNode(std::string name);
	DirectX::XMMATRIX transform;//位置
	DirectX::XMMATRIX geotransform = DirectX::XMMatrixIdentity();//ジオメトリ位置
	std::string name;//名前
	bool bIsBoneNode;//骨ですか
	bool bIsGeoNode = false;//ジオメトリノードですか
	BoneNode* parentnode = nullptr;//親

	void AddChild(BoneNode* child);
	void AddMeshIndex(unsigned int index);
	bool CheckIfContainChild(std::string cname);
	std::vector<unsigned int> meshesIndex;
	std::vector<BoneNode*> children;
};

//アニメーションチャンネルクラス
class AnimChannel
{
public:
	std::string name;//名前
	void AddPositionKey(VectorKey key);
	void AddRotationKey(QuaternionKey key);
	void AddScalingKey(VectorKey key);
	unsigned int NumPositionKeys();
	unsigned int NumRotationKeys();
	unsigned int NumScalingKeys();
	VectorKey& GetPositionKey(unsigned int index);
	QuaternionKey& GetRotationKey(unsigned int index);
	VectorKey& GetScalingKey(unsigned int index);

private:
	std::vector<VectorKey> scaling_keys;//スケールキーアレー
	std::vector<QuaternionKey> rotation_keys;//ローテーションキーアレー
	std::vector<VectorKey> position_keys;//ポジションキーアレー
};

//骨クラス
class Bone
{
public:
	DirectX::XMMATRIX Nodetransform;//位置
	DirectX::XMMATRIX BoneOffset;//オフセット
	DirectX::XMMATRIX FinalTransformation;//最後変化量
	DirectX::XMMATRIX GeoTransform;//ジオメトリ

	int parent_index = -1;//親インデックス

	std::string name = "Empty";//名前
	std::vector<Bone*> children;//子供アレー

	bool CheckIfHaveBoneChild(Bone* cbone);
};

//アニメータークラス
class Animation
{
public:
	void SetName(std::string name);
	std::string GetName();
	void SetTickRate(double rate);
	double GetTickRate();
	void AddChannel(AnimChannel* channel);
	bool HasChannel(std::string name);
	AnimChannel& GetChannel(std::string name);
	void SetDuration(double ticks);
	double GetDuration();
	std::vector<aiNodeAnim* >aichannels;
private:
	std::string name;//名前
	double tick_rate;//チック割合
	double duration;//間隔
	std::map<std::string, unsigned int> channels_mapping;//チャンネルマップ
	std::vector<AnimChannel*> channels;//チャンネルズ
	
};

enum AnimationPlayStyle
{
	PlayOnce,
	PlayLoop,
};


enum KeyEventType
{
	STARTBLADECOLLISION,
	CLOSEBLADECOLLISION,
	STARTANIMATION,
	ENDANIMATION,
};

class AnimationKeyEvent
{
public:
	unsigned int clipindex;
	float keytime;

	KeyEventType keyet;
};

enum CollsionType
{
	Player,
	Enemy,
	Blade
};

class CollsionObject
{
public:
	CollsionObject();
	CollsionObject(const CollsionObject & co);
	bool debugmeshflag;
	int boneindex = -1;
	DirectX::XMMATRIX oritransform;
	std::vector<Mesh> debugmesh;
	BoundingBox aabb;
	CollsionType ct;
};

//モデルクラス
class Model
{
public:
	bool Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);
	void Update(float delta_time, const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix);
	void Draw(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix);

	//animation function
	void PlayAnimaition(const int num,AnimationPlayStyle aps);
	unsigned int GetCurrentAnimationClipIndex();
	unsigned int GetAnimationClipSize();
	void SetAnimationSpeed(float speed);
	float GetAnimationSpeed();

	//collision
	CollsionObject* GetBladeCollsionObject();
	CollsionObject* GetBodyCollsionObject();

	//mesh
	std::vector<Mesh> GetMesh();

	bool IfHasBone();
	bool m_showskeleton = false;//デバッグ用

	void operator = (const Model& model) {
		this->m_showskeleton = model.m_showskeleton;
		this->device = model.device;
		this->deviceContext = model.deviceContext;
		this->cb_vs_vertexshader = model.cb_vs_vertexshader;
		this->cb_bone_info = model.cb_bone_info;
		this->preanimpos = model.preanimpos;
		this->curanimpos = model.curanimpos;
		memcpy(&this->mBoneNodeInfo, &model.mBoneNodeInfo, sizeof(model.mBoneNodeInfo));
		memcpy(&this->meshes, &model.meshes, sizeof(model.meshes));
		this->directory = model.directory;
		this->bHasBone = model.bHasBone;
		this->mNumBones = model.mNumBones;
		memcpy(&this->rootbone, &model.rootbone, sizeof(model.rootbone));
		this->m_GlobalInverseTransform = model.m_GlobalInverseTransform;
		this->rootbonetrans = model.rootbonetrans;
		memcpy(&this->mBoneMaping, &model.mBoneMaping, sizeof(model.mBoneMaping));
		memcpy(&this->mBoneInfo, &model.mBoneInfo, sizeof(model.mBoneInfo));
		this->animplayrate = model.animplayrate;
		this->curanim = model.curanim;
		this->animtimer = model.animtimer;
		memcpy(&this->mAnimations, &model.mAnimations, sizeof(model.mAnimations));
		this->animationPlayStyle = model.animationPlayStyle;
		this->isFbxModel = model.isFbxModel;
		memcpy(&this->debugBlocks, &model.debugBlocks, sizeof(model.debugBlocks));
		memcpy(&this->bodyco, &model.bodyco, sizeof(model.bodyco));
		memcpy(&this->bladeco, &model.bladeco, sizeof(model.bladeco));
		//memcpy(&this->testco, &model.testco, sizeof(model.testco));
		this->precoreslut = model.precoreslut;
	}

private:
	//D3DX refer
	ID3D11Device * device = nullptr;
	ID3D11DeviceContext * deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader> * cb_vs_vertexshader = nullptr;
	ConstantBuffer<CB_Bone_Info> cb_bone_info;

	//transform
	XMFLOAT3 preanimpos = {0,0,0};
	XMFLOAT3 curanimpos = {0,0,0};
	void UpdateAnimationPosition();

	//node
	std::vector<BoneNode*> mBoneNodeInfo;//ノードアレー

	BoneNode* FindBoneNodeByName(std::string bonename);
	void ProcessNode(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix);

	//meshes
	std::vector<Mesh> meshes;//メッシュアレー

	bool LoadModel_Assimp(const std::string & filePath);
	Mesh ProcessMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix);

	//texture
	std::string directory = "";

	TextureStorageType DetermineTextureStorageType(const aiScene * pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene * pScene);// const aiScene* pScene);
	int GetTextureIndex(aiString* pStr);

	//bone
	bool bHasBone = false;//骨がある？
	unsigned int mNumBones = 0;//インデックス
	Bone* rootbone;//ルートボン
	DirectX::XMMATRIX m_GlobalInverseTransform;//足元マトリクス
	DirectX::XMMATRIX rootbonetrans = DirectX::XMMatrixIdentity();//ルートボンマトリクス
	std::map<std::string, unsigned int> mBoneMaping;//ボンマップ
	std::vector<Bone*> mBoneInfo;//骨アレー
	std::vector<DirectX::XMMATRIX> mworkBone;//現在使ってる骨アレー（未使用

	int FindBoneParentInAllNode(std::string childname);
	void ProcessBone(aiNode * node, const aiScene * pmScene);
	void ProcessRootBone();
	void ReBindBone(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix);

	//animation
	float animplayrate = 1.0f;//再生速度
	unsigned int curanim;//現在再生するアニメーション
	float animtimer = 0.0f;//アニメーション時計
	std::vector<Animation*> mAnimations;//アニメーションアレー
	AnimationPlayStyle animationPlayStyle = AnimationPlayStyle::PlayLoop;

	bool LoadAnimation_Assimp(const std::string & filePath);
	void UpdateAnimation(float dt);
	AnimChannel* FineAnimChannelByName(std::string bonename);
	void ProcessAnimations(const aiScene* ai_scene);
	void BoneTransform(float TimeInSeconds, std::vector<DirectX::XMMATRIX>& Transforms);
	void ReadBoneHeirarchy(float AnimationTime, Bone* bone, const DirectX::XMMATRIX& ParentTransform);
	void CalcInterpolatedRotation(DirectX::XMFLOAT4& Out, float AnimationTime, AnimChannel* channel);
	unsigned int FindRotation(float AnimationTime, AnimChannel* pAnimChannel);
	void CalcInterpolatedScaling(DirectX::XMFLOAT3& Out, float AnimationTime, AnimChannel* channel);
	unsigned int FindScaling(float AnimationTime, AnimChannel* panimchannel);
	void CalcInterpolatedPosition(DirectX::XMFLOAT3& Out, float AnimationTime, AnimChannel* channel);
	unsigned int FindPosition(float AnimationTime, AnimChannel* channel);
	

	//fbx sdk parse bone and model(not use in this project )
	//FBXSDK引数　未使用
	bool isFbxModel = false;
	//std::unique_ptr<FBX_Model>	m_fbxmodel;

	//debug
	std::vector<Mesh> debugBlocks;//デバッグ箱
	//Mesh debugblockmesh;
	
	void DisplayNode(const aiScene * scene);
	unsigned int GetChildNodeCount(aiNode* node, unsigned int nodecount);
	void ProcessDebugNode(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix);
	Mesh ProcessDebugMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix);
	void LoadBoneDebugBlock(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);

	//collision
	CollsionObject* bodyco = nullptr;
	CollsionObject* bladeco = nullptr;
	//CollsionObject* testco = nullptr;
	DirectX::ContainmentType precoreslut;
	void ProcessCollsion(aiNode * node, const aiScene * pmScene);
	void UpdateCollisionBox(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix);
	//BoundingBox::CreateFromPoints(this->boundingBox, vertices.size(), &(vertices.at(0)), sizeof(XMFLOAT3));

	//tool
	DirectX::XMMATRIX ToMatrix4f(const aiMatrix4x4 *ai_mat);

};


