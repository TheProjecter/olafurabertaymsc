#include "GlobalParameters.inc"
//#include "PhongLighting.inc"
#include "OrenNayerLighting.inc"

float4 Color;
float4 Force;

struct VS_SIMPLE_INPUT{
	float4 Pos : POSITION;
};

struct PS_SIMPLE_INPUT{
	float4 Pos : SV_POSITION;
};

// simple vs
PS_SIMPLE_INPUT SimpleVS(VS_SIMPLE_INPUT vsIn){
	PS_SIMPLE_INPUT vsOut = (PS_SIMPLE_INPUT) 0;
	
	vsOut.Pos = mul( mul( mul( vsIn.Pos, World), View), Projection);
	
	return vsOut;	
}

// simple ps
float4 SimplePS(PS_SIMPLE_INPUT vsIn) : SV_TARGET{
	return Color  - float4(0.0f, distance(Force, float4(0.0f, 0.0f, 0.0f, 0.0f)) / 1000.0f, distance(Force, float4(0.0f, 0.0f, 0.0f, 0.0f))/ 1000.0f, 0.0f);
}

technique10 PhyxelTechnique
{
	pass P0
	{
	    SetVertexShader( CompileShader( vs_4_0, SimpleVS() ) );
	    SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, SimplePS() ) );
		
		SetDepthStencilState( EnableDepth, 0 );
		
		SetRasterizerState(SOLID);
 	    SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}