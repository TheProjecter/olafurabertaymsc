#include "MathHelper.h"
#include "Globals.h"
#include <algorithm>

D3DXVECTOR3 MathHelper::temp;
D3DXVECTOR3 MathHelper::temp1;
D3DXVECTOR3 MathHelper::temp2;
D3DXVECTOR3 MathHelper::temp3;
D3DXMATRIX MathHelper::I, MathHelper::I3BY3;
D3DXVECTOR3 MathHelper::zero = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

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

// returns the dimensions of the four points
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

bool MathHelper::SameDirection(D3DXVECTOR3 n1, D3DXVECTOR3 n2){
	return abs(GetAngleBetweenVectors(n1, n2, true)) <= Helpers::Globals::PI *0.25f;
}

bool MathHelper::Facing(D3DXVECTOR3 n1, D3DXVECTOR3 n2){
	return D3DXVec3Dot(&n1, &n2) <= 0.0f;
}

bool MathHelper::Facing(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 n2){
	return Facing((p1 - p2), n2);
}

bool MathHelper::D3DXVECTOR3Equals(D3DXVECTOR3 vec, float x, float y, float z){
	return vec.x == x && vec.y == y && vec.z == z;
}

bool MathHelper::Intersection(ProjectStructs::SURFEL* surfel1, ProjectStructs::SURFEL* surfel2){
	return Intersection(surfel1, surfel2, 0.1f);
}

bool MathHelper::Intersection(ProjectStructs::SURFEL* surfel1, ProjectStructs::SURFEL* surfel2, float percentage){

	if(!SameDirection(surfel1->vertex->normal, surfel2->vertex->normal)){
		return false;
	}

	float r1, r2;
	r1 = max(D3DXVec3Length(&surfel1->vertex->minorAxis),D3DXVec3Length(&surfel1->vertex->majorAxis)) * percentage;
	r2 = max(D3DXVec3Length(&surfel2->vertex->minorAxis),D3DXVec3Length(&surfel2->vertex->majorAxis)) * percentage;

	return D3DXVec3Length(&(surfel1->vertex->pos - surfel2->vertex->pos)) < r1 + r2;
}

// method taken from http://stackoverflow.com/questions/1440977/how-can-you-calculate-the-percentage-overlap-of-two-rectangles
float MathHelper::GetOverlapPercentage(ProjectStructs::SURFEL* surfel1, ProjectStructs::SURFEL* surfel2){

	RECT rect1, rect2, intersection;
	float maxX1, maxY1, maxX2, maxY2;

	maxX1 = max(abs(surfel1->vertex->majorAxis.x), abs(surfel1->vertex->minorAxis.x));
	maxY1 = max(abs(surfel1->vertex->majorAxis.y), abs(surfel1->vertex->minorAxis.y));

	maxX2 = max(abs(surfel2->vertex->majorAxis.x), abs(surfel2->vertex->minorAxis.x));
	maxY2 = max(abs(surfel2->vertex->majorAxis.y), abs(surfel2->vertex->minorAxis.y));

	rect1.bottom = surfel1->vertex->pos.y - maxY1;
	rect1.left = surfel1->vertex->pos.x - maxX1;
	rect1.top = surfel1->vertex->pos.y + maxY1;
	rect1.right = surfel1->vertex->pos.x + maxX1;

	rect2.bottom = surfel2->vertex->pos.y - maxY2;
	rect2.left = surfel2->vertex->pos.x - maxX2;
	rect2.top = surfel2->vertex->pos.x + maxY2;
	rect2.right = surfel2->vertex->pos.y + maxX2;

	// check intersection
	if(rect1.left > rect2.right || rect2.left > rect1.right || rect2.bottom > rect1.top || rect1.bottom > rect2.top)
		return 0.0f;

	long horiz[] = {rect1.left, rect1.right, rect2.left, rect2.right};
	std::sort(horiz, horiz + 4);
	long vert[] = {rect1.bottom, rect1.top, rect2.bottom, rect2.top};
	std::sort (vert, vert+ 4);

	intersection.left = horiz[1];
	intersection.bottom = vert[1];
	intersection.right = horiz[2];
	intersection.top = vert[2];

	float percentage = (double)((intersection.right - intersection.left)*(intersection.top - intersection.bottom)) * 2.0 / 
		(double)((rect1.right - rect1.left)*(rect1.top - rect1.bottom) +  
		(rect2.right - rect2.left)*(rect2.top - rect2.bottom));


	return percentage; 
}

D3DXVECTOR3 MathHelper::GetElongatedMajorClipPlane(D3DXVECTOR3 originalClipPlane){
	if((originalClipPlane.x == 1.0f && originalClipPlane.y == -1.0f && originalClipPlane.z == 1.0f) ||
		(originalClipPlane.x == 1.0f && originalClipPlane.y == 1.0f && originalClipPlane.z == 1.0f))
		return D3DXVECTOR3(0.0f, 1.0f, 1.0f);

	return D3DXVECTOR3(0.0f, 1.0f, 0.0f);
}

