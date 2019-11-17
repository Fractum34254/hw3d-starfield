#pragma once
#include <string>
#include <tuple>
#include "ChiliMath.h"
#include "PointLight.h"
#include "Graphics.h"
#include "StarClassMap.h"

class Star
{
public:
	enum UnknownProperties
	{
		SubStarclass = 0x1,
		Radius = 0x2,
		Mass = 0x4,
		AppMag = 0x8,
		AbsMag = 0x16,
		Parallax = 0x32
	};
	Star(const Star& s)
		:
		Star(s.GetProperties(), s.GetGfx(), s.GetStarClassMap())
	{}
	Star(const std::string& properties, Graphics& gfx, StarClassMap& starMap)
		:
		gfx(gfx),
		classMap(starMap)
	{
		fullProperties = properties;
		std::string information = "";
		int fillCount = 0;
		for (size_t i = 0; i < properties.size(); i++)
		{
			if (properties.at(i) == fill)
			{
				switch (fillCount)
				{
				case 0:
					name = information;
					break;
				case 1:
					system = information;
					break;
				case 2:
					type = information;
					break;
				case 3:
					main_class = information;
					break;
				case 4:
					sub_class = information;
					if (information == "")
					{
						unknown += UnknownProperties::SubStarclass;
						sub_class = "5.0";
					}
					break;
				case 5:
					RAHours = information;
					break;
				case 6:
					RAMinutes = information;
					break;
				case 7:
					RASeconds = information;
					break;
				case 8:
					DSign = information;
					break;
				case 9:
					DDegrees = information;
					break;
				case 10:
					DMinutes = information;
					break;
				case 11:
					DSeconds = information;
					break;
				case 12:
					constellation = information;
					break;
				case 13:
					distance = information;
					break;
				case 14:
					parallax = information;
					if (information == "")
					{
						unknown += UnknownProperties::Parallax;
					}
					break;
				case 15:
					appMag = information;
					if (information == "")
					{
						unknown += UnknownProperties::AppMag;
					}
					break;
				case 16:
					absMag = information;
					if (information == "")
					{
						unknown += UnknownProperties::AbsMag;
					}
					break;
				case 17:
					mass = information;
					if (information == "")
					{
						unknown += UnknownProperties::Mass;
					}
					break;
				}
				information = "";
				fillCount++;
			}
			else
			{
				information.push_back(properties.at(i));
			}
		}
		radius = information;
		if (information == "")
		{
			unknown += UnknownProperties::Radius;
		}
		pos = CalculateEquatorialPosition();
		temperature = classMap.GetTemperature(main_class, std::stof(sub_class));
		color = classMap.GetColor(temperature);
	}
	float GetRightAscension() const ///in radians
	{
		return (std::stof(RAHours) + std::stof(RAMinutes) / 60.0f + std::stof(RASeconds) / 60.0f / 60.0f) / 12.0f * PI;
	}
	float GetDeclination() const ///in radians
	{
		return (std::stof(DDegrees) + std::stof(DMinutes) / 60.0f + std::stof(DSeconds) / 60.0f / 60.0f) * ((DSign == "-") ? (-1.0f) : 1.0f) / 180.0f * PI;
	}
	float GetDistance() const ///in lightyears
	{
		return std::stof(distance);
	}
	std::tuple<float, float, float> GetEquatorialPosition() const
	{
		return pos;
	}
	std::tuple<float, float, float> GetColor() const
	{
		return color;
	}
	Graphics& GetGfx() const
	{
		return gfx;
	}
	std::string GetProperties() const
	{
		return fullProperties;
	}
	StarClassMap& GetStarClassMap() const
	{
		return classMap;
	}
private:
	std::tuple<float, float, float> CalculateEquatorialPosition() const
	{
		///on unit circle
		const float dec = GetDeclination();
		const float ra = GetRightAscension();
		const float cosDec = cos(dec);
		float y = sin(dec);
		float z = cosDec * cos(ra);
		float x = -sin(ra) * cosDec;
		///scale all up by distance
		const float dist = GetDistance();
		x *= dist;
		y *= dist;
		z *= dist;
		return std::make_tuple(x, y, z);
	}
	std::tuple<float, float, float> CalculateHRDPosition() const
	{

	}
private:
	Graphics& gfx;
	StarClassMap& classMap;
	static constexpr char fill = '#';
	char unknown = 0;
	std::string fullProperties;
	std::string name;
	std::string system;
	std::string type;
	std::string main_class;
	std::string sub_class;
	std::string RAHours;
	std::string RAMinutes;
	std::string RASeconds;
	std::string DSign;
	std::string DDegrees;
	std::string DMinutes;
	std::string DSeconds;
	std::string constellation;
	std::string distance;
	std::string parallax;
	std::string appMag;
	std::string absMag;
	std::string mass;
	std::string radius;
	std::tuple<float, float, float> pos;
	std::tuple<float, float, float> color;
	float temperature;
};
