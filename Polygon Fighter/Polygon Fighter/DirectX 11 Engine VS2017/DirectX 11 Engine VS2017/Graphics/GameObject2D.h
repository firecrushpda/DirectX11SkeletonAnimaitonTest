//=============================================================================
//
// ゲームオブジェクト2Dクラス [GameObject2D.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "GameObject.h"

class GameObject2D : public GameObject
{
protected:
	virtual void UpdateMatrix();
};