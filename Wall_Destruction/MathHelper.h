#ifndef MATH_HELPER_H
#define MATH_HELPER_H

#include <D3DX10.h>

class MathHelper
{
public:
	static D3DXVECTOR3 Perpendicular(D3DXVECTOR3 v);
	static D3DXVECTOR3 Dimensions(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, D3DXVECTOR3 p4);
private:
	static D3DXVECTOR3 temp;
	static D3DXVECTOR3 temp1;
	static D3DXVECTOR3 temp2;
	static D3DXVECTOR3 temp3;

};

#endif
