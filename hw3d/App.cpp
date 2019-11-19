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

App::App( const std::string& commandLine )
	:
	commandLine( commandLine ),
	wnd( 1600,900,"Starfield" ),
	trashbin(wnd.Gfx(), 2.0f, 0.0f, 0.0f, 0.0f)
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
	}

	trashbin.SetPos({0.0f, 0.0f, 0.0f});
}

void App::DoFrame()
{
	const auto dt = timer.Mark() * speed_factor;
	totalTime += dt;
	wnd.Gfx().BeginFrame( 0.0f,0.0f,0.0f ); ///was: 0.07f, 0.0f, 0.12f
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	
	for (const StarLight& s : starLights)
	{
		s.Draw(cam.GetMatrix());
	}

	while( const auto e = wnd.kbd.ReadKey() )
	{
		if( !e->IsPress() )
		{
			continue;
		}

		switch( e->GetCode() )
		{
		case VK_ESCAPE:
			if( wnd.CursorEnabled() )
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

	const float timeStep = dt * dir * 1.0f / hrdTime;

	for (StarLight& s : starLights)
	{
		s.Update(timeStep);
	}
	actTime += timeStep * hrdTime;

	if (!(dir < 0.0f) && (actTime > eqTime))
	{
		trashbin.Draw(wnd.Gfx());
	}

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

	if( !wnd.CursorEnabled() )
	{
		if( wnd.kbd.KeyIsPressed( 'W' ) )
		{
			cam.Translate( { 0.0f,0.0f,dt } );
		}
		if( wnd.kbd.KeyIsPressed( 'A' ) )
		{
			cam.Translate( { -dt,0.0f,0.0f } );
		}
		if( wnd.kbd.KeyIsPressed( 'S' ) )
		{
			cam.Translate( { 0.0f,0.0f,-dt } );
		}
		if( wnd.kbd.KeyIsPressed( 'D' ) )
		{
			cam.Translate( { dt,0.0f,0.0f } );
		}
		if( wnd.kbd.KeyIsPressed( VK_SPACE ) )
		{
			cam.TranslateWorldSpace( { 0.0f,dt,0.0f } );
		}
		if( wnd.kbd.KeyIsPressed( VK_CONTROL ) )
		{
			cam.TranslateWorldSpace( { 0.0f,-dt,0.0f } );
		}
	}

	while( const auto delta = wnd.mouse.ReadRawDelta() )
	{
		if( !wnd.CursorEnabled() )
		{
			cam.Rotate( (float)delta->x,(float)delta->y );
		}
	}
		
	// imgui windows
	cam.SpawnControlWindow();
	starLights.at(201).SpawnInfoWindow();
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