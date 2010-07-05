#ifndef DRAWABLETEX2D_H
#define DRAWABLETEX2D_H

#include "d3dUtil.h"

class DrawableTex2D
{
public:
	DrawableTex2D();
	~DrawableTex2D();

	void init(UINT width, UINT height, int colorMapCount,
		DXGI_FORMAT colorFormat);

	ID3D10ShaderResourceView* colorMap(int index);
	ID3D10ShaderResourceView* depthMap();

	void begin();
	void end();

private:
	DrawableTex2D(const DrawableTex2D& rhs);
	DrawableTex2D& operator=(const DrawableTex2D& rhs);

	void buildDepthMap();
	void buildColorMap(int count);
private:
	UINT mWidth;
	UINT mHeight;
	DXGI_FORMAT mColorMapFormat;

	ID3D10ShaderResourceView** colorMapsSRV;
	ID3D10RenderTargetView** colorMapsRTV;

	ID3D10ShaderResourceView* mDepthMapSRV;
	ID3D10DepthStencilView* mDepthMapDSV;

	D3D10_VIEWPORT mViewport;
	D3D10_VIEWPORT regularViewport;
	int mColorMapCount;
};

#endif // DRAWABLETEX2D_H