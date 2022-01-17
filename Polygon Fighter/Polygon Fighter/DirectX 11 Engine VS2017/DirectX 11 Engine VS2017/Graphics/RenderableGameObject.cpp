//=============================================================================
//
// レンダオブジェクト処理 [RenderableGameObject.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "RenderableGameObject.h"

//=============================================================================
// 初期化
//=============================================================================
bool RenderableGameObject::Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!model.Initialize(filePath, device, deviceContext, cb_vs_vertexshader))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);//位置
	this->SetRotation(0.0f, 0.0f, 0.0f);//回転
	this->UpdateMatrix();
	return true;
}

//=============================================================================
//描画
//=============================================================================
void RenderableGameObject::Draw(const XMMATRIX & viewProjectionMatrix)
{
	model.Draw(this->worldMatrix, viewProjectionMatrix);
}

//=============================================================================
// マトリクス更新処理
//=============================================================================
void RenderableGameObject::UpdateMatrix()
{
	this->worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}

//=============================================================================
// 更新処理
//=============================================================================
void RenderableGameObject::Update(float dt, const XMMATRIX & viewProjectionMatrix)
{
	model.Update(dt, this->worldMatrix, viewProjectionMatrix);
}

//=============================================================================
// 骨のフラッグ
//=============================================================================
void RenderableGameObject::SetSkeletonDebugFlag() 
{
	model.m_showskeleton = !model.m_showskeleton;
}

//=============================================================================
// アニメーション再生
//=============================================================================
void RenderableGameObject::PlayAnimation(unsigned int num,AnimationPlayStyle aps) 
{
	model.PlayAnimaition(num, aps);
}

//=============================================================================
// 現在再生するアニメーション
//=============================================================================
unsigned int RenderableGameObject::GetCurrentAnimationClipIndex()
{
	return model.GetCurrentAnimationClipIndex();
}

//=============================================================================
// アニメーション数
//=============================================================================
unsigned int RenderableGameObject::GetAnimationClipSize()
{
	return model.GetAnimationClipSize();
}

//=============================================================================
// アニメーション再生スピード設置
//=============================================================================
void RenderableGameObject::SetAnimationSpeed(float speed)
{
	model.SetAnimationSpeed(speed);
}

//=============================================================================
// アニメーション再生スピード取る
//=============================================================================
float RenderableGameObject::GetAnimationSpeed() {
	return model.GetAnimationSpeed();
}

//=============================================================================
// 剣の当たり判定ボックスを取る
//=============================================================================
CollsionObject* RenderableGameObject::GetBladeCollsionObject() {
	return model.GetBladeCollsionObject();
}

//=============================================================================
// 体の当たり判定ボックスを取る
//=============================================================================
CollsionObject* RenderableGameObject::GetBodyCollsionObject() {
	return model.GetBodyCollsionObject();
}

//=============================================================================
// 骨を見るか
//=============================================================================
void RenderableGameObject::ShowBones()
{
	model.m_showskeleton = !model.m_showskeleton;
}

//=============================================================================
// メッシュ情報
//=============================================================================
std::vector<Mesh> RenderableGameObject::GetMesh() {
	return this->model.GetMesh();
}