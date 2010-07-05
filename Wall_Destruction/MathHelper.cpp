#include "MathHelper.h"

D3DXVECTOR3 MathHelper::temp;
D3DXVECTOR3 MathHelper::temp1;
D3DXVECTOR3 MathHelper::temp2;
D3DXVECTOR3 MathHelper::temp3;

// method taken from  Surface Splatting: Theory, Extensions and Implementation by Jussi R�s�nen
D3DXVECTOR3 MathHelper::Perpendicular( D3DXVECTOR3 v )
{
	if(abs(v.x) < abs(v.y)){
		if(abs(v.x) < abs(v.z)){
			temp.x = 1.0f - v.x * v.x;
			temp.y = -v.x * v.y;
			temp.z = -v.x * v.z;
			return temp;
		}
	}
	else{
		if(abs(v.y) < abs(v.z)){
			temp.x = -v.y * v.x;
			temp.y = 1.0f - v.y * v.y;
			temp.z = -v.y * v.z;
			return temp;
		}
	}

	temp.x = -v.z * v.x;
	temp.y = -v.z * v.y;
	temp.z = 1.0f - v.z * v.z;
	return temp;
}

D3DXVECTOR3 MathHelper::Dimensions(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, D3DXVECTOR3 p4 )
{
	temp = p1 - p2;
	temp1 = p2 - p3;
	temp2 = p3 - p4;
	temp3 = p4 - p1;
	
	temp.x = abs(temp.x);
	temp.y = abs(temp.y);
	temp.z = abs(temp.z);
	
	temp1.x = abs(temp1.x);
	temp1.y = abs(temp1.y);
	temp1.z = abs(temp1.z);

	temp2.x = abs(temp2.x);
	temp2.y = abs(temp2.y);
	temp2.z = abs(temp2.z);

	temp3.x = abs(temp3.x);
	temp3.y = abs(temp3.y);
	temp3.z = abs(temp3.z);

	return D3DXVECTOR3(max(temp.x, max(temp1.x, max(temp2.x, temp3.x))), 
		max(temp.y, max(temp1.y, max(temp2.y, temp3.y))),
		max(temp.z, max(temp1.z, max(temp2.z, temp3.z))));

}