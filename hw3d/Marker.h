#pragma once
#include <vector>
#include "Vec3.h"
#include "PointLight.h"

class Marker
{
public:
	Marker(Graphics& gfx, float radius, float sphereSize, Vec3 color, Vec3 pos);
	void Draw(Graphics& gfx) const;
	void SetPos(Vec3 newPos);
private:
	std::vector<PointLight> lights;
	Vec3 pos;
	Vec3 color;
	float radius;
};
