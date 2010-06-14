#include "GlobalParameters.inc"

Texture2D tex;

SamplerState Filter
{
	Filter = ANISOTROPIC;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_INPUT{
	float4 Pos : POSITION;
};

struct VS_OUTPUT{
	float4 Pos : SV_POSITION;	
};

struct VS_INPUT_WITH_TEXTURE{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float2 UV : TEXCOORD0;
};

struct VS_OUTPUT_WITH_TEXTURE{
	float4 Pos : SV_POSITION;	
	float2 UV : TEXCOORD0;
};

//
// Vertex Shader
//
VS_OUTPUT VS( VS_INPUT input) 
{
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.Pos = mul( mul( mul(input.Pos, World), View), Projection);
    return output;
}

//
// Pixel Shader
//
float4 PS( VS_OUTPUT input) : SV_Target
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

//
// Vertex Shader
//
VS_OUTPUT_WITH_TEXTURE VSWithTexture( VS_INPUT_WITH_TEXTURE input) 
{
	VS_OUTPUT_WITH_TEXTURE output = (VS_OUTPUT_WITH_TEXTURE) 0;
	output.Pos = mul( mul( mul(input.Pos, World), View), Projection);
	output.UV = input.UV;
    return output;
}

//
// Pixel Shader
//
float4 PSWithTexture( VS_OUTPUT_WITH_TEXTURE input) : SV_Target
{
	return tex.Sample(Filter, input.UV);
}

technique10 SimpleTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
	    SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(SOLID);
 	    SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 SimpleTextureTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSWithTexture() ) );
	    SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, PSWithTexture() ) );
		
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(SOLID);
 	    SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}