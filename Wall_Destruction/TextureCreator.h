#ifndef TEXTURE_CREATOR_H
#define TEXTURE_CREATOR_H

#include <D3DX10.h>
#include <D3D10.h>

class TextureCreator
{
public:
	static ID3D10ShaderResourceView* CreateSurfelTexture();
	static ID3D10ShaderResourceView* CreateSurfelWireframeTexture();
	static ID3D10ShaderResourceView* CreateEmptyTexture(int width, int height);
	static ID3D10ShaderResourceView* CreateExpTable();
};


#endif