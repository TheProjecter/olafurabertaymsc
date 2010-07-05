#include "Math.inc"
#include "GlobalParameters.inc"
#include "OrenNayerLighting.inc"

float RadiusScale;

Texture2D SurfelTexture;
Texture2D PlaneTexture;
Texture2D ExpTableTexture;

float4x4 InvProj;

float4 vp;
float4 ViewPort; // n, t, b, hvp

SamplerState SurfelFilter
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

// depth pass
struct Depth_Pass_VS_Input{
	float4 Center : Position;
	float4 Normal : Normal;
	float4 MajorRadius : Tangent;
	float4 MinorRadius : Tangent1;
};

struct Depth_Pass_GS_Input{
	float4 Center : Position;
	float4 Normal : Normal;
	float4 MajorRadius : Tangent;
	float4 MinorRadius : Tangent1;
};

struct Depth_Pass_PS_Input{
	float4 Position : SV_Position;
	float Depth : Depth;
};

struct Depth_Pass_PS_Output{
	float4 Depth : SV_Target;
};

Depth_Pass_GS_Input Depth_Pass_VS(Depth_Pass_VS_Input vsIn){
	
	Depth_Pass_GS_Input gsIn = (Depth_Pass_GS_Input) 0;
	
	gsIn.Center = vsIn.Center;
	gsIn.Normal = vsIn.Normal;
	gsIn.MajorRadius = vsIn.MajorRadius;
	gsIn.MinorRadius = vsIn.MinorRadius;
	
	return gsIn;
}

[maxvertexcount(4)]
void Depth_Pass_GS(point Depth_Pass_GS_Input input[1], inout TriangleStream<Depth_Pass_PS_Input> depthPassStream){
	
	Depth_Pass_PS_Input output;
	    
	float xMultiplier[] = {-1.0, -1.0, 1.0, 1.0};
	float yMultiplier[] = {-1.0, 1.0, -1.0, 1.0};
		
	float U[] = {0.0, 0.0, 1.0, 1.0};
	float V[] = {0.0, 1.0, 0.0, 1.0};
	
	[unroll]
	for(int i=0; i<4; i++)
	{					
		float4 originalPos;
				
		originalPos = float4(input[0].Center.xyz + xMultiplier[i] * input[0].MajorRadius.xyz * RadiusScale + yMultiplier[i] * input[0].MinorRadius.xyz * RadiusScale, 1.0f);
		output.Position = mul( originalPos, WVP);
	
		output.Depth = /*1 - */output.Position.z /*/output.Position.w*/;
		depthPassStream.Append(output);
	}   
	    
	depthPassStream.RestartStrip();
}

Depth_Pass_PS_Output Depth_Pass_PS(Depth_Pass_PS_Input psIn){
	
	Depth_Pass_PS_Output psOut = (Depth_Pass_PS_Output) 0;
	float pixelDepth = GetDepth(psIn.Position);
	psOut.Depth = float4(pixelDepth , pixelDepth , pixelDepth , 1.0f);
	
	return psOut;
}

// Attribute pass

struct Att_Pass_VS_Input{
	float4 Center : Position;
	float4 Normal : Normal;
	float4 MajorRadius : Tangent;
	float4 MinorRadius : Tangent1;
};

struct Att_Pass_GS_Input{
	float4 Center : Position;
	float4 Normal : Normal;
	float4 MajorRadius : Tangent;
	float4 MinorRadius : Tangent1;
};

struct Att_Pass_PS_Input{
	float4 Position : SV_Position;
	float4 Normal : Normal;
	float2 UV : TexCoord0;
	float3 MajorRadius : Tangent0;
	float3 MinorRadius : Tangent1;	
	float3 CenterPosition : Position0;
	float3 ScalingNormal: Normal1;
};

struct Att_Pass_PS_Output{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Weight : SV_Target2;
};

Att_Pass_GS_Input Att_Pass_VS(Att_Pass_VS_Input vsIn){
	
	Att_Pass_GS_Input gsIn = (Att_Pass_GS_Input) 0;
	
	gsIn.Center = vsIn.Center;
	gsIn.Normal = vsIn.Normal;
	gsIn.MajorRadius = vsIn.MajorRadius;
	gsIn.MinorRadius = vsIn.MinorRadius;
	
	return gsIn;
}

