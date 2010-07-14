#include "Math.inc"
#include "GlobalParameters.inc"
#include "OrenNayerLighting.inc"


Texture2D EWATexture;
Texture2D SurfaceTexture;
float RadiusScale;

float2 DeltaUV;

SamplerState Filter
{
	Filter = ANISOTROPIC;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct Surfel_VS_Input{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float4 MajorRadius: TEXCOORD0;
	float4 MinorRadius: TEXCOORD1;	
	float2 UV : TEXCOORD2;	
};

struct Surfel_Edge_VS_Input{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float4 MajorRadius: TEXCOORD0;
	float4 MinorRadius: TEXCOORD1;	
	float3 ClipPlane : TEXCOORD2;
	float2 UV : TEXCOORD3;	
};

struct Surfel_GS_Input{	
	float4 Pos : POSITION;
	float4 Normal : NORMAL;	
	float4 MajorRadius: TEXCOORD0;
	float4 MinorRadius: TEXCOORD1;
	float2 UV : TEXCOORD2;	
};

struct Surfel_Edge_GS_Input{	
	float4 Pos : POSITION;
	float4 Normal : NORMAL;	
	float4 MajorRadius: TEXCOORD0;
	float4 MinorRadius: TEXCOORD1;
	float3 ClipPlane : TEXCOORD2;
	float2 UV : TEXCOORD3;	
};

struct Surfel_PS_Input{
	float4 Pos : SV_POSITION;
	float4 NormalW : NORMAL;
	float2 UV : TEXCOORD;
	float2 EWAUV: TEXCOORD1;
	float3 EyeVect: TEXCOORD2;
	float3 LightDir : TEXCOORD3;
};

struct Surfel_Edge_PS_Input{
	float4 Pos : SV_POSITION;
	float4 NormalW : NORMAL;
	float2 UV : TEXCOORD;
	float2 EWAUV: TEXCOORD1;
	float3 EyeVect: TEXCOORD2;
	float3 LightDir : TEXCOORD3;
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
	float2 UV : TEXCOORD0;
	float3 EyeVect: TEXCOORD1;
	float3 LightDir : TEXCOORD2;
	float2 EWAUV: TEXCOORD3;
};

struct ClippedSurfelProperties{
	float xMultiplier[7];
	float yMultiplier[7];
	float U[7];
	float V[7];
	int count;
};

// surfel vertex shader
Surfel_GS_Input SurfelVS(Surfel_VS_Input vsIn){
	Surfel_GS_Input vsOut = (Surfel_GS_Input) 0;
	
	vsOut.Pos = vsIn.Pos;
	vsOut.Normal = vsIn.Normal;
	vsOut.MajorRadius = vsIn.MajorRadius;
	vsOut.MinorRadius = vsIn.MinorRadius;
	vsOut.UV = vsIn.UV;
	
	return vsOut;
}

// surfel edge vertex shader
Surfel_Edge_GS_Input SurfelEdgeVS(Surfel_Edge_VS_Input vsIn){
	Surfel_Edge_GS_Input vsOut = (Surfel_Edge_GS_Input) 0;
	
	vsOut.Pos = vsIn.Pos;
	vsOut.Normal = vsIn.Normal;
	vsOut.MajorRadius = vsIn.MajorRadius;
	vsOut.MinorRadius = vsIn.MinorRadius;
	vsOut.ClipPlane = vsIn.ClipPlane;
	vsOut.UV = vsIn.UV;
	
	return vsOut;
}

//
// Takes in a surfel and outputs a single quad
//
[maxvertexcount(4)]
void SurfelGS(point Surfel_GS_Input input[1], inout TriangleStream<Surfel_PS_Input> surfelStream, uniform bool useWVP)
{
    Surfel_PS_Input output;

	float3 calculatedNormal = cross(input[0].MinorRadius.xyz, input[0].MajorRadius.xyz);
	bool facingWrongWay = sign(calculatedNormal.x) != sign(input[0].Normal.x) || sign(calculatedNormal.y) != sign(input[0].Normal.y) || sign(calculatedNormal.z) != sign(input[0].Normal.z);
	
	float xMultiplier[] = {-1.0f, -1.0f, 1.0f, 1.0f};;
	float yMultiplier[] = {-1.0f, 1.0f, -1.0f, 1.0f};
	float U[] = {-1.0f, -1.0f, 1.0f, 1.0f};
	float V[] = {-1.0f, 1.0f, -1.0f, 1.0f};
	
	if(facingWrongWay){
		xMultiplier[1] *= -1.0f;
		xMultiplier[2] *= -1.0f;
		
		yMultiplier[1] *= -1.0f;
		yMultiplier[2] *= -1.0f;

		U[1] *= -1.0f;
		U[2] *= -1.0f;
		
		V[1] *= -1.0f;
		V[2] *= -1.0f;
	}		
    
	[unroll]
    for(int i=0; i<4; i++)
    {		
		float4 originalPos;
				
		if(useWVP){
			originalPos = float4(input[0].Pos.xyz + xMultiplier[i] * input[0].MajorRadius.xyz * RadiusScale + yMultiplier[i] * input[0].MinorRadius.xyz * RadiusScale, 1.0f);
			output.Pos = originalPos;
			output.Pos = mul( mul( mul( output.Pos, World), View), Projection);
		}
		else{
			originalPos = float4(input[0].Pos.xyz + xMultiplier[i] * input[0].MajorRadius.xyz * RadiusScale + yMultiplier[i] * input[0].MinorRadius.xyz * RadiusScale, 1.0f);
			output.Pos = originalPos;			
		}
		
		output.NormalW = normalize(mul(float4(input[0].Normal.xyz, 0), World));
		
		// Determine the light direction vector. This assumes that the vector
		// is constant relative to the camera.
		output.LightDir = -normalize(float4(LightDirection, 1.0f)).xyz;
		
		// Determine the eye vector for the light
		output.EyeVect = -normalize(float4(CameraPos, 1.0f) + originalPos).xyz;

		output.UV = float2(input[0].UV.x + U[i]*DeltaUV.x * RadiusScale, input[0].UV.y + V[i]*DeltaUV.y * RadiusScale);
		output.EWAUV = float2(U[i]*0.5f + 0.5f, V[i]*0.5f + 0.5f);
		surfelStream.Append(output);
    }

    surfelStream.RestartStrip();
}

int GetVertexCount(float3 clipPlanes){
	
	int count = 0;
	
	if(clipPlanes.z == 1 || clipPlanes.x == 0 || clipPlanes.y == 0)
		count = 4;
	else 
		count = 7;
	
	return count;
}

/*
	Clip planes can be
	(1, -1, 0)		(-1, -1, 0)		(1, 1, 0)		(-1, 1, 0)	   (0, 1, 0)		(1, 0, 0)
	 _ _			 _ _				 _			    _			   _
	|  _|			|_	|				| |_		  _| |			  | |			  _ _
	|_| 			  |_|				|_ _|	   	 |_ _|			  |_|			 |_ _|

	(1, -1, 1)		(-1, -1, 1)		(1, 1, 1)		(-1, 1, 1)	   (0, 1, 1)		(1, 0, 1)
	    			 				       _		  _				 _				  _ _
	   _			 _					  |_|		 |_| 			| |				 |_ _|
	  |_| 			|_|   			     	   	     				|_|				 

*/
ClippedSurfelProperties GetProperties(float3 clipPlane, bool facingTheWrongWay){
	
	ClippedSurfelProperties props = (ClippedSurfelProperties) 0;
	
	props.count = GetVertexCount(clipPlane);
	
	if(props.count == 4)
	{		
		if(clipPlane.x == 0 && clipPlane.y == 1){		
			props.U[0] = clipPlane.z == 1 ? 0.0f : -1.0f; props.U[1] = clipPlane.z == 1 ? 0.0f : -1.0f; props.U[2] = clipPlane.z == 1 ? 1.0f : 0.0f;  props.U[3] = clipPlane.z == 1 ? 1.0f : 0.0f;
			props.V[0] = -1.0f; props.V[1] = 1.0f; props.V[2] = -1.0f;  props.V[3] = 1.0f;			
								
			props.xMultiplier[0] = clipPlane.z == 1 ? 0.0f : -1.0f; props.xMultiplier[1] = clipPlane.z == 1 ? 0.0f : -1.0f;
			props.xMultiplier[2] = clipPlane.z == 1 ? 1.0f : 0.0f; props.xMultiplier[3] = clipPlane.z == 1 ? 1.0f : 0.0f;
			
			props.yMultiplier[0] = -1.0f; props.yMultiplier[1] = 1.0f; props.yMultiplier[2] = -1.0f; props.yMultiplier[3] = 1.0f;
			
			if(facingTheWrongWay){
				if(clipPlane.z == 1){
					props.U[1] = 1.0f; 				props.U[2] = 0.0f;
					props.xMultiplier[1] = 1.0f;	props.xMultiplier[2] = 0.0f; 
				}
				else{
					props.U[1] = 0.0f; 				props.U[2] = -1.0f;
					props.xMultiplier[1] = 0.0f;	props.xMultiplier[2] = -1.0f; 
				}
			
				props.V[1] = -1.0f;				props.V[2] = 1.0f; 
				props.yMultiplier[1] = -1.0f;	props.yMultiplier[2] = 1.0f;  
			}
			
		}
		else if(clipPlane.x == 1 && clipPlane.y == 0){		
			props.U[0] = -1.0f; props.U[1] = -1.0f; props.U[2] = 1.0f;  props.U[3] = 1.0f;
			props.V[0] = clipPlane.z == 1 ? 0.0f : -1.0f; props.V[1] = clipPlane.z == 1 ? 1.0f : 0.0f; props.V[2] = clipPlane.z == 1 ? 0.0f : -1.0f;  props.V[3] = clipPlane.z == 1 ? 1.0f : 0.0f;			
			
			props.xMultiplier[0] = -1.0f; props.xMultiplier[1] = -1.0f; props.xMultiplier[2] = 1.0; props.xMultiplier[3] = 1.0f;			
			
			props.yMultiplier[0] = clipPlane.z == 1 ? 0.0f : -1.0f; props.yMultiplier[1] = clipPlane.z == 1 ? 1.0f : 0.0f;
			props.yMultiplier[2] = clipPlane.z == 1 ? 0.0f : -1.0f; props.yMultiplier[3] = clipPlane.z == 1 ? 1.0f : 0.0f;
			
			if(facingTheWrongWay){
				if(clipPlane.z == 1){
					props.V[1] = 0.0f; 				props.V[2] = 1.0f;
					props.yMultiplier[1] = 0.0f;	props.yMultiplier[2] = 1.0f; 
				}
				else{
					props.V[1] = -1.0f; 				props.V[2] = 0.0f;
					props.yMultiplier[1] = -1.0f;	props.yMultiplier[2] = 0.0f; 
				}
			
				props.U[1] = 1.0f;				props.U[2] = -1.0f; 
				props.xMultiplier[1] = 1.0f;	props.xMultiplier[2] = -1.0f;  
			}
		}
		else{
			props.U[0] = (clipPlane.x == 1 ? 0.0f : -1.0f); 
			props.U[1] = (clipPlane.x == 1 ? 0.0f : -1.0f); 
			props.U[2] = (clipPlane.x == 1 ? 1.0f : 0.0f);  
			props.U[3] = (clipPlane.x == 1 ? 1.0f : 0.0f);
			
			props.V[0] = (clipPlane.y == 1 ? 0.0f : -1.0f); 
			props.V[1] = (clipPlane.y == 1 ? 1.0f : 0.0f); 
			props.V[2] = (clipPlane.y == 1 ? 0.0f : -1.0f);  
			props.V[3] = (clipPlane.y == 1 ? 1.0f : 0.0f);			
			
			props.xMultiplier[0] = clipPlane.x == 1 ? 0.0f : -1.0f; 
			props.xMultiplier[1] = clipPlane.x == 1 ? 0.0f : -1.0f;
			props.xMultiplier[2] = clipPlane.x == 1 ? 1.0f : 0.0f; 
			props.xMultiplier[3] = clipPlane.x == 1 ? 1.0f : 0.0f;
			
			props.yMultiplier[0] = clipPlane.y == -1 ? -1.0f : 0.0f; 
			props.yMultiplier[1] = clipPlane.y == -1 ? 0.0f : 1.0f;
			props.yMultiplier[2] = clipPlane.y == -1 ? -1.0f : 0.0f; 
			props.yMultiplier[3] = clipPlane.y == -1 ? 0.0f : 1.0f;
			
			if(facingTheWrongWay){
				if(clipPlane.y == 1){
					props.V[1] = 0.0f; 				props.V[2] = 1.0f;
					props.yMultiplier[1] = 0.0f;	props.yMultiplier[2] = 1.0f; 
				}
				else{
					props.V[1] = -1.0f; 				props.V[2] = 0.0f;
					props.yMultiplier[1] = -1.0f;	props.yMultiplier[2] = 0.0f; 
				}
				
				if(clipPlane.x == 1){
					props.U[1] = 1.0f; 				props.U[2] = 0.0f;
					props.xMultiplier[1] = 1.0f;	props.xMultiplier[2] = 0.0f; 
				}
				else{
					props.U[1] = 0.0f; 				props.U[2] = -1.0f;
					props.xMultiplier[1] = 0.0f;	props.xMultiplier[2] = -1.0f; 
				}			
			}
		}		
	}
	else{
		if(clipPlane.x == -1 && clipPlane.y == -1){
			props.xMultiplier[0] = 1.0f; props.xMultiplier[1] = 0.0f; props.xMultiplier[2] = 1.0f; props.xMultiplier[3] = 0.0f; 
			props.xMultiplier[4] = 0.0f; props.xMultiplier[5] = -1.0f; props.xMultiplier[6] = -1.0f;
			
			props.yMultiplier[0] = -1.0f; props.yMultiplier[1] = -1.0f; props.yMultiplier[2] = 1.0f; props.yMultiplier[3] = 1.0f; 
			props.yMultiplier[4] = 0.0f; props.yMultiplier[5] = 1.0f; props.yMultiplier[6] = 0.0f;
			
			props.U[0] = 1.0f; props.U[1] = 0.0f; props.U[2] = 1.0f; props.U[3] = 0.0f; props.U[4] = 0.0f; props.U[5] = -1.0f; props.U[6] = -1.0f;
			props.V[0] = -1.0f; props.V[1] = -1.0f; props.V[2] = 1.0f; props.V[3] = 1.0f; props.V[4] = 0.0f; props.V[5] = 1.0f; props.V[6] = 0.0f;
		}	
		else if(clipPlane.x == 1 && clipPlane.y == -1){
			props.xMultiplier[0] = 1.0f; props.xMultiplier[1] = 1.0f; props.xMultiplier[2] = -1.0f; props.xMultiplier[3] = -1.0f; 
			props.xMultiplier[4] = 0.0f; props.xMultiplier[5] = -1.0f; props.xMultiplier[6] = 0.0f;
			
			props.yMultiplier[0] = -1.0f; props.yMultiplier[1] = 0.0f; props.yMultiplier[2] = -1.0f; props.yMultiplier[3] = 0.0f;
			props.yMultiplier[4] = 0.0f; props.yMultiplier[5] = 1.0f; props.yMultiplier[6] = 1.0f;
			
			props.U[0] = 1.0f; props.U[1] = 1.0f; props.U[2] = -1.0f; props.U[3] = -1.0f; props.U[4] = 0.0f; props.U[5] = -1.0f; props.U[6] = 0.0f;
			props.V[0] = -1.0f; props.V[1] = 0.0f; props.V[2] = -1.0f; props.V[3] = 0.0f; props.V[4] = 0.0f; props.V[5] = 1.0; props.V[6] = 1.0f;
		}		
		else if(clipPlane.x == 1 && clipPlane.y == 1){
			props.xMultiplier[0] = -1.0f; props.xMultiplier[1] = 0.0f; props.xMultiplier[2] = -1.0f; props.xMultiplier[3] = 0.0f; 
			props.xMultiplier[4] = 0.0f; props.xMultiplier[5] = 1.0f; props.xMultiplier[6] = 1.0f;
			
			props.yMultiplier[0] = -1.0f; props.yMultiplier[1] = -1.0f; props.yMultiplier[2] = 1.0f; props.yMultiplier[3] = 1.0f;
			props.yMultiplier[4] = 0.0f; props.yMultiplier[5] = 1.0f; props.yMultiplier[6] = 0.0f;
			
			props.U[0] = -1.0f; props.U[1] = 0.0f; props.U[2] = -1.0f; props.U[3] = 0.0f; props.U[4] = 0.0f; props.U[5] = 1.0f; props.U[6] = 1.0f;
			props.V[0] = -1.0f; props.V[1] = -1.0f; props.V[2] = 1.0f; props.V[3] = 1.0f; props.V[4] = 0.0f; props.V[5] = 1.0f; props.V[6] = 0.0f;
		}		
		else if(clipPlane.x == -1 && clipPlane.y == 1){
			props.xMultiplier[0] = -1.0f; props.xMultiplier[1] = -1.0f; props.xMultiplier[2] = 1.0f; props.xMultiplier[3] = 1.0f; 
			props.xMultiplier[4] = 0.0f; props.xMultiplier[5] = 1.0f; props.xMultiplier[6] = 0.0f; 
			
			props.yMultiplier[0] = -1.0f; props.yMultiplier[1] = 0.0f; props.yMultiplier[2] = -1.0f; props.yMultiplier[3] = 0.0f;
			props.yMultiplier[4] = 0.0f; props.yMultiplier[5] = 1.0f; props.yMultiplier[6] = 1.0f;
			
			props.U[0] = -1.0f; props.U[1] = -1.0f; props.U[2] = 1.0f; props.U[3] = 1.0f; props.U[4] = 0.0f; props.U[5] = 1.0f; props.U[6] = 0.0f;
			props.V[0] = -1.0f; props.V[1] = 0.0; props.V[2] = -1.0f; props.V[3] = 0.0f; props.V[4] = 0.0f; props.V[5] = 1.0f; props.V[6] = 1.0f;
		}		
	}
	
	return props;
}

//
// Takes in a surfel edge and outputs maximum 2 quads
//
[maxvertexcount(8)]
void SurfelEdgeGS(point Surfel_Edge_GS_Input input[1], inout TriangleStream<Surfel_Edge_PS_Input> surfelStream, uniform bool useWVP)
{
    Surfel_Edge_PS_Input output;
        
	float4 originalPos;

	float3 calculatedNormal = cross(input[0].MinorRadius.xyz, input[0].MajorRadius.xyz);
	bool facingWrongWay = sign(calculatedNormal.x) != sign(input[0].Normal.x) || sign(calculatedNormal.y) != sign(input[0].Normal.y) || sign(calculatedNormal.z) != sign(input[0].Normal.z);
		
	ClippedSurfelProperties props = GetProperties(input[0].ClipPlane, facingWrongWay);
	
	bool hasRestarted = false;
		
	[unroll]
	for(int i=0; i<props.count; i++)
	{		
		if(i == 4 && !hasRestarted){
			surfelStream.RestartStrip();	
			hasRestarted = true;
			i--;
		}
											
		if(useWVP){
			originalPos = float4(input[0].Pos.xyz + props.xMultiplier[i] * input[0].MajorRadius.xyz * RadiusScale + props.yMultiplier[i] * input[0].MinorRadius.xyz * RadiusScale, 1.0f);
			output.Pos = originalPos;
			output.Pos = mul( mul( mul( output.Pos, World), View), Projection);
		}
		else{
			originalPos = float4(input[0].Pos.xyz + props.xMultiplier[i] * input[0].MajorRadius.xyz * RadiusScale + props.yMultiplier[i] * input[0].MinorRadius.xyz * RadiusScale, 1.0f);
			output.Pos = originalPos;			
		}
		
		output.NormalW = normalize(mul(float4(input[0].Normal.xyz, 0), World));
		
		// Determine the light direction vector. This assumes that the vector
		// is constant relative to the camera.
		output.LightDir = -normalize(float4(LightDirection, 1.0f)).xyz;
		
		// Determine the eye vector for the light
		output.EyeVect = -normalize(float4(CameraPos, 1.0f) + originalPos).xyz;

		output.UV = float2(input[0].UV.x + props.U[i] * DeltaUV.x * RadiusScale, input[0].UV.y + props.V[i] * DeltaUV.y * RadiusScale);
		output.EWAUV = float2(props.U[i]*0.5f + 0.5f, props.V[i]*0.5f + 0.5f);
				
		surfelStream.Append(output);	
	}

	surfelStream.RestartStrip();		
}

/*
		Clip planes can be
		(1, -1, 0)		(-1, -1, 0)		(1, 1, 0)		(-1, 1, 0)	   (0, 1, 0)		(1, 0, 0)
		 _ _			 _ _				 _			    _			   _
		|  _|			|_	|				| |_		  _| |			  | |			  _ _
		|_| 			  |_|				|_ _|	   	 |_ _|			  |_|			 |_ _|

		(1, -1, 1)		(-1, -1, 1)		(1, 1, 1)		(-1, 1, 1)	   (0, 1, 1)		(1, 0, 1)
		    			 				       _		  _				 _				  _ _
		   _			 _					  |_|		 |_| 			| |				 |_ _|
		  |_| 			|_|   			     	   	     				|_|				 
*/

float4 SurfelPS(Surfel_PS_Input psIn): SV_Target
{
	float4 color = EWATexture.Sample(Filter, psIn.EWAUV ) * SurfaceTexture.Sample(Filter, psIn.UV) * Light_OrenNayer(psIn.NormalW.xyz, psIn.EyeVect, psIn.LightDir);
	
	if(color.a <= 0.5f)
		clip(-1);
	
	return color;
}

float4 SurfelEdgePS(Surfel_Edge_PS_Input psIn): SV_Target
{
	float4 color = EWATexture.Sample(Filter, psIn.EWAUV ) * SurfaceTexture.Sample(Filter, psIn.UV) * Light_OrenNayer(psIn.NormalW.xyz, psIn.EyeVect, psIn.LightDir);
	
	if(color.a <= 0.5f)
		clip(-1);
		
	return color;
}

Quad_PS_Input QuadVS(Quad_VS_Input vsIn){
	Quad_PS_Input psOut = (Quad_PS_Input)0;
	
	psOut.Pos = mul(mul(mul(vsIn.Pos, World), View), Projection);
	psOut.NormalW = /*mul(float4(*/vsIn.Normal;/*.xyz, 0.0f), World);*/
	psOut.UV= vsIn.UV;
	psOut.EWAUV= vsIn.EWAUV;
	
	psOut.LightDir = -normalize(float4(LightDirection, 1.0f)).xyz;
	psOut.EyeVect = -normalize(float4(CameraPos, 1.0f) + psOut.Pos).xyz;
		
	return psOut;
}

float4 QuadPS(Quad_PS_Input psIn, uniform bool useTexture) : SV_Target{
	float4 surfelColor;
	
	if(useTexture){
		surfelColor = EWATexture.Sample(Filter, psIn.EWAUV) * SurfaceTexture.Sample(Filter, psIn.UV) * Light_OrenNayer(psIn.NormalW.xyz, psIn.EyeVect, psIn.LightDir);
	}
	else{
		surfelColor = SurfaceTexture.Sample(Filter, psIn.UV) * Light_OrenNayer(psIn.NormalW.xyz, psIn.EyeVect, psIn.LightDir);
	}
	
	if(surfelColor.a <= 0.5f)
		clip(-1);
	
	return surfelColor;
}

GeometryShader SurfelGeometryOut = ConstructGSWithSO( CompileShader( gs_4_0, SurfelGS(false) ), "SV_POSITION.xyz; NORMAL.xyz; TEXCOORD.xy;  TEXCOORD1.xy" );
GeometryShader SurfelEdgeGeometryOut = ConstructGSWithSO( CompileShader( gs_4_0, SurfelEdgeGS(false) ), "SV_POSITION.xyz; NORMAL.xyz; TEXCOORD.xy;  TEXCOORD1.xy" );

technique10 GeometryTechnique
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, SurfelVS() ) );
		SetGeometryShader ( SurfelGeometryOut );
	    SetPixelShader( NULL);

		SetDepthStencilState( DisableDepth, 0 );
		SetRasterizerState(SOLID);
	}
}

