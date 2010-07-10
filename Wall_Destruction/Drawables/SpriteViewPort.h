#ifndef SPRITEVIEWPORT_H
#define SPRITEVIEWPORT_H

#include <D3DX10.h>
#include "CustomEffect.h"

struct VERTEX{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 UV;
};

class SpriteViewPort
{
public:
	SpriteViewPort(void){};
	SpriteViewPort(float width, float height, D3DXVECTOR2 pos);
	~SpriteViewPort(void){};

	void Init();
	void Draw(ID3D10ShaderResourceView* texture);
	void CleanUp();

private:
	float width, height;
	D3DXVECTOR2 pos;
	Helpers::CustomEffect effect;
	ID3D10Buffer *vb;
};

#endif
