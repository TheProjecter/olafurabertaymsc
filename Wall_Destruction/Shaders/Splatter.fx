	#include "Math.inc"
	#include "GlobalParameters.inc"
	#include "OrenNayerLighting.inc"
	
	float F; // the size of the reconstruction filter, usually 2.0f
	
	float conditionThreshold // ill-conditioning threshhold usually 100.0f
	float prefilterSize; // the size of the prefilter, usually 1.0f
	float focus; // the distance of the focal plane
	float aperture; // the aperture of the lens, default value is 2.8f;
	float imagePlane = 1.0f;
	int subPixels; // the number of sampler per pixel
	
	float4x4 objectToScreen; // mapping from object-space to screen-space
	float2 vpScale; // the scaling factor from homogenous screen-space to the viewport
	float2 vpMid; // the coordinates of the viewport origins
	int scissorXMin;
	int scissorXMax;
	int scissorYMin;
	int scissorYMax;
	
	int clipPlanes = 6;
	float4 clipOrigin[clipPlanes];
	float4 clipNormal[clipPlanes];
	
	// original
	// int expTableSize; // 256 works well
	// float expTable[expTableSize]; // exp(-0.5f * F * float(index)/float(exptablesize))
	// I think this works as well
	Texture2D SurfelTexture;
	SamplerState SurfelFilter
	{
		Filter = MIN_MAG_MIP_LINEAR;
		AddressU = WRAP;
		AddressV = WRAP;
	};
	
	// a set of poisson distributed samples locations
	int jitterPatterns; // 64 distributions are usually enough
	int preFilterSelection[selSize * selSize]; // hmm .... texture???
	int dofFilterSelection[selSize * selSize]; // hmm .... texture???
	
	// constants
	float focalLength = imagePlane * focus / (imagePlane + focus);
	float lensRadius = focalLength * 0.5f/aperture;
	float expTableScale = float(expTableSize)/F;
	float2 oovpScale = float2(1.0f/vpScale.x, 1.0f/vpScale.y);
	
	float2x2 VP = float2x2(prefilterSize * oovpScale.x, 0, 
							0, prefilterSize * oovpScale.y);
							
	float4x4 screenToObject = Invert(objectToScreen);
	float4 c = screenToObject.z;
	
	if(c.w != 0.0f) c /= c.w;
	else c = -c;
	
	struct VS_INPUT
	{ 
		float4 Center : POSITION0;
		float4 Normal : NORMAL; // normal
		float4 Major : TANGENT0;  // one tangent
		float4 Minor : TANGENT1;  // other tangent		
		float size : TEXCOORD0; // splat size, length to neighbor
	};
	
	struct GS_INPUT
	{		
		float4 Center : POSITION0;
		float4 Normal : NORMAL; // normal
		float4 Major : TANGENT0;  // one tangent
		float4 Minor : TANGENT1;  // other tangent	
		float size : TEXCOORD0; // splat size, length to neighbor
	};
	
	struct PS_INPUT
	{
		
	};


	//
	// Vertex Shader
	//
	GS_INPUT VS_Surfel( VS_SURFEL_INPUT input) 
	{
		GS_INPUT output = (GS_INPUT) 0;
		output.Center = input.Center;
		output.Normal = input.Normal;
		output.Major = input.Major;
		output.Minor = input.Minor;
		output.size = input.size;
		
		return output;		
	}

	//
	// Takes in a point and output a single quad
	//	
	[maxvertexcount(4)]
	void GS_Surfel_Prerender(point GS_SURFEL_INPUT input[1], inout TriangleStream<PS_SURFEL_PRERENDER_INPUT> surfelStream, uniform bool useWVP)
	{
		float4 cull = float4(input[0].Normal.x, input[0].Normal.y, input[0].Normal.z, -dot(input[0].Position, input[0].Normal));

		// if negative then back facing
		if(dot(c, cull) > 0.0f)
		{
			// calculate the size matrix in CPU
			float2x2 Vr = float2x2(intput[0].size, 0, 0, input[0].size); 
			// calculate the inverse of this matrix in CPU
			float2x2 Q = Invert(Vr * transpose(Vr));
			
			// make the tangent plane (in object-space)
			float3 tu = input[0].Major.xyz;
			float3 tv = input[0].Minor.xyz;
			
			// transform the tangents and the splat center to screen-space
			float4 tuh = float4(tu.x, tu.y, tu.z, 0);
			float4 tvh = float4(tv.x, tv.y, tv.z, 0);
			float4 p0h = float4(p0.x, p0.y, p0.z, 1);
			tuh = mul(tuh, objectToScreen);
			tvh = mul(tvh, objectToScreen);
			p0h = mul(p0h, objectToScreen);
			float detQ = determinant(Q); 
								
			// view frustum culling
			for(int cp=0;cp<clipPlanes;cp++) {
				float a = dot(tuh, clipNormal[cp]);
				float b = dot(tvh, clipNormal[cp]);
				float c = dot(p0h, clipNormal[cp]) - Dot(clipNormal[cp], clipOrigin[cp]);
				if( a == 0.0f && b == 0.0f ) { // the planes are parallel
					if( c < 0.0f ) break; // behind the clip plane
				} else { // if behind the plane, discard
					if( c < 0.0f &&					
						F * (Q[0][0]*b*b - 2.0f*Q[0][1]*a*b + Q[1][1]*a*a) - c*c*detQ < 0.0f )
						break;
				}
			}
			// the splat was not behind any plane
			if( cp >= clipPlanes ){
				// mapping from the splat to screen-space
				float3x3 M = float3x3(tuh.x, tuh.y, tuh.w,
					tvh.x, tvh.y, tvh.w,
					p0h.x, p0h.y, p0h.w);
				float detM = determinant(M);
				
				// mapping from screen-space to the splat
				float3x3 Mi = float3x3( M[1][1]*M[2][2] - M[2][1]*M[1][2],
					M[2][1]*M[0][2] - M[0][1]*M[2][2],
					M[0][1]*M[1][2] - M[1][1]*M[0][2],
					M[2][0]*M[1][2] - M[1][0]*M[2][2],
					M[0][0]*M[2][2] - M[2][0]*M[0][2],
					M[1][0]*M[0][2] - M[0][0]*M[1][2],
					M[1][0]*M[2][1] - M[2][0]*M[1][1],
					M[2][0]*M[0][1] - M[0][0]*M[2][1],
					M[0][0]*M[1][1] - M[1][0]*M[0][1]); // inverse of M * Det(M)

				float3x3 Qh = float3x3( Q[0][0], Q[0][1], 0,
					Q[1][0], Q[1][1], 0,
					0, 0, -F );
					
				float3x3 Qhs = Mi * Qh * transpose(Mi);
			}				
		}
		
	}
	
	//
	// Pixel Shader
	//
	PS_OUT_DEPTH PS_Surfel_Prerender( PS_SURFEL_PRERENDER_INPUT input) 
	{
	
	}


	technique10 SurfelTechniqueFirstPass
	{
		pass P0
		{
			SetVertexShader( CompileShader( vs_4_0, VS_Surfel() ) );
			SetGeometryShader ( CompileShader( gs_4_0, GS_Surfel_Prerender(true) ) );
			SetPixelShader( CompileShader( ps_4_0, PS_Surfel_Prerender() ) );
			SetDepthStencilState( SamePlaneDepth, 0 );
			SetRasterizerState(SURFEL_DEPTH);
 			SetBlendState( AlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		}
	}
	