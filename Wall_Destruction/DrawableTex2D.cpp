#include "DrawableTex2D.h"
#include "Globals.h"

/*
	This class was changed so multiple render targets could be used
*/


DrawableTex2D::DrawableTex2D()
: mWidth(0), mHeight(0), mColorMapFormat(DXGI_FORMAT_UNKNOWN), 
  colorMapsRTV(0), colorMapsSRV(0), mDepthMapSRV(0), mDepthMapDSV(0)
{
	ZeroMemory(&mViewport, sizeof(D3D10_VIEWPORT));
}

DrawableTex2D::~DrawableTex2D()
{
	if( mColorMapCount != 0 ){
		for (int i = 0; i < mColorMapCount ; i++)
		{
			ReleaseCOM(colorMapsSRV[i]);
			ReleaseCOM(colorMapsRTV[i]);
		}
	}
	delete [] colorMapsRTV;
	delete [] colorMapsSRV;

	ReleaseCOM(mDepthMapSRV);
	ReleaseCOM(mDepthMapDSV);
}

void DrawableTex2D::init(UINT width, UINT height, int colorMapCount, 
		                 DXGI_FORMAT colorFormat)
{
	mWidth  = width;
	mHeight = height;
	mColorMapCount = colorMapCount;

	mColorMapFormat = colorFormat;

	buildDepthMap();

	// shadow maps don't need color maps, for example
	if( colorMapCount != 0)
		buildColorMap(colorMapCount);
 
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width    = width;
	mViewport.Height   = height;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

}

ID3D10ShaderResourceView* DrawableTex2D::depthMap()
{
	return mDepthMapSRV;
}

void DrawableTex2D::begin()
{
	Helpers::Globals::Device->OMSetRenderTargets(mColorMapCount, colorMapsRTV, mDepthMapDSV);

	Helpers::Globals::Device->RSSetViewports(1, &mViewport);

	// only clear is we actually created a color map.
	if( mColorMapCount != 0 ){
		for (int i = 0; i < mColorMapCount ; i++)
		{
			Helpers::Globals::Device->ClearRenderTargetView(colorMapsRTV[i], BLACK);			
		}
	}

	Helpers::Globals::Device->ClearDepthStencilView(mDepthMapDSV, D3D10_CLEAR_DEPTH, 1.0f, 0);
}

void DrawableTex2D::end()
{
	// After we have drawn to the color map, have the hardware generate
	// the lower mipmap levels.
	if( mColorMapCount != 0 ){
		for (int i = 0; i < mColorMapCount ; i++)
		{
			Helpers::Globals::Device->GenerateMips(colorMapsSRV[i]);			
		}
	}

	Helpers::Globals::Device->OMSetRenderTargets(1, &Helpers::Globals::RenderTargetView, Helpers::Globals::DepthStencilView);
	
	regularViewport.TopLeftX = 0;
	regularViewport.TopLeftY = 0;
	regularViewport.Width    = Helpers::Globals::ClientWidth;
	regularViewport.Height   = Helpers::Globals::ClientHeight;
	regularViewport.MinDepth = 0.0f;
	regularViewport.MaxDepth = 1.0f;

	Helpers::Globals::Device->RSSetViewports(1, &regularViewport);	
}	

void DrawableTex2D::buildDepthMap()
{
	ID3D10Texture2D* depthMap = 0;

	D3D10_TEXTURE2D_DESC texDesc;
	
	texDesc.Width     = mWidth;
	texDesc.Height    = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format    = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count   = 1;  
	texDesc.SampleDesc.Quality = 0;  
	texDesc.Usage          = D3D10_USAGE_DEFAULT;
	texDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0; 
	texDesc.MiscFlags      = 0;

	HR(Helpers::Globals::Device->CreateTexture2D(&texDesc, 0, &depthMap));

	D3D10_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(Helpers::Globals::Device->CreateDepthStencilView(depthMap, &dsvDesc, &mDepthMapDSV));


	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(Helpers::Globals::Device->CreateShaderResourceView(depthMap, &srvDesc, &mDepthMapSRV));

	// View saves a reference to the texture so we can release our reference.
	ReleaseCOM(depthMap);
}

void DrawableTex2D::buildColorMap(int colorMapCount)
{
	colorMapsSRV = NULL;
	colorMapsSRV = new ID3D10ShaderResourceView*[colorMapCount];

	colorMapsRTV = NULL;
	colorMapsRTV = new ID3D10RenderTargetView*[colorMapCount];

	ID3D10Texture2D** colorMap = NULL;
	colorMap = new ID3D10Texture2D*[colorMapCount];

	for (int i = 0; i < colorMapCount ; i++)
	{
		D3D10_TEXTURE2D_DESC texDesc;
		
		texDesc.Width     = mWidth;
		texDesc.Height    = mHeight;
		texDesc.MipLevels = 0;
		texDesc.ArraySize = 1;
		texDesc.Format    = mColorMapFormat;
		texDesc.SampleDesc.Count   = 1;  
		texDesc.SampleDesc.Quality = 0;  
		texDesc.Usage          = D3D10_USAGE_DEFAULT;
		texDesc.BindFlags      = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0; 
		texDesc.MiscFlags      = D3D10_RESOURCE_MISC_GENERATE_MIPS;

		HR(Helpers::Globals::Device->CreateTexture2D(&texDesc, 0, &colorMap[i]));

		// Null description means to create a view to all mipmap levels using 
		// the format the texture was created with.
	
		HR(Helpers::Globals::Device->CreateRenderTargetView(colorMap[i], 0, &colorMapsRTV[i]));
		HR(Helpers::Globals::Device->CreateShaderResourceView(colorMap[i], 0, &colorMapsSRV[i]));		

		// View saves a reference to the texture so we can release our reference.
		ReleaseCOM(colorMap[i]);
	}
}

ID3D10ShaderResourceView* DrawableTex2D::colorMap( int index )
{
	return colorMapsSRV[index];
}