#pragma once
#include <cmath>
#include "Vec3.h"
#include "Star.h"
#include "PointLight.h"

class StarLight
{
public:
	StarLight(const StarLight& s)
		:
		StarLight(s.GetProperties(), s.GetGfx(), s.GetStarClassMap())
	{
		vel = s.GetVelocity();
	}
	StarLight(const std::string& properties, Graphics& gfx, StarClassMap& starMap)
		:
		star(properties, gfx, starMap),
		light(star.GetGfx(), star.GetEquatorialPosition().x, star.GetEquatorialPosition().y, star.GetEquatorialPosition().z, star.GetColor().x, star.GetColor().y, star.GetColor().z, 0.2f)
	{
		vel = CalculateVelocity();
	}
	void Draw(DirectX::FXMMATRIX view) const
	{
		light.Bind(star.GetGfx(), view);
		light.Draw(star.GetGfx());
	}
	std::string GetProperties() const
	{
		return star.GetProperties();
	}
	Graphics& GetGfx() const
	{
		return star.GetGfx();
	}
	StarClassMap& GetStarClassMap() const
	{
		return star.GetStarClassMap();
	}
	Vec3 GetVelocity() const
	{
		return vel;
	}
	void Update(float dt)
	{
		light.SetPos(light.GetPos() + GetVelocity() * dt);
	}
private:
	Vec3 CalculateVelocity() const
	{
		const Vec3 eqPos = star.GetEquatorialPosition();
		const Vec3 hrdPos = star.GetHRDPosition();
		return hrdPos - eqPos;
	}
private:
	Star star;
	PointLight light;
	Vec3 vel = { 0.0f, 0.0f, 0.0f };
};
