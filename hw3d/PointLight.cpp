#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(const PointLight & rhs)
	:
	PointLight(rhs.GetGfx(), rhs.GetPos().x, rhs.GetPos().y, rhs.GetPos().z, rhs.GetColor().x, rhs.GetColor().y, rhs.GetColor().z, rhs.GetRadius())
{}

PointLight::PointLight( Graphics& gfx, float x, float y, float z, float r, float g, float b, float radius )
	:
	mesh( gfx,radius, r, g, b ),
	cbuf( gfx ),
	gfx(gfx),
	x(x),
	y(y),
	z(z),
	color(r,g,b),
	radius(radius)
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if( ImGui::Begin( "Light" ) )
	{
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&cbData.pos.x,-60.0f,60.0f,"%.1f" );
		ImGui::SliderFloat( "Y",&cbData.pos.y,-60.0f,60.0f,"%.1f" );
		ImGui::SliderFloat( "Z",&cbData.pos.z,-60.0f,60.0f,"%.1f" );
		
		ImGui::Text( "Intensity/Color" );
		ImGui::SliderFloat( "Intensity",&cbData.diffuseIntensity,0.01f,2.0f,"%.2f",2 );
		ImGui::ColorEdit3( "Diffuse Color",&cbData.diffuseColor.x );
		ImGui::ColorEdit3( "Ambient",&cbData.ambient.x );
		
		ImGui::Text( "Falloff" );
		ImGui::SliderFloat( "Constant",&cbData.attConst,0.05f,10.0f,"%.2f",4 );
		ImGui::SliderFloat( "Linear",&cbData.attLin,0.0001f,4.0f,"%.4f",8 );
		ImGui::SliderFloat( "Quadratic",&cbData.attQuad,0.0000001f,10.0f,"%.7f",10 );

		if( ImGui::Button( "Reset" ) )
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	cbData = {
		{ x,y,z },
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
}

void PointLight::Draw( Graphics& gfx ) const noxnd
{
	mesh.SetPos( cbData.pos );
	mesh.Draw( gfx );
}

void PointLight::Bind( Graphics& gfx,DirectX::FXMMATRIX view ) const noexcept
{
	auto dataCopy = cbData;
	const auto pos = DirectX::XMLoadFloat3( &cbData.pos );
	DirectX::XMStoreFloat3( &dataCopy.pos,DirectX::XMVector3Transform( pos,view ) );
	cbuf.Update( gfx,dataCopy );
	cbuf.Bind( gfx );
}

void PointLight::SetPos(float x_in, float y_in, float z_in) noexcept
{
	x = x_in;
	y = y_in;
	z = z_in;
	Reset();
}

void PointLight::SetAmbient(float percentage) noexcept
{
	const float p = std::clamp(percentage, 0.0f, 1.0f);
	cbData.ambient.x = p;
	cbData.ambient.y = p;
	cbData.ambient.z = p;
}

void PointLight::SetPos(Vec3 posXYZ) noexcept
{
	SetPos(posXYZ.x, posXYZ.y, posXYZ.z);
}

Vec3 PointLight::GetPos() const noexcept
{
	return { x,y,z };
}

Vec3 PointLight::GetColor() const noexcept
{
	return color;
}

float PointLight::GetRadius() const noexcept
{
	return radius;
}

Graphics & PointLight::GetGfx() const noexcept
{
	return gfx;
}
