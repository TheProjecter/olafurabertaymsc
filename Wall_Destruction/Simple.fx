
float4x4 World;
float4x4 View;
float4x4 Projection;


struct VS_SURFEL_INPUT{
	float4 Pos : POSITION;
	float4 Color: COLOR;
};

struct VS_OUTPUT{
	float4 Pos : SV_POSITION;	
	float4 Color: COLOR;
};

//
// Vertex Shader
//
VS_OUTPUT VS( VS_SURFEL_INPUT input) 
{
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.Pos = mul( mul( mul(input.Pos, World), View), Projection);
	output.Color = input.Color;
    return output;
}

//
// Pixel Shader
//
float4 PS( VS_OUTPUT input) : SV_Target
{
    return input.Color;
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
	    SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		
		SetRasterizerState(SOLID);
    }
}
