#ifndef GLOBAL_PARAMETERS
#define GLOBAL_PARAMETERS

float4x4 Projection;
float4x4 View;
float4x4 World;

float4x4 WVP;
float4x4 WV;
float4x4 WVInv;

float3 CameraPos;
float3 LightPos;
float3 LightDirection;
float4 LightColor;
float3 AmbientColor;

float A;
float B; 
float rhoOverPi;

float Roughness;

// map for depth checking
Texture2D DepthMap;

float windowHeight;
float windowWidth;

SamplerState DepthFilter
{
	Filter = ANISOTROPIC;
	AddressU = WRAP;
	AddressV = WRAP;
};

float GetDepth(float4 pixelPos){
	return saturate(pixelPos.z / pixelPos.w);
}

bool IsInFront(float4 pixelPos){
	float2 DepthTexC = float2(pixelPos.x / windowWidth, pixelPos.y / windowHeight);
    
    float shadowdepth = DepthMap.Sample(DepthFilter, DepthTexC ).r;    
	  
    // Check our value against the depth value
    float ourdepth = GetDepth(pixelPos);
    
    return shadowdepth-0.003f < ourdepth;
}

RasterizerState SOLID
{
	FillMode = Solid;
	CullMode = BACK;
	FrontCounterClockwise = false;
};


RasterizerState SURFEL
{
	FillMode = Solid;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

RasterizerState SURFEL_DEPTH
{
	FillMode = Solid;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

RasterizerState WF
{
	FillMode = Wireframe;
	CullMode = NONE;
	FrontCounterClockwise = false;
};

DepthStencilState DisableDepth
{
	DepthEnable = FALSE;
	DepthWriteMask = ZERO;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;	
	DepthWriteMask = ALL;
};

DepthStencilState SamePlaneDepth
{
	DepthEnable = TRUE;	
	DepthWriteMask = ZERO;
};

BlendState NoAlphaBlending 
{ 
    AlphaToCoverageEnable = FALSE; 
    BlendEnable[0] = FALSE; 
}; 

BlendState AlphaBlending
{
    /*AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_COLOR;
	DestBlend = DEST_COLOR;
	BlendOp = ADD;
	SrcBlendAlpha  = SRC_ALPHA;
	DestBlendAlpha = ONE;
	BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;*/
    
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = ONE;
	BlendOp = MAX;
	SrcBlendAlpha = SRC_ALPHA;
	DestBlendAlpha = INV_SRC_ALPHA;
	BlendOpAlpha = MAX;
	RenderTargetWriteMask[0] = 0x0F;
	
};

#endif

