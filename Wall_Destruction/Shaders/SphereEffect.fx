#include "GlobalParameters.inc"
//#include "PhongLighting.inc"
#include "OrenNayerLighting.inc"

Texture2D tex;

SamplerState Filter
{
	Filter = ANISOTROPIC;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_INPUT_WITH_TEXTURE{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float2 UV : TEXCOORD0;
};

struct VS_OUTPUT_WITH_TEXTURE{
	float4 Pos : SV_POSITION;	
	float4 PosW : POSITION;
	float4 NormalW : NORMAL;
	float2 UV : TEXCOORD0;
	float3 LightDir : TEXCOORD1;
	float3 EyeVect: TEXCOORD2;
};

//
// Vertex Shader
//
VS_OUTPUT_WITH_TEXTURE VSWithTexture( VS_INPUT_WITH_TEXTURE input) 
{
	VS_OUTPUT_WITH_TEXTURE output = (VS_OUTPUT_WITH_TEXTURE) 0;
	output.Pos = mul( mul( mul(input.Pos, World), View), Projection);
	output.PosW = mul(input.Pos, World);
	
	float4x4 tmp = World;
	tmp[3][0] = 0.0f;
	tmp[3][1] = 0.0f;
	tmp[3][2] = 0.0f;
	
	output.NormalW = normalize(mul(input.Normal, tmp));
	
	// Determine the light direction vector. This assumes that the vector
	// is constant relative to the camera.
	output.LightDir = -normalize(float4(LightDirection, 1.0f)).xyz;
	
	// Determine the eye vector for the light
	output.EyeVect = -normalize(float4(CameraPos, 1.0f) + input.Pos).xyz;
	
	output.UV = input.UV;
    return output;
}

//
// Pixel Shader
//
float4 PSWithTexture( VS_OUTPUT_WITH_TEXTURE input) : SV_Target
{
	return tex.Sample(Filter, input.UV) * Light_OrenNayer(input.NormalW.xyz, input.EyeVect, input.LightDir);	
}

technique10 SphereTechnique
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