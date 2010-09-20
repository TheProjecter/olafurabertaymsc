#include "TextureCreator.h"
#include "Globals.h"

ID3D10ShaderResourceView* TextureCreator::CreateSurfelTexture(){
	// texture creation technique taken from http://msdn.microsoft.com/en-us/library/bb205131(v=VS.85).aspx
	D3D10_TEXTURE2D_DESC desc;
	desc.Width = 129;
	desc.Height = 129;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D10_USAGE_DYNAMIC;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	ID3D10Texture2D* pTexture = 0;
	HR(Helpers::Globals::Device->CreateTexture2D(&desc, 0, &pTexture));

	D3D10_MAPPED_TEXTURE2D mappedTex;
 	pTexture->Map( D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex );

	D3DXVECTOR2 center = D3DXVECTOR2(desc.Width/2.0f, desc.Height/2.0f);
	D3DXVECTOR2 pos = D3DXVECTOR2(0.0f, 0.0f);
	UCHAR* pTexels = (UCHAR*)mappedTex.pData;
	float dist;
	float radius = desc.Width/2.0f;

	for( UINT row = 0; row < desc.Height; row++ )
	{
		UINT rowStart = row * mappedTex.RowPitch;
		for( UINT col = 0; col < desc.Width; col++ )
		{
			pos.x = (float)col;
			pos.y = (float)row;

			D3DXVec2Subtract(&pos, &center, &pos);
			dist = D3DXVec2Length(&pos);

			UINT colStart = col * 4;
			
			if (dist > radius)
			{
				pTexels[rowStart + colStart + 0] = 255; // Red
				pTexels[rowStart + colStart + 1] = 255; // Green
				pTexels[rowStart + colStart + 2] = 255;  // Blue
				pTexels[rowStart + colStart + 3] = 0;  // Alpha
			}
			else 
			{
				int x = (int)(col - radius);
				int y = (int)(row - radius);

				pTexels[rowStart + colStart + 0] = 255; // Red
				pTexels[rowStart + colStart + 1] = 255; // Green
				pTexels[rowStart + colStart + 2] = 255; // Blue
				pTexels[rowStart + colStart + 3] = (UCHAR)((double)exp(-((x * x) + (y * y )) / (radius*radius)) * (double)255.0f);
			}
		}
	}

	pTexture->Unmap( D3D10CalcSubresource(0, 0, 1) );
	
	ID3D10ShaderResourceView *pResourceView= NULL;
	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(Helpers::Globals::Device->CreateShaderResourceView(pTexture,	&srvDesc, &pResourceView));

	return pResourceView;
}

ID3D10ShaderResourceView* TextureCreator::CreateSurfelWireframeTexture(){
	// texture creation technique taken from http://msdn.microsoft.com/en-us/library/bb205131(v=VS.85).aspx
	D3D10_TEXTURE2D_DESC desc;
	desc.Width = 256;
	desc.Height = 256;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D10_USAGE_DYNAMIC;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	ID3D10Texture2D* pTexture = 0;
	HR(Helpers::Globals::Device->CreateTexture2D(&desc, 0, &pTexture));

	D3D10_MAPPED_TEXTURE2D mappedTex;
	pTexture->Map( D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex );

	D3DXVECTOR2 center = D3DXVECTOR2(desc.Width/2.0f, desc.Height/2.0f);
	D3DXVECTOR2 pos = D3DXVECTOR2(0.0f, 0.0f);
	UCHAR* pTexels = (UCHAR*)mappedTex.pData;
	float dist;
	float radius = desc.Width/2.0f;

	for( UINT row = 0; row < desc.Height; row++ )
	{
		UINT rowStart = row * mappedTex.RowPitch;
		for( UINT col = 0; col < desc.Width; col++ )
		{
			pos.x = (float)col;
			pos.y = (float)row;

			D3DXVec2Subtract(&pos, &center, &pos);
			dist = D3DXVec2Length(&pos);

			UINT colStart = col * 4;

			if (dist < radius )
			{
				pTexels[rowStart + colStart + 0] = 255; // Red
				pTexels[rowStart + colStart + 1] = 255; // Green
				pTexels[rowStart + colStart + 2] = 255;  // Blue
				pTexels[rowStart + colStart + 3] = 255;  // Alpha
			}
			else 
			{
				pTexels[rowStart + colStart + 0] = 0; // Red
				pTexels[rowStart + colStart + 1] = 0; // Green
				pTexels[rowStart + colStart + 2] = 0; // Blue
				pTexels[rowStart + colStart + 3] = 0;
			}
		}
	}

	pTexture->Unmap( D3D10CalcSubresource(0, 0, 1) );

	ID3D10ShaderResourceView *pResourceView= NULL;
	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(Helpers::Globals::Device->CreateShaderResourceView(pTexture,	&srvDesc, &pResourceView));

	return pResourceView;
}


ID3D10ShaderResourceView* TextureCreator::CreateEmptyTexture(int width, int height){
	// texture creation technique taken from http://msdn.microsoft.com/en-us/library/bb205131(v=VS.85).aspx
	D3D10_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D10_USAGE_DYNAMIC;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	ID3D10Texture2D* pTexture = 0;
	HR(Helpers::Globals::Device->CreateTexture2D(&desc, 0, &pTexture));

	D3D10_MAPPED_TEXTURE2D mappedTex;
	pTexture->Map( D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex );

	UCHAR* pTexels = (UCHAR*)mappedTex.pData;

	for( UINT row = 0; row < desc.Height; row++ )
	{
		UINT rowStart = row * mappedTex.RowPitch;
		for( UINT col = 0; col < desc.Width; col++ )
		{

			UINT colStart = col * 4;
			pTexels[rowStart + colStart + 0] = 0; // Red
			pTexels[rowStart + colStart + 1] = 0; // Green
			pTexels[rowStart + colStart + 2] = 0;  // Blue
			pTexels[rowStart + colStart + 3] = 0;  // Alpha
		}
	}

	pTexture->Unmap( D3D10CalcSubresource(0, 0, 1) );

	ID3D10ShaderResourceView *pResourceView= NULL;
	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(Helpers::Globals::Device->CreateShaderResourceView(pTexture,	&srvDesc, &pResourceView));

	return pResourceView;
}


ID3D10ShaderResourceView* TextureCreator::CreateExpTable(){
	// texture creation technique taken from http://msdn.microsoft.com/en-us/library/bb205131(v=VS.85).aspx
	D3D10_TEXTURE2D_DESC desc;
	desc.Width = 256;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D10_USAGE_DYNAMIC;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	ID3D10Texture2D* pTexture = 0;
	HR(Helpers::Globals::Device->CreateTexture2D(&desc, 0, &pTexture));

	D3D10_MAPPED_TEXTURE2D mappedTex;
	pTexture->Map( D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedTex );

	UCHAR* pTexels = (UCHAR*)mappedTex.pData;

	for( UINT col = 0; col < desc.Width; col++ )
	{
		UINT colStart = col * 4;
		UCHAR weight = (UCHAR)(exp(-1.0f* float(col)/float(desc.Width)) * 255);
		pTexels[colStart] = weight; // Red
		pTexels[colStart + 1] = weight ; // Green
		pTexels[colStart + 2] = weight ;  // Blue
		pTexels[colStart + 3] = 255;  // Alpha
	}

	pTexture->Unmap( D3D10CalcSubresource(0, 0, 1) );

	ID3D10ShaderResourceView *pResourceView= NULL;
	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(Helpers::Globals::Device->CreateShaderResourceView(pTexture,	&srvDesc, &pResourceView));

	return pResourceView;
}


