#include "GlobalParameters.inc"
//#include "PhongLighting.inc"
#include "OrenNayerLighting.inc"

Texture2D tex;
float4 Color;

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

struct VS_INPUT{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
};

struct VS_OUTPUT{
	float4 Pos : SV_POSITION;	
	float4 PosW : POSITION;
	float4 NormalW : NORMAL;
	float3 LightDir : TEXCOORD1;
	float3 EyeVect: TEXCOORD2;
};

struct VS_DEPTH_INPUT{
	float4 Pos : POSITION;
};

struct VS_DEPTH_OUTPUT{
	float4 Pos : SV_POSITION;	
	float Depth : Depth;
};

//
// Vertex Shader
//
VS_OUTPUT_WITH_TEXTURE VSWithTexture( VS_INPUT_WITH_TEXTURE input) 
{
	VS_OUTPUT_WITH_TEXTURE output = (VS_OUTPUT_WITH_TEXTURE) 0;
	output.Pos = mul( mul( mul(input.Pos, World), View), Projection);
	
	output.NormalW = normalize(mul(float4(input.Normal.xyz, 0.0f), World));
	
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

//
// Vertex Shader
//
VS_OUTPUT VS( VS_INPUT input) 
{
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.Pos = mul( mul( mul(input.Pos, World), View), Projection);
	
	output.NormalW = normalize(mul(float4(input.Normal.xyz, 0.0f), World));
	
	// Determine the light direction vector. This assumes that the vector
	// is constant relative to the camera.
	output.LightDir = -normalize(float4(LightDirection, 1.0f)).xyz;
	
	// Determine the eye vector for the light
	output.EyeVect = -normalize(float4(CameraPos, 1.0f) + input.Pos).xyz;
	
    return output;
}

//
// Pixel Shader
//
float4 PS( VS_OUTPUT input) : SV_Target
{
	return Color * Light_OrenNayer(input.NormalW.xyz, input.EyeVect, input.LightDir);	
}

//
// Depth Vertex Shader
//
VS_DEPTH_OUTPUT VSDepth( VS_DEPTH_INPUT input) 
{
	VS_DEPTH_OUTPUT output = (VS_DEPTH_OUTPUT) 0;
	
	output.Pos =  mul( mul( mul(input.Pos, World), View), Projection);
	output.Depth = output.Pos.z;
    return output;
}

//
// Depth Pixel Shader
//
float4 PSDepth( VS_DEPTH_OUTPUT input) : SV_Target
{
	float pixelDepth = GetDepth(input.Pos);
	return float4(pixelDepth , pixelDepth , pixelDepth , 1.0f);
}

technique10 SphereDepthTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSDepth() ) );
	    SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, PSDepth() ) );
		
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(SOLID);
 	    SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
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
 	    SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 PhyxelTechnique
{
	pass P0
	{
	    SetVertexShader( CompileShader( vs_4_0, VS() ) );
	    SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		
		SetDepthStencilState( EnableDepth, 0 );
		
		SetRasterizerState(SOLID);
 	    SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}