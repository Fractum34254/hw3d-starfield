#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "TestPlane.h"
#include "Mesh.h"
#include "StarLight.h"
#include "StarClassMap.h"
#include "Marker.h"
#include <set>
#include <fstream>

class App
{
public:
	App( const std::string& commandLine = "" );
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame();
	void ShowImguiDemoWindow();
private:
	static constexpr int windowSizeX = 1600;
	static constexpr int windowSizeY = 900;
	float totalTime = 0.0f;
	std::string commandLine;
	bool showDemoWindow = false;
	float dir = 0.0f; //0: no movement	1: to hrd	-1: to eq
	static constexpr float hrdTime = 4.0f;
	static constexpr float eqTime = 0.0f;
	float actTime = 0.0f;
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer;
	float speed_factor = 1.0f;
	Camera cam;
	StarClassMap starclassmap;
	std::vector<StarLight> starLights;
	SolidSphere trashbin;
	Marker marker;
	size_t currentStar = 0;
	TestPlane hrd_grid;
};