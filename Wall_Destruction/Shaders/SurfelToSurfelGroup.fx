#include "Math.inc"
#include "GlobalParameters.inc"
#include "OrenNayerLighting.inc"

// TEMP
Texture2D SurfelTexture;

SamplerState SurfelFilter
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
// TEMP ENDS

struct VS_SURFEL_INPUT{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float2 Dimensions: TEXCOORD0;
	float Radius: TEXCOORD1;
};

struct GS_SURFEL_INPUT{	
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float2 Dimensions: TEXCOORD0;
	float Radius: TEXCOORD1;
};

struct PS_SURFEL_INPUT{
	float4 Pos : SV_POSITION;
	float4 Normal : NORMAL;
	float2 UV : TEXCOORD0;
	float2 Ratio: TEXCOORD1;
	float Radius: TEXCOORD2;
};

struct PS_INPUT{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;
	/*float4 PosW : POSITION;
	float4 NormalW : NORMAL;
	float3 LightDir : TEXCOORD2;
	float3 EyeVect: TEXCOORD1;*/
};

//
// Vertex Shader
//
GS_SURFEL_INPUT VS_Surfel( VS_SURFEL_INPUT input) 
{
	GS_SURFEL_INPUT output = (GS_SURFEL_INPUT) 0;
	output.Pos = input.Pos;
	output.Normal = input.Normal;
	output.Radius = input.Radius;
	output.Dimensions = input.Dimensions;

    return output;
}

//
// Takes in a surface and returns a surfel group
//
[maxvertexcount((MAX_LEVEL + 1) * (MAX_LEVEL + 1) - 1)]
void GS_Surfel(point GS_SURFEL_INPUT input[1], inout TriangleStream<PS_INPUT> surfelStream)
{
    PS_SURFEL_INPUT output;
    
    float3 tan1 = normalize(Perpendicular( input[0].Normal.xyz));
	float3 tan2 = normalize(cross(input[0].Normal.xyz, tan1.xyz));

	float xMultiplier[] = {-1.0f, -1.0f, 1.0f, 1.0f};
	float yMultiplier[] = {-1.0f, 1.0f, -1.0f, 1.0f};
	
	uint U[] = {-0.5f, -0.5f, 0.5f, 0.5f};
	uint V[] = {0.5f, -0.5f, 0.5f, -0.5f};
	
    float width, height;
    width = input[0].Dimensions.x / input[0].Radius;
    height = input[0].Dimensions.y / input[0].Radius;
    float2 ratio = float2(1.0f/(width-1), 1.0f/(height-1));
    
    for(int i = 0; i<width; i++){
		for(int j = 0; j<height; j++){
			output.Pos = float4(input[0].Pos.xyz + xMultiplier[i] * input[0].Radius * tan2 + yMultiplier[i] * input[0].Radius * tan1, 1.0f);			
			
			output.Normal = normalize(input[0].Normal);
						
			output.UV = float2(i * ratio.x, j * ratio.y);
			output.Radius = input[0].Radius;
			output.Ratio = ratio;
			
			for(int k = 0; k<4; k++){
				PS_INPUT ps_output = (PS_INPUT) 0;
				
				ps_output.Pos = float4(output.Pos.xyz + 0.5f*xMultiplier[i] * output.Radius * tan2 + 0.5f*yMultiplier[i] * output.Radius * tan1, 1.0f);
				
				ps_output.Pos = mul( mul( mul( ps_output.Pos, World), View), Projection);
				/*
				ps_output.PosW = mul(output.Pos, World);
				
				float4x4 tmp = World;
				tmp[3][0] = 0.0f;
				tmp[3][1] = 0.0f;
				tmp[3][2] = 0.0f;
				
				ps_output.NormalW = normalize(mul(output.Normal, tmp));
				
				// Determine the light direction vector. This assumes that the vector
				// is constant relative to the camera.
				ps_output.LightDir = -normalize(float4(LightDirection, 1.0f)).xyz;
				
				// Determine the eye vector for the light
				ps_output.EyeVect = -normalize(float4(CameraPos, 1.0f) + output.Pos).xyz;
*/
				ps_output.UV = float2(output.UV.x + U[i]*ratio.x, output.UV.y + V[i]*ratio.y);
				
				surfelStream.Append(ps_output);
			}
			
		}
    }    
	
    surfelStream.RestartStrip();
}

//
// Pixel Shader
//
float4 PS_Surfel( PS_INPUT input) : SV_Target
{
	return SurfelTexture.Sample(SurfelFilter, input.UV);
}

//GeometryShader gsStreamOut = ConstructGSWithSO( CompileShader( gs_4_0, GS_Surfel() ), "SV_POSITION.xyz; NORMAL.xyz; TEXCOORD0.xy; TEXCOORD1.xy; TEXCOORD2.x" );

technique10 MRT
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Surfel() ) );
		SetGeometryShader ( CompileShader( gs_4_0, GS_Surfel() ));
	    SetPixelShader( CompileShader( ps_4_0, PS_Surfel() ) );

		SetDepthStencilState( DisableDepth, 0 );
		SetRasterizerState(SOLID);
	}
}