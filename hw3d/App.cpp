#include "App.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include "TexturePreprocessor.h"
#include <shellapi.h>

namespace dx = DirectX;

GDIPlusManager gdipm;

void LoadFile(const std::string& name, std::vector<std::string>& vector)
{
	std::fstream file(name);

	for (std::string line; std::getline(file, line);)
	{
		if (!line.empty())
		{
			vector.push_back(line);
		}
	}
}

size_t CheckForSelection(float xMouse, float yMouse, Vec3 camPos, Graphics& gfx, float windowSizeX, float windowSizeY, std::vector<StarLight>& vector)
{
	const DirectX::XMFLOAT3 pixel = { xMouse, yMouse, 0.0f };
	const DirectX::XMVECTOR worldSpace = DirectX::XMVector3Unproject(DirectX::XMLoadFloat3(&pixel), 0.0f, 0.0f, windowSizeX, windowSizeY, 0.0f, 1.0f, gfx.GetProjection(), gfx.GetCamera(), DirectX::XMMatrixIdentity());
	DirectX::XMFLOAT3 worldSpaceFloat;
	DirectX::XMStoreFloat3(&worldSpaceFloat, worldSpace);

	const Vec3 dir = (Vec3( worldSpaceFloat.x, worldSpaceFloat.y, worldSpaceFloat.z ) - camPos).GetNormalized() / 2.0f;


	std::vector<std::tuple<const StarLight*, float, size_t>> stars;
	for (size_t n = 0; n < vector.size(); n++)
	{
		stars.push_back({ &vector.at(n), (vector.at(n).GetPos() - (camPos + dir)).LenSq(), n});
	}

	for (size_t i = 2; stars.size() > 0; i++)
	{
		for (size_t k = 0; k < stars.size(); k++)
		{
			const float ls = (std::get<0>(stars.at(k))->GetPos() - (camPos + dir * (float)i)).LenSq();
			if (ls <= 0.25f)
			{
				return std::get<2>(stars.at(k));
			}
			else if(ls > std::get<1>(stars.at(k)))
			{
				stars.erase(stars.begin() + k);
				k--;
			}
			else
			{
				std::get<1>(stars.at(k)) = ls;
			}
		}
	}
	//no stars clicked
	return vector.size();
}

App::App( const std::string& commandLine )
	:
	commandLine( commandLine ),
	wnd( windowSizeX,windowSizeY,"Starfield" ),
	trashbin(wnd.Gfx(), 2.0f, 0.0f, 0.0f, 0.0f),
	marker(wnd.Gfx(), 0.3f, 0.05f, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }),
	hrd_grid(wnd.Gfx(), 65.0f, IDB_BITMAP2),
	hrd_no_grid(wnd.Gfx(), 65.0f, IDB_BITMAP3)
{
	// makeshift cli for doing some preprocessing bullshit (so many hacks here)
	if( this->commandLine != "" )
	{
		int nArgs;
		const auto pLineW = GetCommandLineW();
		const auto pArgs = CommandLineToArgvW( pLineW,&nArgs );
		if( nArgs >= 3 && std::wstring(pArgs[1]) == L"--twerk-objnorm" )
		{
			const std::wstring pathInWide = pArgs[2];
			TexturePreprocessor::FlipYAllNormalMapsInObj(
				std::string( pathInWide.begin(),pathInWide.end() )
			);
			throw std::runtime_error( "Normal maps all processed successfully. Just kidding about that whole runtime error thing." );
		}
		else if( nArgs >= 3 && std::wstring( pArgs[1] ) == L"--twerk-flipy" )
		{
			const std::wstring pathInWide = pArgs[2];
			const std::wstring pathOutWide = pArgs[3];
			TexturePreprocessor::FlipYNormalMap(
				std::string( pathInWide.begin(),pathInWide.end() ),
				std::string( pathOutWide.begin(),pathOutWide.end() )
			);
			throw std::runtime_error( "Normal map processed successfully. Just kidding about that whole runtime error thing." );
		}
		else if( nArgs >= 4 && std::wstring( pArgs[1] ) == L"--twerk-validate" )
		{
			const std::wstring minWide = pArgs[2];
			const std::wstring maxWide = pArgs[3];
			const std::wstring pathWide = pArgs[4];
			TexturePreprocessor::ValidateNormalMap(
				std::string( pathWide.begin(),pathWide.end() ),std::stof( minWide ),std::stof( maxWide )
			);
			throw std::runtime_error( "Normal map validated successfully. Just kidding about that whole runtime error thing." );
		}
	}

	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,9.0f / 16.0f,0.5f,400.0f ) );

	std::vector<std::string> fileLines;
	LoadFile("SternenListeOutput.starlist", fileLines);

	for (std::string& s : fileLines)
	{
		s.pop_back();
		starLights.push_back(std::move(StarLight(s, wnd.Gfx(), starclassmap)));

		if (starLights.back().GetName() == "Sonne")
		{
			starLights.back().BindLight(wnd.Gfx(), cam.GetMatrix());
		}
	}

	trashbin.SetPos({0.0f, 0.0f, 0.0f});
	hrd_grid.SetPos(Vec3(5.0f, -5.0f, -3.0f).GetXMFloat3());
	hrd_no_grid.SetPos(Vec3(5.0f, -5.0f, -3.0f).GetXMFloat3());
}

