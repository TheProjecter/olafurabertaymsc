#ifndef PHONG_LIGHTING
#define PHONG_LIGHTING

#include "GlobalParameters.inc"

//float3 CameraPos;
//float3 LightDirection;

float3 LightAmbientColor;
float3 LightDiffuseColor; // intensity multiplier
float3 LightSpecularColor; // intensity multiplier
float3 DiffuseColor;
float3 SpecularColor;
float SpecularPower;

// this method is taken from Introduction to 3D Game Programming with DirectX 10.
float3 ParallelLight(float3 posW, float3 normalW)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
 
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -LightDirection;
	
	// Add the ambient term.
	litColor += DiffuseColor * LightAmbientColor;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, normalW);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(SpecularPower, 1.0f);
		float3 toEye     = normalize(CameraPos - posW);
		float3 R         = reflect(-lightVec, normalW);
		float specFactor = pow(max(dot(R, CameraPos), 0.0f), specPower);
					
		// diffuse and specular terms
		litColor += diffuseFactor * DiffuseColor * LightDiffuseColor;
		litColor += specFactor * SpecularColor * LightSpecularColor;
	}
	
	return litColor;
}

#endif