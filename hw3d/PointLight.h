#pragma once
#include "Vec3.h"
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"
#include "ConditionalNoexcept.h"

class PointLight
{
public:
	PointLight(const PointLight& rhs);
	PointLight( Graphics& gfx, float x, float y, float z, float r, float g, float b, float radius = 0.5f );
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Draw( Graphics& gfx ) const noxnd;
	void Bind( Graphics& gfx,DirectX::FXMMATRIX view ) const noexcept;
	void SetPos(float x_in, float y_in, float z_in) noexcept;
	void SetAmbient(float percentage) noexcept;
	void SetPos(Vec3 posXYZ) noexcept;
	Vec3 GetPos() const noexcept;
	Vec3 GetColor() const noexcept;
	float GetRadius() const noexcept;
	Graphics& GetGfx() const noexcept;
private:
	struct PointLightCBuf
	{
		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};
private:
	float x;
	float y;
	float z;
	Vec3 color;
	float radius;
	Graphics& gfx;
	PointLightCBuf cbData;
	mutable SolidSphere mesh;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> cbuf;
};