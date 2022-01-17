//=============================================================================
//
// 3Dカメラ処理 [Camera3D.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "Camera3D.h"

//=============================================================================
//初期化
//=============================================================================
Camera3D::Camera3D()
{
	this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->posVector = XMLoadFloat3(&this->pos);
	this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateMatrix();
}

//=============================================================================
// プロジェクションマトリクス設置関数
//=============================================================================
void Camera3D::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

//=============================================================================
// ビューマトリクス取る関数
//=============================================================================
const XMMATRIX & Camera3D::GetViewMatrix() const
{
	return this->viewMatrix;
}

//=============================================================================
// プロジェクションマトリクス取る関数
//=============================================================================
const XMMATRIX & Camera3D::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

//=============================================================================
// マトリクス更新関数
//=============================================================================
void Camera3D::UpdateMatrix() //Updates view matrix and also updates the movement vectors
{
	//Calculate Camera3D rotation matrix
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	//Calculate unit vector of cam target based off Camera3D forward value transformed by cam rotation matrix
	XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	//Adjust cam target to be offset by the Camera3D's current position
	camTarget += this->posVector;
	//Calculate up direction based on current rotation
	XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
	//Rebuild view matrix
	this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);

	this->UpdateDirectionVectors();
}

void Camera3D::ChangeFocusMode(unsigned int index,GameObject3D* go) 
{
	cameratype = index;
	if (go != nullptr) focusgo = go;

	if (index == 1)
	{
		ResetFollowCamera();
	}
}

void Camera3D::ResetFollowCamera() {
	if (focusgo == nullptr)return;

	pos = { 0,0,0 };
	rot = { 0,0,0 };
	//this->SetLookAtPos(focusgo->GetPositionFloat3());
	auto tgpos = focusgo->GetPositionFloat3();
	pos.x = tgpos.x - sinf(rot.y) * focusLength;
	pos.z = tgpos.z - cosf(rot.y) * focusLength;
	//pos.z = tgpos.z - cosf(rot.y) * focusLength;
}