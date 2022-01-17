//=============================================================================
//
// 2Dカメラ処理 [Camera2D.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "Camera2D.h"

//=============================================================================
// モデル初期化関数
//=============================================================================
Camera2D::Camera2D()
{
	pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	posVector = XMLoadFloat3(&pos);
	rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotVector = XMLoadFloat3(&rot);
	UpdateMatrix();
}

//=============================================================================
// プロジェクションマトリクス設置関数
//=============================================================================
void Camera2D::SetProjectionValues(float width, float height, float nearZ, float farZ)
{
	orthoMatrix = XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, nearZ, farZ);
}

//=============================================================================
//オルソマトリクス取る関数
//=============================================================================
const XMMATRIX & Camera2D::GetOrthoMatrix() const
{
	return orthoMatrix;
}

//=============================================================================
//ワールドマトリクス取る関数
//=============================================================================
const XMMATRIX & Camera2D::GetWorldMatrix() const
{
	return worldMatrix;
}

//=============================================================================
//マトリクス更新関数
//=============================================================================
void Camera2D::UpdateMatrix() //Updates view matrix and also updates the movement vectors
{
	XMMATRIX translationOffsetMatrix = XMMatrixTranslation(-pos.x, -pos.y, 0.0f); //z component irrelevant for 2d camera
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	worldMatrix = camRotationMatrix * translationOffsetMatrix;
}