	#include "Math.inc"
	#include "GlobalParameters.inc"
	#include "OrenNayerLighting.inc"

	Texture2D SurfelTexture;

	// map for depth checking
	Texture2D DepthMap;

	// map to check for occlusions between surfels
	Texture2D OcclusionMap;
	
	float2 nearPlaneDimensions;
	float2 nearFar;
	float2 viewPort;

	float4 vp;
	float4 zb;
	float depthOffset;
	
	float4 Color;
	float scalingFactor;
	
	float3 sizeStuff;

	SamplerState SurfelFilter
	{
		Filter = MIN_MAG_MIP_LINEAR;
		AddressU = WRAP;
		AddressV = WRAP;
	};

	struct VS_SURFEL_INPUT_FP{
		float4 Center : POSITION;
		float4 Normal : NORMAL;
		float4 MajorRadius: TEXCOORD0;
		float4 MinorRadius: TEXCOORD1;
	};

	struct GS_SURFEL_INPUT_FP{	
		float4 Center : POSITION;
		float4 Normal : NORMAL;
		float4 MajorRadius: TEXCOORD0;
		float4 MinorRadius: TEXCOORD1;
	};

	struct PS_SURFEL_INPUT_FP{
		float4 PosSS : SV_POSITION;
		float4 CenterInEye : POSITION0;
		float4 NormalInEye : NORMAL0;
		float2 UV : TEXCOORD0;
		float4 MajorRadiusTransformed: TEXCOORD1;
		float4 MinorRadiusTransformed: TEXCOORD2;		
	};

	struct PS_SURFEL_INPUT_SP{
		float4 PosSS : SV_POSITION;
		float2 UV : TEXCOORD0;
		float4 PosVS : TEXCOORD1;
		float4 OtherPosSS: TEXCOORD2;
		float Size: TEXCOORD3;
		float4 NormalVS : TEXCOORD4;
		float4x4 uAndvAndCenterVSAndCenterSS : TEXCOORD5;			
	};

	struct PS_OUT_DEPTH{
		float4 Color : SV_Target;
		float4 Depth : SV_Target1;
		float4 Normal : SV_Target2;
	};
	
	//
	// Vertex Shader
	//
	GS_SURFEL_INPUT_FP VS_Surfel_FirstPass( VS_SURFEL_INPUT_FP input) 
	{
		GS_SURFEL_INPUT_FP output = (GS_SURFEL_INPUT_FP) 0;

		output.Center = input.Center;
		output.Normal = input.Normal;
		output.MajorRadius = input.MajorRadius;
		output.MinorRadius = input.MinorRadius;

		return output;
	}
	
	[maxvertexcount(4)]
	void GS_Surfel_FirstPass(point GS_SURFEL_INPUT_FP input[1], inout TriangleStream<PS_SURFEL_INPUT_FP> surfelStream, uniform bool useWVP)
	{
	    PS_SURFEL_INPUT_FP output;

		float xMultiplier[] = {-1.0, -1.0, 1.0, 1.0};
		float yMultiplier[] = {-1.0, 1.0, -1.0, 1.0};

		float U[] = {0.0, 0.0, 1.0, 1.0};
		float V[] = {0.0, 1.0, 0.0, 1.0};
		
		for(int i = 0; i<4; i++){
		
			output.CenterInEye = mul( mul( input[0].Center, World), View);
			output.CenterInEye.w = 0.0f;
			
			float CenterDotNormal = dot(output.CenterInEye.xyz, input[0].Normal.xyz);
			output.NormalInEye = input[0].Normal / CenterDotNormal;
		
			if(CenterDotNormal <= 3.14f){
				output.PosSS = mul(mul(mul(input[0].Center + xMultiplier[i] * input[0].MajorRadius + yMultiplier[i] * input[0].MinorRadius, World), View), Projection);
				output.UV = float2(U[i], V[i]);
 			
				// Transform U
				output.MajorRadiusTransformed = mul(mul(mul(input[0].MajorRadius, World), View), Projection);
				output.MajorRadiusTransformed  *= sizeStuff.z;
				
				// Transform V
				output.MinorRadiusTransformed = mul(mul(mul(input[0].MinorRadius, World), View), Projection);//mul(input[0].MinorRadius, WVInv);
				output.MinorRadiusTransformed *= sizeStuff.z;
				
				//float invU = 1.0f / dot(input[0].MajorRadius.xyz, input[0].MajorRadius.xyz);
				//float invV = 1.0f / dot(input[0].MinorRadius.xyz, input[0].MinorRadius.xyz);
				
				//float r = max(invU, invV);
				
			
				surfelStream.Append(output);			
			}				
		}		
	   
		surfelStream.RestartStrip();
	}

	//
	// Pixel Shader
	//
	PS_OUT_DEPTH PS_Surfel_FirstPass( PS_SURFEL_INPUT_FP input) 
	{	
		PS_OUT_DEPTH output = (PS_OUT_DEPTH) 0;		
		
		// calculate qn	komið í R0
		float3 qn = (input.PosSS * vp.xxxx + vp.zwyy).xyz;
		
		// calculate q, komið í R1
		float3 q = qn.xyz * rcp(dot(qn, input.NormalInEye.xyz)) - input.CenterInEye.xyz;
		
		// Per pixel depth correction (including depth-offset)
		qn.z = q.z - depthOffset;		
		qn.z = 1/qn.z;		
		output.Depth = float4(zb.y * qn.z + zb.x, zb.y * qn.z + zb.x, zb.y * qn.z + zb.x, 1.0f);
		
		// Orthogonal projection on splat wrt. local tangent frame, yielding (u,v)
		float2 uv = float2(dot(q, input.MajorRadiusTransformed.xyz), dot(q, input.MinorRadiusTransformed.xyz));
		
		output.Normal = abs(mul(input.NormalInEye, Projection));
		
		output.Color = SurfelTexture.Sample(SurfelFilter, input.UV) * Color;
		
		float sqDistance = dot(uv, uv);
		output.Color *= sqDistance * (-zb.w) + zb.w;
		
		return output;		
	}

	technique10 SurfelTechniqueFirstPass
	{
		pass P0
		{
			SetVertexShader( CompileShader( vs_4_0, VS_Surfel_FirstPass() ) );
			SetGeometryShader ( CompileShader( gs_4_0, GS_Surfel_FirstPass(true) ) );
			SetPixelShader( CompileShader( ps_4_0, PS_Surfel_FirstPass() ) );
			SetDepthStencilState( SamePlaneDepth, 0 );
			SetRasterizerState(SURFEL_DEPTH);
 			SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		}
	}

	technique10 SurfelTechniqueSecondPass
	{
		pass P0{
			SetVertexShader( CompileShader( vs_4_0, VS_Surfel_FirstPass() ) );
			SetGeometryShader ( CompileShader( gs_4_0, GS_Surfel_FirstPass(true) ) );
			SetPixelShader( CompileShader( ps_4_0, PS_Surfel_FirstPass() ) );
			SetDepthStencilState( SamePlaneDepth, 0 );
			SetRasterizerState(SURFEL_DEPTH);
 			SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		}
	}
