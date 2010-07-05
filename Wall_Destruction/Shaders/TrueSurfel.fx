	#include "Math.inc"
	#include "GlobalParameters.inc"
	#include "OrenNayerLighting.inc"

	Texture2D SurfelTexture;
	Texture2D PlaneTexture;

	// map for depth checking
	Texture2D DepthMap;

	// map to check for occlusions between surfels
	Texture2D OcclusionMap;
		
	float4 vp;
	float4 zb;
	float depthOffset;

	float4 Color;
	float scalingFactor;
	
	float4x4 mvInvTrans;

	SamplerState SurfelFilter
	{
		Filter = MIN_MAG_MIP_LINEAR;
		AddressU = WRAP;
		AddressV = WRAP;
	};

	struct VS_SURFEL_INPUT{
	/*	float4 Pos : POSITION;
		float4 Normal : NORMAL;
		float2 UV : TEXCOORD0;
		float2 Ratio : TEXCOORD1;
		float Radius: TEXCOORD2;
		*/
		float4 Pos : POSITION;
		float4 Normal : NORMAL;
		float4 MajorRadius: TEXCOORD0;
		float4 MinorRadius: TEXCOORD1;

	};

	struct GS_SURFEL_INPUT{	
		float4 Pos : POSITION;
		float4 Normal : NORMAL;
		float4 MajorRadius: TEXCOORD0;
		float4 MinorRadius: TEXCOORD1;
/*		float4 Pos : POSITION;
		float4 Normal : NORMAL;	
		float2 UV : TEXCOORD0;
		float2 Ratio : TEXCOORD1;
		float Radius: TEXCOORD2;*/
	};

	struct PS_SURFEL_PRERENDER_INPUT{
		float4 Pos : SV_POSITION;
		float Alpha : TEXCOORD1;
		float2 UV : TEXCOORD3;
		float Size: SIZE;
		float4 Center: TEXCOORD5;
		float4 NormalOS : NORMAL;
		float4 NormalVS : NORMAL1;
	};

	struct PS_SURFEL_INPUT{
		float4 PosSS : SV_POSITION;
		float2 UV : TEXCOORD0;
		float4 PosVS : TEXCOORD1;
		float4 OtherPosSS: TEXCOORD2;
		float Size: TEXCOORD3;
		float4 NormalVS : TEXCOORD4;
		float4x4 uAndvAndCenterVSAndCenterSS : TEXCOORD5;			
	};

	struct PS_OUT_DEPTH{
		float4 Color : SV_TARGET0;
		float4 Depth : SV_TARGET1;
		float4 Normal : SV_TARGET2;
	};

	//
	// Vertex Shader
	//
	GS_SURFEL_INPUT VS_Surfel( VS_SURFEL_INPUT input) 
	{
		GS_SURFEL_INPUT output = (GS_SURFEL_INPUT) 0;
		
		output.Pos = input.Pos;	
		output.Normal = input.Normal;
		output.MajorRadius = input.MajorRadius * scalingFactor;
		output.MinorRadius = input.MinorRadius * scalingFactor;
		
		return output;
	}

	//
	// Takes in a point and output a single quad
	//	
	[maxvertexcount(4)]
	void GS_Surfel_Prerender(point GS_SURFEL_INPUT input[1], inout TriangleStream<PS_SURFEL_PRERENDER_INPUT> surfelStream, uniform bool useWVP)
	{
	    PS_SURFEL_PRERENDER_INPUT output;
	    
	    float4 NormalVS = normalize(mul(mul(float4(input[0].Normal.xyz, 0.0f), World), View));
	    float4 NormalWS = normalize(mul(float4(input[0].Normal.xyz, 0.0f), World));
	    float4 cameraDir= normalize(mul(input[0].Pos, World) - float4(CameraPos, 1.0f));
	    // is the surfel facing the camera?
	    float cosangle = acos(dot(NormalWS, cameraDir)/(length(NormalWS)*length(cameraDir)));	    
	    float sinangle = asin(dot(NormalWS, cameraDir)/(length(NormalWS)*length(cameraDir)));	    
	 
	    if(!(cosangle <= 3.14f*0.5f && sinangle <= 3.14f*0.5f)){
		
			float xMultiplier[] = {-1.0, -1.0, 1.0, 1.0};
			float yMultiplier[] = {-1.0, 1.0, -1.0, 1.0};
		
			float U[] = {0.0, 0.0, 1.0, 1.0};
			float V[] = {0.0, 1.0, 0.0, 1.0};
		    
			float size = 2 * max(length(input[0].MajorRadius), length(input[0].MinorRadius)) * (0.001f/CameraPos.z);
				
			if(size < 2.0f)
				size = 2.0f;
			
			float4 NormalOS = normalize(mul(float4(input[0].Normal.xyz, 0.0f), World));
			
			float4 Center = mul( mul( input[0].Pos, World), View); // center
		    
			[unroll]
			for(int i=0; i<4; i++)
			{	
				float4 originalPos = float4(input[0].Pos.xyz + xMultiplier[i] * input[0].MajorRadius.xyz + yMultiplier[i] * input[0].MinorRadius.xyz, 1.0f);
				output.Pos = mul( mul( mul( originalPos, World), View), Projection);
				output.Alpha = 1-mul(mul(originalPos, World), View).z / 1000.0f;	
				output.UV = float2(U[i], V[i]);	
				
				output.NormalOS = NormalOS;
				output.NormalVS = NormalVS;
				output.Center = Center;
				output.Size = size;
							
				surfelStream.Append(output);
			}
		}
		
		surfelStream.RestartStrip();
	}
	
	[maxvertexcount(4)]
	void GS_Surfel(point GS_SURFEL_INPUT input[1], inout TriangleStream<PS_SURFEL_INPUT> surfelStream, uniform bool useWVP)
	{
	    PS_SURFEL_INPUT output;
	    
	    float4 NormalVS = normalize(mul(mul(float4(input[0].Normal.xyz, 0.0f), World), View));
	    float4 NormalWS = normalize(mul(float4(input[0].Normal.xyz, 0.0f), World));
	    float4 cameraDir= normalize(mul(input[0].Pos, World) - float4(CameraPos, 1.0f));
	    // is the surfel facing the camera?
	    float cosangle = acos(dot(NormalWS, cameraDir)/(length(NormalWS)*length(cameraDir)));	    
	    float sinangle = asin(dot(NormalWS, cameraDir)/(length(NormalWS)*length(cameraDir)));	    
	 
	    if(!(cosangle <= 3.14f*0.5f && sinangle <= 3.14f*0.5f)){
			
			float xMultiplier[] = {-1.0, -1.0, 1.0, 1.0};
			float yMultiplier[] = {-1.0, 1.0, -1.0, 1.0};
		
			float U[] = {0.0, 0.0, 1.0, 1.0};
			float V[] = {0.0, 1.0, 0.0, 1.0};
		    
			float size = 2 * max(length(input[0].MajorRadius), length(input[0].MinorRadius)) * (0.001f/CameraPos.z);
				
			if(size < 2.0f)
				size = 2.0f;
			
			float4 NormalOS = normalize(mul(float4(input[0].Normal.xyz, 0.0f), World));
			
			float4 CenterVS = mul( mul( input[0].Pos, World), View); // center
			float4 CenterSS = mul( mul( mul( input[0].Pos, World), View), Projection); // center
			[unroll]
			for(int i=0; i<4; i++)
			{					
				float4 originalPos;
						
				originalPos = float4(input[0].Pos.xyz + xMultiplier[i] * input[0].MajorRadius.xyz + yMultiplier[i] * input[0].MinorRadius.xyz, 1.0f);
				output.PosSS = mul( mul( mul( originalPos, World), View), Projection);
				
				output.UV = float2(U[i], V[i]);
			
				output.uAndvAndCenterVSAndCenterSS[0] = mul(input[0].MajorRadius, mvInvTrans); // u
				output.uAndvAndCenterVSAndCenterSS[1] = mul(input[0].MinorRadius, mvInvTrans); // v
				output.uAndvAndCenterVSAndCenterSS[2] = CenterVS;
				output.uAndvAndCenterVSAndCenterSS[3] = CenterSS;
				
				output.Size = size;
				
				output.PosVS = mul(mul(originalPos, World), View);
				output.NormalVS = NormalVS;
				output.OtherPosSS = output.PosSS;
		
				surfelStream.Append(output);
			}   
		}
	    
		surfelStream.RestartStrip();
	    
	}
	//
	// Takes in a point and output a single quad
	//
	/*
	[maxvertexcount(4)]
	void GS_Surfel(point GS_SURFEL_INPUT input[1], inout TriangleStream<PS_SURFEL_INPUT> surfelStream, uniform bool useWVP)
	{
		PS_SURFEL_INPUT output;
	    
		float3 tan1 = normalize(Perpendicular( input[0].Normal.xyz));
		float3 tan2 = normalize(cross(input[0].Normal.xyz, tan1.xyz));

		float xMultiplier[] = {-1.0, -1.0, 1.0, 1.0};
		float yMultiplier[] = {-1.0, 1.0, -1.0, 1.0};

		float U[] = {-1.0, -1.0, 1.0, 1.0};
		float V[] = {-1.0, 1.0, -1.0, 1.0};
	    
		[unroll]
		for(int i=0; i<4; i++)
		{		
			float4 originalPos;
					
			originalPos = float4(input[0].Pos.xyz + xMultiplier[i] * tan2 * input[0].Radius*2 + yMultiplier[i] * tan1 * input[0].Radius*2, 1.0f);
			output.PosSS = mul( mul( mul( originalPos, World), View), Projection);
			
			output.UV = float2(U[i]*0.5f + 0.5f, V[i]*0.5f + 0.5f);
		
			output.uAndvAndCenterVSAndCenterSS[0] = float4(tan1, 1.0f); // u
			output.uAndvAndCenterVSAndCenterSS[1] = float4(tan2, 1.0f); // v
			output.uAndvAndCenterVSAndCenterSS[2] = mul( mul( input[0].Pos, World), View); // CenterVS 
			output.uAndvAndCenterVSAndCenterSS[3] = mul( mul( mul( input[0].Pos, World), View), Projection); // CenterSS 
			output.uAndvAndCenterVSAndCenterSS[3] /= output.uAndvAndCenterVSAndCenterSS[3].w;
			
			
			output.PosVS = mul(mul(originalPos, World), View);
			output.OtherPosSS = output.PosSS;
			output.OtherPosSS /= output.OtherPosSS.w;
			
			surfelStream.Append(output);
		}

		surfelStream.RestartStrip();
	}
*/
	//
	// Pixel Shader
	//
	PS_OUT_DEPTH PS_Surfel_Prerender( PS_SURFEL_PRERENDER_INPUT input) 
	{
		PS_OUT_DEPTH depthOut = (PS_OUT_DEPTH) 0;
		float4 imageColor = SurfelTexture.Sample(SurfelFilter, input.UV);
		depthOut.Color = PlaneTexture.Sample(SurfelFilter, input.UV) * imageColor;
		depthOut.Normal = abs(input.NormalOS*0.1f);
				
		float3 qn = (input.Pos * vp.xxxx + vp.zwyy).xyz;
		
		//float3 q = qn * dot(input.Center.xyz, input.NormalVS.xyz) / dot(qn, input.NormalVS.xyz);
	
		//float depth = (1.0f / q.z )*((nearFar.x * nearFar.y)/(nearFar.y - nearFar.x))+ nearFar.y/(nearFar.y - nearFar.x);
		// calculate q, komið í R1
		float3 q = qn.xyz * rcp(dot(qn, input.NormalVS.xyz)) - input.Center.xyz;
		
		// Per pixel depth correction (including depth-offset)
		qn.z = q.z - depthOffset;		
		qn.z = rcp(qn.z);		
		float depth = zb.y * qn.z + zb.x;
		//float depth = input.Alpha;
		depthOut.Depth = float4(depth, depth, depth, 1.0f)  ;
	    
		return depthOut;
	}

	float4 PS_Surfel( PS_SURFEL_INPUT input) : SV_TARGET
	{   
		float3 qn = (input.PosSS * vp.xxxx + vp.zwyy).xyz;
		
		float3 q = qn.xyz * rcp(dot(qn, input.NormalVS.xyz)) - input.uAndvAndCenterVSAndCenterSS[2].xyz;
	
		// screen space 
		float w2d = pow(distance(input.PosSS, input.uAndvAndCenterVSAndCenterSS[3]) / (input.Size/2.0f), 2);
		float3 pixelPosMinusPixelSurfelCenter = q - input.uAndvAndCenterVSAndCenterSS[3].xyz;
	     
		// get the position of the pixel
		// code taken from http://mynameismjp.wordpress.com/2009/05/05/reconstructing-position-from-depth-continued/
	/*	float3 pixelCoord = input.PosVS.xyz * (1000/-input.PosVS.z);
		pixelCoord = DepthMap.Sample(SurfelFilter, input.UV).x * input.PosVS.xyz;
	  */  
		float U = dot(input.uAndvAndCenterVSAndCenterSS[0].xyz , pixelPosMinusPixelSurfelCenter);
		float V = dot(input.uAndvAndCenterVSAndCenterSS[1].xyz , pixelPosMinusPixelSurfelCenter);
						
		float w3d = U*U + V*V;
	    
		float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);	    
	    
		if(min(w3d, w2d) <= 1.0f)
		{
			/*color.r = 0.8f * exp(-pow(min(w3d, w2d), 2.0f)/ (2.0f*pow(4.4f, 2.0f)));
			color.g = color.r;
			color.b = color.r;
			color.a = 1.0f;
			*/
			color = PlaneTexture.Sample(SurfelFilter, input.UV) * SurfelTexture.Sample(SurfelFilter, input.UV) * Color;
		}
		else{
			clip(-1);
		}

		return color;
		
	}

	technique10 SurfelTechniqueFirstPass
	{
		pass P0
		{
			SetVertexShader( CompileShader( vs_4_0, VS_Surfel() ) );
			SetGeometryShader ( CompileShader( gs_4_0, GS_Surfel_Prerender(true) ) );
			SetPixelShader( CompileShader( ps_4_0, PS_Surfel_Prerender() ) );
			SetDepthStencilState( DisableDepth, 0 );
			SetRasterizerState(SURFEL_DEPTH);
 			SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		}
	}

	technique10 SurfelTechniqueSecondPass
	{
		pass P0{
			SetVertexShader( CompileShader( vs_4_0, VS_Surfel() ) );
			SetGeometryShader ( CompileShader( gs_4_0, GS_Surfel(true) ) );
			SetPixelShader( CompileShader( ps_4_0, PS_Surfel () ) );
			SetDepthStencilState( SamePlaneDepth, 0 );
			SetRasterizerState(SURFEL_DEPTH);
 			SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		}
	}