D3DXVECTOR3 MathHelper::GetElongatedMinorClipPlane(D3DXVECTOR3 originalClipPlane){

	if((originalClipPlane.x == 1.0f && originalClipPlane.y == -1.0f && originalClipPlane.z == 1.0f) || 
		(originalClipPlane.x == -1.0f && originalClipPlane.y == -1.0f && originalClipPlane.z == 1.0f))
		return D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	return D3DXVECTOR3(1.0f, 0.0f, 1.0f);
}

// method taken from http://wiki.mcneel.com/developer/sdksamples/anglebetweenvectors
float MathHelper::GetAngleBetweenVectors(D3DXVECTOR3 v1, D3DXVECTOR3 v2, bool betweenPiAndMinusPi){
	// Unitize the input vectors
	D3DXVec3Normalize(&v1, &v1);
	D3DXVec3Normalize(&v2, &v2);

	float dot = D3DXVec3Dot(&v1, &v2);

	// Force the dot product of the two input vectors to 
	// fall within the domain for inverse cosine, which 
	// is -1 <= x <= 1. This will prevent runtime 
	// "domain error" math exceptions.
	dot = ( dot < -1.0 ? -1.0 : ( dot > 1.0 ? 1.0 : dot ) );

	float angle = acos( dot );
	if(betweenPiAndMinusPi && angle > Helpers::Globals::PI)
		angle -= Helpers::Globals::TWO_PI;
	else if(!betweenPiAndMinusPi && angle < 0){
		angle += Helpers::Globals::TWO_PI;
	}

	return angle;
}

float MathHelper::Get3DAngle(D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 n){
	D3DXVECTOR3 cross;
	D3DXVec3Cross(&cross, &v1 , &v2 );
	
	float angle = atan2(D3DXVec3Dot(&n, &cross), D3DXVec3Dot(&v2 , &v1)); 
 	
// 	if(angle >= Helpers::Globals::TWO_PI)
// 		angle -= Helpers::Globals::TWO_PI;
// 	else 
	if(angle < 0.0f)
		angle += Helpers::Globals::TWO_PI;

	return angle;
}

bool MathHelper::isCounterClockwise(D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2)
{
	if((p1.y-p0.y)*(p2.x-p0.x) < (p2.y-p0.y)*(p1.x-p0.x)) return true;
	if((p1.y-p0.y)*(p2.x-p0.x) > (p2.y-p0.y)*(p1.x-p0.x)) return false;
}

int MathHelper::Sign(float x){
	return x < 0.0f ? -1 : x > 0.0f ? 1 : 0 ;
}

bool MathHelper::IsCorner(D3DXVECTOR3 clipPlane){
	return clipPlane.x != 0 && clipPlane.y != 0 && clipPlane.z != 0;
}


/*
		Clip planes can be
		(0, 1, 0)		(1, 0, 0)		(0, 1, 1)		(1, 0, 1)
		   _							 _				  _ _
		  | |			  _ _			| |				 |_ _|
		  |_|			 |_ _|			|_|				 

		(1, -1, 1)		(-1, -1, 1)		(1, 1, 1)		(-1, 1, 1)	   
		    			 				       _		  _				
		   _			 _					  |_|		 |_| 			
		  |_| 			|_|   			     	   	     				

	*/

D3DXVECTOR3 MathHelper::GetClipPlane( D3DXVECTOR3 clipPlane1, D3DXVECTOR3 clipPlane2 )
{
	if(clipPlane1 == zero || clipPlane2 == zero)
		return zero;

	if(clipPlane2 == clipPlane1)
		return clipPlane1;

	bool isCorner1 = IsCorner(clipPlane1);
	bool isCorner2 = IsCorner(clipPlane2);

	if(isCorner1 && !isCorner2)
		return clipPlane2;

	if(isCorner2 && !isCorner1)
		return clipPlane1;

	if(isCorner1 && isCorner2){
		bool isBottomLeftCorner, isBottomRightCorner, isUpperLeftCorner, isUpperRightCorner;

		isBottomLeftCorner = (clipPlane1.x == 1 && clipPlane1.y == 1 ) || (clipPlane2.x == 1 && clipPlane2.y == 1) ;
		isBottomRightCorner = (clipPlane1.x == -1 && clipPlane1.y == 1 ) || (clipPlane2.x == -1 && clipPlane2.y == 1) ;
		isUpperLeftCorner = (clipPlane1.x == 1 && clipPlane1.y == -1 ) || (clipPlane2.x == 1 && clipPlane2.y == -1) ;
		isUpperRightCorner = (clipPlane1.x == -1 && clipPlane1.y == -1 ) || (clipPlane2.x == -1 && clipPlane2.y == -1 ) ;

		if(isBottomLeftCorner && isBottomRightCorner)
			return D3DXVECTOR3(1, 0, 1);
		else if(isBottomLeftCorner && isUpperLeftCorner)
			return D3DXVECTOR3(0, 1, 0);
		else if(isUpperLeftCorner && isUpperRightCorner)
			return D3DXVECTOR3(1, 0, 0);
		else if(isUpperRightCorner && isBottomRightCorner)
			return D3DXVECTOR3(0, 1, 1);
		
	}

	// default to zero vector
	return zero;


}
