#ifndef MATH_HELPER_H
#define MATH_HELPER_H

#include <D3DX10.h>
#include "Structs.h"

class MathHelper
{
public:
	static D3DXVECTOR3 Perpendicular(D3DXVECTOR3 v);
	static D3DXVECTOR3 Dimensions(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, D3DXVECTOR3 p4);
	static int Round(float x);

	static D3DXVECTOR3 GetElongatedMajorClipPlane(D3DXVECTOR3 originalClipPlane);
	static D3DXVECTOR3 GetElongatedMinorClipPlane(D3DXVECTOR3 originalClipPlane);

	static int Sign(float x);
	
	static bool D3DXVECTOR3Equals(D3DXVECTOR3 vec, float x, float y, float z);
	static bool Facing(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 n2);
	static bool Facing(D3DXVECTOR3 n1, D3DXVECTOR3 n2);
	static bool SameDirection(D3DXVECTOR3 n1, D3DXVECTOR3 n2);
	static float GetAngleBetweenVectors(D3DXVECTOR3 v1, D3DXVECTOR3 v2, bool betweenPiAndMinusPi);
	static float Get3DAngle(D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 n);
	static float Get3DAngle(ProjectStructs::SURFEL* surfel, D3DXVECTOR3 point, D3DXVECTOR3 zeroAnglePoint);
	static bool isCounterClockwise(D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2);
	static float GetOverlapPercentage(ProjectStructs::SURFEL* surfel1, ProjectStructs::SURFEL* surfel2);
	static bool Intersection(ProjectStructs::SURFEL* surfel1, ProjectStructs::SURFEL* surfel2);
	static bool Intersection(ProjectStructs::SURFEL* surfel1, ProjectStructs::SURFEL* surfel2, float percentage);

	// method taken from http://www.johndcook.com/IEEE_exceptions_in_cpp.html
	static bool IsFiniteNumber(float x){
		return (x <= FLT_MAX && x >= -FLT_MAX); 
	} 
	
	static D3DXMATRIX GetIdentity(){
		if(I._11 == 0.0f)
			D3DXMatrixIdentity(&I);

		return I;
	}

	static D3DXMATRIX Get3x3Identity(){
		if(I3BY3._11 == 0.0f){
			D3DXMatrixIdentity(&I3BY3);
			I3BY3._44 = 0.0f;
		}
		return I3BY3;
	}

	static D3DXVECTOR3 GetZeroVector(){
		return zero;
	}
	static D3DXVECTOR3 GetClipPlane( D3DXVECTOR3 clipPlane1, D3DXVECTOR3 clipPlane2 );
	static bool IsCorner(D3DXVECTOR3 clipPlane);
private:
	static D3DXVECTOR3 temp;
	static D3DXVECTOR3 temp1;
	static D3DXVECTOR3 temp2;
	static D3DXVECTOR3 temp3;
	static D3DXMATRIX I, I3BY3;
	static D3DXVECTOR3 zero;

};

#endif
