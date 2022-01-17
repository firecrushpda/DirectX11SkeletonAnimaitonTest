//=============================================================================
//
// ゲームオブジェクト2Dクラス [GameObject2D.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "GameObject.h"

class GameObject3D : public GameObject
{
public:
	float hp = 100;

	void SetLookAtPos(DirectX::XMFLOAT3 lookAtPos);
	const DirectX::XMVECTOR & GetForwardVector(bool omitY = false);
	const DirectX::XMVECTOR & GetRightVector(bool omitY = false);
	const DirectX::XMVECTOR & GetBackwardVector(bool omitY = false);
	const DirectX::XMVECTOR & GetLeftVector(bool omitY = false);

protected:
	virtual void UpdateMatrix();

	void UpdateDirectionVectors();

	//ディフォルト
	const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	DirectX::XMVECTOR vec_forward;//前向き
	DirectX::XMVECTOR vec_left;//左辺
	DirectX::XMVECTOR vec_right;//右辺
	DirectX::XMVECTOR vec_backward;//後ろ

	DirectX::XMVECTOR vec_forward_noY;//前向きY軸抜き
	DirectX::XMVECTOR vec_left_noY;//左辺Y軸抜き
	DirectX::XMVECTOR vec_right_noY;//右辺Y軸抜き
	DirectX::XMVECTOR vec_backward_noY;//後ろY軸抜き
};