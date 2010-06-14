#include "Light.h"

void Light::Init()
{
	this->pos = D3DXVECTOR3(0.0f, 100.0f, 0.0f);
	this->dir = /*D3DXVECTOR3(0.0f, -1.0f, 0.0f);*/D3DXVECTOR3(-0.57735f, -0.57735f, 0.57735f);
	this->ambientColor = D3DXVECTOR3(0.3f, 0.3f, 0.3f);
	this->diffuseColor = D3DXVECTOR3(0.1f, 0.1f, 0.1f);
	this->specularColor = D3DXVECTOR3(0.1f, 0.1f, 0.1f);

	this->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

