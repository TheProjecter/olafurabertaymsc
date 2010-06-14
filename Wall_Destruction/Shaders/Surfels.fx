#include "Math.inc"
#include "GlobalParameters.inc"
//#include "PhongLighting.inc"
#include "OrenNayerLighting.inc"

Texture2D SurfelTexture;

SamplerState SurfelFilter
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_SURFEL_INPUT{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float2 Dimensions: TEXCOORD0;
};

struct GS_SURFEL_INPUT{	
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float2 Dimensions: TEXCOORD0;
};

struct PS_SURFEL_INPUT{
	float4 Pos : SV_POSITION;
	float4 PosW : POSITION;
	float4 NormalW : NORMAL;
	float3 LightDir : TEXCOORD2;
	float3 EyeVect: TEXCOORD1;
	float2 UV : TEXCOORD;
};

struct VS_INPUT{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float2 UV: TEXCOORD0;
};

struct PS_INPUT{
	float4 Pos : SV_POSITION;
	float4 PosW : POSITION;
	float4 NormalW : NORMAL;
	float3 LightDir : TEXCOORD2;
	float3 EyeVect: TEXCOORD1;
	float2 UV : TEXCOORD0;
};

//
// Vertex Shader
//
GS_SURFEL_INPUT VS_Surfel( VS_SURFEL_INPUT input) 
{
	GS_SURFEL_INPUT output = (GS_SURFEL_INPUT) 0;
	output.Pos = input.Pos;
	output.Normal = input.Normal;
	output.Dimensions = input.Dimensions;

    return output;
}

//
// Takes in a point and output a single quad
//
[maxvertexcount(4)]
void GS_Surfel(point GS_SURFEL_INPUT input[1], inout TriangleStream<PS_SURFEL_INPUT> surfelStream, uniform bool useWVP)
{
    PS_SURFEL_INPUT output;
    
    float3 tan1 = normalize(Perpendicular( input[0].Normal.xyz));
	float3 tan2 = normalize(cross(input[0].Normal.xyz, tan1.xyz));

	float xMultiplier[] = {-0.5f, -0.5f, 0.5f, 0.5f};
	float yMultiplier[] = {-0.5f, 0.5f, -0.5f, 0.5f};

	uint U[] = {0, 0, 1, 1};
	uint V[] = {1, 0, 1, 0};
    
	[unroll]
    for(int i=0; i<4; i++)
    {		
		output.Pos = float4(input[0].Pos.xyz + xMultiplier[i] * input[0].Dimensions.x * tan2 + yMultiplier[i] * input[0].Dimensions.y * tan1, 1.0f);
		
		if(useWVP){
			output.Pos = mul( mul( mul( output.Pos, World), View), Projection);
		}
		output.PosW = mul(output.Pos, World);
		
		float4x4 tmp = World;
		tmp[3][0] = 0.0f;
		tmp[3][1] = 0.0f;
		tmp[3][2] = 0.0f;
		
		output.NormalW = normalize(mul(input[0].Normal, tmp));
		
		// Determine the light direction vector. This assumes that the vector
		// is constant relative to the camera.
		output.LightDir = -normalize(float4(LightDirection, 1.0f)).xyz;
		
		// Determine the eye vector for the light
		output.EyeVect = -normalize(float4(CameraPos, 1.0f) + input[0].Pos).xyz;

		output.UV = float2(U[i], V[i]);
		surfelStream.Append(output);
    }

    surfelStream.RestartStrip();
}

//
// Pixel Shader
//
float4 PS_Surfel( PS_SURFEL_INPUT input) : SV_Target
{
	return SurfelTexture.Sample(SurfelFilter, input.UV) * Light_OrenNayer(input.NormalW.xyz, input.EyeVect, input.LightDir);;// * float4(ParallelLight(input.PosW.xyz, input.NormalW.xyz), 1.0f);
}

//
// Vertex Shader
//
PS_INPUT VS( VS_INPUT input) 
{    
    PS_INPUT output = (PS_INPUT) 0;
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
float4 PS( PS_INPUT input) : SV_Target
{
	return SurfelTexture.Sample(SurfelFilter, input.UV) * Light_OrenNayer(input.NormalW.xyz, input.EyeVect, input.LightDir);//float4(ParallelLight(input.PosW.xyz, input.NormalW.xyz), 1.0f);
}

GeometryShader gsStreamOut = ConstructGSWithSO( CompileShader( gs_4_0, GS_Surfel(false) ), "SV_POSITION.xyz; NORMAL.xyz; TEXCOORD.xy" );

technique10 GeometryTechnique
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Surfel() ) );
		SetGeometryShader ( gsStreamOut );
	    SetPixelShader( NULL);

		SetDepthStencilState( DisableDepth, 0 );
		SetRasterizerState(SOLID);
	}
}

technique10 SurfelTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_Surfel() ) );
		SetGeometryShader ( CompileShader( gs_4_0, GS_Surfel(true) ) );
	    SetPixelShader( CompileShader( ps_4_0, PS_Surfel() ) );
	    SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(SOLID);
 	    SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}


technique10 SolidTechnique
{
    pass P0
    {
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader ( NULL );
	    SetPixelShader( CompileShader( ps_4_0, PS() ) );
	    SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(SOLID);
		SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 WireframeTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader ( NULL );
	    SetPixelShader( CompileShader( ps_4_0, PS() ) );
	    SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(WF);
		SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}