[maxvertexcount(4)]
void Att_Pass_GS(point Att_Pass_GS_Input input[1], inout TriangleStream<Att_Pass_PS_Input> attPassStream){
	
	Att_Pass_PS_Input output;
	    
	float xMultiplier[] = {-1.0, -1.0, 1.0, 1.0};
	float yMultiplier[] = {-1.0, 1.0, -1.0, 1.0};
		
	float U[] = {0.0, 0.0, 1.0, 1.0};
	float V[] = {0.0, 1.0, 0.0, 1.0};
	
	float3 nInEye = mul(mul(float4(input[0].Normal.xyz, 0.0f), World), View).xyz;
	float3 scaledNormal = nInEye * rcp(dot(mul(mul(input[0].Center, World), View).xyz, nInEye));
		
	[unroll]
	for(int i=0; i<4; i++)
	{					
		float4 originalPos = float4(input[0].Center.xyz + xMultiplier[i] * input[0].MajorRadius.xyz * RadiusScale + yMultiplier[i] * input[0].MinorRadius.xyz * RadiusScale, 1.0f);
				
		output.Position = mul(originalPos, WVP);
		output.ScalingNormal = scaledNormal;
		output.UV = float2(U[i], V[i]);
		output.Normal = mul(mul(float4(input[0].Normal.xyz, 0.0f), World), View);
				
		output.CenterPosition = mul(input[0].Center, WVP).xyz / mul(input[0].Center, WVP).w;
		output.MajorRadius = mul(mul(float4(input[0].MajorRadius.xyz, 0.0f), World), View).xyz / mul(mul(float4(input[0].MajorRadius.xyz, 1.0f), World), View).w;
		output.MinorRadius = mul(mul(float4(input[0].MinorRadius.xyz, 0.0f), World), View).xyz / mul(mul(float4(input[0].MinorRadius.xyz, 1.0f), World), View).w;
	
		attPassStream.Append(output);
	}   
	    
	attPassStream.RestartStrip();
}

Att_Pass_PS_Output Att_Pass_PS(Att_Pass_PS_Input psIn){
	
	Att_Pass_PS_Output psOut = (Att_Pass_PS_Output) 0;
	
	// check depthMap
    if(IsInFront(psIn.Position)){
		// weigh the attributes		
		float4 pixelPosVP = mul(psIn.Position, InvProj);
		
		float3 qn = float3(pixelPosVP.x*rcp(pixelPosVP.w) * vp.x + vp.z, 
							pixelPosVP.y*rcp(pixelPosVP.w) * vp.x + vp.z, 
							ViewPort[0]);
		
		float3 q = qn * rcp(dot(qn, psIn.ScalingNormal));
		
		float3 r = q - psIn.CenterPosition;
		
		float U = dot(psIn.MajorRadius, r);
		float V = dot(psIn.MinorRadius, r);
				
		float w3d = U*U + V*V;
		float w2d = distance(pixelPosVP.xyz * rcp(pixelPosVP.w), psIn.CenterPosition.xyz) * rcp(length(r));
		w2d = w2d*w2d;
		float w = min(w3d, w2d);
		
		if(w > 1){			
			clip(-1);
		}
		else{
			psOut.Weight = ExpTableTexture.Sample(SurfelFilter, float2(sqrt(w), 0));
			
			psOut.Color = PlaneTexture.Sample(SurfelFilter, psIn.UV) * SurfelTexture.Sample(SurfelFilter, psIn.UV) * psOut.Weight.r;
			psOut.Normal = abs(psIn.Normal) * psOut.Weight.r;			
		}
	}
	else{
		clip(-1);
	}
	
	return psOut;	
}

// normalization pass
struct Norm_Pass_VS_Input{
	float4 Center : Position;
	float4 Normal : Normal;
	float4 MajorRadius : Tangent;
	float4 MinorRadius : Tangent1;
};

struct Norm_Pass_GS_Input{
	float4 Center : Position;
	float4 Normal : Normal;
	float4 MajorRadius : Tangent;
	float4 MinorRadius : Tangent1;
};

struct Norm_Pass_PS_Input{
	float4 Position : SV_Position;
	float4 VertexPosition : Position1;
	float4 CenterPosition : Position2;
	float4 MajorRadius : Tangent;
	float4 MinorRadius : Tangent1;
	float4 Normal : Normal;
	float2 UV : TexCoord0;
};

struct Norm_Pass_PS_Output{
	float4 Color : SV_Target;
};

technique10 DepthPass
{
	pass P0{

		SetVertexShader( CompileShader( vs_4_0, Depth_Pass_VS() ) );
		SetGeometryShader ( CompileShader( gs_4_0, Depth_Pass_GS() ) );
		SetPixelShader( CompileShader( ps_4_0, Depth_Pass_PS () ) );

		/*SetDepthStencilState( SamePlaneDepth, 0 );
		SetRasterizerState(SURFEL_DEPTH);
		SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );*/
		
		SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(SOLID);
 	    SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}

technique10 AttributePass
{	
	pass P0{	
		SetVertexShader( CompileShader( vs_4_0, Att_Pass_VS() ) );
		SetGeometryShader ( CompileShader( gs_4_0, Att_Pass_GS() ) );
		SetPixelShader( CompileShader( ps_4_0, Att_Pass_PS () ) );

		SetDepthStencilState( SamePlaneDepth, 0 );
		SetRasterizerState(SURFEL_DEPTH);
		SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}	
}
/*
technique10 NormalizationPass{
	pass P0{	
		SetVertexShader( CompileShader( vs_4_0, Norm_Pass_VS() ) );
		SetGeometryShader ( CompileShader( gs_4_0, Norm_Pass_GS() ) );
		SetPixelShader( CompileShader( ps_4_0, Norm_Pass_PS () ) );

		SetDepthStencilState( SamePlaneDepth, 0 );
		SetRasterizerState(SURFEL_DEPTH);
		SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}	
}

*/