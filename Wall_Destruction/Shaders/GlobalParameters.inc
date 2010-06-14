#ifndef GLOBAL_PARAMETERS
#define GLOBAL_PARAMETERS

float4x4 Projection;
float4x4 View;
float4x4 World;

float3 CameraPos;
float3 LightPos;
float3 LightDirection;
float4 LightColor;
float3 AmbientColor;


float A;
float B; 
float rhoOverPi;

float Roughness;

RasterizerState SOLID
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

BlendState AlphaBlending 
{ 
    AlphaToCoverageEnable = TRUE; 
    BlendEnable[0] = TRUE; 
    SrcBlend = SRC_ALPHA; 
    DestBlend = INV_SRC_ALPHA; 
    BlendOp = ADD; 
    SrcBlendAlpha = ZERO; 
    DestBlendAlpha = ZERO; 
    BlendOpAlpha = ADD; 
    RenderTargetWriteMask[0] = 0x0F; 
}; 

#endif