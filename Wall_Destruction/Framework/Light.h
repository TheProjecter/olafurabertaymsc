#ifndef LIGHT_H
#define LIGHT_H
#include <D3DX10.h>
class Light
{
public:
	void Init();
	D3DXVECTOR3 GetPosition(){return pos;}
	D3DXVECTOR3 GetDirection(){return dir;}
	D3DXVECTOR3 GetAmbientColor(){return ambientColor;}
	D3DXVECTOR3 GetDiffuseColor(){return diffuseColor;}
	D3DXVECTOR3 GetSpecularColor(){return specularColor;}
	D3DXCOLOR GetColor(){return color;}
	
private:
	D3DXVECTOR3 dir, pos;
	D3DXCOLOR color;
	D3DXVECTOR3 ambientColor;
	D3DXVECTOR3 diffuseColor;
	D3DXVECTOR3 specularColor;
};

#endif