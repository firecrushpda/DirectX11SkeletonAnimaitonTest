//=============================================================================
//
// レンダゲームオブジェクト [RenderableGameObject.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "GameObject3D.h"

class RenderableGameObject : public GameObject3D
{
public:
	void operator = (const RenderableGameObject& gobject) {
		this->model = gobject.model;
		//memcpy(&this->model, &gobject.model, sizeof(Model));
		this->pos = gobject.pos;
		this->posVector = gobject.posVector;
		this->rot = gobject.rot;
		this->rotVector = gobject.rotVector;
		this->scale = gobject.scale;
		this->vec_backward = gobject.vec_backward;
		this->vec_backward_noY = gobject.vec_backward_noY;
		this->vec_forward = gobject.vec_forward;
		this->vec_forward_noY = gobject.vec_forward_noY;
		this->vec_left = gobject.vec_left;
		this->vec_left_noY = gobject.vec_left_noY;
		this->vec_right = gobject.vec_right;
		this->vec_right_noY = gobject.vec_right_noY;
		this->worldMatrix = gobject.worldMatrix;
	}


	bool Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);

	void Draw(const DirectX::XMMATRIX & viewProjectionMatrix);

	void Update(float dt, const XMMATRIX & viewProjectionMatrix);

	void SetSkeletonDebugFlag();

	void PlayAnimation(unsigned int num, AnimationPlayStyle aps);

	unsigned int GetCurrentAnimationClipIndex();

	unsigned int GetAnimationClipSize();

	void SetAnimationSpeed(float speed);

	float GetAnimationSpeed();

	std::vector<Mesh> GetMesh();

	CollsionObject* GetBladeCollsionObject();

	CollsionObject* GetBodyCollsionObject();

	void ShowBones();

protected:
	Model model;//モデル

	void UpdateMatrix() override;

	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();//位置
};