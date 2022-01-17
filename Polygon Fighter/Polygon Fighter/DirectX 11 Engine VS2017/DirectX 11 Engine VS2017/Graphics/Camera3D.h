//=============================================================================
//
// 3Dカメラ処理 [Camera3D.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "GameObject3D.h"
using namespace DirectX;

class Camera3D : public GameObject3D
{
public:
	Camera3D();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void ChangeFocusMode(unsigned int index, GameObject3D* go);
	void ResetFollowCamera();

	const XMMATRIX & GetViewMatrix() const;
	const XMMATRIX & GetProjectionMatrix() const;

	float focusLength = 50;
	unsigned int cameratype = 0;
	GameObject3D* focusgo;
private:
	void UpdateMatrix() override;
	
	XMMATRIX viewMatrix;//ビューマトリクス
	XMMATRIX projectionMatrix;//プロジェクトマトリクス
};
