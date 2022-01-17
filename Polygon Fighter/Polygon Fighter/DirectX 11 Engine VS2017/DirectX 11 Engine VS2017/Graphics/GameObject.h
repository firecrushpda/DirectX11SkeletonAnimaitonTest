//=============================================================================
//
// ゲームオブジェクトクラス [GameObject.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "Model.h"

class GameObject
{
public:
	//位置ゲート
	const DirectX::XMVECTOR & GetPositionVector() const;
	const DirectX::XMFLOAT3 & GetPositionFloat3() const;
	const DirectX::XMVECTOR & GetRotationVector() const;
	const DirectX::XMFLOAT3 & GetRotationFloat3() const;

	//位置セット
	void SetPosition(const DirectX::XMVECTOR & pos);
	void SetPosition(const DirectX::XMFLOAT3 & pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const DirectX::XMVECTOR & pos);
	void AdjustPosition(const DirectX::XMFLOAT3 & pos);
	void AdjustPosition(float x, float y, float z);
	void SetRotation(const DirectX::XMVECTOR & rot);
	void SetRotation(const DirectX::XMFLOAT3 & rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const DirectX::XMVECTOR & rot);
	void AdjustRotation(const DirectX::XMFLOAT3 & rot);
	void AdjustRotation(float x, float y, float z);
	void SetScale(float xScale, float yScale, float zScale = 1.0f);

protected:
	virtual void UpdateMatrix();

	DirectX::XMVECTOR posVector;//計算用ポジション
	DirectX::XMVECTOR rotVector;//計算用ローテーション
	DirectX::XMFLOAT3 pos;//ポジション
	DirectX::XMFLOAT3 rot;//ローテーション

	DirectX::XMFLOAT3 scale;//スケール

	
};