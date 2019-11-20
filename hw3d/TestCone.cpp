#include "TestCone.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include "Cone.h"

TestCone::TestCone(const TestCone & rhs)
	:
	TestCone(rhs.GetGfx(), rhs.GetSize(), rhs.GetColor())
{}

TestCone::TestCone(Graphics & gfx, float size, Vec3 color)
	:
	TestCone(gfx, size, color.x, color.y, color.z)
{}

TestCone::TestCone(Graphics & gfx, float size, float r, float g, float b)
	:
	gfx(gfx)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Cone::Make();
	model.Transform(dx::XMMatrixScaling(size, size, size));
	const auto geometryTag = "$cone." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "SolidPS.cso"));

	struct PSColorConstant
	{
		dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	} colorConst;
	colorConst.color = { r,g,b };
	AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1u));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void TestCone::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void TestCone::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestCone::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) * 
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
