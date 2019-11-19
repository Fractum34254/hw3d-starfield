#pragma once
#include <string>
#include <unordered_map>
#include "Surface.h"
#include "resource.h"

class StarClassMap
{
public:
	StarClassMap()
		:
		colorMap(IDB_BITMAP1)
	{
		heatMap["O"] = {0.0, 1.0, 50000.0f, 30000.0f };			///Main Sequence
		heatMap["B"] = {1.0, 1.0, 28000.0f, 10000.0f };
		heatMap["A"] = {2.0, 1.0, 9750.0f, 7500.0f };
		heatMap["F"] = {3.0, 1.0, 7350.0f, 6000.0f };
		heatMap["G"] = {4.0, 1.0, 5900.0f, 5000.0f };
		heatMap["K"] = {5.0, 1.0, 4850.0f, 3500.0f };
		heatMap["M"] = {6.0, 1.0, 3350.0f, 2000.0f };
		heatMap["L"] = {7.0, 1.0, 2000.0f, 1300.0f };			///Brown Dwarfes
		heatMap["T"] = {8.0, 1.0, 1300.0f, 600.0f };
		heatMap["Y"] = {9.0, 1.0, 600.0f, 200.0f };
		heatMap["R"] = {4.5, 1.4, 5400.0f, 3500.0f };			///Red Giants
		heatMap["N"] = {5.9, 1.1, 3500.0f, 2000.0f };
		heatMap["S"] = {5.9, 1.2, 3500.0f, 1900.0f };
	}
	float GetTemperature(std::string main_class, float sub_class)
	{
		if (main_class.front() == 'D')
		{
			main_class = main_class.substr(1, 1);
		}
		if (heatMap.find(main_class) == heatMap.end())
		{
			return 0.0f;
		}
		const std::tuple<double, double, float, float> tempBoundaries = heatMap.at(main_class);
		return std::get<2>(tempBoundaries) - sub_class / 10.0f * (std::get<2>(tempBoundaries) - std::get<3>(tempBoundaries));
	}
	double GetHRDXCoordinateNormalized(std::string main_class, float sub_class)
	{
		if (main_class.front() == 'D')
		{
			main_class = main_class.substr(1, 1);
		}
		if (heatMap.find(main_class) == heatMap.end())
		{
			return -1.0f;
		}
		const std::tuple<double, double, float, float> specClass = heatMap.at(main_class);
		return (std::get<0>(specClass) + ((double)sub_class) / 10.0 * std::get<1>(specClass)) / 10.0;
	}
	Vec3 GetColor(float temp)
	{
		if (temp == 0.0f)
		{
			return { 1.0f, 1.0f, 1.0f };
		}
		const unsigned int x = std::max(0u, std::min(static_cast<unsigned int>(747.31603116f * (log(temp) - 6.72978505695f)), colorMap.GetWidth()-1));
		const unsigned int y = 1;
		const Surface::Color color = colorMap.GetSurfacePixel(x, y);
		return { ((float)color.GetR()) / 255.0f, ((float)color.GetG()) / 255.0f, ((float)color.GetB()) / 255.0f };
	}
	Vec3 GetColor(std::string main_class, float sub_class)
	{
		return GetColor(GetTemperature(main_class, sub_class));
	}
private:
	//classchar -> number, width, high, low
	std::unordered_map<std::string, std::tuple<double, double, float, float>> heatMap;
	Surface colorMap;
};
