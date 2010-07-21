#include "GlobalParameters.inc"

struct VS_SIMPLE_INPUT{
	float4 Pos : POSITION;
};

struct GS_SIMPLE_INPUT{
	float4 Pos : POSITION;
};

struct PS_SIMPLE_INPUT{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
};

// simple vs
GS_SIMPLE_INPUT SimpleVS(VS_SIMPLE_INPUT vsIn){
	GS_SIMPLE_INPUT vsOut = (GS_SIMPLE_INPUT) 0;
	
	vsOut.Pos = vsIn.Pos;
	
	return vsOut;	
}

[maxvertexcount(2)]
void SimpleGS(line GS_SIMPLE_INPUT input[2], inout LineStream<PS_SIMPLE_INPUT> ls)
{
    PS_SIMPLE_INPUT output = (PS_SIMPLE_INPUT) 0;
    
    output.Pos =  mul( mul( mul( input[0].Pos, World), View), Projection);
    output.Color =  float4(0.0f, 1.0f, 0.0f, 1.0f);
    
	ls.Append(output);
    
    if(input[0].Pos.x == input[1].Pos.x && input[0].Pos.y == input[1].Pos.y && input[0].Pos.z == input[1].Pos.z)
	{
		input[1].Pos.y += 0.1f; 		
	}
    
    output.Pos = mul( mul( mul( input[1].Pos, World), View), Projection);
    output.Color =  float4(1.0f, 0.0f, 0.0f, 1.0f);
    
	ls.Append(output);
    
	ls.RestartStrip();		  
}

// simple ps
float4 SimplePS(PS_SIMPLE_INPUT vsIn) : SV_TARGET{
	return vsIn.Color;
}

technique10 PhyxelTechnique
{
	pass P0
	{
	    SetVertexShader( CompileShader( vs_4_0, SimpleVS() ) );
	    SetGeometryShader ( CompileShader( gs_4_0, SimpleGS() ) );
        SetPixelShader( CompileShader( ps_4_0, SimplePS() ) );
		
		SetDepthStencilState( EnableDepth, 0 );
		
		SetRasterizerState(SOLID);
 	    SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}