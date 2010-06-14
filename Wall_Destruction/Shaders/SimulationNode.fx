#include "GlobalParameters.inc"

Texture2D texture;

SamplerState TextureFilter
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_SURFEL_INPUT{
	float4 Pos : POSITION;
};

struct VS_OUTPUT{
	float4 Pos : SV_POSITION;	
	float4 Color : COLOR;
	float2 tex0:  TEXCOORD0;
	float size : PSIZE; 
};

//
// Vertex Shader
//
VS_OUTPUT VS( VS_SURFEL_INPUT input) 
{
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.Pos = mul( mul( mul(input.Pos, World), View), Projection);
	output.size = 10;
	output.Color = float4(0.0f, 1.0f, 0.0f, 1.0f);

    return output;
}

//
// Pixel Shader
//
float4 PS( VS_OUTPUT input) : SV_Target
{
    return input.Color * texture.Sample(textureFilter, input.tex0);
}

RasterizerState SOLID
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

technique10 SimpleTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		
		SetRasterizerState(SOLID);
    }
}
