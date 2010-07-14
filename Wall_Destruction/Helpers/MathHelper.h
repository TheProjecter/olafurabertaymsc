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

	static bool NegativeMajor(ProjectStructs::SURFEL_EDGE *edge);
	static bool NegativeMinor(ProjectStructs::SURFEL_EDGE *edge);
	static bool PositiveMajor(ProjectStructs::SURFEL_EDGE *edge);
	static bool PositiveMinor(ProjectStructs::SURFEL_EDGE *edge);

	static int Sign(float x);
	
	static bool D3DXVECTOR3Equals(D3DXVECTOR3 vec, float x, float y, float z);
	static bool Facing(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 n2);

private:
	static D3DXVECTOR3 temp;
	static D3DXVECTOR3 temp1;
	static D3DXVECTOR3 temp2;
	static D3DXVECTOR3 temp3;

};

#endif
