#pragma once
#include <optional>
#include "Vertex.h"
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "ChiliMath.h"

class Cone
{
public:
	static IndexedTriangleList MakeTesselated(Dvtx::VertexLayout layout, int longDiv)
	{
		namespace dx = DirectX;
		assert(longDiv >= 3);

		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI / longDiv;

		// base vertices
		Dvtx::VertexBuffer vb{ std::move(layout) };
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			dx::XMFLOAT3 calculatedPos;
			auto v = dx::XMVector3Transform(
				base,
				dx::XMMatrixRotationZ(longitudeAngle * iLong)
			);
			dx::XMStoreFloat3(&calculatedPos, v);
			vb.EmplaceBack(calculatedPos);
		}
		// the center
		dx::XMFLOAT3 center;
		dx::XMStoreFloat3(&center, { 0.0f,0.0f,-1.0f });
		vb.EmplaceBack(center);
		const auto iCenter = (unsigned short)(vb.Size() - 1);
		// the tip :darkness:
		dx::XMFLOAT3 tip;
		dx::XMStoreFloat3(&tip, { 0.0f,0.0f,1.0f });
		vb.EmplaceBack(tip);
		const auto iTip = (unsigned short)(vb.Size() - 1);


		// base indices
		std::vector<unsigned short> indices;
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iCenter);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iLong);
		}

		// cone indices
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iLong);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iTip);
		}

		return { std::move(vb),std::move(indices) };
	}
	
	static IndexedTriangleList Make(std::optional<Dvtx::VertexLayout> layout = std::nullopt)
	{
		using Element = Dvtx::VertexLayout::ElementType;
		if (!layout)
		{
			layout = Dvtx::VertexLayout{}.Append(Element::Position3D);
		}
		return MakeTesselated(std::move(*layout), 24);
	}
};
