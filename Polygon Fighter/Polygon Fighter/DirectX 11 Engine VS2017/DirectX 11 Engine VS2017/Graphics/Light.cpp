//=============================================================================
//
// 3Dゲームオブジェクト [GameObject3D.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "Light.h"

//=============================================================================
// 初期化
//=============================================================================
bool Light::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!model.Initialize("Data/Objects/light.fbx", device, deviceContext, cb_vs_vertexshader))//
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}