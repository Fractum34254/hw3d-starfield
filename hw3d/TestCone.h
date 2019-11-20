#pragma once
#include "Drawable.h"
#include "Vec3.h"

class TestCone : public Drawable
{
public:
	TestCone(const TestCone& rhs);
	TestCone(Graphics& gfx, float size, Vec3 color);
	TestCone(Graphics& gfx, float size, float r, float g, float b);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	Graphics& GetGfx() const
	{
		return gfx;
	}
	float GetSize() const
	{
		return size;
	}
	Vec3 GetColor() const
	{
		return color;
	}
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
	Graphics& gfx;
	float size;
	Vec3 color;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
};