technique10 GeometryEdgeTechnique
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, SurfelEdgeVS() ) );
		SetGeometryShader ( SurfelEdgeGeometryOut );
	    SetPixelShader( NULL);

		SetDepthStencilState( DisableDepth, 0 );
		SetRasterizerState(SOLID);
	}
}

technique10 SurfelTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, SurfelVS() ) );
		SetGeometryShader ( CompileShader( gs_4_0, SurfelGS(true) ) );
	    SetPixelShader( CompileShader( ps_4_0, SurfelPS() ) );
	    SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(SURFEL);
 	    SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 SurfelEdgeTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, SurfelEdgeVS() ) );
		SetGeometryShader ( CompileShader( gs_4_0, SurfelEdgeGS(true) ) );
	    SetPixelShader( CompileShader( ps_4_0, SurfelEdgePS() ) );
	    SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(SURFEL);
 	    SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 SolidTechnique
{
    pass P0
    {
		SetVertexShader( CompileShader( vs_4_0, QuadVS() ) );
		SetGeometryShader ( NULL );
	    SetPixelShader( CompileShader( ps_4_0, QuadPS(true) ) );
	    SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(SURFEL);
 	    SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}

technique10 WireframeTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, QuadVS() ) );
		SetGeometryShader ( NULL );
	    SetPixelShader( CompileShader( ps_4_0, QuadPS(false) ) );
	    SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState(WF);
		SetBlendState( NoAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
}
