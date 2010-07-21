#include "MathHelper.h"

D3DXVECTOR3 MathHelper::temp;
D3DXVECTOR3 MathHelper::temp1;
D3DXVECTOR3 MathHelper::temp2;
D3DXVECTOR3 MathHelper::temp3;
D3DXMATRIX MathHelper::I;

// method taken from  Surface Splatting: Theory, Extensions and Implementation by Jussi Räsänen
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

int MathHelper::Round(float x){
	return (int)(x + 0.5f);
}

bool MathHelper::Facing(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 n2){
	return D3DXVec3Dot(&(p1 - p2), &n2) <= 0.0f;
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
bool MathHelper::D3DXVECTOR3Equals(D3DXVECTOR3 vec, float x, float y, float z){
	return vec.x == x && vec.y == y && vec.z == z;
}

bool MathHelper::PositiveMajor(ProjectStructs::SURFEL_EDGE *edge){
	return !D3DXVECTOR3Equals(edge->vertex.clipPlane, 0, 1, 1) && !D3DXVECTOR3Equals(edge->vertex.clipPlane, -1, 1, 1) && !D3DXVECTOR3Equals(edge->vertex.clipPlane, -1, -1, 1);
}

bool MathHelper::PositiveMinor(ProjectStructs::SURFEL_EDGE *edge){
	return !D3DXVECTOR3Equals(edge->vertex.clipPlane, 1, 0, 0) && !D3DXVECTOR3Equals(edge->vertex.clipPlane, -1, -1, 1) && !D3DXVECTOR3Equals(edge->vertex.clipPlane, 1, -1, 1);
}

bool MathHelper::NegativeMajor(ProjectStructs::SURFEL_EDGE *edge){
	return !D3DXVECTOR3Equals(edge->vertex.clipPlane, 0, 1, 0) && !D3DXVECTOR3Equals(edge->vertex.clipPlane, 1, 1, 1) && !D3DXVECTOR3Equals(edge->vertex.clipPlane, 1, -1, 1);
}

bool MathHelper::NegativeMinor(ProjectStructs::SURFEL_EDGE *edge){
	return !D3DXVECTOR3Equals(edge->vertex.clipPlane, 1, 0, 1) && !D3DXVECTOR3Equals(edge->vertex.clipPlane, -1, 1, 1) && !D3DXVECTOR3Equals(edge->vertex.clipPlane, 1, 1, 1);
}

int MathHelper::Sign(float x){
	return x < 0.0f ? -1 : x > 0.0f ? 1 : 0 ;
}

// method functionality taken from http://en.wikipedia.org/wiki/Eigenvalue_algorithm
void MathHelper::CalculateMaximumEigenvalues(D3DXMATRIX matrix, float &maximumEigenValue, D3DXVECTOR3 &maximumEigenVector){

	// transform matrix to 3x3
	matrix._14 = 0.0f;
	matrix._24 = 0.0f;
	matrix._34 = 0.0f;
	matrix._44 = 0.0f;
	matrix._43 = 0.0f;
	matrix._42 = 0.0f;
	matrix._41 = 0.0f;

	float a, b, c, d;
	a = -1.0f;
	b = matrix._11 + matrix._22 + matrix._33;
	c = matrix._21 * matrix._12 + 
		matrix._31 * matrix._13 + 
		matrix._23 * matrix._32 - 
		matrix._11 * matrix._22 - 
		matrix._11 * matrix._33 + 
		matrix._22 * matrix._33;
	d = matrix._11 * matrix._22 * matrix._33 - 
		matrix._11 * matrix._23 * matrix._32 - 
		matrix._21 * matrix._12 * matrix._33 +
		matrix._21 * matrix._13 * matrix._32 + 
		matrix._31 * matrix._12 * matrix._23 - 
		matrix._31 * matrix._13 * matrix._22;


	float x, y, z;
	x = ((3.0f*c/a) - b*b/ (a*a))/3.0f;
	y = ((2.0f*b*b*b/(a*a*a)) - (9.0f*b*c/(a*a)) + (27.0f*d/a))/27.0f;
	z = y*y/4.0f + x*x*x/27.0f;

	float i, j, k, m, n, p;
	i = sqrt(y*y / 4.0f - z);
	j = -pow(i, 1.0f/3.0f);
	k = acos(-(y/(2.0f * i)));

	m = cos(k / 3.0f);
	n = sqrt(3.0f)*sin(k / 3.0f);
	p = -(b / (3.0f*a));

	float eig1, eig2, eig3;

	eig1 = -2.0f*j*m + p;
	eig2 = j * (m+n) + p;
	eig3 = j *(m - n) + p;

	maximumEigenValue = max(eig1, max(eig2, eig3));
	matrix = matrix - maximumEigenValue * GetIdentity();

	// calculate the x, y and z of the maximum eigen vector
	

}