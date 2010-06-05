#include "CustomRectangle.h"
#include "Globals.h"

/*
http://members.gamedev.net/jhoxley/directx/DirectX10/VertexBuffersinDirect3D10.htm
*/

using namespace Helpers;

namespace Drawables{

	void CustomRectangle::Init(){
	
		// Define the input layout
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};

		// Create vertex buffer
		SimpleVertex vertices[] =
		{
			{D3DXVECTOR3( -0.5f, 0.5f, 0.5f ), color},
			{D3DXVECTOR3( 0.5f, -0.5f, 0.5f ), color},
			{D3DXVECTOR3( -0.5f, -0.5f, 0.5f ), color},
			{D3DXVECTOR3( 0.5f, -0.5f, 0.5f ), color},
			{D3DXVECTOR3( -0.5f, 0.5f, 0.5f ), color},
			{D3DXVECTOR3( 0.5f, 0.5f, 0.5f ), color},
		};

		D3D10_BUFFER_DESC bd;
		bd.Usage = D3D10_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( vertices );
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D10_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = vertices;
		
		HR(Globals::Device->CreateBuffer( &bd, &InitData, &mVB ));
		
		effect = CustomEffect("Simple.fx", "SimpleTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, layout, 2);
				
		effect.AddVariable("World");
		effect.AddVariable("View");
		effect.AddVariable("Projection");

		D3DXMATRIX world;
		D3DXMatrixTranslation(&world, -10.0f, 0.0f, 0.0f);

		effect.SetMatrix("World", world);
	}

	void CustomRectangle::Draw(){

		effect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		effect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

		effect.PreDraw();

		UINT stride = sizeof( SimpleVertex);
		UINT offset = 0;
		Globals::Device->IASetVertexBuffers( 0, 1, &mVB, &stride, &offset );
		Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		effect.Draw(6);
	}
}