#include "Crosshair.h"

Crosshair::Crosshair(void)
{
}

Crosshair::~Crosshair(void)
{
}

void Crosshair::Init(){
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // UV
	};

	this->size = 0.02f;

	crosshairEffect = Helpers::CustomEffect("Simple.fx", "SimpleTextureConstantPosTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, layout, 2);

	crosshairEffect.AddTexture("tex", "Textures\\Crosshair.png");
	crosshairEffect.SetTexture("tex", "Textures\\Crosshair.png");	

	ResetVertexBuffer();
}

void Crosshair::ResetVertexBuffer(){
	ProjectStructs::SIMPLE_TEXTURE_VERTEX vertices[4];
	vertices[0].pos = D3DXVECTOR3(-size, -size * Helpers::Globals::ClientWidth / Helpers::Globals::ClientHeight, 0.0f);
	vertices[0].UV = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].pos = D3DXVECTOR3(-size, size * Helpers::Globals::ClientWidth / Helpers::Globals::ClientHeight, 0.0f);
	vertices[1].UV = D3DXVECTOR2(0.0f, 1.0f);

	vertices[2].pos = D3DXVECTOR3(size, -size * Helpers::Globals::ClientWidth / Helpers::Globals::ClientHeight, 0.0f);
	vertices[2].UV = D3DXVECTOR2(1.0f, 0.0f);

	vertices[3].pos = D3DXVECTOR3(size, size * Helpers::Globals::ClientWidth / Helpers::Globals::ClientHeight, 0.0f);
	vertices[3].UV = D3DXVECTOR2(1.0f, 1.0f);

/*
	vertices[0].pos = D3DXVECTOR3(pos.x, pos.y , 0.0f);
	vertices[0].UV = D3DXVECTOR2(pos.x + width < 0.0f ? 1.0f : 0.0f, pos.y + height < 0.0f ? 1.0f : 0.0f);
	vertices[1].pos = D3DXVECTOR3(pos.x, pos.y + height, 0.0f);
	vertices[1].UV = D3DXVECTOR2(pos.x + width < 0.0f ? 1.0f : 0.0f, pos.y + height >= 0.0f ? 1.0f : 0.0f);
	vertices[2].pos = D3DXVECTOR3(pos.x + width, pos.y , 0.0f);
	vertices[2].UV = D3DXVECTOR2(pos.x + width >= 0.0f ? 1.0f : 0.0f, pos.y + height < 0.0f ? 1.0f : 0.0f);
	vertices[3].pos = D3DXVECTOR3(pos.x + width, pos.y+ height, 0.0f);
	vertices[3].UV = D3DXVECTOR2(pos.x + width >= 0.0f ? 1.0f : 0.0f, pos.y + height >= 0.0f ? 1.0f : 0.0f);
*/
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = 4 * sizeof( ProjectStructs::SIMPLE_TEXTURE_VERTEX);
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;

	HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &crosshairBuffer));	
}

void Crosshair::Draw(){
	
	crosshairEffect.PreDraw();

	UINT stride = sizeof( ProjectStructs::SIMPLE_TEXTURE_VERTEX);
	UINT offset = 0;

	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &crosshairBuffer, &stride, &offset );

	crosshairEffect.Draw(4);
}

void Crosshair::CleanUp(){
	crosshairBuffer->Release();
	crosshairBuffer = NULL;

	crosshairEffect.CleanUp();
}
