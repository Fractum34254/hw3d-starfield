#pragma once
#include <tuple>
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
		heatMap["O"] = { 50000.0f, 30000.0f };			///Main Sequence
		heatMap["B"] = { 28000.0f, 10000.0f };
		heatMap["A"] = { 9750.0f, 7500.0f };
		heatMap["F"] = { 7350.0f, 6000.0f };
		heatMap["G"] = { 5900.0f, 5000.0f };
		heatMap["K"] = { 4850.0f, 3500.0f };
		heatMap["M"] = { 3350.0f, 2000.0f };
		heatMap["L"] = { 2000.0f, 1300.0f };			///Brown Dwarfes
		heatMap["T"] = { 1300.0f, 600.0f };
		heatMap["Y"] = { 600.0f, 200.0f };
		heatMap["R"] = { 5400.0f, 3500.0f };			///Red Giants
		heatMap["N"] = { 3500.0f, 2000.0f };
		heatMap["S"] = { 3500.0f, 1900.0f };
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
		const std::pair<float, float> tempBoundaries = heatMap.at(main_class);
		return tempBoundaries.first - sub_class / 10.0f * (tempBoundaries.first - tempBoundaries.second);
	}
	std::tuple<float, float, float> GetColor(float temp)
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
	std::tuple<float, float, float> GetColor(std::string main_class, float sub_class)
	{
		return GetColor(GetTemperature(main_class, sub_class));
	}
private:
	std::unordered_map<std::string, std::pair<float, float>> heatMap;
	Surface colorMap;
};
