#pragma once
#include <cmath>
#include "Vec3.h"
#include "Star.h"
#include "PointLight.h"
#include "imgui/imgui.h"

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
	void SetPosToEq()
	{
		light.SetPos(star.GetEquatorialPosition());
	}
	void SetPosToHRD()
	{
		light.SetPos(star.GetHRDPosition());
	}
	Vec3 GetPos() const
	{
		return light.GetPos();
	}
	void SpawnInfoWindow()
	{
		if (ImGui::Begin(star.GetName().c_str()))
		{
			ImGui::SetWindowFontScale(1.2f);
			std::string info = "Name: " + star.GetName();
			ImGui::Text(info.c_str());
			info = "System: " + star.GetSystem();
			ImGui::Text(info.c_str());
			info = "Typ: " + star.GetType();
			ImGui::Text(info.c_str());
			if (star.GetUnknown() & Star::UnknownProperties::SubStarclass)
			{
				info = "Klasse: " + star.GetMainClass();
				ImGui::Text(info.c_str());
			}
			else
			{
				info = "Klasse: " + star.GetFullClass();
				ImGui::Text(info.c_str());
			}
			info = "Rektaszension: " + star.GetFullRightAscension();
			ImGui::Text(info.c_str());
			info = "Deklination: " + star.GetFullDeclination();
			ImGui::Text(info.c_str());
			info = "Entfernung: " + star.GetDistanceString();
			ImGui::Text(info.c_str());
			if (!(star.GetUnknown() & Star::UnknownProperties::Parallax))
			{
				info = "Parallaxe: " + star.GetParallaxString();
				ImGui::Text(info.c_str());
			}
			if (!(star.GetUnknown() & Star::UnknownProperties::AppMag))
			{
				info = "Relative Helligkeit: " + star.GetAppMagString();
				ImGui::Text(info.c_str());
			}
			if (!(star.GetUnknown() & Star::UnknownProperties::AbsMag))
			{
				info = "Absolute Helligkeit: " + star.GetAbsMagString();
				ImGui::Text(info.c_str());
			}
			info = "Sternbild: " + star.GetConstellation();
			ImGui::Text(info.c_str());
			if (!(star.GetUnknown() & Star::UnknownProperties::Mass))
			{
				info = "Masse: " + star.GetMass();
				ImGui::Text(info.c_str());
			}
			if (!(star.GetUnknown() & Star::UnknownProperties::Radius))
			{
				info = "Radius: " + star.GetRadius();
				ImGui::Text(info.c_str());
			}
		}
		ImGui::End();
	}
	std::string GetName() const
	{
		return star.GetName();
	}
	void BindLight(Graphics& gfx, DirectX::FXMMATRIX view)
	{
		light.Bind(gfx, view);
	}
	void SetAmbient(float perc)
	{
		light.SetAmbient(perc);
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
