#include "GlobalParameters.inc"
float4x4 invViewMatrix;

// shader method taken from Shaders for Game programmers and artists
float4 Light_OrenNayer(float3 Normal, float3 EyeVect, float3 LightDir)
{
	float4 color;	
	float3 HalfVect = normalize(LightDir+EyeVect);

	// calculate all the dot products
	float EdotH = dot(EyeVect, HalfVect);
	float LdotH = dot(LightDir, HalfVect);
	float NdotH = dot(Normal, HalfVect);
	float NdotL = dot(Normal, LightDir);
	float NdotE = dot(Normal, EyeVect);

	// calculate the zenith angles
	float sinTheta_r = length(cross(EyeVect,Normal));
	float cosTheta_r = max(NdotE,0.001);
	float sinTheta_i = length(cross(LightDir,Normal));
	float cosTheta_i = max(NdotL,0.001);
	float tanTheta_i = sinTheta_i / cosTheta_i;
	float tanTheta_r = sinTheta_r / cosTheta_r;

	// calculate the azimuth angles
	float3 E_p = normalize(EyeVect-NdotE*Normal);
	float3 L_p = normalize(LightDir-NdotL*Normal);
	float cosAzimuth = dot(E_p, L_p);

	// Compute final lighting
	float inten = rhoOverPi * cosTheta_i * (A + B * max(0, cosAzimuth) * max(sinTheta_r, sinTheta_i) * min(tanTheta_i, tanTheta_r)) * 4.0f;

	return float4(LightColor.xyz * clamp(inten,0.0,1) + AmbientColor, 1.0f);
}