void App::DoFrame()
{
	//Start Frame
	const auto dt = timer.Mark() * speed_factor;
	totalTime += dt;
	wnd.Gfx().BeginFrame( 0.0f,0.0f,0.0f ); ///was: 0.07f, 0.0f, 0.12f
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	
	//Keyboard input
	while (const auto e = wnd.kbd.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_F1:
			showDemoWindow = true;
			break;
		case VK_RETURN:
			if (dir == 0.0f)
			{
				if (actTime <= eqTime)
				{
					dir = 1.0f;
				}
				else
				{
					dir = -1.0f;
				}
			}
			else
			{
				dir *= (-1);
			}
			break;
		}
	}
	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed('W'))
		{
			cam.Translate({ 0.0f,0.0f,dt });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cam.Translate({ -dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cam.Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cam.Translate({ dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed(VK_SPACE))
		{
			cam.TranslateWorldSpace({ 0.0f,dt,0.0f });
		}
		if (wnd.kbd.KeyIsPressed(VK_CONTROL))
		{
			cam.TranslateWorldSpace({ 0.0f,-dt,0.0f });
		}
	}

	//Mouse input
	if (wnd.mouse.LeftIsPressed() && wnd.CursorEnabled())
	{
		size_t temp = CheckForSelection((float)wnd.mouse.GetPosX(), (float)wnd.mouse.GetPosY(), cam.GetPos(), wnd.Gfx(), (float)windowSizeX, (float)windowSizeY, starLights);
		if (temp != starLights.size())
		{
			currentStar = temp;
		}
	}
	
	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cam.Rotate((float)delta->x, (float)delta->y);
		}
	}

	//Update all objects
	///Marker
	marker.SetPos(starLights.at(currentStar).GetPos().GetXMFloat3());
	
	///Star positions
	const float timeStep = dt * dir * 1.0f / hrdTime;

	for (StarLight& s : starLights)
	{
		s.Update(timeStep);
	}
	actTime += timeStep * hrdTime;
	
	if (actTime < eqTime)
	{
		dir = 0.0f;
		actTime = eqTime;
		for (StarLight& s : starLights)
		{
			s.SetPosToEq();
		}
	}
	else if (actTime > hrdTime)
	{
		dir = 0.0f;
		actTime = hrdTime;
		for (StarLight& s : starLights)
		{
			s.SetPosToHRD();
		}
	}

	//draw everything
	if (!(dir < 0.0f) && (actTime > eqTime))
	{
		trashbin.Draw(wnd.Gfx());
		if (grid)
		{
			hrd_grid.Draw(wnd.Gfx());
		}
		else
		{
			hrd_no_grid.Draw(wnd.Gfx());
		}
	}
	for (const StarLight& s : starLights)
	{
		s.Draw(cam.GetMatrix());
	}
	marker.Draw(wnd.Gfx());
		
	// imgui windows
	if (ImGui::Begin("Grid"))
	{
		ImGui::Text("Grid enabled?");
		ImGui::Checkbox("Enabled", &grid);
	}
	ImGui::End();
	cam.SpawnControlWindow();
	starLights.at(currentStar).SpawnInfoWindow();
	ShowImguiDemoWindow();

	// present
	wnd.Gfx().EndFrame();
}

void App::ShowImguiDemoWindow()
{
	if( showDemoWindow )
	{
		ImGui::ShowDemoWindow( &showDemoWindow );
	}
}

App::~App()
{}


int App::Go()
{
	while( true )
	{
		// process all messages pending, but to not block for new messages
		if( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}