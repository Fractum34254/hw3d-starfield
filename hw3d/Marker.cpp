#include "Marker.h"
#include "ChiliMath.h"

Marker::Marker(Graphics& gfx, float radius, float sphereSize, Vec3 color, Vec3 pos)
	:
	radius(radius),
	color(color),
	pos(pos)
{
	for (int i = 0; i < 6; i++)
	{
		lights.emplace_back(PointLight(gfx, pos.x, pos.y, pos.z, color.x, color.y, color.z, sphereSize));
	}

	//top
	lights.at(0).SetPos((pos + Vec3(0.0f, radius, 0.0f)).GetXMFloat3());
	//bottom
	lights.at(1).SetPos((pos - Vec3(0.0f, radius, 0.0f)).GetXMFloat3());
	//front
	lights.at(2).SetPos((pos - Vec3(radius, 0.0f, 0.0f)).GetXMFloat3());
	//back
	lights.at(3).SetPos((pos + Vec3(radius, 0.0f, 0.0f)).GetXMFloat3());
	//right side
	lights.at(4).SetPos((pos + Vec3(0.0f, 0.0f, radius)).GetXMFloat3());
	//left side
	lights.at(5).SetPos((pos - Vec3(0.0f, 0.0f, radius)).GetXMFloat3());
}

void Marker::Draw(Graphics& gfx) const
{
	for (const PointLight& p : lights)
	{
		p.Draw(gfx);
	}
}

void Marker::SetPos(Vec3 newPos)
{
	pos = newPos;
	//top
	lights.at(0).SetPos((pos + Vec3(0.0f, radius, 0.0f)).GetXMFloat3());
	//bottom
	lights.at(1).SetPos((pos - Vec3(0.0f, radius, 0.0f)).GetXMFloat3());
	//front
	lights.at(2).SetPos((pos - Vec3(radius, 0.0f, 0.0f)).GetXMFloat3());
	//back
	lights.at(3).SetPos((pos + Vec3(radius, 0.0f, 0.0f)).GetXMFloat3());
	//right side
	lights.at(4).SetPos((pos + Vec3(0.0f, 0.0f, radius)).GetXMFloat3());
	//left side
	lights.at(5).SetPos((pos - Vec3(0.0f, 0.0f, radius)).GetXMFloat3());
}
