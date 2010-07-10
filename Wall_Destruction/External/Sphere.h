/*
	Sphere class taken from Luna's Framework
*/
#ifndef SPHERE_H
#define SPHERE_H

//#include "PhysicsWrapper.h"
#include "d3dUtil.h"
#include "Structs.h"
#include "CustomEffect.h"
#include <vector>
#include <D3DX10.h>

class Sphere
{
public:
	Sphere();
	~Sphere();

	void init(float radius, UINT numSlices, UINT numStacks);
	void Draw(Helpers::CustomEffect *effect);
	void DrawDepth(Helpers::CustomEffect *effect);
	void CleanUp();

	void UpdateWorld();

	void SetPosition(float x, float y, float z){
		D3DXMatrixTranslation(&Translation, x, y, z);
		pos.x = x;
		pos.y = y;
		pos.z = z;
	}

	void SetRotation(float yaw, float pitch, float roll){D3DXMatrixRotationYawPitchRoll(&Rotation, yaw, pitch, roll);}

	D3DXMATRIX GetPosition(){return Translation;}
	D3DXVECTOR3 GetPositionVector(){return pos;}
	D3DXMATRIX GetRotation(){return Rotation;}
	D3DXMATRIX GetWorld(){return World;}
	void SetWorld(D3DXMATRIX newWorld){ World = newWorld;}

	float GetRadius(){return mRadius;}
	void SetRigidBody(hkpRigidBody* rb){rigidBody = rb;}
	hkpRigidBody* GetRigidBody(){return rigidBody;}
	D3DXMATRIX GetRadiusTranslation(){return RadiusTranslation;}

private:
	typedef std::vector<ProjectStructs::SOLID_VERTEX> VertexList;
	typedef std::vector<DWORD> IndexList;

	void buildStacks(VertexList& vertices, IndexList& indices);

private:
	float mRadius;
	UINT  mNumSlices;
	UINT  mNumStacks;

	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Buffer *mVB, *mVBDepth;
	ID3D10Buffer* mIB;

	D3DXMATRIX Translation, Rotation, RadiusTranslation, World;
	D3DXVECTOR3 pos;
	hkpRigidBody *rigidBody;
};

#endif // SPHERE_H
