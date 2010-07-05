#include "Math.inc"
#include "GlobalParameters.inc"
//#include "PhongLighting.inc"
#include "OrenNayerLighting.inc"

Texture2D EWATexture;
Texture2D SurfaceTexture;

float4 Color;

SamplerState SurfelFilter
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_SURFEL_INPUT{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float4 MajorRadius: TEXCOORD0;
	float4 MinorRadius: TEXCOORD1;
	
	/*float2 UV : TEXCOORD0;
	float2 Ratio : TEXCOORD1;
	float Radius: TEXCOORD2;*/
};

struct GS_SURFEL_INPUT{	
	float4 Pos : POSITION;
	float4 Normal : NORMAL;	
	float4 MajorRadius: TEXCOORD0;
	float4 MinorRadius: TEXCOORD1;
	/*float2 UV : TEXCOORD0;
	float2 Ratio : TEXCOORD1;
	float Radius: TEXCOORD2;*/
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
	output.MajorRadius = input.MajorRadius;
	output.MinorRadius = input.MinorRadius;
	/*output.Radius = input.Radius;
	output.UV = input.UV;
	output.Ratio = input.Ratio;
	output.Radius = input.Radius;*/
	
    return output;
}

//
// Takes in a point and output a single quad
//
[maxvertexcount(4)]
void GS_Surfel(point GS_SURFEL_INPUT input[1], inout TriangleStream<PS_SURFEL_INPUT> surfelStream, uniform bool useWVP)
{
    PS_SURFEL_INPUT output;

	float xMultiplier[] = {-1.0, -1.0, 1.0, 1.0};
	float yMultiplier[] = {-1.0, 1.0, -1.0, 1.0};

	float U[] = {0.0, 0.0, 1.0, 1.0};
	float V[] = {0.0, 1.0, 0.0, 1.0};
    
	[unroll]
    for(int i=0; i<4; i++)
    {		
		float4 originalPos;
				
		if(useWVP){
			originalPos = float4(input[0].Pos.xyz + xMultiplier[i] * input[0].MajorRadius.xyz + yMultiplier[i] * input[0].MinorRadius.xyz, 1.0f);
			output.Pos = originalPos;
			output.Pos = mul( mul( mul( output.Pos, World), View), Projection);
		}
		else{
			originalPos = float4(input[0].Pos.xyz + xMultiplier[i] * input[0].MajorRadius.xyz + yMultiplier[i] * input[0].MinorRadius.xyz , 1.0f);
			output.Pos = originalPos;			
		}
		
		output.PosW = mul(output.Pos, World);
		
		output.NormalW = normalize(mul(float4(input[0].Normal.xyz, 0), World));
		
		// Determine the light direction vector. This assumes that the vector
		// is constant relative to the camera.
		output.LightDir = -normalize(float4(LightDirection, 1.0f)).xyz;
		
		// Determine the eye vector for the light
		output.EyeVect = -normalize(float4(CameraPos, 1.0f) + originalPos).xyz;

		//if(useWVP)
			output.UV = float2(U[i], V[i]);
		/*else
			output.UV = float2(input[0].UV.x + U[i] * input[0].Ratio.x*0.5f, input[0].UV.y + V[i] * input[0].Ratio.y*0.5f);*/
		surfelStream.Append(output);
    }

    surfelStream.RestartStrip();
}

//
// Pixel Shader
//
float4 PS_Surfel( PS_SURFEL_INPUT input) : SV_Target
{
	return EWATexture.Sample(SurfelFilter, input.UV) * SurfaceTexture.Sample(SurfelFilter, input.UV) * Light_OrenNayer(input.NormalW.xyz, input.EyeVect, input.LightDir);
}

//
// Vertex Shader
//
PS_INPUT VS( VS_INPUT input) 
{    
    PS_INPUT output = (PS_INPUT) 0;
	output.Pos = mul( mul( mul(input.Pos, World), View), Projection);
	output.PosW = mul(input.Pos, World);
	
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
float4 PS( PS_INPUT input) : SV_Target
{
	return SurfaceTexture.Sample(SurfelFilter, input.UV) * Light_OrenNayer(input.NormalW.xyz, input.EyeVect, input.LightDir);//float4(ParallelLight(input.PosW.xyz, input.NormalW.xyz), 1.0f);
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
		SetRasterizerState(SURFEL);
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
		SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
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
		SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}
