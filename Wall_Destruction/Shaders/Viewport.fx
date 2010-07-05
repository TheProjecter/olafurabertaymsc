#include "GlobalParameters.inc"

Texture2D tex;

SamplerState Filter
{
	Filter = ANISOTROPIC;
// 	AddressU = WRAP;
// 	AddressV = WRAP;
// 	BorderColor = 
	AddressU = BORDER;
	AddressV = BORDER;
	// Blue border color
	BorderColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct VS_INPUT_WITH_TEXTURE{
	float3 Pos : POSITION;
	float2 UV : TEXCOORD0;
};

struct VS_OUTPUT_WITH_TEXTURE{
	float4 Pos : SV_POSITION;	
	float2 UV : TEXCOORD0;
};

//
// Vertex Shader
//
VS_OUTPUT_WITH_TEXTURE VSWithTexture( VS_INPUT_WITH_TEXTURE input) 
{
	VS_OUTPUT_WITH_TEXTURE output = (VS_OUTPUT_WITH_TEXTURE) 0;
	output.Pos = float4(input.Pos, 1.0f);
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

technique10 ViewportTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSWithTexture() ) );
	    SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, PSWithTexture() ) );
		
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(SOLID);
 	    SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}