#include "SpriteViewPort.h"

SpriteViewPort::SpriteViewPort(float width, float height, D3DXVECTOR2 pos)
{
	this->width = width;
	this->height = height;
	this->pos = pos;
}

void SpriteViewPort::Init(){

	VERTEX *vertices = NULL;
	vertices = new VERTEX[4];

	vertices[0].pos = D3DXVECTOR3(pos.x, pos.y , 0.0f);
	vertices[0].UV = D3DXVECTOR2(pos.x + width < 0.0f ? 1.0f : 0.0f, pos.y + height < 0.0f ? 1.0f : 0.0f);
	vertices[1].pos = D3DXVECTOR3(pos.x, pos.y + height, 0.0f);
	vertices[1].UV = D3DXVECTOR2(pos.x + width < 0.0f ? 1.0f : 0.0f, pos.y + height >= 0.0f ? 1.0f : 0.0f);
	vertices[2].pos = D3DXVECTOR3(pos.x + width, pos.y , 0.0f);
	vertices[2].UV = D3DXVECTOR2(pos.x + width >= 0.0f ? 1.0f : 0.0f, pos.y + height < 0.0f ? 1.0f : 0.0f);
	vertices[3].pos = D3DXVECTOR3(pos.x + width, pos.y+ height, 0.0f);
	vertices[3].UV = D3DXVECTOR2(pos.x + width >= 0.0f ? 1.0f : 0.0f, pos.y + height >= 0.0f ? 1.0f : 0.0f);

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = 4 * sizeof(VERTEX);
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;

	HR(Helpers::Globals::Device->CreateBuffer( &bd, &InitData, &vb));	
	// Define the input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // UV
	};

	// create the effect
	effect = Helpers::CustomEffect("Viewport.fx", "ViewportTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_GEOMETRY | CUSTOM_EFFECT_TYPE_VERTEX , layout, 2);

	effect.AddVariable("tex");
	effect.AddVariable("View");
	effect.AddVariable("World");
	effect.AddVariable("Projection");
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	effect.SetMatrix("World", I);
}

void SpriteViewPort::Draw(ID3D10ShaderResourceView* texture){
	
	effect.SetMatrix("View", Helpers::Globals::AppCamera.View());
	effect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
	effect.SetTexture("tex", texture);

	effect.PreDraw();

	UINT stride = sizeof( VERTEX );
	UINT offset = 0;

	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Helpers::Globals::Device->IASetVertexBuffers( 0, 1, &vb, &stride, &offset );

	effect.Draw(4);
}

void SpriteViewPort::CleanUp(){
	vb->Release();
	vb = NULL;
}

