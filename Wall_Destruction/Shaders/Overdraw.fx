#include "GlobalParameters.inc"

Texture2D EWATexture;
Texture2D SurfaceTexture;

SamplerState Filter
{
	Filter = ANISOTROPIC;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct Quad_VS_Input{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float2 UV: TEXCOORD0;
	float2 EWAUV: TEXCOORD1;
};

struct Quad_PS_Input{
	float4 Pos : SV_POSITION;
	float4 NormalW : NORMAL;
	float2 EWAUV: TEXCOORD3;
};

Quad_PS_Input QuadVS(Quad_VS_Input vsIn){
	Quad_PS_Input psOut = (Quad_PS_Input)0;
	
	psOut.Pos = mul(mul(mul(vsIn.Pos, World), View), Projection);
	psOut.NormalW = vsIn.Normal;
	psOut.EWAUV= vsIn.EWAUV;
	
	return psOut;
}

float4 QuadPS(Quad_PS_Input psIn) : SV_Target{
	float4 surfelColor;
	
	if(EWATexture.Sample(Filter, psIn.EWAUV).a == 0.0f)
		clip(-1);
		
		
	surfelColor =  float4(1.0f, 1.0f, 1.0f, 0.25f);
	
	
	
	
	/*if(surfelColor.a == 0.0)
		clip(-1);
	*/
	return surfelColor;
}

technique10 SolidTechnique
{
    pass P0
    {
		SetVertexShader( CompileShader( vs_4_0, QuadVS() ) );
		SetGeometryShader ( NULL );
	    SetPixelShader( CompileShader( ps_4_0, QuadPS() ) );
	    SetDepthStencilState( SamePlaneDepth, 0 );
		SetRasterizerState(OVERDRAW);
 	    SetBlendState( OverDrawBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}