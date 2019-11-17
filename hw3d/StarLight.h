#pragma once
#include "Star.h"
#include "PointLight.h"

class StarLight
{
public:
	StarLight(const StarLight& s)
		:
		StarLight(s.GetProperties(), s.GetGfx(), s.GetStarClassMap())
	{}
	StarLight(const std::string& properties, Graphics& gfx, StarClassMap& starMap)
		:
		star(properties, gfx, starMap),
		light(star.GetGfx(), std::get<0>(star.GetEquatorialPosition()), std::get<1>(star.GetEquatorialPosition()), std::get<2>(star.GetEquatorialPosition()), std::get<0>(star.GetColor()), std::get<1>(star.GetColor()), std::get<2>(star.GetColor()), 0.2f)
	{}
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
private:
	Star star;
	PointLight light;
};
