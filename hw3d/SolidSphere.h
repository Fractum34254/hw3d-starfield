#pragma once
#include "Drawable.h"

class SolidSphere : public Drawable
{
public:
	SolidSphere( Graphics& gfx,float radius, float r, float g, float b);
	void SetPos( DirectX::XMFLOAT3 pos ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
};